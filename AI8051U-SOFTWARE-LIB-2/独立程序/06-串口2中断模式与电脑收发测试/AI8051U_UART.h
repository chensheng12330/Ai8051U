/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_UART_H
#define __AI8051U_UART_H     

#include "config.h"

//========================================================================
//                              ��������
//========================================================================

#define UART1    1       //ʹ����Щ���ھͿ���Ӧ�Ķ��壬���õĴ��ڿ����ε����壬��ʡ��Դ
#define UART2    2
#define UART3    3
#define UART4    4

#define UART_BUF_type    xdata  //���ô����շ����ݻ���ռ䣬��ѡ edata ���� xdata

#define UART_QUEUE_MODE    0    //���ô��ڷ���ģʽ��0������ģʽ��1������ģʽ

#define PRINTF_SELECT  UART2    //ѡ�� printf ������ʹ�õĴ��ڣ����� UART1~UART4

#ifdef UART1
#define COM_TX1_Lenth    128    //���ô���1�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#define COM_RX1_Lenth    128    //���ô���1�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#endif
#ifdef UART2
#define COM_TX2_Lenth    128    //���ô���2�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#define COM_RX2_Lenth    128    //���ô���2�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#endif
#ifdef UART3
#define COM_TX3_Lenth    64     //���ô���3�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#define COM_RX3_Lenth    64     //���ô���3�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#endif
#ifdef UART4
#define COM_TX4_Lenth    64     //���ô���4�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#define COM_RX4_Lenth    64     //���ô���4�������ݻ�������С��������ȶ������256����Ҫ����������ó��ȱȽϵı���������u8��Ϊu16/u32.
#endif

#define UART_ShiftRight    0        //ͬ����λ���
#define UART_8bit_BRTx    (1<<6)    //8λ����,�ɱ䲨����
#define UART_9bit         (2<<6)    //9λ����,�̶�������
#define UART_9bit_BRTx    (3<<6)    //9λ����,�ɱ䲨����


#define BRT_Timer1         1         //�����ʷ�����ѡ��
#define BRT_Timer2         2
#define BRT_Timer3         3
#define BRT_Timer4         4

#define PARITY_NONE        0         //��У��
#define PARITY_EVEN        4         //żУ��
#define PARITY_ODD         6         //��У��

#define TO_SCALE_BRT       0         //��ʱ����ʱ��Դ����������λ��(������)
#define TO_SCALE_SYSCLK    1         //��ʱ����ʱ��Դ��ϵͳʱ��

//========================================================================
//                              UART����
//========================================================================

#define UART1_RxEnable(n)        (n==0?(REN = 0):(REN = 1))            /* UART1����ʹ�� */
#define UART2_RxEnable(n)        (n==0?(S2REN = 0):(S2REN = 1))        /* UART2����ʹ�� */
#define UART3_RxEnable(n)        (n==0?(S3REN = 0):(S3REN = 1))        /* UART3����ʹ�� */
#define UART4_RxEnable(n)        (n==0?(S4REN = 0):(S4REN = 1))        /* UART4����ʹ�� */


#define CLR_TI()             TI = 0         /* ���TI  */
#define CLR_RI()             RI = 0         /* ���RI  */
#define CLR_TI2()            S2TI = 0       /* ���TI2 */
#define CLR_RI2()            S2RI = 0       /* ���RI2 */
#define CLR_TI3()            S3TI = 0       /* ���TI3 */
#define CLR_RI3()            S3RI = 0       /* ���RI3 */
#define CLR_TI4()            S4TI = 0       /* ���TI3 */
#define CLR_RI4()            S4RI = 0       /* ���RI3 */

#define S3_8bit()            S3SM0 = 0      /* ����3ģʽ0��8λUART�������� = ��ʱ��������� / 4 */
#define S3_9bit()            S3SM0 = 1      /* ����3ģʽ1��9λUART�������� = ��ʱ��������� / 4 */
#define S3_BRT_UseTimer3()   S3ST3 = 1      /* BRT select Timer3 */
#define S3_BRT_UseTimer2()   S3ST3 = 0      /* BRT select Timer2 */

#define S4_8bit()            S4SM0 = 0      /* ����4ģʽ0��8λUART�������� = ��ʱ��������� / 4 */
#define S4_9bit()            S4SM0 = 1      /* ����4ģʽ1��9λUART�������� = ��ʱ��������� / 4 */
#define S4_BRT_UseTimer4()   S4ST4 = 1      /* BRT select Timer4 */
#define S4_BRT_UseTimer2()   S4ST4 = 0      /* BRT select Timer2 */

#define S1_CLR_TOIF()        UR1TOSR |= 0x80    /* �������1�жϳ�ʱ��־λTOIF */
#define S2_CLR_TOIF()        UR2TOSR |= 0x80    /* �������2�жϳ�ʱ��־λTOIF */
#define S3_CLR_TOIF()        UR3TOSR |= 0x80    /* �������3�жϳ�ʱ��־λTOIF */
#define S4_CLR_TOIF()        UR4TOSR |= 0x80    /* �������4�жϳ�ʱ��־λTOIF */

//========================================================================
//                              ��������
//========================================================================

typedef struct
{ 
    u8    TX_send;      //�ѷ���ָ��
    u8    TX_write;     //����дָ��
    u8    B_TX_busy;    //æ��־

    u8    RX_Cnt;       //�����ֽڼ���
    u8    RX_TimeOut;   //���ճ�ʱ
} COMx_Define; 

typedef struct
{ 
    u8    UART_Mode;        //ģʽ,        UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    u8    UART_BRT_Use;     //ʹ�ò�����,  BRT_Timer1,BRT_Timer2,BRT_Timer3,BRT_Timer4
    u32   UART_BaudRate;    //������,      һ�� 110 ~ 115200
    u8    UART_RxEnable;    //�������,   ENABLE,DISABLE
    u8    BaudRateDouble;   //�����ʼӱ�, ENABLE,DISABLE
    u8    ParityMode;       //У��ģʽ,   PARITY_NONE,PARITY_EVEN,PARITY_ODD

    u8    TimeOutEnable;    //���ճ�ʱʹ��, ENABLE,DISABLE
    u8    TimeOutINTEnable; //��ʱ�ж�ʹ��, ENABLE,DISABLE
    u8    TimeOutScale;     //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    u32   TimeOutTimer;     //��ʱʱ��, 1 ~ 0xffffff
} COMx_InitDefine; 

#ifdef UART1
extern COMx_Define    COM1;
extern u8 UART_BUF_type TX1_Buffer[COM_TX1_Lenth];    //���ͻ���
extern u8 UART_BUF_type RX1_Buffer[COM_RX1_Lenth];    //���ջ���
#endif
#ifdef UART2
extern COMx_Define    COM2;
extern u8 UART_BUF_type TX2_Buffer[COM_TX2_Lenth];    //���ͻ���
extern u8 UART_BUF_type RX2_Buffer[COM_RX2_Lenth];    //���ջ���
#endif
#ifdef UART3
extern COMx_Define    COM3;
extern u8 UART_BUF_type TX3_Buffer[COM_TX3_Lenth];    //���ͻ���
extern u8 UART_BUF_type RX3_Buffer[COM_RX3_Lenth];    //���ջ���
#endif
#ifdef UART4
extern COMx_Define    COM4;
extern u8 UART_BUF_type TX4_Buffer[COM_TX4_Lenth];    //���ͻ���
extern u8 UART_BUF_type RX4_Buffer[COM_RX4_Lenth];    //���ջ���
#endif

u8 UART_Configuration(u8 UARTx, COMx_InitDefine *COMx);
#ifdef UART1
void TX1_write2buff(u8 dat);    //����1���ͺ���
void PrintString1(u8 *puts);
#endif
#ifdef UART2
void TX2_write2buff(u8 dat);    //����2���ͺ���
void PrintString2(u8 *puts);
#endif
#ifdef UART3
void TX3_write2buff(u8 dat);    //����3���ͺ���
void PrintString3(u8 *puts);
#endif
#ifdef UART4
void TX4_write2buff(u8 dat);    //����4���ͺ���
void PrintString4(u8 *puts);
#endif

//void COMx_write2buff(u8 UARTx, u8 dat);    //���ڷ��ͺ���
//void PrintString(u8 UARTx, u8 *puts);

#endif

