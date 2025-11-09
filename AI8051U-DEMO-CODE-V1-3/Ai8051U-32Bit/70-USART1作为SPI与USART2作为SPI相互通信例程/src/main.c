/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

USART1复用SPI与USART2复用SPI相互通信。

通过P0口LED灯指示通信结果，P0=0x5a表示数据传输正确。

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "../comm/AI8051U.h"

#define FOSC    24000000UL                      //系统工作频率

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

sbit S1SS       =   P4^0;
sbit S1MOSI     =   P4^1;
sbit S1MISO     =   P4^2;
sbit S1SCLK     =   P4^3;

sbit S2SS       =   P4^0;
sbit S2MOSI     =   P4^1;
sbit S2MISO     =   P4^2;
sbit S2SCLK     =   P4^3;

void sys_init();
void usart1_spi_init();
void usart2_spi_init();
void test();

BYTE xdata buffer1[256];                        //定义缓冲区
BYTE xdata buffer2[256];                        //定义缓冲区
BYTE rptr;
BYTE wptr;
bit over;

void main()
{
    int i;
    
    sys_init();                                 //系统初始化
    usart1_spi_init();                          //USART1使能SPI主模式初始化
    usart2_spi_init();                          //USART2使能SPI从模式初始化
    EA = 1;
    
    for (i=0; i<128; i++)
    {
        buffer1[i] = i;                         //初始化缓冲区
        buffer2[i] = 0;
    }
    test();
    
    while (1);
}

void uart1_isr() interrupt UART1_VECTOR
{
    if (TI)
    {
        TI = 0;
        
        if (rptr < 128)
        {
            SBUF = buffer1[rptr++];
        }
        else
        {
            over = 1;
        }
    }
    
    if (RI)
    {
        RI = 0;
    }
}

void uart2_isr() interrupt UART2_VECTOR
{
    if (S2TI)
    {
        S2TI = 0;
    }
    
    if (S2RI)
    {
        S2RI = 0;
        buffer2[wptr++] = S2BUF;
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
    
    P0 = 0xff;
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
 
    ES = 1;
}

void usart2_spi_init()
{
    S2SPI_S1 = 1;   //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    S2SPI_S0 = 0;                       //切换S2SPI到P4.0/S2SS,P4.1/S2MOSI,P4.2/S2MISO,P4.3/S2SCLK
    S2CON = 0x10;                       //使能接收,必须设置为串口模式0
    
    USART2CR1 = 0x10;                   //使能USART2的SPI模式
//  USART2CR1 |= 0x40;                  //DORD=1
    USART2CR1 &= ~0x40;                 //DORD=0
    USART2CR1 |= 0x04;                  //从机模式
//  USART2CR1 &= ~0x04;                 //主机模式
    USART2CR1 |= 0x00;                  //CPOL=0, CPHA=0
//  USART2CR1 |= 0x01;                  //CPOL=0, CPHA=1
//  USART2CR1 |= 0x02 ;                 //CPOL=1, CPHA=0
//  USART2CR1 |= 0x03;                  //CPOL=1, CPHA=1
//  USART2CR4 = 0x00;                   //SPI速度为SYSCLK/4
//  USART2CR4 = 0x01;                   //SPI速度为SYSCLK/8
    USART2CR4 = 0x02;                   //SPI速度为SYSCLK/16
//  USART2CR4 = 0x03;                   //SPI速度为SYSCLK/2
    USART2CR1 |= 0x08;                  //使能SPI功能

    ES2 = 1;
}

void test()
{
    BYTE i;
    BYTE ret;
    
    wptr = 0;
    rptr = 0;
    over = 0;
    
    S1SS = 0;
    SBUF = buffer1[rptr++];             //启动数据传输
    while (!over);                      //等待128个数据传输完成
    S1SS = 1;
    
    ret = 0x5a;
    for (i=0; i<128; i++)
    {
        if (buffer1[i] != buffer2[i])   //校验数据
        {
            ret = 0xfe;
            break;
        }
    }
    P40 = 0;    //打开实验箱P0口LED电源
    P0 = ret;                           //P0=0x5a表示数据传输正确
}

