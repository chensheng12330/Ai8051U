#include "stc32g.h"
#include "stc32_stc8_usb.h"

void usb_callback();

void main()
{
    EAXFR = 1;							//���������չ������Ĵ�����XFR
	WTST = 0;							//����ȡ�������ȴ�ʱ�䣬
										//��ֵΪ0��ʾ���ȴ�������������ٶ�����
	CKCON = 0;							//���÷���Ƭ�ڵ�xdata�ٶȣ�
										//��ֵΪ 0��ʾ������ٶȷ��ʣ�
										//�����Ӷ���ĵȴ�ʱ��    
    
    P0M1 = 0x00;   P0M0 = 0x00;
    P1M1 = 0x00;   P1M0 = 0x00;
    P2M1 = 0x00;   P2M0 = 0x00;
    P3M1 = 0x00;   P3M0 = 0x00;
    P4M1 = 0x00;   P4M0 = 0x00;
    P5M1 = 0x00;   P5M0 = 0x00;
    P6M1 = 0x00;   P6M0 = 0x00;
    P7M1 = 0x00;   P7M0 = 0x00;
    
    usb_init();                                     //USB CDC �ӿ�����
    set_usb_OUT_callback(usb_callback);             //�����жϻص��ص�����
    
    EA = 1;

    while (1);
}

void usb_callback()
{
    USB_SendData(UsbOutBuffer,OutNumber);           //�������ݻ����������ȣ���������ԭ������, ���ڲ��ԣ�
}
