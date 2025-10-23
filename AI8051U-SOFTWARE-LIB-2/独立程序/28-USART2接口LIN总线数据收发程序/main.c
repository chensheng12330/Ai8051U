/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "ai_usb.h"
#include "AI8051U_USART_LIN.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Delay.h"
#include "AI8051U_Switch.h"

/*************   ����˵��   ***************

Lin����ģʽ�շ����ԣ�
��һ��P32�ڰ���, ������������һ֡����.
��һ��P33�ڰ���, ��������֡ͷ����ȡ�ӻ�Ӧ�����ݣ��ϲ���һ��������֡����USB-CDC���ڴ�ӡӦ������.

Lin�ӻ�ģʽ�շ����ԣ�
�յ�һ���Ǳ���Ӧ�������֡��ͨ��USB-CDC�������.
�յ�һ������Ӧ���֡ͷ��(���磺ID=0x12), ���ͻ������ݽ���Ӧ��.

Ĭ�ϴ������ʣ�9600������, �û��������޸�.

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

sbit SLP_N  = P5^0;     //0: Sleep

/*************	���س�������	**************/

#define	LIN_MASTER_MODE		1    //0: �ӻ�ģʽ; 1: ����ģʽ

/*************	���ر�������	**************/

bit Key1_Flag;
bit Key2_Flag;

u8 Key1_cnt;
u8 Key2_cnt;

u8 U2Lin_ID;
u8 USART2_BUF[8];

/*************	���غ�������	**************/


/*************  �ⲿ�����ͱ������� *****************/

extern bit B_ULinRX2_Flag;

/******************** IO������ ********************/
void GPIO_config(void)
{
	P4_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3); //P4.2,P4.3 ����Ϊ׼˫���
	P5_MODE_IO_PU(GPIO_Pin_0);		//P5.0 ����Ϊ׼˫���
    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2,P3.3 ʹ���ڲ�����
}

/******************** LIN ���� ********************/
void LIN_config(void)
{
	USARTx_LIN_InitDefine LIN_InitStructure;            //�ṹ����

#if(LIN_MASTER_MODE==1)
	LIN_InitStructure.LIN_Mode = LinMasterMode;         //LIN����ģʽ  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = DISABLE;           //�Զ�ͬ��ʹ��  	ENABLE,DISABLE
#else
	LIN_InitStructure.LIN_Mode = LinSlaveMode;          //LIN����ģʽ  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = ENABLE;            //�Զ�ͬ��ʹ��  	ENABLE,DISABLE
#endif
	LIN_InitStructure.LIN_Enable   = ENABLE;		    //LIN����ʹ��  	ENABLE,DISABLE
	LIN_InitStructure.LIN_Baudrate = 9600;			    //LIN������
    LIN_InitStructure.TimeOutEnable  = ENABLE;          //���ճ�ʱʹ��, ENABLE,DISABLE
    LIN_InitStructure.TimeOutINTEnable  = ENABLE;       //��ʱ�ж�ʹ��, ENABLE,DISABLE
    LIN_InitStructure.TimeOutScale  = TO_SCALE_BRT;     //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    LIN_InitStructure.TimeOutTimer  = 32ul;             //��ʱʱ��, 1 ~ 0xffffff
	UASRT_LIN_Configuration(USART2,&LIN_InitStructure); //LIN ��ʼ��

	NVIC_UART2_Init(ENABLE,Priority_1);     //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    UART2_SW(UART2_SW_P42_P43);     //UART2_SW_P12_P13,UART2_SW_P42_P43
}

/**********************************************/
void main(void)
{
	u8 i;

	WTST = 0;		//���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
	EAXSFR();		//��չSFR(XFR)����ʹ�� 
	CKCON = 0;      //��߷���XRAM�ٶ�

	GPIO_config();
	LIN_config();
    usb_init();     //USB CDC �ӿ�����
	EA = 1;
	//====��ʼ������=====
	SLP_N = 1;
	U2Lin_ID = 0x32;
	USART2_BUF[0] = 0x81;
	USART2_BUF[1] = 0x22;
	USART2_BUF[2] = 0x33;
	USART2_BUF[3] = 0x44;
	USART2_BUF[4] = 0x55;
	USART2_BUF[5] = 0x66;
	USART2_BUF[6] = 0x77;
	USART2_BUF[7] = 0x88;

	while (1)
	{
		delay_ms(1);
#if(LIN_MASTER_MODE==1)
		if(!P32)
		{
			if(!Key1_Flag)
			{
				Key1_cnt++;
				if(Key1_cnt > 50)
				{
					Key1_Flag = 1;
					UsartLinSendFrame(USART2,U2Lin_ID, USART2_BUF, FRAME_LEN);  //����һ����������
				}
			}
		}
		else
		{
			Key1_cnt = 0;
			Key1_Flag = 0;
		}

		if(!P33)
		{
			if(!Key2_Flag)
			{
				Key2_cnt++;
				if(Key2_cnt > 50)
				{
					Key2_Flag = 1;
					UsartLinSendHeader(USART2,0x13);  //����֡ͷ����ȡ����֡�����һ��������֡
				}
			}
		}
		else
		{
			Key2_cnt = 0;
			Key2_Flag = 0;
		}
#else
		if((B_ULinRX2_Flag) && (COM2.RX_Cnt >= 2))
		{
			B_ULinRX2_Flag = 0;

			if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
			{
				UsartLinSendData(USART2,USART2_BUF,FRAME_LEN);
				UsartLinSendChecksum(USART2,USART2_BUF,FRAME_LEN);
			}
		}
#endif

        if(COM2.RX_TimeOut)     //��ʱ����
        {
            COM2.RX_TimeOut = 0;

            printf_usb("Read Cnt = %d.\r\n",COM2.RX_Cnt);
            for(i=0; i<COM2.RX_Cnt; i++)    printf_usb("0x%02x ",RX2_Buffer[i]);    //�Ӵ�������յ��Ĵӻ�����
            COM2.RX_Cnt  = 0;   //����ֽ���
            printf_usb("\r\n");
		}

        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,OutNumber);   //�������ݻ����������ȣ���������ԭ������, ���ڲ��ԣ�
            
            usb_OUT_done();
        }
	}
}
