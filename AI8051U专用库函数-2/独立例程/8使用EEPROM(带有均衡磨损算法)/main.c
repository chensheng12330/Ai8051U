#include "AI8051U.H"
#include "stdio.h"
#include "string.h"
#include "set_eeprom.h"
#include "ai_usb.h"

/*
使用说明：
使用USB-CDC库生成CDC接口，打开USB-CDC端口，通过发送set:数字，即可保存一个数字到EEPROM中
通过发送get，即可从USB-CDC端口读到刚才保存的数字，断电再上电依然可以读到
*/
int start_cnt = 0;//开机次数存储变量
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	
	//set_eeprom.h示例
	set_eeprom_mode(Hex_Mode, &start_cnt, sizeof(start_cnt));//绑定变量到EEPROM
	EA = 1;//打开总中断
	while(1)
	{
		if(bUsbOutReady)
		{
			if(strncmp(UsbOutBuffer, "get", 3) == 0)//判断是读取
			{
				set_eeprom_sync(Pull);//拉取变量的值到本地变量
				printf_usb("上次保存的值是：%d\r\n", start_cnt);//输出上次保存的值
			}
			if(strncmp(UsbOutBuffer, "set", 3) == 0)//判断是写入
			{
				sscanf(UsbOutBuffer, "set:%d", &start_cnt);
				set_eeprom_sync(Push);//将本地变量的值更新到EEPROM
				printf_usb("已保存 %d 到EEPROM中\r\n", start_cnt);//输出上次保存的值
			}
			usb_OUT_done();
		}
	}
}
