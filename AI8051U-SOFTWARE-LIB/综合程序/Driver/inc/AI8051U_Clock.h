/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_CLOCK_H
#define __AI8051U_CLOCK_H

#include "config.h"

//========================================================================
//                              时钟设置
//========================================================================

#define MainClockSel(n)     CLKSEL = (CLKSEL & ~0x0f) | (n)         /* 系统时钟选择 */
#define PLLClockSel(n)      CLKSEL = (CLKSEL & ~0x80) | (n<<7)      /* PLL时钟选择 */
#define HSIOClockSel(n)     CLKSEL = (CLKSEL & ~0x40) | (n<<6)      /* 高速IO时钟选择 */
#define PLLClockIn(n)       USBCLK = (USBCLK & ~0x60) | (n<<4)      /* 系统时钟 n 分频作为PLL时钟源,确保分频后为12M */
#define PLLEnable(n)        USBCLK = (USBCLK & ~0x80) | (n<<7)      /* PLL倍频使能 */
#define HSClockDiv(n)       HSCLKDIV = (n)        /* 高速IO时钟分频系数 */

//========================================================================
//                              定义声明
//========================================================================

/* 高速IO时钟选择参数 */
#define HSCK_MCLK       0
#define HSCK_PLL        1

/* PLL时钟选择参数 */
#define PLL_96M         0
#define PLL_144M        1
#define PLL_SEL         PLL_96M

/* 系统时钟选择参数 */
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

/* 系统时钟 n 分频作为PLL时钟源参数,确保分频后为12M */
#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

//========================================================================
//                              外部声明
//========================================================================

void HSPllClkConfig(u8 clksrc, u8 pllsel, u8 div);

#endif
