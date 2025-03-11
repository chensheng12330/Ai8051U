#include "config.h"
#include "qspi.h"
#include "w25qxx.h"


void W25Q_WriteEnable_06()
{
    QSPI_WRITE_INSTR(0x06);
}

void W25Q_QPI_WriteEnable_06()
{
    QSPI_WRITE_QINSTR(0x06);
}

void W25Q_WriteEnableVSR_50()
{
    QSPI_WRITE_INSTR(0x50);
}

void W25Q_QPI_WriteEnableVSR_50()
{
    QSPI_WRITE_QINSTR(0x50);
}

void W25Q_WriteDisable_04()
{
    QSPI_WRITE_INSTR(0x04);
}

void W25Q_QPI_WriteDisable_04()
{
    QSPI_WRITE_QINSTR(0x04);
}

BYTE W25Q_ReadSR1_05()
{
    BYTE dat;
    
    QSPI_READ_INSTR_SDATA(0x05, (BYTE *)&dat, 1);

    return dat;
}

void W25Q_WaitBusy(int n)       //自动轮询周期时间单位: us
{
    while (W25Q_ReadSR1_05() & 0x01);

    if (n);
//    QSPI_POLLING_READ_INSTR_SDATA(0x05, 0x01, 0x00, 12*n);
}

BYTE W25Q_QPI_ReadSR1_05()
{
    BYTE dat;
    
    QSPI_READ_QINSTR_QDATA(0x05, (BYTE *)&dat, 1);

    return dat;
}

void W25Q_QPI_WaitBusy(int n)
{
    QSPI_POLLING_READ_QINSTR_QDATA(0x05, 0x01, 0x00, 12*n);
}

BYTE W25Q_ReadSR2_35()
{
    BYTE dat;
    
    QSPI_READ_INSTR_SDATA(0x35, (BYTE *)&dat, 1);

    return dat;
}

BYTE W25Q_QPI_ReadSR2_35()
{
    BYTE dat;
    
    QSPI_READ_QINSTR_QDATA(0x35, (BYTE *)&dat, 1);

    return dat;
}

BYTE W25Q_ReadSR3_15()
{
    BYTE dat;
    
    QSPI_READ_INSTR_SDATA(0x15, (BYTE *)&dat, 1);

    return dat;
}

BYTE W25Q_QPI_ReadSR3_15()
{
    BYTE dat;
    
    QSPI_READ_QINSTR_QDATA(0x15, (BYTE *)&dat, 1);

    return dat;
}

void W25Q_WriteSR1_01(BYTE dat)
{
    QSPI_WRITE_INSTR_SADDR8(0x01, dat);
}

void W25Q_WriteSR12_01(WORD dat)
{
    QSPI_WRITE_INSTR_SADDR16(0x01, dat);    //SR1+SR2合并为16位地址
}

void W25Q_QPI_WriteSR1_01(BYTE dat)
{
    QSPI_WRITE_QINSTR_QADDR8(0x01, dat);
}

void W25Q_WriteSR2_31(BYTE dat)
{
    QSPI_WRITE_INSTR_SADDR8(0x31, dat);
    W25Q_WaitBusy(100);
}

void W25Q_QPI_WriteSR2_31(BYTE dat)
{
    QSPI_WRITE_QINSTR_QADDR8(0x31, dat);
}

void W25Q_WriteSR3_11(BYTE dat)
{
    QSPI_WRITE_INSTR_SADDR8(0x11, dat);
}

void W25Q_QPI_WriteSR3_11(BYTE dat)
{
    QSPI_WRITE_QINSTR_QADDR8(0x11, dat);
}

BYTE W25Q_ReadEAR_C8()
{
    BYTE dat;
    
    QSPI_READ_INSTR_SDATA(0xc8, (BYTE *)&dat, 1);

    return dat;
}

void W25Q_WriteEAR_C5(BYTE dat)
{
    QSPI_WRITE_INSTR_SADDR8(0xc5, dat);
}

void W25Q_EnterAddr32Mode_B7()
{
    QSPI_WRITE_INSTR(0xb7);
}

void W25Q_ExitAddr32Mode_E9()
{
    QSPI_WRITE_INSTR(0xe9);
}

void W25Q_ReadData_03(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_SDATA(0x03, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_ReadData_03(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_SDATA(0x03, addr, pdat, datalen);
}

void W25Q_ReadData_Addr32_13(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR32_SDATA(0x13, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_ReadData_Addr32_13(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_SDATA(0x13, addr, pdat, datalen);
}

void W25Q_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(0x0b, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(0x0b, addr, 8, pdat, datalen);
}

void W25Q_QPI_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(0x0b, addr, 2, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_QPI_DMA_FastRead_0B(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_QINSTR_QADDR24_DUMMY_QDATA(0x0b, addr, 2, pdat, datalen);
}

void W25Q_FastRead_Addr32_0C(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(0x0C, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_Addr32_0C(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(0x0C, addr, 8, pdat, datalen);
}

void W25Q_FastRead_3B(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_DUMMY_DDATA(0x3b, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_3B(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_DUMMY_DDATA(0x3b, addr, 8, pdat, datalen);
}

void W25Q_FastRead_Addr32_3C(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR32_DUMMY_DDATA(0x3c, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_Addr32_3C(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_DUMMY_DDATA(0x3c, addr, 8, pdat, datalen);
}

void W25Q_FastRead_6B(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_DUMMY_QDATA(0x6b, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_6B(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_DUMMY_QDATA(0x6b, addr, 8, pdat, datalen);
}

void W25Q_FastRead_Addr32_6C(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR32_DUMMY_QDATA(0x6c, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_Addr32_6C(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_DUMMY_QDATA(0x6c, addr, 8, pdat, datalen);
}

void W25Q_FastRead_BB(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_DADDR24_DALT8_DDATA(0xbb, addr, 0x00, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_BB(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_DADDR24_DALT8_DDATA(0xbb, addr, 0x00, pdat, datalen);
}

void W25Q_FastRead_Addr32_BC(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_DADDR32_DALT8_DDATA(0xbc, addr, 0x00, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_Addr32_BC(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_DADDR32_DALT8_DDATA(0xbc, addr, 0x00, pdat, datalen);
}

void W25Q_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xeb, addr, 0x00, 4, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xeb, addr, 0x00, 4, pdat, datalen);
}

void W25Q_QPI_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(0xeb, addr, 0x00, 0, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_QPI_DMA_FastRead_EB(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(0xeb, addr, 0x00, 0, pdat, datalen);
}

void W25Q_FastRead_Addr32_EC(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(0xec, addr, 0x00, 4, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_FastRead_Addr32_EC(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(0xec, addr, 0x00, 4, pdat, datalen);
}

void W25Q_WordRead_E7(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xe7, addr, 0x00, 2, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_WordRead_E7(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xe7, addr, 0x00, 2, pdat, datalen);
}

void W25Q_OctalWordRead_E3(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xe3, addr, 0x00, 0, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_DMA_OctalWordRead_E3(DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0xe3, addr, 0x00, 0, pdat, datalen);
}

void W25Q_SetBurstWrap_77(BYTE wrap)
{
    QSPI_WRITE_INSTR_QADDR32(0x77, wrap);
}

void W25Q_PageProgram_02(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    W25Q_WriteEnable_06();
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        if (cnt + (addr & 0xff) >= 0x100)
            cnt = 0x100 - (addr & 0xff);
            
        QSPI_WRITE_INSTR_SADDR24_SDATA(0x02, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
        
        if (datalen > 0)
        {
            W25Q_WaitBusy(100);
            W25Q_WriteEnable_06();
        }
    }
    W25Q_WaitBusy(100);
}

void W25Q_DMA_PageProgram_02(DWORD addr, BYTE *pdat, WORD datalen)
{
    W25Q_WriteEnable_06();
    QSPI_DMA_WRITE_INSTR_SADDR24_SDATA(0x02, addr, pdat, datalen);
    W25Q_WaitBusy(100);
}

void W25Q_PageProgram_Addr32_12(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    W25Q_WriteEnable_06();
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        if (cnt + (addr & 0xff) >= 0x100)
            cnt = 0x100 - (addr & 0xff);
            
        QSPI_WRITE_INSTR_SADDR32_SDATA(0x12, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
        
        if (datalen > 0)
        {
            W25Q_WaitBusy(100);
            W25Q_WriteEnable_06();
        }
    }
    W25Q_WaitBusy(100);
}

void W25Q_DMA_PageProgram_Addr32_12(DWORD addr, BYTE *pdat, WORD datalen)
{
    W25Q_WriteEnable_06();
    QSPI_DMA_WRITE_INSTR_SADDR32_SDATA(0x12, addr, pdat, datalen);
    W25Q_WaitBusy(100);
}

void W25Q_PageProgram_32(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    W25Q_WriteEnable_06();
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        if (cnt + (addr & 0xff) >= 0x100)
            cnt = 0x100 - (addr & 0xff);
            
        QSPI_WRITE_INSTR_SADDR24_QDATA(0x32, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
        
        if (datalen > 0)
        {
            W25Q_WaitBusy(100);
            W25Q_WriteEnable_06();
        }
    }
    W25Q_WaitBusy(100);
}

void W25Q_DMA_PageProgram_32(DWORD addr, BYTE *pdat, WORD datalen)
{
    W25Q_WriteEnable_06();
    QSPI_DMA_WRITE_INSTR_SADDR24_QDATA(0x32, addr, pdat, datalen);
    W25Q_WaitBusy(100);
}

void W25Q_PageProgram_Addr32_34(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    W25Q_WriteEnable_06();
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        if (cnt + (addr & 0xff) >= 0x100)
            cnt = 0x100 - (addr & 0xff);
            
        QSPI_WRITE_INSTR_SADDR32_QDATA(0x34, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
        
        if (datalen > 0)
        {
            W25Q_WaitBusy(100);
            W25Q_WriteEnable_06();
        }
    }
    W25Q_WaitBusy(100);
}

void W25Q_DMA_PageProgram_Addr32_34(DWORD addr, BYTE *pdat, WORD datalen)
{
    W25Q_WriteEnable_06();
    QSPI_DMA_WRITE_INSTR_SADDR32_QDATA(0x34, addr, pdat, datalen);
    W25Q_WaitBusy(100);
}

void W25Q_Erase4K_20(DWORD addr, BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR_SADDR24(0x20, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_QPI_Erase4K_20(DWORD addr, BOOL wait)
{
    W25Q_QPI_WriteEnable_06();
    QSPI_WRITE_QINSTR_QADDR24(0x20, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_Erase4K_Addr32_21(DWORD addr, BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR_SADDR32(0x21, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_Erase32K_52(DWORD addr, BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR_SADDR24(0x52, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_QPI_Erase32K_52(DWORD addr, BOOL wait)
{
    W25Q_QPI_WriteEnable_06();
    QSPI_WRITE_QINSTR_QADDR24(0x52, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_Erase64K_D8(DWORD addr, BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR_SADDR24(0xd8, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_QPI_Erase64K_D8(DWORD addr, BOOL wait)
{
    W25Q_QPI_WriteEnable_06();
    QSPI_WRITE_QINSTR_QADDR24(0xd8, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_Erase64K_Addr32_DC(DWORD addr, BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR_SADDR32(0xdc, addr);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_EraseChip_C7(BOOL wait)
{
    W25Q_WriteEnable_06();
    QSPI_WRITE_INSTR(0xc7);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_QPI_EraseChip_C7(BOOL wait)
{
    W25Q_QPI_WriteEnable_06();
    QSPI_WRITE_QINSTR(0xc7);
    W25Q_QPI_WaitBusy(100);
    if (wait)
        W25Q_WaitBusy(100);
}

void W25Q_EraseSuspend_75()
{
    QSPI_WRITE_INSTR(0x75);
}

void W25Q_QPI_EraseSuspend_75()
{
    QSPI_WRITE_QINSTR(0x75);
}

void W25Q_EraseResume_7A()
{
    QSPI_WRITE_INSTR(0x7a);
}

void W25Q_QPI_EraseResume_7A()
{
    QSPI_WRITE_QINSTR(0x7a);
}

void W25Q_ProgramSuspend_75()
{
    W25Q_EraseSuspend_75();
}

void W25Q_QPI_ProgramSuspend_75()
{
    W25Q_QPI_EraseSuspend_75();
}

void W25Q_ProgramResume_7A()
{
    W25Q_EraseResume_7A();
}

void W25Q_QPI_ProgramResume_7A()
{
    W25Q_QPI_EraseResume_7A();
}

void W25Q_PowerDown_B9()
{
    QSPI_WRITE_INSTR(0xb9);
}

void W25Q_QPI_PowerDown_B9()
{
    QSPI_WRITE_QINSTR(0xb9);
}

void W25Q_ReleasePowerDown_AB()
{
    QSPI_WRITE_INSTR(0xab);
}

void W25Q_QPI_ReleasePowerDown_AB()
{
    QSPI_WRITE_QINSTR(0xab);
}

BYTE W25Q_ReadDeviceID_AB()
{
    BYTE dat;

    QSPI_READ_INSTR_SADDR24_SDATA(0xab, 0x000000, &dat, 1);
    
    return dat;
}

BYTE W25Q_QPI_ReadDeviceID_AB()
{
    BYTE dat;

    QSPI_READ_QINSTR_QADDR24_QDATA(0xab, 0x000000, &dat, 1);
    
    return dat;
}

WORD W25Q_ReadID_90()
{
    WORD dat;
    
    QSPI_READ_INSTR_SADDR24_SDATA(0x90, 0x000000, (BYTE *)&dat, 2);

    return dat;
}

WORD W25Q_ReadID_92()
{
    WORD dat;
    
    QSPI_READ_INSTR_DADDR24_DALT8_DDATA(0x92, 0x000000, 0x00, (BYTE *)&dat, 2);

    return dat;
}

WORD W25Q_ReadID_94()
{
    WORD dat;
    
    QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(0x94, 0x000000, 0x00, 4, (BYTE *)&dat, 2);

    return dat;
}

void W25Q_ReadUniqueID_4B(BYTE *pdat)
{
    QSPI_READ_INSTR_SADDR32_SDATA(0x4b, 0x00000000, pdat, 8);
}

DWORD W25Q_ReadJEDECID_9F()
{
    DWORD dat;
    
    dat = 0;
    QSPI_READ_INSTR_SDATA(0x9f, (BYTE *)&dat+1, 3);

    return dat;
}

DWORD W25Q_QPI_ReadJEDECID_9F()
{
    DWORD dat;
    
    dat = 0;
    QSPI_READ_QINSTR_QDATA(0x9f, (BYTE *)&dat+1, 3);

    return dat;
}

void W25Q_ReadSFDP_5A(BYTE addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(0x5a, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_EraseSecurityReg_44(DWORD addr)
{
    QSPI_WRITE_INSTR_SADDR24(0x44, addr);
}

void W25Q_ProgramSecurityReg_42(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    W25Q_WriteEnable_06();
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_WRITE_INSTR_SADDR24_SDATA(0x42, addr, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
        
        if (datalen > 0)
        {
            W25Q_WaitBusy(100);
            W25Q_WriteEnable_06();
        }
    }
    W25Q_WaitBusy(100);
}

void W25Q_ReadSecurityReg_48(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(0x48, addr, 8, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_QPI_SetReadParam_C0(BYTE dat)
{
    QSPI_WRITE_QINSTR_QADDR8(0xc0, dat);
}

void W25Q_QPI_BurstReadWithWrap_0C(DWORD addr, BYTE *pdat, WORD datalen)
{
    BYTE cnt;
    
    while (datalen > 0)
    {
        cnt = 32;
        if (datalen < 32)
            cnt = datalen;
            
        QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(0x0c, addr, 2, pdat, cnt);
        
        datalen -= cnt;
        addr += cnt;
        pdat += cnt;
    }
}

void W25Q_EnterQPIMode_38()
{
    QSPI_WRITE_INSTR(0x38);
}

void W25Q_QPI_ExitQPIMode_FF()
{
    QSPI_WRITE_QINSTR(0xFF);
}

void W25Q_LockSector_36(DWORD addr)
{
    QSPI_WRITE_INSTR_SADDR24(0x36, addr);
}

void W25Q_QPI_LockSector_36(DWORD addr)
{
    QSPI_WRITE_QINSTR_QADDR24(0x36, addr);
}

void W25Q_UnlockSector_39(DWORD addr)
{
    QSPI_WRITE_INSTR_SADDR24(0x39, addr);
}

void W25Q_QPI_UnlockSector_39(DWORD addr)
{
    QSPI_WRITE_QINSTR_QADDR24(0x39, addr);
}

BYTE W25Q_ReadLockSector_3D(DWORD addr)
{
    BYTE dat;

    QSPI_READ_INSTR_SADDR24_SDATA(0x3d, addr, &dat, 1);
    
    return dat;
}

BYTE W25Q_QPI_ReadLockSector_3D(DWORD addr)
{
    BYTE dat;

    QSPI_READ_QINSTR_QADDR24_QDATA(0x3d, addr, &dat, 1);
    
    return dat;
}

void W25Q_LockGlobal_7E()
{
    QSPI_WRITE_INSTR(0x7e);
}

void W25Q_QPI_LockGlobal_7E()
{
    QSPI_WRITE_QINSTR(0x7e);
}

void W25Q_UnlockGlobal_98()
{
    QSPI_WRITE_INSTR(0x98);
}

void W25Q_QPI_UnlockGlobal_98()
{
    QSPI_WRITE_QINSTR(0x98);
}

void W25Q_ResetDevice_66_99()
{
    QSPI_WRITE_INSTR(0x66);
    QSPI_WRITE_INSTR(0x99);
}

void W25Q_QPI_ResetDevice_66_99()
{
    QSPI_WRITE_QINSTR(0x66);
    QSPI_WRITE_QINSTR(0x99);
}

void W25Q_Enable_QE()
{
    if ((W25Q_ReadJEDECID_9F() & 0xffff00) == 0xef4000)
    {
        if ((W25Q_ReadSR2_35() & 0x02) == 0)
        {
            W25Q_WriteEnableVSR_50();
            if ((W25Q_ReadJEDECID_9F() & 0xffff) == 0x4014)
            {
                W25Q_WriteSR12_01(0x0002);
            }
            else
            {
                W25Q_WriteSR2_31(0x02);
            }
        }
    }
}
