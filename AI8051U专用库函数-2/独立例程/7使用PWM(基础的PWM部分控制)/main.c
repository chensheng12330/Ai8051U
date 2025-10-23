#include "set_io.h"	 //设置I/O部分的库函数
#include "set_pwm.h" //设置PWM部分的库函数，支持基本的占空比和周期设置
#include "set_uart.h" //设置串口部分的库函数
#include "AI8051U.H"
/*
使用说明：
PWM通过P00和P02输出两个不同占空比的PWM波，从LED灯上观察到两个灯的亮度会不一样
其中，P01上通过PWM1N输出PWM，由PWM5P进行捕获后，使用串口显示，串口发送任意字符触发捕获占空比和周期
因为捕获到的是PWM1N互补信号，所以占空比相应的应该为20%（100%减去80%得到）
*/
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	
	//set_uart.h 函数使用例程
	set_uart_mode(Uart1, Uart_End);//全默认值快速配置PWM

	// set_io.h 函数使用例程
	set_io_mode(pp_mode, Pin40, Pin00, Pin01, Pin02, Pin_End);
	// 配置实验箱上的LED电源P40，以及LED灯所在的IO口P00和P02为推挽输出模式
	P40 = 0; // 打开LED灯的电源
	
	// set_pwm.h 函数使用例程
	set_pwm_mode(Pwm1, Pwm1_P00_01, "2khz", "80%", En_Out_PN, Pwm_End);//打开互补输出
	set_pwm_mode(Pwm2, Pwm2_P02_03, "20%", Pwm_End);
	// 切换PWM为不同的占空比
	set_pwm_mode(Pwm5, Pwm5_P01, Pwm_In_Mode, Self_Capture, Cap_In_Rising_Edge, Pwm_End);
	//将PWM5P切换到P01口，切换到PWM输入模式，映射到当前通道，捕获上升沿
	set_pwm_mode(Pwm6, Pwm_In_Mode, Cross_Capture, Cap_In_Falling_Edge, Pwm_End);
	//切换为输入模式，映射到旁边通道(PWM5P),捕获下降沿
	EA = 1;//使能总中断
	while (1)
	{
		if(get_uart_state(Uart1))//检测到Uart1有输入数据
		{
			uart_printf(Uart1, "duty:%.2f, cycl:%.2fkhz\r\n",get_pwm_duty(Pwm5_Pwm6), get_pwm_period(Pwm5, khz));
			//返回当前捕获到的占空比和周期（周期单位为khz）
		}
	}
}