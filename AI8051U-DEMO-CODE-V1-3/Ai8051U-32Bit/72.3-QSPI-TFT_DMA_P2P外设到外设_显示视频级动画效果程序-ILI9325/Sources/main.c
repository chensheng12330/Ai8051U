/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

QSPI DMA + LCM DMA驱动液晶屏程序

按住P3.3后上电，芯片进入FLASH下载模式，使用6.94P版的ISP下载软件中的“串行Flash编程器”

工具将多幅图片下载到实验箱的Flash中，然后重新上电（P3.3不要按），即可进入动画显示模式

显示时QSPI 读取Flash图片数据，通过P2P DMA直接发送数据到LCM显示图片。

QSPI读取Flash与LCM显示DMA的数据长度设置51200字节。

8bit I8080模式, P2口接数据线

LCD_RS = P4^5;         //数据/命令切换
LCD_WR = P3^6;         //写控制
LCD_RD = P3^7;         //读控制
LCD_CS = P0^5;//P5^3;  //片选
LCD_RESET = P4^7;      //复位

QSPI Flash 接口：
sbit    QSPI_CS          =   P4^0;

sbit    QSPI_SDI_IO0     =   P4^1;

sbit    QSPI_SDO_IO1     =   P4^2;

sbit    QSPI_SCK         =   P4^3;

sbit    QSPI_WP_IO2      =   P5^2;

sbit    QSPI_HLD_IO3     =   P5^3;

下载时, 选择时钟 40MHz (频率定义参数在system.h修改).

Ver6.94P新版软件说明：

  1. 增加串行Flash编程工具
     选择 USB-HID/CDC串口助手，点击 发送文件窗口，
     右下角增加Flash编程功能按钮
     支持多文件同时发送, 支持自动生成文件分配表数据
     和电脑串口助手通信的MCU要有对Flash编程的对应程序
     有配套的范例程序可供参考，
     用户可根据对应的Flash修改相应程序
  2. 图片取模工具支持对gif文件格式的支持
     工具菜单 中的 图片取模工具 增加支持转换gif文件
     支持自动转换多帧图片的所有数据，
     方便做视频级的动画显示

******************************************/

#include "config.h"
#include "qspi.h"
#include "w25qxx.h"
#include "lcm.h"
#include "tft.h"
#include "usbcdc.h"
#include "timer.h"

#define IMG_SIZE            (320UL * 240 * 2)
#define DMA_AMT_LEN         (51200UL)
#define DMA_CNT             (IMG_SIZE / DMA_AMT_LEN)

void QSPI2TFT_Start();
void QSPI2TFT_Next();

typedef struct
{
    char strSign[4];
    DWORD dwCount;
    DWORD dwAddress[62];
} FAT;

FAT     Fat;                            //文件分配表
DWORD   dwOffset;                       //图片数据的偏移地址
int     nIndex;                         //图片索引
int     nCount;                         //图片数据装载次数
BOOL    fLoading;                       //装载数据标志

void main()
{
    P_SW2 = 0x80;
    WTST = 0x00;
    CKCON = 0x00;

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x00; P5M1 = 0x00;
    
    TIMER0_Init();
    QSPI_Init();
    LCM_Init();
    TFT_Init();
    CDC_Init();
    
    EA = 1;

    W25Q_Enable_QE();                   //使能QSPI FLASH的4线读写模式
    CDC_WaitStable();                   //等待USB-CDC配置完成
    
    if (!P33)                           //按住P3.3复位,进入图片数据下载模式
    {
        while (1)
        {
            CDC_Process();              //CDC接口处理图片数据
        }
    }
    
    W25Q_FastRead_6B(0, (BYTE *)&Fat, 256);     //从FLASH第一页读取FAT
    
    nIndex = 0;                         //从第一幅图片开始显示
    while (1)
    {
        if (f100ms)
        {
            f100ms = 0;
            
            QSPI2TFT_Start();           //每隔100ms自动显示下一幅图片
        }
    }
}

void QSPI_DMA_Isr() interrupt DMA_QSPI_VECTOR
{
    DMA_QSPI_STA = 0x00;
    QSPI2TFT_Next();                    //DMA传输下一包数据
}

void QSPI2TFT_Start()
{
    if (fLoading)                       //如果正在装载图片,则退出
        return;

    if (nIndex >= Fat.dwCount)          //如果图片索引达到最大值
        nIndex = 0;                     //则从第一幅图片开始循环

    dwOffset = Fat.dwAddress[nIndex++]; //获取当前图片的偏移地址
    nCount = 0;                         //初始化图片数据装载次数
    
    while (QSPI_CheckBusy());           //检测忙状态
    QSPI_SetReadMode();                 //读模式
    QSPI_SetDataLength(DMA_AMT_LEN-1);  //设置数据长度
    QSPI_SetAddressSize(2);             //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(8);             //设置DUMMY时钟
    QSPI_NoInstruction();               //设置无指令模式(防止误触发)
    QSPI_NoAddress();                   //设置无地址模式(防止误触发)
    QSPI_NoAlternate();                 //无间隔字节
    QSPI_DataQuadMode();                //设置数据为四线模式
    QSPI_SetInstruction(0x6B);          //设置指令
    QSPI_InstructionSingMode();         //设置指令为单线模式
    QSPI_NoAddress();                   //设置无地址模式(防止误触发)
    QSPI_SetAddress(dwOffset);          //设置地址
    QSPI_AddressSingMode();             //设置地址为单线模式
                
    TFT_ShowStart();                    //开始TFT彩屏显示

    DMA_P2P_CR1 = 0x87;                 //P2P_SRC_QSPIRX(0x80) | P2P_DEST_LCMTX(0x07);
    DMA_QSPI_CFG = 0xa0;                //使能DMA读取操作
    DMA_QSPI_STA = 0x00;                //清除DMA状态
    DMA_QSPI_AMT = (DMA_AMT_LEN-1);     //设置DMA数据长度
    DMA_QSPI_AMTH = (DMA_AMT_LEN-1) >> 8;
    DMA_LCM_CR = 0xa0;
    DMA_QSPI_CR = 0xa1;                 //启动DMA并触发QSPI读操作
    
    fLoading = TRUE;
}

void QSPI2TFT_Next()
{
    dwOffset += DMA_AMT_LEN;            //更新偏移地址
    nCount++;                           //装载次数+1
    
    if (nCount < DMA_CNT)               //判断图片数据装载次数
    {
        QSPI_NoAddress();               //设置无地址模式(防止误触发)
        QSPI_SetAddress(dwOffset);      //设置地址
        QSPI_AddressSingMode();         //设置地址为单线模式
        
        DMA_QSPI_CR = 0xa1;             //启动DMA并触发QSPI读操作
    }
    else
    {
        fLoading = FALSE;               //图片数据装载完成
        DMA_QSPI_CR = 0x00;             //停止QSPI_DMA
        DMA_QSPI_CFG = 0x00;
        DMA_LCM_CR = 0x00;
        DMA_P2P_CR1 = 0x00;
        
        TFT_ShowEnd();                  //结束TFT彩屏显示数据输入
    }
}

void delay_ms(WORD ms)
{
    WORD i;
    
    do
    {
        i = FOSC / 6000;
        while(--i);
    } while(--ms);
}
