#include "ai8051u.h"
#include "set_io.h"
#include "set_timer.h"
#include "set_task.h"

/*
使用说明：
使用协程前需要设定协程的时间基准base，推荐使用1ms的时间基准，并且使用定时器中断方式设置
本例子通过一个简单的多任务程序展现协程的使用效果
任务1：LED00每隔100ms闪烁一次，按下P32按键时，暂停LED00的闪烁，循环执行
任务2：LED01亮200ms，灭500ms，循环执行
任务3：LED02先灭1000ms，然后{快速闪烁(50ms)3次，常亮300ms}<-如此循环5次，循环执行
*/

int cnt1 = 0, cnt2 = 0;//定义循环用变量

void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	set_timer_mode(Timer0, "1ms", Timer_End);//设定定时器0定时时间为1ms
	set_timer_isr(Timer0, set_task_mode);//设置协程时间基准
	
	set_io_mode(pu_mode, Pin40, Pin00, Pin01, Pin02, Pin_End);//设定P40、P00、P01、P02的模式为准双向口模式
	P40 = 0;//打开LED的供电
	//set_io_mode(hz_mode, Pin32, Pin_End);//上电端口默认为高阻，所以可以不用重复设置端口模式
	set_io_mode(en_pur, Pin32, Pin_End);//打开P32端口的上拉电阻，方便读取按键状态
	EA = 1;//打开总中断
	while(1)
	{
		task_start(0);//协程0开始
		P00 = ~P00;//每次执行取反P00端口电平
		task_delay(100);//设定延时100ms
		task_wait(!P32);//P32电平作为判断条件，为1则等待，为0则继续向下执行
		//因为按键按下为0，所以正常是一直闪烁，按下P32则LED00停止闪烁
		task_end(1);//为1则循环执行
		
		task_start(1);//协程1开始
		P01 = 0;//P01端口置0，LED亮
		task_delay(200);//设定延时200ms
		P01 = 1;//P01端口置1，LED灭
		task_delay(500);//设定延时500ms
		task_end(1);//为1则循环执行
		
		task_start(2);//协程2开始
		task_delay(1000);//先长延时一段
		task_for(cnt1=0, cnt1++)//初始化和运行表达式(中间要用逗号链接!)
		{
			task_for(cnt2=0, cnt2++)//第二层for循环
			{
				P02 = ~P02;//取反P02端口
				task_delay(50);//小延时，快速闪烁
			}
			task_break(cnt2<3);//第二层for的判断条件，为1则返回for开头，为0则继续执行
			P02 = 0;//强制给P02端口置低
			task_delay(300);//设定延时300ms
		}
		task_break(cnt1<5);//第一层for的判断条件，为1则返回for开头，为0则继续执行
		task_end(1);//为1则循环运行
	}
}

