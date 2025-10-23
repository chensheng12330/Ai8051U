/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "i2s.h"

BOOL WaveUpdate;                        //���ݸ��±�־λ
BOOL WavePlayEn;                        //����ʹ��
BOOL WaveOverrun;                       //���������־λ
BOOL WaveUnderrun;                      //���������־λ
BYTE WaveMute;                          //�������ƼĴ���
WORD WavePreSamp;                       //I2S���ݻ���
WORD WaveWritePtr;                      //���л�дָ��
WORD WaveReadPtr;                       //���л���ָ��
WORD WaveDumpSize;                      //�������������
WORD xdata WaveBuffer[BUFFER_SIZE];     //�������ݻ�����

u16	cnt_2ms = 0;
bit	B_2ms;

//====================== I2S��ʼ������ ==================================================
#define	MCKOE		1		//I2S��ʱ���������, 0:��ֹI2S��ʱ�����, 1:����I2S��ʱ�����

#define	I2SEN		0x04	//I2Sģ��ʹ��, 0x00:��ֹ, 0x04:����
#define	I2S_MODE	2		//I2Sģʽ, 0:�ӻ�����ģʽ, 1:�ӻ�����ģʽ, 2:��������ģʽ, 3:��������ģʽ,

#define	PCMSYNC		0		//PCM֡ͬ��, 0: ��֡ͬ��, 1: ��֡ͬ��
#define	STD_MODE	0		//I2S��׼ѡ��, 0: I2S�����ֱ�׼, 1: MSB������׼, 2:LSB�Ҷ����׼, 3:PCM��׼, CS4334��CS4344ʹ��0:I2S�����ֱ�׼��PT8211ʹ��1: MSB������׼��
#define	CKPOL		0		//I2S��̬ʱ�Ӽ���, 0:ʱ���ȶ�״̬Ϊ�͵�ƽ, 1:ʱ���ȶ�״̬Ϊ�ߵ�ƽ
#define	DATLEN		0		//���ݳ���, 0:16λ, 1:24λ, 2:32λ, 3:����
#define	CHLEN		0		//ͨ������(ÿ����Ƶͨ����λ��), 0:16λ, 1: 32λ

#define I2S_MCLKDIV		(FOSC/(8*16*2*S_SAMPFREQ))	//MCLK��Ƶϵ��, ����˫����16bit.
#define I2S_BCLKDIV		(FOSC/(16*2*S_SAMPFREQ))	//BCLK��Ƶϵ��, ����˫����16bit.

void i2s_init()
{
	I2SMD = 0xff;					//�ڲ������ֽ�,������ΪFFH
	I2SSR = 0x00;					//״̬�Ĵ�����0
	I2SCR = 0x80+0x00;				//ʹ�ܷ��ͻ��������ж�(0x80), +0x00:Motorola��ʽ, +0x10:TI��ʽ
	HSCLKDIV   = 1;					//HSCLKDIV��ʱ�ӷ�Ƶ
	I2S_CLKDIV = 1;					//I2S��ʱ�ӷ�Ƶ
	I2SMCKDIV  = I2S_MCLKDIV;					//I2Sʱ�ӷ�Ƶ��I2SMCLK = ��Ƶ/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV,  ��I2SMCLK = PLLCLK/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV
	I2SPRH = (MCKOE << 1) + (I2S_BCLKDIV & 1);	//����I2S_BMCLK��Ƶϵ����bit0, ��������ֹ���MCLK��
	I2SPRL = I2S_BCLKDIV/2;						//����I2S_BMCLK��Ƶϵ����bit8~bit1
	I2SCFGH = I2S_MODE;				//����I2SģʽΪ��������ģʽ
	I2SCFGL = (PCMSYNC << 7) + (STD_MODE << 4) + (CKPOL << 3) + (DATLEN << 1) + CHLEN;
//	P_SW3 = (P_SW3 & 0x3f) | (3<<6);	//I2S�˿��л�, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-6-18
	P_SW3 = (P_SW3 & 0x3f) | (1<<6);	//I2S�˿��л�, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-7-21
										//             1: P1.7(BCLK) P1.6(MCLK) P1.5(SD) P1.4(WS),
										//             2: P2.3(BCLK) P2.2(MCLK) P2.1(SD) P2.0(WS),
										//             3: P4.3(BCLK) P1.6(MCLK) P4.1(SD) P4.0(WS),
	I2SCFGH |= I2SEN;                   //ʹ��I2Sģ��

	IP3  |= 0x08;	                    //��I2S�ж�����Ϊ������ȼ�
	IP3H |= 0x08;

    WavePlayEn = 0;
    WaveMute = 1;
    WaveOverrun = 0;
    WaveUnderrun = 0;
    WavePreSamp = 0;
    WaveWritePtr = 0;
    WaveReadPtr = 0;
    WaveDumpSize = 0;
}

void i2s_isr(void) interrupt 62
{
    if (I2SSR & 0x02)	//���ͻ�������
    {
        I2SDRH = WavePreSamp >> 8;      //���׼������������,���������Զ��л�
        I2SDRL = WavePreSamp;           //ע�⣺I2S���ݼĴ���������д���ֽڣ���д���ֽ�

        if (WaveMute)
        {
            WavePreSamp = 0;            //����ʱ,׼����������0
        }
        else
        {
            if (WavePlayEn)
            {
                if (WaveDumpSize)       //�ж��Ƿ����㹻������
            	{
                    WavePreSamp = WaveBuffer[WaveReadPtr++];
                    WaveReadPtr &= BUFFER_MASK;
                    WaveDumpSize--;     //��ǰ�ж�Ϊ������ȼ�,����Ҫ���ٽ����WaveDumpSize���б���
            	}
                else
                {
                    WaveUnderrun = 1;   //��������
                    WavePlayEn = 0;
                }
            }
        }
    }
    if(++cnt_2ms == (96*2))
    {
		cnt_2ms = 0;
		B_2ms = 1;
	}
}

