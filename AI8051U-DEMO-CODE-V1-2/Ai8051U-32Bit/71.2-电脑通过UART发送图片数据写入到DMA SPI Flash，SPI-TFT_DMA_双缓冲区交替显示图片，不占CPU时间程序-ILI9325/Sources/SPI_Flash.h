#ifndef	__SPI_FLASH_H
#define	__SPI_FLASH_H

extern u32 Flash_addr;
extern bit SpiDmaFlag;

void SPI_init(void);
void SPI_DMA_Config(void);
void FlashChipErase(void);

void SPI_Write_Nbytes(u32 addr, u16 len);
void SPI_Read_Nbytes(u32 addr, u16 len);

#endif
