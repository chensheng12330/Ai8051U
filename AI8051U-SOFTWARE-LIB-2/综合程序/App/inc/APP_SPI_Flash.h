/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_SPI_FLASH_H_
#define __APP_SPI_FLASH_H_

#include "config.h"

//========================================================================
//                              定义声明
//========================================================================

/******************* FLASH相关定义 ************************/
#define SFC_WREN        0x06        //串行Flash命令集
#define SFC_WRDI        0x04
#define SFC_RDSR        0x05
#define SFC_WRSR        0x01
#define SFC_READ        0x03
#define SFC_FASTREAD    0x0B
#define SFC_RDID        0xAB
#define SFC_PAGEPROG    0x02
#define SFC_RDCR        0xA1
#define SFC_WRCR        0xF1
#define SFC_SECTORER1   0xD7        //PM25LV040 扇区擦除指令
#define SFC_SECTORER2   0x20        //W25Xxx 扇区擦除指令
#define SFC_BLOCKER     0xD8
#define SFC_CHIPER      0xC7

//========================================================================
//                              外部声明
//========================================================================

extern u32 Flash_addr;
extern u8  B_FlashOK;
extern u8  FLASH_ID, FLASH_ID1, FLASH_ID2;

void Sample_SPI_Flash(void);
void SPI_Flash_init(void);

void RX1_Check(void);
u8  CheckData(u8 dat);
u32 GetAddress(void);
u8  GetDataLength(void);
void FlashCheckID(void);
u8 CheckFlashBusy(void);
void FlashWriteEnable(void);
void FlashSectorErase(u32 addr);
void SPI_Read_Nbytes(u32 addr, u8 *buffer, u16 size);
u8 SPI_Read_Compare(u32 addr, u8 *buffer, u16 size);
void SPI_Write_Nbytes(u32 addr, u8 *buffer, u8 size);

#endif

