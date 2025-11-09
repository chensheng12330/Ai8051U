/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  本程序功能说明  **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过硬件I2C接口读取AT24C02前8个字节数据，通过串口打印读取结果.

将读取的数据加1后写回AT24C02前8个字节.

重新读取AT24C02前8个字节数据，通过串口打印读取结果.

MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

串口配置UART1(P3.0,P3.1): 115200,N,8,1.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

/***********************************************************/

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** 用户定义宏 ***********************************/

#define MAIN_Fosc       24000000L   //定义主时钟
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf 使用 UART1; 2:printf 使用 UART2
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/

sbit SDA = P3^3;
sbit SCL = P3^2;

/*************  本地常量声明    **************/

#define SLAW        0xA0
#define SLAR        0xA1

/*************  本地变量声明    **************/


/*************  本地函数声明    **************/
void WriteNbyte(u8 addr, u8 *p, u8 number);
void ReadNbyte( u8 addr, u8 *p, u8 number);
void delay_ms(u8 ms);
void UartInit(void);

/**********************************************/
void main(void)
{
    u8  i;
    u8  tmp[8];

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
    
    UartInit();

    I2C_S1 =1;      //I2C功能脚选择，00:P2.4,P2.3; 01:P1.5,P1.4; 11:P3.2,P3.3
    I2C_S0 =1;
    I2CCFG = 0xc2;  //使能I2C主机模式
    I2CPSCR = 0x00; //MSSPEED[13:6]
    I2CMSST = 0x00;

    EA = 1;         //打开总中断
    
    ReadNbyte(0, tmp, 8);
    printf("Read1 = ");     //打印第一次读取内容
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
        tmp[i]++;
    }
    printf("\r\n");

    WriteNbyte(0, tmp, 8);  //写入新的内容
    delay_ms(250);
    delay_ms(250);

    ReadNbyte(0, tmp, 8);
    printf("Read2 = ");     //打印第二次读取内容
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
    }
    printf("\r\n");

    while(1);
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

/********************** I2C函数 ************************/
void Wait()
{
    while (!(I2CMSST & 0x40));
    I2CMSST &= ~0x40;
}

void Start()
{
    I2CMSCR = 0x01;                         //发送START命令
    Wait();
}

void SendData(char dat)
{
    I2CTXD = dat;                           //写数据到数据缓冲区
    I2CMSCR = 0x02;                         //发送SEND命令
    Wait();
}

void RecvACK()
{
    I2CMSCR = 0x03;                         //发送读ACK命令
    Wait();
}

char RecvData()
{
    I2CMSCR = 0x04;                         //发送RECV命令
    Wait();
    return I2CRXD;
}

void SendACK()
{
    I2CMSST = 0x00;                         //设置ACK信号
    I2CMSCR = 0x05;                         //发送ACK命令
    Wait();
}

void SendNAK()
{
    I2CMSST = 0x01;                         //设置NAK信号
    I2CMSCR = 0x05;                         //发送ACK命令
    Wait();
}

void Stop()
{
    I2CMSCR = 0x06;                         //发送STOP命令
    Wait();
}

void WriteNbyte(u8 addr, u8 *p, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //发送起始命令
    SendData(SLAW);                         //发送设备地址+写命令
    RecvACK();
    SendData(addr);                         //发送存储地址
    RecvACK();
    do
    {
        SendData(*p++);
        RecvACK();
    }
    while(--number);
    Stop();                                 //发送停止命令
}

void ReadNbyte(u8 addr, u8 *p, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //发送起始命令
    SendData(SLAW);                         //发送设备地址+写命令
    RecvACK();
    SendData(addr);                         //发送存储地址
    RecvACK();
    Start();                                //发送起始命令
    SendData(SLAR);                         //发送设备地址+读命令
    RecvACK();
    do
    {
        *p = RecvData();
        p++;
        if(number != 1) SendACK();          //send ACK
    }
    while(--number);
    SendNAK();                              //send no ACK	
    Stop();                                 //发送停止命令
}

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;      //定时器时钟1T模式
	S1BRT = 0;      //串口1选择定时器1为波特率发生器
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;        //定时器1开始计时

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
	S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}
