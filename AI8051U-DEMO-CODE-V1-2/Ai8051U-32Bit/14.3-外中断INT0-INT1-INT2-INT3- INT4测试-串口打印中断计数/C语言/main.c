/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д����.

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�ô��ڴ�ӡINT�жϼ��������INT0������ܿ��ƽӿڸ��ã��������������ʾ����

����(P30,P31)Ĭ�����ã�115200,N,8,1��ʹ���ı�ģʽ��ӡ.

���ڰ����ǻ�е����, �����ж���, ��������û��ȥ��������, ���԰�һ���ж������Ҳ��������.

INT2, INT3, INT4 ʵ�����û���������԰���������Ҫʱ�ο�ʹ��.

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

//==========================================================================

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

/*************  ���ر�������    **************/

bit int0Flag;
bit int1Flag;
bit int2Flag;
bit int3Flag;
bit int4Flag;
u8  INT0_cnt, INT1_cnt; //�����õļ�������
u8  INT2_cnt, INT3_cnt, INT4_cnt; //�����õļ�������

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

//    SCON = (SCON & 0x3f) | 0x40; 
//    T2L  = TM;
//    T2H  = TM>>8;
//    AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
    S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
    S2CON = (S2CON & 0x3f) | 0x40; 
    T2L  = TM;
    T2H  = TM>>8;
    AUXR |= 0x14;   //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
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

/******************** ������ **************************/
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
    
    P3PU = 0x0c;    //P3.2,P3.3ʹ���ڲ�����

    UartInit();
    
    INT0_cnt = 0;
    INT1_cnt = 0;

    IE1  = 0;   //���ж�1��־λ
    IE0  = 0;   //���ж�0��־λ
    EX1 = 1;    //INT1 Enable
    EX0 = 1;    //INT0 Enable

    IT0 = 1;    //INT0 �½����ж�
//  IT0 = 0;    //INT0 ����,�½����ж�  
    IT1 = 1;    //INT1 �½����ж�
//  IT1 = 0;    //INT1 ����,�½����ж�  

    //INT2, INT3, INT4 ʵ�����û���������԰���������Ҫʱ�ο�ʹ��
    EX2 = 1;    //ʹ�� INT2 �½����ж�
    EX3 = 1;    //ʹ�� INT3 �½����ж�
    EX4 = 1;    //ʹ�� INT4 �½����ж�

    EA = 1;     //�������ж�

    while(1)
    {
        if(int0Flag)
        {
            int0Flag = 0;
            printf("int0 cnt=%bu\r\n",INT0_cnt);
        }

        if(int1Flag)
        {
            int1Flag = 0;
            printf("int1 cnt=%bu\r\n",INT1_cnt);
        }
    }
}

/********************* INT0�жϺ��� *************************/
void INT0_int (void) interrupt 0      //���ж�ʱ�Ѿ������־
{
    INT0_cnt++; //�ж�+1
    int0Flag = 1;
}

/********************* INT1�жϺ��� *************************/
void INT1_int (void) interrupt 2      //���ж�ʱ�Ѿ������־
{
    INT1_cnt++; //�ж�+1
    int1Flag = 1;
}

/********************* INT2�жϺ��� *************************/
void INT2_int (void) interrupt 10     //���ж�ʱ�Ѿ������־
{
    INT2_cnt++; //�ж�+1
    int2Flag = 1;
}

/********************* INT3�жϺ��� *************************/
void INT3_int (void) interrupt 11     //���ж�ʱ�Ѿ������־
{
    INT3_cnt++; //�ж�+1
    int3Flag = 1;
}

/********************* INT4�жϺ��� *************************/
void INT4_int (void) interrupt 16     //���ж�ʱ�Ѿ������־
{
    INT4_cnt++; //�ж�+1
    int4Flag = 1;
}
