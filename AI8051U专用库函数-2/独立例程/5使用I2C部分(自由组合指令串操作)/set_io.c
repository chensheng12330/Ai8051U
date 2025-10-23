#include "set_io.h"
#include "stdarg.h"

// // 定义通用宏：清除后赋值
#define _SET_IO_REG(port, reg, num, value) { P##port##reg &= ~(1 << (num)); P##port##reg |= ((value) & 1) << (num);}
#define _XDATA_REG(offset) (*(unsigned char volatile far *)(0x7efe10+offset))
#define _SET_XDATA_REG(offset, num, value) { _XDATA_REG(offset) &= ~(1<<(num));_XDATA_REG(offset) |= ((value) & 1) << (num);}
// // 定义宏：通用设置 IO 功能
char io_offset[8] = {0,0x0,0x8,0x10,0x18,0x20,0x30,0x38};
#define SET_IO(port, num, type) { \
    if(type<=3){_SET_IO_REG(port, M0, num, (type) & 1);_SET_IO_REG(port, M1, num, ((type) >> 1) & 1); break;}\
    if(type>=10&&type<=71){_SET_XDATA_REG(io_offset[type/10]+port, num, type%10);}}
void set_bit_io(char port, char num, io_mode mode)
{
    if (port < 0 || port > 5)return; // 检查端口号是否有效
    switch (port) {
//        case 0:SET_IO(0,num,mode);break;
//        case 1:SET_IO(1,num,mode);break;
//        case 2:SET_IO(2,num,mode);break;
        case 3:SET_IO(3,num,mode);break;
//        case 4:SET_IO(4,num,mode);break;
//        case 5:SET_IO(5,num,mode);break;
    }
}
void set_io_mode(io_mode mode, ...)
{
    va_list args;// 可变参数列表
    char port,num;// 端口号和引脚号
    va_start(args, mode);  // 初始化可变参数列表
    while (1) {
        io_name pin = va_arg(args, io_name);  // 获取下一个引脚编号
        if (pin == Pin_End) break;        // 遇到哨兵值，结束循环
        port = pin / 8;  // 计算端口号（P0~P5）
        num = pin % 8;   // 计算引脚号（0~7）
        set_bit_io(port, num, mode);// 设置引脚模式
    }
    va_end(args);  // 清理可变参数列表
}
