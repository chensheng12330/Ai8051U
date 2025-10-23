#ifndef __CONFIG_H__
#define __CONFIG_H__



//����I2S��ʱ�ӱ����� 256*SampleRate��������������������16λ����������ʱ��WS=SampleRate������ʱ��BCLK=32*SampleRate����ʱ��MCLK=8*BCLK=256*SampleRate��

//	#define FOSC			36864000UL		//������ʱ��
//	#define SampleRate		48000			//���������
//	#define SampleRate		36000			//���������
//	#define SampleRate		24000			//���������
//	#define SampleRate		16000			//���������
//	#define SampleRate		12000			//���������
//	#define SampleRate		8000			//���������
//	#define SampleRate		6000			//���������

	#define FOSC			40960000UL		//������ʱ��
//	#define FOSC			32768000UL		//������ʱ��
	#define SampleRate		32000			//���������
//	#define SampleRate		16000			//���������
//	#define SampleRate		 8000			//���������

//	#define FOSC			33868800UL		//������ʱ��
//	#define SampleRate		44100			//���������
//	#define SampleRate		22050			//���������
//	#define SampleRate		11025			//���������


#define	VOICE_BUFF_LENGTH	16384		//������4096��8192��16384��3����֮һ
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
