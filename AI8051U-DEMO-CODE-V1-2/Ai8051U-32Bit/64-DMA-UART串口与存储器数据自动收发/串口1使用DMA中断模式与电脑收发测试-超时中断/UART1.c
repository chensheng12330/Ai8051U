/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

串口1全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU将收到的数据自动存入DMA空间。

当DMA空间存满设置大小的内容后，通过串口1的DMA自动发送功能把存储空间的数据原样返回。

设置串口超时中断，MCU串口接收完一串数据后如果超过设置时间没有收到新数据，触发超时中断，将已经收取的数据原样发出。

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate1       115200L

bit DmaTxFlag;
bit DmaRxFlag;
bit B_RX1_TimeOut;

u8 xdata DmaBuffer[1024];

void UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void DMA_Config(void);

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

    for(i=0; i<512; i++)
    {
        DmaBuffer[i] = i;
    }

    UART1_config(1);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    printf("AI8051U UART1 TimeOut Test Programme!\r\n");  //UART1发送一个字符串

    DMA_Config();
    EA = 1; //允许总中断

//    DmaTxFlag = 0;
//    DmaRxFlag = 0;

    while (1)
    {
        if(B_RX1_TimeOut)       //接收一串数据结束，产生超时中断
        {
            B_RX1_TimeOut = 0;

            i = ((u16)DMA_UR1R_DONEH << 8) + DMA_UR1R_DONE; //获取已接收字节个数
            TI = 0; //清除发送标志（DMA发送完不会自动清除标志位）
            printf("cnt=%u\r\n",i);

            DMA_UR1R_CR = 0;
            i--;    //传输字节设置为(i-1)个数据
            DMA_UR1T_AMT = (u8)i;       //设置传输总字节数(低8位)：n+1
            DMA_UR1T_AMTH = (u8)(i>>8); //设置传输总字节数(高8位)：n+1

            DMA_UR1T_CR = 0xc0;         //bit7 1:使能 UART1_DMA, bit6 1:开始 UART1_DMA 自动发送
            DMA_UR1R_CR = 0xa1;         //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
        }

//        if((DmaTxFlag) && (DmaRxFlag))  //收发完成DMA指定字节数据
//        {
//            DmaTxFlag = 0;
//            DMA_UR1T_CR = 0xc0;         //bit7 1:使能 UART1_DMA, bit6 1:开始 UART1_DMA 自动发送
//            DmaRxFlag = 0;
//            DMA_UR1R_CR = 0xa1;         //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
//        }
    }
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: UART DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_UR1T_CFG = 0x00;        //bit7 0:Disable Interrupt
    DMA_UR1T_STA = 0x00;
    DMA_UR1T_AMT = 0xff;        //设置传输总字节数(低8位)：n+1
    DMA_UR1T_AMTH = 0x01;       //设置传输总字节数(高8位)：n+1
    DMA_UR1T_TXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR1T_TXAL = (u8)((u16)&DmaBuffer);
    DMA_UR1T_CR = 0x80;         //bit7 1:使能 UART1_DMA, bit6 1:开始 UART1_DMA 自动发送

    DMA_UR1R_CFG = 0x00;        //bit7 0:Disable Interrupt
    DMA_UR1R_STA = 0x00;
    DMA_UR1R_AMT = 0xff;        //设置传输总字节数(低8位)：n+1
    DMA_UR1R_AMTH = 0x03;       //设置传输总字节数(高8位)：n+1
    DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer);
    DMA_UR1R_CR = 0xa1;         //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
}

//========================================================================
// 函数: SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
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
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
    /*********** 波特率使用定时器2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;      //S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate1));
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;      //S1 BRT Use Timer1;
        T1_CT = 0;      //Timer1 set As Timer
        T1x12 = 1;      //Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;        //禁止中断
        TR1  = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
//  ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;  //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    UR1TOCR = 0xc0; //使能超时接收，使能超时中断，超时时钟选择 1:系统时钟  0:串口数据位率(波特率)
    UR1TOTL = 0x10;
    UR1TOTH = 0x00; //0x5666 = 22118
    UR1TOTE = 0x00; //需要写 UR1TOTE 后，新的TM值才会生效
}

//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
//    if(RI)
//    {
//        RI = 0;
//        RX1_Buffer[RX1_Cnt] = SBUF;
//        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
//    }

//    if(TI)
//    {
//        TI = 0;
//        B_TX1_Busy = 0;
//    }
    
    if(UR1TOSR & 0x01)
    {
        P42 = !P42;
        B_RX1_TimeOut = 1;
        UR1TOSR = 0x80; //设置 RTOCF 清除超时标志位 TOIF
    }
}

//========================================================================
// 函数: void UART1_DMA_Interrupt (void) interrupt 50/51
// 描述: UART1 DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
//void UART1_DMA_Interrupt(void) interrupt 13
//{
//    if (DMA_UR1T_STA & 0x01)    //发送完成
//    {
//        DMA_UR1T_STA &= ~0x01;
//        DmaTxFlag = 1;
//    }
//    if (DMA_UR1T_STA & 0x04)    //数据覆盖
//    {
//        DMA_UR1T_STA &= ~0x04;
//    }
//    
//    if (DMA_UR1R_STA & 0x01)    //接收完成
//    {
//        DMA_UR1R_STA &= ~0x01;
//        DmaRxFlag = 1;
//    }
//    if (DMA_UR1R_STA & 0x02)    //数据丢弃
//    {
//        DMA_UR1R_STA &= ~0x02;
//    }
//}
