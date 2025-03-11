/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_USART_LIN_H
#define __AI8051U_USART_LIN_H     

#include "config.h"

//========================================================================
//                              ��������
//========================================================================

#define USART1          1
#define USART2          2

#define FRAME_LEN       8    //���ݳ���: 8 �ֽ�

#define LinMasterMode   0
#define LinSlaveMode    1

//========================================================================
//                              USART����
//========================================================================


//========================================================================
//                              ��������
//========================================================================

typedef struct
{ 
    u8  LIN_Enable;         //LIN����ʹ��, ENABLE,DISABLE
    u8  LIN_Mode;           //LIN����ģʽ, LinMasterMode,LinSlaveMode
    u8  LIN_AutoSync;       //�Զ�ͬ��ʹ��, ENABLE,DISABLE
    u16 LIN_Baudrate;       //LIN������

    u8  TimeOutEnable;      //���ճ�ʱʹ��, ENABLE,DISABLE
    u8  TimeOutINTEnable;   //��ʱ�ж�ʹ��, ENABLE,DISABLE
    u8  TimeOutScale;       //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    u32 TimeOutTimer;       //��ʱʱ��, 1 ~ 0xffffff
} USARTx_LIN_InitDefine; 

//========================================================================
//                              �ⲿ����
//========================================================================

u8 UASRT_LIN_Configuration(u8 USARTx, USARTx_LIN_InitDefine *USART);
void UsartLinSendChecksum(u8 USARTx, u8 *dat, u8 len);
void UsartLinSendData(u8 USARTx, u8 *pdat, u8 len);
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat, u8 len);
void UsartLinSendHeader(u8 USARTx, u8 lid);

#endif
