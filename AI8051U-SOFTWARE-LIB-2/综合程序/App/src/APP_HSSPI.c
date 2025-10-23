/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_HSSPI.h"
#include "APP_SPI_Flash.h"
#include "AI8051U_Clock.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_SPI.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    ����˵��    **************

ͨ������SPI��Flash���ж�д���ԡ�

���ڷ�ָ���FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,8,N,1. ������"���ڳ�ʼ������"���޸�.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ����������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ����������ָ��ʮ�����Ƶ�ַ������Ϊ�����ֽ�. 
    C                       --> �����ⲻ��ָ��SPI Flash������Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����ʱ, ѡ��ʱ�� 24MHz (PLL����ʱ��Ϊ12M��������12M������Ƶ��).

******************************************/

//========================================================================
//                               ���س�������    
//========================================================================

sbit SPI_CE  = P4^0;     //PIN1
sbit SPI_SO  = P4^2;     //PIN2
sbit SPI_SI  = P4^1;     //PIN5
sbit SPI_SCK = P4^3;     //PIN6

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
// ����: HSSPI_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void HSSPI_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    COMx_InitDefine COMx_InitStructure;     //�ṹ����

    //----------------------------------------------
    P4_MODE_IO_PU(GPIO_Pin_LOW);    //P40~P43 ����Ϊ׼˫���
    P4_SPEED_HIGH(GPIO_Pin_LOW);    //��ƽת���ٶȿ죨���IO�ڷ�ת�ٶȣ�
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2,P5.3 �ڲ�����ʹ��
    SPI_SW(SPI_P40_P41_P42_P43);    //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32

    SPI_SCK = 0;
    SPI_SI = 1;

    //----------------------------------------------
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    printf("��������:\r\n");
    printf("E 0x001234            --> ��������\xfd  ʮ�����Ƶ�ַ\r\n");
    printf("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�\r\n");
    printf("C                     --> �����ⲻ��ָ��SPI Flash, ����Cǿ���������.\r\n\r\n");

    //----------------------------------------------
    SPI_InitStructure.SPI_Enable    = ENABLE;               //SPI����    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = ENABLE;               //Ƭѡλ     ENABLE(����SS���Ź���), DISABLE(SSȷ�������ӻ�)
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;              //��λ����   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;      //����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;        //ʱ����λ   SPI_CPOL_High,   SPI_CPOL_Low
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;       //���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_4;          //SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(DISABLE,Priority_0);      //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    SPI_ClearFlag();   //��� SPIF��WCOL ��־

    HSPllClkConfig(MCKSEL_HIRC,PLL_96M,4);  //ϵͳʱ��ѡ��,PLLʱ��ѡ��,ʱ�ӷ�Ƶϵ��
    HSSPI_Enable();

    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  printf("δ��⵽PM25LV040/W25X40CL/W25Q80BV/W25Q128FV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            printf("��⵽PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            printf("��⵽W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            printf("��⵽W25Q80BV!\r\n");
        }
        else if(B_FlashOK == 4)
        {
            printf("��⵽W25Q128FV!\r\n");
        }
    }
    printf("������ID1 = 0x%02X",FLASH_ID1);
    printf("\r\n      ID2 = 0x%02X",FLASH_ID2);
    printf("\r\n   �豸ID = 0x%02X\r\n",FLASH_ID);
}

//========================================================================
// ����: Sample_HSSPI
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void Sample_HSSPI(void)
{
    if(COM1.RX_TimeOut)
    {
        COM1.RX_TimeOut = 0;

        if(COM1.RX_Cnt > 0)
        {
            RX1_Check();    //������������
        }
        COM1.RX_Cnt = 0;
    }
}
