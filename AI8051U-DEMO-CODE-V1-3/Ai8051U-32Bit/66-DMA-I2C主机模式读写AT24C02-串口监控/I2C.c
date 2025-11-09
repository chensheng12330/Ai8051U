/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  本程序功能说明  **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

串口发指令通过I2C DMA读写AT24C02数据.

串口(P3.0,P3.1)默认设置: 115200,8,N,1. 

串口命令设置: (字母不区分大小写)
    W 0x10 12345678 --> 写入操作  十六进制地址  写入内容.
    R 0x10 8        --> 读出操作  十六进制地址  读出字节数.

24CXX连续写入的字节数不同, 1\2Kbit 8-Byte/Page, 4\8\16Kbit 16-Byte/Page.
连续写入的话，地址+数据长度不要超过一个PAGE范围.

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "../comm/AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/


/*************  本地常量声明    **************/

#define EE_BUF_LENGTH       255          //
#define UART1_BUF_LENGTH    (EE_BUF_LENGTH+7)   //串口缓冲长度

#define SLAW    0xA0
#define SLAR    0xA1

/*************  本地变量声明    **************/

u8 EEPROM_addr;
u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

u8  RX1_TimeOut;
u16 RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志
bit	DmaTxFlag=0;
bit	DmaRxFlag=0;

u8  RX1_Buffer[UART1_BUF_LENGTH];   //接收缓冲

/*************  本地函数声明    **************/

void I2C_init(void);
void WriteNbyte(u8 addr, u8 number);
void ReadNbyte( u8 addr, u8 number);
void delay_ms(u8 ms);
void RX1_Check(void);
void DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //定时器时钟1T模式
    S1BRT = 0;      //串口1选择定时器1为波特率发生器
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //定时器1开始计时

//    SCON = (SCON & 0x3f) | 0x40; 
//    T2L  = TM;
//    T2H  = TM>>8;
//    AUXR |= 0x15; //串口1选择定时器2为波特率发生器

    REN = 1;        //允许接收
    ES = 1;         //使能串口1中断
}

void UartPutc(unsigned char dat)
{
    SBUF = dat; 
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}
 
char putchar(char c)
{
    UartPutc(c);
    return c;
}

/**********************************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    I2C_init();
    UartInit();
    DMA_Config();
    EA = 1;     //允许总中断

    printf("命令设置:\r\n");
    printf("W 0x10 12345678 --> 写入操作  十六进制地址  写入内容\r\n");
    printf("R 0x10 8        --> 读出操作  十六进制地址  读出字节内容\r\n");

    while(1)
    {
        delay_ms(1);

        if(RX1_TimeOut > 0)
        {
            if(--RX1_TimeOut == 0)  //超时,则串口接收结束
            {
                if(RX1_Cnt > 0)
                {
                    RX1_Check();    //串口1处理数据
                }
                RX1_Cnt = 0;
            }
        }
    }
} 

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/**************** ASCII码转BIN ****************************/
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** 获取写入地址 ****************************/
u8 GetAddress(void)
{
    u8 address;
    u8  i,j;
    
    address = 0;
    if((RX1_Buffer[2] == '0') && (RX1_Buffer[3] == 'X'))
    {
        for(i=4; i<6; i++)
        {
            j = CheckData(RX1_Buffer[i]);
            if(j >= 0x10)   return 0;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0; //error
}

/**************** 获取要读出数据的字节数 ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=7; i<RX1_Cnt; i++)
    {
        if(CheckData(RX1_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX1_Buffer[i]);
    }
    return (length);
}

/**************** 串口处理函数 ****************************/

void RX1_Check(void)
{
    u8  i,j;

    F0 = 0;
    if((RX1_Cnt >= 8) && (RX1_Buffer[1] == ' '))   //最短命令为8个字节
    {
        for(i=0; i<6; i++)
        {
            if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';//小写转大写
        }
        EEPROM_addr = GetAddress();
        if(EEPROM_addr <= 255)
        {
            if((RX1_Buffer[0] == 'W') && (RX1_Cnt >= 8) && (RX1_Buffer[6] == ' '))   //写入N个字节
            {
                j = RX1_Cnt - 7;
                
                for(i=0; i<j; i++)  DmaTxBuffer[i+2] = RX1_Buffer[i+7];
                WriteNbyte(EEPROM_addr, j);     //写N个字节 
                printf("已写入%d字节内容!\r\n",j);
                delay_ms(5);

                ReadNbyte(EEPROM_addr, j);
                printf("读出%d个字节内容如下：\r\n",j);
                for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                printf("\r\n");

                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (RX1_Cnt >= 8) && (RX1_Buffer[6] == ' '))   //读出N个字节
            {
                j = GetDataLength();
                if((j > 0) && (j <= EE_BUF_LENGTH))
                {
                    ReadNbyte(EEPROM_addr, j);
                    printf("读出%d个字节内容如下：\r\n",j);
                    for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("命令错误!\r\n");
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: I2C DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_I2CT_STA = 0x00;
    DMA_I2CT_CFG = 0x80;	//bit7 1:Enable Interrupt
    DMA_I2CT_AMT = 0xff;	//设置传输总字节数(低8位)：n+1
    DMA_I2CT_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
    DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C发送数据存储地址
    DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
    DMA_I2CT_CR = 0x80;		//bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

    DMA_I2CR_STA = 0x00;
    DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
    DMA_I2CR_AMT = 0xff;	//设置传输总字节数(低8位)：n+1
    DMA_I2CR_AMTH = 0x00;	//设置传输总字节数(高8位)：n+1
    DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C接收数据存储地址
    DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
    DMA_I2CR_CR = 0x81;		//bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO

    DMA_I2C_ST1 = 0xff;		//设置需要传输字节数(低8位)：n+1
    DMA_I2C_ST2 = 0x00;		//设置需要传输字节数(高8位)：n+1
}

//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;    //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;    //Clear Tx flag
        B_TX1_Busy = 0;
    }
}

/********************** I2C函数 ************************/
void I2C_init(void)
{
    P_SW2 = (P_SW2 & ~(3<<4)) | (3<<4); //IO口切换. 0: P1.4 P1.5, 1: P2.4 P2.5, 3: P3.3 P3.2
    I2CCFG = 0xe0;              //使能I2C主机模式
    I2CMSST = 0x00;
}

void WriteNbyte(u8 addr, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);  //检查I2C控制器忙碌状态

    DmaTxFlag = 1;
    DmaTxBuffer[0] = SLAW;
    DmaTxBuffer[1] = addr;

    I2CMSST = 0x00;
    I2CMSCR = 0x89;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = number+1;    //设置传输总字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2CT_AMTH = 0x00;       //设置传输总字节数(高8位)：n+1
    DMA_I2C_ST1 = number+1;     //设置需要传输字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2C_ST2 = 0x00;         //设置需要传输字节数(高8位)：n+1
    DMA_I2CT_CR |= 0x40;        //bit7 1:使能 I2CT_DMA, bit6 1:开始 I2CT_DMA

    while(DmaTxFlag);           //DMA忙检测
    DMA_I2C_CR = 0x00;
}

void ReadNbyte(u8 addr, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //检查I2C控制器忙碌状态
    DMA_I2C_CR = 0x00;
    I2CMSST = 0x00;

    //发送起始信号+设备地址+写信号
    I2CTXD = SLAW;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    //发送存储器地址
    I2CTXD = addr;
    I2CMSCR = 0x0a;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;
    
    //发送起始信号+设备地址+读信号
    I2CTXD = SLAR;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    DmaRxFlag = 1;
    //触发数据读取命令
    I2CMSCR = 0x8b;
    DMA_I2C_CR = 0x01;

    DMA_I2CR_AMT = number-1;    //设置传输总字节数(低8位)：n+1
    DMA_I2CR_AMTH = 0x00;       //设置传输总字节数(高8位)：n+1
    DMA_I2C_ST1 = number-1;     //设置需要传输字节数(低8位)：number + 设备地址 + 存储地址
    DMA_I2C_ST2 = 0x00;         //设置需要传输字节数(高8位)：n+1
    DMA_I2CR_CR |= 0x40;        //bit7 1:使能 I2CT_DMA, bit5 1:开始 I2CT_DMA, bit0 1:清除 FIFO
    while(DmaRxFlag);           //DMA忙检测
    DMA_I2C_CR = 0x00;
}

//========================================================================
// 函数: void I2C_DMA_Interrupt (void) interrupt 60/61
// 描述: I2C DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void I2C_DMA_Interrupt(void) interrupt 13
{
    if(DMA_I2CT_STA & 0x01)     //发送完成
    {
        DMA_I2CT_STA &= ~0x01;  //清除标志位
        DmaTxFlag = 0;
    }
    if(DMA_I2CT_STA & 0x04)     //数据覆盖
    {
        DMA_I2CT_STA &= ~0x04;  //清除标志位
    }

    if(DMA_I2CR_STA & 0x01)     //接收完成
    {
        DMA_I2CR_STA &= ~0x01;  //清除标志位
        DmaRxFlag = 0;
    }
    if(DMA_I2CR_STA & 0x02)     //数据丢弃
    {
        DMA_I2CR_STA &= ~0x02;  //清除标志位
    }
}

//========================================================================
// 函数: void I2C_Interrupt (void) interrupt 24
// 描述: I2C 中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-3-18
// 备注: 
//========================================================================
void I2C_Interrupt() interrupt 24
{
	I2CMSST &= ~0x40;       //I2C指令发送完成状态清除

	if(DMA_I2C_CR & 0x04)   //ACKERR
	{
		DMA_I2C_CR &= ~0x04;  //发数据后收到NAK
	}
}

