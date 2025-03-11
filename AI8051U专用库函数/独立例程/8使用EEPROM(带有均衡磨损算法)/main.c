#include "AI8051U.H"
#include "set_uart.h"
#include "set_eeprom.h"

/*
使用说明：
串口波特率为115200bps，通过发送任意字符，即可从串口获取当前的累计开机次数
*/
int start_cnt = 0;//开机次数存储变量
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	IAP_TPS = 40;
	P2M0 = 0x00; P2M1 = 0x00; 
	set_uart_mode(Uart1, Uart_End);
	EA = 1;//打开总中断
	set_eeprom_mode(Hex_Mode, &start_cnt, sizeof(start_cnt));//绑定变量到EEPROM
	set_eeprom_sync(Pull);//拉取变量的值到本地变量
	start_cnt++;//开机次数加1
	set_eeprom_sync(Push);//将本地变量的值更新到EEPROM
	while(1)
	{
		if(get_uart_state(Uart1))
		{
			uart_printf(Uart1, "start_cnt:%d\r\n", start_cnt);//输出开机次数
		}
	}
}