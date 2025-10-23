/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱V1.1版本进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

128*64的LCD显示程序，实验箱MCU用3.3V供电，LCD背光需要接5V(R175连接,R176断开).

显示图形，汉字，英文，数字

其中图形显示发送命令和数据均使用DMA操作，本例运行于24MHz, 每次LCM DMA中断处理占用CPU时间为2.3us, 传输总时间84ms.

将要显示的内容放在1024字节的显存中，启动DMA传输即可。

下载时, 选择时钟 24MHz (用户可自行修改频率后重新编译即可).

******************************************/

//	#define MAIN_Fosc        12000000UL
	#define MAIN_Fosc        24000000UL
//	#define MAIN_Fosc        40000000UL

	#include	"AI8051U.h"

	#include	<stdio.h>
	#include	<intrins.h>
	#include	"picture1.h"
	#include	"picture2.h"

/****************************** IO定义 ***********************************/
	sbit	LCD_RS	= P4^5;
	sbit	LCD_RW	= P3^6;
	sbit	LCD_E	= P3^7;
	sbit	LCD_RST	= P4^7;
	#define	LCD_Data	P2

/*****************************************************************************/

/*************  本地常量声明    **************/
const u8  uctech[] = {"LCD12864图文显示"};
const u8  net[]    = {"驱\xfd动芯片：ST7920"};
const u8  mcu[]    = {"  内带汉字字库  "};
const u8  qq[]     = {" AI8051U LQFP48 "};

/*************  本地变量声明    **************/
u16	LCM_TxCnt;		//LCM DMA触发发送次数, 一次16字节, 一共64次
bit	B_LCM_DMA_busy;	//LCM DMA忙标志， 1标志LCM-DMA忙，LCM DMA中断中清除此标志，使用LCM DMA前要确认此标志为0
u16	LCM_TxAddr;		//LCM DMA要发送数据的首地址
bit	B_TxCmd;		//已发送命令标志
u8	xdata CmdTmp[2];	//命令缓冲
u8	xdata DisTmp[1024] _at_ 0x0000;	//显示缓冲，将要显示的内容放在显存里，启动DMA即可. 由于LCM DMA有4字节对齐问题，所以这里定位对地址为4的倍数

/*************  本地函数声明    **************/
void    delay_ms(u16 ms);
void    WriteDataLCD(u8 WDLCD);
void    WriteCommandLCD(u8 WCLCD);
void    ReadStatusLCD(void);
void    LCDInit(void);
void    LCDClear(void);
void    DisplayListChar(u8 X, u8 Y, const u8 *DData);
void    DisplayImage (u8 xdata *DData);
void 	LCM_Config(void);
void 	DMA_Config(u8 xdata *pic);

/********************* 主函数 *************************/
void main(void)
{
	u16	i;

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
	P3n_push_pull(Pin7+Pin6);	//P3.7 P3.6设置为推挽输出，因为跟双串口IC连接了

	PullUpEnable(P2PU, 0xff);	// 允许端口内部上拉电阻       PxPU, 要设置的端口对应位为1
	PullUpEnable(P3PU, 0xc0);	// 允许端口内部上拉电阻       PxPU, 要设置的端口对应位为1
	PullUpEnable(P4PU, 0xa0);	// 允许端口内部上拉电阻       PxPU, 要设置的端口对应位为1

	delay_ms(100);	//启动等待，等LCD讲入工作状态
	LCM_Config();
	delay_ms(10);
	LCDInit();		//LCM初始化
	delay_ms(10);	//延时片刻
	EA = 1;

    while(1)
    {
		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture1[i];	//将图片装载到显存
		LCDClear();
		DMA_Config(DisTmp);	//启动DMA，显示图形
		delay_ms(3000);

		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture2[i];	//将图片装载到显存
		LCDClear();
		DMA_Config(DisTmp);	//启动DMA，显示图形
		delay_ms(3000);

		LCDClear();
		DisplayListChar(0,1,uctech);    //显示字库中的中文数字
		DisplayListChar(0,2,net);       //显示字库中的中文数字
		DisplayListChar(0,3,mcu);       //显示字库中的中文
		DisplayListChar(0,4,qq);        //显示字库中的中文数字
		delay_ms(3000);
    }
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注:
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do
    {
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

//========================================================================
// 函数: void LCM_Config(void)
// 描述: LCM配置函数。ST7920 @540KHz，清屏指令典型时间为1.6ms，读忙信号0us, 写入、读出数据、命令72us. 本例子DMA刷新时间84ms @24MHz.
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
#define	LCMSETUPT		(4<<2)	//TFT彩屏接口数据建立时间控制位(bit4~bit2), 0~7, 对应1~8个LCMIF时钟, 对于6800模式就是E高电平时间, 至少20us.
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
	LCMIFPSCR = (u8)(MAIN_Fosc/250000UL-1);	//TFT彩屏接口时钟预分频0~255, LCMIF时钟频率 = SYSclk/(LCMIFPSCR+1), LCD12864速度慢，最快250KHz
	LCMIFCR   = ENLCMIF;
}

//========================================================================
// 函数: void DMA_Config(u8 xdata *pic)
// 描述: LCM DMA配置函数。
// 参数: *pic: 数据首地址.
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

//DMA_LCM_CFG 	(7EfA70H)   SPI_DMA配置寄存器
#define		DMA_LCMIE	(1<<7)	// LCM DMA中断使能控制位，bit7, 0:禁止SPI DMA中断，     1：允许中断。
#define		DMA_LCMIP	(0<<2)	// LCM DMA中断优先级控制位，bit3~bit2, (最低)0~3(最高).
#define		DMA_LCMPTY	0		// LCM DMA数据总线访问优先级控制位，bit1~bit0, (最低)0~3(最高).

//DMA_LCM_STA  (7EfA72) 	LCM_DMA状态寄存器
#define		LCM_TXOVW	(1<<1)	// LCM DMA数据覆盖标志位，bit1, 软件清0.
#define		DMA_LCMIF	1		// LCM DMA中断请求标志位，bit0, 软件清0.

void DMA_Config(u8 xdata *pic)
{
	WriteCommandLCD(0x36);			//选择扩充指令集, 显示图形

	LCM_TxAddr   = (u16)pic;		//要发送数据的首地址
	DMA_LCM_STA  = 0x00;
	DMA_LCM_CFG  = DMA_LCMIE | DMA_LCMIP | DMA_LCMPTY;;
	DMA_LCM_ITVH = 0;	//设置传输间隔时间(高8位)，对应N+1个LCMIF时钟(1~65536个LCMIF时钟)
	DMA_LCM_ITVL = 9-1;	//设置传输间隔时间(低8位)

	B_TxCmd   = 0;		//已发送命令标志
	LCM_TxCnt = 0;		//LCM DMA触发发送次数, 一次16字节, 一共64次
	B_LCM_DMA_busy = 1;	//标志LCM-DMA忙，LCM DMA中断中清除此标志，使用LCM DMA前要确认此标志为0
	DMA_LCM_STA = DMA_LCMIF;	//软件触发LCM DMA中断，启动发送
}

//========================================================================
// 函数: void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
// 描述: LCM DMA中断函数。中断处理时间:2.3us @24MHz. 刷新时间：84ms @24MHz.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-8-17
// 备注:
//========================================================================
void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
{
	if(DMA_LCM_STA & DMA_LCMIF)
	{
		if(LCM_TxCnt >= 64)	//判断发送是否完毕
		{
			DMA_LCM_CR = 0;
			B_LCM_DMA_busy = 0;		//清除LCM-DMA忙标志，LCM DMA中断中清除此标志，使用LCM DMA前要确认此标志为0
		}
		else		//仍有数据要发送
		{
			if(!B_TxCmd)	//还没有发设置地址命令，则先发设置地址命令
			{
				B_TxCmd = 1;	//指示已发地址命令
				if(LCM_TxCnt <32)   //上半屏
				{
					CmdTmp[0] = (u8)(0x80+LCM_TxCnt);	//列地址
					CmdTmp[1] = 0x80;					//行地址，上半屏地址0X80
				}
				else   //下半屏
				{
					CmdTmp[0] = (u8)(0x80+LCM_TxCnt-32);	//列地址
					CmdTmp[1] = 0x88;						//行地址，下半屏地址0X88
				}
				DMA_LCM_TXAH = (u8)((u16)CmdTmp >> 8);	//LCM DMA发送命令首地址
				DMA_LCM_TXAL = (u8)CmdTmp;
				DMA_LCM_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
				DMA_LCM_AMT  = 2-1;				//设置传输总字节数(低8位).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWC;	//启动LCM DMA发送命令
			}
			else
			{
				B_TxCmd = 0;	//清除已发地址命令
				DMA_LCM_TXAH = (u8)(LCM_TxAddr >> 8);	//LCM DMA发送数据首地址
				DMA_LCM_TXAL = (u8)LCM_TxAddr;
				DMA_LCM_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
				DMA_LCM_AMT  = 16-1;			//设置传输总字节数(低8位).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//启动LCM DMA发送数据
				LCM_TxAddr  += 16;	//要发送数据的首地址, 一次DMA传输16字节
				LCM_TxCnt++;		//发送次数+1
			}
		}
	}
	DMA_LCM_STA = 0;
}

//读状态
void ReadStatusLCD(void)
{
//	LCD_Data = 0xFF;
	LCMIFSTA = 0;	//清除完成标志
    do
    {
		LCMIFCR   = ENLCMIF | LCM_RDCMD;	//读状态
		while((LCMIFSTA & 1) == 0)	;	//等待读完成
		LCMIFSTA = 0;	//清除完成标志
		LCD_E = 0;
    }while(LCMIFDATL & 0x80);	//bit7==0则空闲
}

//写数据
void WriteDataLCD(u8 WDLCD)
{
	ReadStatusLCD(); //检测忙
	LCMIFDATL = WDLCD;
	LCMIFCR   = ENLCMIF | LCM_WRDAT;	//写数据
	while((LCMIFSTA & 1) == 0)	;	//等待写完成
	LCMIFSTA = 0;	//清除完成标志
}

//写指令
void WriteCommandLCD(u8 WCLCD)
{
	ReadStatusLCD(); //检测忙
	LCMIFDATL = WCLCD;
	LCMIFCR   = ENLCMIF | LCM_WRCMD;	//写命令
	while((LCMIFSTA & 1) == 0)	;	//等待写完成
	LCMIFSTA = 0;	//清除完成标志
}

void LCDInit(void) //LCM初始化
{
	delay_ms(10);
	LCD_RST = 0;
	delay_ms(10);
	LCD_RST = 1;
	delay_ms(100);	//至少40ms

	WriteCommandLCD(0x30);	//显示模式设置,开始要求每次检测忙信号
	WriteCommandLCD(0x01);	//显示清屏
	WriteCommandLCD(0x04);	//反白显示设置，不反白
	WriteCommandLCD(0x0C);	//不睡眠
}

void LCDClear(void) //清屏
{
	WriteCommandLCD(0x30); //选择基本指令集
	WriteCommandLCD(0x01); //显示清屏, 忙信号长达1.2ms
}

//按指定位置显示一串字符
void DisplayListChar(u8 X, u8 Y, const u8 *DData)
{
	u8 ListLength,X2;
	X2 = X;
	if(Y < 1)   Y=1;
	if(Y > 4)   Y=4;
	X &= 0x0F; //限制X不能大于16，Y在1-4之内
	WriteCommandLCD(0x30);		//选择基本指令集
	switch(Y)
	{
		case 1: X2 |= 0X80; break;  //根据行数来选择相应地址
		case 2: X2 |= 0X90; break;
		case 3: X2 |= 0X88; break;
		case 4: X2 |= 0X98; break;
	}
	WriteCommandLCD(X2); //发送地址码
	ListLength = 0;
	while (DData[ListLength] >= 0x20) //若到达字串尾则退出
	{
		if (X <= 0x0F) //X坐标应小于0xF
		{
			WriteDataLCD(DData[ListLength]); //
			ListLength++;
			X++;
		}
	}
}
