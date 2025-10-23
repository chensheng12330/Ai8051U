/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ�����MDU32���ļ�ʵ��AI8051UӲ���˳������滻��׼�㷨���㷨

����1(115200,N,8,1)��ӡ������

��ͨ������"STC32_MDU32_V1.x.LIB"�ļ�����ʾ��������IO�ڵ͵�ƽʱ�䣬���Ա�AI8051UӲ���˳�����Ԫ���׼�㷨��ļ���Ч��

��������ʱ��ʱ�ɽ����ڴ�ӡָ�����Σ�����鿴ÿ����ʽ�ļ���ʱ��

����ʱ, Ĭ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        24000000UL

volatile  unsigned long int near uint1, uint2;
volatile unsigned long int near xuint;

volatile long int sint1, sint2;
volatile long int xsint;

unsigned long ultest;
long ltest;

/*****************************************************************************/

sbit TPIN  =  P4^2;

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

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

void delay(unsigned char ms)
{
     while(--ms);
}

/*****************************************************************************/
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

    UartInit();

    printf("AI8051U MDU32 Test.\r\n");
    
    TPIN = 0;  //���㿪ʼͬ���ź�
    delay(2);
    TPIN = 1;
    delay(2);
    TPIN = 0;
    delay(2);
    TPIN = 1;
    delay(2);
    
    ultest = 12345678UL;
    ltest = 12345678;
    ultest = ultest / 12;
    ltest = ltest / 12;

    sint1 = 0x31030F05;
    sint2 = 0x00401350;
    TPIN = 0;
    xsint = sint1 * sint2;
    TPIN = 1;
    printf("Result1=0x%lx\r\n",xsint);

    uint1 =  5;
    uint2 =  50;
    TPIN = 0;
    xuint = uint1 * uint2;
    TPIN = 1;
    printf("Result2=%d\r\n",xuint);

    uint1 = 654689;
    uint2 = 528;
    TPIN = 0;
    xuint = uint1 / uint2;
    TPIN = 1;
    printf("Result3=%u\r\n",xuint);

    sint1 = 2134135177;
    sint2 = 20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result4=0x%lx\r\n",xsint);

    sint1 = -2134135177;
    sint2 = -20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result5=0x%lx\r\n",xsint);

    sint1 = 2134135177;
    sint2 = -20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result6=0x%lx\r\n",xsint);

    while(1);
}
/*****************************************************************************/
