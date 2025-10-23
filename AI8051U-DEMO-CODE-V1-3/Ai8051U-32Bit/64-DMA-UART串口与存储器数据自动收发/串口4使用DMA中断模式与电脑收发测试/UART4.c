/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

串口4全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU将收到的数据自动存入DMA空间.

当串口DMA接收完数据产生超时中断后，通过串口的DMA自动发送功能把存储空间的数据原样返回.

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc   22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate4   115200L

bit DmaTxFlag;
bit DmaRxFlag;
bit B_RX4_TimeOut;

u8 xdata DmaBuffer[256];

void UART4_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void DMA_Config(void);

void UartPutc(unsigned char dat)
{
    S4BUF = dat; 
    while(S4TI == 0);
    S4TI = 0;
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
// 日期: 2024-07-28
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
        DmaBuffer[i] = i;
    }

    UART4_config(0);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
    printf("AI8051U UART4 Test Programme!\r\n");  //UART4发送一个字符串

    DMA_Config();
    EA = 1; //允许全局中断

    DmaTxFlag = 0;
    DmaRxFlag = 0;
    
    while (1)
    {
        if(B_RX4_TimeOut)
        {
            B_RX4_TimeOut = 0;

            i = ((u16)DMA_UR4R_DONEH << 8) + DMA_UR4R_DONE;
            i--;
            DMA_UR4T_AMT = (u8)i;       //设置传输总字节数(低8位)：i+1
            DMA_UR4T_AMTH = (u8)(i>>8); //设置传输总字节数(高8位)：i+1

            //关闭接收DMA，下次接收的数据重新存放在起始地址位置，否则下次接收数据继续往后面存放。同时会清除 DMA_UR4R_DONE
            DMA_UR4R_CR = 0;
//            printf("cnt=%u\r\n",i);
            DMA_UR4T_CR = 0xc0;         //bit7 1:使能 UART4_DMA, bit6 1:开始 UART4_DMA 自动发送
            DMA_UR4R_CR = 0xa1;         //bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
        }

//        if((DmaTxFlag) && (DmaRxFlag))
//        {
//            DmaTxFlag = 0;
//            DMA_UR4T_CR = 0xc0;     //bit7 1:使能 UART4_DMA, bit6 1:开始 UART4_DMA 自动发送
//            DmaRxFlag = 0;
//            DMA_UR4R_CR = 0xa1;     //bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
//        }
    }
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: UART DMA 功能配置。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-07-28
// 备注: 
//========================================================================
void DMA_Config(void)
{
    DMA_UR4T_CFG = 0x00;    //bit7 1:Enable Interrupt
    DMA_UR4T_STA = 0x00;
    DMA_UR4T_AMT = 0xff;    //设置传输总字节数(低8位)：n+1
    DMA_UR4T_AMTH = 0x00;   //设置传输总字节数(高8位)：n+1
    DMA_UR4T_TXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR4T_TXAL = (u8)((u16)&DmaBuffer);
    DMA_UR4T_CR = 0xc0;     //bit7 1:使能 UART4_DMA, bit6 1:开始 UART4_DMA 自动发送

    DMA_UR4R_CFG = 0x00;    //bit7 1:Enable Interrupt
    DMA_UR4R_STA = 0x00;
    DMA_UR4R_AMT = 0xff;    //设置传输总字节数(低8位)：n+1
    DMA_UR4R_AMTH = 0x00;   //设置传输总字节数(高8位)：n+1
    DMA_UR4R_RXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR4R_RXAL = (u8)((u16)&DmaBuffer);
    DMA_UR4R_CR = 0xa1;     //bit7 1:使能 UART4_DMA, bit5 1:开始 UART4_DMA 自动接收, bit0 1:清除 FIFO
}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-07-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 使用Timer2做波特率.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    ET2 = 0;    //禁止中断
    T2R = 1;    //Timer run enable
}

//========================================================================
// 函数: void UART4_config(u8 brt)
// 描述: UART4初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-07-28
// 备注: 
//========================================================================
void UART4_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
        S4CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
    }
    else
    {
        T4R = 0;    //Timer stop
        S4CON = 0x50;   //8位数据, 使用Timer4做波特率发生器, 允许接收
        T4H = (65536UL - (MAIN_Fosc / 4) / Baudrate4) / 256;
        T4L = (65536UL - (MAIN_Fosc / 4) / Baudrate4) % 256;
        T4_CT = 0;  //Timer4 set As Timer
        T4x12 = 1;  //Timer4 set as 1T mode
        T4R = 1;    //Timer run enable
    }
//    ES4  = 1;       //允许UART4中断
    S4_S = 0;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    UR4TOCR = 0xc0; //bit7:使能超时接收，bit6:使能超时中断，bit5:超时时钟选择 1:系统时钟  0:串口数据位率(波特率)
    UR4TOTL = 0x40;
    UR4TOTH = 0x00; //0x5666 = 22118
    UR4TOTE = 0x00; //需要写 UR4TOTE 后，新的TO值才会生效
}

//========================================================================
// 函数: void UART4_int (void) interrupt UART4_VECTOR
// 描述: UART4中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-07-28
// 备注: 
//========================================================================
void UART4_int (void) interrupt 18
{
//    if(S4RI)
//    {
//        S4RI = 0;    //Clear Rx flag
//        RX4_Buffer[RX4_Cnt] = S4BUF;
//        if(++RX4_Cnt >= UART4_BUF_LENGTH)   RX4_Cnt = 0;
//    }

//    if(S4TI)
//    {
//        S4TI = 0;    //Clear Tx flag
//        B_TX4_Busy = 0;
//    }

    if(UR4TOSR & 0x01)
    {
        B_RX4_TimeOut = 1;
        UR4TOSR = 0x80; //设置 RTOCF 清除超时标志位 TOIF
    }
}

//========================================================================
// 函数: void UART4_DMA_Interrupt (void) interrupt 56/57
// 描述: UART4 DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-07-28
// 备注: 
//========================================================================
void UART4_DMA_Interrupt(void) interrupt 13
{
    if (DMA_UR4T_STA & 0x01)    //发送完成
    {
        DMA_UR4T_STA &= ~0x01;
        DmaTxFlag = 1;
    }
    if (DMA_UR4T_STA & 0x04)    //数据覆盖
    {
        DMA_UR4T_STA &= ~0x04;
    }
    
    if (DMA_UR4R_STA & 0x01)    //接收完成
    {
        DMA_UR4R_STA &= ~0x01;
        DmaRxFlag = 1;
    }
    if (DMA_UR4R_STA & 0x02)    //数据丢弃
    {
        DMA_UR4R_STA &= ~0x02;
    }
}
