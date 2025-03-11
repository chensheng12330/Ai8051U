
/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/


#include	"AI8051U.h"
#include	"intrins.h"
#include	"TLV320AIC23.h"
#include	"W25Q128FV.h"

/*************	功能说明	**************
本程序使用AI8051U实验箱V1.2验证。用户先别修改程序，直接下载HEX文件到AI8051U实验箱V1.2来验证，下载时选择主频36.864MHz。

使用AI8051U系列MCU做数字录音测试，语音存储于FLASH中. 话筒放大电路低通滤波转折频率为3400Hz左右，合适8~16K采样。
为了能少用点空间，语音使用A率压缩/解压缩，每秒字节数等于采样率。
 8KHz采样，码率为 8KB/S，16MB FLASH可以录音34分钟。
16KHz采样，码率为16KB/S，16MB FLASH可以录音17分钟。

******************************************/



//对于I2S的时钟必须是 256*SampleRate的整数倍。对于立体声16位，左右声道时钟WS=SampleRate，数据时钟BCLK=32*SampleRate，主时钟MCLK=8*BCLK=256*SampleRate。

	#define FOSC			36864000UL		//定义主时钟
//	#define SampleRate		48000			//定义采样率
//	#define SampleRate		36000			//定义采样率
//	#define SampleRate		24000			//定义采样率
//	#define SampleRate		16000			//定义采样率
//	#define SampleRate		12000			//定义采样率
	#define SampleRate		8000			//定义采样率
//	#define SampleRate		6000			//定义采样率

//	#define FOSC			40960000UL		//定义主时钟
//	#define FOSC			32768000UL		//定义主时钟
//	#define SampleRate		32000			//定义采样率
//	#define SampleRate		16000			//定义采样率
//	#define SampleRate		 8000			//定义采样率

//	#define FOSC			33868800UL		//定义主时钟
//	#define SampleRate		44100			//定义采样率
//	#define SampleRate		22050			//定义采样率
//	#define SampleRate		11025			//定义采样率

#define  CHANNEL  2			  //设置ADC通道(咪头通道),   取值为0~7, 对应P1.0~P1.7, 使用别的ADC输入口则要修改ADC初始化函数.

#define	VOICE_BUFF_LENGTH	16384		//必须是4096、8192、16384这3个数之一
#define	VOICE_BUFF_MASK		(VOICE_BUFF_LENGTH-1)
#define	FLASH_CAP			(16380*1024)	// FLASH容量


/*************	IO口定义	**************/
void 	Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

/*************	变量定义	**************/
u32	FileLength;		//文件长度(字节)
u32	PlayByteCnt;	//播放字节计数
u16	dac;			//解码后输出的左后声道DAC值
bit	B_PlayEn;		//允许播放
bit	B_record;		//允许录音
bit B_stop;			//1: 停止录音或放音

u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u16	wr_index;		//写缓冲索引
u16	rd_index;		//读缓冲索引
u32	FlashAddr;		//读FLASH地址
u8	OP_index;		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH

u16	second;
u16	cnt_1s;

u8	cnt_1ms;		//1ms计数，用户层不可见
u8	cnt_20ms;		//20ms计数，用户层不可见
bit	B_20ms;			//20ms标志，用户层使用并清除
u16	HeadPhoneVol;	//耳机音量, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.

u8 	LED8[8];		//显示缓冲
u8	display_index;	//显示位索引
u8	KeyCode;		//给用户使用的键码
u8 IO_KeyState;		//行列键盘变量


//===================================================
#define DIS_DOT		0x20
#define DIS_BLACK	0x10
#define DIS_		0x11

#define DIS_S		0x05
#define DIS_T		0x1A
#define DIS_O		0x17
#define DIS_P		0x18
#define DIS_R		0x1D
#define DIS_E		0x0E
#define DIS_C		0x0C
#define DIS_D		0x0D
#define DIS_L		0x15
#define DIS_A		0x0A
#define DIS_Y		0x1F

#define	K0	0x01
#define	K1	0x02
#define	K2	0x04
#define	K3	0x08
#define	K4	0x10
#define	K5	0x20
#define	K6	0x40
#define	K7	0x80


u8 code t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//位码


/*************	函数声明	**************/

void	I2S_config(void);
void	SetSampleRate(void);
void  	delay_ms(u16 ms);
void	SetHeadPhoneVolume(u16 vol);
void	IO_KeyScan(void);	//50ms call
u8		CheckString(u8 *px, u8 const *pc, u8 num);
void 	CDC_StringPrint(u8 *puts);
void	PlayProcess(void);
void	ADC_config(void);


u8 const F_ALAW[]="ALAW";			//文件头 00H~03H


//==============================================================================================================
//	*******************		   					main()							*********************************
//===============================================================================================================
void main(void)
{
	u16	i;

	EAXFR = 1;	//SFR enable
	WTST  = 0;
	CKCON = 0;

	P0M0 = 0x00;
	P0M1 = 0x00;
	P1M0 = 0x00;
	P1M1 = 0x00;
	P2M0 = 0x00;
	P2M1 = 0x00;
	P3M0 = 0x00;
	P3M1 = 0x00;
	P4M0 = 0x00;
	P4M1 = 0x00;
	P5M0 = 0x00;
	P5M1 = 0x00;

	EA = 1;

	SPI_Config(2, 0);	//(SPI_io, SPI_speed), 参数: 	SPI_io: 切换IO(SS MOSI MISO SCLK), 0: 切换到P1.4 P1.5 P1.6 P1.7,  1: 切换到P2.4 P2.5 P2.6 P2.7, 2: 切换到P4.0 P4.1 P4.2 P4.3,  3: 切换到P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI的速度, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
	ADC_config();

	cnt_1ms = 0;	// 1ms计数，用户层不可见
	cnt_20ms = 0;	//20ms计数，用户层不可见
	B_20ms    = 0;	//20ms标志，用户层使用并清除
	for(i=0; i<8; i++)	LED8[i] = DIS_BLACK;	//上电消隐
	LED8[0] = DIS_S;	//显示stop
	LED8[1] = DIS_T;
	LED8[2] = DIS_O;
	LED8[3] = DIS_P;
	cnt_1s = 0;

	delay_ms(10);
	AIC23_Init();
	delay_ms(50);
	AIC32_InitSet();
	delay_ms(50);
	HeadPhoneVol = 80 - 30;//默认音量
	SetHeadPhoneVolume(HeadPhoneVol);	//设置音量
	LED8[6] = HeadPhoneVol / 10;		//显示音量
	LED8[7] = HeadPhoneVol % 10;

	I2S_config();
	EA = 1;

	OP_index = 0;

	B_FlashOK = 0;
	FlashCheckID();
	FlashCheckID();
	B_FlashOK = 0;
	if((FLASH_ID >= 0x12) && (FLASH_ID <= 0x19))	B_FlashOK = 1;	//检测到FLASH

	while(1)
	{
		if(OP_index != 0)	PlayProcess();

		if(B_20ms)	//20ms时隙
		{
			B_20ms = 0;

			if(B_PlayEn || B_record)
			{
				if(++cnt_1s >= 50)	//秒计时
				{
					cnt_1s = 0;
					second++;
					i = second / 60;
					LED8[1] = (u8)(i/10);
					LED8[2] = (u8)(i%10 +DIS_DOT);
					i = second % 60;
					LED8[3] = i/10;
					LED8[4] = i%10;		//放音时间
				}
			}

			if(B_stop && !B_SPI_DMA_busy)	//停止录音或放音
			{
				B_stop = 0;
				OP_index = 0;
				LED8[0] = DIS_S;	//显示stop
				LED8[1] = DIS_T;
				LED8[2] = DIS_O;
				LED8[3] = DIS_P;
				LED8[4] = DIS_BLACK;

				if(B_record)	//录音结束, 保存ALAW标识和数据长度
				{
					B_record = 0;
					for(i=0; i<4; i++)	voice_buff[i] = F_ALAW[i];	//A LAW标识
					voice_buff[4] = (u8)(FlashAddr >> 24);	//录音FLASH长度，大端模式
					voice_buff[5] = (u8)(FlashAddr >> 16);
					voice_buff[6] = (u8)(FlashAddr >> 8);
					voice_buff[7] = (u8)FlashAddr;
					FlashWrite_Nbytes(0, voice_buff, 8);	//(u32 addr, u8 *buffer, u16 size)	写入数据
				}
			}

			IO_KeyScan();


			if(KeyCode != 0)	//有键按下
			{
				if(KeyCode == K1)	//停止播放或录音
				{
					OP_index = 0;	//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
					B_PlayEn = 0;	//停止播放
					B_stop   = 1;
				}

				else if(KeyCode == K2)	//录音
				{
					if(B_record)	OP_index = 0, B_stop = 1;	//正在录音则停止录音
					else if(B_FlashOK)	//FLASH存在, 则启动录音
					{
						if(!B_PlayEn)	//未放音才可以录音
						{
							OP_index = 3;	//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
							cnt_1s = 0;
							second = 0;
							LED8[0] = DIS_R;	//显示R
							LED8[1] = 0;
							LED8[2] = 0+DIS_DOT;
							LED8[3] = 0;
							LED8[4] = 0;		//录音时间
						}
					}
				}

				else if(KeyCode == K3)	//放音
				{
					if(B_PlayEn)	B_PlayEn = 0, B_stop = 1;	//正在播放停止播放
					else if(B_FlashOK)	//FLASH存在, 则启动放音
					{
						if(!B_record)	//未录音则可以播放
						{
							OP_index = 1;	//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
							cnt_1s = 0;
							second = 0;
							LED8[0] = DIS_P;	//显示P
							LED8[1] = 0;
							LED8[2] = 0+DIS_DOT;
							LED8[3] = 0;
							LED8[4] = 0;		//播放时间
						}
					}
				}

				else if(KeyCode == K6)	//音量+
				{
					if(++HeadPhoneVol > 80)	HeadPhoneVol = 80;	//最大音量
					SetHeadPhoneVolume(HeadPhoneVol);	//设置音量
					LED8[6] = HeadPhoneVol / 10;	//显示音量
					LED8[7] = HeadPhoneVol % 10;
				}
				else if(KeyCode == K7)	//音量-
				{
					if(HeadPhoneVol != 0)	HeadPhoneVol--;	//最小音量
					SetHeadPhoneVolume(HeadPhoneVol);	//设置音量
					LED8[6] = HeadPhoneVol / 10;	//显示音量
					LED8[7] = HeadPhoneVol % 10;
				}

				KeyCode = 0;
			}
		}
	}
}


// 校验一个字符串，正确返回0，错误返回非0
u8	CheckString(u8 *px, u8 const *pc, u8 num)
{
	u8	i;
	for(i=0; i<num; i++)
	{
		if(px[i] != pc[i])	return 1;	//字符不相等错误
	}
	return 0;	//字符串正确
}

//播放 录音处理
void	PlayProcess(void)
{
	u16	j;
	if(OP_index == 1)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
	{
		FlashRead_Nbytes(0, voice_buff, 1024);//(u32 addr, u8 *buffer, u16 size)	读取文件
		if(CheckString(voice_buff, F_ALAW, 4) == 0)			//检测ALAW		文件头 00H~03H
		{
			B_PlayEn = 0;	//停止播放
			B_record = 0;	//停止录音
			FileLength = ((u32)voice_buff[4] << 24) + ((u32)voice_buff[5] << 16) + (u32)voice_buff[6]*256 + voice_buff[7];	//数据字节长度, 大端模式, [4] [5] [6] [7]
			if(FileLength < FLASH_CAP)	//小于falsh容量
			{
				FlashAddr = 1024;
				wr_index  = 1024;	//写缓冲索引
				rd_index  = 16;		//读缓冲索引, 从16开始存放语音数据
				PlayByteCnt = 16;	//播放字节计数
				dac = 0;
				B_PlayEn = 1;	//启动播放
				OP_index = 2;
			}
		}
		else OP_index = 0, B_stop = 1;	//无语音数据
	}

	else if(OP_index == 2)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
	{
		if(!B_SPI_DMA_busy)
		{
			j = (rd_index - wr_index) & VOICE_BUFF_MASK;	//计算空闲缓冲字节数
			if((j > 1024) || (j == 0))	//空出了超过1024字节
			{
				if(FlashAddr < FileLength)	//未到文件结束, 继续读FLASH
				{
				//	FlashRead_Nbytes(FlashAddr, voice_buff+wr_index, 1024);	//(u32 addr, u8 *buffer, u16 size)	读取数据
					SPI_DMA_RxTRIG(FlashAddr, voice_buff+wr_index, 1024);//(u32 addr, u8 *buffer, u16 size);	SPI DMA读取数据
					FlashAddr += 1024;	//读FLASH的地址+1024
					wr_index  += 1024;	//写缓冲索引+1024
					wr_index  &= VOICE_BUFF_MASK;	//溢出处理
				}
			}
		}
	}

	else if(OP_index == 3)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
	{
		B_PlayEn = 0;	//停止播放
		B_record = 0;	//停止录音
		FlashAddr= 0;
		wr_index = 16;	//写缓冲索引, 语音数据从16开始
		rd_index = 0;		//读缓冲索引, 从16开始存放语音数据
		for(j=0; j<16; j++)	voice_buff[j] = 0xff;	//预留16字节
		B_record = 1;	//开始录音
		OP_index = 4;
	}
	else if(OP_index == 4)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:启动录音，4:保存录音数据
	{
		if(!B_SPI_DMA_busy)	//SPI DMA空闲
		{
			j = (wr_index - rd_index) & VOICE_BUFF_MASK;	//已有数据字节数
			if(j > 256)	//空出了超过256字节
			{
				if((FlashAddr & 0x00ffff) == 0)
				{
					FlashSectorErase(FlashAddr, 64);	//(u32 addr, u8 sec)	擦除一个扇区64K
					while(FlashCheckBusy() != 0);		//Flash忙检测
				}
			//	FlashWrite_Nbytes(FlashAddr, voice_buff+rd_index, 256);	//(u32 addr, u8 *buffer, u16 size)	写入数据
				SPI_DMA_TxTRIG(FlashAddr, voice_buff+rd_index, 256);//(u32 addr, u8 *buffer, u16 size);	SPI DMA写入数据
				rd_index  += 256;	//读缓冲索引+256
				rd_index  &= VOICE_BUFF_MASK;	//溢出处理
				FlashAddr += 256;	//写FLASH的地址+256
				if(FlashAddr >= FLASH_CAP)	OP_index = 0, B_record = 1, B_stop = 1;	//播放完成了
			}
		}
	}
}


/*****************************************************
	行列键扫描程序
	使用XY查找4x4键的方法，只能单键，速度快

   Y     P00      P01      P02      P03
          |        |        |        |
X         |        |        |        |
P07 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P06 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
******************************************************/

void IO_KeyDelay(void)
{
	u8 i;
	i = 40;
	while(--i)	;
}

void	IO_KeyScan(void)	//50ms call
{
	u8	j;

	j = IO_KeyState;	//保存上一次状态

	P06 = 0;
	IO_KeyDelay();
	IO_KeyState = P0 & 0x0f;
	P06 = 1;

	P07 = 0;
	IO_KeyDelay();
	IO_KeyState |= (P0 << 4) & 0xf0;
	IO_KeyState ^= 0xff;	//取反
	P07 = 1;

	KeyCode |= (j ^ IO_KeyState) & IO_KeyState;
}


//=================== 设置耳机音量 ==========================================
void	SetHeadPhoneVolume(u16 vol)
{
	AIC23_WriteCmd(R_HeadphoneVolume_L, (LinVol_LRS | LinVol_Mute | (vol+47)));	//耳机输出音量
	AIC23_WriteCmd(R_HeadphoneVolume_R, (RinVol_RLS | RinVol_Mute | (vol+47)));
}

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
	 	i = FOSC / 6000;
		while(--i)	;
     }while(--ms);
}

/**************** 向HC595发送一个字节函数 ******************/
void Send_595(u8 dat)
{
	u8	i;
	for(i=0; i<8; i++)
	{
		dat <<= 1;
		P_HC595_SER   = CY;
		NOP(1);
		P_HC595_SRCLK = 1;
		NOP(1);
		P_HC595_SRCLK = 0;
	}
}

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{
	Send_595(t_display[LED8[display_index]]);	//输出段码
	Send_595(~T_COM[display_index]);			//输出位码

	P_HC595_RCLK = 1;
	NOP(2);
	P_HC595_RCLK = 0;							//锁存输出数据
	if(++display_index >= 8)	display_index = 0;	//8位结束回0
}

/********************** ADC初始化函数 ************************/
#define D_ADC_POWER	(1<<7)	/* ADC电源，1开启，0关闭 */
#define D_ADC_START	(1<<6)	/* 启动转换，自动清0 */
#define D_ADC_FLAG	(1<<5)	/* 完成标志，软件清0 */
#define D_ADC_EPWMT	(1<<4)	/* 允许PWMA触发ADC */

#define	D_ADC_SPEED	5		/* 0~15, ADC时钟 = SYSclk/2/(n+1) */
#define	D_RES_FMT	(1<<5)	/* ADC结果格式 0: 左对齐, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: 右对齐, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */
#define CSSETUP		(1<<7)	/* 0~1,  ADC通道选择时间      0: 1个ADC时钟, 1: 2个ADC时钟,  默认0(默认1个ADC时钟)	*/
#define CSHOLD		(1<<5)	/* 0~3,  ADC通道选择保持时间  (n+1)个ADC时钟, 默认1(默认2个ADC时钟)					*/
#define SMPDUTY		10		/* 10~31, ADC模拟信号采样时间  (n+1)个ADC时钟, 默认10(默认11个ADC时钟)				*/
							/* ADC转换时间: 10位ADC固定为10个ADC时钟, 12位ADC固定为12个ADC时钟. 				*/
void	ADC_config(void)
{
//	EAXFR = 1;	//SFR enable
	P1n_pure_input(1<<CHANNEL);	//设置要做ADC的IO做高阻输入
	ADC_CONTR = D_ADC_POWER + CHANNEL;	//ADC on + channel
	ADCCFG = D_RES_FMT + D_ADC_SPEED;
	ADCTIM = CSSETUP + CSHOLD + SMPDUTY;
//	ADC_START   =  1;	//启动ADC转换, 完成后自动清零
//	ADC_FLAG    =  0;	//清除ADC完成(中断)标志
//	ADC_EPWMT   =  1;	//允许PWM触发ADC
//	EADC  = 1;	//允许ADC中断
//	PADCH = 1;	//ADC 中断优先级高位
//	PADC  = 1;	//ADC 中断优先级
}


/*********** A率压缩/解压缩算法 ******************************

  压缩: 根据国际标准，A率是以12位ADC，A=87.6为基准的，13折线法逼近。
解压缩: 本程序是I2S声音输出，是16位的DAC，所以计算时，所有数据都*16。

	law_data    b7    b6 b5 b4    b3 b2 b1 b0
	           minus  Exponent      Mantissa
***************************************************************/

const u8	T_Alaw_encode[256]={
 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,45,45,46,46,47,47,
48,48,48,48,49,49,49,49,50,50,50,50,51,51,51,51,52,52,52,52,53,53,53,53,54,54,54,54,55,55,55,55,
56,56,56,56,57,57,57,57,58,58,58,58,59,59,59,59,60,60,60,60,61,61,61,61,62,62,62,62,63,63,63,63,
64,64,64,64,64,64,64,64,65,65,65,65,65,65,65,65,66,66,66,66,66,66,66,66,67,67,67,67,67,67,67,67,
68,68,68,68,68,68,68,68,69,69,69,69,69,69,69,69,70,70,70,70,70,70,70,70,71,71,71,71,71,71,71,71,
72,72,72,72,72,72,72,72,73,73,73,73,73,73,73,73,74,74,74,74,74,74,74,74,75,75,75,75,75,75,75,75,
76,76,76,76,76,76,76,76,77,77,77,77,77,77,77,77,78,78,78,78,78,78,78,78,79,79,79,79,79,79,79,79};

const	u8	T_Alaw_encodeH[]={		/* 113个数据, 避免溢出 */
 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
 96, 96, 97, 97, 98, 98, 99, 99,100,100,101,101,102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,111,111,
112,112,112,112,113,113,113,113,114,114,114,114,115,115,115,115,116,116,116,116,117,117,117,117,118,118,118,118,119,119,119,119,
120,120,120,120,121,121,121,121,122,122,122,122,123,123,123,123,124,124,124,124,125,125,125,125,126,126,126,126,127,127,127,127,127};


const	u16	T_Alaw_decode[128]={
0x0000,0x0010,0x0020,0x0030,0x0040,0x0050,0x0060,0x0070,0x0080,0x0090,0x00A0,0x00B0,0x00C0,0x00D0,0x00E0,0x00F0,
0x0100,0x0110,0x0120,0x0130,0x0140,0x0150,0x0160,0x0170,0x0180,0x0190,0x01A0,0x01B0,0x01C0,0x01D0,0x01E0,0x01F0,
0x0200,0x0220,0x0240,0x0260,0x0280,0x02A0,0x02C0,0x02E0,0x0300,0x0320,0x0340,0x0360,0x0380,0x03A0,0x03C0,0x03E0,
0x0400,0x0440,0x0480,0x04C0,0x0500,0x0540,0x0580,0x05C0,0x0600,0x0640,0x0680,0x06C0,0x0700,0x0740,0x0780,0x07C0,
0x0800,0x0880,0x0900,0x0980,0x0A00,0x0A80,0x0B00,0x0B80,0x0C00,0x0C80,0x0D00,0x0D80,0x0E00,0x0E80,0x0F00,0x0F80,
0x1000,0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,0x1E00,0x1F00,
0x2000,0x2200,0x2400,0x2600,0x2800,0x2A00,0x2C00,0x2E00,0x3000,0x3200,0x3400,0x3600,0x3800,0x3A00,0x3C00,0x3E00,
0x4000,0x4400,0x4800,0x4C00,0x5000,0x5400,0x5800,0x5C00,0x6000,0x6400,0x6800,0x6C00,0x7000,0x7400,0x7800,0x7C00};

/**************** A率压缩 *******************
  将12bit ADC线性二进制值数据压缩成8bit A-law数据并存入缓冲
	Convert 12 Bit Liner Data to 8bit ALAW data
**************************************************/
void Alaw_encode(u16 adc)	//输入12bit无符号的ADC值
{
	u8	law_data;
	u8	minus;

	if(adc & 0x0800)	minus = 0x00,	adc = adc - 2048;	// >= 2048为正, 0~2047, 以中间值2048为0点.
	else				minus = 0x80,	adc = 2048 - adc;	//  < 2048为负. 1~2048

	if((adc & 0xff00) == 0)	law_data = T_Alaw_encode[(u8)adc];	//0~255 --> 0~79
	else					law_data = T_Alaw_encodeH[(u8)((adc - 256)/16)];	//256~2047 --> 80~127

	voice_buff[wr_index] = (law_data + minus) ^ 0x55;	//数据写入缓冲，偶数位取反	循环队列，指向下一个数据
	wr_index++;
	wr_index &= VOICE_BUFF_MASK;	//溢出处理
}

//**************** A率解压缩 *******************
//  将8bit A-law数据解压缩成16bit数据并输出到DAC
//	Convert 8bit Alaw data to 16 Bit Liner Data
//**************************************************
void Alaw_decode(void)	//解压缩后得到16位有符号DAC
{
	u8	law_data;
	law_data = voice_buff[rd_index] ^ 0x55;		//Get Complement 偶数位取反, 从缓冲读数据字节，偶数位取反，循环队列，指向下一个数据
	dac = T_Alaw_decode[law_data & 0x7f];	// 解压缩码
	if(law_data & 0x80)	dac = 0-dac;		//如果是负的, 转成有符号
	rd_index++;
	rd_index &= VOICE_BUFF_MASK;	//溢出处理
}


//====================== I2S初始化函数 ==================================================
#define	MCKOE		1		//I2S主时钟输出控制, 0:禁止I2S主时钟输出, 1:允许I2S主时钟输出

#define	I2SEN		0x04	//I2S模块使能, 0x00:禁止, 0x04:允许
#define	I2S_MODE	2		//I2S模式, 0:从机发送模式, 1:从机接收模式, 2:主机发送模式, 3:主机接收模式,

#define	PCMSYNC		0		//PCM帧同步, 0: 短帧同步, 1: 长帧同步
#define	STD_MODE	0		//I2S标准选择, 0: I2S飞利浦标准, 1: MSB左对齐标准, 2:LSB右对齐标准, 3:PCM标准, CS4334、CS4344使用0:I2S飞利浦标准，PT8211使用1: MSB左对齐标准。
#define	CKPOL		0		//I2S稳态时钟极性, 0:时钟稳定状态为低电平, 1:时钟稳定状态为高电平
#define	DATLEN		0		//数据长度, 0:16位, 1:24位, 2:32位, 3:保留
#define	CHLEN		0		//通道长度(每个音频通道的位数), 0:16位, 1: 32位

#define I2S_MCLKDIV		(FOSC/(8*16*2*SampleRate))	//MCLK分频系数, 对于双声道16bit.
#define I2S_BCLKDIV		(FOSC/(16*2*SampleRate))		//BCLK分频系数, 对于双声道16bit.

void	I2S_config(void)
{
	I2SMD = 0xff;					//内部保留字节,需设置为FFH
	I2SSR = 0x00;					//状态寄存器清0
	I2SCR = 0x80+0x00;				//使能发送缓冲区空中断(0x80), +0x00:Motorola格式, +0x10:TI格式
	HSCLKDIV    = 1;				//高速时钟分频器 1~255 (默认2)
	I2S_CLKDIV = 1;					//I2S主时钟分频
	I2SMCKDIV  = I2S_MCLKDIV;					//I2S时钟分频，I2SMCLK = 主频/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV,  或I2SMCLK = PLLCLK/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV
	I2SPRH = (MCKOE << 1) + (I2S_BCLKDIV & 1);	//设置I2S_BMCLK分频系数的bit0, 并允许或禁止输出MCLK。
	I2SPRL = I2S_BCLKDIV/2;						//设置I2S_BMCLK分频系数的bit8~bit1
	I2SCFGH = I2S_MODE;				//设置I2S模式为主机发送模式
	I2SCFGL = (PCMSYNC << 7) + (STD_MODE << 4) + (CKPOL << 3) + (DATLEN << 1) + CHLEN;
	P_SW3 = (P_SW3 & 0x3f) | (1<<6);	//I2S端口切换, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-7-21
										//             1: P1.7(BCLK) P1.6(MCLK) P1.5(SD) P1.4(WS),
										//             2: P2.3(BCLK) P2.2(MCLK) P2.1(SD) P2.0(WS),
										//             3: P4.3(BCLK) P1.6(MCLK) P4.1(SD) P4.0(WS),
	I2SCFGH |= I2SEN;                //使能I2S模块
}


//====================== I2S中断函数 ==================================================
void I2S_ISR(void) interrupt I2S_VECTOR
{
	u16	j;
	if (I2SSR & 0x02)				//发送缓冲区空
	{
		I2SDRH = (u8)(dac /256);	//更新音频数据, 单声道不分左右
		I2SDRL = (u8)(dac %256);

		if((I2SSR & 0x04) == 0)		//左声道,  解码ADPCM，启动ADC
		{
			if(B_record)	//正在录音, 则启动ADC
			{
				ADC_RES = 0;	ADC_RESL = 0;
				ADC_START   =  1;	//启动ADC转换, 完成后自动清零
			}
			if(++cnt_1ms >= (u8)(SampleRate/1000))
			{
				DisplayScan();	//1ms扫描显示一位
				cnt_1ms = 0;
				if(++cnt_20ms == 20)	cnt_20ms = 0, B_20ms = 1;		//20ms时隙
			}
		}
		else	//右声道
		{
			if(B_PlayEn)	//正在播放
			{
				Alaw_decode();	//解压缩后得到16位有符号DAC
				if(++PlayByteCnt >= FileLength)	B_PlayEn = 0, B_stop = 1;	//播放完成了
			}
			else	dac = 0;
			if(B_record)	//正在录音, 读取ADC，A-LAW压缩
			{
				j = (u16)ADC_RES * 256 + (u16)ADC_RESL;
				ADC_FLAG    =  0;	//清除ADC完成(中断)标志
				Alaw_encode(j);	//输入12bit无符号的ADC值
			}
		}
	}
//	I2SSR &= ~0x5B;		//已自动清除中断标志
}

