/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

USART类型Lin主机总线收发测试用例，默认通信脚：P3.6 P3.7.

按一下P32口按键, 主机发送完整一帧数据.
按一下P33口按键, 主机发送帧头并获取从机应答数据（合并成一串完整的帧）.

默认传输速率：9600波特率, 用户可自行修改.

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL
#define Baudrate1        9600UL
#define Timer0_Reload    (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒
#define TM               (65536UL -(MAIN_Fosc/Baudrate1/4))

/*****************************************************************************/

sbit SLP_N  = P4^7;     //0: Sleep

/*************  本地常量声明    **************/

#define FRAME_LEN           8    //数据长度: 8 字节
#define UART1_BUF_LENGTH    64

/*************  本地变量声明    **************/

u8 Lin_ID;
u8 TX_BUF[8];

bit B_1ms;          //1ms标志

bit B_TX1_Busy; //发送忙标志
u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
u8  RX1_TimeOut;

u8  Key1_cnt;
u8  Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;

u8  RX1_Buffer[UART1_BUF_LENGTH]; //接收缓冲

/*************  本地函数声明    **************/

void LinInit();
void LinSendMsg(u8 lid, u8 *pdat);
void LinSendHeader(u8 lid);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
	S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40;
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
}

void UartPutc(unsigned char dat)
{
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}

/********************* 主函数 *************************/
void main(void)
{
	u8 i;
	
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

	AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (u8)(Timer0_Reload / 256);
	TL0 = (u8)(Timer0_Reload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run

	Lin_ID = 0x32;
	UartInit();
	LinInit();
	EA = 1;               //打开总中断

	SLP_N = 1;
	TX_BUF[0] = 0x81;
	TX_BUF[1] = 0x22;
	TX_BUF[2] = 0x33;
	TX_BUF[3] = 0x44;
	TX_BUF[4] = 0x55;
	TX_BUF[5] = 0x66;
	TX_BUF[6] = 0x77;
	TX_BUF[7] = 0x88;
	
	while(1)
	{
		if(B_1ms)   //1ms到
		{
			B_1ms = 0;

			if(!P32)
			{
				if(!Key1_Flag)
				{
					Key1_cnt++;
					if(Key1_cnt > 50)
					{
						Key1_Flag = 1;
						LinSendMsg(Lin_ID, TX_BUF);  //发送一串完整数据
					}
				}
			}
			else
			{
				Key1_cnt = 0;
				Key1_Flag = 0;
			}

			if(!P33)
			{
				if(!Key2_Flag)
				{
					Key2_cnt++;
					if(Key2_cnt > 50)
					{
						Key2_Flag = 1;
						LinSendHeader(0x13);  //发送帧头，获取数据帧，组成一个完整的帧
					}
				}
			}
			else
			{
				Key2_cnt = 0;
				Key2_Flag = 0;
			}
			
			if(RX1_TimeOut > 0)     //超时计数
			{
				if(--RX1_TimeOut == 0)
				{
					for(i=0; i<RX1_Cnt; i++)    printf("0x%bx ",RX1_Buffer[i]);    //从串口输出收到的从机数据
					RX1_Cnt  = 0;   //清除字节数
					printf("\r\n");
				}
			}
		}
	}
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void LinSendByte(u8 dat)
// 描述: 发送一个字节函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinSendByte(u8 dat)
{
	B_TX1_Busy = 1;
	SBUF = dat;
	while(B_TX1_Busy);
}

//========================================================================
// 函数: u16 LinSendData(u8 *pdat)
// 描述: Lin发送数据函数。
// 参数: *pdat: 设置数据缓冲区.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinSendData(u8 *pdat)
{
	u8 i;

	for(i=0;i<FRAME_LEN;i++)
	{
		LinSendByte(pdat[i]);
	}
}

//========================================================================
// 函数: void SendBreak(void)
// 描述: 主模式发送Lin总线Break函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void SendBreak(void)
{
	USARTCR5 |= 0x04;		//主模式 Send Break
	LinSendByte(0x00);
}

//========================================================================
// 函数: void LinSendPID(u8 id)
// 描述: ID码加上校验符，转成PID码并发送。
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void LinSendPID(u8 id)
{
	u8 P0 ;
	u8 P1 ;
	
	P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
	P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
	
	LinSendByte(id|P0|P1);
}

//========================================================================
// 函数: void LINSendChecksum(u8 *dat)
// 描述: 计算校验码并发送。
// 参数: 数据场传输的数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void LINSendChecksum(u8 *dat)
{
    u16 sum = 0;
    u8 i;

    for(i = 0; i < FRAME_LEN; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
	LinSendByte((u8)sum);
}

//========================================================================
// 函数: void LinSendHeader(u8 lid)
// 描述: Lin主机发送帧头函数。
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinSendHeader(u8 lid)
{
	SendBreak();					//Send Break
	LinSendByte(0x55);				//Send Sync Field
	LinSendPID(lid);				//设置总线ID
}

//========================================================================
// 函数: void LinSendMsg(u8 lid, u8 *pdat)
// 描述: Lin主机发送完整帧函数。
// 参数: lid: Lin ID; *pdat: 发送数据缓冲区.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinSendMsg(u8 lid, u8 *pdat)
{
	SendBreak();					//Send Break
	LinSendByte(0x55);				//Send Sync Field

	LinSendPID(lid);				//设置总线ID
	LinSendData(pdat);
	LINSendChecksum(pdat);
}

//========================================================================
// 函数: void LinSetBaudrate(u16 brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinSetBaudrate(u16 brt)
{
	u16 tmp;
	
	tmp = (MAIN_Fosc >> 4) / brt;
	USARTBRH = (u8)(tmp>>8);
	USARTBRL = (u8)tmp;
}

//========================================================================
// 函数: void LinInit()
// 描述: Lin初始化函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void LinInit()
{
    P_SW1 = (P_SW1 & 0x3f) | 0x40;  //USART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

	SCON = (SCON & 0x3f) | 0x40;    //USART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
	TI = 0;
	REN = 1;    //允许接收
	ES  = 1;    //允许中断

	LinSetBaudrate(Baudrate1);		//设置波特率
	SMOD = 1;				//串口1波特率翻倍
	USARTCR1 |= 0x80;		//LIN Mode Enable
	USARTCR5 &= ~0x20;		//LIN Master Mode
	
//	LinSendMsg(Lin_ID, TX_BUF);  //Send data
}

//========================================================================
// 函数: void USART1_int (void) interrupt UART1_VECTOR
// 描述: USART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-15
// 备注: 
//========================================================================
void USART1_int (void) interrupt 4
{
	if(RI)
	{
		RI = 0;    //Clear Rx flag
		if(RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
		RX1_Buffer[RX1_Cnt] = SBUF;
		RX1_Cnt++;
		RX1_TimeOut = 5;
	}

	if(TI)
	{
		TI = 0;    //Clear Tx flag
		B_TX1_Busy = 0;
	}
}

