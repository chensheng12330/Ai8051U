#ifndef __SPI_H__
#define __SPI_H__

#include "system.h"

#define U8g2_SPI_MODE   1    /* SPI 模式使能 */
#define SPI_DMA_CFG     1    /* SPI DMA 使能 */

sbit    SPI_CS  = P3^5;     //PIN1
sbit    SPI_MISO = P3^4;    //PIN2
sbit    SPI_MOSI = P3^3;    //PIN5
sbit    SPI_SCK = P3^2;     //PIN6

#if (U8g2_SPI_MODE == 1)
sbit    OLED_DC = P1^1;
sbit    OLED_REST = P4^7;
#endif

extern void HAL_SPI_Init();
extern void HAL_SPI_Transmit(uint8_t *Mem, uint16_t MemSize, uint32_t Timeout);
#endif /*__SPI_H__*/

