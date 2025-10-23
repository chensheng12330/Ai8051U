
/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱V1.2版本进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

使用TFT彩屏接口+DMA将正弦波数据输出P2口接R-2R做DAC输出正弦波(或任意波形)。
本例为32点正弦波，此法将极大程序减少对CPU的占用，中断频率为输出波形的频率，比如输出2000Hz的正弦波，如果用传统的中断重装方式，将需要64KHz(15.625us)的中断，严重拖累CPU。
而使用本方法，中断率为2000Hz，并且中断仅仅是重启DMA。
注意：重启DMA需要大约1us的时间，会附加到输出波形中，引起失真，当波形小于5000Hz时，影响可以忽略，大于10KHz时，影响开始能检测到。

******************************************/

	#define MAIN_Fosc        40000000UL

	#include "AI8051U.h"


/****************************** 用户定义宏 ***********************************/
	sbit    LCD_RS = P4^5;  //悬空不用
	sbit    LCD_RW = P3^6;  //悬空不用
	sbit    LCD_E  = P3^7;	//悬空不用
	sbit    LCD_RESET   =   P4^7;   //悬空
	#define LCD_Data P2


/*****************************************************************************/

/*************  本地常量声明    **************/


/*************  本地变量声明    **************/

u8	xdata T_SINE[32]={
128,
153,
177,
199,
218,
234,
245,
253,
255,
253,
245,
234,
218,
199,
177,
153,
128,
103,
79,
57,
38,
22,
11,
3,
1,
3,
11,
22,
38,
57,
79,
103,
};


/*************  本地函数声明    **************/
void 	LCM_Config(void);
void 	DMA_Config(u8 xdata *wav);

/********************* 主函数 *************************/
void main(void)
{
	EAXFR = 1;	//扩展寄存器(XFR)访问使能
	WTST  = 0;	//设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
	CKCON = 0;	//提高访问XRAM速度

	P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
	P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
	P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
	P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
	P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
	P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
	P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
	P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

	LCM_Config();
	EA = 1;

	DMA_Config(T_SINE);
	P40 = 0;		//接R-2R的地(这样接方便，但接GND更好).
	P2n_pure_input(0xff);		// P2要设置为高阻，否则IO输出电平会干扰数据，即LCM不控制IO时回复IO的输出。
	BypassEnable(P2BP, 0xff);	// 允许外设自动控制端口   PxBP, 要设置的端口对应位为1,   外设自动控制P0口, 使用外部总线时，如果P2BP=0x00则P2由硬件控制，P2的模式设置不影响总线，如果P2BP=0xff则P2设置会影响总线访问。
	BypassEnable(P3BP, 0xc0);	// 允许外设自动控制端口   PxBP, 要设置的端口对应位为1,   外设自动控制P0口, 使用外部总线时，如果P2BP=0x00则P2由硬件控制，P2的模式设置不影响总线，如果P2BP=0xff则P2设置会影响总线访问。
	BypassEnable(P4BP, 0xa0);	// 允许外设自动控制端口   PxBP, 要设置的端口对应位为1,   外设自动控制P0口, 使用外部总线时，如果P2BP=0x00则P2由硬件控制，P2的模式设置不影响总线，如果P2BP=0xff则P2设置会影响总线访问。

    while(1)
    {
		NOP(10);

    }
}



//========================================================================
// 函数: void LCM_Config(void)
// 描述: LCM配置函数。 LCM DMA发送周期：LCMIFPSCR=0(1分频)时，=(1+E+K+S)T+3Ts，LCMIFPSCR!=0(>=2分频)时, =(1+E+K+S)T+4Ts, T为LCMIF时钟周期, Ts为系统时钟周期, E为数据建立时间，K为数据保持时间，S为DMA间隔时间。
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-8-17
// 备注:
//========================================================================
//LCMIFCFG  (7EFE50H)	TFT彩屏接口配置寄存器
#define	LCMIFIE			(0<<7)	//TFT彩屏接口中断使能控制位(bit7), 1->允许中断，0->禁止
#define	LCMIFIP			(0<<4)	//TFT彩屏接口中断优先级控制位(bit5~bit4), (最低)0~3(最高).
#define	LCMIFDPS		(0<<2)	//TFT彩屏接口数据脚选择位(bit3~bit2), D18_D8=0: 0或2->8位数据在P2, D18_D8=1: 0: P2-高字节, P0低字节, 2: P2-高字节,低字节P0[7:4] P4.7 P4.6 P4.3 P4.1.
#define	LCM_D16_D8		(0<<1)	//TFT彩屏接口数据宽度控制位(bit1), 0: 8位数据，1:16位数据
#define	LCM_M68_I80		1		//TFT彩屏接口数据模式选择位(bit0), 0: I8080模式，1:M6800模式

//LCMIFCFG2  (7EFE51H)	TFT彩屏接口配置寄存器2
#define	LCMIFCPS		(1<<5)	//TFT彩屏接口控制脚选择位(bit6~bit5), RS RD(E) WR(RW), 0->P4.5 P4.4 P4.2，1->P4.5 P3.7 P3.6, 2->P4.0 P4.4 P4.2, 3->P4.0 P3.7 P3.6
#define	LCMSETUPT		(7<<2)	//TFT彩屏接口数据建立时间控制位(bit4~bit2), 0~7, 对应1~8个LCMIF时钟, 即E信号高电平时间.
#define	LCMHOLDT		3		//TFT彩屏接口数据保持时间控制位(bit1~bit0), 0~3, 对应1~4个LCMIF时钟

//LCMIFCR  (7EFE52H)	TFT彩屏接口控制寄存器
#define	ENLCMIF			(1<<7)	//TFT彩屏接口使能控制位(bit7), 1->允许TFT彩屏接口功能, 0->禁止
#define	LCM_WRCMD		4		//TFT彩屏接口触发命令(bit2~bit0), 4->写命令
#define	LCM_WRDAT		5		//TFT彩屏接口触发命令(bit2~bit0), 5->写数据
#define	LCM_RDCMD		6		//TFT彩屏接口触发命令(bit2~bit0), 6->读命令/状态
#define	LCM_RDDAT		7		//TFT彩屏接口触发命令(bit2~bit0), 7->读数据

void LCM_Config(void)
{
	LCMIFCFG  = LCMIFIE  | LCMIFIP   | LCMIFDPS | LCM_D16_D8 | LCM_M68_I80;
	LCMIFCFG2 = LCMIFCPS | LCMSETUPT | LCMHOLDT;	//RS:P45,E:P37,RW:P36; Setup Time,HOLD Time
	LCMIFSTA  = 0x00;	//TFT彩屏接口中断请求标志，软件清0
	LCMIFPSCR = 1-1;	//TFT彩屏接口时钟预分频0~255, LCMIF时钟频率 = SYSclk/(LCMIFPSCR+1), LCD12864速度慢，最快250KHz
	LCMIFCR   = ENLCMIF;
}

//========================================================================
// 函数: void DMA_Config(u8 xdata *wav)
// 描述: LCM DMA配置函数。
// 参数: *wav: 数据首地址.
// 返回: none.
// 版本: V1.0, 2024-8-17
// 备注:
//========================================================================
//DMA_LCM_CR (7EfA71H) 	LCM_DMA控制寄存器
#define		DMA_ENLCM	(1<<7)	// LCM DMA功能使能控制位，    bit7, 0:禁止LCM DMA功能，  1：允许LCM DMA功能。
#define		LCM_TRIGWC	(1<<6)	// LCM DMA触发写命令，bit6, 0:写0无效，          1：写1开始LCM DMA开始写命令。
#define		LCM_TRIGWD	(1<<5)	// LCM DMA触发写数据，bit5, 0:写0无效，          1：写1开始LCM DMA开始写数据。
#define		LCM_TRIGRC	(1<<4)	// LCM DMA触发读命令，bit4, 0:写0无效，          1：写1开始LCM DMA开始读命令。
#define		LCM_TRIGRD	(1<<3)	// LCM DMA触发读数据，bit3, 0:写0无效，          1：写1开始LCM DMA开始读数据。
#define		LCM_CLRFIFO	0		// 清除LCM DMA接收FIFO控制位，bit0, 0:写0无效，  1：写1复位FIFO指针。

//DMA_LCM_CFG 	(7EfA70H)   LCM_DMA配置寄存器
#define		DMA_LCMIE	(1<<7)	// LCM DMA中断使能控制位，bit7, 0:禁止SPI DMA中断，     1：允许中断。
#define		DMA_LCMIP	(0<<2)	// LCM DMA中断优先级控制位，bit3~bit2, (最低)0~3(最高).
#define		DMA_LCMPTY	0		// LCM DMA数据总线访问优先级控制位，bit1~bit0, (最低)0~3(最高).

//DMA_LCM_STA  (7EfA72) 	LCM_DMA状态寄存器
#define		LCM_TXOVW	(1<<1)	// LCM DMA数据覆盖标志位，bit1, 软件清0.
#define		DMA_LCMIF	1		// LCM DMA中断请求标志位，bit0, 软件清0.

	#define	LCM_ITVH	1233	//输出1000Hz正弦波@40MHz, 32点正弦波表，LCM耗时17T，则间隔时间=40000/32-17=1233
//	#define	LCM_ITVH	608		//输出2000Hz正弦波@40MHz, 32点正弦波表，LCM耗时17T，则间隔时间=40000/64-17=608
//	#define	LCM_ITVH	233		//输出5000Hz正弦波@40MHz, 32点正弦波表，LCM耗时17T，则间隔时间=40000/160-17=233
//	#define	LCM_ITVH	108		//输出 10KHz正弦波@40MHz, 32点正弦波表，LCM耗时17T，则间隔时间=40000/320-17=108

void DMA_Config(u8 xdata *wav)
{
	u16	LCM_TxAddr;
	LCM_TxAddr   = (u16)wav;		//要发送数据的首地址
	DMA_LCM_TXAH = (u8)(LCM_TxAddr >> 8);	//LCM DMA发送数据首地址
	DMA_LCM_TXAL = (u8)LCM_TxAddr;
	DMA_LCM_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
	DMA_LCM_AMT  = 32-1;			//设置传输总字节数(低8位).
	DMA_LCM_STA  = 0x00;
	DMA_LCM_CFG  = DMA_LCMIE | DMA_LCMIP | DMA_LCMPTY;;
	DMA_LCM_ITVH = (u8)((LCM_ITVH-1)/256);	//设置传输间隔时间(高8位)，对应N+1个LCMIF时钟(1~65536个LCMIF时钟)
	DMA_LCM_ITVL = (u8)((LCM_ITVH-1)%256);	//设置传输间隔时间(低8位)
	DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//启动LCM DMA发送数据
}

//========================================================================
// 函数: void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
// 描述: LCM DMA中断函数。
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-8-17
// 备注:
//========================================================================
void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
{
	DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//启动DAM
	DMA_LCM_STA = 0;
}

