/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    ����˵��    **************

˫����ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU�յ���ͨ�����ڰ��յ�������ԭ������, Ĭ�ϲ����ʣ�115200,N,8,1.

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

    GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;  //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);  //��ʼ��
}

/***************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;                 //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer4;     //ѡ�����ʷ�����, BRT_Timer4, BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART4, &COMx_InitStructure);     //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART4_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART4_SW(UART4_SW_P02_P03);        //UART4_SW_P02_P03,UART4_SW_P52_P53
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
    EA = 1;
    
    PrintString4("AI8051U UART4 Test Programme!\r\n");    //UART4����һ���ַ���

    while (1)
    {
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
