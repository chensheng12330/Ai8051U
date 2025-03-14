/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

本程序演示5个定时器的使用, 本例程均使用16位自动重装.

定时器0做16位自动重装, 中断频率为1000HZ，中断函数从P0.7取反输出500HZ方波信号.

定时器1做16位自动重装, 中断频率为2000HZ，中断函数从P0.6取反输出1000HZ方波信号.

定时器2做16位自动重装, 中断频率为3000HZ，中断函数从P0.5取反输出1500HZ方波信号.

定时器3做16位自动重装, 中断频率为4000HZ，中断函数从P0.4取反输出2000HZ方波信号.

定时器4做16位自动重装, 中断频率为5000HZ，中断函数从P0.3取反输出2500HZ方波信号.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

//==========================================================================

#define Timer0_Reload   (MAIN_Fosc / 1000)      //Timer 0 中断频率, 1000次/秒
#define Timer1_Reload   (MAIN_Fosc / 2000)      //Timer 1 中断频率, 2000次/秒
#define Timer2_Reload   (MAIN_Fosc / 3000)      //Timer 2 中断频率, 3000次/秒
#define Timer3_Reload   (MAIN_Fosc / 4000)      //Timer 3 中断频率, 4000次/秒
#define Timer4_Reload   (MAIN_Fosc / 5000)      //Timer 4 中断频率, 5000次/秒

void    Timer0_init(void);
void    Timer1_init(void);
void    Timer2_init(void);
void    Timer3_init(void);
void    Timer4_init(void);

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
    P4M1 = 0x00;   P4M0 = 0x0b;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    EA = 1;     //打开总中断
    P40 = 0;		//打开LED电源
    
    Timer0_init();
    Timer1_init();
    Timer2_init();
    Timer3_init();
    Timer4_init();

    while (1);
}

//========================================================================
// 函数: void Timer0_init(void)
// 描述: timer0初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer0_init(void)
{
        TR0 = 0;    //停止计数

    #if (Timer0_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer0设置的中断过快!"

    #elif ((Timer0_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET0 = 1;    //允许中断
    //  PT0 = 1;    //高优先级中断
        TMOD &= ~0x03;
        TMOD |= 0;  //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装, 3: 16位自动重装, 不可屏蔽中断
    //  T0_CT = 1;  //计数
        T0_CT = 0;  //定时
    //  T0CLKO = 1; //输出时钟
        T0CLKO = 0; //不输出时钟

        #if (Timer0_Reload < 65536UL)
            T0x12 = 1;  //1T mode
            TH0 = (u8)((65536UL - Timer0_Reload) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload) % 256);
        #else
            T0x12 = 0;  //12T mode
            TH0 = (u8)((65536UL - Timer0_Reload/12) / 256);
            TL0 = (u8)((65536UL - Timer0_Reload/12) % 256);
        #endif

        TR0 = 1;    //开始运行

    #else
        #error "Timer0设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer1_init(void)
// 描述: timer1初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer1_init(void)
{
        TR1 = 0;    //停止计数

    #if (Timer1_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer1设置的中断过快!"

    #elif ((Timer1_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET1 = 1;    //允许中断
    //  PT1 = 1;    //高优先级中断
        TMOD &= ~0x30;
        TMOD |= (0 << 4);   //工作模式, 0: 16位自动重装, 1: 16位定时/计数, 2: 8位自动重装
    //  T1_CT = 1;  //计数
        T1_CT = 0;  //定时
    //  T1CLKO = 1; //输出时钟
        T1CLKO = 0; //不输出时钟

        #if (Timer1_Reload < 65536UL)
            T1x12 = 1;  //1T mode
            TH1 = (u8)((65536UL - Timer1_Reload) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload) % 256);
        #else
            T1x12 = 0;  //12T mode
            TH1 = (u8)((65536UL - Timer1_Reload/12) / 256);
            TL1 = (u8)((65536UL - Timer1_Reload/12) % 256);
        #endif

        TR1 = 1;    //开始运行

    #else
        #error "Timer1设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer2_init(void)
// 描述: timer2初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer2_init(void)
{
        T2R = 0;    //停止计数

    #if (Timer2_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer2设置的中断过快!"

    #elif ((Timer2_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET2 = 1;    //允许中断
    //  T2_CT = 1;  //计数
        T2_CT = 0;  //定时
    //  T2CLKO = 1; //输出时钟
        T2CLKO = 0; //不输出时钟

        #if (Timer2_Reload < 65536UL)
            T2x12 = 1;    //1T mode
            T2H = (u8)((65536UL - Timer2_Reload) / 256);
            T2L = (u8)((65536UL - Timer2_Reload) % 256);
        #else
            T2x12 = 0;    //12T mode
            T2H = (u8)((65536UL - Timer2_Reload/12) / 256);
            T2L = (u8)((65536UL - Timer2_Reload/12) % 256);
        #endif

        T2R = 1;    //开始运行

    #else
        #error "Timer2设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer3_init(void)
// 描述: timer3初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer3_init(void)
{
        T3R = 0;    //停止计数

    #if (Timer3_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer3设置的中断过快!"

    #elif ((Timer3_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET3 = 1;    //允许中断
    //  T3_CT = 1;  //计数
        T3_CT = 0;  //定时
    //  T3CLKO = 1; //输出时钟
        T3CLKO = 0; //不输出时钟

        #if (Timer3_Reload < 65536UL)
            T3x12 = 1;    //1T mode
            T3H = (u8)((65536UL - Timer3_Reload) / 256);
            T3L = (u8)((65536UL - Timer3_Reload) % 256);
        #else
            T3x12 = 0;    //12T mode
            T3H = (u8)((65536UL - Timer3_Reload/12) / 256);
            T3L = (u8)((65536UL - Timer3_Reload/12) % 256);
        #endif

        T3R = 1;    //开始运行

    #else
        #error "Timer3设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void Timer4_init(void)
// 描述: timer4初始化函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void Timer4_init(void)
{
        T4R = 0;    //停止计数

    #if (Timer4_Reload < 64)    // 如果用户设置值不合适， 则不启动定时器
        #error "Timer4设置的中断过快!"

    #elif ((Timer4_Reload/12) < 65536UL)    // 如果用户设置值不合适， 则不启动定时器
        ET4 = 1;    //允许中断
    //  T4_CT = 1;  //计数
        T4_CT = 0;  //定时
    //  T4CLKO = 1; //输出时钟
        T4CLKO = 0; //不输出时钟

        #if (Timer4_Reload < 65536UL)
            T4x12 = 1;    //1T mode
            T4H = (u8)((65536UL - Timer4_Reload) / 256);
            T4L = (u8)((65536UL - Timer4_Reload) % 256);
        #else
            T4x12 = 0;    //12T mode
            T4H = (u8)((65536UL - Timer4_Reload/12) / 256);
            T4L = (u8)((65536UL - Timer4_Reload/12) % 256);
        #endif

        T4R = 1;    //开始运行

    #else
        #error "Timer4设置的中断过慢!"
    #endif
}

//========================================================================
// 函数: void timer0_int (void) interrupt TIMER0_VECTOR
// 描述:  timer0中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer0_int (void) interrupt 1
{
   P07 = ~P07;
}

//========================================================================
// 函数: void timer1_int (void) interrupt TIMER1_VECTOR
// 描述:  timer1中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer1_int (void) interrupt 3
{
   P06 = ~P06;
}

//========================================================================
// 函数: void timer2_int (void) interrupt TIMER2_VECTOR
// 描述:  timer2中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer2_int (void) interrupt 12
{
    P05 = ~P05;
}

//========================================================================
// 函数: void timer3_int (void) interrupt TIMER3_VECTOR
// 描述:  timer3中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer3_int(void) interrupt 19
{
    P04 = ~P04;
}

//========================================================================
// 函数: void timer4_int (void) interrupt TIMER4_VECTOR
// 描述:  timer4中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2015-1-12
//========================================================================
void timer4_int(void) interrupt 20
{
    P03 = ~P03;
}
