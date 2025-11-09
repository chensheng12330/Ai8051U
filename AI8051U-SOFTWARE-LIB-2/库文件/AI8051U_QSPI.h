/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef	__AI8051U_QSPI_H
#define	__AI8051U_QSPI_H

#include "config.h"

//========================================================================
//                              QSPI设置
//========================================================================

#define QSPI_Enable()               QSPI_CR1 |= 0x01
#define QSPI_Disable()              QSPI_CR1 &= ~0x01
#define QSPI_Abort()                QSPI_CR1 |= 0x02

#define QSPI_SetFIFOLevel(n)        QSPI_CR2 = (n) & 0x1f

#define QSPI_PollingMatchAND()      QSPI_CR3 &= ~0x80
#define QSPI_PollingMatchOR()       QSPI_CR3 |= 0x80
#define QSPI_PollingManualStop()    QSPI_CR3 &= ~0x40
#define QSPI_PollingAutoStop()      QSPI_CR3 |= 0x40
#define QSPI_EnableTimeoutInt()     QSPI_CR3 |= 0x10
#define QSPI_EnableMatchInt()       QSPI_CR3 |= 0x08
#define QSPI_EnableFIFOInt()        QSPI_CR3 |= 0x04
#define QSPI_EnableTransferInt()    QSPI_CR3 |= 0x02
#define QSPI_EnableErrorInt()       QSPI_CR3 |= 0x01

#define QSPI_SetClockDivider(n)     QSPI_CR4 = (n)

#define QSPI_SetCSHold(n)           QSPI_DCR1 = (QSPI_DCR1 & ~0x70) | ((n) << 4)
#define QSPI_SetIdleCLKHigh()       QSPI_DCR1 |= 0x01
#define QSPI_SetIdleCLKLow()        QSPI_DCR1 &= ~0x01

#define QSPI_SetFlashSize(n)        QSPI_DCR2 = (n) & 0x1f

#define QSPI_CheckBusy()            (QSPI_SR1 & 0x20)
#define QSPI_CheckTimeout()         (QSPI_SR1 & 0x10)
#define QSPI_CheckMatch()           (QSPI_SR1 & 0x08)
#define QSPI_CheckFIFO()            (QSPI_SR1 & 0x04)
#define QSPI_CheckTransfer()        (QSPI_SR1 & 0x02)
#define QSPI_CheckError()           (QSPI_SR1 & 0x01)

#define QSPI_CheckFIFOLevel()       (QSPI_SR2 & 0x3f)

#define QSPI_ClearTimeout()         QSPI_FCR = 0x10
#define QSPI_ClearMatch()           QSPI_FCR = 0x08
#define QSPI_ClearTransfer()        QSPI_FCR = 0x02
#define QSPI_ClearError()           QSPI_FCR = 0x01

#define QSPI_SetDataLength(n)       QSPI_DLR2 = ((n) >> 8) & 0xff;  \
                                    QSPI_DLR1 = (n) & 0xff

#define QSPI_SetAddress(n)          QSPI_AR4 = ((DWORD)(n) >> 24) & 0xff;  \
                                    QSPI_AR3 = ((DWORD)(n) >> 16) & 0xff;  \
                                    QSPI_AR2 = ((DWORD)(n) >> 8) & 0xff;   \
                                    QSPI_AR1 = (n) & 0xff

#define QSPI_SetAlternate(n)        QSPI_ABR = (n) & 0xff

#define QSPI_SetInstruction(n)      QSPI_CCR1 = (n)
#define QSPI_SetAddressSize(n)      QSPI_CCR2 = (QSPI_CCR2 & ~0x30) | (((n) & 0x03) << 4)
#define QSPI_SetAlternateSize(n)    QSPI_CCR3 = (QSPI_CCR3 & ~0x03) | ((n) & 0x03)
#define QSPI_SetDummyCycles(n)      QSPI_CCR3 = (QSPI_CCR3 & ~0x7c) | (((n) & 0x1f) << 2)

#define QSPI_NoInstruction()        QSPI_CCR2 &= ~0x03
#define QSPI_InstructionSingMode()  QSPI_CCR2 = (QSPI_CCR2 & ~0x03) | 0x01
#define QSPI_InstructionDualMode()  QSPI_CCR2 = (QSPI_CCR2 & ~0x03) | 0x02
#define QSPI_InstructionQuadMode()  QSPI_CCR2 |= 0x03
#define QSPI_NoAddress()            QSPI_CCR2 &= ~0x0c
#define QSPI_AddressSingMode()      QSPI_CCR2 = (QSPI_CCR2 & ~0x0c) | 0x04
#define QSPI_AddressDualMode()      QSPI_CCR2 = (QSPI_CCR2 & ~0x0c) | 0x08
#define QSPI_AddressQuadMode()      QSPI_CCR2 |= 0x0c
#define QSPI_NoAlternate()          QSPI_CCR2 &= ~0xc0
#define QSPI_AlternateSingMode()    QSPI_CCR2 = (QSPI_CCR2 & ~0xc0) | 0x40
#define QSPI_AlternateDualMode()    QSPI_CCR2 = (QSPI_CCR2 & ~0xc0) | 0x80
#define QSPI_AlternateQuadMode()    QSPI_CCR2 |= 0xc0
#define QSPI_NoData()               QSPI_CCR4 &= ~0x03
#define QSPI_DataSingMode()         QSPI_CCR4 = (QSPI_CCR4 & ~0x03) | 0x01
#define QSPI_DataDualMode()         QSPI_CCR4 = (QSPI_CCR4 & ~0x03) | 0x02
#define QSPI_DataQuadMode()         QSPI_CCR4 |= 0x03

#define QSPI_SetWriteMode()         QSPI_CCR4 &= ~0x0c
#define QSPI_SetReadMode()          QSPI_CCR4 = (QSPI_CCR4 & ~0x0c) | 0x04
#define QSPI_SetPollingMode()       QSPI_CCR4 = (QSPI_CCR4 & ~0x0c) | 0x08

#define QSPI_InstructionOnce()      QSPI_CCR4 |= 0x10
#define QSPI_InstructionAlways()    QSPI_CCR4 &= ~0x10

#define QSPI_ReadData()             ACC = QSPI_DR
#define QSPI_WriteData(d)           QSPI_DR = (d)

#define QSPI_SetPollingMask(n)      QSPI_PSMKR1 = (n) & 0xff

#define QSPI_SetPollingMatch(n)     QSPI_PSMAR1 = (n) & 0xff

#define QSPI_SetPollingInterval(n)  QSPI_PIR2 = ((n) >> 8) & 0xff;      \
                                    QSPI_PIR1 = (n) & 0xff

//========================================================================
//                              参数定义
//========================================================================

typedef struct
{
	u8	FIFOLevel;		//设置FIFO阈值, 0~31
	u8	ClockDiv;		//设置时钟预分频器, 0~255
	u8	CSHold;     	//设置片选高电平时间, 0~7
	u8	CKMode;		    //设置空闲时CLK电平, 0/1
	u8	FlashSize;		//设置Flash大小, 0~31
	u8	SIOO;		    //仅发送指令一次模式, ENABLE/DISABLE
	u8	QSPI_EN;        //QSPI使能, ENABLE/DISABLE
} QSPI_InitTypeDef;

//========================================================================
//                              外部声明
//========================================================================

u8 QSPI_Inilize(QSPI_InitTypeDef *QSPI);
void QSPI_WRITE_INSTR(BYTE cmd);
void QSPI_WRITE_INSTR_SADDR8(BYTE cmd, BYTE addr);
void QSPI_WRITE_INSTR_SADDR16(BYTE cmd, WORD addr);
void QSPI_WRITE_INSTR_SADDR24(BYTE cmd, DWORD addr);
void QSPI_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_WRITE_INSTR_SADDR32(BYTE cmd, DWORD addr);
void QSPI_WRITE_INSTR_QADDR32(BYTE cmd, DWORD addr);
void QSPI_READ_INSTR_SDATA(BYTE cmd, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);

void QSPI_WRITE_QINSTR(BYTE cmd);
void QSPI_WRITE_QINSTR_QADDR8(BYTE cmd, BYTE addr);
void QSPI_WRITE_QINSTR_QADDR24(BYTE cmd, DWORD addr);
void QSPI_READ_QINSTR_QDATA(BYTE cmd, BYTE *pdat, WORD datalen);
void QSPI_READ_QINSTR_QADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);

void QSPI_DMA_READ(BYTE *pdat, WORD datalen);
void QSPI_DMA_WRITE(BYTE *pdat, WORD datalen);

void QSPI_DMA_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);

void QSPI_DMA_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen);
void QSPI_DMA_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen);

void QSPI_DMA_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_DMA_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_DMA_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);
void QSPI_DMA_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen);

void QSPI_POLLING_READ_INSTR_SDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks);
void QSPI_POLLING_READ_QINSTR_QDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks);

#endif
