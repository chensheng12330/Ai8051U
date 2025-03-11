/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��  **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ������"usb_hid_32.lib"���ļ�������ʵ��ʹ��USB�ӿڴ�ӡ������Ϣ�������ڳ������.

P3.2�ڰ�����ʾ"printf"�����������;

P3.3�ڰ�����ʾ"ShowLong"�����������������;

P3.4�ڰ�����ʾ"ShowFloat"�����������������;

P3.5�ڰ�����ʾ"ShowCode"�������8�ֽ���������;

��"config.h"�ļ���ѡ��"PRINTF_SEGLED"���壬printf������ݸ�ʽ�ض���ISP��������е�7�������

��"config.h"�ļ���ѡ��"PRINTF_HID"���壬printf���ֱ���ض���USB HID�ӿ�

����ʱ, ѡ��ʱ�� 24MHz (����"config.h"�ļ����޸�).

******************************************/

#include "config.h"
#include "../comm/AI8051U.h"
#include "../comm/usb.h"
#include <stdio.h>

void sys_init();

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //�����Զ���λ��ISP�����û��ӿ�����

BYTE xdata cod[8];

void main()
{
    sys_init();
    usb_init();  //USB��ʼ��
    EA = 1;
    
    while (1)
    {
        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,64);  //�������ݻ����������ȣ���������ԭ������, ���ڲ��ԣ�
            
            usb_OUT_done(); //����Ӧ�𣨹̶���ʽ��
        }
        
        if (!P32)
        {
            while (!P32);
            printf("%08lx", 0x1234abcdL);  //ʹ��printf�����ض���USB�������
        }
        else if (!P33)
        {
            while (!P33);
            SEG7_ShowLong(0x98765432, 16);  //�������ܳ���������
        }
        else if (!P34)
        {
            while (!P34);
            SEG7_ShowFloat(3.1415);  //�������ܸ���������
        }
        else if (!P35)
        {
            cod[0] = 0x3f;  //����ܶ���
            cod[1] = 0x06;
            cod[2] = 0x5b;
            cod[3] = 0x4f;
            cod[4] = 0x66;
            cod[5] = 0x6d;
            cod[6] = 0x7d;
            cod[7] = 0x27;
            while (!P35);
            SEG7_ShowCode(cod);  //����������ֵ
        }
    }
}

void sys_init()
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
}

