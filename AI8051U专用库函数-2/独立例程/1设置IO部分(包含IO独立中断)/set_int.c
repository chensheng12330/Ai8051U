#include "set_int.h"
#include "stdarg.h"

//设置模式宏
#define SET_MODE(mode, x) \
switch(mode) { \
case rising_falling_edge_mode: IT##x = 0; EX##x = 1; break; \
case falling_edge_mode: IT##x = 1; EX##x = 1; break; \
case dis_int: IT##x = 0; EX##x = 0; break; }\

char int_flag = 0;//中断标志,用于缓存获取到的中断标志
void (*_int_isr[5])(void) = {0};//定义函数指针数组

//设置中断模式
void set_int_mode(int mode, ...)
{
    va_list args;// 可变参数列表
    va_start(args, mode);  // 初始化可变参数列表
    while (1) {
        int_num num = va_arg(args, int_num);  // 获取下一个INT编号
        if (num == Int_End||num>Int4) break;  // 遇到哨兵值，结束循环
        switch(num)
        {
            case Int0:SET_MODE(mode,0);break;//设置INT0相关的模式
            case Int1:SET_MODE(mode,1);break;//设置INT1相关的模式
            default:
            if(mode == dis_int) INTCLKO &= ~(1<<(num+2));//禁止中断
            else INTCLKO |= (1<<(num+2));break;//设置INT2、INT3、INT4相关的模式
        }
    }
    va_end(args);  // 清理可变参数列表
}

//获取中断状态，传入中断编号，返回0为没有中断，返回1为有中断
char get_int_state(int_num num)
{
    char state = 0;// 中断状态
    if(int_flag == 0)return 0;// 没有中断,提前返回
    state = ((int_flag>>num)&1);// 获取中断状态
    if(state == 0)return 0;
    int_flag &= ~(1<<num);// 清除中断标志缓存
    return state;// 返回中断状态
}

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P0 = ~P0};
//set_int_isr(Int0, isr);//设置isr函数为Int0的中断函数
void set_int_isr(int_num num, void (*isr)(void)) // 设置中断服务程序
{
    _int_isr[num] = isr;
}

#define int_isr(n) \
void int##n##_isr(void) interrupt INT##n##_VECTOR \
{ int_flag |= (1 << n); if(_int_isr[n] != 0)_int_isr[n]();}
int_isr(0)int_isr(1)int_isr(2)int_isr(3)int_isr(4)// 中断服务函数