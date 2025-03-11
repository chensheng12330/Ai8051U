/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ����V1.1�汾���б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

128*64��LCD��ʾ����ʵ����MCU��3.3V���磬LCD������Ҫ��5V(R175����,R176�Ͽ�).

��ʾͼ�Σ����֣�Ӣ�ģ�����

����ͼ����ʾ������������ݾ�ʹ��DMA����������������24MHz, ÿ��LCM DMA�жϴ���ռ��CPUʱ��Ϊ2.3us, ������ʱ��84ms.

��Ҫ��ʾ�����ݷ���1024�ֽڵ��Դ��У�����DMA���伴�ɡ�

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ�ʺ����±��뼴��).

******************************************/

//	#define MAIN_Fosc        12000000UL
	#define MAIN_Fosc        24000000UL
//	#define MAIN_Fosc        40000000UL

	#include	"AI8051U.h"

	#include	<stdio.h>
	#include	<intrins.h>
	#include	"picture1.h"
	#include	"picture2.h"

/****************************** IO���� ***********************************/
	sbit	LCD_RS	= P4^5;
	sbit	LCD_RW	= P3^6;
	sbit	LCD_E	= P3^7;
	sbit	LCD_RST	= P4^7;
	#define	LCD_Data	P2

/*****************************************************************************/

/*************  ���س�������    **************/
const u8  uctech[] = {"LCD12864ͼ����ʾ"};
const u8  net[]    = {"��\xfd��оƬ��ST7920"};
const u8  mcu[]    = {"  �ڴ������ֿ�  "};
const u8  qq[]     = {" AI8051U LQFP48 "};

/*************  ���ر�������    **************/
u16	LCM_TxCnt;		//LCM DMA�������ʹ���, һ��16�ֽ�, һ��64��
bit	B_LCM_DMA_busy;	//LCM DMAæ��־�� 1��־LCM-DMAæ��LCM DMA�ж�������˱�־��ʹ��LCM DMAǰҪȷ�ϴ˱�־Ϊ0
u16	LCM_TxAddr;		//LCM DMAҪ�������ݵ��׵�ַ
bit	B_TxCmd;		//�ѷ��������־
u8	xdata CmdTmp[2];	//�����
u8	xdata DisTmp[1024] _at_ 0x0000;	//��ʾ���壬��Ҫ��ʾ�����ݷ����Դ������DMA����. ����LCM DMA��4�ֽڶ������⣬�������ﶨλ�Ե�ַΪ4�ı���

/*************  ���غ�������    **************/
void    delay_ms(u16 ms);
void    WriteDataLCD(u8 WDLCD);
void    WriteCommandLCD(u8 WCLCD);
void    ReadStatusLCD(void);
void    LCDInit(void);
void    LCDClear(void);
void    DisplayListChar(u8 X, u8 Y, const u8 *DData);
void    DisplayImage (u8 xdata *DData);
void 	LCM_Config(void);
void 	DMA_Config(u8 xdata *pic);

/********************* ������ *************************/
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
	P3n_push_pull(Pin7+Pin6);	//P3.7 P3.6����Ϊ�����������Ϊ��˫����IC������

	PullUpEnable(P2PU, 0xff);	// ����˿��ڲ���������       PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	PullUpEnable(P3PU, 0xc0);	// ����˿��ڲ���������       PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	PullUpEnable(P4PU, 0xa0);	// ����˿��ڲ���������       PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1

	delay_ms(100);	//�����ȴ�����LCD���빤��״̬
	LCM_Config();
	delay_ms(10);
	LCDInit();		//LCM��ʼ��
	delay_ms(10);	//��ʱƬ��
	EA = 1;

    while(1)
    {
		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture1[i];	//��ͼƬװ�ص��Դ�
		LCDClear();
		DMA_Config(DisTmp);	//����DMA����ʾͼ��
		delay_ms(3000);

		for(i=0; i<1024; i++)	DisTmp[i] = gImage_picture2[i];	//��ͼƬװ�ص��Դ�
		LCDClear();
		DMA_Config(DisTmp);	//����DMA����ʾͼ��
		delay_ms(3000);

		LCDClear();
		DisplayListChar(0,1,uctech);    //��ʾ�ֿ��е���������
		DisplayListChar(0,2,net);       //��ʾ�ֿ��е���������
		DisplayListChar(0,3,mcu);       //��ʾ�ֿ��е�����
		DisplayListChar(0,4,qq);        //��ʾ�ֿ��е���������
		delay_ms(3000);
    }
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע:
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do
    {
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

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
	LCMIFPSCR = (u8)(MAIN_Fosc/250000UL-1);	//TFT�����ӿ�ʱ��Ԥ��Ƶ0~255, LCMIFʱ��Ƶ�� = SYSclk/(LCMIFPSCR+1), LCD12864�ٶ��������250KHz
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

//DMA_LCM_CFG 	(7EfA70H)   SPI_DMA���üĴ���
#define		DMA_LCMIE	(1<<7)	// LCM DMA�ж�ʹ�ܿ���λ��bit7, 0:��ֹSPI DMA�жϣ�     1�������жϡ�
#define		DMA_LCMIP	(0<<2)	// LCM DMA�ж����ȼ�����λ��bit3~bit2, (���)0~3(���).
#define		DMA_LCMPTY	0		// LCM DMA�������߷������ȼ�����λ��bit1~bit0, (���)0~3(���).

//DMA_LCM_STA  (7EfA72) 	LCM_DMA״̬�Ĵ���
#define		LCM_TXOVW	(1<<1)	// LCM DMA���ݸ��Ǳ�־λ��bit1, �����0.
#define		DMA_LCMIF	1		// LCM DMA�ж������־λ��bit0, �����0.

void DMA_Config(u8 xdata *pic)
{
	WriteCommandLCD(0x36);			//ѡ������ָ�, ��ʾͼ��

	LCM_TxAddr   = (u16)pic;		//Ҫ�������ݵ��׵�ַ
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
		if(LCM_TxCnt >= 64)	//�жϷ����Ƿ����
		{
			DMA_LCM_CR = 0;
			B_LCM_DMA_busy = 0;		//���LCM-DMAæ��־��LCM DMA�ж�������˱�־��ʹ��LCM DMAǰҪȷ�ϴ˱�־Ϊ0
		}
		else		//��������Ҫ����
		{
			if(!B_TxCmd)	//��û�з����õ�ַ������ȷ����õ�ַ����
			{
				B_TxCmd = 1;	//ָʾ�ѷ���ַ����
				if(LCM_TxCnt <32)   //�ϰ���
				{
					CmdTmp[0] = (u8)(0x80+LCM_TxCnt);	//�е�ַ
					CmdTmp[1] = 0x80;					//�е�ַ���ϰ�����ַ0X80
				}
				else   //�°���
				{
					CmdTmp[0] = (u8)(0x80+LCM_TxCnt-32);	//�е�ַ
					CmdTmp[1] = 0x88;						//�е�ַ���°�����ַ0X88
				}
				DMA_LCM_TXAH = (u8)((u16)CmdTmp >> 8);	//LCM DMA���������׵�ַ
				DMA_LCM_TXAL = (u8)CmdTmp;
				DMA_LCM_AMTH = 0;				//���ô������ֽ���(��8λ),	���ô������ֽ��� = N+1
				DMA_LCM_AMT  = 2-1;				//���ô������ֽ���(��8λ).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWC;	//����LCM DMA��������
			}
			else
			{
				B_TxCmd = 0;	//����ѷ���ַ����
				DMA_LCM_TXAH = (u8)(LCM_TxAddr >> 8);	//LCM DMA���������׵�ַ
				DMA_LCM_TXAL = (u8)LCM_TxAddr;
				DMA_LCM_AMTH = 0;				//���ô������ֽ���(��8λ),	���ô������ֽ��� = N+1
				DMA_LCM_AMT  = 16-1;			//���ô������ֽ���(��8λ).
				DMA_LCM_CR   = DMA_ENLCM | LCM_TRIGWD;	//����LCM DMA��������
				LCM_TxAddr  += 16;	//Ҫ�������ݵ��׵�ַ, һ��DMA����16�ֽ�
				LCM_TxCnt++;		//���ʹ���+1
			}
		}
	}
	DMA_LCM_STA = 0;
}

//��״̬
void ReadStatusLCD(void)
{
//	LCD_Data = 0xFF;
	LCMIFSTA = 0;	//�����ɱ�־
    do
    {
		LCMIFCR   = ENLCMIF | LCM_RDCMD;	//��״̬
		while((LCMIFSTA & 1) == 0)	;	//�ȴ������
		LCMIFSTA = 0;	//�����ɱ�־
		LCD_E = 0;
    }while(LCMIFDATL & 0x80);	//bit7==0�����
}

//д����
void WriteDataLCD(u8 WDLCD)
{
	ReadStatusLCD(); //���æ
	LCMIFDATL = WDLCD;
	LCMIFCR   = ENLCMIF | LCM_WRDAT;	//д����
	while((LCMIFSTA & 1) == 0)	;	//�ȴ�д���
	LCMIFSTA = 0;	//�����ɱ�־
}

//дָ��
void WriteCommandLCD(u8 WCLCD)
{
	ReadStatusLCD(); //���æ
	LCMIFDATL = WCLCD;
	LCMIFCR   = ENLCMIF | LCM_WRCMD;	//д����
	while((LCMIFSTA & 1) == 0)	;	//�ȴ�д���
	LCMIFSTA = 0;	//�����ɱ�־
}

void LCDInit(void) //LCM��ʼ��
{
	delay_ms(10);
	LCD_RST = 0;
	delay_ms(10);
	LCD_RST = 1;
	delay_ms(100);	//����40ms

	WriteCommandLCD(0x30);	//��ʾģʽ����,��ʼҪ��ÿ�μ��æ�ź�
	WriteCommandLCD(0x01);	//��ʾ����
	WriteCommandLCD(0x04);	//������ʾ���ã�������
	WriteCommandLCD(0x0C);	//��˯��
}

void LCDClear(void) //����
{
	WriteCommandLCD(0x30); //ѡ�����ָ�
	WriteCommandLCD(0x01); //��ʾ����, æ�źų���1.2ms
}

//��ָ��λ����ʾһ���ַ�
void DisplayListChar(u8 X, u8 Y, const u8 *DData)
{
	u8 ListLength,X2;
	X2 = X;
	if(Y < 1)   Y=1;
	if(Y > 4)   Y=4;
	X &= 0x0F; //����X���ܴ���16��Y��1-4֮��
	WriteCommandLCD(0x30);		//ѡ�����ָ�
	switch(Y)
	{
		case 1: X2 |= 0X80; break;  //����������ѡ����Ӧ��ַ
		case 2: X2 |= 0X90; break;
		case 3: X2 |= 0X88; break;
		case 4: X2 |= 0X98; break;
	}
	WriteCommandLCD(X2); //���͵�ַ��
	ListLength = 0;
	while (DData[ListLength] >= 0x20) //�������ִ�β���˳�
	{
		if (X <= 0x0F) //X����ӦС��0xF
		{
			WriteDataLCD(DData[ListLength]); //
			ListLength++;
			X++;
		}
	}
}
