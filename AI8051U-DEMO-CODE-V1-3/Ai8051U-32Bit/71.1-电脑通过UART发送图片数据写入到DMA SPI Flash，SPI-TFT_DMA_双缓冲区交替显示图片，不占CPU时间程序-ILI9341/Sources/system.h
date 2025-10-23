#ifndef	__SYSTEM_H
#define	__SYSTEM_H

#include "../comm/AI8051U.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc     40000000L   //定义主时钟

#define DMA_WR_LEN    256      //定义 DMA 传输字节数（SPI Flash 一次性最多写入256字节）
#define DMA_AMT_LEN   15360    //定义 DMA 传输字节数（LCD整屏数据量可被2048整除：320*240*2/15360=10）

extern bit Mode_Flag;
extern bit DmaBufferSW;
extern u16 lcdIndex;

extern u8 xdata DmaBuffer1[DMA_AMT_LEN];
extern u8 xdata DmaBuffer2[DMA_AMT_LEN];

void delay_ms(u16 ms);

#endif
