/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

设置2个存储器空间，一个发送一个接收，分别初始化2个存储器空间内容。

设置M2M DMA，上电后自动将发送存储里的内容写入到接收存储器空间.

根据不同的读取顺序、写入顺序，接收到不同的数据结果.

通过串口1(P3.0 P3.1)打印接收存储器数据(上电打印一次).

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc     22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

bit DmaFlag;

u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

void DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
    S1_S1 = 0;      //UART1 switch to, 00: P3.0 P3.1, 01: P3.6 P3.7, 10: P1.6 P1.7, 11: P4.3 P4.4
    S1_S0 = 0;
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //定时器时钟1T模式
    S1BRT = 0;      //串口1选择定时器1为波特率发生器
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //定时器1开始计时
}

void UartPutc(unsigned char dat)
{
    SBUF = dat; 
    while(TI == 0);
    TI = 0;
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}

//========================================================================
// 函数: void main(void)
// 描述: 主函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void main(void)
{
    u16 i;
    
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    for(i=0; i<256; i++)
    {
        DmaTxBuffer[i] = i;
        DmaRxBuffer[i] = 0;
    }

    UartInit();
    printf("AI8051U Memory to Memory DMA Test Programme!\r\n");  //UART1发送一个字符串

    DMA_Config();
    EA = 1;     //允许总中断

    DmaFlag = 0;

    while (1)
    {
        if(DmaFlag)
        {
            DmaFlag = 0;

            for(i=0; i<256; i++)
            {
                printf("%02X ", DmaRxBuffer[i]);
                if((i & 0x0f) == 0x0f)
                    printf("\r\n");
            }
        }
    }
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: UART DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_M2M_CFG = 0x80;     //r++ = t++
    DMA_M2M_STA = 0x00;
    DMA_M2M_AMT = 0x7f;     //设置传输总字节数：n+1
    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//    DMA_M2M_CFG = 0xa0;     //r++ = t--
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //设置传输总字节数：n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//    DMA_M2M_CFG = 0x90;     //r-- = t++
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //设置传输总字节数：n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);

//    DMA_M2M_CFG = 0xb0;     //r-- = t--
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //设置传输总字节数：n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);
    
//    DMA_M2M_CFG |= 0x0c;    //设置中断优先级
    DMA_M2M_CR = 0xc0;        //bit7 1:使能 M2M_DMA, bit6 1:开始 M2M_DMA 自动接收
}

//========================================================================
// 函数: void M2M_DMA_Interrupt (void) interrupt 47
// 描述: M2M DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void M2M_DMA_Interrupt(void) interrupt 13
{
    if (DMA_M2M_STA & 0x01)    //发送完成
    {
        DMA_M2M_STA &= ~0x01;
        DmaFlag = 1;
    }
}
