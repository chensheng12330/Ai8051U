/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_QSPI_FLASH.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_QSPI.h"
#include "AI8051U_UART.h"
#include "AI8051U_Switch.h"

/*************    功能说明    **************

通过硬件QSPI接口1线、2线、4线模式对支持QSPI协议的Flash进行读写测试。

串口(P3.0,P3.1)打印访问结果，默认设置: 115200,8,N,1.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/


//========================================================================
//                               本地常量声明    
//========================================================================

sbit    QSPI_CS     =   P4^0;
sbit    QSPI_IO0    =   P4^1;
sbit    QSPI_IO1    =   P4^2;
sbit    QSPI_SCK    =   P4^3;
sbit    QSPI_IO2    =   P5^2;
sbit    QSPI_IO3    =   P5^3;

//========================================================================
//                               本地变量声明
//========================================================================

int i;
BYTE xdata buf[1024];

//========================================================================
//                               本地函数声明
//========================================================================

#define SIZE            30

#define INIT_BUF()      for (i=0; i<SIZE; i++) buf[i] = 0;
#define SET_BUF()       for (i=0; i<SIZE; i++) buf[i] = (BYTE)(i + 0x00);
#define PRINT_BUF()     for (i=0; i<SIZE; i++)                              \
                        {                                                   \
                            printf("%02bx ", buf[i]);                       \
                            if ((i % 32) == 31)                             \
                                printf("\n                            ");   \
                        }                                                   \
                        printf("\n");

//========================================================================
//                            外部函数和变量声明
//========================================================================

extern u8 Key1_cnt;
extern u8 Key2_cnt;
extern bit Key1_Flag;
extern bit Key2_Flag;

//========================================================================
// QSPI Flash 操作函数
//========================================================================
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

//========================================================================
// 函数: QSPI_Flash_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-02-08
//========================================================================
void QSPI_Flash_init(void)
{
    COMx_InitDefine COMx_InitStructure;     //结构定义
    QSPI_InitTypeDef QSPI_InitStructure;    //结构定义

    P4_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_3);    //P4.0, P4.3 设置为推挽输出
    P4_SPEED_HIGH(GPIO_Pin_LOW);                //P4.0~P4.3 设置快速电平转换
    P5_SPEED_HIGH(GPIO_Pin_2 | GPIO_Pin_3);     //P5.2, P5.3 设置快速电平转换
    P4_PULL_UP_ENABLE(GPIO_Pin_LOW);            //P4.0~P4.3 设置上拉使能
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2, P5.3 设置上拉使能
    P4_BP_ENABLE(GPIO_Pin_1 | GPIO_Pin_2);      //P4.1, P4.2 硬件自动设置端口模式
    P5_BP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3);      //P5.2, P5.3 硬件自动设置端口模式
    QSPI_SW(QSPI_P40_P41_P42_P52_P53_P43);      //QSPI_P14_P15_P16_P13_P12_P17,QSPI_P40_P41_P42_P52_P53_P43,QSPI_P47_P25_P26_P46_P45_P27

    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2 设置上拉使能
    
    QSPI_CS = 1;
    QSPI_SCK = 1;
    QSPI_IO0 = 1;
    QSPI_IO1 = 1;
    QSPI_IO2 = 1;
    QSPI_IO3 = 1;
    
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE 或 DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    NVIC_QSPI_Init(DISABLE,Priority_0);     //中断使能设置, QSPI_SMIE/QSPI_FTIE/QSPI_TCIE/QSPI_TEIE/DISABLE; 优先级(低到高) Priority_0~Priority_3
    QSPI_InitStructure.FIFOLevel  = 31;     //设置FIFO阈值, 0~31
    QSPI_InitStructure.ClockDiv   = 3;      //设置QSPI时钟 = 系统时钟/(n+1), 0~255
    QSPI_InitStructure.CSHold     = 1;      //设置CS保持时间为(n+1)个QSPI时钟, 0~7
    QSPI_InitStructure.CKMode     = 1;      //设置空闲时CLK电平, 0/1
    QSPI_InitStructure.FlashSize  = 25;     //设置Flash大小为2^(25+1)=64M字节, 0~31
    QSPI_InitStructure.SIOO       = DISABLE;//发送一次指令模式, ENABLE(仅第一条事务发送指令)/DISABLE(每个事务均发送指令)
    QSPI_InitStructure.QSPI_EN    = ENABLE; //QSPI使能, ENABLE/DISABLE
    QSPI_Inilize(&QSPI_InitStructure);      //初始化

    printf("QSPI Test !\n");
    printf("W25Q_ReadJEDECID_9F         %08lx\n", W25Q_ReadJEDECID_9F());
    printf("SR1: %02bx\n", W25Q_ReadSR1_05());
    printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    printf("SR3: %02bx\n", W25Q_ReadSR3_15());
    
    if ((W25Q_ReadSR2_35() & 0x02) == 0)
    {
        W25Q_WriteEnableVSR_50();
        if ((W25Q_ReadJEDECID_9F() & 0xffff) == 0x4014) //W25Q80写SR2方法不同
        {
            W25Q_WriteSR12_01(0x0002);
        }
        else
        {
            W25Q_WriteSR2_31(0x02);
        }
        printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    }
}

//========================================================================
// 函数: Sample_QSPI_Flash
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-02-08
//========================================================================
void Sample_QSPI_Flash(void)
{
	if(!P32)
	{
		if(!Key1_Flag)
		{
			Key1_cnt++;
			if(Key1_cnt > 50)
			{
				Key1_Flag = 1;

                printf("W25Q_Erase4K_20\n");
                W25Q_Erase4K_20(0, TRUE);
                
                printf("W25Q_ReadData_03            ");
                INIT_BUF();
                W25Q_ReadData_03(0, buf, SIZE);
                PRINT_BUF();

                printf("W25Q_PageProgram_02\n");
                SET_BUF();
                W25Q_PageProgram_02(0, buf, SIZE);
                
                printf("W25Q_ReadData_03            ");
                INIT_BUF();
                W25Q_ReadData_03(0, buf, SIZE);
                PRINT_BUF();
                
                printf("W25Q_FastRead_0B            ");
                INIT_BUF();
                W25Q_FastRead_0B(0, buf, SIZE);
                PRINT_BUF();
                
                printf("W25Q_FastRead_3B            ");
                INIT_BUF();
                W25Q_FastRead_3B(0, buf, SIZE);
                PRINT_BUF();
                
                printf("W25Q_FastRead_6B            ");
                INIT_BUF();
                W25Q_FastRead_6B(0, buf, SIZE);
                PRINT_BUF();
			}
		}
	}
	else
	{
		Key1_cnt = 0;
		Key1_Flag = 0;
	}
}
