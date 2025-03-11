#include "io_int.h"
#include "stdarg.h"

char xdata ioint_isr_state[6] = {0,0,0,0,0,0}; // 定义中断状态变量
char ioint_flag = 0;                  // 触发中断标志，用于加速查询过程
void (*_ioint_isr[6])(void) = {0};    //定义函数指针数组

#define _XDATA_REG(offset) (*(unsigned char volatile far *)(0x7efd00+offset))
#define _SET_XDATA_REG(offset, num, value) { _XDATA_REG(offset) &= ~(1<<(num));_XDATA_REG(offset) |= ((value) & 1) << (num);}
char ioint_offset[4] = {0x0,0x20,0x40,0x42};//中断使能偏移地址，中断模式偏移地址，中断优先级偏移地址，中断唤醒偏移地址
// 定义宏：通用设置 IO 功能
#define SET_IOINT(port, num, value, type){if((type/10)==1){_SET_XDATA_REG(ioint_offset[type/10]+port, num, (type%10)&1);\
    _SET_XDATA_REG(ioint_offset[type/10]+0x10+port, num, ((type%10)>>1)&1);/*设置中断模式*/\
    }else if((type/10)==2){_SET_XDATA_REG(ioint_offset[type/10], port, (type%10)&1);\
    _SET_XDATA_REG(ioint_offset[type/10]+1, port, ((type%10)>>1)&1);/*设置中断优先级*/\
    }else{_SET_XDATA_REG(ioint_offset[type/10]+port, num, (type%10)&1);}/*设置中断唤醒、中断使能*/}

void set_bit_ioint(char port, char num, ioint_mode mode)
{
    if (port < 0 || port > 5)return; // 检查端口号是否有效
    SET_IOINT(port, num, mode, mode);
}

void set_ioint_mode(ioint_mode mode, ...)
{
    va_list args;         // 可变参数列表
    char port, num;       // 端口号
    va_start(args, mode); // 初始化可变参数列表
    while (1)
    {
        io_name pin = va_arg(args, io_name); // 获取下一个引脚编号
        if (pin == Pin_End)
            break;                      // 遇到哨兵值，结束循环
        port = pin / 8;                 // 计算端口号（P0~P5）
        num = pin % 8;                  // 计算引脚号（0~7）
        set_bit_ioint(port, num, mode); // 设置引脚模式
    }
    va_end(args); // 清理可变参数列表
}

char get_ioint_state(io_name pin)
{
    char state, port, num;
    if (ioint_flag == 0)
        return 0;
    port = pin / 8; // 计算端口号（P0~P5）
    num = pin % 8;  // 计算引脚号（0~7）
    state = ((ioint_isr_state[port] >> num) & 1);
    ioint_isr_state[port] &= ~(1 << num); // 清除中断状态位
		ioint_flag = ioint_isr_state[0]|ioint_isr_state[1]|ioint_isr_state[2]|
		ioint_isr_state[3]|ioint_isr_state[4]|ioint_isr_state[5];
    return (state); // 返回中断状态
}

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P1 = ~P1};
//set_ioint_isr(Pin01, isr);//设置isr函数为P0口的中断函数(Pin00~Pin07都是一个效果)
void set_ioint_isr(io_name pin, void (*isr)(void)) // 设置中断服务程序
{
		char port = pin / 8;//计算端口号
		if(port >= 6)return;//范围限制
    _ioint_isr[port] = isr;
}

// 中断部分宏定义
#define pin_int_isr(port_num)                                          \
    void p##port_num##_int_isr(void) interrupt P##port_num##INT_VECTOR \
    {                                                                  \
				ioint_flag = 1;/* 设置中断加速标志位 */												\
        ioint_isr_state[port_num] |= (P##port_num##INTF&P##port_num##INTE); \
        P##port_num##INTF = 0; /* 清除中断标志位 */             \
				if(_ioint_isr[port_num] != 0)_ioint_isr[port_num]();                       \
    }
pin_int_isr(0) pin_int_isr(1) pin_int_isr(2) pin_int_isr(3) pin_int_isr(4) pin_int_isr(5)