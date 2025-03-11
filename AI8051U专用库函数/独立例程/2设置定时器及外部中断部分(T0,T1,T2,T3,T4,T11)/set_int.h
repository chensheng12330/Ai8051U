#ifndef __SET_INT_H__
#define __SET_INT_H__

#include "AI8051U.H"

// INT模式枚举
typedef enum
{
    rising_falling_edge_mode = 0, // 边沿中断模式
    falling_edge_mode,            // 下降沿中断模式
    dis_int                       // 关闭中断
} int_mode;

// INT模式枚举
typedef enum
{
    Int0 = 0,      // 外部中断0，支持边沿中断和下降沿中断
    Int1,          // 外部中断1，支持边沿中断和下降沿中断
    Int2,          // 外部中断2，仅支持下降沿中断
    Int3,          // 外部中断3，仅支持下降沿中断
    Int4,          // 外部中断4，仅支持下降沿中断
    Int_End = 0xff // 中断数字结束
} int_num;

//函数功能：设置外部中断INT0~INT4的触发方式.可以一次性设置同一种模式的多个中断。
//因为是变长函数，所以最后需要加上INT_End结束。
//举例：set_int_mode(all_edge_mode,Int0,Int1,Int_End);//设置INT0和INT1为边沿中断
void set_int_mode(int_mode mode, ...);

// 函数功能：获取中断状态
// 返回值：0表示没有中断，1表示有中断
// 举例：if(get_int_state(Int0))//判断INT0是否有中断
char get_int_state(int_num num);

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P0 = ~P0};
//set_int_isr(Int0, isr);//设置isr函数为Int0的中断函数
void set_int_isr(int_num num, void (*isr)(void)); // 设置中断服务程序

#endif