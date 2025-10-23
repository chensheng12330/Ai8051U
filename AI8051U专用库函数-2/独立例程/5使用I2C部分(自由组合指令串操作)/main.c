#include "AI8051U.h"
#include "set_i2c.h"  //设置I2C的库函数，支持快捷指令集合操作
#include "set_io.h"	  //设置I/O部分的库函数
#include "set_uart.h" //设置串口的库函数，丰富的使用方法详见H文件中的详细说明
#include "stdio.h"	  //使用sscanf必须引用的库
#include "string.h"	  //使用strncmp必须引用的库
/*
使用说明：
串口1（波特率115200，P30、P31口），向串口发送"get"，即可返回一个数字。
使用"set:1"(1可以替换为0~255内的任意整数)可以设置这个数字
这个数字使用试验箱上的AT24C02进行存储，断电也不会丢失。
*/

int num = 0;
char c = 0;

void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	// set_uart.h 函数使用例程
	set_uart_mode(Uart1, Uart_End);
	// 设置串口1为默认模式，115200bps，64byte停止中断，默认timer2，p30p31

	// set_i2c.h 函数使用例程
	set_i2c_mode(I2c0, "50khz", I2c_P32_3, I2c_End);
	// 使用50khz的通讯速度,默认主机模式，引脚切换为I2c_P32_3

	// set_io.h 函数使用例程
	set_io_mode(pu_mode, Pin32, Pin33, Pin_End); // 设置P24,P23为准双向
	set_io_mode(en_pur, Pin32, Pin33, Pin_End);	 // 设置P24,P23的内部上拉电阻打开
	EA = 1;										 // 打开总中断
	while (1)
	{
		if (get_uart_state(Uart1)) // 检测串口1是否有数据
		{
			if (strncmp(_uart1_rx_buff, "get", 3) == 0)					  // 检测串口1是否发送了get字符,strncmp匹配成功返回0，其他结果返回正数或负数
				set_i2c_cmd(I2c0, 1, S_Tx_Rack, 0xa0, Tx_Rack, 0x00,	  // 器件地址0xa0，写地址00
							S_Tx_Rack, 0xa1, Rx_Tnak, &c, Stop, Cmd_End); // 重新开始后器件地址0xa1，读数据
			if (sscanf(_uart1_rx_buff, "set:%d", &num) == 1)			  // 检测串口1是发送了set:数字
				set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xa0, Tx_Rack, 0x00,	  // 发送i2c命令，设置任务序号为0,器件地址0xa0
							Tx_Rack, num, Stop, Cmd_End);				  // 随机写，地址0写入num
		}
		if (get_i2c_state(I2c0, 0))
			uart_printf(Uart1, "set_yes\r\n"); // 检测i2c的任务0是否完成,完成回传yes
		if (get_i2c_state(I2c0, 1))
			uart_printf(Uart1, "num:%d\r\n", (int)c); // 检测i2c的任务1是否完成,完成后回传num
	}
}