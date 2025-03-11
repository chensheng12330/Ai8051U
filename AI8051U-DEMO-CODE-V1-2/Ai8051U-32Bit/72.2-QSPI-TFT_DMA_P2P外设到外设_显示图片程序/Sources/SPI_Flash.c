/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "system.h"
#include "intrins.h"
#include "SPI_Flash.h"
#include "stdio.h"
#include "uart.h"
#include "lcm.h"
#include "qspi.h"
#include "w25qxx.h"

/*************  本地常量声明    **************/

/*************  本地变量声明    **************/

u32 Flash_addr;
u16 lcdIndex;
u16 spiIndex;

/*************  FLASH相关变量声明   **************/
u8  B_FlashOK;                                //Flash状态
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

bit SpiDmaFlag;

void FlashCheckID(void);

//========================================================================
// 函数: void SPI_DMA_Config(void)
// 描述: SPI DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Config(void)
{
    //关闭接收DMA，下次接收的数据重新存放在起始地址位置，否则下次接收数据继续往后面存放。
    DMA_QSPI_CR = 0x00;        //bit7 1:使能 UART1_DMA, bit5 1:开始 Ubit0 1:清除 FIFO

    DMA_QSPI_STA = 0x00;
    DMA_QSPI_CFG = 0x20;                //使能DMA读取操作
    DMA_QSPI_AMT = (u8)(DMA_WR_LEN-1);         //设置传输总字节数(低8位)：n+1
    DMA_QSPI_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //设置传输总字节数(高8位)：n+1
    DMA_QSPI_CR = 0x81;        //bit7 1:使能 QSPI_DMA, bit6 1:开始, bit0 1:清除 SPI_DMA FIFO
}

/************************************************************************/
void SPI_init(void)
{
    QSPI_Init();

    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  printf("未检测到PM25LV040/W25X40CL/W25Q80BV/W25Q128FV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            printf("检测到PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            printf("检测到W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            printf("检测到W25Q80BV!\r\n");
        }
        else if(B_FlashOK == 4)
        {
            printf("检测到W25Q128FV!\r\n");
        }
    }
    printf("制造商ID1 = 0x%02X",PM25LV040_ID1);
    printf("\r\n      ID2 = 0x%02X",PM25LV040_ID2);
    printf("\r\n   设备ID = 0x%02X\r\n",PM25LV040_ID);
    
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

/************************************************
检测Flash是否准备就绪
入口参数: 无
出口参数:
    0 : 没有检测到正确的Flash
    1 : Flash准备就绪
************************************************/
void FlashCheckID(void)
{
    u8 id;
    
    id = W25Q_ReadDeviceID_AB();
    PM25LV040_ID1 = id;
    PM25LV040_ID  = id;
    PM25LV040_ID2 = id;

//    printf("ID1=%x\r\n",PM25LV040_ID1);
//    printf("ID=%x\r\n",PM25LV040_ID);
//    printf("ID2=%x\r\n",PM25LV040_ID2);
    
    if((PM25LV040_ID1 == 0x9d) && (PM25LV040_ID2 == 0x7f))  B_FlashOK = 1;  //检测是否为PM25LVxx系列的Flash
    else if(PM25LV040_ID == 0x12)  B_FlashOK = 2;                           //检测是否为W25X4x系列的Flash
    else if(PM25LV040_ID == 0x13)  B_FlashOK = 3;                           //检测是否为W25X8x系列的Flash
    else if(PM25LV040_ID == 0x17)  B_FlashOK = 4;                           //检测是否为W25X128系列的Flash
    else                                                    B_FlashOK = 0;
}

/************************************************
检测Flash的忙状态
入口参数: 无
出口参数:
    0 : Flash处于空闲状态
    1 : Flash处于忙状态
************************************************/
u8 CheckFlashBusy(void)
{
    return (W25Q_ReadSR1_05() & 0x01);
}


/************************************************
擦除整片Flash
入口参数: 无
出口参数: 无
************************************************/
void FlashChipErase(void)
{
    if(B_FlashOK)
    {
        W25Q_EraseChip_C7();            //发送片擦除命令
    }
}


/************************************************
从Flash中读取数据
入口参数:
    addr   : 地址参数
    buffer : 缓冲从Flash中读取的数据
    size   : 数据块大小
出口参数:
    无
************************************************/
void SPI_Read_Nbytes(u32 addr, u16 len)
{
    if(len == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiDmaFlag);                  //DMA忙检测

    while (QSPI_CheckBusy());           //检测忙状态
    QSPI_SetReadMode();                 //读模式
    QSPI_SetDataLength(len-1);          //设置数据长度
    QSPI_SetAddressSize(2);             //设置地址宽度为24位(2+1字节)
    QSPI_SetDummyCycles(8);             //设置DUMMY时钟
    QSPI_NoInstruction();               //设置无指令模式(防止误触发)
    QSPI_NoAddress();                   //设置无地址模式(防止误触发)
    QSPI_NoAlternate();                 //无间隔字节
    QSPI_DataQuadMode();                //设置数据为四线模式
    QSPI_SetInstruction(0x6b);          //设置指令(四线快速读取命令)
    QSPI_SetAddress(addr);              //设置地址
    QSPI_InstructionSingMode();         //设置指令为单线模式
    QSPI_AddressSingMode();             //设置地址为单线模式

    DMA_P2P_CR1 = 0x87;                 //P2P_SRC_QSPIRX(0x80) | P2P_DEST_LCMTX(0x07);
    DMA_QSPI_CFG = 0xa0;                //使能DMA读取操作
    DMA_QSPI_STA = 0x00;                //清除DMA状态
    DMA_QSPI_AMT = (len-1);             //设置DMA数据长度
    DMA_QSPI_AMTH = (len-1) >> 8;
    DMA_LCM_CR = 0xa1;
    DMA_QSPI_CR = 0xa1;                 //启动DMA并触发QSPI读操作

    SpiDmaFlag = 1;
}

/************************************************
写数据到Flash中
入口参数:
    addr   : 地址参数
    size   : 数据块大小
出口参数: 无
************************************************/
//void SPI_Write_Nbytes(u32 addr, u16 len)
//{
//}

//========================================================================
// 函数: void QSPI_DMA_Interrupt (void) interrupt DMA_QSPI_VECTOR
// 描述: QSPI DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void QSPI_DMA_Interrupt(void) interrupt DMA_QSPI_VECTOR   //中断向量号超过31，编译报错的话需安装（例程包根目录下的）Keil中断向量号拓展插件
{
    DMA_QSPI_STA = 0;
    SpiDmaFlag = 0;
}
