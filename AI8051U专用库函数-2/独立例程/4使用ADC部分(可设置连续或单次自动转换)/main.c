#include "AI8051U.h"
#include "set_uart.h" //设置串口的库函数，丰富的使用方法详见H文件中的详细说明
#include "set_timer.h"//设置定时器的库函数，用于固定时间唤醒发送给上位机ADC采样结果
#include "set_adc.h"//设置ADC的库函数，用于ADC采样

/*
使用说明：
串口1（波特率115200）会每隔0.5S返回一次当前读取到的ADC数据。
长按试验箱右侧ADC按键，就可以看到返回的读数变化。
需要注意的是，需要长按才能看到读数的变化，如果点按的话，下次扫描的时候已经是松开的状态了。
*/

unsigned int show_value;
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	// set_uart.h 函数使用例程
	set_uart_mode(Uart1, Uart_End);
	// 设置串口1为默认模式，115200bps，64byte停止中断，默认timer2，p30p31

	// set_timer.h 函数使用例程
	set_timer_mode(Timer0, "0.5s", Timer_End);
	// 设置定时器0的定时时间为0.5s

	// set_adc.h 函数使用例程
	set_adc_mode(cycl_mode, Adc0_P10, Adc_End);
	// 设置为循环触发模式，每次完成adc转换后自动开始下一次
	EA = 1; // 打开总中断
	while (1)
	{
		if(get_adc_state(Adc0_P10))//获取ADC0通道是否转换完成
		{
			show_value = adc_value[Adc0_P10];//对通道数据进行读取
		}
		if(get_timer_state(Timer0))//设定的0.5s时间到
		{
			uart_printf(Uart1,"ADC_KEY_Value:%d\r\n",show_value);//发送当前的ADC数据
			//在试验箱上的现象是：长按某个ADC按键即可得到串口上显示稳定的ADC数值
			//例如长按ADC按键0，和非长按按键零状态有显著区别。
		}
	}
}