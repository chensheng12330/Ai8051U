#include "AI8051U.H"
#include "set_io.h"
#include "set_spi.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"
#pragma maxargs (200)
/*
使用说明：
通过实验箱上的数码管可以显示一个简单的红绿灯倒计时，底下的led作为红绿灯指示，黄灯会闪烁
使用了三个线程，线程0用于刷新数码管显示，线程1是红绿灯1的执行线程，线程2是红绿灯2的执行线程
*/


//74HC595控制引脚
#define RCK P35 //刷新刚才的数据
char show_buff[8] = {17, 17, 17, 17, 17, 17, 0, 0};//显示数码管数据缓冲区,左侧消隐显示
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
unsigned char key_scanf = 0;//按键扫描变量
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "1ms", Timer_End);//设置定时器0定时时间
	set_timer_isr(Timer0, set_task_mode);//设置定时器中断为任务调度
	set_io_mode(pu_mode, Pin00, Pin01, Pin02, Pin03, Pin06, Pin07, Pin_End);
	set_io_mode(en_pur, Pin00, Pin01, Pin02, Pin03, Pin_End);
	//设置按键部分引脚为准双向模式,并且打开内部上拉电阻
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
		
		task_start(1);//线程1，用于扫描按键
		P06 = 0;P07 = 1;//先扫描0~3按键
		task_delay(1);//等待电平稳定
		if((P0&0x0f) != 0x0f)//判断有按键按下
		{
			task_delay(2);//延时2ms,按键消抖
			if((P0&0x0f) != 0x0f)//判断有按键按下
			{
				key_scanf = P0&0x0f;//记录按键值
			}
		}
		P06 = 1;P07 = 0;//再扫描4~7按键
		task_delay(1);//等待电平稳定
		if((P0&0x0f) != 0x0f)//判断有按键按下
		{
			task_delay(2);//延时2ms,按键消抖
			if((P0&0x0f) != 0x0f)//判断有按键按下
			{
				key_scanf = P0&0x0f|0x10;//记录按键值，添加标记
			}
		}
		task_end(1);//线程1结束，循环执行

		task_start(2);//线程2，用于显示键值
		show_buff[6] = key_scanf/16;//显示键值
		show_buff[7] = key_scanf%16;//显示键值
		task_delay(100);//延时100ms刷新一次
		task_end(1);//线程2结束，循环执行
	}
}