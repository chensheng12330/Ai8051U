#include "AI8051U.H"
#include "set_uart.h"
#include "set_eeprom.h"

/*
ʹ��˵����
���ڲ�����Ϊ115200bps��ͨ�����������ַ������ɴӴ��ڻ�ȡ��ǰ���ۼƿ�������
*/
int start_cnt = 0;//���������洢����
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	IAP_TPS = 40;
	P2M0 = 0x00; P2M1 = 0x00; 
	set_uart_mode(Uart1, Uart_End);
	EA = 1;//�����ж�
	set_eeprom_mode(Hex_Mode, &start_cnt, sizeof(start_cnt));//�󶨱�����EEPROM
	set_eeprom_sync(Pull);//��ȡ������ֵ�����ر���
	start_cnt++;//����������1
	set_eeprom_sync(Push);//�����ر�����ֵ���µ�EEPROM
	while(1)
	{
		if(get_uart_state(Uart1))
		{
			uart_printf(Uart1, "start_cnt:%d\r\n", start_cnt);//�����������
		}
	}
}