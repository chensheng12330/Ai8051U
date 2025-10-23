/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

��ʾЧ��Ϊ: �ϵ���������ʾ2��, Ȼ�����˯��ģʽ.

�����ϵ�P32��P33��P34��P35��������, ������ʾ2����ٽ���˯��ģʽ.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

//==========================================================================

#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

/*************  ���س�������    **************/

u8 code ledNum[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*************  ���ر�������    **************/

u8 ledIndex;
u16 msecond;    //1000ms����
u8 ioIndex; 

/*************  ���غ�������    **************/

void delay_ms(u8 ms);
void UartInit(void);

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

    P40 = 0;    //LED Power On

    //PnIM1,PnIM0  00:�½����ж�; 01:�������ж�; 10:�͵�ƽ�ж�; 11:�ߵ�ƽ�ж�
//    P0IM0 = 0xff;  //�ߵ�ƽ�ж�
//    P0IM1 = 0xff; 
//    P0INTE = 0xff; //ʹ�� P0 ���ж�
//    P0WKUE = 0xff; //ʹ�� P0 ���жϻ���

//    P1IM0 = 0xff;  //�������ж�
//    P1IM1 = 0x00; 
//    P1INTE = 0xff; //ʹ�� P1 ���ж�
//    P1WKUE = 0xff; //ʹ�� P1 ���жϻ���

//    P2IM0 = 0x00;  //�͵�ƽ�ж�
//    P2IM1 = 0xff; 
//    P2INTE = 0xff; //ʹ�� P2 ���ж�
//    P2WKUE = 0xff; //ʹ�� P2 ���жϻ���

    P3IM0 = 0x00;  //�½����ж�
    P3IM1 = 0x00; 
    P3INTE = 0x3c; //ʹ�� P3.2~P3.5 ���ж�
    P3WKUE = 0x3c; //ʹ�� P3.2~P3.5 ���жϻ���

//    P4IM0 = 0x00;  //�½����ж�
//    P4IM1 = 0x00; 
//    P4INTE = 0xff; //ʹ�� P4 ���ж�
//    P4WKUE = 0xff; //ʹ�� P4 ���жϻ���

//    P5IM0 = 0x00;  //�½����ж�
//    P5IM1 = 0x00; 
//    P5INTE = 0xff; //ʹ�� P5 ���ж�
//    P5WKUE = 0xff; //ʹ�� P5 ���жϻ���

    UartInit();
    
//    IRCDB = 0x10;

    P0INTF = 0;     //���жϱ�־
    P1INTF = 0;
    P2INTF = 0;
    P3INTF = 0;
    P4INTF = 0;
    P5INTF = 0;
    EA = 1;     //�������ж�

    while(1)
    {
        delay_ms(100);      //��ʱ100ms
        //�����ָʾ����״̬
        P0 = ~ledNum[ledIndex];    //���������
        ledIndex++;
        if(ledIndex > 7)
        {
            ledIndex = 0;
        }

        //2���MCU��������״̬
        if(++msecond >= 20)
        {
            msecond = 0;    //�����

            P0 = 0xff;      //�ȹر���ʾ��ʡ��
            printf("MCU Sleep.\r\n");

            PD = 1;         //Sleep
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            printf("MCU wakeup from P%02X.\r\n", ioIndex);
        }
    }
}

//========================================================================
//�����ж��������� 31���� KEIL ���޷�ֱ�ӱ���
//���õ� 13 ���ж���ڵ�ַ�������"isr.asm"�ļ�����Ŀ
//========================================================================
void common_isr() interrupt 13 
{ 
    u8 intf; 

    intf = P0INTF; //P0 ���ж�
    if (intf) 
    { 
        P0INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x00;
        if (intf & 0x02) ioIndex = 0x01;
        if (intf & 0x04) ioIndex = 0x02;
        if (intf & 0x08) ioIndex = 0x03;
        if (intf & 0x10) ioIndex = 0x04;
        if (intf & 0x20) ioIndex = 0x05;
        if (intf & 0x40) ioIndex = 0x06;
        if (intf & 0x80) ioIndex = 0x07;
    } 

    intf = P1INTF; //P1 ���ж�
    if (intf) 
    { 
        P1INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x10;
        if (intf & 0x02) ioIndex = 0x11;
        if (intf & 0x04) ioIndex = 0x12;
        if (intf & 0x08) ioIndex = 0x13;
        if (intf & 0x10) ioIndex = 0x14;
        if (intf & 0x20) ioIndex = 0x15;
        if (intf & 0x40) ioIndex = 0x16;
        if (intf & 0x80) ioIndex = 0x17;
    } 

    intf = P2INTF; //P2 ���ж�
    if (intf) 
    { 
        P2INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x20;
        if (intf & 0x02) ioIndex = 0x21;
        if (intf & 0x04) ioIndex = 0x22;
        if (intf & 0x08) ioIndex = 0x23;
        if (intf & 0x10) ioIndex = 0x24;
        if (intf & 0x20) ioIndex = 0x25;
        if (intf & 0x40) ioIndex = 0x26;
        if (intf & 0x80) ioIndex = 0x27;
    } 

    intf = P3INTF; //P3 ���ж�
    if (intf) 
    { 
        P3INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x30;
        if (intf & 0x02) ioIndex = 0x31;
        if (intf & 0x04) ioIndex = 0x32;
        if (intf & 0x08) ioIndex = 0x33;
        if (intf & 0x10) ioIndex = 0x34;
        if (intf & 0x20) ioIndex = 0x35;
        if (intf & 0x40) ioIndex = 0x36;
        if (intf & 0x80) ioIndex = 0x37;
    } 

    intf = P4INTF; //P4 ���ж�
    if (intf) 
    { 
        P4INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x40;
        if (intf & 0x02) ioIndex = 0x41;
        if (intf & 0x04) ioIndex = 0x42;
        if (intf & 0x08) ioIndex = 0x43;
        if (intf & 0x10) ioIndex = 0x44;
        if (intf & 0x20) ioIndex = 0x45;
        if (intf & 0x40) ioIndex = 0x46;
        if (intf & 0x80) ioIndex = 0x47;
    } 

    intf = P5INTF; //P5 ���ж�
    if (intf) 
    { 
        P5INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x50;
        if (intf & 0x02) ioIndex = 0x51;
        if (intf & 0x04) ioIndex = 0x52;
        if (intf & 0x08) ioIndex = 0x53;
        if (intf & 0x10) ioIndex = 0x54;
        if (intf & 0x20) ioIndex = 0x55;
        if (intf & 0x40) ioIndex = 0x56;
        if (intf & 0x80) ioIndex = 0x57;
    } 
}

//========================================================================
// ����: void delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2025-1-10
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
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
    S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
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
    S2TI = 0;       //Clear Tx flag
#endif
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}
