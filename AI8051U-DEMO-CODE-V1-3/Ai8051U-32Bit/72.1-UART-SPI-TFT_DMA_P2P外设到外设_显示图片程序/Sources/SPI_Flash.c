/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "system.h"
#include "intrins.h"
#include "SPI_Flash.h"
#include "stdio.h"
#include "uart.h"
#include "lcm.h"

/*************  本地常量声明    **************/

//======== PLL 相关定义 =========
#define HSCK_MCLK       0
#define HSCK_PLL        1
#define HSCK_SEL        HSCK_PLL

#define PLL_96M         0       //PLL时钟 = PLL输入时钟*8
#define PLL_144M        1       //PLL时钟 = PLL输入时钟*12
#define PLL_SEL         PLL_144M

#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

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
    DMA_SPI_CR = 0x00;      //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO

    DMA_SPI_STA = 0x00;
    DMA_SPI_CFG = 0xE5;     //bit7 1:Enable Interrupt, 提供SPI_DMA优先级
    DMA_SPI_AMT = (u8)(DMA_AMT_LEN-1);         //设置传输总字节数(低8位)：n+1
    DMA_SPI_AMTH = (u8)((DMA_AMT_LEN-1) >> 8); //设置传输总字节数(高8位)：n+1

    DMA_SPI_CFG2 = 0x02;    //10:P4.0
    DMA_SPI_CR = 0x81;      //bit7 1:使能 SPI_DMA, bit6 1:开始 SPI_DMA 主机模式, bit0 1:清除 SPI_DMA FIFO
}

//========================================================================
// 函数: void SPI_DMA_Reset(void)
// 描述: SPI DMA 寄存器重置配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Reset(void)
{
    DMA_SPI_CR = 0x00;      //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
    DMA_SPI_STA = 0x00;
    DMA_SPI_CFG = 0x00;     //bit7 1:Enable Interrupt, 提供SPI_DMA优先级
}

/******************* FLASH相关程序 ************************/
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

#define SPI_CE_High()   SPI_CS  = 1     // set CE high
#define SPI_CE_Low()    SPI_CS  = 0     // clear CE low

/************************************************************************/
/*
void delay()
{
    int i;
    
    for (i=0; i<100; i++);
}

void PLL_Init(void)
{
    //选择PLL输出时钟
#if (PLL_SEL == PLL_96M)
    CLKSEL &= ~CKMS;            //选择PLL的96M作为PLL的输出时钟
#elif (PLL_SEL == PLL_144M)
    CLKSEL |= CKMS;             //选择PLL的144M作为PLL的输出时钟
#else
    CLKSEL &= ~CKMS;            //默认选择PLL的96M作为PLL的输出时钟
#endif
    
    //选择PLL输入时钟分频,保证输入时钟为12M(允许范围：8M~16M, 12M +/- 4M)
    USBCLK &= ~PCKI_MSK;
//#if (MAIN_Fosc == 12000000UL)
//    USBCLK |= PCKI_D1;          //PLL输入时钟1分频
//#elif (MAIN_Fosc == 24000000UL)
//    USBCLK |= PCKI_D2;          //PLL输入时钟2分频
//#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL输入时钟4分频
//#elif (MAIN_Fosc == 96000000UL)
//    USBCLK |= PCKI_D8;          //PLL输入时钟8分频
//#else
//    USBCLK |= PCKI_D1;          //默认PLL输入时钟1分频
//#endif

    //启动PLL
    USBCLK |= ENCKM;            //使能PLL倍频
    
    delay();                    //等待PLL锁频

    //选择HSPWM/HSSPI时钟
#if (HSCK_SEL == HSCK_MCLK)
    CLKSEL &= ~HSIOCK;          //HSPWM/HSSPI选择主时钟为时钟源
#elif (HSCK_SEL == HSCK_PLL)
    CLKSEL |= HSIOCK;           //HSPWM/HSSPI选择PLL输出时钟为时钟源
#else
    CLKSEL &= ~HSIOCK;          //默认HSPWM/HSSPI选择主时钟为时钟源
#endif

    //主频35M，PLL=35M/4*12/2=103.2MHz/2=52.5MHz
    HSCLKDIV = 2;               //HSPWM/HSSPI时钟源2分频
    
    SPCTL = 0xd3;               //设置SPI为主机模式,速度为SPI时钟/2(52.5M/2=26.25M)
    HSSPI_CFG2 |= 0x20;         //使能SPI高速模式

    P4SR = 0xf0;                //电平转换速度快（改善IO口高速翻转信号）
}
*/
/************************************************************************/
void SPI_init(void)
{
    SPI_S1 = 1;     //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    SPI_S0 = 0;

//    PLL_Init();       //SPI 使用PLL高速时钟信号(速度太快Flash通信不正常)
    
    SSIG = 1; //忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
    SPEN = 1; //使能 SPI 功能
    DORD = 0; //先发送/接收数据的高位（MSB）
    MSTR = 1; //设置主机模式
    CPOL = 1; //SCLK 空闲时为低电平，SCLK 的前时钟沿为上升沿，后时钟沿为下降沿
    CPHA = 1; //数据 SS 管脚为低电平驱动第一位数据并在 SCLK 的后时钟沿改变数据
    SPCTL = (SPCTL & ~3) | 0;   //SPI 时钟频率选择, 0: 4T, 1: 8T,  2: 16T,  3: 2T

    SPI_SCK = 0;    // set clock to low initial state
    SPI_MOSI = 1;
    SPI_HOLD = 1;
    SPI_WP = 1;
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志

    HSCLKDIV = 1;       //设置高速IO时钟分频为1分频，默认2分频
//    HSSPI_CFG = 0;      //设置高速模式时，HOLD时间与SETUP时间
//    HSSPI_CFG2 = 0x10; //使能FIFO模式，减少SPI发送间隔时间
    
    printf("检测ID...\r\n");
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
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    SPDAT = out;
    while(SPIF == 0);
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    while(SPIF == 0);
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
    return (SPDAT);
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
    SPI_CE_Low();
    SPI_WriteByte(SFC_RDID);        //发送读取ID命令
    SPI_WriteByte(0x00);            //空读3个字节
    SPI_WriteByte(0x00);
    SPI_WriteByte(0x00);
    PM25LV040_ID1 = SPI_ReadByte();         //读取制造商ID1
    PM25LV040_ID  = SPI_ReadByte();         //读取设备ID
    PM25LV040_ID2 = SPI_ReadByte();         //读取制造商ID2
    SPI_CE_High();

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
    u8  dat;

    SPI_CE_Low();
    SPI_WriteByte(SFC_RDSR);        //发送读取状态命令
    dat = SPI_ReadByte();           //读取状态
    SPI_CE_High();

    return (dat);                   //状态值的Bit0即为忙标志
}

/************************************************
使能Flash写命令
入口参数: 无
出口参数: 无
************************************************/
void FlashWriteEnable(void)
{
    u16 timeout = 2000;
    while((CheckFlashBusy() > 0) && timeout)    //Flash忙检测
    {
        timeout--;
    }
//    while(CheckFlashBusy());
    SPI_CE_Low();
    SPI_WriteByte(SFC_WREN);        //发送写使能命令
    SPI_CE_High();
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
        FlashWriteEnable();             //使能Flash写命令
        SPI_CE_Low();
        SPI_WriteByte(SFC_CHIPER);      //发送片擦除命令
        SPI_CE_High();
        while(CheckFlashBusy() > 0);    //Flash忙检测
    }
}

/************************************************
擦除扇区, 一个扇区4KB
入口参数: 无
出口参数: 无
************************************************/
//void FlashSectorErase(u32 addr)
//{
//    if(B_FlashOK)
//    {
//        FlashWriteEnable();             //使能Flash写命令
//        SPI_CE_Low();
//        if(B_FlashOK == 1)
//        {
//            SPI_WriteByte(SFC_SECTORER1);    //发送扇区擦除命令
//        }
//        else
//        {
//            SPI_WriteByte(SFC_SECTORER2);    //发送扇区擦除命令
//        }
//        SPI_WriteByte(((u8 *)&addr)[1]);     //设置起始地址
//        SPI_WriteByte(((u8 *)&addr)[2]);
//        SPI_WriteByte(((u8 *)&addr)[3]);
//        SPI_CE_High();
//    }
//}

/************************************************
从Flash中读取数据
入口参数:
    addr   : 地址参数
    buffer : 缓冲从Flash中读取的数据
    size   : 数据块大小
出口参数:
    无
************************************************/
void SPI_Read_P2P(u32 addr)
{
    u16 timeout = 2000;
    if(!B_FlashOK)  return;
    while(SpiDmaFlag);                     //DMA忙检测
//    while(CheckFlashBusy() > 0);        //Flash忙检测
    while((CheckFlashBusy() > 0) && timeout)    //Flash忙检测
    {
        timeout--;
    }

    SPI_CE_Low();                       //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //设置起始地址
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    DMA_P2P_CR2 = P2P_SRC_SPIRX | P2P_DEST_LCMTX;   //P2P2 SPI RX -> LCM TX
    SpiDmaFlag = 1;
    DMA_LCM_CR |= 0x21;     //LCM DMA Write dat    
    DMA_SPI_CR |= 0x40;     //开始SPI_DMA主模式操作
}

/************************************************
写数据到Flash中
入口参数:
    addr   : 地址参数
    size   : 数据块大小
出口参数: 无
************************************************/
void SPI_P2P_Write(u32 addr)
{
    if(!B_FlashOK)  return;
    DMA_P2P_CR1 = 0;
    SPI_DMA_Reset();

    FlashWriteEnable();                 //使能Flash写命令

    SPI_CE_Low();                       // enable device
    SPI_WriteByte(SFC_PAGEPROG);        // 发送页编程命令
    SPI_WriteByte(((u8 *)&addr)[1]);    //设置起始地址
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    DMA_P2P_CR1 = P2P_SRC_UR1RX | P2P_DEST_SPITX;   //P2P2 UART1 RX -> SPI TX
}

//========================================================================
// 函数: void SPI_DMA_Interrupt (void) interrupt 49
// 描述: SPI DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void SPI_DMA_Interrupt(void) interrupt 49   //中断向量号超过31，编译报错的话需安装（例程包根目录下的）Keil中断向量号拓展插件
{
    DMA_SPI_STA = 0;
    SpiDmaFlag = 0;
    SPI_CE_High();
}
