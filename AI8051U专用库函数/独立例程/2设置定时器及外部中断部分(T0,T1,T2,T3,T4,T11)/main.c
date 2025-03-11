#include "AI8051U.h"
#include "set_int.h"   //设置外部中断的库函数
#include "set_io.h"    //设置IO模式的库函数，可以阅读H文件中的详细说明，或者直接看以下例程的使用
#include "set_timer.h" //设置定时器的库函数

/*
使用说明：
效果应该是按下P32处的按键，P00（LED00）处的灯改变一次亮/灭状态。同理，P33按下改变P01（LED01）
同时，P02（LED02）以1秒的间隔交替亮灭。
*/

void main(void)
{
    EAXFR = 1; // 允许访问扩展寄存器
    WTST = 0;
    CKCON = 0;
    // AI8051U试验箱所需配置
    P40 = 0; // 打开试验箱LED电源供电
    // set_io.h 函数使用例程
    set_io_mode(pp_mode, Pin40, Pin_End);
    // 设置P40为推挽输出模式
    set_io_mode(pu_mode, Pin00, Pin01, Pin02, Pin32, Pin33, Pin_End);
    // 设置P00,P01,P02,P32(INT0),P33(INT1)为准双向模式
    // 最后一个参数必须是Pin_End，否则函数无法停止解析，Pin_End是结束标志
    // 变长函数，可以设定任意数量的引脚

    // set_int.h 函数使用例程
    set_int_mode(falling_edge_mode, Int0, Int1, Int_End); // 设置INT0、INT1中断模式为下降沿触发

    // set_timer.h 函数使用例程
    set_timer_mode(Timer0, Timer_End); // 设置定时器0为默认配置，定时时间为为1s，开启中断，关闭时钟输出
    // 定时参数分为定时时间和定时周期两种输入方式。如果不填入，默认值为定时时间1s
    // 定时周期支持单位hz（小写的赫兹），定时时间支持单位ms（小写的毫秒）和s （大写的秒）
    // 支持浮点输入，类似于"1.5s"、"120ms"、"333hz"都是合法的输入
    // 设置后会自动启动定时器和对应中断，只需要使用get_timer_state查询即可
    EA = 1; // 打开总中断
    while (1)
    {
        // 现象是按下P32处的按键，P00处的灯改变一次亮/灭状态。同理，P33按下改变P01
        // 同时，P02以1秒的间隔交替亮灭。
        if (get_int_state(Int0))
            P00 = ~P00; // 如果INT0中断，则翻转P00电平
        if (get_int_state(Int1))
            P01 = ~P01; // 如果INT1中断，则翻转P01电平
        if (get_timer_state(Timer0))
            P02 = ~P02; // 对应IO口上的LED灯1S变化一次
    }
}