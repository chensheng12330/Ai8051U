/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

串口4全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口4把收到的数据原样返回.

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        22118400L   //定义主时钟（精确计算115200波特率）

//==========================================================================

#define Baudrate4   (65536 - MAIN_Fosc / 115200 / 4)

#define UART4_BUF_LENGTH    64

//==========================================================================

/*************  本地常量声明    **************/


/*************  IO口定义    **************/

/*************  本地变量声明    **************/

u8  TX4_Cnt;    //发送计数
u8  RX4_Cnt;    //接收计数
bit B_TX4_Busy; //发送忙标志

u8  RX4_Buffer[UART4_BUF_LENGTH]; //接收缓冲

/*************  本地函数声明    **************/

void UART4_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
void PrintString4(u8 *puts);

/****************  外部函数声明和外部变量声明 *****************/


/******************** 主函数 **************************/
void main(void)
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

    UART4_config(0);    //选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
    EA = 1;             //允许全局中断

    PrintString4("AI8051U UART4 Test Programme!\r\n");  //UART4发送一个字符串

    while (1)
    {
        if((TX4_Cnt != RX4_Cnt) && (!B_TX4_Busy))   //收到数据, 发送空闲
        {
            S4BUF = RX4_Buffer[TX4_Cnt];
            B_TX4_Busy = 1;
            if(++TX4_Cnt >= UART4_BUF_LENGTH)   TX4_Cnt = 0;
        }
    }
}

//========================================================================
// 函数: void PrintString4(u8 *puts)
// 描述: 串口4发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString4(u8 *puts)
{
    for (; *puts != 0;  puts++)     //遇到停止符0结束
    {
        S4BUF = *puts;
        B_TX4_Busy = 1;
        while(B_TX4_Busy);
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
void SetTimer2Baudraye(u32 dat)  // 使用Timer2做波特率.
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
// 函数: void UART4_config(u8 brt)
// 描述: UART4初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer4做波特率.
{
    if(brt == 2)
    {
        SetTimer2Baudraye(Baudrate4);
        S4CON = 0x10;       //8位数据, 使用Timer2做波特率发生器, 允许接收
    }
    else
    {
        T4R = 0;	//Timer stop
        S4CON = 0x50;       //8位数据, 使用Timer4做波特率发生器, 允许接收
        T4H = (u8)(Baudrate4 / 256);
        T4L = (u8)(Baudrate4 % 256);
        T4_CT = 0;	//Timer3 set As Timer
        T4x12 = 1;	//Timer3 set as 1T mode
        T4R = 1;	//Timer run enable
    }
    ES4  = 1;       //允许UART4中断
    S4_S = 0;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    B_TX4_Busy = 0;
    TX4_Cnt = 0;
    RX4_Cnt = 0;
}


//========================================================================
// 函数: void UART4_int (void) interrupt UART4_VECTOR
// 描述: UART4中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART4_int (void) interrupt 18
{
    if(S4RI)
    {
        S4RI = 0;    //Clear Rx flag
        RX4_Buffer[RX4_Cnt] = S4BUF;
        if(++RX4_Cnt >= UART4_BUF_LENGTH)   RX4_Cnt = 0;
    }

    if(S4TI)
    {
        S4TI = 0;    //Clear Tx flag
        B_TX4_Busy = 0;
    }
}

