#include "AI8051U.H"
#include "set_io.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
使用说明：
通过实验箱上的跳线帽3：蜂鸣器接口插好后，即可输出变调的蜂鸣器声音
使用了两个线程，线程0用于输出蜂鸣器声音，线程1用于修改蜂鸣器的频率
初始化时将多线程时间基准修改到了为100us，方便频率调节
*/

#define Beep P50 //蜂鸣器引脚
char x_delay = 0;//蜂鸣器延时变量,临时变量
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "100us", Timer_End);//设置定时器0定时时间,调整时基为100us
	set_timer_isr(Timer0, set_task_mode);//设置定时器中断为任务调度
	set_io_mode(pu_mode, Pin50, Pin_End);//设置蜂鸣器引脚为准双向口
	EA = 1;//打开总中断
	while(1)
	{
		task_start(0);//线程0开始,蜂鸣器鸣叫
		Beep = ~Beep;
		task_delay(x_delay);
		task_end(1);//线程0结束,循环执行

		task_start(1);//线程1开始,控制蜂鸣器变调
		task_for(x_delay = 0, x_delay++)
		{
			task_delay(1000);//缓慢上升100us*1000=100ms
		}
		task_break(x_delay < 20);
		task_for(x_delay = 20, x_delay--)
		{
			task_delay(300);//迅速下降100us*300=30ms
		}
		task_break(x_delay > 0);
		task_end(1);//线程1结束,循环执行
	}
}
