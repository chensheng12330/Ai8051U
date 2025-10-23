
#define 	MAIN_Fosc		40000000UL	//������ʱ��

#include	"AI8051U.h"


/*************	����˵��	**************

���ȱ��޸ĳ���, ֱ������"LCD1602.hex"����, ����ʱѡ����Ƶ40MHz.

LCM1602�ַ�Һ��ģ�飬8λ���ݣ�ȫ����LCM DMA��������ʡCPUʱ��(LCD1602��IO��ʽ�����ٶȺ���!)��

��������AI8051Uʵ��������֤��ʹ��3.3V��LCD1602ģ�顣

*******************************************/


/*************  ���س�������    **************/
const u8  text1[] = {" LCD1602 8-DATA "};
const u8  text2[] = {" AI8051U-34K64  "};
const u8  text3[] = {"2024-10-22 21:29"};
const u8  text4[] = {" WWW.STCAI.com  "};


/*************  ���ر�������    **************/
u16	LCM_TxCnt;		//LCM DMA�������ʹ���, һ��16�ֽ�, һ��64��
bit	B_LCM_DMA_busy;	//LCM DMAæ��־�� 1��־LCM-DMAæ��LCM DMA�ж�������˱�־��ʹ��LCM DMAǰҪȷ�ϴ˱�־Ϊ0
bit	B_TxCmd;		//�ѷ��������־
u8	xdata CmdTmp[2];	//�����
u8	xdata DisTmp1[16];	//��һ����ʾ���壬��Ҫ��ʾ�����ݷ����Դ������DMA����.
u8	xdata DisTmp2[16];	//�ڶ�����ʾ���壬��Ҫ��ʾ�����ݷ����Դ������DMA����.


/*************  ���غ�������    **************/



/*************	Pin define	*****************************************************/
sfr		LCD_DATA = 0xa0;	//P0--0x80,  P1--0x90,  P2--0xa0,  P3--0xb0
sbit	LCD_B7  = P2^7;	//Pin 14	B3--Pin 10		LED+ -- Pin 15
						//Pin 13	B2--Pin 9		LED- -- Pin 16
						//Pin 12	B1--Pin 8		Vo   -- Pin 3
						//Pin 11	B0--Pin 7		VDD  -- Pin 2		VSS -- Pin 1

sbit	LCD_ENA	= P3^7;	//Pin 6
sbit	LCD_RW	= P3^6;	//Pin 5	//LCD_RS   R/W   DB7--DB0        FOUNCTION
sbit	LCD_RS	= P4^5;	//Pin 4	//	0		0	  INPUT      write the command to LCD model
								//	0		1     OUTPUT     read BF and AC pointer from LCD model
								//	1		0     INPUT      write the data to LCD  model
								//	1		1     OUTPUT     read the data from LCD model

/******************************************************************************
                 HD44780U    LCD_MODUL DRIVE PROGRAMME
*******************************************************************************

total 2 lines, 16x2= 32
first line address:  0~15
second line address: 64~79

total 2 lines, 20x2= 40
first line address:  0~19
second line address: 64~83

total 2 lines, 40x2= 80
first line address:  0~39
second line address: 64~103
*/

#define C_CLEAR			0x01		//clear LCD
#define C_HOME 			0x02		//cursor go home
#define C_CUR_L			0x04		//cursor shift left after input
#define C_RIGHT			0x05		//picture shift right after input
#define C_CUR_R			0x06		//cursor shift right after input
#define C_LEFT 			0x07		//picture shift left after input
#define C_OFF  			0x08		//turn off LCD
#define C_ON   			0x0C		//turn on  LCD
#define C_FLASH			0x0D		//turn on  LCD, flash
#define C_CURSOR		0x0E		//turn on  LCD and cursor
#define C_FLASH_ALL		0x0F		//turn on  LCD and cursor, flash
#define C_CURSOR_LEFT	0x10		//single cursor shift left
#define C_CURSOR_RIGHT	0x10		//single cursor shift right
#define C_PICTURE_LEFT	0x10		//single picture shift left
#define C_PICTURE_RIGHT	0x10		//single picture shift right
#define C_BIT8			0x30		//set the data is 8 bits
#define C_BIT4			0x20		//set the data is 8 bits
#define C_L1DOT7		0x30		//8 bits,one line 5*7  dots
#define C_L1DOT10		0x34		//8 bits,one line 5*10 dots
#define C_8bitL2DOT7	0x38		//8 bits,tow lines 5*7 dots
#define C_4bitL2DOT7	0x28		//4 bits,tow lines 5*7 dots
#define C_CGADDRESS0	0x40		//CGRAM address0 (addr=40H+x)
#define C_DDADDRESS0	0x80		//DDRAM address0 (addr=80H+x)


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
	 	i = MAIN_Fosc / 6000;
		while(--i)	;
     }while(--ms);
}

void	LCD_DelayNop(void)
{
	NOP(27);	// �Ҳ��Ե������� ���÷���6T+24��NOP @40MHz, ����NOP���ٸ���=FOSC(MHz) * 0.75-6
}

void	CheckBusy(void)		//���æ��־
{
	u16	i;
	LCD_RS = 0;		//��æ��־��ACָ��
	LCD_RW = 1;
	LCD_DATA = 0xff;
	LCD_DelayNop();
	LCD_ENA = 1;
	for(i=0; i<10000; i++)		//æ��� check the LCD busy or not. With time out. �Ҳ��Ե�������6500@40MHz
	{
		if(!LCD_B7)	break;
	}
	LCD_ENA = 0;
}

/**********************************************/
void IniSendCMD(u8 cmd)		//write the command to LCD
{
	LCD_RS = 0;		//д����
	LCD_RW = 0;
	LCD_DATA = cmd;
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_DATA = 0xff;
	LCD_DelayNop();
}

/**********************************************/
void Write_CMD(u8 cmd)		//write the command to LCD
{
	CheckBusy();	//���æ��־  check the LCD busy or not.

	LCD_RS = 0;		//д����
	LCD_RW = 0;
	LCD_DATA = cmd;
	LCD_DelayNop();
	LCD_ENA = 1;
	LCD_DelayNop();
	LCD_ENA = 0;
	LCD_DATA = 0xff;
	LCD_DelayNop();
}


/*********	��ʼ������	**************************/
void Initialize_LCD(void)		//intilize LCD, input none, output none
{
	P2n_standard(0xff);
	P3n_standard(Pin7+Pin6);
	P4n_standard(Pin5);
	delay_ms(1);

	LCD_ENA = 0;
	LCD_RS  = 0;
	LCD_RW  = 0;

	delay_ms(100);
	IniSendCMD(C_BIT8);		//set the data is 4 bits

	delay_ms(10);
	IniSendCMD(C_BIT8);		//set the data is 4 bits

	delay_ms(10);
	IniSendCMD(C_8bitL2DOT7);	//tow lines 5*7 dots

	delay_ms(6);
	Write_CMD(C_CLEAR);		//clear LCD RAM
	Write_CMD(C_CUR_R);		//Curror Shift Right
	Write_CMD(C_ON);		//turn on  LCD
}

//******************** LCD40 Module END ***************************

//========================================================================
// ����: void LCM_Config(void)
// ����: LCM���ú�����ST7920 @540KHz������ָ�����ʱ��Ϊ1.6ms����æ�ź�0us, д�롢�������ݡ�����72us. ������DMAˢ��ʱ��84ms @24MHz.
// ����: none.
// ����: none.
// �汾: V1.0, 2024-8-17
// ��ע:
//========================================================================
//LCMIFCFG  (7EFE50H)	TFT�����ӿ����üĴ���
#define	LCMIFIE			(0<<7)	//TFT�����ӿ��ж�ʹ�ܿ���λ(bit7), 1->�����жϣ�0->��ֹ
#define	LCMIFIP			(0<<4)	//TFT�����ӿ��ж����ȼ�����λ(bit5~bit4), (���)0~3(���).
#define	LCMIFDPS		(0<<2)	//TFT�����ӿ����ݽ�ѡ��λ(bit3~bit2), D18_D8=0: 0��2->8λ������P2, D18_D8=1: 0: P2-���ֽ�, P0���ֽ�, 2: P2-���ֽ�,���ֽ�P0[7:4] P4.7 P4.6 P4.3 P4.1.
#define	LCM_D16_D8		(0<<1)	//TFT�����ӿ����ݿ�ȿ���λ(bit1), 0: 8λ���ݣ�1:16λ����
#define	LCM_M68_I80		1		//TFT�����ӿ�����ģʽѡ��λ(bit0), 0: I8080ģʽ��1:M6800ģʽ

//LCMIFCFG2  (7EFE51H)	TFT�����ӿ����üĴ���2
#define	LCMIFCPS		(1<<5)	//TFT�����ӿڿ��ƽ�ѡ��λ(bit6~bit5), RS RD(E) WR(RW), 0->P4.5 P4.4 P4.2��1->P4.5 P3.7 P3.6, 2->P4.0 P4.4 P4.2, 3->P4.0 P3.7 P3.6
#define	LCMSETUPT		(4<<2)	//TFT�����ӿ����ݽ���ʱ�����λ(bit4~bit2), 0~7, ��Ӧ1~8��LCMIFʱ��, ����6800ģʽ����E�ߵ�ƽʱ��, ����20us.
#define	LCMHOLDT		3		//TFT�����ӿ����ݱ���ʱ�����λ(bit1~bit0), 0~3, ��Ӧ1~4��LCMIFʱ��

//LCMIFCR  (7EFE52H)	TFT�����ӿڿ��ƼĴ���
#define	ENLCMIF			(1<<7)	//TFT�����ӿ�ʹ�ܿ���λ(bit7), 1->����TFT�����ӿڹ���, 0->��ֹ
#define	LCM_WRCMD		4		//TFT�����ӿڴ�������(bit2~bit0), 4->д����
#define	LCM_WRDAT		5		//TFT�����ӿڴ�������(bit2~bit0), 5->д����
#define	LCM_RDCMD		6		//TFT�����ӿڴ�������(bit2~bit0), 6->������/״̬
#define	LCM_RDDAT		7		//TFT�����ӿڴ�������(bit2~bit0), 7->������

void LCM_Config(void)
{
	LCMIFCFG  = LCMIFIE  | LCMIFIP   | LCMIFDPS | LCM_D16_D8 | LCM_M68_I80;
	LCMIFCFG2 = LCMIFCPS | LCMSETUPT | LCMHOLDT;	//RS:P45,E:P37,RW:P36; Setup Time,HOLD Time
	LCMIFSTA  = 0x00;	//TFT�����ӿ��ж������־�������0
	LCMIFPSCR = (u8)(MAIN_Fosc/250000UL-1);	//TFT�����ӿ�ʱ��Ԥ��Ƶ0~255, LCMIFʱ��Ƶ�� = SYSclk/(LCMIFPSCR+1), LCD1602�ٶ��������250KHz
	LCMIFCR   = ENLCMIF;
}

//========================================================================
// ����: void DMA_Config(u8 xdata *pic)
// ����: LCM DMA���ú�����
// ����: *pic: �����׵�ַ.
// ����: none.
// �汾: V1.0, 2024-8-17
// ��ע:
//========================================================================
//DMA_LCM_CR (7EfA71H) 	LCM_DMA���ƼĴ���
#define		DMA_ENLCM	(1<<7)	// LCM DMA����ʹ�ܿ���λ��    bit7, 0:��ֹLCM DMA���ܣ�  1������LCM DMA���ܡ�
#define		LCM_TRIGWC	(1<<6)	// LCM DMA����д���bit6, 0:д0��Ч��          1��д1��ʼLCM DMA��ʼд���
#define		LCM_TRIGWD	(1<<5)	// LCM DMA����д���ݣ�bit5, 0:д0��Ч��          1��д1��ʼLCM DMA��ʼд���ݡ�
#define		LCM_TRIGRC	(1<<4)	// LCM DMA���������bit4, 0:д0��Ч��          1��д1��ʼLCM DMA��ʼ�����
#define		LCM_TRIGRD	(1<<3)	// LCM DMA���������ݣ�bit3, 0:д0��Ч��          1��д1��ʼLCM DMA��ʼ�����ݡ�
#define		LCM_CLRFIFO	0		// ���LCM DMA����FIFO����λ��bit0, 0:д0��Ч��  1��д1��λFIFOָ�롣

//DMA_LCM_CFG 	(7EfA70H)   LCM_DMA���üĴ���
#define		DMA_LCMIE	(1<<7)	// LCM DMA�ж�ʹ�ܿ���λ��bit7, 0:��ֹSPI DMA�жϣ�     1�������жϡ�
#define		DMA_LCMIP	(0<<2)	// LCM DMA�ж����ȼ�����λ��bit3~bit2, (���)0~3(���).
#define		DMA_LCMPTY	0		// LCM DMA�������߷������ȼ�����λ��bit1~bit0, (���)0~3(���).

//DMA_LCM_STA  (7EfA72) 	LCM_DMA״̬�Ĵ���
#define		LCM_TXOVW	(1<<1)	// LCM DMA���ݸ��Ǳ�־λ��bit1, �����0.
#define		DMA_LCMIF	1		// LCM DMA�ж������־λ��bit0, �����0.

void LCM_DMA_Trig(void)
{
	DMA_LCM_STA  = 0x00;
	DMA_LCM_CFG  = DMA_LCMIE | DMA_LCMIP | DMA_LCMPTY;;
	DMA_LCM_ITVH = 0;	//���ô�����ʱ��(��8λ)����ӦN+1��LCMIFʱ��(1~65536��LCMIFʱ��)
	DMA_LCM_ITVL = 9-1;	//���ô�����ʱ��(��8λ)

	B_TxCmd   = 0;		//�ѷ��������־
	LCM_TxCnt = 0;		//LCM DMA�������ʹ���, һ��16�ֽ�, һ��64��
	B_LCM_DMA_busy = 1;	//��־LCM-DMAæ��LCM DMA�ж�������˱�־��ʹ��LCM DMAǰҪȷ�ϴ˱�־Ϊ0
	DMA_LCM_STA = DMA_LCMIF;	//�������LCM DMA�жϣ���������
}

//========================================================================
// ����: void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
// ����: LCM DMA�жϺ������жϴ���ʱ��:2.3us @24MHz. ˢ��ʱ�䣺84ms @24MHz.
// ����: none.
// ����: none.
// �汾: V1.0, 2024-8-17
// ��ע:
//========================================================================
void LCMIF_DMA_ISR(void) interrupt DMA_LCM_VECTOR
{
	if(DMA_LCM_STA & DMA_LCMIF)
	{
		if(LCM_TxCnt >= 2)	//�жϷ����Ƿ����
		{
			DMA_LCM_CR = 0;
			B_LCM_DMA_busy = 0;		//���LCM-DMAæ��־��LCM DMA�ж�������˱�־��ʹ��LCM DMAǰҪȷ�ϴ˱�־Ϊ0
		}
		else		//��������Ҫ����
		{
			if(!B_TxCmd)	//��û�з����õ�ַ������ȷ����õ�ַ����
			{
				B_TxCmd = 1;	//ָʾ�ѷ���ַ����
				if(LCM_TxCnt == 0)   //��һ�е�ַ��������
				{
					LCD_RS = 0;		//д����
					LCD_RW = 0;
					CmdTmp[0] = 0x80;	//��һ��AC��ַ
				}
				else if(LCM_TxCnt == 1)   //�ڶ��е�ַ��������
				{
					LCD_RS = 0;		//д����
					LCD_RW = 0;
					CmdTmp[0] = 0x80 | 64;	//�ڶ���AC��ַ
				}
				DMA_LCM_TXAH = (u8)((u16)CmdTmp >> 8);	//LCM DMA���������׵�ַ
				DMA_LCM_TXAL = (u8)CmdTmp;
				DMA_LCM_AMTH = 0;			//���ô������ֽ���(��8λ),	���ô������ֽ��� = N+1
				DMA_LCM_AMT  = 1-1;			//���ô������ֽ���(��8λ).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWC;	//����LCM DMA��������
			}
			else	//д����
			{
				LCD_RS = 1;		//д��ʾ����
				LCD_RW = 0;
				B_TxCmd = 0;	//����ѷ���ַ����
				if(LCM_TxCnt == 0)   //��һ����ʾ�����ַ
				{
					DMA_LCM_TXAH = (u8)((u16)DisTmp1 >> 8);	//LCM DMA���������׵�ַ
					DMA_LCM_TXAL = (u8)DisTmp1;
				}
				else //if(LCM_TxCnt == 1)   //�ڶ�����ʾ�����ַ
				{
					DMA_LCM_TXAH = (u8)((u16)DisTmp2 >> 8);	//LCM DMA���������׵�ַ
					DMA_LCM_TXAL = (u8)DisTmp2;
				}
				DMA_LCM_AMTH = 0;				//���ô������ֽ���(��8λ),	���ô������ֽ��� = N+1
				DMA_LCM_AMT  = 16-1;			//���ô������ֽ���(��8λ).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//����LCM DMA��������
				LCM_TxCnt++;		//��������+1
			}
		}
	}
	DMA_LCM_STA = 0;
}


/*************** ������ *******************************/

void main(void)
{
	u16	i;

	EAXFR = 1;	//��չ�Ĵ���(XFR)����ʹ��
	WTST  = 0;	//���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
	CKCON = 0;	//��߷���XRAM�ٶ�

	P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
	P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
	P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
	P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
	P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
	P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
	P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
	P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

	delay_ms(100);	//�ȴ�һ�£���LCD���빤��״̬
	LCM_Config();
	Initialize_LCD();	//��ʼ��LCD
	EA = 1;

	while (1)
	{
		if(!B_LCM_DMA_busy)		//DMA����
		{
			for(i=0; i<16; i++)	//��Ҫ��ʾ���ı�װ�ص��Դ�
			{
				DisTmp1[i] = text1[i];	//���ص�һ���Դ�
				DisTmp2[i] = text2[i];	//���صڶ����Դ�
			}
			LCM_DMA_Trig();		//����DMA����ʾ�ı�
		}
		delay_ms(2000);

		if(!B_LCM_DMA_busy)		//DMA����
		{
			for(i=0; i<16; i++)	//��Ҫ��ʾ���ı�װ�ص��Դ�
			{
				DisTmp1[i] = text3[i];	//���ص�һ���Դ�
				DisTmp2[i] = text4[i];	//���صڶ����Դ�
			}
			LCM_DMA_Trig();		//����DMA����ʾ�ı�
		}
		delay_ms(2000);
	}
}

