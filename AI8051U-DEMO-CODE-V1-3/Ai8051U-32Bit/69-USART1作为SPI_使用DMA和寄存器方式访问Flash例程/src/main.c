/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

USART1的SPI模式，使用DMA和寄存器方式访问实验箱上的外挂flash.

通过USB-CDC串口输出读写结果. 

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#define PRINTF_HID            //printf输出直接重定向到USB接口

#include "../comm/AI8051U.h"
#include "../comm/usb.h"
#include "stdio.h"

#define MAIN_Fosc   24000000UL                      //系统工作频率
#define BAUD        (65536 - MAIN_Fosc/4/115200)    //调试串口波特率

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                  //设置自动复位到ISP区的用户接口命令

sbit S1SS       =   P4^0;
sbit S1MOSI     =   P4^1;
sbit S1MISO     =   P4^2;
sbit S1SCLK     =   P4^3;

sbit KEY1       =   P3^2;
//按键所需变量
bit Key_Flag;
bit Key_Function;
WORD Key_cnt;

BYTE xdata buffer1[256];                        //定义缓冲区
BYTE xdata buffer2[256];                        //注意:如果需要使用DMA发送数据,则缓冲区必须定义在xdata区域内

void sys_init();
void usart1_spi_init();
void usart1_tx_dma(WORD size, BYTE xdata *pdat);
void usart1_rx_dma(WORD size, BYTE xdata *pdat);
BOOL flash_is_busy();
void flash_read_id();
void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_write_enable();
void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_erase_sector(DWORD addr);
void delay_ms(BYTE ms);
void KeyScan(void);

void main()
{
    int i;
    
    sys_init();                                 //系统初始化
    usb_init();  //USB初始化
    usart1_spi_init();                          //USART1使能SPI模式初始化
    EA = 1;

    while (1)
    {
        delay_ms(1);
        KeyScan();      //按键扫描

        if(DeviceState != DEVSTATE_CONFIGURED)  //等待USB完成配置
            continue;
        
        if(Key_Function)
        {
            Key_Function = 0;

            printf("\r\nUSART_SPI_DMA test !\r\n");
            flash_read_id();
            flash_read_data(0x0000, 0x80, buffer1);     //测试使用USART1的SPI模式读取外挂FLASH的数据
            flash_erase_sector(0x0000);                 //测试使用USART1的SPI模式擦除外挂FLASH的一个扇区
            flash_read_data(0x0000, 0x80, buffer1);
            for (i=0; i<128; i++)
                buffer2[i] = i;
            flash_write_data(0x0000, 0x80, buffer2);    //测试使用USART1的SPI模式写数据到外挂FLASH
            flash_read_data(0x0000, 0x80, buffer1);
        }

        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,64);  //发送数据缓冲区，长度（接收数据原样返回, 用于测试）
            
            usb_OUT_done(); //接收应答（固定格式）
        }
    }
}

void sys_init()
{
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

    P4SR = 0xf0;                  //P4.0~P4.3快速翻转
}

void usart1_spi_init()
{
    S1SPI_S1 = 1;   //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    S1SPI_S0 = 0;                       //切换S1SPI到P4.0/S1SS,P4.1/S1MOSI,P4.2/S1MISO,P4.3/S1SCLK
    SCON = 0x10;                        //使能接收,必须设置为串口模式0
    
    USARTCR1 = 0x10;                    //使能USART1的SPI模式
//  USARTCR1 |= 0x40;                   //DORD=1
    USARTCR1 &= ~0x40;                  //DORD=0
//  USARTCR1 |= 0x04;                   //从机模式
    USARTCR1 &= ~0x04;                  //主机模式
    USARTCR1 |= 0x00;                   //CPOL=0, CPHA=0
//  USARTCR1 |= 0x01;                   //CPOL=0, CPHA=1
//  USARTCR1 |= 0x02 ;                  //CPOL=1, CPHA=0
//  USARTCR1 |= 0x03;                   //CPOL=1, CPHA=1
//  USARTCR4 = 0x00;                    //SPI速度为SYSCLK/4
//  USARTCR4 = 0x01;                    //SPI速度为SYSCLK/8
    USARTCR4 = 0x02;                    //SPI速度为SYSCLK/16
//  USARTCR4 = 0x03;                    //SPI速度为SYSCLK/2
    USARTCR1 |= 0x08;                   //使能SPI功能
}

BYTE usart1_spi_shift(BYTE dat)
{
    TI = 0;
    SBUF = dat;                         //发送数据
    while (!TI);                        //TI标志是主机模式发送/接收数据完成标志
    
    return SBUF;                        //读取接收的数据
}

BOOL flash_is_busy()
{
    BYTE dat;

    S1SS = 0;
    
    usart1_spi_shift(0x05);             //发送读取状态寄存器命令
    dat = usart1_spi_shift(0);          //读取状态寄存器
    
    S1SS = 1;

    return (dat & 0x01);                //检测FLASH的忙标志
}

void flash_read_id()
{
    BYTE id[3];
    
    S1SS = 0;
    
    usart1_spi_shift(0x9f);             //发送读取FLASH ID命令
    id[0] = usart1_spi_shift(0);       //读取ID1
    id[1] = usart1_spi_shift(0);       //读取ID2
    id[2] = usart1_spi_shift(0);       //读取ID3
    
    S1SS = 1;

    printf("ReadID : ");
    printf("%02bx", id[0]);
    printf("%02bx", id[1]);
    printf("%02bx\r\n", id[2]);
}

void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;
    BYTE *ptr;

    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x03);             //发送读取FLASH数据命令
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));     //设置目标地址
    
//  sz = size;
//  ptr = pdat;
//  while (sz--)
//      *ptr++ = usart1_spi_shift(0);   //寄存器方式读数据
      
    usart1_rx_dma(size, pdat);          //DMA方式读数据

    S1SS = 1;

    printf("ReadData : ");
    sz = size;
    ptr = pdat;
    for (sz=0; sz<size; sz++)
    {
        printf("%02bx ", *ptr++);        //将读到的数据发送到串口,调试使用
        if ((sz % 16) == 15)
        {
            printf("\r\n           ");
        }
    }
    printf("\r\n");
}

void flash_write_enable()
{
    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x06);             //发送写使能命令
    
    S1SS = 1;
}

void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;

    sz = size;
    while (sz)
    {
        flash_write_enable();

        S1SS = 0;
        
        usart1_spi_shift(0x02);         //发送写数据命令
        usart1_spi_shift((BYTE)(addr >> 16));
        usart1_spi_shift((BYTE)(addr >> 8));
        usart1_spi_shift((BYTE)(addr));
        
//      do
//      {
//          usart1_spi_shift(*pdat++);  //寄存器方式写数据
//          addr++;
//
//          if ((BYTE)(addr) == 0x00)
//              break;
//      } while (--sz);

        usart1_tx_dma(sz, pdat);        //DMA方式写数据(注意:数据必须在一个page之内)
        sz = 0;
        
        S1SS = 1;
    }

    printf("Program !\r\n");
}

void flash_erase_sector(DWORD addr)
{
    flash_write_enable();

    S1SS = 0;
    usart1_spi_shift(0x20);             //发送擦除命令
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));
    S1SS = 1;

    printf("Erase Sector !\r\n");
}

void usart1_tx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA传输字节数比实际少1
    
    DMA_UR1T_CFG = 0x00;                //关闭DMA中断
    DMA_UR1T_STA = 0x00;                //清除DMA状态
    DMA_UR1T_AMT = size;                //设置DMA传输字节数
    DMA_UR1T_AMTH = size >> 8;
    DMA_UR1T_TXAL = (BYTE)pdat;         //设置缓冲区地址(注意:缓冲区必须是xdata类型)
    DMA_UR1T_TXAH = (WORD)pdat >> 8;
    DMA_UR1T_CR = 0xc0;                 //使能DMA,触发串口1发送数据
    
    while (!(DMA_UR1T_STA & 0x01));     //等待DMA数据传输完成
    DMA_UR1T_STA = 0x00;                //清除DMA状态
    DMA_UR1T_CR = 0x00;                 //关闭DMA
}

void usart1_rx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA传输字节数比实际少1
    
    DMA_UR1R_CFG = 0x00;                //关闭DMA中断
    DMA_UR1R_STA = 0x00;                //清除DMA状态
    DMA_UR1R_AMT = size;                //设置DMA传输字节数
    DMA_UR1R_AMTH = size >> 8;
    DMA_UR1R_RXAL = (BYTE)pdat;         //设置缓冲区地址(注意:缓冲区必须是xdata类型)
    DMA_UR1R_RXAH = (WORD)pdat >> 8;
    DMA_UR1R_CR = 0xa1;                 //使能DMA,清空接收FIFO,触发串口1接收数据
    
                                        //!!!!!!!!!!!!!
    usart1_tx_dma(size+1, pdat);        //注意:接收数据时必须同时启动发送DMA
                                        //!!!!!!!!!!!!!
    
    while (!(DMA_UR1R_STA & 0x01));     //等待DMA数据传输完成
    DMA_UR1R_STA = 0x00;                //清除DMA状态
    DMA_UR1R_CR = 0x00;                 //关闭DMA
}

//========================================================================
// 函数: void delay_ms(BYTE ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(BYTE ms)
{
     WORD i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);   //6T per loop
     }while(--ms);
}

//========================================================================
// 函数: void KeyScan(void)
// 描述: 按键扫描函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-6-11
// 备注: 
//========================================================================
void KeyScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)		//连续50ms有效按键检测，防抖
            {
                Key_Flag = 1;		//设置按键状态，防止重复触发
                Key_Function = 1;
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}
