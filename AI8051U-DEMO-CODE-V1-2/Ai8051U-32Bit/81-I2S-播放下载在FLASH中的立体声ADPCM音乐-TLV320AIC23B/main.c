
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

/*************	����˵��	**************
������ʹ��AI8051Uʵ����V1.2��֤���û��ȱ��޸ĳ���ֱ������HEX�ļ���AI8051Uʵ����V1.2����֤������ʱѡ����Ƶ40.96MHz��

ʹ��AI8051Uϵ��MCU�������ش洢��FLASH�е�ADPCM���֣�֧���������������������ʹ̶�Ϊ32KHz���û����������޸Ĳ����ʡ�
�����ļ������ع������ء�
ʹ��ADPCM��Ϊ�˼���������16MB��FLASH���Բ���8��30������������֣�2�׸��������

******************************************/

/*************	IO�ڶ���	**************/
void Send_595(u8 dat);
sbit	P_HC595_SER   = P3^4;	//pin 14	SER		data input
sbit	P_HC595_RCLK  = P3^5;	//pin 12	RCLk	store (latch) clock
sbit	P_HC595_SRCLK = P3^2;	//pin 11	SRCLK	Shift data clock

/*************	��������	**************/
u8	xdata voice_buff[VOICE_BUFF_LENGTH];
u8	MusicType;			//WAV����, fmttag=0x01-->PCM, 0x02-->Windows ADPCM, 0x06-->A Law, 0x07-->Mu Law, 0x11-->IMA ADPCM
u8	MusicChannel;		//��������
u16	MusicSampleRate;	//������
u32	FileLength;			//�ļ�����(�ֽ�)
u32	PlayByteCnt;		//�����ֽڼ���
u16	dac_L, dac_R;		//�����������������DACֵ
bit	B_PlayEn;			//������

u16	wr_index;		//д��������
u16	rd_index;		//����������
u32	FlashAddr;		//��FLASH��ַ
u8	OP_index;		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
u8	RcvTimeOut;
bit	B_DownLoadStart;	//��ʼ����

//======= ADPCM����ר�ñ��� ==========
u8		edata ADPCM_Data_L[4];	//�������ֲ�����
u8		edata ADPCM_Data_R[4];	//�������ֲ�����
u8		decode;					//����ֵ
long	delta;				//ƫ��
long	cur_sample_L;		//����������ֵ
long	cur_sample_R;		//����������ֵ
char	index_L;			//��������������
char	index_R;			//��������������
u8		DecodeCnt;			//�������
u16		MusicBlock;			//BLOCK��С
bit		B_high_nibble;		//�߰��ֽ�ָʾ
//====================================

bit B_stop;		//1: ֹͣ¼�������
u16	second;
u8	cnt_1s;

u8	de_index;		//ADPCM����
u16	pre_sample;		//ǰһ������ֵ
bit	B_high_nibble;	//�߰��ֽڱ�־
u8	adpcm_data;		//ADPCM����

u8	cnt_1ms;		// 1ms�������û��㲻�ɼ�
u8	cnt_20ms;		//20ms�������û��㲻�ɼ�
bit	B_20ms;			//20msʱ϶���û���ʹ�ò����
u16	HeadPhoneVol;	//��������, 0~80, 0->mute, 1->-73db, 80->+6db, 74->0db, 1db/step.

u8 	LED8[8];		//��ʾ����
u8	display_index;	//��ʾλ����
u8	KeyCode;		//���û�ʹ�õļ���
u8	IO_KeyState;	//���м��̱���



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

	P3n_pure_input(0x03);	//P3.0(D-)��P3.1(D+)����Ϊ����
	IRC48MCR = 0x80;
	while (!(IRC48MCR & 0x01));

	uart_init();
	usb_init();
	EA = 1;
	delay_ms(1500);
//	while(DeviceState != DEVSTATE_CONFIGURED)	{	NOP(3);	}

	TFPU_CLKDIV = 1;		//TFPU��Ƶ

	SPI_Config(2, 0);	//(SPI_io, SPI_speed), ����: 	SPI_io: �л�IO(SS MOSI MISO SCLK), 0: �л���P1.4 P1.5 P1.6 P1.7,  1: �л���P2.4 P2.5 P2.6 P2.7, 2: �л���P4.0 P4.1 P4.2 P4.3,  3: �л���P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2

	cnt_1ms = 0;	// 1ms�������û��㲻�ɼ�
	cnt_20ms = 0;	//20ms�������û��㲻�ɼ�
	B_20ms    = 0;	//20msʱ϶���û���ʹ�ò����
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
	LED8[6] = HeadPhoneVol / 10;	//��ʾ����
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

		if(OP_index != 4)	//������ģʽ
		{
			if (RxFlag)		//��RxFlagΪ1ʱ, ��ʾ�ѽ��յ�CDC��������, ���յ����ݴ�С������RxCount����,ÿ����������64�ֽ�, ���ݱ�����RxBuffer������.
			{
				if(RxCount == 1)	KeyCode = RxBuffer[0];	//PC�´�����
				else if(RxCount == 8)
				{
					if(CheckFile(RxBuffer, C_DownLoad, 8) == 0)		//(u8 *px, u8 const *pc, u8 num) �ж��ǲ�����������
					{
						if(B_FlashOK)	//FLASH����
						{
							B_PlayEn = 0;	//ֹͣ����
							CDC_StringPrint("��\xfd�ڲ���\xfd FLASH�����Ժ�...\r\n");
							FlashChipErase();	//ִ��Ƭ��������
							OP_index = 3;	//�ȴ��������
							LED8[0] = DIS_;	//��ʾ-EA-
							LED8[1] = DIS_E;
							LED8[2] = DIS_A;
							LED8[3] = DIS_;
							LED8[4] = DIS_BLACK;	//¼��ʱ����������ʾ
						}
					}
				}
				uart_recv_done();	//�Խ��յ����ݴ�����ɺ�,һ��Ҫ����һ���������,�Ա�CDC������һ�ʴ�������

				if(KeyCode == 'r')	//����80�ֽ�ͷ�ļ�
				{
					FlashRead_Nbytes(0, TxBuffer, 80);	//(u32 addr, u8 *buffer, u16 size)	��ȡ����
					uart_send(80);	//��������. ��Ҫ���͵����ݱ�����TxBuffer��������, Ȼ�����uart_send(n)�����������ݷ���,����Ϊ���͵��ֽ���. һ�����ɷ���64K,�����ڲ����Զ�����USB�ְ�.
				}
			}
		}

		if(OP_index != 0)	PlayProcess();


		if(B_20ms)	//1msʱ϶
		{
			B_20ms = 0;
			if(RcvTimeOut != 0)	RcvTimeOut--;	//���ճ�ʱ����

			if(B_PlayEn)
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

			if(B_stop)	//ֹͣ¼�������
			{
				B_stop = 0;
				B_PlayEn = 0;	//ֹͣ����
				LED8[0] = DIS_S;	//��ʾstop
				LED8[1] = DIS_T;
				LED8[2] = DIS_O;
				LED8[3] = DIS_P;
				LED8[4] = DIS_BLACK;	//¼��ʱ����������ʾ
			}

			IO_KeyScan();
			if(KeyCode != 0)	//�м�����
			{
				if(KeyCode == K2)	//ֹͣ����
				{
					OP_index = 0;	//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
					B_PlayEn = 0;	//ֹͣ����
					B_stop   = 1;
				}
				else if(KeyCode == K3)	//����
				{
					if(B_FlashOK)	//FLASH����
					{
						OP_index = 1;	//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
						second = 0;
						LED8[0] = DIS_P;	//��ʾP
						LED8[1] = 0;
						LED8[2] = 0+DIS_DOT;
						LED8[3] = 0;
						LED8[4] = 0;		//����ʱ��
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



u8 const F_RIFF[]="RIFF";			//�ļ�ͷ 00H~03H
u8 const F_WAVEfmt[]="WAVEfmt ";	//�ļ�ͷ 08H~0FH
u8 const F_fact[]="fact";			//�ļ�ͷ 28H~2BH
u8 const F_data[]="data";			//�ļ�ͷ 34H~37H
u8	CheckFile(u8 *px, u8 const *pc, u8 num)
{
	u8	i;
	for(i=0; i<num; i++)
	{
		if(px[i] != pc[i])	return 1;	//�ַ�����ȴ���
	}
	return 0;	//�ַ�����ȷ
}

void	PlayProcess(void)
{
	u16	j;
	if(OP_index == 1)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
	{
		FlashRead_Nbytes(0, voice_buff, 100);//(u32 addr, u8 *buffer, u16 size)	��ȡͷ�ļ�
		j  = 0;
		j  = CheckFile(voice_buff, F_RIFF, 4);			//���RIFF		�ļ�ͷ 00H~03H
		j |= CheckFile(voice_buff+0x08, F_WAVEfmt, 8);	//���WAVEfmt	�ļ�ͷ 08H~0FH
		j |= CheckFile(voice_buff+0x28, F_fact, 4);		//���fact		�ļ�ͷ 28H~2BH
		j |= CheckFile(voice_buff+0x34, F_data, 4);		//���data		�ļ�ͷ 34H~37H
		if((j == 0) && (voice_buff[0x14]==0x11) && (voice_buff[0x15]==0x00))	//�ļ�ͷ 14H~15H ΪIMA-ADPCM�����ͱ�ʶ0x0011
		{
			B_PlayEn  = 0;	//ֹͣ����
			MusicChannel    = voice_buff[0x16];	//������, С��ģʽ, [0x16] [0x17]
			MusicSampleRate = (u16)voice_buff[0x19]*256 + voice_buff[0x18];	//������, С��ģʽ, [0x18] [0x19] [0x1a] [0x1b]
			FileLength      = ((u32)voice_buff[0x3b] << 24) + ((u32)voice_buff[0x3a] << 16) + (u32)voice_buff[0x39]*256 + voice_buff[0x38];	//�����ֽڳ���, С��ģʽ, [0x38] [0x39] [0x3a] [0x3b]
			MusicBlock      = (u16)voice_buff[0x21]*256+voice_buff[0x20];	//����BLOCK����(ADPCMʹ��), С��ģʽ, [0x20] [0x21]
			MusicBlock--;
			FlashAddr = 0x003c;	//��FLASH�ĵ�ַ
			FlashRead_Nbytes(FlashAddr, voice_buff, 1024);	//(u32 addr, u8 *buffer, u16 size)	��ȡ����
			FlashAddr += 1024;
			FlashRead_Nbytes(FlashAddr, voice_buff+1024, 1024);	//(u32 addr, u8 *buffer, u16 size)	��ȡ����
			FlashAddr += 1024;
			wr_index = 2048;	//д��������
			rd_index = 0;		//����������
			PlayByteCnt = 0;	//�����ֽڼ�����0
			dac_L = 0;	//��ʼDACֵ
			dac_R = 0;	//��ʼDACֵ
			B_PlayEn = 1;	//��������
			OP_index = 2;
		}
		else OP_index = 0, B_stop = 1;	//��IMA-ADPCM�ļ�
	}

	else if(OP_index == 2)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
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

	else if(OP_index == 3)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
	{
		if(FlashCheckBusy() == 0)
		{
			CDC_StringPrint("FLASH����\xfd���! �뷢��ADPCM�����ļ�!\r\n");
			FlashAddr  = 0;
			MusicBlock = 0;
			RcvTimeOut = 0;
			B_DownLoadStart = 0;
			OP_index = 4;		//�ȴ��´�����
			LED8[1] = DIS_D;	//��ʾ-DL-
			LED8[2] = DIS_L;
		}
	}
	else if(OP_index == 4)		//��������, 0:�޲���, 1:��ȡͷ�ļ�������������, 2: ��ȡ����, 3:����FLASH, 4:д��FFLASH
	{
		if (RxFlag)			//��RxFlagΪ1ʱ, ��ʾ�ѽ��յ�CDC��������, ���յ����ݴ�С������RxCount����,ÿ����������64�ֽ�, ���ݱ�����RxBuffer������.
		{
			B_DownLoadStart  =1;
			RcvTimeOut = 5;	// ��ʱ100ms����
			if(RxCount == 64)	//������һ������64�ֽ�
			{
				if((FlashAddr == 0) && (MusicBlock == 0))	//�յ���һ֡����
					FileLength   = ((u32)RxBuffer[7] << 24) + ((u32)RxBuffer[6] << 16) + (u32)RxBuffer[5]*256 + RxBuffer[4]+8;	//�ļ��ܳ�-8(����ȥRIFF�ͱ���4�ֽ�), С��ģʽ, [0x04] [0x05] [0x06] [0x07]
				for(j=0; j<64; j++)	voice_buff[MusicBlock++] = RxBuffer[j];	//�ݴ�����
				if(MusicBlock >= 256)	//��һҳ����дFLASH
				{
					FlashWrite_Nbytes(FlashAddr, voice_buff, 256);	//u32 addr, u8 *buffer, u16 size)
					FlashAddr += 256;
					if(FlashAddr >= FileLength)	OP_index = 0, B_stop = 1;	//�������
					MusicBlock = 0;
				}
			}
			else	//����64�ֽڣ��������ʣ����ֽ�
			{
				if((RxCount == 1) && (RxBuffer[0] == 'c'))	OP_index = 0;	//ȡ������
				for(j=0; j<RxCount; j++)	voice_buff[MusicBlock++] = RxBuffer[j];	//�ݴ�����
				FlashWrite_Nbytes(FlashAddr, voice_buff, MusicBlock);	//u32 addr, u8 *buffer, u16 size)
				FlashAddr += MusicBlock;
				OP_index = 0;	//�������
			}

			uart_recv_done();               //�Խ��յ����ݴ�����ɺ�,һ��Ҫ����һ���������,�Ա�CDC������һ�ʴ�������

		}
		if(B_DownLoadStart && (RcvTimeOut == 0))	B_DownLoadStart = 0, OP_index = 0, B_stop = 1;	//��ʱ����
		if(OP_index == 0)	CDC_StringPrint("���ؽ���!\r\n");
	}
}



//========================================================================
// ����: void CDC_StringPrint(u8 *puts)
// ����: ����1�ַ�����ӡ����
// ����: puts: �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2018-4-2
// ��ע:
//========================================================================
void CDC_StringPrint(u8 *puts)
{
	u16	j;
    for (j=0; *puts != 0;	puts++)	TxBuffer[j++] = *puts;
	uart_send(j);	//��������. ��Ҫ���͵����ݱ�����TxBuffer��������, Ȼ�����uart_send(n)�����������ݷ���,����Ϊ���͵��ֽ���. һ�����ɷ���64K,�����ڲ����Զ�����USB�ְ�.
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

// IMA-ADPCM�ı������
char idata index_adjust[16] = {-1,-1,-1,-1,2,4,6,8,-1,-1,-1,-1,2,4,6,8};

u16 const step_table[89] =
{
	7,8,9,10,11,12,13,14,16,17,19,21,23,25,28,31,34,37,41,45,
	50,55,60,66,73,80,88,97,107,118,130,143,157,173,190,209,230,253,279,307,337,371,
	408,449,494,544,598,658,724,796,876,963,1060,1166,1282,1411,1552,1707,1878,2066,
	2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,5894,6484,7132,7845,8630,9493,
	10442,11487,12635,13899,15289,16818,18500,20350,22385,24623,27086,29794,32767
};

void I2S_ISR(void) interrupt 62		//���2�����Ҳ�
{
	if (I2SSR & 0x02)				//���ͻ�������
	{
		if((I2SSR & 0x04) == 0)		//������
		{
			I2SDRH = (u8)(dac_L /256);		//������һ֡��Ƶ����
			I2SDRL = (u8)(dac_L %256);

			if(++cnt_1ms >= (u8)(SampleRate/1000))
			{
				DisplayScan();	//1msɨ����ʾһλ
				cnt_1ms = 0;
				if(++cnt_20ms == 20)	cnt_20ms = 0, B_20ms = 1;		//20msʱ϶
			}
		}
		else	//������  6~8us @40.96MHz
		{
			I2SDRH = (u8)(dac_R /256);		//������һ֡��Ƶ����
			I2SDRL = (u8)(dac_R %256);

			if(B_PlayEn)	//���ڲ���
			{
				if(PlayByteCnt >= FileLength)	B_PlayEn = 0, B_stop = 1;	//���������
				if(MusicChannel == 1)	//ADPCM ������
				{
					if((rd_index & MusicBlock) == 0)	//BLOCK��ʼ, һ����256��512��1024�ֽ�һ��BLOAK
					{
						dac_L = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// δѹ���Ĳ���ֵ, С��ģʽ
						rd_index += 2;	//ָ��index
						index_L = voice_buff[rd_index];	//indexֵ
						rd_index     +=2;
						PlayByteCnt  += 4;	//�Ѳ����ֽ�+4���ֽ�
						B_high_nibble = 0;
						dac_L += 32768;		//ת���޷�������
						cur_sample_L  = dac_L;
						cur_sample_L -= 32768;	//ת���з���
					}
					else	//����BLOCK��ʼ
					{
						decode = voice_buff[rd_index];	//�ӻ���������ֽ�
						if(B_high_nibble)		//����߰��ֽ�
						{
							decode >>= 4;
							PlayByteCnt++;	//�Ѳ����ֽ�+1�ֽ�
							rd_index++;		//ָ����һ������
							rd_index &= VOICE_BUFF_MASK;	//�������
						}
						decode &= 0x0f;
						delta = ((u32)step_table[index_L] * ((decode & 0x07)*2 +1)) / 8;	// ����delta
						if(decode & 8 )		delta = -delta;	//����delta
						cur_sample_L += delta;	//�������ǰ�Ĳ�������
							 if(cur_sample_L >= 32768)	dac_L = 65535;
						else if(cur_sample_L < -32768)	dac_L = 0;
						else 							dac_L = (u16)(cur_sample_L + 32768);

						index_L += index_adjust[decode];
							 if (index_L < 0)	index_L = 0;
						else if (index_L > 88)	index_L = 88;

						B_high_nibble = ~B_high_nibble;
					}
						dac_R = dac_L;		//������
				}

				else	//ADPCM ˫����
				{
					if((rd_index & MusicBlock) == 0)	//BLOCK��ʼ, 512��1024�ֽ�һ��BLOAK
					{
						dac_L = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// δѹ���Ĳ���ֵ
						rd_index += 2;	//ָ��index
						index_L = voice_buff[rd_index];	//indexֵ
						rd_index += 2;

						dac_R = (u16)voice_buff[rd_index] + ((u16)voice_buff[rd_index+1] << 8);	// δѹ���Ĳ���ֵ
						rd_index += 2;
						index_R = voice_buff[rd_index];	//indexֵ
						rd_index += 2;

						PlayByteCnt += 8;	//�Ѳ����ֽ���+8���ֽ�
						B_high_nibble = 0;
						dac_L += 32768;			//ת���޷�������
						cur_sample_L = dac_L;
						cur_sample_L -= 32768;	//ת���з���
						dac_R += 32768;			//ת���޷�������
						cur_sample_R = dac_R;
						cur_sample_R -= 32768;	//ת���з���
					}
					else	//����BLOCK��ʼ
					{
						if((rd_index & 0x0007) == 0)	//2��DWORD 8���ֽ�
						{
							ADPCM_Data_L[0] = voice_buff[rd_index++];	//����ѭ����ʱ�価����
							ADPCM_Data_L[1] = voice_buff[rd_index++];
							ADPCM_Data_L[2] = voice_buff[rd_index++];
							ADPCM_Data_L[3] = voice_buff[rd_index++];
							ADPCM_Data_R[0] = voice_buff[rd_index++];	//����ѭ����ʱ�価����
							ADPCM_Data_R[1] = voice_buff[rd_index++];
							ADPCM_Data_R[2] = voice_buff[rd_index++];
							ADPCM_Data_R[3] = voice_buff[rd_index];		//���һ����������+1, ��������Щ���ݲ�+1������д�뻺������
							B_high_nibble = 0;
							DecodeCnt     = 0;
						}

						//================ ��������ѹ�� ============================
						decode = ADPCM_Data_L[DecodeCnt];	//�ӻ���������ֽ�
						if(B_high_nibble)	decode >>= 4;	//����߰��ֽ�
						decode &= 0x0f;

						delta = ((u32)step_table[index_L] * ((decode & 0x07)*2 +1)) / 8;	// ����delta
						if(decode & 8 )		delta = -delta;	//����delta
						cur_sample_L += delta;	//�������ǰ�Ĳ�������
							 if(cur_sample_L >= 32768)	dac_L = 65535;
						else if(cur_sample_L < -32768)	dac_L = 0;
						else 							dac_L = (u16)(cur_sample_L + 32768);

						index_L += index_adjust[decode];
							 if (index_L < 0)	index_L = 0;
						else if (index_L > 88)	index_L = 88;
						//======================== ��������ѹ����� ==================================

						//======================== ��������ѹ�� ============================
						decode = ADPCM_Data_R[DecodeCnt];	//�ӻ���������ֽ�
						if(B_high_nibble)			//����߰��ֽ�
						{
							decode >>= 4;
							DecodeCnt++;
							if(DecodeCnt >= 4)	//4���ֽڶ��������
							{
								PlayByteCnt += 8;	//�Ѳ����ֽ���+8���ֽ�
								rd_index++;			//ָ����һ������
								rd_index &= VOICE_BUFF_MASK;	//�������
							}
						}
						decode &= 0x0f;

						delta = ((u32)step_table[index_R] * ((decode & 0x07)*2 +1)) / 8;	// ����delta
						if(decode & 8 )		delta = -delta;	//����delta
						cur_sample_R += delta;	//�������ǰ�Ĳ�������
							 if(cur_sample_R >= 32768)	dac_R = 65535;
						else if(cur_sample_R < -32768)	dac_R = 0;
						else 							dac_R = (u16)(cur_sample_R + 32768);

						index_R += index_adjust[decode];
							 if (index_R < 0)	index_R = 0;
						else if (index_R > 88)	index_R = 88;
						//======================== ��������ѹ����� ==================================

						B_high_nibble = ~B_high_nibble;	//�߰��ֽ�ָʾ
					}
				}
				dac_L -= 32768;		//ת���з���
				dac_R -= 32768;		//ת���з���
			}
			else
			{
				dac_L = 0;		//�޲��ž���
				dac_R = 0;		//�޲��ž���
			}
		}
	}
//	I2SSR &= ~0x5B;		//���Զ�����жϱ�־
}

