/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************	����˵��	**************

AI8051U-USB¼��������:
����(����)������I2S�ж���������ݣ���Ƶ��Ƶ��MCLK��������48KHz��USB����192�ֽ�һ֡��1msһ֡��
����(¼��)����������������32KHz��USB����128�ֽ�һ֡��1msһ֡��

�û����ȱ��޸ĳ���, ֱ������"Obj"���"usb_audio_i2s_48k_16b_2ch.hex"����. ����ʱѡ����Ƶ36.864MHZ��
������ʹ��AI8051U��ѧ�ƻ�ʵ����V1.2-2024-8-20��֤��MCU�ڲ�ROSC������36.864MHz��
AI8051U��I2S�ӿ�����TLV320AIC23B��������.

ֻ֧��16λPCM��ʽ.

=========================== PCM˫�������ݸ�ʽ ====================================
16λ˫����PCM��С��ģʽ�����ֽ���ǰ����
        ˫��0(4�ֽ�)            |         ˫��1(4�ֽ�)            | .......
             ����1              |              ����2              | .......
    ������     |     ������     |     ������     |     ������     | .......
�ֽ�0   �ֽ�1  | �ֽ�2   �ֽ�3  | �ֽ�4   �ֽ�5  | �ֽ�6   �ֽ�7  | .......
���ֽ�  ���ֽ� | ���ֽ�  ���ֽ� | ���ֽ�  ���ֽ� | ���ֽ�  ���ֽ� | .......
==================================================================================

******************************************/

#include "stc.h"
#include "i2s.h"
#include "usb.h"
#include "clock.h"
#include "TLV320AIC23.h"

#define  CHANNEL  2		//����ADCͨ��(��ͷͨ��),   ȡֵΪ0~7, ��ӦP1.0~P1.7, ʹ�ñ��ADC�������Ҫ�޸�ADC��ʼ������.

/*************	IO�ڶ���	**************/
void 	Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

//=============== ��Ͳ��ر��� =====================================
#define	VOICE_BUFF_LENGTH	8192	//һ����2^n
#define	VOICE_BUFF_MASK		(VOICE_BUFF_LENGTH-1)
#define S_BUFFER_SIZE		(VOICE_BUFF_LENGTH / 4)
#define S_BUFFER_MASK		(BUFFER_SIZE - 1)
#define S_OVERRUN_POINT		(BUFFER_SIZE / 3 * 2)
#define S_UNDERRUN_POINT	(BUFFER_SIZE / 3)

u16	wr_index, rd_index;
u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u16 S_WaveDumpSize;	//�ѻ������������, ����һ��ֵ�岹һ��ֵ, ����һ��ֵ��ȥһ��ֵ
u16	adc;

bit	B_MicEn;
u8	Trim_UpDown;		//0:�޲���, 1:��������һ������,  2:�������һ������
//==================================================================


void sys_init();
void clk_trim();
void  delay_ms(u16 ms);


#ifdef DEBUG
    void show8(BYTE dat);
    void show16(WORD dat);
#endif

u8 	LED8[8];		//��ʾ����
u8	display_index;	//��ʾλ����
bit	B_1ms;			//1ms��־
u8	KeyCode;		//���û�ʹ�õļ���
u8 IO_KeyState;		//���м��̱���
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


const u8  t_display[]={						//��׼�ֿ�
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

const u8  T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//λ��

u8		KeyState, KeyCode;	//����

u16		HeadPhoneVol;		//��������, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.
void	SetHeadPhoneVolume(u16 vol);
void	IO_KeyScan(void);	//50ms call
void	DisplayScan(void);
void	ADC_config(void);
void 	PWMA_config(void);


u8 xdata buf[128];


void main()
{
	u16	j;

	P0M0 = 0;	//������������ź�
	P0M1 = 0;

    sys_init();
    clock_init();
    i2s_init();
    usb_init();
    EA = 1;

	for(j=0; j<8; j++)	LED8[j] = DIS_BLACK;	//�ϵ�����

	HeadPhoneVol = 80 - 20;	//Ĭ������60, �������80
	AIC23_Init();
	delay_ms(50);
	AIC32_InitSet();
	LED8[6] = HeadPhoneVol / 10;	//��ʾ����
	LED8[7] = HeadPhoneVol % 10;
	SetHeadPhoneVolume(HeadPhoneVol);	//��������

    for (j = 0; j < 128; j++)	buf[j] = 0;	//������
	wr_index = 0;
	rd_index = 0;
	S_WaveDumpSize = 0;		//�ѻ������������, ����һ��ֵ�岹һ��ֵ, ����һ��ֵ��ȥһ��ֵ
	B_MicEn = 0;
	Trim_UpDown = 0;		//0:�޲���, 1:��������һ������,  2:�������һ������
	ADC_config();
	PWMA_config();


    while (1)
    {
        if (DeviceState != DEVSTATE_CONFIGURED)
            continue;

        clock_trim();	//ʵʱ��̬΢��IRCƵ��

        if (UsbReq)			//�����ϴ���һ��¼������
        {
            UsbReq = 0;

			if (B_MicEn)
			{
				j = (wr_index - rd_index) & VOICE_BUFF_MASK;	//���л��峤��
				if(j > 256)		//���㹻������ݲ��ϴ�
				{
					USB_SendData(voice_buff+rd_index);	//�ϴ�¼������
					rd_index = (rd_index + 128) & VOICE_BUFF_MASK;
					S_WaveDumpSize -= 32;	// 32�������� * 2�ֽڲ��� *2ͨ�� = 128�ֽ�
				}
				else	USB_SendData(buf);	//�ϴ�¼������(�հ�)

				if (S_WaveDumpSize < S_UNDERRUN_POINT)      //��������ƫ/��
				{
					Trim_UpDown = 1;		//0:�޲���, 1:��������һ������,  2:�������һ������
				}
				else if (S_WaveDumpSize > S_OVERRUN_POINT)  //��������ƫ��
				{
					Trim_UpDown = 2;		//0:�޲���, 1:��������һ������,  2:�������һ������
				}
			}
			else	USB_SendData(buf);	//�ϴ�¼������(�հ�)
        }

        if(B_2ms)	//2ms��ʱʱ϶
        {
			B_2ms = 0;
			DisplayScan();	//1msɨ����ʾһλ

			if(++cnt20ms == 20)	//20msʱ϶
			{
				cnt20ms = 0;
				IO_KeyScan();		//ɨ�谴��
				if(KeyCode != 0)	//�м�����
				{
					if(KeyCode == K6)	//����+
					{
						if(++HeadPhoneVol > 80)	HeadPhoneVol = 80;	//�������
						SetHeadPhoneVolume(HeadPhoneVol);	//��������
						LED8[6] = HeadPhoneVol / 10;	//��ʾ����
						LED8[7] = HeadPhoneVol % 10;
					}
					else if(KeyCode == K7)	//����-
					{
						if(HeadPhoneVol != 0)	HeadPhoneVol--;	//��С����
						SetHeadPhoneVolume(HeadPhoneVol);	//��������
						LED8[6] = HeadPhoneVol / 10;	//��ʾ����
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
	���м�ɨ�����
	ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�

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

	j = IO_KeyState;	//������һ��״̬

	P06 = 0;
	IO_KeyDelay();
	IO_KeyState = P0 & 0x0f;
	P06 = 1;

	P07 = 0;
	IO_KeyDelay();
	IO_KeyState |= (P0 << 4) & 0xf0;
	IO_KeyState ^= 0xff;	//ȡ��
	P07 = 1;

	KeyCode |= (j ^ IO_KeyState) & IO_KeyState;
}
/**************** ��HC595����һ���ֽں��� ******************/
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

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{
	Send_595(t_display[LED8[display_index]]);	//�������
	Send_595(~T_COM[display_index]);			//���λ��

	P_HC595_RCLK = 1;
	NOP(2);
	P_HC595_RCLK = 0;							//�����������
	if(++display_index >= 8)	display_index = 0;	//8λ������0
}



//=================== ���ö������� ==========================================
void	SetHeadPhoneVolume(u16 vol)
{
	AIC23_WriteCmd(R_HeadphoneVolume_L, (LinVol_LRS | LinVol_Mute | (vol+47)));	//�����������
	AIC23_WriteCmd(R_HeadphoneVolume_R, (RinVol_RLS | RinVol_Mute | (vol+47)));

//	WriteASCII(66,5,Vol/10,0);
//	WriteASCII(72,5,Vol%10,0);
}



//========================================================================
// ����: void  delay_ms(u16 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, 1~65535ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע:
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
// ����: void PWMA_config(void)
// ����: PWM���ú�����
// ����: noe.
// ����: none.
// �汾: V1.0, 2022-3-15
// ��ע:
//========================================================================
void PWMA_config(void)
{
	PWMA_ENO    = 0;	// IO�����ֹ
	PWMA_IER    = 0;	// ��ֹ�ж�
	PWMA_SR1    = 0;	// ���״̬
	PWMA_SR2    = 0;	// ���״̬

	PWMA_PSCRH = 0x00;		// Ԥ��Ƶ�Ĵ���, ��Ƶ Fck_cnt = Fck_psc/(PSCR[15:0}+1), ���ض���PWMƵ�� = SYSclk/((PSCR+1)*(AAR+1)), �������PWMƵ�� = SYSclk/((PSCR+1)*(AAR+1)*2).
	PWMA_PSCRL = 0x00;
	PWMA_DTR   = 0;			// ����ʱ������, n=0~127: DTR= n T,   0x80 ~(0x80+n), n=0~63: DTR=(64+n)*2T,
							//				0xc0 ~(0xc0+n), n=0~31: DTR=(32+n)*8T,   0xE0 ~(0xE0+n), n=0~31: DTR=(32+n)*16T,
	PWMA_ARRH   = (u8)((FOSC/32000-1)/256);	// �Զ���װ�ؼĴ���,  ����PWM����
	PWMA_ARRL   = (u8)((FOSC/32000-1)%256);
	PWMA_CR2    = 0x20;		// �����¼�ΪTRG0, ���ڴ���ADC

	PWMA_CCER1  = 0;	// ����/�Ƚ�ʹ�ܼĴ���1
	PWMA_CCER2  = 0;	// ����/�Ƚ�ʹ�ܼĴ���2

	PWMA_BKR    = 0x80;		// �����ʹ�� �൱���ܿ���
	PWMA_CR1    = 0x81;		// ʹ�ܼ�����, �����Զ���װ�ؼĴ�������, ���ض���ģʽ, ���ϼ���,  bit7=1:д�Զ���װ�ؼĴ�������(�����ڲ��ᱻ����), =0:ֱ��д�Զ���װ�ؼĴ�����(���ڿ��ܻ��ҵ�)
	PWMA_EGR    = 0x01;		// ����һ�θ����¼�, �����������Ԥ��Ƶ������, װ��Ԥ��Ƶ�Ĵ�����ֵ
}

//	PWMA_PS   = (0<<6)+(0<<4)+(0<<2)+0;	//ѡ��IO, 4��Ӹߵ���(������)��ӦPWM1 PWM2 PWM3 PWM4, 0:ѡ��P1.x, 1:ѡ��P2.x, 2:ѡ��P6.x,
//  PWMA_PS    PWM4N PWM4P    PWM3N PWM3P    PWM2N PWM2P    PWM1N PWM1P
//    00       P1.7  P1.6     P1.5  P1.4     P1.3  P1.2     P1.1  P1.0
//    01       P0.7  P0.6     P0.5  P0.4     P0.3  P0.2     P0.1  P0.0
//    02       P2.7  P2.6     P2.5  P2.4     P2.3  P2.2     P2.1  P2.0
//    03        --    --       --    --       --    --       --    --

//========================================================================
// ����: void	ADC_config(void)
// ����: ADC��ʼ������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-8-1
// ��ע:
//========================================================================
#define D_ADC_POWER	(1<<7)	/* ADC��Դ��1������0�ر� */
#define D_ADC_START	(1<<6)	/* ����ת�����Զ���0 */
#define D_ADC_FLAG	(1<<5)	/* ��ɱ�־�������0 */
#define D_ADC_EPWMT	(1<<4)	/* ����PWMA����ADC */

#define	D_ADC_SPEED	5		/* 0~15, ADCʱ�� = SYSclk/2/(n+1) */
#define	D_RES_FMT	(1<<5)	/* ADC�����ʽ 0: �����, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: �Ҷ���, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */
#define CSSETUP		(0<<7)	/* 0~1,  ADCͨ��ѡ��ʱ��      0: 1��ADCʱ��, 1: 2��ADCʱ��,  Ĭ��0(Ĭ��1��ADCʱ��)	*/
#define CSHOLD		(0<<5)	/* 0~3,  ADCͨ��ѡ�񱣳�ʱ��  (n+1)��ADCʱ��, Ĭ��1(Ĭ��2��ADCʱ��)					*/
#define SMPDUTY		10		/* 10~31, ADCģ���źŲ���ʱ��  (n+1)��ADCʱ��, Ĭ��10(Ĭ��11��ADCʱ��)				*/
							/* ADCת��ʱ��: 10λADC�̶�Ϊ10��ADCʱ��, 12λADC�̶�Ϊ12��ADCʱ��. 				*/
void	ADC_config(void)
{
//	EAXFR = 1;	//SFR enable
	P1n_pure_input(1<<CHANNEL);	//����Ҫ��ADC��IO����������
	ADC_CONTR = D_ADC_POWER + CHANNEL;	//ADC on + channel
	ADCCFG = D_RES_FMT + D_ADC_SPEED;
	ADCTIM = CSSETUP + CSHOLD + SMPDUTY;
//	ADC_START   =  1;	//����ADCת��, ��ɺ��Զ�����
//	ADC_FLAG    =  0;	//���ADC���(�ж�)��־
	ADC_EPWMT   =  1;	//����PWM����ADC
	EADC  = 1;	//����ADC�ж�
	PADCH = 1;	//ADC �ж����ȼ���λ
	PADC  = 1;	//ADC �ж����ȼ�
}


#define	HighPassEnable	1		// 1: �����ͨ�˲�������ֱ��ƫ��, 0: ��ֹ��ͨ�˲�

		//	32741, -32741, 0, -32715, 0			//1�װ�����˹��ͨ  8Hz/32000Hz
#define	COEF_A0		 32741
#define	COEF_B0		-32715
#if (HighPassEnable ==1)
long	Xin1, Xin0, Yout0, out;
#endif

void ADC_ISR (void) interrupt ADC_VECTOR	//@36.864MHz  �޸�ͨ�˲� 2us����һ�׸�ͨ�˲�4.2us
{
	ADC_FLAG    =  0;	//���ADC���(�ж�)��־

	#if (HighPassEnable ==1)	//�����ͨ�˲�������ֱ��ƫ��
		adc = (u16)ADC_RES * 256 + (u16)ADC_RESL;	//��ȡADCֵ��ת��˫����
		Xin1 = Xin0;	Xin0 = (long)adc - 2048;	//����ADCֵΪ12λ, תΪ˫����
		out = (COEF_A0 * (Xin0 - Xin1) - COEF_B0 * Yout0)/32768;	//һ�׸�ͨ
		Yout0 = out;
		if(out >=  2048)	out =  2047;	//����޷�
		if(out <= -2048)	out = -2047;	//����޷�
		adc = (u16)out;
	#else
		adc = ((u16)ADC_RES * 256 + (u16)ADC_RESL)-2048;	//��ȡADCֵ��ת��˫����
	#endif

	if(S_WaveDumpSize < (S_BUFFER_SIZE-4))	//�������
	{
		voice_buff[wr_index++] = (u8)(adc % 256);	//С��ģʽ
		voice_buff[wr_index++] = (u8)(adc / 256);
		voice_buff[wr_index++] = (u8)(adc % 256);	//С��ģʽ
		voice_buff[wr_index++] = (u8)(adc / 256);
		wr_index &= VOICE_BUFF_MASK;	//��ֹ���
		S_WaveDumpSize++;	//�ѻ������������, ����һ��ֵ�岹һ��ֵ, ����һ��ֵ��ȥһ��ֵ

		if(S_WaveDumpSize >= ( S_BUFFER_SIZE/2))	B_MicEn = 1;	//����������������
		if(S_WaveDumpSize < 256)					B_MicEn = 0;	//������ٹرմ���

		if(Trim_UpDown == 1)		//0:�޲���, 1:��������һ������,  2:�������һ������
		{
			Trim_UpDown = 0;
			voice_buff[wr_index++] = (u8)(adc % 256);	//С��ģʽ
			voice_buff[wr_index++] = (u8)(adc / 256);
			voice_buff[wr_index++] = (u8)(adc % 256);	//С��ģʽ
			voice_buff[wr_index++] = (u8)(adc / 256);
			wr_index &= VOICE_BUFF_MASK;	//��ֹ���
			S_WaveDumpSize++;	//�ѻ������������, ����һ��ֵ�岹һ��ֵ, ����һ��ֵ��ȥһ��ֵ
		}
		if(Trim_UpDown == 2)		//0:�޲���, 1:��������һ������,  2:�������һ������
		{
			Trim_UpDown = 0;
			wr_index -= 4;
			wr_index &= VOICE_BUFF_MASK;	//��ֹ���
			if(S_WaveDumpSize != 0)		S_WaveDumpSize--;		//�ѻ������������, ����һ��ֵ�岹һ��ֵ, ����һ��ֵ��ȥһ��ֵ
		}
	}
}

