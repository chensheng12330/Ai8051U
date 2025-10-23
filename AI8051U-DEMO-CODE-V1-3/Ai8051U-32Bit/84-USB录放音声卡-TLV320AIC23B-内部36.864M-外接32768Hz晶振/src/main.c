/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

AI8051U-USB录放音声卡:
下行(播放)数据送I2S中断里更新数据，主频分频做MCLK。采样率48KHz，USB数据192字节一帧，1ms一帧。
上行(录音)数据立体声采样率32KHz，USB数据128字节一帧，1ms一帧。

用户请先别修改程序, 直接下载"Obj"里的"usb_audio_i2s_48k_16b_2ch.hex"测试. 下载时选择主频36.864MHZ。
本例程使用AI8051U大学计划实验箱V1.2-2024-8-20验证，MCU内部ROSC工作于36.864MHz。
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

#define  CHANNEL  2		//设置ADC通道(咪头通道),   取值为0~7, 对应P1.0~P1.7, 使用别的ADC输入口则要修改ADC初始化函数.

/*************	IO口定义	**************/
void 	Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

//=============== 话筒相关变量 =====================================
#define	VOICE_BUFF_LENGTH	8192	//一定是2^n
#define	VOICE_BUFF_MASK		(VOICE_BUFF_LENGTH-1)
#define S_BUFFER_SIZE		(VOICE_BUFF_LENGTH / 4)
#define S_BUFFER_MASK		(BUFFER_SIZE - 1)
#define S_OVERRUN_POINT		(BUFFER_SIZE / 3 * 2)
#define S_UNDERRUN_POINT	(BUFFER_SIZE / 3)

u16	wr_index, rd_index;
u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u16 S_WaveDumpSize;	//已缓存的声音数据, 低于一定值插补一个值, 高于一定值丢去一个值
u16	adc;

bit	B_MicEn;
u8	Trim_UpDown;		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
//==================================================================


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
void	ADC_config(void);
void 	PWMA_config(void);


u8 xdata buf[128];


void main()
{
	u16	j;

	P0M0 = 0;	//用于输出测试信号
	P0M1 = 0;

    sys_init();
    clock_init();
    i2s_init();
    usb_init();
    EA = 1;

	for(j=0; j<8; j++)	LED8[j] = DIS_BLACK;	//上电消隐

	HeadPhoneVol = 80 - 20;	//默认音量60, 最大音量80
	AIC23_Init();
	delay_ms(50);
	AIC32_InitSet();
	LED8[6] = HeadPhoneVol / 10;	//显示音量
	LED8[7] = HeadPhoneVol % 10;
	SetHeadPhoneVolume(HeadPhoneVol);	//设置音量

    for (j = 0; j < 128; j++)	buf[j] = 0;	//空数据
	wr_index = 0;
	rd_index = 0;
	S_WaveDumpSize = 0;		//已缓存的声音数据, 低于一定值插补一个值, 高于一定值丢去一个值
	B_MicEn = 0;
	Trim_UpDown = 0;		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
	ADC_config();
	PWMA_config();


    while (1)
    {
        if (DeviceState != DEVSTATE_CONFIGURED)
            continue;

        clock_trim();	//实时动态微调IRC频率

        if (UsbReq)			//请求上传下一包录音数据
        {
            UsbReq = 0;

			if (B_MicEn)
			{
				j = (wr_index - rd_index) & VOICE_BUFF_MASK;	//已有缓冲长度
				if(j > 256)		//有足够多的数据才上传
				{
					USB_SendData(voice_buff+rd_index);	//上传录音数据
					rd_index = (rd_index + 128) & VOICE_BUFF_MASK;
					S_WaveDumpSize -= 32;	// 32个采样点 * 2字节采样 *2通道 = 128字节
				}
				else	USB_SendData(buf);	//上传录音数据(空包)

				if (S_WaveDumpSize < S_UNDERRUN_POINT)      //缓存数据偏/少
				{
					Trim_UpDown = 1;		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
				}
				else if (S_WaveDumpSize > S_OVERRUN_POINT)  //缓存数据偏多
				{
					Trim_UpDown = 2;		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
				}
			}
			else	USB_SendData(buf);	//上传录音数据(空包)
        }

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



//========================================================================
// 函数: void PWMA_config(void)
// 描述: PWM配置函数。
// 参数: noe.
// 返回: none.
// 版本: V1.0, 2022-3-15
// 备注:
//========================================================================
void PWMA_config(void)
{
	PWMA_ENO    = 0;	// IO输出禁止
	PWMA_IER    = 0;	// 禁止中断
	PWMA_SR1    = 0;	// 清除状态
	PWMA_SR2    = 0;	// 清除状态

	PWMA_PSCRH = 0x00;		// 预分频寄存器, 分频 Fck_cnt = Fck_psc/(PSCR[15:0}+1), 边沿对齐PWM频率 = SYSclk/((PSCR+1)*(AAR+1)), 中央对齐PWM频率 = SYSclk/((PSCR+1)*(AAR+1)*2).
	PWMA_PSCRL = 0x00;
	PWMA_DTR   = 0;			// 死区时间配置, n=0~127: DTR= n T,   0x80 ~(0x80+n), n=0~63: DTR=(64+n)*2T,
							//				0xc0 ~(0xc0+n), n=0~31: DTR=(32+n)*8T,   0xE0 ~(0xE0+n), n=0~31: DTR=(32+n)*16T,
	PWMA_ARRH   = (u8)((FOSC/32000-1)/256);	// 自动重装载寄存器,  控制PWM周期
	PWMA_ARRL   = (u8)((FOSC/32000-1)%256);
	PWMA_CR2    = 0x20;		// 更新事件为TRG0, 用于触发ADC

	PWMA_CCER1  = 0;	// 捕获/比较使能寄存器1
	PWMA_CCER2  = 0;	// 捕获/比较使能寄存器2

	PWMA_BKR    = 0x80;		// 主输出使能 相当于总开关
	PWMA_CR1    = 0x81;		// 使能计数器, 允许自动重装载寄存器缓冲, 边沿对齐模式, 向上计数,  bit7=1:写自动重装载寄存器缓冲(本周期不会被打扰), =0:直接写自动重装载寄存器本(周期可能会乱掉)
	PWMA_EGR    = 0x01;		// 产生一次更新事件, 清除计数器和预分频计数器, 装载预分频寄存器的值
}

//	PWMA_PS   = (0<<6)+(0<<4)+(0<<2)+0;	//选择IO, 4项从高到低(从左到右)对应PWM1 PWM2 PWM3 PWM4, 0:选择P1.x, 1:选择P2.x, 2:选择P6.x,
//  PWMA_PS    PWM4N PWM4P    PWM3N PWM3P    PWM2N PWM2P    PWM1N PWM1P
//    00       P1.7  P1.6     P1.5  P1.4     P1.3  P1.2     P1.1  P1.0
//    01       P0.7  P0.6     P0.5  P0.4     P0.3  P0.2     P0.1  P0.0
//    02       P2.7  P2.6     P2.5  P2.4     P2.3  P2.2     P2.1  P2.0
//    03        --    --       --    --       --    --       --    --

//========================================================================
// 函数: void	ADC_config(void)
// 描述: ADC初始化函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-8-1
// 备注:
//========================================================================
#define D_ADC_POWER	(1<<7)	/* ADC电源，1开启，0关闭 */
#define D_ADC_START	(1<<6)	/* 启动转换，自动清0 */
#define D_ADC_FLAG	(1<<5)	/* 完成标志，软件清0 */
#define D_ADC_EPWMT	(1<<4)	/* 允许PWMA触发ADC */

#define	D_ADC_SPEED	5		/* 0~15, ADC时钟 = SYSclk/2/(n+1) */
#define	D_RES_FMT	(1<<5)	/* ADC结果格式 0: 左对齐, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: 右对齐, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */
#define CSSETUP		(0<<7)	/* 0~1,  ADC通道选择时间      0: 1个ADC时钟, 1: 2个ADC时钟,  默认0(默认1个ADC时钟)	*/
#define CSHOLD		(0<<5)	/* 0~3,  ADC通道选择保持时间  (n+1)个ADC时钟, 默认1(默认2个ADC时钟)					*/
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
	ADC_EPWMT   =  1;	//允许PWM触发ADC
	EADC  = 1;	//允许ADC中断
	PADCH = 1;	//ADC 中断优先级高位
	PADC  = 1;	//ADC 中断优先级
}


#define	HighPassEnable	1		// 1: 允许高通滤波，消除直流偏移, 0: 禁止高通滤波

		//	32741, -32741, 0, -32715, 0			//1阶巴特沃斯高通  8Hz/32000Hz
#define	COEF_A0		 32741
#define	COEF_B0		-32715
#if (HighPassEnable ==1)
long	Xin1, Xin0, Yout0, out;
#endif

void ADC_ISR (void) interrupt ADC_VECTOR	//@36.864MHz  无高通滤波 2us，加一阶高通滤波4.2us
{
	ADC_FLAG    =  0;	//清除ADC完成(中断)标志

	#if (HighPassEnable ==1)	//允许高通滤波，消除直流偏移
		adc = (u16)ADC_RES * 256 + (u16)ADC_RESL;	//读取ADC值，转成双极性
		Xin1 = Xin0;	Xin0 = (long)adc - 2048;	//输入ADC值为12位, 转为双极性
		out = (COEF_A0 * (Xin0 - Xin1) - COEF_B0 * Yout0)/32768;	//一阶高通
		Yout0 = out;
		if(out >=  2048)	out =  2047;	//输出限幅
		if(out <= -2048)	out = -2047;	//输出限幅
		adc = (u16)out;
	#else
		adc = ((u16)ADC_RES * 256 + (u16)ADC_RESL)-2048;	//读取ADC值，转成双极性
	#endif

	if(S_WaveDumpSize < (S_BUFFER_SIZE-4))	//避免溢出
	{
		voice_buff[wr_index++] = (u8)(adc % 256);	//小端模式
		voice_buff[wr_index++] = (u8)(adc / 256);
		voice_buff[wr_index++] = (u8)(adc % 256);	//小端模式
		voice_buff[wr_index++] = (u8)(adc / 256);
		wr_index &= VOICE_BUFF_MASK;	//防止溢出
		S_WaveDumpSize++;	//已缓存的声音数据, 低于一定值插补一个值, 高于一定值丢去一个值

		if(S_WaveDumpSize >= ( S_BUFFER_SIZE/2))	B_MicEn = 1;	//缓冲过半才启动传输
		if(S_WaveDumpSize < 256)					B_MicEn = 0;	//缓冲过少关闭传输

		if(Trim_UpDown == 1)		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
		{
			Trim_UpDown = 0;
			voice_buff[wr_index++] = (u8)(adc % 256);	//小端模式
			voice_buff[wr_index++] = (u8)(adc / 256);
			voice_buff[wr_index++] = (u8)(adc % 256);	//小端模式
			voice_buff[wr_index++] = (u8)(adc / 256);
			wr_index &= VOICE_BUFF_MASK;	//防止溢出
			S_WaveDumpSize++;	//已缓存的声音数据, 低于一定值插补一个值, 高于一定值丢去一个值
		}
		if(Trim_UpDown == 2)		//0:无操作, 1:请求增加一个数据,  2:请求减少一个数据
		{
			Trim_UpDown = 0;
			wr_index -= 4;
			wr_index &= VOICE_BUFF_MASK;	//防止溢出
			if(S_WaveDumpSize != 0)		S_WaveDumpSize--;		//已缓存的声音数据, 低于一定值插补一个值, 高于一定值丢去一个值
		}
	}
}

