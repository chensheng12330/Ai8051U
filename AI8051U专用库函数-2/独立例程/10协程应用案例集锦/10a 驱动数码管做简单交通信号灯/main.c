#include "AI8051U.H"
#include "set_io.h"
#include "set_spi.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
使用说明：
通过实验箱上的数码管可以显示一个简单的红绿灯倒计时，底下的led作为红绿灯指示，黄灯会闪烁
使用了三个线程，线程0用于刷新数码管显示，线程1是红绿灯1的执行线程，线程2是红绿灯2的执行线程
*/

//宏定义红绿灯端口引脚
//第一组红绿灯
#define RED1 P00
#define YELLOW1 P01
#define GREEN1 P02
//第二组红绿灯
#define RED2 P05
#define YELLOW2 P06
#define GREEN2 P07
//暂且按照10s红灯，7s绿灯，3s黄灯来循环执行

//74HC595控制引脚
#define RCK P35 //刷新刚才的数据
char show_buff[8] = {0, 0, 17, 17, 17, 17, 0, 0};//显示数码管数据缓冲区,中间消隐显示
char _show_buff[2] = {0};//实际刷新所用缓冲区
char code seg_data[19] = {
	0x3F,       /*'0', 0*/
    0x06,       /*'1', 1*/
    0x5B,       /*'2', 2*/
    0x4F,       /*'3', 3*/
    0x66,       /*'4', 4*/
    0x6D,       /*'5', 5*/
    0x7D,       /*'6', 6*/
    0x07,       /*'7', 7*/
    0x7F,       /*'8', 8*/
    0x6F,       /*'9', 9*/
    0x77,       /*'A', 10*/
    0x7C,       /*'B', 11*/
    0x39,       /*'C', 12*/
    0x5E,       /*'D', 13*/
    0x79,       /*'E', 14*/
    0x71,       /*'F', 15*/
    0x40,       /*'-', 16*/
    0x00,       /*' ', 17*/
    0x80,       /*'.', 18*/};

char seg_num = 0;//动态刷新用到的临时变量
char traffic_lights1 = 0, traffic_lights2 = 0;//交通灯控制变量
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "1ms", Timer_End);//设置定时器0定时时间
	set_timer_isr(Timer0, set_task_mode);//设置定时器中断为任务调度
	set_io_mode(pu_mode, Pin40, Pin00, Pin01, Pin02, Pin05, Pin06, Pin07, Pin_End);
	//设置LED部分引脚为准双向模式
	set_io_mode(pu_mode, Pin32, Pin34, Pin35, Pin_End);//设置74HC595部分引脚为准双向模式
	P40 = 0;//打开LED部分电源
	set_spi_mode(SPI0, Spi_P35_4_3_2, Spi_End);//使用SPI来驱动74HC595
	EA = 1;//打开总中断
	while(1)
	{
		task_start(0);//线程0，用于刷新显示
		task_for(seg_num = 0, seg_num++)
		{
			_show_buff[0] = seg_data[show_buff[seg_num]];//段码刷新，1有效
			_show_buff[1] = ~(1<<seg_num);//位选,0有效
			task_delay(1);//延时一下，防止刷新过快太暗淡
			spi_printf(SPI0, Buff_Mode, _show_buff, 2);
			task_wait(!get_spi_state(SPI0));//等待SPI传输完成
			RCK = 1;RCK = 0;//触发刷新
		}
		task_break(seg_num < 8);
		task_end(1);//线程0结束，循环执行

		task_start(1);//用于控制红路灯1
		//红绿灯1：10s红灯，7s绿灯，3s黄灯
		YELLOW1 = 1;RED1 = 0;//关闭黄灯，打开红灯
		task_for(traffic_lights1 = 10, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//遇0消隐
			show_buff[1] = traffic_lights1%10;
			task_delay(1000);
		}
		task_break(traffic_lights1>0);
		RED1 = 1;GREEN1 = 0;//关闭红灯，打开绿灯
		task_for(traffic_lights1 = 7, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//遇0消隐
			show_buff[1] = traffic_lights1%10;
			task_delay(1000);
		}
		task_break(traffic_lights1>0);
		GREEN1 = 1;YELLOW1 = 0;//关闭绿灯，打开黄灯
		task_for(traffic_lights1 = 3, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//遇0消隐
			show_buff[1] = traffic_lights1%10;
			task_delay(500);
			YELLOW1 = 1;
			task_delay(500);
			YELLOW1 = 0;//闪烁黄灯
		}
		task_break(traffic_lights1>0);
		task_end(1);//线程1结束，循环执行

		task_start(2);//用于控制红路灯2
		//红绿灯2：7s绿灯，3s黄灯，10s红灯
		RED2 = 1;GREEN2 = 0;// 关闭红灯，打开绿灯
		task_for(traffic_lights2 = 7, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//遇0消隐
			show_buff[7] = traffic_lights2%10;
			task_delay(1000);
		}
		task_break(traffic_lights2>0);
		GREEN2 = 1;YELLOW2 = 0;// 关闭绿灯，打开黄灯
		task_for(traffic_lights2 = 3, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//遇0消隐
			show_buff[7] = traffic_lights2%10;
			task_delay(500);
			YELLOW2 = 1;
			task_delay(500);
			YELLOW2 = 0;//闪烁黄灯
		}
		task_break(traffic_lights2>0);
		YELLOW2 = 1;RED2 = 0;// 关闭黄灯，打开红灯
		task_for(traffic_lights2 = 10, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//遇0消隐
			show_buff[7] = traffic_lights2%10;
			task_delay(1000);
		}
		task_break(traffic_lights2>0);
		task_end(1);//线程1结束，循环执行
	}
}