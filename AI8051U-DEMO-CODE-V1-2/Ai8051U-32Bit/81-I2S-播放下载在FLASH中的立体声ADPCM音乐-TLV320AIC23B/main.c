
/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/


//#include	"config.h"
#include	"stc.h"
#include	"usb.h"
#include	"uart.h"
#include 	<math.h>

#include	"TLV320AIC23.h"
#include	"W25Q128FV.h"

/*************	功能说明	**************
本程序使用AI8051U实验箱V1.2验证。用户先别修改程序，直接下载HEX文件到AI8051U实验箱V1.2来验证，下载时选择主频40.96MHz。

使用AI8051U系列MCU播放下载存储于FLASH中的ADPCM音乐，支持立体声或单声道，采样率固定为32KHz，用户可以自行修改采样率。
音乐文件由下载工具下载。
使用ADPCM是为了减少容量，16MB的FLASH可以播放8分30秒的立体声音乐，2首歌的容量。

******************************************/

/*************	IO口定义	**************/
void Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

/*************	变量定义	**************/
u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u8	MusicType;			//WAV类型, fmttag=0x01-->PCM, 0x02-->Windows ADPCM, 0x06-->A Law, 0x07-->Mu Law, 0x11-->IMA ADPCM
u8	MusicChannel;		//音乐声道
u16	MusicSampleRate;	//采样率
u32	FileLength;			//文件长度(字节)
u32	PlayByteCnt;		//播放字节计数
u16	dac_L, dac_R;		//解码后输出的左后声道DAC值
bit	B_PlayEn;			//允许播放

u16	wr_index;		//写缓冲索引
u16	rd_index;		//读缓冲索引
u32	FlashAddr;		//读FLASH地址
u8	OP_index;		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
u8	RcvTimeOut;
bit	B_DownLoadStart;	//开始下载

//======= ADPCM解码专用变量 ==========
u8		edata ADPCM_Data_L[4];	//左声道局部缓冲
u8		edata ADPCM_Data_R[4];	//右声道局部缓冲
u8		decode;					//编码值
long	delta;				//偏差
long	cur_sample_L;		//左声道解码值
long	cur_sample_R;		//右声道解码值
char	index_L;			//左声道解码索引
char	index_R;			//右声道解码索引
u8		DecodeCnt;			//解码计数
u16		MusicBlock;			//BLOCK大小
bit		B_high_nibble;		//高半字节指示
//====================================

bit B_stop;		//1: 停止录音或放音
u16	second;
u8	cnt_1s;

u8	de_index;		//ADPCM索引
u16	pre_sample;		//前一个采样值
bit	B_high_nibble;	//高半字节标志
u8	adpcm_data;		//ADPCM数据

u8	cnt_1ms;		// 1ms计数，用户层不可见
u8	cnt_20ms;		//20ms计数，用户层不可见
bit	B_20ms;			//20ms时隙，用户层使用并清除
u16	HeadPhoneVol;	//耳机音量, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.

u8 	LED8[8];		//显示缓冲
u8	display_index;	//显示位索引
u8	KeyCode;		//给用户使用的键码
u8	IO_KeyState;	//行列键盘变量



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
u8		CheckFile(u8 *px, u8 const *pc, u8 num);
void 	CDC_StringPrint(u8 *puts);
void	PlayProcess(void);



u8 const C_DownLoad[]="DownLoad";

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

	P3n_pure_input(0x03);	//P3.0(D-)、P3.1(D+)设置为高阻
	IRC48MCR = 0x80;
	while (!(IRC48MCR & 0x01));

	uart_init();
	usb_init();
	EA = 1;
	delay_ms(1500);
//	while(DeviceState != DEVSTATE_CONFIGURED)	{	NOP(3);	}

	TFPU_CLKDIV = 1;		//TFPU分频

	SPI_Config(2, 0);	//(SPI_io, SPI_speed), 参数: 	SPI_io: 切换IO(SS MOSI MISO SCLK), 0: 切换到P1.4 P1.5 P1.6 P1.7,  1: 切换到P2.4 P2.5 P2.6 P2.7, 2: 切换到P4.0 P4.1 P4.2 P4.3,  3: 切换到P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI的速度, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2

	cnt_1ms = 0;	// 1ms计数，用户层不可见
	cnt_20ms = 0;	//20ms计数，用户层不可见
	B_20ms    = 0;	//20ms时隙，用户层使用并清除
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
	LED8[6] = HeadPhoneVol / 10;	//显示音量
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

		if(OP_index != 4)	//非下载模式
		{
			if (RxFlag)		//当RxFlag为1时, 表示已接收到CDC串口数据, 接收的数据大小保存在RxCount里面,每个包最多接收64字节, 数据保存在RxBuffer缓冲区.
			{
				if(RxCount == 1)	KeyCode = RxBuffer[0];	//PC下传键码
				else if(RxCount == 8)
				{
					if(CheckFile(RxBuffer, C_DownLoad, 8) == 0)		//(u8 *px, u8 const *pc, u8 num) 判断是不是下载请求
					{
						if(B_FlashOK)	//FLASH存在
						{
							B_PlayEn = 0;	//停止播放
							CDC_StringPrint("正\xfd在擦除\xfd FLASH，请稍后...\r\n");
							FlashChipErase();	//执行片擦除命令
							OP_index = 3;	//等待擦除完成
							LED8[0] = DIS_;	//显示-EA-
							LED8[1] = DIS_E;
							LED8[2] = DIS_A;
							LED8[3] = DIS_;
							LED8[4] = DIS_BLACK;	//录放时间消隐不显示
						}
					}
				}
				uart_recv_done();	//对接收的数据处理完成后,一定要调用一次这个函数,以便CDC接收下一笔串口数据

				if(KeyCode == 'r')	//返回80字节头文件
				{
					FlashRead_Nbytes(0, TxBuffer, 80);	//(u32 addr, u8 *buffer, u16 size)	读取数据
					uart_send(80);	//触发发送. 将要发送的数据保存在TxBuffer缓冲区中, 然后调用uart_send(n)函数触发数据发送,参数为发送的字节数. 一次最多可发送64K,函数内部会自动进行USB分包.
				}
			}
		}

		if(OP_index != 0)	PlayProcess();


		if(B_20ms)	//1ms时隙
		{
			B_20ms = 0;
			if(RcvTimeOut != 0)	RcvTimeOut--;	//接收超时控制

			if(B_PlayEn)
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

			if(B_stop)	//停止录音或放音
			{
				B_stop = 0;
				B_PlayEn = 0;	//停止播放
				LED8[0] = DIS_S;	//显示stop
				LED8[1] = DIS_T;
				LED8[2] = DIS_O;
				LED8[3] = DIS_P;
				LED8[4] = DIS_BLACK;	//录放时间消隐不显示
			}

			IO_KeyScan();
			if(KeyCode != 0)	//有键按下
			{
				if(KeyCode == K2)	//停止播放
				{
					OP_index = 0;	//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
					B_PlayEn = 0;	//停止播放
					B_stop   = 1;
				}
				else if(KeyCode == K3)	//放音
				{
					if(B_FlashOK)	//FLASH存在
					{
						OP_index = 1;	//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
						second = 0;
						LED8[0] = DIS_P;	//显示P
						LED8[1] = 0;
						LED8[2] = 0+DIS_DOT;
						LED8[3] = 0;
						LED8[4] = 0;		//放音时间
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



u8 const F_RIFF[]="RIFF";			//文件头 00H~03H
u8 const F_WAVEfmt[]="WAVEfmt ";	//文件头 08H~0FH
u8 const F_fact[]="fact";			//文件头 28H~2BH
u8 const F_data[]="data";			//文件头 34H~37H
u8	CheckFile(u8 *px, u8 const *pc, u8 num)
{
	u8	i;
	for(i=0; i<num; i++)
	{
		if(px[i] != pc[i])	return 1;	//字符不相等错误
	}
	return 0;	//字符串正确
}

void	PlayProcess(void)
{
	u16	j;
	if(OP_index == 1)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
	{
		FlashRead_Nbytes(0, voice_buff, 100);//(u32 addr, u8 *buffer, u16 size)	读取头文件
		j  = 0;
		j  = CheckFile(voice_buff, F_RIFF, 4);			//检测RIFF		文件头 00H~03H
		j |= CheckFile(voice_buff+0x08, F_WAVEfmt, 8);	//检测WAVEfmt	文件头 08H~0FH
		j |= CheckFile(voice_buff+0x28, F_fact, 4);		//检测fact		文件头 28H~2BH
		j |= CheckFile(voice_buff+0x34, F_data, 4);		//检测data		文件头 34H~37H
		if((j == 0) && (voice_buff[0x14]==0x11) && (voice_buff[0x15]==0x00))	//文件头 14H~15H 为IMA-ADPCM的类型标识0x0011
		{
			B_PlayEn  = 0;	//停止播放
			MusicChannel    = voice_buff[0x16];	//声道数, 小端模式, [0x16] [0x17]
			MusicSampleRate = (u16)voice_buff[0x19]*256 + voice_buff[0x18];	//采样率, 小端模式, [0x18] [0x19] [0x1a] [0x1b]
			FileLength      = ((u32)voice_buff[0x3b] << 24) + ((u32)voice_buff[0x3a] << 16) + (u32)voice_buff[0x39]*256 + voice_buff[0x38];	//数据字节长度, 小端模式, [0x38] [0x39] [0x3a] [0x3b]
			MusicBlock      = (u16)voice_buff[0x21]*256+voice_buff[0x20];	//数据BLOCK长度(ADPCM使用), 小端模式, [0x20] [0x21]
			MusicBlock--;
			FlashAddr = 0x003c;	//读FLASH的地址
			FlashRead_Nbytes(FlashAddr, voice_buff, 1024);	//(u32 addr, u8 *buffer, u16 size)	读取数据
			FlashAddr += 1024;
			FlashRead_Nbytes(FlashAddr, voice_buff+1024, 1024);	//(u32 addr, u8 *buffer, u16 size)	读取数据
			FlashAddr += 1024;
			wr_index = 2048;	//写缓冲索引
			rd_index = 0;		//读缓冲索引
			PlayByteCnt = 0;	//播放字节计数清0
			dac_L = 0;	//初始DAC值
			dac_R = 0;	//初始DAC值
			B_PlayEn = 1;	//启动播放
			OP_index = 2;
		}
		else OP_index = 0, B_stop = 1;	//无IMA-ADPCM文件
	}

	else if(OP_index == 2)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
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

	else if(OP_index == 3)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
	{
		if(FlashCheckBusy() == 0)
		{
			CDC_StringPrint("FLASH擦除\xfd完成! 请发送ADPCM音乐文件!\r\n");
			FlashAddr  = 0;
			MusicBlock = 0;
			RcvTimeOut = 0;
			B_DownLoadStart = 0;
			OP_index = 4;		//等待下传数据
			LED8[1] = DIS_D;	//显示-DL-
			LED8[2] = DIS_L;
		}
	}
	else if(OP_index == 4)		//操作索引, 0:无操作, 1:读取头文件处理并启动播放, 2: 读取数据, 3:擦除FLASH, 4:写入FFLASH
	{
		if (RxFlag)			//当RxFlag为1时, 表示已接收到CDC串口数据, 接收的数据大小保存在RxCount里面,每个包最多接收64字节, 数据保存在RxBuffer缓冲区.
		{
			B_DownLoadStart  =1;
			RcvTimeOut = 5;	// 超时100ms计数
			if(RxCount == 64)	//完整的一块数据64字节
			{
				if((FlashAddr == 0) && (MusicBlock == 0))	//收到第一帧数据
					FileLength   = ((u32)RxBuffer[7] << 24) + ((u32)RxBuffer[6] << 16) + (u32)RxBuffer[5]*256 + RxBuffer[4]+8;	//文件总长-8(即减去RIFF和本段4字节), 小端模式, [0x04] [0x05] [0x06] [0x07]
				for(j=0; j<64; j++)	voice_buff[MusicBlock++] = RxBuffer[j];	//暂存数据
				if(MusicBlock >= 256)	//满一页，则写FLASH
				{
					FlashWrite_Nbytes(FlashAddr, voice_buff, 256);	//u32 addr, u8 *buffer, u16 size)
					FlashAddr += 256;
					if(FlashAddr >= FileLength)	OP_index = 0, B_stop = 1;	//下载完成
					MusicBlock = 0;
				}
			}
			else	//不满64字节，则是最后剩余的字节
			{
				if((RxCount == 1) && (RxBuffer[0] == 'c'))	OP_index = 0;	//取消下载
				for(j=0; j<RxCount; j++)	voice_buff[MusicBlock++] = RxBuffer[j];	//暂存数据
				FlashWrite_Nbytes(FlashAddr, voice_buff, MusicBlock);	//u32 addr, u8 *buffer, u16 size)
				FlashAddr += MusicBlock;
				OP_index = 0;	//下载完成
			}

			uart_recv_done();               //对接收的数据处理完成后,一定要调用一次这个函数,以便CDC接收下一笔串口数据

		}
		if(B_DownLoadStart && (RcvTimeOut == 0))	B_DownLoadStart = 0, OP_index = 0, B_stop = 1;	//超时结束
		if(OP_index == 0)	CDC_StringPrint("下载结束!\r\n");
	}
}



//========================================================================
// 函数: void CDC_StringPrint(u8 *puts)
// 描述: 串口1字符串打印函数
// 参数: puts: 字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2018-4-2
// 备注:
//========================================================================
void CDC_StringPrint(u8 *puts)
{
	u16	j;
    for (j=0; *puts != 0;	puts++)	TxBuffer[j++] = *puts;
	uart_send(j);	//触发发送. 将要发送的数据保存在TxBuffer缓冲区中, 然后调用uart_send(n)函数触发数据发送,参数为发送的字节数. 一次最多可发送64K,函数内部会自动进行USB分包.
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

// IMA-ADPCM的表格数据
char idata index_adjust[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};

u16 const step_table[89] =
{
	7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,
	50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,
	408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,
	2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
	10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767
};

void I2S_ISR(void) interrupt 62		//输出2个正弦波
{
	if (I2SSR & 0x02)				//发送缓冲区空
	{
		if((I2SSR & 0x04) == 0)		//左声道
		{
			I2SDRH = (u8)(dac_L /256);		//发送下一帧音频数据
			I2SDRL = (u8)(dac_L %256);

			if(++cnt_1ms >= (u8)(SampleRate/1000))
			{
				DisplayScan();	//1ms扫描显示一位
				cnt_1ms = 0;
				if(++cnt_20ms == 20)	cnt_20ms = 0, B_20ms = 1;		//20ms时隙
			}
		}
		else	//右声道  6~8us @40.96MHz
		{
			I2SDRH = (u8)(dac_R /256);		//发送下一帧音频数据
			I2SDRL = (u8)(dac_R %256);

			if(B_PlayEn)	//正在播放
			{
				if(PlayByteCnt >= FileLength)	B_PlayEn = 0, B_stop = 1;	//播放完成了
				if(MusicChannel == 1)	//ADPCM 单声道
				{
					if((rd_index & MusicBlock) == 0)	//BLOCK开始, 一般是256或512或1024字节一个BLOAK
					{
						dac_L = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// 未压缩的采样值, 小端模式
						rd_index += 2;	//指向index
						index_L = voice_buff[rd_index];	//index值
						rd_index     +=2;
						PlayByteCnt  += 4;	//已播放字节+4个字节
						B_high_nibble = 0;
						dac_L += 32768;		//转成无符号线性
						cur_sample_L  = dac_L;
						cur_sample_L -= 32768;	//转回有符号
					}
					else	//不是BLOCK开始
					{
						decode = voice_buff[rd_index];	//从缓冲读数据字节
						if(B_high_nibble)		//处理高半字节
						{
							decode >>= 4;
							PlayByteCnt++;	//已播放字节+1字节
							rd_index++;		//指向下一个数据
							rd_index &= VOICE_BUFF_MASK;	//溢出处理
						}
						decode &= 0x0f;
						delta = ((u32)step_table[index_L] * ((decode & 0x07)*2 +1)) / 8;	// 计算delta
						if(decode & 8 )		delta = -delta;	//负的delta
						cur_sample_L += delta;	//计算出当前的波形数据
							 if(cur_sample_L >= 32768)	dac_L = 65535;
						else if(cur_sample_L < -32768)	dac_L = 0;
						else 							dac_L = (u16)(cur_sample_L + 32768);

						index_L += index_adjust[decode];
							 if (index_L < 0)	index_L = 0;
						else if (index_L > 88)	index_L = 88;

						B_high_nibble = ~B_high_nibble;
					}
						dac_R = dac_L;		//单声道
				}

				else	//ADPCM 双声道
				{
					if((rd_index & MusicBlock) == 0)	//BLOCK开始, 512或1024字节一个BLOAK
					{
						dac_L = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// 未压缩的采样值
						rd_index += 2;	//指向index
						index_L = voice_buff[rd_index];	//index值
						rd_index += 2;

						dac_R = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// 未压缩的采样值
						rd_index += 2;
						index_R = voice_buff[rd_index];	//index值
						rd_index += 2;

						PlayByteCnt += 8;	//已播放字节数+8个字节
						B_high_nibble = 0;
						dac_L += 32768;			//转成无符号线性
						cur_sample_L = dac_L;
						cur_sample_L -= 32768;	//转回有符号
						dac_R += 32768;			//转成无符号线性
						cur_sample_R = dac_R;
						cur_sample_R -= 32768;	//转回有符号
					}
					else	//不是BLOCK开始
					{
						if((rd_index & 0x0007) == 0)	//2个DWORD 8个字节
						{
							ADPCM_Data_L[0] = voice_buff[rd_index++];	//不用循坏，时间尽量短
							ADPCM_Data_L[1] = voice_buff[rd_index++];
							ADPCM_Data_L[2] = voice_buff[rd_index++];
							ADPCM_Data_L[3] = voice_buff[rd_index++];
							ADPCM_Data_R[0] = voice_buff[rd_index++];	//不用循坏，时间尽量短
							ADPCM_Data_R[1] = voice_buff[rd_index++];
							ADPCM_Data_R[2] = voice_buff[rd_index++];
							ADPCM_Data_R[3] = voice_buff[rd_index];		//最后一个读索引不+1, 处理完这些数据才+1，避免写入缓冲误判
							B_high_nibble = 0;
							DecodeCnt     = 0;
						}

						//================ 左声道解压缩 ============================
						decode = ADPCM_Data_L[DecodeCnt];	//从缓冲读数据字节
						if(B_high_nibble)	decode >>= 4;	//处理高半字节
						decode &= 0x0f;

						delta = ((u32)step_table[index_L] * ((decode & 0x07)*2 +1)) / 8;	// 计算delta
						if(decode & 8 )		delta = -delta;	//负的delta
						cur_sample_L += delta;	//计算出当前的波形数据
							 if(cur_sample_L >= 32768)	dac_L = 65535;
						else if(cur_sample_L < -32768)	dac_L = 0;
						else 							dac_L = (u16)(cur_sample_L + 32768);

						index_L += index_adjust[decode];
							 if (index_L < 0)	index_L = 0;
						else if (index_L > 88)	index_L = 88;
						//======================== 左声道解压缩完毕 ==================================

						//======================== 右声道解压缩 ============================
						decode = ADPCM_Data_R[DecodeCnt];	//从缓冲读数据字节
						if(B_high_nibble)			//处理高半字节
						{
							decode >>= 4;
							DecodeCnt++;
							if(DecodeCnt >= 4)	//4个字节都处理完成
							{
								PlayByteCnt += 8;	//已播放字节数+8个字节
								rd_index++;			//指向下一个数据
								rd_index &= VOICE_BUFF_MASK;	//溢出处理
							}
						}
						decode &= 0x0f;

						delta = ((u32)step_table[index_R] * ((decode & 0x07)*2 +1)) / 8;	// 计算delta
						if(decode & 8 )		delta = -delta;	//负的delta
						cur_sample_R += delta;	//计算出当前的波形数据
							 if(cur_sample_R >= 32768)	dac_R = 65535;
						else if(cur_sample_R < -32768)	dac_R = 0;
						else 							dac_R = (u16)(cur_sample_R + 32768);

						index_R += index_adjust[decode];
							 if (index_R < 0)	index_R = 0;
						else if (index_R > 88)	index_R = 88;
						//======================== 右声道解压缩完毕 ==================================

						B_high_nibble = ~B_high_nibble;	//高半字节指示
					}
				}
				dac_L -= 32768;		//转成有符号
				dac_R -= 32768;		//转成有符号
			}
			else
			{
				dac_L = 0;		//无播放静音
				dac_R = 0;		//无播放静音
			}
		}
	}
//	I2SSR &= ~0x5B;		//已自动清楚中断标志
}

