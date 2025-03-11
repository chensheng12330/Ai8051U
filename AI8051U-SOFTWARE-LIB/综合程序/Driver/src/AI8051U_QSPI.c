/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_QSPI.h"

//========================================================================
// 函数: u8 QSPI_Inilize(ADC_InitTypeDef *ADCx)
// 描述: ADC初始化程序.
// 参数: ADCx: 结构参数,请参考adc.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 QSPI_Inilize(QSPI_InitTypeDef *QSPI)
{
    if(QSPI->FIFOLevel > 31)    return FAIL;    //错误
    if(QSPI->FlashSize > 31)    return FAIL;    //错误
    if(QSPI->CSHold > 7)        return FAIL;    //错误

    while (QSPI_CheckBusy());       //检测忙状态

    QSPI_SetFIFOLevel(QSPI->FIFOLevel);         //设置FIFO阈值为(31+1)=32字节
    QSPI_SetClockDivider(QSPI->ClockDiv);       //设置QSPI时钟为系统时钟/(4+1)
    QSPI_SetCSHold(QSPI->CSHold);               //设置CS保持时间为(1+1)=2个QSPI时钟

    if(QSPI->CKMode)    QSPI_SetIdleCLKHigh();  //空闲时SCK为高电平
    else    QSPI_SetIdleCLKLow();               //空闲时SCK为低电平

    QSPI_SetFlashSize(QSPI->FlashSize);         //设置Flash大小为2^(25+1)=64M字节,

    if(QSPI->SIOO)    QSPI_InstructionOnce();   //设置仅第一条事务发送指令
    else    QSPI_InstructionAlways();           //设置每个事务均发送指令

    if(QSPI->QSPI_EN)    QSPI_Enable();         //使能QSPI
    else    QSPI_Disable();                     //禁止QSPI

    return SUCCESS;
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
    QSPI_ClearTransfer();           //清除传输完成标志
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
    QSPI_ClearTransfer();           //清除传输完成标志
}
