#include "AI8051U.H"
#include "stdio.h"
#include "string.h"
#include "set_eeprom.h"
#include "ai_usb.h"

/*
ʹ��˵����
ʹ��USB-CDC������CDC�ӿڣ���USB-CDC�˿ڣ�ͨ������set:���֣����ɱ���һ�����ֵ�EEPROM��
ͨ������get�����ɴ�USB-CDC�˿ڶ����ղű�������֣��ϵ����ϵ���Ȼ���Զ���
*/
int start_cnt = 0;//���������洢����
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	
	//set_eeprom.hʾ��
	set_eeprom_mode(Hex_Mode, &start_cnt, sizeof(start_cnt));//�󶨱�����EEPROM
	EA = 1;//�����ж�
	while(1)
	{
		if(bUsbOutReady)
		{
			if(strncmp(UsbOutBuffer, "get", 3) == 0)//�ж��Ƕ�ȡ
			{
				set_eeprom_sync(Pull);//��ȡ������ֵ�����ر���
				printf_usb("�ϴα����ֵ�ǣ�%d\r\n", start_cnt);//����ϴα����ֵ
			}
			if(strncmp(UsbOutBuffer, "set", 3) == 0)//�ж���д��
			{
				sscanf(UsbOutBuffer, "set:%d", &start_cnt);
				set_eeprom_sync(Push);//�����ر�����ֵ���µ�EEPROM
				printf_usb("�ѱ��� %d ��EEPROM��\r\n", start_cnt);//����ϴα����ֵ
			}
			usb_OUT_done();
		}
	}
}
