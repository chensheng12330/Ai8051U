/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_GPIO_INT.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_Delay.h"
#include "AI8051U_NVIC.h"

/*************    ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

��ʾ������ͨIO�ڶ����Բ����жϣ����ҿ��Խ�MCU�����߻��ѵĹ���.

�Ӵ����������Դ��оƬ״̬��115200,N,8,1.

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 24MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

//========================================================================
//                               ���س�������    
//========================================================================


//========================================================================
//                               ���ر�������
//========================================================================


//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern u8 code ledNum[];
extern u8 ledIndex;
extern u16 msecond;

//========================================================================
// ����: GPIO_INTtoUART_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2025-02-11
//========================================================================
void GPIO_INTtoUART_init(void)
{
    COMx_InitDefine  COMx_InitStructure;            //�ṹ����
    GPIO_Int_InitTypeDef  GPIO_Int_InitStructure;   //�ṹ����

    P0_MODE_IO_PU(GPIO_Pin_All);            //P0 ����Ϊ׼˫���
    P1_MODE_IO_PU(GPIO_Pin_All);            //P1 ����Ϊ׼˫���
    P2_MODE_IO_PU(GPIO_Pin_All);            //P2 ����Ϊ׼˫���
    P3_MODE_IO_PU(GPIO_Pin_All);            //P3 ����Ϊ׼˫���
    P4_MODE_IO_PU(GPIO_Pin_All);            //P4 ����Ϊ׼˫���
    P5_MODE_IO_PU(GPIO_Pin_All);            //P5 ����Ϊ׼˫���

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;     //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;         //ѡ�����ʷ�����, BRT_Timer1,BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 115200ul;           //������,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = DISABLE;            //���ս�ֹ,   ENABLE �� DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);         //��ʼ������  UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1); //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    //------------------------------------------------
    P0IntClrFlag();     //���жϱ�־
    P1IntClrFlag();     //���жϱ�־
    P2IntClrFlag();     //���жϱ�־
    P3IntClrFlag();     //���жϱ�־
    P4IntClrFlag();     //���жϱ�־
    P5IntClrFlag();     //���жϱ�־

    GPIO_Int_InitStructure.Mode = GPIO_FALLING_EDGE;    //IOģʽ, GPIO_FALLING_EDGE,GPIO_RISING_EDGE,GPIO_LOW_LEVEL,GPIO_HIGH_LEVEL
    GPIO_Int_InitStructure.Pin  = GPIO_Pin_All;
    GPIO_Int_InitStructure.WakeUpEn  = ENABLE;          //����ʹ��״̬, ENABLE/DISABLE
    GPIO_Int_InitStructure.Priority  = Priority_0;      //�ж����ȼ�, Priority_0,Priority_1,Priority_2,Priority_3
    GPIO_Int_InitStructure.IntEnable = ENABLE;          //�ж�ʹ��״̬, ENABLE/DISABLE
    GPIO_INT_Inilize(GPIO_P0, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P1, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P2, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P5, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7

    GPIO_Int_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_HIGH;  //P3.2~P3.7
    GPIO_INT_Inilize(GPIO_P3, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7

    GPIO_Int_InitStructure.Mode = GPIO_RISING_EDGE;     //IOģʽ, GPIO_FALLING_EDGE,GPIO_RISING_EDGE,GPIO_LOW_LEVEL,GPIO_HIGH_LEVEL
    GPIO_Int_InitStructure.Pin  = GPIO_Pin_7;
    GPIO_INT_Inilize(GPIO_P4, &GPIO_Int_InitStructure); //��ʼ��IO�ж�  GPIO_P0~GPIO_P7

//    IRC_Debounce(0x10); //����IRCʱ�Ӵ����߻��ѻָ��ȶ���Ҫ�ȴ���ʱ����
    P40 = 0;    //LED Power On
    printf("��ͨIO���ж�/���Ѳ���");
}

//========================================================================
// ����: Sample_GPIO_INTtoUART
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2020-09-24
//========================================================================
void Sample_GPIO_INTtoUART(void)
{
    //�����ָʾ����״̬
    P0 = ~ledNum[ledIndex];    //���������
    ledIndex++;
    if(ledIndex > 7)
    {
        ledIndex = 0;
    }

    //1���MCU��������״̬
    if(++msecond >= 10)
    {
        msecond = 0;    //�����

        P0 = 0xff;      //�ȹر���ʾ��ʡ��
        printf("MCU Sleep.\r\n");

        _nop_();
        _nop_();
        _nop_();
        PD = 1;         //Sleep
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        printf("MCU wakeup from P%02X.\r\n", ioIndex);
    }
}
