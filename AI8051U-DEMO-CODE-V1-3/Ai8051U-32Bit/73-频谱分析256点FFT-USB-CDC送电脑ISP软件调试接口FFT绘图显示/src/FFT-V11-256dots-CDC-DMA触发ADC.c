
/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/



#include	"stc.h"
#include	"usb.h"
#include	"uart.h"
#include 	<math.h>
#include	"T-A-LAW.h"	//A率对数表, 电平使用对数表示


/*************	功能说明	**************

使用AI8051U系列MCU做256点FFT分析, 上传给aiapp-isp显示频谱. MCU主频:40MHz.
ADC使用DMA处理。
使用AI8051U实验箱V1.1验证, 从示波器输入电路输入音频信号, ADC采样后进行FFT计算, 将结果发给aiapp-isp的示波器显示波形.

采样率: 25600Hz, 采样时间10ms
FFT采样点数: 256点.
FFT频率点数: 128.
FFT频谱频率: 0~12700Hz, 分辨率100Hz.

单片机上传数据协议：
上传一帧数据：FB DAT0 DAT1 ..... DAT127 FF
    FB：帧数据
    DAT0 ... DAT127：128个Y轴数据（数值为0~250），字节序号0~399就是X轴。
    FF：帧数据结束

AI8051U @40MHz : 25.6KHz采样率, 采样时间10ms, 加载硬件浮点库+整数库, 只计算6.5ms, 计算+上传 6.8ms, 计算2次上传一次, 显示刷新频率50Hz, 视频级刷新.


******************************************/


//---------------------------------------------------------------------


/*************	本地常量声明	**************/


/*************	IO定义声明	**************/


/*************	本地变量声明	**************/

#define  CHANNEL  2			  //设置ADC通道(咪头通道),   取值为0~7, 对应P1.0~P1.7, 使用别的ADC输入口则要修改ADC初始化函数.
#define	SAMPLE_RATE	36000	//定义采样率, 实际采样率为25600Hz, 因为ADC DMA自动触发的时间间隔是ADC完成并且DMA保存数据后开始定时的。实际间隔时间 = ADC时间+DMA保存数据时间+间隔时间
#define	SAMPLES		256		//定义采样点数

int	xdata adc_buf1[SAMPLES*3];	//ADC结果存放的数组, [n][0]为每次转换的值, [n][1]高字节为ADC通道号,低字节为取平均值后的余数,  [n][2)]为平均值
int	xdata adc_buf2[SAMPLES*3];	//ADC结果存放的数组, [n][0]为每次转换的值, [n][1]高字节为ADC通道号,低字节为取平均值后的余数,  [n][2)]为平均值
bit	B_ADC_DMA_ok;	//ADC DMA已完成标志(用户标志)

int	xdata	sample[SAMPLES];	// ADC采样拷贝
int xdata	FFT_Real[SAMPLES];	// fft的实部
int xdata	FFT_Image[SAMPLES];	// fft的虚部
u16	xdata 	FFT_A[SAMPLES/2];	// 频点的幅度
u8	cal_cnt;	//计算次数
u8	edata amplitude[SAMPLES/2];	//幅度
bit	B_adc_buf;	// 0: 操作adc_buf1[], 1: 操作adc_buf2[],
bit	B_SampleOk;	// 0: 操作adc_buf1[], 1: 操作adc_buf2[],

u8	KeyCode;


/*************	本地函数声明	**************/


#define	SCALE	1024	//正弦值、余弦值放大系数

//0~180度余弦表，放大了1024倍(256点FFT时用)
const int COS_TABLE[SAMPLES/2]=
{
  1024,  1024,  1023,  1021,  1019,  1016,  1013,  1009,  1004,  999,   993,   987,   980,   972,   964,   955,
   946,   936,   926,   915,   903,   891,   878,   865,   851,  837,   822,   807,   792,   775,   759,   742,
   724,   706,   688,   669,   650,   630,   610,  590,    569,  548,   526,   505,   483,   460,   438,   415,
   392,   369,   345,   321,   297,   273,   249,   224,   200,  175,   150,   125,   100,    75,    50,    25,
     0,   -25,   -50,   -75,  -100,  -125,  -150,  -175,  -200, -224,  -249,  -273,  -297,  -321,  -345,  -369,
  -392,  -415,  -438,  -460,  -483,  -505,  -526,  -548,  -569, -590,  -610,  -630,  -650,  -669,  -688,  -706,
  -724,  -742,  -759,  -775,  -792,  -807,  -822,  -837,  -851, -865,  -878,  -891,  -903,  -915,  -926,  -936,
  -946,  -955,  -964,  -972,  -980,  -987,  -993,  -999, -1004,-1009, -1013, -1016, -1019, -1021, -1023,  -1024
};

//0~180度正弦表，放大了1024倍(256点FFT时用)
const int SIN_TABLE[SAMPLES/2]=
{
     0,    25,    50,    75,   100,   125,   150,   175,   200,   224,   249,   273,   297,   321,   345,   369,
   392,   415,   438,   460,   483,   505,   526,   548,   569,   590,   610,   630,   650,   669,   688,   706,
   724,   742,   759,   775,   792,   807,   822,   837,   851,   865,   878,   891,   903,   915,   926,   936,
   946,   955,   964,   972,   980,   987,   993,   999,  1004,  1009,  1013,  1016,  1019,  1021,  1023,  1024,
  1024,  1024,  1023,  1021,  1019,  1016,  1013,  1009,  1004,   999,   993,   987,   980,   972,   964,   955,
   946,   936,   926,   915,   903,   891,   878,   865,   851,   837,   822,   807,   792,   775,   759,   742,
   724,   706,   688,   669,   650,   630,   610,   590,   569,   548,   526,   505,   483,   460,   438,   415,
   392,   369,   345,   321,   297,   273,   249,   224,   200,   175,   150,   125,   100,    75,    50,    25,
};

//采样存储序列表(倒序)	FFT 256点反序十进制:
const u8 LIST_TAB[256] = {
   0, 128,  64, 192,  32, 160,  96, 224,  16, 144,  80, 208,  48, 176, 112, 240,
   8, 136,  72, 200,  40, 168, 104, 232,  24, 152,  88, 216,  56, 184, 120, 248,
   4, 132,  68, 196,  36, 164, 100, 228,  20, 148,  84, 212,  52, 180, 116, 244,
  12, 140,  76, 204,  44, 172, 108, 236,  28, 156,  92, 220,  60, 188, 124, 252,
   2, 130,  66, 194,  34, 162,  98, 226,  18, 146,  82, 210,  50, 178, 114, 242,
  10, 138,  74, 202,  42, 170, 106, 234,  26, 154,  90, 218,  58, 186, 122, 250,
   6, 134,  70, 198,  38, 166, 102, 230,  22, 150,  86, 214,  54, 182, 118, 246,
  14, 142,  78, 206,  46, 174, 110, 238,  30, 158,  94, 222,  62, 190, 126, 254,
   1, 129,  65, 193,  33, 161,  97, 225,  17, 145,  81, 209,  49, 177, 113, 241,
   9, 137,  73, 201,  41, 169, 105, 233,  25, 153,  89, 217,  57, 185, 121, 249,
   5, 133,  69, 197,  37, 165, 101, 229,  21, 149,  85, 213,  53, 181, 117, 245,
  13, 141,  77, 205,  45, 173, 109, 237,  29, 157,  93, 221,  61, 189, 125, 253,
   3, 131,  67, 195,  35, 163,  99, 227,  19, 147,  83, 211,  51, 179, 115, 243,
  11, 139,  75, 203,  43, 171, 107, 235,  27, 155,  91, 219,  59, 187, 123, 251,
   7, 135,  71, 199,  39, 167, 103, 231,  23, 151,  87, 215,  55, 183, 119, 247,
  15, 143,  79, 207,  47, 175, 111, 239,  31, 159,  95, 223,  63, 191, 127, 255
};


/********************************************************************
函数功能：进行FFT运算。
入口参数：none.
返    回：none.
********************************************************************/
void FFT(void)
{
	u16 i,j;
	u16 BlockSize;
	int tr,ti;
	u8 OffSet1,OffSet2;
	long co,si;

	for(j=0; j<SAMPLES; j+=2)	//先计算2点的
	{
		tr       = sample[j+1];
		FFT_Real[j+1] = (sample[j] - tr);
		FFT_Image[j+1] = 0;
		FFT_Real[j]   = (sample[j] + tr);
		FFT_Image[j]   = 0;
	}

	for(BlockSize=4; BlockSize<=SAMPLES; BlockSize<<=1) //再一层层计算
	{
		for(j=0; j<SAMPLES; j+=BlockSize)
		{
			for(i=0; i<BlockSize/2; i++)
			{
				OffSet1 = SAMPLES/BlockSize * i;
				co = (long)COS_TABLE[OffSet1];
				si = (long)SIN_TABLE[OffSet1];

				OffSet1 = i + j;
				OffSet2 = OffSet1 + BlockSize/2;
				tr = (co*FFT_Real[ OffSet2] + si*FFT_Image[OffSet2]) / SCALE;
				ti = (co*FFT_Image[OffSet2] - si*FFT_Real[ OffSet2]) / SCALE;

				FFT_Real[ OffSet2] = (FFT_Real[ OffSet1] - tr) >> 1;
				FFT_Image[OffSet2] = (FFT_Image[OffSet1] - ti) >> 1;
				FFT_Real[ OffSet1] = (FFT_Real[ OffSet1] + tr) >> 1;
				FFT_Image[OffSet1] = (FFT_Image[OffSet1] + ti) >> 1;
			}
		}
	}
	FFT_Real[0]  = FFT_Real[0]  >> 1;
	FFT_Image[0] = FFT_Image[0] >> 1;
}



//================== USB-CDC返回数据 ======================
void CDC_RetuanData(void)
{
	u8	edata *xp;
	u8	i,j;
	u16	y;

	if(++cal_cnt == 2)	cal_cnt = 0;	//2次刷新一次

	for(j=0; j<128; j++)
	{
		if(cal_cnt == 0)	//减慢一点掉落速度
		{
			if(amplitude[j] != 0)	amplitude[j]--;		//刷新衰减
		}

		y = FFT_A[j];	//取幅度
		if((y & 0xf800) != 0)	y = 2047;	//	if(y >= 2048)	y = 2047;	//限幅
		i = T_Alaw_encode[y];	//取对数, 借用A率压缩的对数, 用户可以自己决定使用什么对数.
		if(amplitude[j] < i)	amplitude[j] = i;	//值更大, 则更新显示
	}
	amplitude[0] = 0;	//第一个是直流电平(0Hz), 不需要, 写0

	if(cal_cnt == 0)
	{
		TxBuffer[0] = 0xfb;	//上传一帧数据：FB DAT0 DAT1 ..... DAT127 FF
		y = 1;		//发送字节数
		xp = &amplitude[0];	//首地址
		amplitude[0] = 0;	//直流电平，不是频率信号
		for(i=0; i<128; i++)
		{
			TxBuffer[y++] = *xp;
			xp++;
		}
		TxBuffer[y++] = 0xff;	//命令结束
		uart_send(y);	//触发发送. 将要发送的数据保存在TxBuffer缓冲区中, 然后调用uart_send(n)函数触发数据发送,参数为发送的字节数. 一次最多可发送64K,函数内部会自动进行USB分包.
	}
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
	 	i = MAIN_Fosc / 6000;
		while(--i)	;
     }while(--ms);
}


void delay(void)
{
	u16	i;
	for	(i=0; i<100; i++);
}

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

#define	D_ADC_SPEED	4		/* 0~15, ADC时钟 = SYSclk/2/(n+1) */
#define	D_RES_FMT	(1<<5)	/* ADC结果格式 0: 左对齐, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: 右对齐, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */
#define CSSETUP		(1<<7)	/* 0~1,  ADC通道选择时间      0: 1个ADC时钟, 1: 2个ADC时钟,  默认0(默认1个ADC时钟)	*/
#define CSHOLD		(1<<5)	/* 0~3,  ADC通道选择保持时间  (n+1)个ADC时钟, 默认1(默认2个ADC时钟)					*/
#define SMPDUTY		23		/* 10~31, ADC模拟信号采样时间  (n+1)个ADC时钟, 默认10(默认11个ADC时钟)				*/
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

void	ADC_DMA_config(void)	//初始化ADC DMA函数
{
	DMA_ADC_CFG2 = 0;		//每个通道转换的次数, 0-->1次, 8-->2次, 9-->4次, 10-->8次, 11-->16次, 12-->32次, 13-->64次, 14-->128次, 15-->256次.
	DMA_ADC_CFG  = (1<<7) + (0<<2) + 0;	//bit7=1: 允许ADC_DMA中断, 0:禁止.  bit3 bit2: ADC_DMA中断优先级(低0~3高), bit1 bit0: ADC_DMA数据总线访问优先级(低0~3高)
	DMA_ADC_STA  = 0;		//bit0: ADC_DMA完成(中断请求)标志, 需要软件清0.
	DMA_ADC_AMTH = (u8)((SAMPLES-1)/256);		//重复次数 = 9DMA_ADC_AMTH*256+DMA_ADC_AMT)+1
	DMA_ADC_AMT  = (u8)((SAMPLES-1)%256);		//
	DMA_ADC_CHSW1 = 0;				//ADC通道使能, bit7~bit0对应通道15~8.
	DMA_ADC_CHSW0 = (1<<CHANNEL);	//ADC通道使能, bit7~bit0对应通道7~0.
	DMA_ADC_ITVH = (u8)(((MAIN_Fosc + SAMPLE_RATE/2) / SAMPLE_RATE) /256);   	/*  ADC_DMA时间间隔寄存器高字节 */
	DMA_ADC_ITVL = (u8)(((MAIN_Fosc + SAMPLE_RATE/2) / SAMPLE_RATE) %256);   /*  ADC_DMA时间间隔寄存器低字节 */
//	DMA_ADC_RXAH = (u8)((u16)(&adc_buf1) /256);	//存放的地址
//	DMA_ADC_RXAL = (u8)((u16)(&adc_buf1) %256);	//存放的地址
//	DMA_ADC_CR   = (1<<7) + (1<<6);		//启动ADC_DMA,  bit7=1:允许ADC_DMA功能, 0:禁止.   bit6=1:触发ADC_DMA功能.
}

//========================================================================
// 函数: void	ADC_DMA_ISR(void) interrupt DMA_ADC_VECTOR
// 描述: ADC DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-8-1
// 备注:
//========================================================================
void	ADC_DMA_ISR(void) interrupt DMA_ADC_VECTOR
{
	if(!B_adc_buf)	//第1个缓冲完成,	触发第2个缓冲
	{
		DMA_ADC_RXAH  = (u8)((u16)(&adc_buf2)/256);	//存放的地址
		DMA_ADC_RXAL  = (u8)((u16)(&adc_buf2)%256);	//存放的地址
	}
	else //第2个缓冲完成,	触发第1个缓冲
	{
		DMA_ADC_RXAH  = (u8)((u16)(&adc_buf1)/256);	//存放的地址
		DMA_ADC_RXAL  = (u8)((u16)(&adc_buf1)%256);	//存放的地址
	}
	DMA_ADC_CR   = (1<<7) + (1<<6);		//启动ADC_DMA,  bit7=1:允许ADC_DMA功能, 0:禁止.   bit6=1:触发ADC_DMA功能.

	DMA_ADC_STA = 0;	//清除ADC_DMA完成(中断请求)标志
	B_SampleOk = B_adc_buf;	//0: 操作adc_buf1[], B1: 操作adc_buf2[]
	B_adc_buf = ~B_adc_buf;		//指向下一个缓冲
	B_ADC_DMA_ok = 1;	//标志已完成
}


//==============================================================================================================
//	*******************		   					main()							*********************************
//===============================================================================================================

void main(void)
{
	u16	i,j;

	EAXFR = 1;	//SFR enable
	WTST  = 0;
	CKCON = 0;

	P0n_standard(0xff);	P0 = 0xff;
	P1n_standard(0xff);	P1 = 0xff;
	P2n_standard(0xff);	P2 = 0xff;
	P3n_standard(0xfc);	P3 = 0xff;
	P4n_standard(0xff);	P4 = 0xff;
	P5n_standard(0xff);	P5 = 0xff;
	P6n_standard(0xff);	P6 = 0xff;
	P7n_standard(0xff);	P7 = 0xff;
	P54 = 1;

	HSCLKDIV    = 1;	//高速时钟分频器 1~255 (默认2)
	TFPU_CLKDIV = 1;	//TFPU分频
//	MCLKO47_DIV(100);	//输出主频分频

	DMAIR = 0x3f;		//算数运算单元
	CLKSEL  = 0x00;						//下载使用40M, PLL=40M/4*12=120MHz, 给FPU浮点库.
	CLKSEL |= 0x80;						//内部PLL输出时钟选择, |=0x80: 选择PLL 144MHz, &=~0x80: 选择96MHz(默认)
	USBCLK  = (USBCLK &~0x60) | (2<<5);	//选择PLL输入时钟分频,保证输入时钟为12M左右, 0: 1分频(对应12MHz), 1: 2分频(对应24MHz), 2: 4分频(对应48MHz), 3: 8分频(对应96MHz)
	USBCLK |= 0x80;						//PLL倍频控制, |=0x80: 使能PLL倍频. &= ~0x80: 禁止PLL倍频
	delay();							//等待PLL锁频
	CLKSEL |= 0x00;						//高速IO时钟源选择, |=0x40: 选择PLLCLK,  &=~0x40: 选择MCLK(默认)
	CLKDIV  = 1; 						//主时钟分频系数, 1~255,  144/5=28.8MHz
//	HSCLKDIV = 1;						//高速时钟分频器 1~255 (默认2)
//	TFPU_CLKDIV = 1;					//TFPU分频
	CLKSEL |= (0<<2);					//主时钟源选择2, 0: MCKSEL选择的时钟源(默认), 1: 内部PLL输出, 2: 内部PLL输出/2, 3: 内部48MHz高速IRC
	CLKSEL |=  0;						//主时钟源选择,  0: 内部高精度IRC(默认), 1: 外部高速晶振, 2: 外部32K晶振, 3: 内部32K低速IRC


	P3n_pure_input(0x03);	//P3.0(D-)、P3.1(D+)设置为高阻
	IRC48MCR = 0x80;
	while (!(IRC48MCR & 0x01));

	uart_init();
	usb_init();
	EA = 1;

	delay_ms(1500);
	while(DeviceState != DEVSTATE_CONFIGURED)	//等待USB初始化完成
	{
		NOP(3);
	}

	ADC_config();
	ADC_DMA_config();

	for(i=0; i<SAMPLES; i++)
	{
		FFT_Real[i]   = 0;	// fft的实部
		FFT_Image[i]  = 0;	// fft的虚部
	}
	for(i=0; i<SAMPLES/2; i++)
	{
		FFT_A[i]     = 0;	// 频率的幅度
		amplitude[i] = 0;	//幅度
	}


	B_adc_buf = 0;	//第1个缓冲
	DMA_ADC_RXAH  = (u8)((u16)(&adc_buf1)/256);	//ADC DMA存放的地址高字节
	DMA_ADC_RXAL  = (u8)((u16)(&adc_buf1)%256);	//ADC DMA存放的地址低字节
	DMA_ADC_CR   = (1<<7) + (1<<6);		//启动ADC_DMA,  bit7=1:允许ADC_DMA功能, 0:禁止.   bit6=1:触发ADC_DMA功能.

	while(1)
	{
		if (RxFlag)                         //当RxFlag为1时,表示已接收到CDC串口数据
											//接收的数据大小保存在RxCount里面,每个包最多接收64字节
											//数据保存在RxBuffer缓冲区
		{
			if((RxCount == 4) && (RxBuffer[0]==0xfe) &&(RxBuffer[1] == 0x01) && (RxBuffer[3] == 0xff))
			KeyCode = RxBuffer[2];			//PC下传键代码：FE 01 DAT0 FF
			uart_recv_done();               //对接收的数据处理完成后,一定要调用一次这个函数,以便CDC接收下一笔串口数据
		}

		if(B_ADC_DMA_ok)    // DMA完成
		{
			B_ADC_DMA_ok = 0;
			if(!B_SampleOk)	//第1个缓冲完成,	数据准备188us @40MHz+整数库+浮点库
			{
				for(i=0,j=0; i<SAMPLES; i++,j+=3)	sample[LIST_TAB[i]] = adc_buf1[j]-2048;
			}
			else //第2个缓冲完成
			{
				for(i=0,j=0; i<SAMPLES; i++,j+=3)	sample[LIST_TAB[i]] = adc_buf2[j]-2048;
			}

			FFT();		//FFT运算 AI8051U 5.5ms @40MHz +整数

			for(i=0; i<SAMPLES/2; i++)
			{
				FFT_A[i] = sqrt((long)FFT_Real[i]*(long)FFT_Real[i] + (long)FFT_Image[i]*(long)FFT_Image[i]);   //计算模值,   KEIL开平方根670us @40MHz +浮点库.
			}

			CDC_RetuanData();	//  AI8051U @40MHz(+整数), 仅计算115us, 计算+上传 475us
		}
	}
}


