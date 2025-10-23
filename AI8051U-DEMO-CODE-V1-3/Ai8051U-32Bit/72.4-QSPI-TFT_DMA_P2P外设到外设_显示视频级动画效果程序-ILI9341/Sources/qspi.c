#include "config.h"
#include "qspi.h"

#define QSPI_SW     2

#if (QSPI_SW == 2)
    sbit    CS          =   P4^0;
    sbit    SDI_IO0     =   P4^1;
    sbit    SDO_IO1     =   P4^2;
    sbit    SCK         =   P4^3;
    sbit    WP_IO2      =   P5^2;
    sbit    HLD_IO3     =   P5^3;
#else
    sbit    CS          =   P1^4;
    sbit    SDI_IO0     =   P1^5;
    sbit    SDO_IO1     =   P1^6;
    sbit    SCK         =   P1^7;
    sbit    WP_IO2      =   P1^3;
    sbit    HLD_IO3     =   P1^2;
#endif


void QSPI_Init()
{
#if (QSPI_SW == 2)
    P4M0 |= 0x09;               //设置CS,SCK为强推挽模式
    P4M1 &= ~0x09;
    P4SR &= ~0x0f;              //设置所有的QSPI口为快速模式
    P5SR &= ~0x0c;
    P4PU |= 0x0f;               //使能所有的QSPI口的内部10K上拉电阻
    P5PU |= 0x0c;
    P4BP &= ~0x06;              //使能QSPI的IO0~IO3数据硬件自动设置端口模式
    P5BP &= ~0x0c;
    P_SW4 = (P_SW4 & ~0x03) | 0x01;
                                //使用QSPI的第2组切换(P4.0/P4.1/P4.2/P4.3/P5.2/P5.3)
#else
    P1M0 |= 0x90;               //设置CS,SCK为强推挽模式
    P1M1 &= ~0x90;
    P1SR &= ~0xfc;              //设置所有的QSPI口为快速模式
    P1PU |= 0xfc;               //使能所有的QSPI口的内部10K上拉电阻
    P1BP &= ~0x6c;              //使能QSPI的IO0~IO3数据硬件自动设置端口模式
    P_SW4 &= ~0x03;             //使用QSPI的第1组切换(PP1.2/P1.3/P1.4/P1.5/P1.6/P1.7)
#endif
    
    CS = 1;
    SCK = 1;
    SDI_IO0 = 1;
    SDO_IO1 = 1;
    WP_IO2 = 1;
    HLD_IO3 = 1;

    while (QSPI_CheckBusy());       //检测忙状态
    
    QSPI_HCR1 = 0x00;               //设置READ_HOLD时间1(0x00)
    QSPI_HCR2 = 0x00;               //设置READ_HOLD时间2(0x00)
    QSPI_SetFIFOLevel(31);          //设置FIFO阈值为(31+1)=32字节
    QSPI_CR3 = 0x00;                //关闭QSPI中断
    QSPI_SetClockDivider(3);        //设置QSPI时钟为系统时钟/(3+1)
    QSPI_SetCSHold(1);              //设置CS保持时间为(1+1)=2个QSPI时钟
    QSPI_SetSCKNormalHigh();        //空闲时SCK为高电平
    QSPI_SetFlashSize(25);          //设置Flash大小为2^(25+1)=64M字节,
    QSPI_InstructionAlways();       //设置每个事务均发送指令
    
    QSPI_Enable();                  //使能QSPI
}

void QSPI_WRITE_INSTR(BYTE cmd)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_READ_INSTR_SDATA(BYTE cmd, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_WRITE_INSTR_SADDR8(BYTE cmd, BYTE addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(0);         //设置地址宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_WRITE_INSTR_SADDR16(BYTE cmd, WORD addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(1);         //设置地址宽度为16位(1+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_WRITE_INSTR_SADDR24(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_WRITE_INSTR_SADDR32(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_WRITE_INSTR_QADDR32(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    
    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressDualMode();         //设置地址为双线模式
    QSPI_AlternateDualMode();       //设置间隔字节为双线模式
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_AlternateDualMode();       //设置间隔字节为双线模式
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressDualMode();         //设置地址为双线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressDualMode();         //设置地址为双线模式
    QSPI_AlternateDualMode();       //设置间隔字节为双线模式
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_AlternateDualMode();       //设置间隔字节为双线模式
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressDualMode();         //设置地址为双线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataDualMode();            //设置数据为双线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //写数据到FIFO中
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_DMA_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (datalen)
    {
        QSPI_WriteData(*pdat);      //写数据到FIFO中
        pdat++;
        datalen--;
    }
    
    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_DMA_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //写数据到FIFO中
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_DMA_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //写数据到FIFO中
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_DMA_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(3);         //设置地址宽度为32位(3+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_AddressSingMode();         //设置地址为单线模式
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_QINSTR(BYTE cmd)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_READ_QINSTR_QDATA(BYTE cmd, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_WRITE_QINSTR_QADDR8(BYTE cmd, BYTE addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(0);         //设置地址宽度为8位(0+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //从FIFO中读取数据
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetDataLength(datalen-1);  //设置数据长度
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetAlternateSize(0);       //设置间隔字节宽度为8位(0+1字节)
    QSPI_SetDummyCycles(dcyc);      //设置DUMMY时钟
    QSPI_NoInstruction();           //设置无指令模式(防止误触发)
    QSPI_NoAddress();               //设置无地址模式(防止误触发)
    QSPI_AlternateQuadMode();       //设置间隔字节为四线模式
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetAlternate(alt);         //设置间隔字节
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_WRITE_QINSTR_QADDR24(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetWriteMode();            //写模式
    QSPI_SetAddressSize(2);         //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_AddressQuadMode();         //设置地址为四线模式
    QSPI_NoAlternate();             //无间隔字节
    QSPI_NoData();                  //无数据
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetAddress(addr);          //设置地址

    while (!QSPI_CheckTransfer());  //等到数据传输完成
    QSPI_ClearTransfer();           //清除传输完成标志
}

void QSPI_READ_QINSTR_QADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_POLLING_READ_INSTR_SDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetPollingMask(mask);      //设置轮询状态屏蔽位
    QSPI_SetPollingMatch(match);    //设置轮询状态匹配位
    QSPI_SetPollingInterval(clks);  //设置轮询周期
    QSPI_PollingMatchAND();         //设置轮询匹配模式
    QSPI_PollingAutoStop();         //设置轮询相匹配时自动停止轮询
    QSPI_SetDataLength(0);          //设置数据长度
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionSingMode();     //设置指令为单线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_DataSingMode();            //设置数据为单线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetPollingMode();          //轮询模式

    while (!QSPI_CheckMatch());     //等到轮询完成
    QSPI_ClearMatch();              //清除轮询完成标志

    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_POLLING_READ_QINSTR_QDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks)
{
    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetReadMode();             //读模式
    QSPI_SetPollingMask(mask);      //设置轮询状态屏蔽位
    QSPI_SetPollingMatch(match);    //设置轮询状态匹配位
    QSPI_SetPollingInterval(clks);  //设置轮询周期
    QSPI_PollingMatchAND();         //设置轮询匹配模式
    QSPI_PollingAutoStop();         //设置轮询相匹配时自动停止轮询
    QSPI_SetDataLength(0);          //设置数据长度
    QSPI_SetDummyCycles(0);         //设置DUMMY时钟
    QSPI_InstructionQuadMode();     //设置指令为四线模式
    QSPI_NoAddress();               //无地址字节
  	QSPI_NoAlternate();             //无间隔字节
    QSPI_DataQuadMode();            //设置数据为四线模式
    QSPI_SetInstruction(cmd);       //设置指令
    QSPI_SetPollingMode();          //轮询模式

    while (!QSPI_CheckMatch());     //等到轮询完成
    QSPI_ClearMatch();              //清除轮询完成标志

    while (QSPI_CheckFIFOLevel())   //清空FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ(BYTE *pdat, WORD datalen)
{
    DMA_QSPI_AMT = datalen-1;       //设置DMA数据长度
    DMA_QSPI_AMTH = (datalen-1) >> 8;
    DMA_QSPI_RXAH = (WORD)pdat >> 8;//设置DMA的存储器起始地址
    DMA_QSPI_RXAL = (BYTE)pdat;     //设置DMA的存储器起始地址
    DMA_QSPI_STA = 0x00;            //清除DMA状态
    DMA_QSPI_CFG = 0x20;            //使能DMA读取操作
    DMA_QSPI_CR = 0xa1;             //启动DMA并触发QSPI读操作
    while (!(DMA_QSPI_STA & 0x01)); //等待DMA操作完成
    DMA_QSPI_STA = 0x00;            //清除DMA状态
    DMA_QSPI_CFG = 0x00;
    DMA_QSPI_CR = 0x00;
}

void QSPI_DMA_WRITE(BYTE *pdat, WORD datalen)
{
    DMA_QSPI_AMT = datalen-1;       //设置DMA数据长度
    DMA_QSPI_AMTH = (datalen-1) >> 8;
    DMA_QSPI_TXAH = (WORD)pdat >> 8;//设置DMA的存储器起始地址
    DMA_QSPI_TXAL = (BYTE)pdat;     //设置DMA的存储器起始地址
    DMA_QSPI_STA = 0x00;            //清除DMA状态
    DMA_QSPI_CFG = 0x40;            //使能DMA写操作
    DMA_QSPI_CR = 0xc2;             //启动DMA并触发QSPI写操作
    while (!(DMA_QSPI_STA & 0x01)); //等待DMA操作完成
    DMA_QSPI_STA = 0x00;            //清除DMA状态
    DMA_QSPI_CFG = 0x00;
    DMA_QSPI_CR = 0x00;
}
