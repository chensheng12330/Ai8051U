#ifndef __CONFIG_H__
#define __CONFIG_H__



//对于I2S的时钟必须是 256*SampleRate的整数倍。对于立体声16位，左右声道时钟WS=SampleRate，数据时钟BCLK=32*SampleRate，主时钟MCLK=8*BCLK=256*SampleRate。

//	#define FOSC			36864000UL		//定义主时钟
//	#define SampleRate		48000			//定义采样率
//	#define SampleRate		36000			//定义采样率
//	#define SampleRate		24000			//定义采样率
//	#define SampleRate		16000			//定义采样率
//	#define SampleRate		12000			//定义采样率
//	#define SampleRate		8000			//定义采样率
//	#define SampleRate		6000			//定义采样率

	#define FOSC			40960000UL		//定义主时钟
//	#define FOSC			32768000UL		//定义主时钟
	#define SampleRate		32000			//定义采样率
//	#define SampleRate		16000			//定义采样率
//	#define SampleRate		 8000			//定义采样率

//	#define FOSC			33868800UL		//定义主时钟
//	#define SampleRate		44100			//定义采样率
//	#define SampleRate		22050			//定义采样率
//	#define SampleRate		11025			//定义采样率


#define	VOICE_BUFF_LENGTH	16384		//必须是4096、8192、16384这3个数之一
#define	VOICE_BUFF_MASK		(VOICE_BUFF_LENGTH-1)



//=================================================

#define EN_EP1IN
#define EN_EP2IN
//#define EN_EP3IN
//#define EN_EP4IN
//#define EN_EP5IN
#define EN_EP1OUT
//#define EN_EP2OUT
//#define EN_EP3OUT
//#define EN_EP4OUT
//#define EN_EP5OUT

#define EP0_SIZE                64

#ifdef EN_EP1IN
#define EP1IN_SIZE              64
#endif
#ifdef EN_EP2IN
#define EP2IN_SIZE              64
#endif
#ifdef EN_EP3IN
#define EP3IN_SIZE              64
#endif
#ifdef EN_EP4IN
#define EP4IN_SIZE              64
#endif
#ifdef EN_EP5IN
#define EP5IN_SIZE              64
#endif
#ifdef EN_EP1OUT
#define EP1OUT_SIZE             64
#endif
#ifdef EN_EP2OUT
#define EP2OUT_SIZE             64
#endif
#ifdef EN_EP3OUT
#define EP3OUT_SIZE             64
#endif
#ifdef EN_EP4OUT
#define EP4OUT_SIZE             64
#endif
#ifdef EN_EP5OUT
#define EP5OUT_SIZE             64
#endif

#endif
