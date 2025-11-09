
#include	"AI8051U.h"

/*************  FLASH相关变量声明   **************/
sbit    P_FLASH_CE   = P4^0;     //PIN1
sbit    P_FLASH_MOSI = P4^1;     //PIN5
sbit    P_FLASH_MISO = P4^2;     //PIN2
sbit    P_FLASH_SCLK = P4^3;     //PIN6
sbit    P_FLASH_WP   = P5^2;     //PIN3
sbit    P_FLASH_HOLD = P5^3;     //PIN7

#define	SPI_CE_High()	P_FLASH_CE		= 1		// set CE high
#define	SPI_CE_Low()	P_FLASH_CE		= 0		// clear CE low
#define	SPI_Hold()		P_FLASH_Hold	= 0		// clear Hold pin
#define	SPI_UnHold()	P_FLASH_Hold	= 1		// set Hold pin
#define	SPI_WP()		P_FLASH_WP		= 0		// clear WP pin
#define	SPI_UnWP()		P_FLASH_WP		= 1		// set WP pin


//ID: W25Q40=0x12,  W25Q80=0x13, W25Q16=0x14, W25Q32=0x15, W25Q64=0x16, W25Q128=0x17, W25Q256=0x18, W25Q512=0x19,

/******************* FLASH相关程序 ************************/
	#define SFC_WREN        0x06	//允许写 Write Enable
	#define SFC_WRDI        0x04	//禁止写 Write Disable
	#define SFC_VSRWREN     0x50	//Volatile Write Enable
	#define SFC_RDSR1       0x05	//读状态寄存器1  S7~S0
	#define SFC_WRSR1       0x01	//写状态寄存器1  S7~S0
	#define SFC_RDSR2       0x35	//读状态寄存器2  S15~S8
	#define SFC_WRSR2       0x31	//写状态寄存器2  S15~S8
	#define SFC_RDSR3       0x15	//读状态寄存器3  S23~S16
	#define SFC_WRSR3       0x11	//写状态寄存器3  S23~S16
	#define SFC_RDSFDPR     0x5A	//Read SFDP Register

	#define SFC_READ         0x03	//读命令
	#define SFC_FastRead     0x0B	//快速读
	#define SFC_FastReadDual 0x3B	//快速读2线
	#define SFC_FastReadQuad 0x6B	//快速读4线
	#define SFC_FastReadDualIO 0xBB	//快速读2线
	#define SFC_FastReadQuadIO 0xEB	//快速读4线

	#define SFC_RDID        0xAB	//读ID
	#define SFC_RDMFID      0x90	//读MF ID
	#define SFC_PAGEPROG    0x02	// 页写入, 后跟24位地址A23~A16 A15~A8 A7~A0 D7~D0 D7~D0.....
	#define SFC_QPAGEPROG   0x32	//4页写入, 后跟24位地址A23~A16 A15~A8 A7~A0 D7~D0 D7~D0.....
	#define SFC_SECTORER4K  0x20    // 4KB 扇区擦除指令
	#define SFC_SECTORER32K 0x52    //32KB 扇区擦除指令
	#define SFC_SECTORER64K 0xd8    //64KB 扇区擦除指令
	#define SFC_CHIPER      0xC7	//片擦除  0xC7或0x60


bit	B_FlashOK;
bit	B_SPI_DMA_busy;
u8	FLASH_ID;


/************************************************************************/
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

	B_SPI_DMA_busy = 0;
	HSCLKDIV   = 1;					//HSCLKDIV主时钟分频
	SPI_CLKDIV = 1;					//SPI_CLKDIV主时钟分频
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志

	if(SPI_io == 0)
	{
		P1n_standard(0xf0);		//切换到 P1.4(SS) P1.5(MOSI) P1.6(MISO) P1.7(SCLK), 设置为准双向口
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

	P5n_standard(Pin3+Pin2);		//WP HOLD设置为准双向口
	PullUpEnable(P5PU, Pin3+Pin2);	//WP HOLD设置上拉电阻    允许端口内部上拉电阻   PxPU, 要设置的端口对应位为1
	P_FLASH_CE		= 1;		//PIN1		PIN8--VDD
	P_FLASH_MISO	= 1;		//PIN2      PIN7--HOLD
	P_FLASH_WP		= 1;		//PIN3
	P_FLASH_MOSI	= 1;		//PIN5      PIN4--GND
	P_FLASH_SCLK	= 1;		//PIN6
	P_FLASH_HOLD	= 1;		//PIN7
}


/************************************************************************/
void	SPI_WriteByte(u8 dat)
{
	SPDAT = dat;		//发送一个字节
	while(SPIF == 0)	;			//等待发送完成
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
	u8	i;
	SPDAT = 0xff;		//发送一个空字节
	while(SPIF == 0)	;			//等待发送完成
	i = SPDAT;
	SPSTAT = 0x80 + 0x40;			//清0 SPIF和WCOL标志
	return i;//SPDAT;		//接收返回的字节
}



/************************************************
检测Flash是否准备就绪, 任何时候均可读
入口参数: 无
出口参数:
    0 : 没有检测到正确的Flash
    1 : Flash准备就绪
************************************************/
void	FlashCheckID(void)
{
	SPI_CE_Low();
	FLASH_ID = 0;
	SPI_WriteByte(SFC_RDID);		//发送读取ID命令
	SPI_WriteByte(0x00);			//空读3个字节
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);			//ID 地址0x00或0x01
	FLASH_ID = SPI_ReadByte();		//读取制造商ID
	SPI_CE_High();
}


/************************************************
检测Flash的忙状态
入口参数: 无
出口参数:
    0 : Flash处于空闲状态
    1 : Flash处于忙状态
************************************************/
u8	FlashCheckBusy(void)
{
	u8	dat;

	SPI_CE_Low();
	SPI_WriteByte(SFC_RDSR1);	//发送读取状态寄存器1命令
	dat = SPI_ReadByte();		//读取状态寄存器1
	SPI_CE_High();

	return (dat & 1);			//状态值的Bit0即为忙标志
}

/************************************************
使能Flash写命令
入口参数: 无
出口参数: 无
************************************************/
void	FlashWriteEnable(void)
{
	while(FlashCheckBusy() != 0);	//Flash忙检测
	SPI_CE_Low();
	SPI_WriteByte(SFC_WREN);		//发送写使能命令
	SPI_CE_High();
}

/************************************************
擦除整片Flash
入口参数: 无
出口参数: 无
************************************************/
/*
void	FlashChipErase(void)
{
	if(B_FlashOK)
	{
		FlashWriteEnable();				//使能Flash写命令
		SPI_CE_Low();
		SPI_WriteByte(SFC_CHIPER);		//发送片擦除命令
		SPI_CE_High();
	}
}
*/

/************************************************
擦除扇区,
入口参数: u32 addr: 擦除地址, u8 sec:扇区大小4 32 64
出口参数: 无
************************************************/
void	FlashSectorErase(u32 addr, u8 sec)
{
	if(B_FlashOK)
	{
		FlashWriteEnable();				//使能Flash写命令
		SPI_CE_Low();
			 if(sec == 32)	SPI_WriteByte(SFC_SECTORER32K);	//发送扇区擦除命令
		else if(sec == 64)	SPI_WriteByte(SFC_SECTORER64K);	//发送扇区擦除命令
		else 				SPI_WriteByte(SFC_SECTORER4K);	//发送扇区擦除命令
		SPI_WriteByte(((u8 *)&addr)[1]);           //设置起始地址
		SPI_WriteByte(((u8 *)&addr)[2]);
		SPI_WriteByte(((u8 *)&addr)[3]);
		SPI_CE_High();
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
void	FlashRead_Nbytes(u32 addr, u8 *buffer, u16 size)
{
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	while(FlashCheckBusy() != 0);		//Flash忙检测
	SPI_CE_Low();						//enable device
	SPI_WriteByte(SFC_READ); 			//read command
	SPI_WriteByte(((u8 *)&addr)[1]);	//设置起始地址
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	do{
		*buffer = SPI_ReadByte();		//receive byte and store at buffer
		buffer++;
	}while(--size);						//read until no_bytes is reached
	SPI_CE_High();						//disable device
}


/************************************************
写数据到Flash中
入口参数:
    addr   : 地址参数
    buffer : 缓冲需要写入Flash的数据
    size   : 数据块大小
出口参数: 无
************************************************/
void	FlashWrite_Nbytes(u32 addr, u8 *buffer, u16 size)
{
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	FlashWriteEnable();					//使能Flash写命令

	SPI_CE_Low();						// enable device
	SPI_WriteByte(SFC_PAGEPROG);		// 发送页编程命令
	SPI_WriteByte(((u8 *)&addr)[1]);	//设置起始地址
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);
	do{
		SPI_WriteByte(*buffer++);		//连续页内写
		addr++;
		if ((addr & 0xff) == 0) break;
	}while(--size);
	SPI_CE_High();						// disable device
}



//DMA_SPI_CR 	SPI_DMA控制寄存器
#define		DMA_ENSPI		(1<<7)	// SPI DMA功能使能控制位，    bit7, 0:禁止SPI DMA功能，  1：允许SPI DMA功能。
#define		SPI_TRIG_M		(1<<6)	// SPI DMA主机模式触发控制位，bit6, 0:写0无效，          1：写1开始SPI DMA主机模式操作。
#define		SPI_TRIG_S		(0<<5)	// SPI DMA从机模式触发控制位，bit5, 0:写0无效，          1：写1开始SPI DMA从机模式操作。
#define		SPI_CLRFIFO			0	// 清除SPI DMA接收FIFO控制位，bit0, 0:写0无效，          1：写1复位FIFO指针。


//DMA_SPI_CFG 	SPI_DMA配置寄存器
#define		DMA_SPIIE	(1<<7)	// SPI DMA中断使能控制位，bit7, 0:禁止SPI DMA中断，     1：允许中断。
#define		SPI_ACT_TX	(1<<6)	// SPI DMA发送数据控制位，bit6, 0:禁止SPI DMA发送数据，主机只发时钟不发数据，从机也不发. 1：允许发送。
#define		SPI_ACT_RX	(1<<5)	// SPI DMA接收数据控制位，bit5, 0:禁止SPI DMA接收数据，主机只发时钟不收数据，从机也不收. 1：允许接收。
#define		DMA_SPIIP	(0<<2)	// SPI DMA中断优先级控制位，bit3~bit2, (最低)0~3(最高).
#define		DMA_SPIPTY		0	// SPI DMA数据总线访问优先级控制位，bit1~bit0, (最低)0~3(最高).

//DMA_SPI_CFG2 	SPI_DMA配置寄存器2
#define		SPI_WRPSS	(0<<2)	// SPI DMA过程中使能SS脚控制位，bit2, 0: SPI DMA传输过程不自动控制SS脚。  1：自动拉低SS脚。
#define		SPI_SSS	    	0	// SPI DMA过程中自动控制SS脚选择位，bit1~bit0, 0: P1.4,  1：P2.4,  2: P4.0,  3:P3.5。

//DMA_SPI_STA 	SPI_DMA状态寄存器
#define		SPI_TXOVW	(1<<2)	// SPI DMA数据覆盖标志位，bit2, 软件清0.
#define		SPI_RXLOSS	(1<<1)	// SPI DMA接收数据丢弃标志位，bit1, 软件清0.
#define		DMA_SPIIF		1	// SPI DMA中断请求标志位，bit0, 软件清0.

//HSSPI_CFG  高速SPI配置寄存器
#define		SS_HOLD		(3<<4)	//高速模式时SS控制信号的HOLD时间， 0~15, 默认3. 在DMA中会增加N个系统时钟，当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.
#define		SS_SETUP		3	//高速模式时SS控制信号的SETUP时间，0~15, 默认3. 在DMA中不影响时间，       当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.

//HSSPI_CFG2  高速SPI配置寄存器2
#define		SPI_IOSW	(0<<6)	//bit6:交换MOSI和MISO脚位，0：不交换，1：交换
#define		HSSPIEN		(0<<5)	//bit5:高速SPI使能位，0：关闭高速模式，1：使能高速模式
#define		FIFOEN		(0<<4)	//bit4:高速SPI的FIFO模式使能位，0：关闭FIFO模式，1：使能FIFO模式，使能FIFO模式在DMA中减少13个系统时间。
#define		SS_DACT			3	//bit3~0:高速模式时SS控制信号的DEACTIVE时间，0~15, 默认3. 当SPI速度为系统时钟/2时执行DMA，SS_HOLD、SS_SETUP和SS_DACT都必须设置大于2的值.

void	SPI_DMA_RxTRIG(u32 addr, u8 *buffer, u16 size)	//注意：允许FIFO会导致读出错，读出全部是0xff.
{
	u16	i;		//@40MHz, Fosc/4, 200字节258us，100字节  130us，50字节66us，N个字节耗时 N*1.280+2 us, 51T一个字节，其中状态机19T, 传输耗时32T.
				//@40MHz, Fosc/2, 200字节177us，100字节 89.5us，50字节46us，N个字节耗时 N*0.875+2 us, 35T一个字节，其中状态机19T, 传输耗时16T.
				//@40MHz, Fosc/2, SPI DMA传输一个字节, FIFO=1, HOLD=0，耗时16+3=19T(0.475us), HOLD=3，耗时16+6=22T(0.55us).
				//@40MHz, Fosc/4, SPI DMA传输一个字节, FIFO=1, HOLD=0，耗时32+3=35T(0.875us), HOLD=3，耗时32+6=38T(0.95us).
	if(size == 0)	return;
	if(!B_FlashOK)	return;
	while(FlashCheckBusy() != 0);		//Flash忙检测

	SPI_CE_Low();						//enable device
	SPI_WriteByte(SFC_READ); 			//read command
	SPI_WriteByte(((u8 *)&addr)[1]);	//设置起始地址
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD会增加N个系统时钟, SS_SETUP没有增加时钟。
	HSSPI_CFG2 = SPI_IOSW | HSSPIEN | FIFOEN | SS_DACT;	//FIFOEN允许FIFO会减小13个时钟. @40MHz FIFOEN=1, SS_HOLD=0时523us @2T, 943us @4T;    FIFOEN=1, SS_HOLD=3时600us @2T, 1020us @4T.

	i = (u16)buffer;	//取首地址
	DMA_SPI_RXAH = (u8)(i >> 8);		//接收地址寄存器高字节
	DMA_SPI_RXAL = (u8)i;				//接收地址寄存器低字节
	DMA_SPI_AMTH = (u8)((size-1)/256);	//设置传输总字节数 = n+1
	DMA_SPI_AMT  = (u8)((size-1)%256);	//设置传输总字节数 = n+1
	DMA_SPI_ITVH = 0;					//增加的间隔时间，N+1个系统时钟
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_RX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
	DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;
	B_SPI_DMA_busy = 1;	//标志SPI-DMA忙，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
}

void	SPI_DMA_TxTRIG(u32 addr, u8 *buffer, u16 size)	//
{
	u16	i;
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	FlashWriteEnable();					//使能Flash写命令
	SPI_CE_Low();						// enable device
	SPI_WriteByte(SFC_PAGEPROG);		// 发送页编程命令
	SPI_WriteByte(((u8 *)&addr)[1]);	//设置起始地址
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD会增加N个系统时钟, SS_SETUP没有增加时钟。
	HSSPI_CFG2 = SPI_IOSW | HSSPIEN | FIFOEN | SS_DACT;	//FIFOEN允许FIFO会减小13个时钟. @40MHz FIFOEN=1, SS_HOLD=0时523us @2T, 943us @4T;    FIFOEN=1, SS_HOLD=3时600us @2T, 1020us @4T.

	i = (u16)buffer;	//取首地址
	DMA_SPI_TXAH = (u8)(i >> 8);		//接收地址寄存器高字节
	DMA_SPI_TXAL = (u8)i;				//接收地址寄存器低字节
	DMA_SPI_AMTH = (u8)((size-1)/256);	//设置传输总字节数 = n+1
	DMA_SPI_AMT  = (u8)((size-1)%256);	//设置传输总字节数 = n+1
	DMA_SPI_ITVH = 0;					//增加的间隔时间，N+1个系统时钟
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_TX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
	DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;
	B_SPI_DMA_busy = 1;	//标志SPI-DMA忙，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
}

//=====================================40.96/4====================================
// 函数: void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
// 描述:  SPI_DMA中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-1-5
//========================================================================
void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
{
	DMA_SPI_STA = 0;		//清除中断标志
	DMA_SPI_CR  = 0;		//禁止DMA功能
	SPSTAT = 0x80 + 0x40;	//清0 SPIF和WCOL标志
	B_SPI_DMA_busy = 0;		//清除SPI-DMA忙标志，SPI DMA中断中清除此标志，使用SPI DMA前要确认此标志为0
	HSSPI_CFG2 = SPI_IOSW | SS_DACT;	//使用SPI查询或中断方式时，要禁止FIFO
	SPI_CE_High();						// disable device
}
