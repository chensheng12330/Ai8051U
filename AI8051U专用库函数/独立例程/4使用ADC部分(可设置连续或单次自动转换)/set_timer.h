#ifndef __SET_TIMER_H__
#define __SET_TIMER_H__

#include "AI8051U.H"

// 定时器枚举
typedef enum
{
    Timer0 = 0, // 定时器0可以使用模式0~3
    Timer1,     // 定时器1可以使用模式0~2
    Timer2,     // 定时器2~4只能使用模式0
    Timer3,
    Timer4,
    Timer11
} timer_num;

// 定时器参数定义
#define Timer_End "end"              // 结束标志
#define En_Int "\x01int1"         // 打开对应定时器的中断功能，默认是打开的
#define Dis_Int "\x01int0"       // 关闭对应定时器的中断功能
#define En_OutClk "\x01outclk1"   // 打开对应定时器的时钟输出功能，默认是关闭的
#define Dis_OutClk "\x01outclk0" // 关闭对应定时器的时钟输出功能
// T0CLKO=P35, T1CLKO=P34, T2CLKO=P11, T3CLKO=P05, T4CLKO=P07, T11CLKO=P21
// 定时参数分为定时时间和定时周期两种输入方式。如果不填入，默认值为定时时间1s
// 定时周期支持单位hz（小写的赫兹），定时时间支持单位ms（小写的毫秒）和s （大写的秒）
// 支持浮点输入，类似于"1.5s"、"120ms"、"333hz"都是合法的输入

// 函数介绍：设置定时器模式，支持参数乱序输入和默认值配置功能
// 默认值为1s定时，打开中断，关闭时钟输出功能。即set_timer_mode(Timer0, Timer_End);
// 等效于：set_timer_mode(Timer0, "1s", Dis_OutClk, En_Int, Timer_End);
// 下面这个例子的功能是设置Timer0为1000hz的定时器。
// set_timer_mode(Timer0, "1000hz", Timer_End);
// 下面这个例子的功能是设置Timer0为10ms的定时器。
// set_timer_mode(Timer0, "10ms", Timer_End);
void set_timer_mode(timer_num num, ...);

//这里是获取定时器的中断状态的，使用高级配置功能后，会自动开启定时器和对应的中断。
//中断处理函数在已经被定义过了，用户只需要关心定时器状态返回值即可。
//函数传入的参数是定时器的编号，返回值是定时器的状态，0表示定时器没有中断，1表示定时器中断。
char get_timer_state(timer_num num);

//用于设置定时器部分的时钟源频率，fosc为时钟频率，单位为Hz。
//不使用这个函数的情况下，主频是自动获取，用户可以不用关心这个问题。
void set_timer_fosc(long fosc);

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P0 = ~P0};
//set_timer_isr(Timer0, isr);//设置isr函数为Timer0的中断函数
void set_timer_isr(timer_num num, void (*isr)(void)); // 设置中断服务程序

#endif