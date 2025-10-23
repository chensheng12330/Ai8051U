/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    ����˵��    **************

����Ϊ˫����ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������.

����1Ĭ�ϲ����ʣ�115200,N,8,1.

����2Ĭ�ϲ����ʣ�9600,N,8,1.

����3Ĭ�ϲ����ʣ�1000000,N,8,1.

����4Ĭ�ϲ����ʣ�2000000,N,8,1.

��������ͬ�Ĵ��ڣ��ɹ��ö�ʱ��2��Ϊ�����ʷ�������

ͨ������ "AI8051U_UART.h" ͷ�ļ������ UART1~UART4 ���壬������ͬͨ���Ĵ���ͨ�š�

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/


/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/


/******************* IO���ú��� *******************/
void GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;        //�ṹ����

    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;  //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);  //��ʼ��

    GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;  //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);  //��ʼ��

    GPIO_InitStructure.Pin  = GPIO_Pin_LOW;     //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);  //��ʼ��
}

/***************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;                 //�ṹ����

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

    //��ͬ�Ĳ�������Ҫ�ظ����ã�ֻ���������ò�ͬ�Ĳ�����
//    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
    COMx_InitStructure.UART_BaudRate  = 9600ul;         //������, һ�� 110 ~ 115200
    UART_Configuration(UART2, &COMx_InitStructure);     //��ʼ������2 UART1,UART2,UART3,UART4

    COMx_InitStructure.UART_BRT_Use   = BRT_Timer3;     //ѡ�����ʷ�����, BRT_Timer3, BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 1000000ul;      //������, һ�� 110 ~ 115200
    UART_Configuration(UART3, &COMx_InitStructure);     //��ʼ������3 UART1,UART2,UART3,UART4

    COMx_InitStructure.UART_BRT_Use   = BRT_Timer4;     //ѡ�����ʷ�����, BRT_Timer4, BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 2000000ul;      //������, һ�� 110 ~ 115200
    UART_Configuration(UART4, &COMx_InitStructure);     //��ʼ������4 UART1,UART2,UART3,UART4

    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART2_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART3_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART4_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);         //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
    UART2_SW(UART2_SW_P42_P43);         //UART2_SW_P12_P13,UART2_SW_P42_P43
    UART3_SW(UART3_SW_P00_P01);         //UART3_SW_P00_P01,UART3_SW_P50_P51
    UART4_SW(UART4_SW_P02_P03);         //UART4_SW_P02_P03,UART4_SW_P52_P53
}

/***********************************************/
void main(void)
{
    u8 i;

    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

    GPIO_config();
    UART_config();
    EA = 1;

    PrintString1("AI8051U UART1 Test Programme!\r\n");    //UART1����һ���ַ���
    PrintString2("AI8051U UART2 Test Programme!\r\n");    //UART2����һ���ַ���
    PrintString3("AI8051U UART3 Test Programme!\r\n");    //UART3����һ���ַ���
    PrintString4("AI8051U UART4 Test Programme!\r\n");    //UART4����һ���ַ���

    while (1)
    {
        if(COM1.RX_TimeOut)             //���ڽ��ճ�ʱ��־
        {
            COM1.RX_TimeOut = 0;

            if(COM1.RX_Cnt > 0)
            {
                for(i=0; i<COM1.RX_Cnt; i++)    TX1_write2buff(RX1_Buffer[i]);    //�յ�������ԭ������
            }
            COM1.RX_Cnt = 0;
        }

        if(COM2.RX_TimeOut)             //���ڽ��ճ�ʱ��־
        {
            COM2.RX_TimeOut = 0;

            if(COM2.RX_Cnt > 0)
            {
                for(i=0; i<COM2.RX_Cnt; i++)    TX2_write2buff(RX2_Buffer[i]);    //�յ�������ԭ������
            }
            COM2.RX_Cnt = 0;
        }

        if(COM3.RX_TimeOut)             //���ڽ��ճ�ʱ��־
        {
            COM3.RX_TimeOut = 0;

            if(COM3.RX_Cnt > 0)
            {
                for(i=0; i<COM3.RX_Cnt; i++)    TX3_write2buff(RX3_Buffer[i]);    //�յ�������ԭ������
            }
            COM3.RX_Cnt = 0;
        }

        if(COM4.RX_TimeOut)             //���ڽ��ճ�ʱ��־
        {
            COM4.RX_TimeOut = 0;

            if(COM4.RX_Cnt > 0)
            {
                for(i=0; i<COM4.RX_Cnt; i++)    TX4_write2buff(RX4_Buffer[i]);    //�յ�������ԭ������
            }
            COM4.RX_Cnt = 0;
        }
    }
}
