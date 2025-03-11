/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef	__SPI_FLASH_H
#define	__SPI_FLASH_H

sbit    SPI_CS  = P4^0;     //PIN1
sbit    SPI_MISO = P4^2;    //PIN2
sbit    SPI_MOSI = P4^1;    //PIN5
sbit    SPI_SCK = P4^3;     //PIN6
sbit    SPI_WP = P5^2;      //PIN3
sbit    SPI_HOLD = P5^3;    //PIN7

extern u32 Flash_addr;
extern bit SpiDmaFlag;

void SPI_init(void);
void SPI_DMA_Config(void);
void FlashChipErase(void);
void SPI_DMA_Reset(void);

void SPI_P2P_Write(u32 addr);
void SPI_Read_P2P(u32 addr);

#endif
