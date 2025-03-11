/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_USART2_LIN.h"
#include "AI8051U_USART_LIN.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************   ����˵��   ***************

Lin����ģʽ�շ����ԣ�
��һ��P32�ڰ���, ������������һ֡����.
��һ��P33�ڰ���, ��������֡ͷ����ȡ�ӻ�Ӧ�����ݣ��ϲ���һ��������֡��.

Lin�ӻ�ģʽ�շ����ԣ�
�յ�һ���Ǳ���Ӧ�������֡��ͨ������1���.
�յ�һ������Ӧ���֡ͷ��(���磺ID=0x12), ���ͻ������ݽ���Ӧ��.
��Ҫ�޸�ͷ�ļ� "AI8051U_UART.h" ��Ķ��� "#define    PRINTF_SELECT  UART1"��ͨ������1��ӡ��Ϣ

Ĭ�ϴ������ʣ�9600������, �û��������޸�.

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

sbit SLP_N  = P5^0;     //0: Sleep

//========================================================================
//                               ���س�������    
//========================================================================

#define USART2_LIN_MASTER_MODE     1    //0: �ӻ�ģʽ; 1: ����ģʽ

//========================================================================
//                               ���ر�������
//========================================================================

u8 U2Lin_ID;
u8 USART2_BUF[8];

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern bit B_ULinRX2_Flag;

extern u8 Key1_cnt;
extern u8 Key2_cnt;
extern bit Key1_Flag;
extern bit Key2_Flag;

//========================================================================
// ����: USART2_LIN_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void USART2_LIN_init(void)
{
    USARTx_LIN_InitDefine LIN_InitStructure;    //�ṹ����
    COMx_InitDefine COMx_InitStructure;         //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE �� DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

#if(USART2_LIN_MASTER_MODE==1)
    LIN_InitStructure.LIN_Mode = LinMasterMode;         //LIN����ģʽ   LinMasterMode,LinSlaveMode
    LIN_InitStructure.LIN_AutoSync = DISABLE;           //�Զ�ͬ��ʹ��  ENABLE,DISABLE
#else
    LIN_InitStructure.LIN_Mode = LinSlaveMode;          //LIN����ģʽ   LinMasterMode,LinSlaveMode
    LIN_InitStructure.LIN_AutoSync = ENABLE;            //�Զ�ͬ��ʹ��  ENABLE,DISABLE
#endif
    LIN_InitStructure.LIN_Enable   = ENABLE;            //LIN����ʹ��   ENABLE,DISABLE
    LIN_InitStructure.LIN_Baudrate = 9600;              //LIN������
    LIN_InitStructure.TimeOutEnable  = ENABLE;          //���ճ�ʱʹ��, ENABLE,DISABLE
    LIN_InitStructure.TimeOutINTEnable  = ENABLE;       //��ʱ�ж�ʹ��, ENABLE,DISABLE
    LIN_InitStructure.TimeOutScale  = TO_SCALE_BRT;     //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    LIN_InitStructure.TimeOutTimer  = 32ul;             //��ʱʱ��, 1 ~ 0xffffff
    UASRT_LIN_Configuration(USART2,&LIN_InitStructure); //LIN ��ʼ��

    NVIC_UART2_Init(ENABLE,Priority_1);         //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);     //P3.0,P3.1 ����Ϊ׼˫���
    P4_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);     //P4.2,P4.3 ����Ϊ׼˫���
    P5_MODE_IO_PU(GPIO_Pin_0);      //P5.0 ����Ϊ׼˫���
    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2,P3.3 ʹ���ڲ�����
    
    UART1_SW(UART1_SW_P30_P31);     //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
    UART2_SW(UART2_SW_P42_P43);     //UART2_SW_P12_P13,UART2_SW_P42_P43

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
}

//========================================================================
// ����: Sample_USART2_LIN
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2022-03-27
//========================================================================
void Sample_USART2_LIN(void)
{
    u8 i;

#if(USART2_LIN_MASTER_MODE==1)
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

        printf("Read Cnt = %d.\r\n",COM2.RX_Cnt);
        for(i=0; i<COM2.RX_Cnt; i++)    printf("0x%02x ",RX2_Buffer[i]);    //�Ӵ�������յ��Ĵӻ�����
        COM2.RX_Cnt  = 0;   //����ֽ���
        printf("\r\n");
    }
}
