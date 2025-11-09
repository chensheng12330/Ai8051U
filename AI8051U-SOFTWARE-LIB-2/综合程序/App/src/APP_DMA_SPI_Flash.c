/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_DMA_SPI_Flash.h"
#include "APP_SPI_Flash.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_SPI.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_DMA.h"
#include "AI8051U_Switch.h"

/*************	功能说明	**************

通过串口对SPI Flash进行读写测试。

对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

串口(P3.0,P3.1)默认波特率:  115200,8,N,1.

串口命令设置: (字母不区分大小写)
    E 0x001234              --> 扇区擦除，指定十六进制地址.
    W 0x001234 1234567890   --> 写入操作，指定十六进制地址，后面为写入内容.
    R 0x001234 10           --> 读出操作，指定十六进制地址，后面为读出字节数. 
    C                       --> 如果检测不到SPI Flash, 发送C强制允许操作.

注意：为了通用，程序不识别地址是否有效，用户自己根据具体的型号来决定。

串口写操作的内容放入SPI发送DMA空间，然后启动SPI_DMA进行发送.
读操作的内容通过SPI读取后放在DMA接收空间，由串口进行打印显示.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/

//========================================================================
//                               本地常量声明	
//========================================================================

sbit SPI_CE  = P4^0;     //PIN1
sbit SPI_SO  = P4^2;     //PIN2
sbit SPI_SI  = P4^1;     //PIN5
sbit SPI_SCK = P4^3;     //PIN6

#define DMA_BUF_LENGTH   255

//========================================================================
//                               本地变量声明
//========================================================================


//========================================================================
//                               本地函数声明
//========================================================================

void Command_Check(void);

//========================================================================
//                            外部函数和变量声明
//========================================================================


//========================================================================
// 函数: DMA_SPI_PS_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void DMA_SPI_Flash_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    COMx_InitDefine COMx_InitStructure;     //结构定义
    DMA_SPI_InitTypeDef DMA_SPI_InitStructure;

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE或DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //接收超时使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //超时中断使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //超时时间, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    SPI_InitStructure.SPI_Enable    = ENABLE;           //SPI启动    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = ENABLE;           //片选位     ENABLE, DISABLE
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;          //移位方向   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;  //主从选择   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;    //时钟相位   SPI_CPOL_Low,    SPI_CPOL_High
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;   //数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_16;     //SPI速度    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(DISABLE,Priority_0);      //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    SPI_ClearFlag();    //清除SPIF和WCOL标志

    SPI_SW(SPI_P40_P41_P42_P43);            //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32
    P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1); //P3.0,P3.1 设置为准双向口
    P4_MODE_IO_PU(GPIO_Pin_LOW);            //P40~P43 设置为准双向口
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2,P5.3 内部上拉使能

    SPI_SCK = 0;    // set clock to low initial state
    SPI_SI = 1;

	//----------------------------------------------
	DMA_SPI_InitStructure.DMA_Enable = ENABLE;				//DMA使能  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Tx_Enable = ENABLE;			//DMA发送数据使能  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Rx_Enable = ENABLE;			//DMA接收数据使能  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Length = DMA_BUF_LENGTH;		//DMA传输总字节数  	(0~65535) + 1
	DMA_SPI_InitStructure.DMA_Tx_Buffer = (u16)DmaTxBuffer;	//发送数据存储地址
	DMA_SPI_InitStructure.DMA_Rx_Buffer = (u16)DmaRxBuffer;	//接收数据存储地址
	DMA_SPI_InitStructure.DMA_SS_Sel = SPI_SS_P40;			//自动控制SS脚选择 	SPI_SS_P14,SPI_SS_P24,SPI_SS_P40,SPI_SS_P35
	DMA_SPI_InitStructure.DMA_AUTO_SS = DISABLE;			//自动控制SS脚使能  	ENABLE,DISABLE
	DMA_SPI_Inilize(&DMA_SPI_InitStructure);		        //初始化
	SET_DMA_SPI_CR(DMA_ENABLE | CLR_FIFO);	                //bit7 1:使能 SPI_DMA, bit5 1:开始 SPI_DMA 从机模式, bit0 1:清除 SPI_DMA FIFO
	NVIC_DMA_SPI_Init(ENABLE,Priority_0,Priority_0);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0~Priority_3; 总线优先级(低到高) Priority_0~Priority_3

	//----------------------------------------------
    printf("命令设置:\r\n");
    printf("E 0x001234            --> 扇区擦掉  十六进制地址\r\n");
    printf("W 0x001234 1234567890 --> 写入操作  十六进制地址  写入内容\r\n");
    printf("R 0x001234 10         --> 读出操作  十六进制地址  读出字节\r\n");
    printf("C                     --> 如果检测不到SPI Flash, 发送C强制允许操作.\r\n\r\n");

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
    printf("制造商ID1 = 0x%02X",FLASH_ID1);
    printf("\r\n      ID2 = 0x%02X",FLASH_ID2);
    printf("\r\n   设备ID = 0x%02X\r\n",FLASH_ID);
}

//========================================================================
// 函数: Sample_DMA_SPI_PS
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void Sample_DMA_SPI_Flash(void)
{
    if(COM1.RX_TimeOut)
    {
        COM1.RX_TimeOut = 0;

        if(COM1.RX_Cnt > 0)
        {
            Command_Check();        //命令解析处理
        }
        COM1.RX_Cnt = 0;
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
void SPI_DMA_Read_Nbytes(u32 addr, u16 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiTxFlag);                     //DMA忙检测
    while(CheckFlashBusy() > 0);        //Flash忙检测

    SPI_CE  = 0;                        //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //设置起始地址
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiTxFlag = 1;
    SET_SPI_DMA_LEN(size-1);    //设置传输总字节数：n+1
    DMA_SPI_TRIG_M();           //开始SPI_DMA主模式操作
}

/************************************************************************
读出n个字节,跟指定的数据进行比较, 错误返回1,正确返回0
************************************************************************/
u8 SPI_DMA_Read_Compare(u16 size)
{
    u8  j=0;
    if(size == 0)   return 2;
    if(!B_FlashOK)  return 2;
    while(SpiTxFlag);                         //DMA忙检测

    do
    {
        if(DmaRxBuffer[j] != DmaTxBuffer[j])       //receive byte and store at buffer
        {
            return 1;
        }
        j++;
    }while(--size);         //read until no_bytes is reached
    return 0;
}

/************************************************
写数据到Flash中
入口参数:
    addr   : 地址参数
    size   : 数据块大小
出口参数: 无
************************************************/
void SPI_DMA_Write_Nbytes(u32 addr, u8 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiTxFlag);                     //DMA忙检测
    while(CheckFlashBusy() > 0);        //Flash忙检测

    FlashWriteEnable();                 //使能Flash写命令

    SPI_CE  = 0;                        //enable device
    SPI_WriteByte(SFC_PAGEPROG);        // 发送页编程命令
    SPI_WriteByte(((u8 *)&addr)[1]);    //设置起始地址
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiTxFlag = 1;
    SET_SPI_DMA_LEN(size-1);    //设置传输总字节数：n+1
    DMA_SPI_TRIG_M();           //开始SPI_DMA主模式操作
}

void Command_Check(void)
{
    u8  i,j;

    if((COM1.RX_Cnt == 1) && (RX1_Buffer[0] == 'C'))    //发送C强制允许操作
    {
        B_FlashOK = 1;
        printf("强制允许操作FLASH!\r\n");
    }

    if(!B_FlashOK)
    {
        printf("PM25LV040/W25X40CL/W25Q80BV不存在, 不能操作FLASH!\r\n");
        return;
    }
    
    F0 = 0;
    if((COM1.RX_Cnt >= 10) && (RX1_Buffer[1] == ' '))   //最短命令为10个字节
    {
        for(i=0; i<10; i++)
        {
            if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';//小写转大写
        }
        Flash_addr = GetAddress();
        if(Flash_addr < 0x80000000)
        {
            if(RX1_Buffer[0] == 'E')    //擦除
            {
                FlashSectorErase(Flash_addr);
                printf("已擦掉一个扇区内容!\r\n");
                F0 = 1;
            }

            else if((RX1_Buffer[0] == 'W') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //写入N个字节
            {
                j = COM1.RX_Cnt - 11;
                for(i=0; i<j; i++)  DmaTxBuffer[i] = 0xff;      //检测要写入的空间是否为空
                SPI_DMA_Read_Nbytes(Flash_addr,j);
                i = SPI_DMA_Read_Compare(j);
                if(i > 0)
                {
                    printf("要写入的地址为非空,不能写入,请先擦掉!\r\n");
                }
                else
                {
                    for(i=0; i<j; i++)  DmaTxBuffer[i] = RX1_Buffer[i+11];
                    SPI_DMA_Write_Nbytes(Flash_addr,j);     //写N个字节 
                    SPI_DMA_Read_Nbytes(Flash_addr,j);
                    i = SPI_DMA_Read_Compare(j); //比较写入的数据
                    if(i == 0)
                    {
                        printf("已写入%d字节内容!\r\n",j);
                    }
                    else printf("写入错误!\r\n");
                }
                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //读出N个字节
            {
                j = GetDataLength();
                if((j > 0) && (j < DMA_BUF_LENGTH))
                {
                    SPI_DMA_Read_Nbytes(Flash_addr,j);
                    printf("读出%d个字节内容如下：\r\n",j);
                    for(i=0; i<j; i++) printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("命令错误!\r\n");
}
