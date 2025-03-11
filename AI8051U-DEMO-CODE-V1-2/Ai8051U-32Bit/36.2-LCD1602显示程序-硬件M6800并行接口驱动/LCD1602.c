/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

Ӳ��LCM M6800�ӿ�����LCD1602�ַ���.

��ʾЧ��Ϊ: LCD��ʾʱ��.

��һ����ʾ ---Clock demo---
�ڶ�����ʾ     12-00-00

LCD1602�ַ�������12864ģ��ӿ�1~16PIN.

R120����0ŷ���裬��16PIN�̽ӵ�GND.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include    "AI8051U.h"     //������ͷ�ļ��󣬲���Ҫ�ٰ���"reg51.h"ͷ�ļ�
#include    "intrins.h"

#define     MAIN_Fosc       24000000L   //������ʱ��

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/*************	Pin define	*****************************************************/

sbit	LCD_B7  = P2^7;	//D7 -- Pin 14		LED- -- Pin 16 
sbit	LCD_B6  = P2^6;	//D6 -- Pin 13		LED+ -- Pin 15
sbit	LCD_B5  = P2^5;	//D5 -- Pin 12		Vo   -- Pin 3
sbit	LCD_B4  = P2^4;	//D4 -- Pin 11		VDD  -- Pin 2
sbit	LCD_B3  = P2^3;	//D3 -- Pin 10		VSS  -- Pin 1
sbit	LCD_B2  = P2^2;	//D2 -- Pin  9
sbit	LCD_B1  = P2^1;	//D1 -- Pin  8
sbit	LCD_B0  = P2^0;	//D0 -- Pin  7

sbit	LCD_ENA	= P3^7;	//Pin 6
sbit	LCD_RW	= P3^6;	//Pin 5	//LCD_RS   R/W   DB7--DB0        FOUNCTION
sbit	LCD_RS	= P4^5;	//Pin 4	//	0		0	  INPUT      write the command to LCD model
								//	0		1     OUTPUT     read BF and AC pointer from LCD model
								//	1		0     INPUT      write the data to LCD  model
								//	1		1     OUTPUT     read the data from LCD model

u8	hour,minute,second;

void RTC(void);
void ClearLine(u8 row);
void Initialize_LCD(void);
void PutString(u8 row, u8 column, u8 *puts);
void DisplayRTC(void);
void delay_ms(u16 ms);
void WriteChar(u8 row, u8 column, u8 dat);
void LCM_Config(void);

void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    LCM_Config();
    Initialize_LCD();
    ClearLine(0);
    ClearLine(1);

    PutString(0,0,"---Clock demo---");
	
    hour   = 12;	//��ʼ��ʱ��ֵ
    minute = 0;
    second = 0;
    DisplayRTC();

    while(1)
    {
        delay_ms(1000);
        RTC();
        DisplayRTC();
    }
}

//========================================================================
// ����: void delay_ms(u16 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~65535ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void delay_ms(u16 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);
     }while(--ms);
}

//========================================================================
// ����: void DisplayRTC(void)
// ����: ��ʾʱ�Ӻ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void DisplayRTC(void)
{
	if(hour >= 10)	WriteChar(1,4,(u8)(hour / 10 + '0'));
	else			WriteChar(1,4,' ');
	WriteChar(1,5,(u8)(hour % 10 +'0'));
	WriteChar(1,6,'-');
	WriteChar(1,7,(u8)(minute / 10+'0'));
	WriteChar(1,8,(u8)(minute % 10+'0'));
	WriteChar(1,9,'-');
	WriteChar(1,10,(u8)(second / 10 +'0'));
	WriteChar(1,11,(u8)(second % 10 +'0'));
}

//========================================================================
// ����: void RTC(void)
// ����: RTC��ʾ����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void RTC(void)
{
	if(++second >= 60)
	{
		second = 0;
		if(++minute >= 60)
		{
			minute = 0;
			if(++hour >= 24)	hour = 0;
		}
	}
}

/************* LCD1602��س���	*****************************************************/
//8λ���ݷ��ʷ�ʽ	LCD1602		��׼����	������д	2014-2-21

#define LineLength	16		//16x2

/*
total 2 lines, 16x2= 32
first line address:  0~15
second line address: 64~79

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
#define C_BIT4			0x20		//set the data is 4 bits
#define C_L1DOT7		0x30		//8 bits,one line 5*7  dots
#define C_L1DOT10		0x34		//8 bits,one line 5*10 dots
#define C_L2DOT7		0x38		//8 bits,tow lines 5*7 dots
#define C_4bitL2DOT7	0x28		//4 bits,tow lines 5*7 dots
#define C_CGADDRESS0	0x40		//CGRAM address0 (addr=40H+x)
#define C_DDADDRESS0	0x80		//DDRAM address0 (addr=80H+x)

//========================================================================
// ����: void LCM_Config(void)
// ����: LCM���ú�����
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
// ����: void CheckBusy(void)
// ����: ���æ����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void CheckBusy(void)
{
	LCMIFSTA = 0;	//�����ɱ�־
    do
    {
		LCMIFCR   = ENLCMIF | LCM_RDCMD;	//��״̬
		while((LCMIFSTA & 1) == 0)	;	//�ȴ������
		LCMIFSTA = 0;	//�����ɱ�־
		LCD_ENA = 0;
    }while(LCMIFDATL & 0x80);	//bit7==0�����
}

//========================================================================
// ����: void IniSendCMD(u8 cmd)
// ����: ��ʼ��д����(�����æ)
// ����: cmd: Ҫд������.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void IniSendCMD(u8 cmd)
{
	LCMIFDATL = cmd;
	LCMIFCR   = ENLCMIF | LCM_WRCMD;	//д����
	while((LCMIFSTA & 1) == 0)	;	//�ȴ�д���
	LCMIFSTA = 0;	//�����ɱ�־
}

//========================================================================
// ����: void Write_CMD(u8 cmd)
// ����: д����(���æ)
// ����: cmd: Ҫд������.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void Write_CMD(u8 cmd)
{
	CheckBusy(); //���æ
	LCMIFDATL = cmd;
	LCMIFCR   = ENLCMIF | LCM_WRCMD;	//д����
	while((LCMIFSTA & 1) == 0)	;	//�ȴ�д���
	LCMIFSTA = 0;	//�����ɱ�־
}

//========================================================================
// ����: void Write_DIS_Data(u8 dat)
// ����: д��ʾ����(���æ)
// ����: dat: Ҫд������.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void Write_DIS_Data(u8 dat)
{
	CheckBusy(); //���æ
	LCMIFDATL = dat;
	LCMIFCR   = ENLCMIF | LCM_WRDAT;	//д����
	while((LCMIFSTA & 1) == 0)	;	//�ȴ�д���
	LCMIFSTA = 0;	//�����ɱ�־
}

//========================================================================
// ����: void Initialize_LCD(void)
// ����: ��ʼ������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void Initialize_LCD(void)
{
	LCD_ENA = 0;
	LCD_RS  = 0;
	LCD_RW = 0;

	delay_ms(100);
	IniSendCMD(C_BIT8);		//set the data is 8 bits

	delay_ms(10);
	Write_CMD(C_L2DOT7);		//tow lines 5*7 dots

	delay_ms(6);
	Write_CMD(C_CLEAR);		//clear LCD RAM
	Write_CMD(C_CUR_R);		//Curror Shift Right
	Write_CMD(C_ON);		//turn on  LCD
}

//========================================================================
// ����: void ClearLine(u8 row)
// ����: ���1��
// ����: row: ��(0��1)
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void ClearLine(u8 row)
{
	u8 i;
	Write_CMD(((row & 1) << 6) | 0x80);
	for(i=0; i<LineLength; i++)	Write_DIS_Data(' ');
}

//========================================================================
// ����: void WriteChar(u8 row, u8 column, u8 dat)
// ����: ָ���С��к��ַ�, дһ���ַ�
// ����: row: ��(0��1),  column: �ڼ����ַ�(0~15),  dat: Ҫд���ַ�.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void WriteChar(u8 row, u8 column, u8 dat)
{
	Write_CMD((((row & 1) << 6) + column) | 0x80);
	Write_DIS_Data(dat);
}

//========================================================================
// ����: void PutString(u8 row, u8 column, u8 *puts)
// ����: дһ���ַ�����ָ���С��к��ַ����׵�ַ
// ����: row: ��(0��1),  column: �ڼ����ַ�(0~15),  puts: Ҫд���ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2024-9-30
// ��ע: 
//========================================================================
void PutString(u8 row, u8 column, u8 *puts)
{
	Write_CMD((((row & 1) << 6) + column) | 0x80);
	for ( ;  *puts != 0;  puts++)		//����ֹͣ��0����
	{
		Write_DIS_Data(*puts);
		if(++column >= LineLength)	break;
	}
}

//******************** LCD Module END ***************************
