#include <AI8051U.H>
#include "set_io.h"//设置I/O部分的库函数
#include "set_pwm.h"//设置PWM部分的库函数，支持基本的占空比和周期设置
/*
使用说明：
PWM通过P00和P02输出两个不同占空比的PWM波，从LED灯上观察到两个灯的亮度会不一样
*/
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;

	// set_io.h 函数使用例程
	set_io_mode(pp_mode, Pin40, Pin00, Pin02, Pin_End); 
	//配置实验箱上的LED电源P40，以及LED灯所在的IO口P00和P02为推挽输出模式
	P40 = 0;//打开LED灯的电源

	// set_pwm.h 函数使用例程
	set_pwm_mode(Pwm1, Pwm1_P00_01, "10khz", "80%", Pwm_End);
	set_pwm_mode(Pwm2, Pwm2_P02_03, "20%", Pwm_End);
	//切换PWM为不同的占空比
	while(1)
	{
		//空循环
	}
}