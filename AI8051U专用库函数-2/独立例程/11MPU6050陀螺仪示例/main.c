#pragma maxargs (200) //拓展变长数组参数解析长度

#include "AI8051U.H"
#include "set_io.h"
#include "set_i2c.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
使用说明：
通过实验箱上LCD接口复用插入MPU6050模块，使用模块型号为GY-521
模块的VCC连接实验箱上的DB6，GND连接DB5，SCL连接DB4，SDA连接DB3，XDA~INT不要焊接排针连接
（或者使用杜邦线的话就不要接），然后打开USB-CDC接口即可观察到三路加速度和角速度数据的输出
*/

int AccX = 0, AccY = 0, AccZ = 0;
int GyroX = 0, GyroY = 0, GyroZ = 0;
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	EA = 1;//打开总中断
	P25 = 0;//GND打开电源
	set_io_mode(pp_mode, Pin24, Pin25, Pin26, Pin_End);//设置两个电源线和SCL
	set_io_mode(od_mode, Pin23, Pin_End);//设置SDA为开漏输出
	set_io_mode(en_pur, Pin23, Pin_End);//设置SDA脚内部的4K上拉电阻打开
	set_timer_mode(Timer0, "0.2s", Timer_End);//设定0.2s的定时器
	set_i2c_mode(I2c0 ,I2c_P24_3, I2c_End);//切换引脚到P24、P25
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x6b, Tx_Rack, 0x01, Stop, Cmd_End);//唤醒并设置电源
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1a, Tx_Rack, 0x06, Stop, Cmd_End);//配置滤波器5Hz
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1b, Tx_Rack, 0x18, Stop, Cmd_End);//配置陀螺仪2000dps
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1c, Tx_Rack, 0x18, Stop, Cmd_End);//配置加速度计16g
	while(1)
	{
		if(get_timer_state(Timer0))//定时器0产生中断
		{
			set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x43, 
				S_Tx_Rack, 0xd1, Rx_Tack, (char *)&GyroX, Rx_Tack, ((char *)(&GyroX))+1, 
				Rx_Tack, (char *)&GyroY, Rx_Tack, ((char *)(&GyroY))+1,
				Rx_Tack, (char *)&GyroZ, Rx_Tnak, ((char *)(&GyroZ))+1, Stop, Cmd_End);
			set_i2c_cmd(I2c0, 1, S_Tx_Rack, 0xd0, Tx_Rack, 0x3b, 
				S_Tx_Rack, 0xd1, Rx_Tack, (char *)&AccX, Rx_Tack, ((char *)(&AccX))+1, 
				Rx_Tack, (char *)&AccY, Rx_Tack, ((char *)(&AccY))+1,
				Rx_Tack, (char *)&AccZ, Rx_Tnak, ((char *)(&AccZ))+1, Stop, Cmd_End);
		}
		if(get_i2c_state(I2c0, 0))
		{
			printf_usb("gx:%d,gy:%d,gz:%d,",GyroX, GyroY, GyroZ);
			
		}
		if(get_i2c_state(I2c0, 1))
		{
			printf_usb("ax%d,ay:%d,az:%d\r\n",AccX, AccY, AccZ);
		}
	}
}