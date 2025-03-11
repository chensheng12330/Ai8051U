/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

��ADC�����ⲿ��ѹ��ʹ���ڲ���׼�����ѹ.

��STC��MCU��IO��ʽ����74HC595����8λ����ܡ�

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

�ұ�4λ�������ʾ�����ĵ�ѹֵ.

����1(P3.0,P3.1)���ã�115200,N,8,1��ʹ���ı�ģʽ��ӡ��ѹֵ.

�ⲿ��ѹ�Ӱ��ϲ��µ�����������, �����ѹ0~Vref, ��Ҫ����Vref�����0V. 

ʵ����Ŀʹ���봮һ��1K�ĵ��赽ADC�����, ADC������ٲ�һ��102~103���ݵ���.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/****************************** �û������ ***********************************/
#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/
#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*****************************************************************************/

    #define Cal_MODE    0   //ÿ�β���ֻ��1��ADC. �ֱ���0.01V
//  #define Cal_MODE    1   //ÿ�β���������16��ADC ��ƽ������. �ֱ���0.01V
	
/*****************************************************************************/

/*************  IO�ڶ���    **************/
sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  ���س�������    **************/
u8 code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��

/*************  ���ر�������    **************/
u8  LED8[8];        //��ʾ����
u8  display_index;  //��ʾλ����
bit B_1ms;          //1ms��־

u16 msecond;
u16 Bandgap;    //

/*************  ���غ�������    **************/
u16 Get_ADC12bitResult(u8 channel); //channel = 0~15
void UartInit(void);

/********************* ������ *************************/
void main(void)
{
    u8  i;
    u16 j;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x08;   P1M0 = 0x00;   //����Ϊ׼˫���,P1.3���ø�������
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x02;   //����Ϊ׼˫���,P5.1�����������
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    display_index = 0;

    ADCTIM = 0x3f;      //���� ADC �ڲ�ʱ��ADC����ʱ�佨�������ֵ
    ADCCFG = 0x2f;      //���� ADC ʱ��Ϊϵͳʱ��/2/16
    ADC_CONTR = 0x80;   //ʹ�� ADC ģ��

    AUXR |= 0x80;   //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;        //Timer0 interrupt enable
    TR0 = 1;        //Tiner0 run

    UartInit();
    EA = 1;         //�����ж�
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //�ϵ�����
    P51 = 1;        //��NTC����

    while(1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            if(++msecond >= 300)    //300ms��
            {
                msecond = 0;

            #if (Cal_MODE == 0)
            //=================== ֻ��1��ADC, 12bit ADC. �ֱ���0.01V ===============================
                Get_ADC12bitResult(15);  //�ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
                Bandgap = Get_ADC12bitResult(15);    //���ڲ���׼ADC, ��15ͨ��
                Get_ADC12bitResult(3);   //�ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
                j = Get_ADC12bitResult(3);  //���ⲿ��ѹADC
                j = (u16)((u32)j * 119 / Bandgap);  //�����ⲿ��ѹ, BandgapΪ1.19V, ���ѹ�ֱ���0.01V
            #endif
            //==========================================================================

            //===== ������16��ADC ��ƽ������. �ֱ���0.01V =========
            #if (Cal_MODE == 1)
                Get_ADC12bitResult(15);  //�ȶ�һ�β��������, ���ڲ��Ĳ������ݵĵ�ѹ��������ֵ.
                for(j=0, i=0; i<16; i++)
                {
                    j += Get_ADC12bitResult(15); //���ڲ���׼ADC, ��15ͨ��
                }
                Bandgap = j >> 4;   //16��ƽ��
                for(j=0, i=0; i<16; i++)
                {
                    j += Get_ADC12bitResult(3); //���ⲿ��ѹADC
                }
                j = j >> 4; //16��ƽ��
                j = (u16)((u32)j * 119 / Bandgap);  //�����ⲿ��ѹ, BandgapΪ1.19V, ���ѹ�ֱ���0.01V
            #endif
            //==========================================================================

                printf("VCC=%0.2fV\r\n",(float)j/100);

                LED8[5] = j / 100 + DIS_DOT;    //��ʾ�ⲿ��ѹֵ
                LED8[6] = (j % 100) / 10;
                LED8[7] = j % 10;
/*
                j = Bandgap;
                LED8[0] = j / 1000;     //��ʾBandgap ADCֵ
                LED8[1] = (j % 1000) / 100;
                LED8[2] = (j % 100) / 10;
                LED8[3] = j % 10;
*/
            }
        }
    }
}

//========================================================================
// ����: u16 Get_ADC12bitResult(u8 channel)
// ����: ��ѯ����һ��ADC���.
// ����: channel: ѡ��Ҫת����ADC.
// ����: 12λADC���.
// �汾: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //����ADCת��ͨ��
    ADC_START = 1;//����ADCת��
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //���ADC������־
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

/**************** ��HC595����һ���ֽں��� ******************/
void Send_595(u8 dat)
{
    u8  i;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        P_HC595_SER   = CY;
        P_HC595_SRCLK = 1;
        P_HC595_SRCLK = 0;
    }
}

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{
    Send_595(t_display[LED8[display_index]]);   //�������
    Send_595(~T_COM[display_index]);            //���λ��

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1msɨ����ʾһλ
    B_1ms = 1;      //1ms��־
}

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;      //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;      //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;        //��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}
