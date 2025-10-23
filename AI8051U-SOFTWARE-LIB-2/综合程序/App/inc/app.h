/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_H_
#define __APP_H_

//========================================================================
//                                ͷ�ļ�
//========================================================================

#include "config.h"
#include "APP_Lamp.h"
#include "APP_AD_UART.h"
#include "APP_INT_UART.h"
#include "APP_RTC.h"
#include "APP_I2C.h"
#include "APP_SPI_Flash.h"
#include "APP_WDT.h"
#include "APP_PWM.h"
#include "APP_EEPROM.h"
#include "APP_DMA_AD.h"
#include "APP_DMA_M2M.h"
#include "APP_DMA_UART.h"
#include "APP_DMA_SPI_Flash.h"
#include "APP_DMA_LCM.h"
#include "APP_DMA_I2C.h"
#include "APP_USART_LIN.h"
#include "APP_USART2_LIN.h"
#include "APP_HSSPI.h"
#include "APP_HSPWM.h"
#include "APP_USB.h"
#include "APP_QSPI_Flash.h"
#include "APP_GPIO_INT.h"

//========================================================================
//                               ���س�������	
//========================================================================

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

extern u8 code t_display[];

extern u8 code T_COM[];      //λ��

extern u8 code T_KeyTable[16];

//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================

extern u8  LED8[8];        //��ʾ����
extern u8  display_index;  //��ʾλ����

extern u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //���м��̱���
extern u8  KeyHoldCnt; //�����¼�ʱ
extern u8  KeyCode;    //���û�ʹ�õļ���
extern u8  cnt50ms;

extern u8  hour,minute,second; //RTC����
extern u16 msecond;

void APP_config(void);
void DisplayScan(void);

#endif
