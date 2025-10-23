/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

AI8051U-USB声卡，I2S中断里更新数据，主频分频做MCLK。采样率48KHz，USB数据192字节一帧，1ms一帧。
用户请先别修改程序, 直接下载"Obj"里的"usb_audio_i2s_48k_16b_2ch.hex"测试. 下载时选择主频36.864MHZ。
本例程使用AI8051U验证，MCU内部ROSC工作于36.864MHz。
AI8051U的I2S接口连接TLV320AIC23B播放声音.

只支持16位PCM格式.

=========================== PCM双声道数据格式 ====================================
16位双声道PCM（小端模式，低字节在前）：
        双字0(4字节)            |         双字1(4字节)            | .......
             采样1              |              采样2              | .......
    左声道     |     右声道     |     左声道     |     右声道     | .......
字节0   字节1  | 字节2   字节3  | 字节4   字节5  | 字节6   字节7  | .......
低字节  高字节 | 低字节  高字节 | 低字节  高字节 | 低字节  高字节 | .......
==================================================================================

******************************************/

#include "stc.h"
#include "i2s.h"
#include "usb.h"
#include "clock.h"
#include "TLV320AIC23.h"


/*************	IO口定义	**************/
void Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock


void sys_init();
void clk_trim();
void  delay_ms(u16 ms);


#ifdef DEBUG
    void show8(BYTE dat);
    void show16(WORD dat);
#endif

u8 	LED8[8];		//显示缓冲
u8	display_index;	//显示位索引
bit	B_1ms;			//1ms标志
u8	KeyCode;		//给用户使用的键码
u8 IO_KeyState;		//行列键盘变量
u8	cnt20ms;

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


const u8  t_display[]={						//标准字库
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

const u8  T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//位码

u8		KeyState, KeyCode;	//按键

u16		HeadPhoneVol;		//耳机音量, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.
void	SetHeadPhoneVolume(u16 vol);
void	IO_KeyScan(void);	//50ms call
void	DisplayScan(void);


void main()
{
	u8	i;

    sys_init();
    clock_init();
    i2s_init();
    usb_init();
    EA = 1;

	for(i=0; i<8; i++)	LED8[i] = DIS_BLACK;	//上电消隐
	HeadPhoneVol = 80 - 20;	//默认音量60, 最大音量80
	AIC23_Init();
	delay_ms(50);
	AIC32_InitSet();
	LED8[6] = HeadPhoneVol / 10;	//显示音量
	LED8[7] = HeadPhoneVol % 10;
	SetHeadPhoneVolume(HeadPhoneVol);	//设置音量

    while (1)
    {
        if (DeviceState != DEVSTATE_CONFIGURED)
            continue;

        clock_trim();           //实时动态微调

        if(B_2ms)	//2ms定时时隙
        {
			B_2ms = 0;
			DisplayScan();	//1ms扫描显示一位

			if(++cnt20ms == 20)	//20ms时隙
			{
				cnt20ms = 0;
				IO_KeyScan();		//扫描按键
				if(KeyCode != 0)	//有键按下
				{
					if(KeyCode == K6)	//音量+
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

#ifdef DEBUG
        show16(WaveDumpSize);
        show8(IRTRIM);

        if (WaveOverrun)
        {
            WaveOverrun = 0;
            P20 = ~P20;
        }
        if (WaveUnderrun)
        {
            WaveUnderrun = 0;
            P21 = ~P21;
        }
#endif
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



//=================== 设置耳机音量 ==========================================
void	SetHeadPhoneVolume(u16 vol)
{
	AIC23_WriteCmd(R_HeadphoneVolume_L, (LinVol_LRS | LinVol_Mute | (vol+47)));	//耳机输出音量
	AIC23_WriteCmd(R_HeadphoneVolume_R, (RinVol_RLS | RinVol_Mute | (vol+47)));

//	WriteASCII(66,5,Vol/10,0);
//	WriteASCII(72,5,Vol%10,0);
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


void sys_init()
{
	CKCON = 0;
	EAXFR = 1;	//SFR enable

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x00; P5M1 = 0x00;
    P6M0 = 0x00; P6M1 = 0x00;
    P7M0 = 0x00; P7M1 = 0x00;
    P4 = 0xff;
    P3 = 0xff;

    P3M0 &= ~0x03;
    P3M1 |= 0x03;

    IRC48MCR = 0x80;
    while (!(IRC48MCR & 0x01));

    USBCLK = 0x00;
    USBCON = 0x90;
}

#ifdef DEBUG
void show8(BYTE dat)
{
    dat = ~dat;
    P37 = dat & 0x01;
    P36 = dat & 0x02;
    P35 = dat & 0x04;
    P51 = dat & 0x08;
    P50 = dat & 0x10;
    P34 = dat & 0x20;
    P33 = dat & 0x40;
    P32 = dat & 0x80;
}

void show16(WORD dat)
{
    dat = ~dat;
    P46 = dat & 0x0001;
    P00 = dat & 0x0002;
    P01 = dat & 0x0004;
    P02 = dat & 0x0008;
    P03 = dat & 0x0010;
    P04 = dat & 0x0020;
    P52 = dat & 0x0040;
    P53 = dat & 0x0080;
    P05 = dat & 0x0100;
    P06 = dat & 0x0200;
    P07 = dat & 0x0400;
    P10 = dat & 0x0800;
    P11 = dat & 0x1000;
    P47 = dat & 0x2000;
    P14 = dat & 0x4000;
    P15 = dat & 0x8000;
}
#endif
