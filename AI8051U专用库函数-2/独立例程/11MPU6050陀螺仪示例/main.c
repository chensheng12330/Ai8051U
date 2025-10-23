#pragma maxargs (200) //��չ�䳤���������������

#include "AI8051U.H"
#include "set_io.h"
#include "set_i2c.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
ʹ��˵����
ͨ��ʵ������LCD�ӿڸ��ò���MPU6050ģ�飬ʹ��ģ���ͺ�ΪGY-521
ģ���VCC����ʵ�����ϵ�DB6��GND����DB5��SCL����DB4��SDA����DB3��XDA~INT��Ҫ������������
������ʹ�öŰ��ߵĻ��Ͳ�Ҫ�ӣ���Ȼ���USB-CDC�ӿڼ��ɹ۲쵽��·���ٶȺͽ��ٶ����ݵ����
*/

int AccX = 0, AccY = 0, AccZ = 0;
int GyroX = 0, GyroY = 0, GyroZ = 0;
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	EA = 1;//�����ж�
	P25 = 0;//GND�򿪵�Դ
	set_io_mode(pp_mode, Pin24, Pin25, Pin26, Pin_End);//����������Դ�ߺ�SCL
	set_io_mode(od_mode, Pin23, Pin_End);//����SDAΪ��©���
	set_io_mode(en_pur, Pin23, Pin_End);//����SDA���ڲ���4K���������
	set_timer_mode(Timer0, "0.2s", Timer_End);//�趨0.2s�Ķ�ʱ��
	set_i2c_mode(I2c0 ,I2c_P24_3, I2c_End);//�л����ŵ�P24��P25
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x6b, Tx_Rack, 0x01, Stop, Cmd_End);//���Ѳ����õ�Դ
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1a, Tx_Rack, 0x06, Stop, Cmd_End);//�����˲���5Hz
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1b, Tx_Rack, 0x18, Stop, Cmd_End);//����������2000dps
	set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x1c, Tx_Rack, 0x18, Stop, Cmd_End);//���ü��ٶȼ�16g
	while(1)
	{
		if(get_timer_state(Timer0))//��ʱ��0�����ж�
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