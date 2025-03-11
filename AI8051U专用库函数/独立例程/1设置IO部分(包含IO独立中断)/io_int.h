#ifndef __IO_INT_H__
#define __IO_INT_H__

#include "AI8051U.H"
#include "set_io.h"
// IOINT模式枚举
typedef enum
{
    dis_int = 0,              // 禁止IO中断
    en_int,                   // 使能IO中断
    falling_edge_mode = 10,   // 下降沿中断模式
    rising_edge_mode,         // 上升沿中断模式
    low_level_mode,           // 低电平中断模式
    high_level_mode,          // 高电平中断模式
    priority_base = 20,       // 中断优先级最低(默认)
    priority_low,             // 中断优先级低
    priority_medium,          // 中断优先级中
    priority_high,            // 中断优先级高
    en_wakeup = 30,           // 使能IO唤醒
    dis_wakeup                // 禁止IO唤醒
} ioint_mode;

// 外部使用函数，第一个参数为ioint_mode枚举类型，第二个参数及其后面为io_name枚举类型
// 这是一个变长函数，举一个例子：
// set_ioint_mode(en_int,Pin00,Pin01,Pin02,Pin20,Pin_End);
// 这样是将Pin00,Pin01,Pin02,Pin20的IO中断模式都使能，最后必须为Pin_End
void set_ioint_mode(ioint_mode mode, ...);

// 这是一个获取中断标志位的函数，参数为io_name枚举类型，返回值为char类型
// 返回值只会出现0和1，0表示没有中断，1表示有中断，查询一次后自动清除
char get_ioint_state (io_name pin);

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P1 = ~P1};
//set_ioint_isr(Pin01, isr);//设置isr函数为P0口的中断函数(Pin00~Pin07都是一个效果)
void set_ioint_isr(io_name pin, void (*isr)(void)); // 设置中断服务程序

#endif