
#define 	MAIN_Fosc		40000000UL	//定义主时钟

#include	"AI8051U.h"


/*************	功能说明	**************

请先别修改程序, 直接下载"LCD1602.hex"测试, 下载时选择主频40MHz.

LCM1602字符液晶模块，8位数据，全部由LCM DMA驱动，节省CPU时间(LCD1602用IO方式访问速度很慢!)。

本程序在AI8051U实验箱上验证，使用3.3V的LCD1602模块。

*******************************************/


/*************  本地常量声明    **************/
const u8  text1[] = {" LCD1602 8-DATA "};
const u8  text2[] = {" AI8051U-34K64  "};
const u8  text3[] = {"2024-10-22 21:29"};
const u8  text4[] = {" WWW.STCAI.com  "};


/*************  本地变量声明    **************/
u16	LCM_TxCnt;		//LCM DMA触发发送次数, 一次16字节, 一共64次
bit	B_LCM_DMA_busy;	//LCM DMA忙标志， 1标志LCM-DMA忙，LCM DMA中断中清除此标志，使用LCM DMA前要确认此标志为0
bit	B_TxCmd;		//已发送命令标志
u8	xdata CmdTmp[2];	//命令缓冲
u8	xdata DisTmp1[16];	//第一行显示缓冲，将要显示的内容放在显存里，启动DMA即可.
u8	xdata DisTmp2[16];	//第二行显示缓冲，将要显示的内容放在显存里，启动DMA即可.


/*************  本地函数声明    **************/



/*************	Pin define	*****************************************************/
sfr		LCD_DATA = 0xa0;	//P0--0x80,  P1--0x90,  P2--0xa0,  P3--0xb0
sbit	LCD_B7  = P2^7;	//Pin 14	B3--Pin 10		LED+ -- Pin 15
						//Pin 13	B2--Pin 9		LED- -- Pin 16
						//Pin 12	B1--Pin 8		Vo   -- Pin 3
						//Pin 11	B0--Pin 7		VDD  -- Pin 2		VSS -- Pin 1

sbit	LCD_ENA	= P3^7;	//Pin 6
sbit	LCD_RW	= P3^6;	//Pin 5	//LCD_RS   R/W   DB7--DB0        FOUNCTION
sbit	LCD_RS	= P4^5;	//Pin 4	//	0		0	  INPUT      write the command to LCD model
								//	0		1     OUTPUT     read BF and AC pointer from LCD model
								//	1		0     INPUT      write the data to LCD  model
								//	1		1     OUTPUT     read the data from LCD model

/******************************************************************************
                 HD44780U    LCD_MODUL DRIVE PROGRAMME
*******************************************************************************

total 2 lines, 16x2= 32
first line address:  0~15
second line address: 64~79

total 2 lines, 20x2= 40
first line address:  0~19
second line address: 64~83

total 2 lines, 40x2= 80
first line address:  0~39
second line address: 64~103
*/

#define C_CLEAR			0x01		//clear LCD
#define C_HOME 			0x02		//cursor go home
#define C_CUR_L			0x04		//cursor shift left after input
#define C_RIGHT			0x05		//picture shift right after input
#define C_CUR_R			0x06		//cursor shift right after input
#define C_LEFT 			0x07		//picture shift left after input
#define C_OFF  			0x08		//turn off LCD
#define C_ON   			0x0C		//turn on  LCD
#define C_FLASH			0x0D		//turn on  LCD, flash
#define C_CURSOR		0x0E		//turn on  LCD and cursor
#define C_FLASH_ALL		0x0F		//turn on  LCD and cursor, flash
#define C_CURSOR_LEFT	0x10		//single cursor shift left
#define C_CURSOR_RIGHT	0x10		//single cursor shift right
#define C_PICTURE_LEFT	0x10		//single picture shift left
#define C_PICTURE_RIGHT	0x10		//single picture shift right
#define C_BIT8			0x30		//set the data is 8 bits
#define C_BIT4			0x20		//set the data is 8 bits
#define C_L1DOT7		0x30		//8 bits,one line 5*7  dots
#define C_L1DOT10		0x34		//8 bits,one line 5*10 dots
#define C_8bitL2DOT7	0x38		//8 bits,tow lines 5*7 dots
#define C_4bitL2DOT7	0x28		//4 bits,tow lines 5*7 dots
#define C_CGADDRESS0	0x40		//CGRAM address0 (addr=40H+x)
#define C_DDADDRESS0	0x80		//DDRAM address0 (addr=80H+x)


//========================================================================
// 函数: void  delay_ms(u16 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 1~65535ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注:
//========================================================================
void  delay_ms(u16 ms)
{
     u16 i;
	 do
	 {
	 	i = MAIN_Fosc / 6000;
		while(--i)	;
     }while(--ms);
}

void	LCD_DelayNop(void)
{
	NOP(27);	// 我测试的屏至少 调用返回6T+24个NOP @40MHz, 建议NOP至少个数=FOSC(MHz) * 0.75-6
}

void	CheckBusy(void)		//检测忙标志
{
	u16	i;
	LCD_RS = 0;		//读忙标志或AC指针
	LCD_RW = 1;
	LCD_DATA = 0xff;
	LCD_DelayNop();
	LCD_ENA = 1;
	for(i=0; i<10000; i++)		//忙检测 check the LCD busy or not. With time out. 我测试的屏至少6500@40MHz
	{
		if(!LCD_B7)	break;
	}
	LCD_ENA = 0;
}

/**********************************************/
void IniSendCMD(u8 cmd)		//write the command to LCD
{
	LCD_RS = 0;		//写命令
	LCD_RW = 0;
	LCD_DATA = cmd;
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_DATA = 0xff;
	LCD_DelayNop();
}

/**********************************************/
void Write_CMD(u8 cmd)		//write the command to LCD
{
	CheckBusy();	//检测忙标志  check the LCD busy or not.

	LCD_RS = 0;		//写命令
	LCD_RW = 0;
	LCD_DATA = cmd;
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_DATA = 0xff;
	LCD_DelayNop();
}


/*********	初始化函数	**************************/
void Initialize_LCD(void)		//intilize LCD, input none, output none
{
	P2n_standard(0xff);
	P3n_standard(Pin7+Pin6);
	P4n_standard(Pin5);
	delay_ms(1);

	LCD_ENA = 0;
	LCD_RS  = 0;
	LCD_RW  = 0;

	delay_ms(100);
	IniSendCMD(C_BIT8);		//set the data is 4 bits

	delay_ms(10);
	IniSendCMD(C_BIT8);		//set the data is 4 bits

	delay_ms(10);
	IniSendCMD(C_8bitL2DOT7);	//tow lines 5*7 dots

	delay_ms(6);
	Write_CMD(C_CLEAR);		//clear LCD RAM
	Write_CMD(C_CUR_R);		//Curror Shift Right
	Write_CMD(C_ON);		//turn on  LCD
}

//******************** LCD40 Module END ***************************

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
	LCMIFPSCR = (u8)(MAIN_Fosc/250000UL-1);	//TFT彩屏接口时钟预分频0~255, LCMIF时钟频率 = SYSclk/(LCMIFPSCR+1), LCD1602速度慢，最快250KHz
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

//DMA_LCM_CFG 	(7EfA70H)   LCM_DMA配置寄存器
#define		DMA_LCMIE	(1<<7)	// LCM DMA中断使能控制位，bit7, 0:禁止SPI DMA中断，     1：允许中断。
#define		DMA_LCMIP	(0<<2)	// LCM DMA中断优先级控制位，bit3~bit2, (最低)0~3(最高).
#define		DMA_LCMPTY	0		// LCM DMA数据总线访问优先级控制位，bit1~bit0, (最低)0~3(最高).

//DMA_LCM_STA  (7EfA72) 	LCM_DMA状态寄存器
#define		LCM_TXOVW	(1<<1)	// LCM DMA数据覆盖标志位，bit1, 软件清0.
#define		DMA_LCMIF	1		// LCM DMA中断请求标志位，bit0, 软件清0.

void LCM_DMA_Trig(void)
{
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
		if(LCM_TxCnt >= 2)	//判断发送是否完毕
		{
			DMA_LCM_CR = 0;
			B_LCM_DMA_busy = 0;		//清除LCM-DMA忙标志，LCM DMA中断中清除此标志，使用LCM DMA前要确认此标志为0
		}
		else		//仍有数据要发送
		{
			if(!B_TxCmd)	//还没有发设置地址命令，则先发设置地址命令
			{
				B_TxCmd = 1;	//指示已发地址命令
				if(LCM_TxCnt == 0)   //第一行地址设置命令
				{
					LCD_RS = 0;		//写命令
					LCD_RW = 0;
					CmdTmp[0] = 0x80;	//第一行AC地址
				}
				else if(LCM_TxCnt == 1)   //第二行地址设置命令
				{
					LCD_RS = 0;		//写命令
					LCD_RW = 0;
					CmdTmp[0] = 0x80 | 64;	//第二行AC地址
				}
				DMA_LCM_TXAH = (u8)((u16)CmdTmp >> 8);	//LCM DMA发送命令首地址
				DMA_LCM_TXAL = (u8)CmdTmp;
				DMA_LCM_AMTH = 0;			//设置传输总字节数(高8位),	设置传输总字节数 = N+1
				DMA_LCM_AMT  = 1-1;			//设置传输总字节数(低8位).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWC;	//启动LCM DMA发送命令
			}
			else	//写数据
			{
				LCD_RS = 1;		//写显示数据
				LCD_RW = 0;
				B_TxCmd = 0;	//清除已发地址命令
				if(LCM_TxCnt == 0)   //第一行显示缓冲地址
				{
					DMA_LCM_TXAH = (u8)((u16)DisTmp1 >> 8);	//LCM DMA发送数据首地址
					DMA_LCM_TXAL = (u8)DisTmp1;
				}
				else //if(LCM_TxCnt == 1)   //第二行显示缓冲地址
				{
					DMA_LCM_TXAH = (u8)((u16)DisTmp2 >> 8);	//LCM DMA发送数据首地址
					DMA_LCM_TXAL = (u8)DisTmp2;
				}
				DMA_LCM_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
				DMA_LCM_AMT  = 16-1;			//设置传输总字节数(低8位).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//启动LCM DMA发送数据
				LCM_TxCnt++;		//发送行数+1
			}
		}
	}
	DMA_LCM_STA = 0;
}


/*************** 主函数 *******************************/

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

	delay_ms(100);	//等待一下，等LCD讲入工作状态
	LCM_Config();
	Initialize_LCD();	//初始化LCD
	EA = 1;

	while (1)
	{
		if(!B_LCM_DMA_busy)		//DMA空闲
		{
			for(i=0; i<16; i++)	//将要显示的文本装载到显存
			{
				DisTmp1[i] = text1[i];	//加载第一行显存
				DisTmp2[i] = text2[i];	//加载第二行显存
			}
			LCM_DMA_Trig();		//启动DMA，显示文本
		}
		delay_ms(2000);

		if(!B_LCM_DMA_busy)		//DMA空闲
		{
			for(i=0; i<16; i++)	//将要显示的文本装载到显存
			{
				DisTmp1[i] = text3[i];	//加载第一行显存
				DisTmp2[i] = text4[i];	//加载第二行显存
			}
			LCM_DMA_Trig();		//启动DMA，显示文本
		}
		delay_ms(2000);
	}
}

