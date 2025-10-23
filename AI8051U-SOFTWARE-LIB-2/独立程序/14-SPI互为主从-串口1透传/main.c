/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_SPI.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    ����˵��    **************

ͨ�����ڷ������ݸ�MCU1��MCU1�����յ���������SPI���͸�MCU2��MCU2��ͨ�����ڷ��ͳ�ȥ.

���÷��� 2��
�����豸��ʼ��ʱ������ SSIG Ϊ 0��MSTR ����Ϊ0����ʱ�����豸���ǲ����� SS �Ĵӻ�ģʽ��
������һ���豸��Ҫ��������ʱ���ȼ�� SS �ܽŵĵ�ƽ�����ʱ��ߵ�ƽ��
�ͽ��Լ����óɺ��� SS ����ģʽ���Լ��� SS ������͵�ƽ�����ͶԷ��� SS �ţ����ɽ������ݴ��䡣

         MCU1                          MCU2
  |-----------------|           |-----------------|
  |            MISO |-----------| MISO            |
--| TX         MOSI |-----------| MOSI         TX |--
  |            SCLK |-----------| SCLK            |
--| RX           SS |-----------| SS           RX |--
  |-----------------|           |-----------------|


����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/

bit UartReceived=0;

/*************    ���غ�������    **************/



/*************  �ⲿ�����ͱ������� *****************/



/******************** IO������ ********************/
void GPIO_config(void)
{
    P2_MODE_IO_PU(GPIO_Pin_All);    //P2 ����Ϊ׼˫���
}

/****************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure; //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
    COMx_InitStructure.ParityMode  = PARITY_NONE;       //У��ģʽ,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (ʹ��У��λ��Ҫ����9λģʽ)
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);         //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/****************  SPI��ʼ������ *****************/
void SPI_config(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Enable    = ENABLE;           //SPI����    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = DISABLE;          //Ƭѡλ     ENABLE, DISABLE
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;          //��λ����   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave;   //����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;    //ʱ����λ   SPI_CPOL_Low,    SPI_CPOL_High
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;   //���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_4;      //SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2

    SPI_InitStructure.TimeOutEnable  = ENABLE;          //�ӻ���ʱʹ��, ENABLE,DISABLE
    SPI_InitStructure.TimeOutINTEnable  = ENABLE;       //��ʱ�ж�ʹ��, ENABLE,DISABLE
    SPI_InitStructure.TimeOutScale  = TO_SCALE_1US;     //��ʱʱ��Դѡ��, TO_SCALE_1US,TO_SCALE_SYSCLK
    SPI_InitStructure.TimeOutTimer  = 1000ul;           //��ʱʱ��, 1 ~ 0xffffff
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(ENABLE,Priority_3);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    SPI_SW(SPI_P24_P25_P26_P27);    //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32
    SPI_SS_2 = 1;
}

/**********************************************/
void main(void)
{
    u8 i;

    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

    GPIO_config();
    UART_config();
    SPI_config();
    EA = 1;

    printf("AI8051U UART1��SPI͸������\r\n");    //UART����һ���ַ���

    while (1)
    {
        if(COM1.RX_TimeOut)
        {
            COM1.RX_TimeOut = 0;
            if(COM1.RX_Cnt > 0)
            {
                UartReceived = 1;   //���ô��ڽ��ձ�־
            }
        }

        if((UartReceived) && (SPI_SS_2))
        {
            SPI_SS_2 = 0;     //���ʹӻ� SS �ܽ�
            SPI_SetMode(SPI_Mode_Master);    //SPI��������ģʽ����ʼ��������
            for(i=0;i<COM1.RX_Cnt;i++)
            {
                SPI_WriteByte(RX1_Buffer[i]); //���ʹ�������
            }
            SPI_SS_2 = 1;    //���ߴӻ��� SS �ܽ�
            SPI_SetMode(SPI_Mode_Slave);    //SPI���ôӻ�ģʽ���������״̬
            COM1.RX_Cnt = 0;
            UartReceived = 0;
        }
        
        if(SPI_RxTimerOut)
        {
            SPI_RxTimerOut = 0;

            if(SPI_RxCnt > 0)    //SPI���ճ�ʱ���ж�SPI�������ݳ����Ƿ����
            {
                for(i=0; i<SPI_RxCnt; i++)    TX1_write2buff(SPI_RxBuffer[i]);    //ͨ�������������
            }
            SPI_RxCnt = 0;    //���SPI�ѽ������ݳ���
        }
    }
}
