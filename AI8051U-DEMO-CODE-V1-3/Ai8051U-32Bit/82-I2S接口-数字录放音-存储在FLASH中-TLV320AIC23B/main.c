
/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/


#include	"AI8051U.h"
#include	"intrins.h"
#include	"TLV320AIC23.h"
#include	"W25Q128FV.h"

/*************	����˵��	**************
������ʹ��AI8051Uʵ����V1.2��֤���û��ȱ��޸ĳ���ֱ������HEX�ļ���AI8051Uʵ����V1.2����֤������ʱѡ����Ƶ36.864MHz��

ʹ��AI8051Uϵ��MCU������¼�����ԣ������洢��FLASH��. ��Ͳ�Ŵ��·��ͨ�˲�ת��Ƶ��Ϊ3400Hz���ң�����8~16K������
Ϊ�������õ�ռ䣬����ʹ��A��ѹ��/��ѹ����ÿ���ֽ������ڲ����ʡ�
 8KHz����������Ϊ 8KB/S��16MB FLASH����¼��34���ӡ�
16KHz����������Ϊ16KB/S��16MB FLASH����¼��17���ӡ�

******************************************/



//����I2S��ʱ�ӱ����� 256*SampleRate��������������������16λ����������ʱ��WS=SampleRate������ʱ��BCLK=32*SampleRate����ʱ��MCLK=8*BCLK=256*SampleRate��

	#define FOSC			36864000UL		//������ʱ��
//	#define SampleRate		48000			//���������
//	#define SampleRate		36000			//���������
//	#define SampleRate		24000			//���������
//	#define SampleRate		16000			//���������
//	#define SampleRate		12000			//���������
	#define SampleRate		8000			//���������
//	#define SampleRate		6000			//���������

//	#define FOSC			40960000UL		//������ʱ��
//	#define FOSC			32768000UL		//������ʱ��
//	#define SampleRate		32000			//���������
//	#define SampleRate		16000			//���������
//	#define SampleRate		 8000			//���������

//	#define FOSC			33868800UL		//������ʱ��
//	#define SampleRate		44100			//���������
//	#define SampleRate		22050			//���������
//	#define SampleRate		11025			//���������

#define  CHANNEL  2			  //����ADCͨ��(��ͷͨ��),   ȡֵΪ0~7, ��ӦP1.0~P1.7, ʹ�ñ��ADC�������Ҫ�޸�ADC��ʼ������.

#define	VOICE_BUFF_LENGTH	16384		//������4096��8192��16384��3����֮һ
#define	VOICE_BUFF_MASK		(VOICE_BUFF_LENGTH-1)
#define	FLASH_CAP			(16380*1024)	// FLASH����


/*************	IO�ڶ���	**************/
void 	Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

/*************	��������	**************/
u32	FileLength;		//�ļ�����(�ֽ�)
u32	PlayByteCnt;	//�����ֽڼ���
u16	dac;			//�����������������DACֵ
bit	B_PlayEn;		//������
bit	B_record;		//����¼��
bit B_stop;			//1: ֹͣ¼�������

u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u16	wr_index;		//д��������
u16	rd_index;		//����������
u32	FlashAddr;		//��FLASH��ַ
u8	OP_index;		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH

u16	second;
u16	cnt_1s;

u8	cnt_1ms;		//1ms�������û��㲻�ɼ�
u8	cnt_20ms;		//20ms�������û��㲻�ɼ�
bit	B_20ms;			//20ms��־���û���ʹ�ò����
u16	HeadPhoneVol;	//��������, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.

u8 	LED8[8];		//��ʾ����
u8	display_index;	//��ʾλ����
u8	KeyCode;		//���û�ʹ�õļ���
u8 IO_KeyState;		//���м��̱���


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


u8 code t_display[]={						//��׼�ֿ�
//	 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
	0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black	 -     H    J	 K	  L	   N	o   P	 U     t    G    Q    r   M    y
	0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
	0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};	//0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};		//λ��


/*************	��������	**************/

void	I2S_config(void);
void	SetSampleRate(void);
void  	delay_ms(u16 ms);
void	SetHeadPhoneVolume(u16 vol);
void	IO_KeyScan(void);	//50ms call
u8		CheckString(u8 *px, u8 const *pc, u8 num);
void 	CDC_StringPrint(u8 *puts);
void	PlayProcess(void);
void	ADC_config(void);


u8 const F_ALAW[]="ALAW";			//�ļ�ͷ 00H~03H


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

	SPI_Config(2, 0);	//(SPI_io, SPI_speed), ����: 	SPI_io: �л�IO(SS MOSI MISO SCLK), 0: �л���P1.4 P1.5 P1.6 P1.7,  1: �л���P2.4 P2.5 P2.6 P2.7, 2: �л���P4.0 P4.1 P4.2 P4.3,  3: �л���P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
	ADC_config();

	cnt_1ms = 0;	// 1ms�������û��㲻�ɼ�
	cnt_20ms = 0;	//20ms�������û��㲻�ɼ�
	B_20ms    = 0;	//20ms��־���û���ʹ�ò����
	for(i=0; i<8; i++)	LED8[i] = DIS_BLACK;	//�ϵ�����
	LED8[0] = DIS_S;	//��ʾstop
	LED8[1] = DIS_T;
	LED8[2] = DIS_O;
	LED8[3] = DIS_P;
	cnt_1s = 0;

	delay_ms(10);
	AIC23_Init();
	delay_ms(50);
	AIC32_InitSet();
	delay_ms(50);
	HeadPhoneVol = 80 - 30;//Ĭ������
	SetHeadPhoneVolume(HeadPhoneVol);	//��������
	LED8[6] = HeadPhoneVol / 10;		//��ʾ����
	LED8[7] = HeadPhoneVol % 10;

	I2S_config();
	EA = 1;

	OP_index = 0;

	B_FlashOK = 0;
	FlashCheckID();
	FlashCheckID();
	B_FlashOK = 0;
	if((FLASH_ID >= 0x12) && (FLASH_ID <= 0x19))	B_FlashOK = 1;	//��⵽FLASH

	while(1)
	{
		if(OP_index != 0)	PlayProcess();

		if(B_20ms)	//20msʱ϶
		{
			B_20ms = 0;

			if(B_PlayEn || B_record)
			{
				if(++cnt_1s >= 50)	//���ʱ
				{
					cnt_1s = 0;
					second++;
					i = second / 60;
					LED8[1] = (u8)(i/10);
					LED8[2] = (u8)(i%10 +DIS_DOT);
					i = second % 60;
					LED8[3] = i/10;
					LED8[4] = i%10;		//����ʱ��
				}
			}

			if(B_stop && !B_SPI_DMA_busy)	//ֹͣ¼�������
			{
				B_stop = 0;
				OP_index = 0;
				LED8[0] = DIS_S;	//��ʾstop
				LED8[1] = DIS_T;
				LED8[2] = DIS_O;
				LED8[3] = DIS_P;
				LED8[4] = DIS_BLACK;

				if(B_record)	//¼������, ����ALAW��ʶ�����ݳ���
				{
					B_record = 0;
					for(i=0; i<4; i++)	voice_buff[i] = F_ALAW[i];	//A LAW��ʶ
					voice_buff[4] = (u8)(FlashAddr >> 24);	//¼��FLASH���ȣ����ģʽ
					voice_buff[5] = (u8)(FlashAddr >> 16);
					voice_buff[6] = (u8)(FlashAddr >> 8);
					voice_buff[7] = (u8)FlashAddr;
					FlashWrite_Nbytes(0, voice_buff, 8);	//(u32 addr, u8 *buffer, u16 size)	д������
				}
			}

			IO_KeyScan();


			if(KeyCode != 0)	//�м�����
			{
				if(KeyCode == K1)	//ֹͣ���Ż�¼��
				{
					OP_index = 0;	//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
					B_PlayEn = 0;	//ֹͣ����
					B_stop   = 1;
				}

				else if(KeyCode == K2)	//¼��
				{
					if(B_record)	OP_index = 0, B_stop = 1;	//����¼����ֹͣ¼��
					else if(B_FlashOK)	//FLASH����, ������¼��
					{
						if(!B_PlayEn)	//δ�����ſ���¼��
						{
							OP_index = 3;	//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
							cnt_1s = 0;
							second = 0;
							LED8[0] = DIS_R;	//��ʾR
							LED8[1] = 0;
							LED8[2] = 0+DIS_DOT;
							LED8[3] = 0;
							LED8[4] = 0;		//¼��ʱ��
						}
					}
				}

				else if(KeyCode == K3)	//����
				{
					if(B_PlayEn)	B_PlayEn = 0, B_stop = 1;	//���ڲ���ֹͣ����
					else if(B_FlashOK)	//FLASH����, ����������
					{
						if(!B_record)	//δ¼������Բ���
						{
							OP_index = 1;	//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
							cnt_1s = 0;
							second = 0;
							LED8[0] = DIS_P;	//��ʾP
							LED8[1] = 0;
							LED8[2] = 0+DIS_DOT;
							LED8[3] = 0;
							LED8[4] = 0;		//����ʱ��
						}
					}
				}

				else if(KeyCode == K6)	//����+
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
}


// У��һ���ַ�������ȷ����0�����󷵻ط�0
u8	CheckString(u8 *px, u8 const *pc, u8 num)
{
	u8	i;
	for(i=0; i<num; i++)
	{
		if(px[i] != pc[i])	return 1;	//�ַ�����ȴ���
	}
	return 0;	//�ַ�����ȷ
}

//���� ¼������
void	PlayProcess(void)
{
	u16	j;
	if(OP_index == 1)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
	{
		FlashRead_Nbytes(0, voice_buff, 1024);//(u32 addr, u8 *buffer, u16 size)	��ȡ�ļ�
		if(CheckString(voice_buff, F_ALAW, 4) == 0)			//���ALAW		�ļ�ͷ 00H~03H
		{
			B_PlayEn = 0;	//ֹͣ����
			B_record = 0;	//ֹͣ¼��
			FileLength = ((u32)voice_buff[4] << 24) + ((u32)voice_buff[5] << 16) + (u32)voice_buff[6]*256 + voice_buff[7];	//�����ֽڳ���, ���ģʽ, [4] [5] [6] [7]
			if(FileLength < FLASH_CAP)	//С��falsh����
			{
				FlashAddr = 1024;
				wr_index  = 1024;	//д��������
				rd_index  = 16;		//����������, ��16��ʼ�����������
				PlayByteCnt = 16;	//�����ֽڼ���
				dac = 0;
				B_PlayEn = 1;	//��������
				OP_index = 2;
			}
		}
		else OP_index = 0, B_stop = 1;	//����������
	}

	else if(OP_index == 2)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
	{
		if(!B_SPI_DMA_busy)
		{
			j = (rd_index - wr_index) & VOICE_BUFF_MASK;	//������л����ֽ���
			if((j > 1024) || (j == 0))	//�ճ��˳���1024�ֽ�
			{
				if(FlashAddr < FileLength)	//δ���ļ�����, ������FLASH
				{
				//	FlashRead_Nbytes(FlashAddr, voice_buff+wr_index, 1024);	//(u32 addr, u8 *buffer, u16 size)	��ȡ����
					SPI_DMA_RxTRIG(FlashAddr, voice_buff+wr_index, 1024);//(u32 addr, u8 *buffer, u16 size);	SPI DMA��ȡ����
					FlashAddr += 1024;	//��FLASH�ĵ�ַ+1024
					wr_index  += 1024;	//д��������+1024
					wr_index  &= VOICE_BUFF_MASK;	//�������
				}
			}
		}
	}

	else if(OP_index == 3)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
	{
		B_PlayEn = 0;	//ֹͣ����
		B_record = 0;	//ֹͣ¼��
		FlashAddr= 0;
		wr_index = 16;	//д��������, �������ݴ�16��ʼ
		rd_index = 0;		//����������, ��16��ʼ�����������
		for(j=0; j<16; j++)	voice_buff[j] = 0xff;	//Ԥ��16�ֽ�
		B_record = 1;	//��ʼ¼��
		OP_index = 4;
	}
	else if(OP_index == 4)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����¼����4:����¼������
	{
		if(!B_SPI_DMA_busy)	//SPI DMA����
		{
			j = (wr_index - rd_index) & VOICE_BUFF_MASK;	//���������ֽ���
			if(j > 256)	//�ճ��˳���256�ֽ�
			{
				if((FlashAddr & 0x00ffff) == 0)
				{
					FlashSectorErase(FlashAddr, 64);	//(u32 addr, u8 sec)	����һ������64K
					while(FlashCheckBusy() != 0);		//Flashæ���
				}
			//	FlashWrite_Nbytes(FlashAddr, voice_buff+rd_index, 256);	//(u32 addr, u8 *buffer, u16 size)	д������
				SPI_DMA_TxTRIG(FlashAddr, voice_buff+rd_index, 256);//(u32 addr, u8 *buffer, u16 size);	SPI DMAд������
				rd_index  += 256;	//����������+256
				rd_index  &= VOICE_BUFF_MASK;	//�������
				FlashAddr += 256;	//дFLASH�ĵ�ַ+256
				if(FlashAddr >= FLASH_CAP)	OP_index = 0, B_record = 1, B_stop = 1;	//���������
			}
		}
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


//=================== ���ö������� ==========================================
void	SetHeadPhoneVolume(u16 vol)
{
	AIC23_WriteCmd(R_HeadphoneVolume_L, (LinVol_LRS | LinVol_Mute | (vol+47)));	//�����������
	AIC23_WriteCmd(R_HeadphoneVolume_R, (RinVol_RLS | RinVol_Mute | (vol+47)));
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

/********************** ADC��ʼ������ ************************/
#define D_ADC_POWER	(1<<7)	/* ADC��Դ��1������0�ر� */
#define D_ADC_START	(1<<6)	/* ����ת�����Զ���0 */
#define D_ADC_FLAG	(1<<5)	/* ��ɱ�־�������0 */
#define D_ADC_EPWMT	(1<<4)	/* ����PWMA����ADC */

#define	D_ADC_SPEED	5		/* 0~15, ADCʱ�� = SYSclk/2/(n+1) */
#define	D_RES_FMT	(1<<5)	/* ADC�����ʽ 0: �����, ADC_RES: D9 D8 D7 D6 D5 D4 D3 D2, ADC_RESL: D1 D0 0  0  0  0  0  0 */
							/*             1: �Ҷ���, ADC_RES: 0  0  0  0  0  0  D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */
#define CSSETUP		(1<<7)	/* 0~1,  ADCͨ��ѡ��ʱ��      0: 1��ADCʱ��, 1: 2��ADCʱ��,  Ĭ��0(Ĭ��1��ADCʱ��)	*/
#define CSHOLD		(1<<5)	/* 0~3,  ADCͨ��ѡ�񱣳�ʱ��  (n+1)��ADCʱ��, Ĭ��1(Ĭ��2��ADCʱ��)					*/
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
//	ADC_EPWMT   =  1;	//����PWM����ADC
//	EADC  = 1;	//����ADC�ж�
//	PADCH = 1;	//ADC �ж����ȼ���λ
//	PADC  = 1;	//ADC �ж����ȼ�
}


/*********** A��ѹ��/��ѹ���㷨 ******************************

  ѹ��: ���ݹ��ʱ�׼��A������12λADC��A=87.6Ϊ��׼�ģ�13���߷��ƽ���
��ѹ��: ��������I2S�����������16λ��DAC�����Լ���ʱ���������ݶ�*16��

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

const	u8	T_Alaw_encodeH[]={		/* 113������, ������� */
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

/**************** A��ѹ�� *******************
  ��12bit ADC���Զ�����ֵ����ѹ����8bit A-law���ݲ����뻺��
	Convert 12 Bit Liner Data to 8bit ALAW data
**************************************************/
void Alaw_encode(u16 adc)	//����12bit�޷��ŵ�ADCֵ
{
	u8	law_data;
	u8	minus;

	if(adc & 0x0800)	minus = 0x00,	adc = adc - 2048;	// >= 2048Ϊ��, 0~2047, ���м�ֵ2048Ϊ0��.
	else				minus = 0x80,	adc = 2048 - adc;	//  < 2048Ϊ��. 1~2048

	if((adc & 0xff00) == 0)	law_data = T_Alaw_encode[(u8)adc];	//0~255 --> 0~79
	else					law_data = T_Alaw_encodeH[(u8)((adc - 256)/16)];	//256~2047 --> 80~127

	voice_buff[wr_index] = (law_data + minus) ^ 0x55;	//����д�뻺�壬ż��λȡ��	ѭ�����У�ָ����һ������
	wr_index++;
	wr_index &= VOICE_BUFF_MASK;	//�������
}

//**************** A�ʽ�ѹ�� *******************
//  ��8bit A-law���ݽ�ѹ����16bit���ݲ������DAC
//	Convert 8bit Alaw data to 16 Bit Liner Data
//**************************************************
void Alaw_decode(void)	//��ѹ����õ�16λ�з���DAC
{
	u8	law_data;
	law_data = voice_buff[rd_index] ^ 0x55;		//Get Complement ż��λȡ��, �ӻ���������ֽڣ�ż��λȡ����ѭ�����У�ָ����һ������
	dac = T_Alaw_decode[law_data & 0x7f];	// ��ѹ����
	if(law_data & 0x80)	dac = 0-dac;		//����Ǹ���, ת���з���
	rd_index++;
	rd_index &= VOICE_BUFF_MASK;	//�������
}


//====================== I2S��ʼ������ ==================================================
#define	MCKOE		1		//I2S��ʱ���������, 0:��ֹI2S��ʱ�����, 1:����I2S��ʱ�����

#define	I2SEN		0x04	//I2Sģ��ʹ��, 0x00:��ֹ, 0x04:����
#define	I2S_MODE	2		//I2Sģʽ, 0:�ӻ�����ģʽ, 1:�ӻ�����ģʽ, 2:��������ģʽ, 3:��������ģʽ,

#define	PCMSYNC		0		//PCM֡ͬ��, 0: ��֡ͬ��, 1: ��֡ͬ��
#define	STD_MODE	0		//I2S��׼ѡ��, 0: I2S�����ֱ�׼, 1: MSB������׼, 2:LSB�Ҷ����׼, 3:PCM��׼, CS4334��CS4344ʹ��0:I2S�����ֱ�׼��PT8211ʹ��1: MSB������׼��
#define	CKPOL		0		//I2S��̬ʱ�Ӽ���, 0:ʱ���ȶ�״̬Ϊ�͵�ƽ, 1:ʱ���ȶ�״̬Ϊ�ߵ�ƽ
#define	DATLEN		0		//���ݳ���, 0:16λ, 1:24λ, 2:32λ, 3:����
#define	CHLEN		0		//ͨ������(ÿ����Ƶͨ����λ��), 0:16λ, 1: 32λ

#define I2S_MCLKDIV		(FOSC/(8*16*2*SampleRate))	//MCLK��Ƶϵ��, ����˫����16bit.
#define I2S_BCLKDIV		(FOSC/(16*2*SampleRate))		//BCLK��Ƶϵ��, ����˫����16bit.

void	I2S_config(void)
{
	I2SMD = 0xff;					//�ڲ������ֽ�,������ΪFFH
	I2SSR = 0x00;					//״̬�Ĵ�����0
	I2SCR = 0x80+0x00;				//ʹ�ܷ��ͻ��������ж�(0x80), +0x00:Motorola��ʽ, +0x10:TI��ʽ
	HSCLKDIV    = 1;				//����ʱ�ӷ�Ƶ�� 1~255 (Ĭ��2)
	I2S_CLKDIV = 1;					//I2S��ʱ�ӷ�Ƶ
	I2SMCKDIV  = I2S_MCLKDIV;					//I2Sʱ�ӷ�Ƶ��I2SMCLK = ��Ƶ/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV,  ��I2SMCLK = PLLCLK/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV
	I2SPRH = (MCKOE << 1) + (I2S_BCLKDIV & 1);	//����I2S_BMCLK��Ƶϵ����bit0, ��������ֹ���MCLK��
	I2SPRL = I2S_BCLKDIV/2;						//����I2S_BMCLK��Ƶϵ����bit8~bit1
	I2SCFGH = I2S_MODE;				//����I2SģʽΪ��������ģʽ
	I2SCFGL = (PCMSYNC << 7) + (STD_MODE << 4) + (CKPOL << 3) + (DATLEN << 1) + CHLEN;
	P_SW3 = (P_SW3 & 0x3f) | (1<<6);	//I2S�˿��л�, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-7-21
										//             1: P1.7(BCLK) P1.6(MCLK) P1.5(SD) P1.4(WS),
										//             2: P2.3(BCLK) P2.2(MCLK) P2.1(SD) P2.0(WS),
										//             3: P4.3(BCLK) P1.6(MCLK) P4.1(SD) P4.0(WS),
	I2SCFGH |= I2SEN;                //ʹ��I2Sģ��
}


//====================== I2S�жϺ��� ==================================================
void I2S_ISR(void) interrupt I2S_VECTOR
{
	u16	j;
	if (I2SSR & 0x02)				//���ͻ�������
	{
		I2SDRH = (u8)(dac /256);	//������Ƶ����, ��������������
		I2SDRL = (u8)(dac %256);

		if((I2SSR & 0x04) == 0)		//������,  ����ADPCM������ADC
		{
			if(B_record)	//����¼��, ������ADC
			{
				ADC_RES = 0;	ADC_RESL = 0;
				ADC_START   =  1;	//����ADCת��, ��ɺ��Զ�����
			}
			if(++cnt_1ms >= (u8)(SampleRate/1000))
			{
				DisplayScan();	//1msɨ����ʾһλ
				cnt_1ms = 0;
				if(++cnt_20ms == 20)	cnt_20ms = 0, B_20ms = 1;		//20msʱ϶
			}
		}
		else	//������
		{
			if(B_PlayEn)	//���ڲ���
			{
				Alaw_decode();	//��ѹ����õ�16λ�з���DAC
				if(++PlayByteCnt >= FileLength)	B_PlayEn = 0, B_stop = 1;	//���������
			}
			else	dac = 0;
			if(B_record)	//����¼��, ��ȡADC��A-LAWѹ��
			{
				j = (u16)ADC_RES * 256 + (u16)ADC_RESL;
				ADC_FLAG    =  0;	//���ADC���(�ж�)��־
				Alaw_encode(j);	//����12bit�޷��ŵ�ADCֵ
			}
		}
	}
//	I2SSR &= ~0x5B;		//���Զ�����жϱ�־
}

