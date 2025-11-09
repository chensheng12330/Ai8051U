#ifndef __W25Q512_H__
#define __W25Q512_H__

#include "def.h"

void W25Q_WaitBusy(int n);
void W25Q_QPI_WaitBusy(int n);
void W25Q_Enable_QE();

void W25Q_WriteEnable_06();
void W25Q_WriteEnableVSR_50();
void W25Q_WriteDisable_04();
BYTE W25Q_ReadSR1_05();
BYTE W25Q_ReadSR2_35();
BYTE W25Q_ReadSR3_15();
void W25Q_WriteSR1_01(BYTE dat);
void W25Q_WriteSR12_01(WORD dat);
void W25Q_WriteSR2_31(BYTE dat);
void W25Q_WriteSR3_11(BYTE dat);
BYTE W25Q_ReadEAR_C8();
void W25Q_WriteEAR_C5(BYTE dat);
void W25Q_ReadData_03(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_3B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_6B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_BB(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_WordRead_E7(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_OctalWordRead_E3(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_SetBurstWrap_77(BYTE wrap);
void W25Q_PageProgram_02(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_PageProgram_32(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_Erase4K_20(DWORD addr, BOOL wait);
void W25Q_Erase32K_52(DWORD addr, BOOL wait);
void W25Q_Erase64K_D8(DWORD addr, BOOL wait);
void W25Q_EraseChip_C7(BOOL wait);
void W25Q_EraseSuspend_75();
void W25Q_EraseResume_7A();
void W25Q_ProgramSuspend_75();
void W25Q_ProgramResume_7A();
void W25Q_PowerDown_B9();
void W25Q_ReleasePowerDown_AB();
BYTE W25Q_ReadDeviceID_AB();
WORD W25Q_ReadID_90();
WORD W25Q_ReadID_92();
WORD W25Q_ReadID_94();
void W25Q_ReadUniqueID_4B(BYTE *pdat);
DWORD W25Q_ReadJEDECID_9F();
void W25Q_ReadSFDP_5A(BYTE addr, BYTE *pdat, WORD datalen);
void W25Q_EraseSecurityReg_44(DWORD addr);
void W25Q_ProgramSecurityReg_42(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_ReadSecurityReg_48(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_LockSector_36(DWORD addr);
void W25Q_UnlockSector_39(DWORD addr);
BYTE W25Q_ReadLockSector_3D(DWORD addr);
void W25Q_LockGlobal_7E();
void W25Q_UnlockGlobal_98();
void W25Q_ResetDevice_66_99();

void W25Q_EnterAddr32Mode_B7();
void W25Q_ExitAddr32Mode_E9();
void W25Q_ReadData_Addr32_13(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_Addr32_0C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_Addr32_3C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_Addr32_6C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_Addr32_BC(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_FastRead_Addr32_EC(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_PageProgram_Addr32_12(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_PageProgram_Addr32_34(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_Erase4K_Addr32_21(DWORD addr, BOOL wait);
void W25Q_Erase64K_Addr32_DC(DWORD addr, BOOL wait);

void W25Q_EnterQPIMode_38();
void W25Q_QPI_ExitQPIMode_FF();
void W25Q_QPI_WriteEnable_06();
void W25Q_QPI_WriteEnableVSR_50();
void W25Q_QPI_WriteDisable_04();
BYTE W25Q_QPI_ReadSR1_05();
BYTE W25Q_QPI_ReadSR2_35();
BYTE W25Q_QPI_ReadSR3_15();
void W25Q_QPI_WriteSR1_01(BYTE dat);
void W25Q_QPI_WriteSR2_31(BYTE dat);
void W25Q_QPI_WriteSR3_11(BYTE dat);
void W25Q_QPI_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_QPI_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_QPI_Erase4K_20(DWORD addr, BOOL wait);
void W25Q_QPI_Erase32K_52(DWORD addr, BOOL wait);
void W25Q_QPI_Erase64K_D8(DWORD addr, BOOL wait);
void W25Q_QPI_EraseChip_C7(BOOL wait);
void W25Q_QPI_EraseResume_7A();
void W25Q_QPI_EraseSuspend_75();
void W25Q_QPI_ProgramSuspend_75();
void W25Q_QPI_ProgramResume_7A();
void W25Q_QPI_PowerDown_B9();
void W25Q_QPI_ReleasePowerDown_AB();
BYTE W25Q_QPI_ReadDeviceID_AB();
DWORD W25Q_QPI_ReadJEDECID_9F();
void W25Q_QPI_SetReadParam_C0(BYTE dat);
void W25Q_QPI_BurstReadWithWrap_0C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_QPI_LockSector_36(DWORD addr);
void W25Q_QPI_UnlockSector_39(DWORD addr);
BYTE W25Q_QPI_ReadLockSector_3D(DWORD addr);
void W25Q_QPI_LockGlobal_7E();
void W25Q_QPI_UnlockGlobal_98();
void W25Q_QPI_ResetDevice_66_99();

void W25Q_DMA_ReadData_03(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_3B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_6B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_BB(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_WordRead_E7(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_OctalWordRead_E3(DWORD addr, BYTE *pdat, WORD datalen);

void W25Q_DMA_ReadData_Addr32_13(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_Addr32_0C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_Addr32_3C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_Addr32_6C(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_Addr32_BC(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_FastRead_Addr32_EC(DWORD addr, BYTE *pdat, WORD datalen);

void W25Q_QPI_DMA_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_QPI_DMA_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen);

void W25Q_DMA_PageProgram_02(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_PageProgram_32(DWORD addr, BYTE *pdat, WORD datalen);

void W25Q_DMA_PageProgram_Addr32_12(DWORD addr, BYTE *pdat, WORD datalen);
void W25Q_DMA_PageProgram_Addr32_34(DWORD addr, BYTE *pdat, WORD datalen);

#endif
