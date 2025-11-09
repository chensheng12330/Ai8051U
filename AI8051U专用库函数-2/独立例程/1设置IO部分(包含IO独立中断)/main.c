#include "AI8051U.h"
#include "io_int.h" //设置IO中断的库函数,会依赖set_io.h
#include "set_io.h" //设置IO模式的库函数，可以阅读H文件中的详细说明，或者直接看以下例程的使用

/*
使用说明：
效果应该是P02、P04、P07口亮起，P00，001微微亮。
按一次试验箱上的P34按键就会对原来LED的状态取反。
*/

void main(void)
{
    EAXFR = 1; // 允许访问扩展寄存器
    WTST = 0;
    CKCON = 0;
    // AI8051U试验箱所需配置
    P40 = 0; // 打开试验箱LED电源供电
    // set_io.h 函数使用例程
    set_io_mode(pp_mode, Pin02, Pin04, Pin07, Pin40, Pin_End); // 设置P02,P04,P07,P40为推挽输出模式
    set_io_mode(en_pdr, Pin00, Pin01, Pin_End);              // 设置P00,P01为下拉电阻开启
    set_io_mode(en_pur, Pin34, Pin_End);                     // 设置P34按键的上拉电阻开启
    // 最后一个参数必须是Pin_End，否则函数无法停止解析，Pin_End是结束标志
    // 变长函数，可以设定任意数量的引脚
	
    // io_int.h 函数使用例程
    set_ioint_mode(falling_edge_mode, Pin34, Pin_End); // 设置P34为下降沿中断模式
    set_ioint_mode(en_int, Pin34, Pin_End);         // 打开P34的IO口中断
    EA = 1;                                         // 打开总中断
	
    P0 = 0x00;
    // 让P0口的测试LED亮起来，这里使用AI8051U试验箱测试
    // 实际效果应该是P02、P04、P07口亮起，P00，001微微亮
    while (1)
    {
        if (get_ioint_state (Pin34))
        {
            P0 = ~P0; // 翻转P0的值,按一次试验箱上的P34按键就会完成一次亮/灭切换
        }
    }
}