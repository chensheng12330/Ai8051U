/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_I2C.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Delay.h"
#include "AI8051U_Switch.h"

/*************	����˵��	**************

ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

���¶�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

��������UART1(P3.0,P3.1): 115200,N,8,1.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

//========================================================================
//                               ���س�������	
//========================================================================

#define SLAW        0xA0
#define SLAR        0xA1

//========================================================================
//                               ���ر�������
//========================================================================


//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: I2C_PS_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-25
//========================================================================
void I2C_24C02_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;
    COMx_InitDefine COMx_InitStructure; //�ṹ����

	P3_MODE_IO_PU(GPIO_Pin_LOW);	    //P3.0~P3.3 ����Ϊ׼˫���
	I2C_SW(I2C_P32_P33);				//I2C_P24_P23,I2C_P15_P14,I2C_P32_P33

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//����ѡ��   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C����ʹ��,   ENABLE, DISABLE
    I2C_InitStructure.I2C_MS_WDTA   = DISABLE;          //����ʹ���Զ�����,  ENABLE, DISABLE
    I2C_InitStructure.I2C_Speed     = 16;               //�����ٶ�=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
    NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);  //����ģʽ, I2C_Mode_Master, I2C_Mode_Slave; �ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1/BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

//========================================================================
// ����: Sample_I2C_PS
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-25
//========================================================================
void Sample_I2C_24C02(void)
{
    u8  i;
    u8  tmp[8];

    I2C_ReadNbyte(SLAW, 0, tmp, 8);
    printf("Read = ");      //��ӡ��ȡ��ֵ
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
        tmp[i]++;           //��ȡ��ֵ+1
    }
    printf("\r\n");

    I2C_WriteNbyte(SLAW, 0, tmp, 8);  //д���µ���ֵ
}
