/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

将实验箱上的S2拨动开关拨到"通"进行测试.

串口1(P3.6,P3.7)定时发送一串数据给串口2(P4.2,P4.3).

串口2(P4.2,P4.3)输出从串口1(P3.6,P3.7)接收到的数据.

下载时, 选择时钟 22.1184MHZ (用户可自行修改频率).

******************************************/

#include    "..\comm\AI8051U.h"
#include    "intrins.h"

#define     MAIN_Fosc       22118400L   //定义主时钟

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define Baudrate2   115200UL
#define Baudrate1   115200UL

#define UART2_BUF_LENGTH    128
#define UART1_BUF_LENGTH    128

/*****************************************************************************/

/*************  本地变量声明    **************/
bit B_1ms;      //1ms标志
u16 Sec_Cnt;    //1秒计数

u8  TX2_Cnt;    //发送计数
u8  RX2_Cnt;    //接收计数
u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
bit B_TX2_Busy; //发送忙标志
bit B_TX1_Busy; //发送忙标志
u8 RX2_TimeOut;
u8 RX1_TimeOut;

u8  xdata RX2_Buffer[UART2_BUF_LENGTH]; //接收缓冲
u8  xdata RX1_Buffer[UART1_BUF_LENGTH]; //接收缓冲

void UART2_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
void UART2_TxByte(u8 dat);
void UART1_TxByte(u8 dat);
void PrintString2(u8 *puts);
void PrintString1(u8 *puts);
/******************** 主函数 **************************/
void main(void)
{
    u8 i;
	
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

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    UART2_config(2);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
    UART1_config(1);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    EA = 1;     //打开总中断

    PrintString2("AI8051U UART2 Test Programme!\r\n");  //UART2发送一个字符串
    PrintString1("AI8051U UART1 Test Programme!\r\n");  //UART1发送一个字符串

    while (1)
    {
        if(B_1ms)
        {
            B_1ms = 0;
            Sec_Cnt++;
            if(Sec_Cnt>=1000)
            {
                Sec_Cnt = 0;
                PrintString1("AI8051U UART1-UART2 Test Programme!\r\n");  //UART1发送一个字符串
            }
//            if(RX1_TimeOut > 0)     //超时计数
//            {
//                if(--RX1_TimeOut == 0)
//                {
//                    for(i=0; i<RX1_Cnt; i++)    UART1_TxByte(RX1_Buffer[i]);    //串口3把收到的数据原样返回
//                    RX1_Cnt  = 0;   //清除字节数
//                }
//            }

            if(RX2_TimeOut > 0)     //超时计数
            {
                if(--RX2_TimeOut == 0)
                {
                    for(i=0; i<RX2_Cnt; i++)    UART2_TxByte(RX2_Buffer[i]);    //把收到的数据通过串口2输出
                    RX2_Cnt  = 0;   //清除字节数
                }
            }
        }
    }
}


/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
	B_1ms = 1;
}

//========================================================================
// 函数: void UART2_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART2_TxByte(u8 dat)
{
    B_TX2_Busy = 1;
    S2BUF = dat;
    while(B_TX2_Busy);
}

//========================================================================
// 函数: void UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    B_TX1_Busy = 1;
    SBUF = dat;
    while(B_TX1_Busy);
}

//========================================================================
// 函数: void PrintString2(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString2(u8 *puts)
{
    for (; *puts != 0;  puts++)     //遇到停止符0结束
    {
        UART2_TxByte(*puts);
    }
}

//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口3发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString1(u8 *puts)
{
    for (; *puts != 0;  puts++)     //遇到停止符0结束
    {
        UART1_TxByte(*puts);
    }
}

//========================================================================
// 函数: SetTimer2Baudraye(u32 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u32 dat)
{
    T2R = 0;		//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;		//Timer run enable
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
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;    //禁止中断
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x40;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;

}

//========================================================================
// 函数: void UART2_config(u8 brt)
// 描述: UART2初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    if(brt == 2)
    {
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

        S2CFG |= 0x01;     //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
        S2CON = (S2CON & 0x3f) | 0x40;    //UART2模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
        ES2   = 1;         //允许中断
        S2REN = 1;         //允许接收
        S2_S  = 1;         //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3

        B_TX2_Busy = 0;
        TX2_Cnt = 0;
        RX2_Cnt = 0;
    }
}

//========================================================================
// 函数: void UART2_int (void) interrupt UART2_VECTOR
// 描述: UART2中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;    //Clear Rx flag
        if(RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_Buffer[RX2_Cnt] = S2BUF;
        RX2_Cnt++;
        RX2_TimeOut = 5;
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
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
    if(RI)
    {
        RI = 0;    //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;   //Clear Tx flag
        B_TX1_Busy = 0;
    }
}
