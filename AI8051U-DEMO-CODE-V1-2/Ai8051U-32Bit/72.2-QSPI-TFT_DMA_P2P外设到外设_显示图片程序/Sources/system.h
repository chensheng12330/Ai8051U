/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef	__SYSTEM_H
#define	__SYSTEM_H

#include "AI8051U.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

typedef 	unsigned char	BYTE;
typedef 	unsigned int	WORD;
typedef 	unsigned long	DWORD;

#define MAIN_Fosc     40000000L   //定义主时钟

#define DMA_WR_LEN    32      //定义 DMA 传输字节数（SPI Flash 一次性最多写入256字节）
#define DMA_AMT_LEN   51200    //定义 DMA 传输字节数（LCD整屏数据量可被2048整除：320*240*2/51200=3）

#define     P2P_SRC_UR1RX           0x10
#define     P2P_SRC_UR2RX           0x20
#define     P2P_SRC_UR3RX           0x30
#define     P2P_SRC_UR4RX           0x40
#define     P2P_SRC_SPIRX           0x50
#define     P2P_SRC_I2CRX           0x60
#define     P2P_SRC_LCMRX           0x70
#define     P2P_SRC_QSPIRX          0x80
#define     P2P_SRC_ADCRX           0x90
#define     P2P_SRC_I2SRX           0xA0

#define     P2P_DEST_UR1TX          0x01
#define     P2P_DEST_UR2TX          0x02
#define     P2P_DEST_UR3TX          0x03
#define     P2P_DEST_UR4TX          0x04
#define     P2P_DEST_SPITX          0x05
#define     P2P_DEST_I2CTX          0x06
#define     P2P_DEST_LCMTX          0x07
#define     P2P_DEST_QSPITX         0x08
#define     P2P_DEST_I2STX          0x0A

extern bit Mode_Flag;
extern u16 lcdIndex;
extern u16 spiIndex;

//extern u8 xdata DmaBuffer1[DMA_AMT_LEN];
//extern u8 xdata DmaBuffer2[DMA_AMT_LEN];

void delay_ms(u16 ms);

#endif
