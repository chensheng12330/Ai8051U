#ifndef __W25Q128FV_H__
#define __W25Q128FV_H__

void  	SPI_Config(u8 SPI_io, u8 SPI_speed);
void	FlashCheckID(void);
u8		FlashCheckBusy(void);
void	FlashChipErase(void);
void	FlashSectorErase(u32 addr, u8 sec);
void	FlashRead_Nbytes(u32 addr, u8 *buffer, u16 size);
u8		FlashReadCompare(u32 addr, u8 *buffer, u16 size);
void	FlashWrite_Nbytes(u32 addr, u8 *buffer, u16 size);
void	SPI_DMA_RxTRIG(u32 addr, u8 *buffer, u16 size);
void	SPI_DMA_TxTRIG(u32 addr, u8 *buffer, u16 size);

extern	bit	B_SPI_DMA_busy;
extern	bit	B_FlashOK;
extern	u8	FLASH_ID;

#endif
