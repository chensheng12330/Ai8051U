/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_CLOCK_H
#define __AI8051U_CLOCK_H

#include "config.h"

//========================================================================
//                              ʱ������
//========================================================================

#define MainClockSel(n)     CLKSEL = (CLKSEL & ~0x0f) | (n)         /* ϵͳʱ��ѡ�� */
#define PLLClockSel(n)      CLKSEL = (CLKSEL & ~0x80) | (n<<7)      /* PLLʱ��ѡ�� */
#define HSIOClockSel(n)     CLKSEL = (CLKSEL & ~0x40) | (n<<6)      /* ����IOʱ��ѡ�� */
#define PLLClockIn(n)       USBCLK = (USBCLK & ~0x60) | (n<<4)      /* ϵͳʱ�� n ��Ƶ��ΪPLLʱ��Դ,ȷ����Ƶ��Ϊ12M */
#define PLLEnable(n)        USBCLK = (USBCLK & ~0x80) | (n<<7)      /* PLL��Ƶʹ�� */
#define HSClockDiv(n)       HSCLKDIV = (n)        /* ����IOʱ�ӷ�Ƶϵ�� */

//========================================================================
//                              ��������
//========================================================================

/* ����IOʱ��ѡ����� */
#define HSCK_MCLK       0
#define HSCK_PLL        1

/* PLLʱ��ѡ����� */
#define PLL_96M         0
#define PLL_144M        1
#define PLL_SEL         PLL_96M

/* ϵͳʱ��ѡ����� */
#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

/* ϵͳʱ�� n ��Ƶ��ΪPLLʱ��Դ����,ȷ����Ƶ��Ϊ12M */
#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

//========================================================================
//                              �ⲿ����
//========================================================================

void HSPllClkConfig(u8 clksrc, u8 pllsel, u8 div);

#endif
