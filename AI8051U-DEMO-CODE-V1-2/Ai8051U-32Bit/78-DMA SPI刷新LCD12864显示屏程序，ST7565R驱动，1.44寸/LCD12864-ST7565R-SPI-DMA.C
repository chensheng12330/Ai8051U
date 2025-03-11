
/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱V1.1版本进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

黑白点阵屏OLED12864驱动程序，驱动IC为ST7565R，SPI接口，通过SPI DMA将1024字节的图片数据送到彩屏，传送时不占用CPU时间。

显示图形，汉字，英文，数字.
驱动芯片: ST7565R, ST7567A, UC1701(注意: UC1701的对比度设置不同).

其中图形显示发送命令和图片数据使用SPI DMA操作，传输数据时不占用CPU时间。做GUI最方便了，可以先操作定义于xdata的1024字节缓存，然后触发SPI DMA即可，523us或943us即可自己动刷完。
本例运行于40MHz, SPI速度为主频4分频(10MHz)，每次SPI DMA传输总时间943us，SPI速度为主频2分频(20MHz)，每次SPI DMA传输总时间523us。

将要显示的内容放在1024字节的显存中，启动DMA传输即可。

下载时, 选择时钟 40MHz (用户可自行修改频率后重新编译即可).

******************************************/

	#define	MAIN_Fosc	40000000UL


	#include	"AI8051U.h"
	#include	"ASCII6x8.h"
	#include	"HZK16.h"
	#include	"ASCII-10x24.h"


/****************************** IO定义 ***********************************/
						// AI8051U实验箱 V1.1,
sbit P_LCD_CS   = P3^5;	// 1----CS		使能信号, L有效
sbit P_LCD_RST  = P1^1;	// 2----RST		复位信号, L复位, H正常工作
sbit P_LCD_RS   = P4^7;	// 3---RS(或A0)	数据/指令选择, H选择数据, L选择指令
sbit P_LCD_SDA  = P3^3;	// 4----SDA		串行数据			9----ROM_CS			注意UC1701的对比度调整
sbit P_LCD_SCLK = P3^2;	// 5----SCK		串行时钟			10---ROM_SCK


/*****************************************************************************/

/*************  本地常量声明    **************/


/*************  本地变量声明    **************/
u16	SPI_TxCnt;		//SPI DMA触发发送次数, 一次128字节, 一共8次
bit	B_SPI_DMA_busy;	//SPI DMA忙标志， 1标志SPI-DMA忙，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
u16	SPI_TxAddr;		//SPI DMA要发送数据的首地址
bit	B_TxCmd;		//已发送命令标志
u8 xdata CmdTmp[5];	//命令缓冲


/*************  本地函数声明    **************/



//========================================================================
// 函数: void  SPI_Config(u8 SPI_io, u8 SPI_speed)
// 描述: SPI初始化函数。
// 参数: io: 切换到的IO,            SS  MOSI MISO SCLK
//                       0: 切换到 P1.4 P1.5 P1.6 P1.7
//                       1: 切换到 P2.4 P2.5 P2.6 P2.7
//                       2: 切换到 P4.0 P4.1 P4.2 P4.3
//                       3: 切换到 P3.5 P3.4 P3.3 P3.2
//       SPI_speed: SPI的速度, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
// 返回: none.
// 版本: VER1.0
// 日期: 2024-8-13
// 备注:
//========================================================================
void  SPI_Config(u8 SPI_io, u8 SPI_speed)
{
	SPI_io &= 3;

	SPCTL = SPI_speed & 3;	//配置SPI 速度, 这条指令先执行, 顺便Bit7~Bit2清0
	SSIG = 1;	//1: 忽略SS脚，由MSTR位决定主机还是从机		0: SS脚用于决定主机还是从机。
	SPEN = 1;	//1: 允许SPI，								0：禁止SPI，所有SPI管脚均为普通IO
	DORD = 0;	//1：LSB先发，								0：MSB先发
	MSTR = 1;	//1：设为主机								0：设为从机
	CPOL = 1;	//1: 空闲时SCLK为高电平，					0：空闲时SCLK为低电平
	CPHA = 1;	//1: 数据在SCLK前沿驱动,后沿采样.			0: 数据在SCLK前沿采样,后沿驱动.
//	SPR1 = 0;	//SPR1,SPR0   00: fosc/4,     01: fosc/8
//	SPR0 = 0;	//            10: fosc/16,    11: fosc/2
	P_SW1 = (P_SW1 & ~0x0c) | ((SPI_io<<2) & 0x0c);		//切换IO

	HSCLKDIV   = 1;					//HSCLKDIV主时钟分频
	SPI_CLKDIV = 1;					//SPI_CLKDIV主时钟分频
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志

	if(SPI_io == 0)
	{
		P1n_standard(0xf0);			//切换到 P1.4(SS) P1.5(MOSI) P1.6(MISO) P1.7(SCLK), 设置为准双向口
		PullUpEnable(P1PU, 0xf0);	//设置上拉电阻    允许端口内部上拉电阻   PxPU, 要设置的端口对应位为1
		P1n_push_pull(Pin7+Pin5);	//MOSI SCLK设置为推挽输出
		SlewRateHigh(P1SR, Pin7+Pin5);	//MOSI SCLK端口输出设置为高速模式   PxSR, 要设置的端口对应位为1.    高速模式在3.3V供电时速度可以到13.5MHz(27MHz主频，SPI速度2分频)
	}
	else if(SPI_io == 1)
	{
		P2n_standard(0xf0);			//切换到P2.4(SS) P2.5(MOSI) P2.6(MISO) P2.7(SCLK), 设置为准双向口
		PullUpEnable(P2PU, 0xf0);	//设置上拉电阻    允许端口内部上拉电阻   PxPU, 要设置的端口对应位为1
		P2n_push_pull(Pin7+Pin5);	//MOSI SCLK设置为推挽输出
		SlewRateHigh(P2SR, Pin7+Pin5);	//MOSI SCLK端口输出设置为高速模式   PxSR, 要设置的端口对应位为1.    高速模式在3.3V供电时速度可以到13.5MHz(27MHz主频，SPI速度2分频)
	}
	else if(SPI_io == 2)
	{
		P4n_standard(0x0f);			//切换到P4.0(SS) P4.1(MOSI) P4.2(MISO) P4.3(SCLK), 设置为准双向口
		PullUpEnable(P4PU, 0x0f);	//设置上拉电阻    允许端口内部上拉电阻   PxPU, 要设置的端口对应位为1
		P4n_push_pull(Pin3+Pin1);	//MOSI SCLK设置为推挽输出
		SlewRateHigh(P4SR, Pin3+Pin1);	//MOSI SCLK端口输出设置为高速模式   PxSR, 要设置的端口对应位为1.    高速模式在3.3V供电时速度可以到13.5MHz(27MHz主频，SPI速度2分频)
	}
	else if(SPI_io == 3)
	{
		P3n_standard(0x3C);		//切换到P3.5(SS) P3.4(MOSI) P3.3(MISO) P3.2(SCLK), 设置为准双向口
		PullUpEnable(P3PU, 0x3c);	//设置上拉电阻    允许端口内部上拉电阻   PxPU, 要设置的端口对应位为1
		P3n_push_pull(Pin4+Pin2);	//MOSI SCLK设置为推挽输出
		SlewRateHigh(P3SR, Pin4+Pin2);	//MOSI SCLK端口输出设置为高速模式   PxSR, 要设置的端口对应位为1.    高速模式在3.3V供电时速度可以到13.5MHz(27MHz主频，SPI速度2分频)
	}
}


void  LCD_delay_ms(u16 ms)	// 1~65535
{
	u16 i;
	do
	{
		i = MAIN_Fosc / 6000;
		while(--i)	;
	}while(--ms);
}


void LCD_WriteData(u8 dat)
{
	P_LCD_CS = 0;
	P_LCD_RS = 1;	//写数据
	SPDAT = dat;	//发送一个字节
	while(SPIF == 0)	;			//等待发送完成
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志
	P_LCD_CS = 1;
}


void	LCD_WriteCMD(u8 dat)
{
	P_LCD_CS = 0;
	P_LCD_RS = 0;	//写命令
	SPDAT = dat;	//发送一个字节
	while(SPIF == 0)	;			//等待发送完成
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志
	P_LCD_CS = 1;
}


void Set_Dot_Addr(u8 x, u8 y)	// x为横向的点0~127, y为纵向的页0~7
{
	LCD_WriteCMD((u8)(0xb0 + y));
	LCD_WriteCMD((u8)((x>>4) | 0x10)); //设置列地址的高4 位
	LCD_WriteCMD(x & 0x0f);		//设置列地址的低4 位
}

//******************************************
void FillPage(u8 y, u8 color)			//Clear Page LCD RAM, y为页码 0~7
{
	u8 j;
	Set_Dot_Addr(0,y);
	for(j=0; j<128; j++)	LCD_WriteData(color);
}

//******************************************
void FillAll(u8 color)			//Clear CSn LCD RAM, color为填充的颜色
{
	u8 i;
	for(i=0; i<8; i++)	FillPage(i, color);
}


/*LCD 模块初始化*/
void	LCD12864_config(void)
{
	SPI_Config(3, 0);	//(SPI_io, SPI_speed), 参数: 	SPI_io: 切换IO(SS MOSI MISO SCLK), 0: 切换到P1.4 P1.5 P1.6 P1.7,  1: 切换到P2.4 P2.5 P2.6 P2.7, 2: 切换到P4.0 P4.1 P4.2 P4.3,  3: 切换到P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI的速度, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
	HSSPI_CFG2 = 0x40;	//交换MOSI MISO, P3.3是MOSI

	P1n_standard(Pin1);			// SPI引脚设置为准双向口, SPI和控制信号
	PullUpEnable(P1PU, Pin1);	// 允许端口内部上拉电阻     PxPU, 要设置的端口对应位为1
	P3n_standard(0x2c);			// SPI引脚设置为准双向口, SPI和控制信号
	PullUpEnable(P3PU, 0x2c);	// 允许端口内部上拉电阻     PxPU, 要设置的端口对应位为1
	P4n_standard(Pin7);			// SPI引脚设置为准双向口, SPI和控制信号
	PullUpEnable(P4PU, Pin7);	// 允许端口内部上拉电阻     PxPU, 要设置的端口对应位为1

	P_LCD_CS   = 1;
	P_LCD_RST  = 1;
	P_LCD_RS   = 1;
	P_LCD_SDA  = 1;
	P_LCD_SCLK = 1;

	LCD_delay_ms(20);
	P_LCD_CS  = 0;
	P_LCD_RST = 0;		// 低电平复位
	LCD_delay_ms(20);
	P_LCD_RST = 1;		// 复位完毕
	LCD_delay_ms(20);
	LCD_WriteCMD(0xe2);	// 软复位
	LCD_delay_ms(50);
	LCD_WriteCMD(0x2c);	// 升压步聚1
	LCD_delay_ms(50);
	LCD_WriteCMD(0x2e); // 升压步聚2
	LCD_delay_ms(50);
	LCD_WriteCMD(0x2f); // 升压步聚3
	LCD_delay_ms(50);
	LCD_WriteCMD(0x24); // 粗调对比度，可设置范围0x20～0x27
	LCD_WriteCMD(0x81); // 微调对比度命令, 与下面的设置参数命令要连续发送
//	LCD_WriteCMD(35);	// 微调对比度的值, 可设置范围0~63, ST7565 ST7567
	LCD_WriteCMD(20);;	// 微调对比度的值, 可设置范围0~63, UC1701
	LCD_WriteCMD(0xa2); // 偏压比(bias), 0xA2: 1/9,  0xA3: 1/7.
	LCD_WriteCMD(0xc8); // 行扫描顺序: 0xC0: 正方向(从上到下),  0xC8: 反方向(从下到上).
	LCD_WriteCMD(0xa0); // 列扫描顺序: 0xA0: 正方向(从左到右),  0xA1: 反方向(从右到左).
	LCD_WriteCMD(0x40); // 起始行：第一行开始
	LCD_WriteCMD(0xaf); // 开显示
	P_LCD_CS = 1;
	FillAll(0x00);		//清除所有显示
}


//=====================================================
void WriteAscii6x8(u8 x, u8 y, u8 chr)	//向指定位置写一个ASCII码字符, x为横向的点0~127, y为纵向的页0~7, chr为要写的字符
{
	u8 const *p;
	u8 i;

	if(x > (128-6))	return;
	if(y > 7)		return;
	p = chr * 6 + ASCII6x8;
	Set_Dot_Addr(x, y);
	for(i=0; i<6; i++)	{	LCD_WriteData(*p);	p++;}
}

//=====================================================
void WriteHZ16(u8 x, u8 y, u16 hz)	//向指定位置写一个汉字, x为横向的点0~127, y为纵向的页0~7, hz为要写的汉字.
{
	u8 const *p;
	u8 i;

	if(x > (128-16))	return;
	if(y > 6)			return;
	p = hz * 32 + HZK16;
	Set_Dot_Addr(x, y);
	for(i=0; i<16; i++)		{	LCD_WriteData(*p);	p++;}

	Set_Dot_Addr(x, (u8)(y+1));
	for(i=0; i<16; i++)		{	LCD_WriteData(*p);	p++;}
}

/************ 打印ASCII字符串 *************************/
void	printf_ascii(u8 x, u8 y, u8 *ptr)	//x为横向的点0~127, y为纵向的页0~7, *ptr为要打印的字符串指针.  打印21个ASCII码(21*6+9=135个字节)耗时625us@44MHZ, 652us@36MHz, 850us@24MHz
{
    u8 c;

	for (;;)
	{
        c = *ptr;
		if(c == 0)		return;	//遇到停止符0结束
		if(c < 0x80)			//ASCII码
		{
			WriteAscii6x8(x,y,c);
			x += 6;
		}
		ptr++;
	}
}


//******************************************
void WriteAscii_10x24(u8 x, u8 y, u8 chr)	//向指定位置写一个ASCII码字符, x为横向的点0~127, y为纵向的页0~7, chr为要写的字符
{
	u8 const *p;
	u8 i;

	if(x > (128-10))	return;
	if(y >= 6)			return;
	p = (u16)chr * 30 + ASCII10x24;

	Set_Dot_Addr(x, y);
	for(i=0; i<10; i++)		{	LCD_WriteData(*p);	p++;	}

	Set_Dot_Addr(x, (u8)(y+1));
	for(i=0; i<10; i++)		{	LCD_WriteData(*p);	p++;	}

	Set_Dot_Addr(x, (u8)(y+2));
	for(i=0; i<10; i++)		{	LCD_WriteData(*p);	p++;	}
}

//******************************************
void WriteDot_3x3(u8 x, u8 y)	//向指定位置写一个小数点, x为横向的点0~127, y为纵向的页0~7
{
	if(x > (128-3))	return;
	if(y >= 8)		return;

	Set_Dot_Addr(x, y);
	LCD_WriteData(0x38);
	LCD_WriteData(0x38);
	LCD_WriteData(0x38);
}


//************ 打印ASCII 10x24英文字符串 *************************
void	printf_ascii_10x24(u8 x, u8 y, u8 const *ptr)	//x为横向的点0~127, y为纵向的页0~7, *ptr为要打印的字符串指针, 间隔2点.
{
    u8 c;

	for (;;)
	{
		if(x > (128-10))	return;
		if(y > 5)			return;
		c = *ptr;
		if(c == 0)		return;	//遇到停止符0结束
		if((c >= '0') && (c <= '9'))			//ASCII码
		{
			WriteAscii_10x24(x,y,(u8)(c-'0'));
			x += 12;
		}
		else if(c == '.')
		{
			WriteDot_3x3(x,(u8)(y+2));
			x += 6;
		}
		else if(c == ' ')	//显示空格
		{
			WriteAscii_10x24(x,y,11);
			x += 12;
		}
		else if(c == '-')	//显示空格
		{
			WriteAscii_10x24(x,y,10);
			x += 12;
		}
			ptr++;
	}
}

//DMA_SPI_CR 	SPI_DMA控制寄存器
#define		DMA_ENSPI		(1<<7)	// SPI DMA功能使能控制位，    bit7, 0:禁止SPI DMA功能，  1：允许SPI DMA功能。
#define		SPI_TRIG_M		(1<<6)	// SPI DMA主机模式触发控制位，bit6, 0:写0无效，          1：写1开始SPI DMA主机模式操作。
#define		SPI_TRIG_S		(0<<5)	// SPI DMA从机模式触发控制位，bit5, 0:写0无效，          1：写1开始SPI DMA从机模式操作。
#define		SPI_CLRFIFO		1		// 清除SPI DMA接收FIFO控制位，bit0, 0:写0无效，          1：写1复位FIFO指针。


//DMA_SPI_CFG 	SPI_DMA配置寄存器
#define		DMA_SPIIE	(1<<7)	// SPI DMA中断使能控制位，bit7, 0:禁止SPI DMA中断，     1：允许中断。
#define		SPI_ACT_TX	(1<<6)	// SPI DMA发送数据控制位，bit6, 0:禁止SPI DMA发送数据，主机只发时钟不发数据，从机也不发. 1：允许发送。
#define		SPI_ACT_RX	(0<<5)	// SPI DMA接收数据控制位，bit5, 0:禁止SPI DMA接收数据，主机只发时钟不收数据，从机也不收. 1：允许接收。
#define		DMA_SPIIP	(0<<2)	// SPI DMA中断优先级控制位，bit3~bit2, (最低)0~3(最高).
#define		DMA_SPIPTY	0		// SPI DMA数据总线访问优先级控制位，bit1~bit0, (最低)0~3(最高).

//DMA_SPI_CFG2 	SPI_DMA配置寄存器2
#define		SPI_WRPSS	(0<<2)	// SPI DMA过程中使能SS脚控制位，bit2, 0: SPI DMA传输过程不自动控制SS脚。  1：自动拉低SS脚。
#define		SPI_SSS	    0		// SPI DMA过程中自动控制SS脚选择位，bit1~bit0, 0: P1.4,  1：P2.4,  2: P4.0,  3:P3.5。

//DMA_SPI_STA 	SPI_DMA状态寄存器
#define		SPI_TXOVW	(1<<2)	// SPI DMA数据覆盖标志位，bit2, 软件清0.
#define		SPI_RXLOSS	(1<<1)	// SPI DMA接收数据丢弃标志位，bit1, 软件清0.
#define		DMA_SPIIF	1		// SPI DMA中断请求标志位，bit0, 软件清0.

//HSSPI_CFG  高速SPI配置寄存器
#define		SS_HOLD		(3<<4)	//高速模式时SS控制信号的HOLD时间， 0~15, 默认3. 在DMA中会增加N个系统时钟，当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.
#define		SS_SETUP		3	//高速模式时SS控制信号的SETUP时间，0~15, 默认3. 在DMA中不影响时间，       当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.

//HSSPI_CFG2  高速SPI配置寄存器2
#define		SPI_IOSW	(1<<6)	//bit6:交换MOSI和MISO脚位，0：不交换，1：交换
#define		HSSPIEN		(0<<5)	//bit5:高速SPI使能位，0：关闭高速模式，1：使能高速模式
#define		FIFOEN		(1<<4)	//bit4:高速SPI的FIFO模式使能位，0：关闭FIFO模式，1：使能FIFO模式，使能FIFO模式在DMA中减少13个系统时间。
#define		SS_DACT			3	//bit3~0:高速模式时SS控制信号的DEACTIVE时间，0~15, 默认3, 不影响DMA时间.  当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.


void	SPI_DMA_TRIG(u8 xdata *TxBuf)
{
				//@40MHz, Fosc/4, 200字节258us，100字节  130us，50字节66us，N个字节耗时 N*1.280+2 us, 51T一个字节，其中状态机19T, 传输耗时32T.
				//@40MHz, Fosc/2, 200字节177us，100字节 89.5us，50字节46us，N个字节耗时 N*0.875+2 us, 35T一个字节，其中状态机19T, 传输耗时16T.
				//@40MHz, Fosc/2, SPI DMA传输一个字节, FIFO=1, HOLD=0，耗时16+3=19T(0.475us), HOLD=3，耗时16+6=22T(0.55us).
				//@40MHz, Fosc/4, SPI DMA传输一个字节, FIFO=1, HOLD=0，耗时32+3=35T(0.875us), HOLD=3，耗时32+6=38T(0.95us).
	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD会增加N个系统时钟, SS_SETUP没有增加时钟。驱动OLED 40MHz时SS_HOLD可以设置为0，
	HSSPI_CFG2 = SPI_IOSW | FIFOEN | SS_DACT;	//FIFOEN允许FIFO会减小13个时钟. @40MHz FIFOEN=1, SS_HOLD=0时523us @2T, 943us @4T;    FIFOEN=1, SS_HOLD=3时600us @2T, 1020us @4T.

	SPI_TxAddr   = (u16)TxBuf;		//要发送数据的首地址
	DMA_SPI_ITVH = 0;
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_TX | SPI_ACT_RX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;

	P_LCD_CS = 0;
	B_TxCmd   = 0;		//已发送命令标志
	SPI_TxCnt = 0;		//SPI DMA触发发送次数, 一次128字节, 一共8次
	B_SPI_DMA_busy = 1;	//标志SPI-DMA忙，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
	DMA_SPI_STA = DMA_SPIIF;	//软件触发SPI DMA中断，启动发送
}

//========================================================================
// 函数: void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
// 描述:  SPI_DMA中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-1-5
//========================================================================
void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
{
	if(SPI_TxCnt >= 8)	//判断发送是否完毕,  1.46ms @40MHz SPI-4T, 1.04ms @40MHz SPI-2T.
	{
		DMA_SPI_CR = 0;			//关闭SPI DMA
		B_SPI_DMA_busy = 0;		//清除SPI-DMA忙标志，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
		SPSTAT = 0x80 + 0x40;	//清0 SPIF和WCOL标志
		HSSPI_CFG2 = SPI_IOSW | SS_DACT;	//使用SPI查询或中断方式时，要禁止FIFO
		P_LCD_CS = 1;
	}
	else		//仍有数据要发送
	{
		if(!B_TxCmd)	//还没有发设置地址命令，则先发设置地址命令
		{
			B_TxCmd = 1;	//指示已发地址命令
			CmdTmp[0] = (u8)(0xb0 + SPI_TxCnt);
			CmdTmp[1] = (u8)(((0>>4) & 0x0f) + 0x10); //设置列地址的高4 位
			CmdTmp[2] = 0 & 0x0f;		//设置列地址的低4 位

			P_LCD_RS = 0;	//写命令
			DMA_SPI_TXAH = (u8)((u16)CmdTmp >> 8);	//SPI DMA发送命令首地址
			DMA_SPI_TXAL = (u8)CmdTmp;
			DMA_SPI_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
			DMA_SPI_AMT  = 3-1;				//设置传输总字节数(低8位).
			DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;	//启动SPI DMA发送命令
		}
		else
		{
			B_TxCmd = 0;	//清除已发地址命令
			P_LCD_RS = 1;	//写数据
			DMA_SPI_TXAH = (u8)(SPI_TxAddr >> 8);	//SPI DMA发送数据首地址
			DMA_SPI_TXAL = (u8)SPI_TxAddr;
			DMA_SPI_AMTH = 0;				//设置传输总字节数(高8位),	设置传输总字节数 = N+1
			DMA_SPI_AMT  = 128-1;			//设置传输总字节数(低8位).
			DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;	//启动SPI DMA发送命令
			SPI_TxAddr  += 128;	//要发送数据的首地址, 一次DMA传输16字节
			SPI_TxCnt++;		//发送次数+1
		}
	}

	DMA_SPI_STA = 0;		//清除中断标志
}



//====================================================================================
u8	xdata DisTmp[1024];	//显示缓冲，将要显示的内容放在显存里，启动DMA即可. 由于LCM DMA有4字节对齐问题，所以这里定位对地址为4的倍数


	#include	"picture1.h"
	#include	"picture2.h"

//~~~~~~~~~~~~~~~~~~~~~@~~~~~~~~&~~~~~~~~~~@~~~~~~~~~~~~程序开始
void main(void)
{
	u16	i;

	EAXFR = 1;	//允许访问扩展寄存器
	WTST  = 0;
	CKCON = 0;


	P0M1 = 0;	P0M0 = 0;	//设置为准双向口
	P1M1 = 0;	P1M0 = 0;	//设置为准双向口
	P2M1 = 0;	P2M0 = 0;	//设置为准双向口
	P3M1 = 0;	P3M0 = 0;	//设置为准双向口
	P4M1 = 0;	P4M0 = 0;	//设置为准双向口
	P5M1 = 0;	P5M0 = 0;	//设置为准双向口
	P6M1 = 0;	P6M0 = 0;	//设置为准双向口
	P7M1 = 0;	P7M0 = 0;	//设置为准双向口

	LCD12864_config();
	EA = 1;


	while(1)
	{
		for(i=0; i<1024; i++)	DisTmp[i] = 0;	//清除显存
		SPI_DMA_TRIG(DisTmp);
		while(B_SPI_DMA_busy);	//等待SPI DMA完成

		printf_ascii(0, 0, "Test LCD12864 UC1701");
		for(i=0; i<8; i++)	WriteHZ16((u8)(i*16),2,i);
		printf_ascii_10x24(0,5,"-12.345 678");
		LCD_delay_ms(3000);

		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture1[i];	//将图片装载到显存
		SPI_DMA_TRIG(DisTmp);
		while(B_SPI_DMA_busy);	//等待SPI DMA完成
		LCD_delay_ms(3000);

		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture2[i];	//将图片装载到显存
		SPI_DMA_TRIG(DisTmp);
		while(B_SPI_DMA_busy);	//等待SPI DMA完成
		LCD_delay_ms(3000);

	}
}




