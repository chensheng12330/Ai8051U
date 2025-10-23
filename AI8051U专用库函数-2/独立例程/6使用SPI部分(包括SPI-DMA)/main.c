#include "AI8051U.h"
#include "set_spi.h"  //设置SPI部分的函数，带有DMA操作
#include "set_io.h"	  //设置I/O部分的库函数
#include "set_uart.h" //设置串口的库函数，丰富的使用方法详见H文件中的详细说明
/*
使用说明：
串口1（波特率115200，P30、P31口），向串口发送任意内容，即可返回读取到的FLASHID
根据不同FLASH可能会读取到不同的值，只要是非0x00的ID，都可以认为是正确的
*/

char tx_tmp[7] = {0xab, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//需要发送的flash数据
//第一个是读取ID指令（并从掉电唤醒），后面的为触发读取所用，任意值都可以
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	// set_uart.h 函数使用例程
	set_uart_mode(Uart1, Use_Timerx, Uart_End);
	// 设置串口1为默认模式，115200bps，64byte停止中断，默认timer2，p30p31

	// set_io.h 函数使用例程
	set_io_mode(pp_mode, Pin40, Pin41, Pin43, Pin52, Pin53, Pin_End); 
	// 设置P40,P41,P43,P52,P53为准双向,设置IC所使用的所有引脚，MISO(P42)不设置，为默认高阻
	set_io_mode(en_pur, Pin42, Pin_End); // 设置P42内部的上拉电阻打开，增强抗干扰

	// set_spi.h 函数使用例程
	set_spi_mode(SPI0, Spi_P40_1_2_3, Spi_End);//全部设置默认参数，切换SPI的IO到P40_1_2_3
	EA = 1;										 // 打开总中断
	while (1)
	{
		if (get_uart_state(Uart1)) // 检测串口1是否有数据
		{
			//spi_printf(SPI2, Hex_Mode, 0x0f);//使用Hex模式的示例
			P40 = 0;//CS引脚拉低，开始操作
			spi_printf(SPI0, Buff_Mode, tx_tmp, 7);//使用Buff模式的示例
		}
		if(get_spi_state(SPI0))//操作完成
		{
			P40 = 1;//CS引脚拉高，停止操作
			uart_printf(Uart1, "flash_id:0x%02bx,0x%02bx,0x%02bx\r\n",
				(char)_spi0_rx_buff[4],(char)_spi0_rx_buff[5],(char)_spi0_rx_buff[6]);
		}
	}
}