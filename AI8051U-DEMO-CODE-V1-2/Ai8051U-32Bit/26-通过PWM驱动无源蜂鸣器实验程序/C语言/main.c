/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ��P5.0�����PWM������Դ������.

����ɨ�谴��ÿ���������º��������һ��.

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
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/


/*************  ���س�������    **************/


/*************  ���ر�������    **************/

bit B_1ms;          //1ms��־
u8 cnt50ms;
u8  KeyCode;    //���û�ʹ�õļ���, 1~16��Ч
u8 IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //���м��̱���

/*************  ���غ�������    **************/
void IO_KeyScan(void);   //50ms call
void  delay_ms(u8 ms);

/********************* ������ *************************/
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

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMB_CCER1 = 0x00;  //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMB_CCMR1 = 0x30;  //ͨ��ģʽ���ã���תģʽ
    PWMB_CCER1 = 0x01;  //����ͨ�����ʹ�ܺͼ���

    PWMB_ARRH = 0x07;   //��������ʱ��
    PWMB_ARRL = 0xff;

    PWMB_ENO = 0x01;    //PWM5ʹ�����
    PWMB_PS = 0x03;     //�߼� PWM ͨ�������ѡ��λ, P50

    PWMB_BKR = 0x80;    //ʹ�������
    PWMB_CR1 |= 0x01;   //��ʼ��ʱ

    delay_ms(250);
    delay_ms(250);
    PWMB_ENO = 0x00;    //PWM5�ر����
    EA = 1;     //�����ж�

    while(1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            if(++cnt50ms >= 50)     //50msɨ��һ�����м���
            {
                cnt50ms = 0;
                IO_KeyScan();
            }
            
            if(KeyCode > 0)     //�м�����
            {
                PWMB_ENO = 0x01;    //PWM5ʹ�����
                delay_ms(250);
                PWMB_ENO = 0x00;    //PWM5�ر����
                KeyCode = 0;
            }
        }
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
void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);
     }while(--ms);
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}


/*****************************************************
    ���м�ɨ�����
    ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/


u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
    u8 i;
    i = 60;
    while(--i)  ;
}

void IO_KeyScan(void)    //50ms call
{
    u8  j;

    j = IO_KeyState1;   //������һ��״̬

    P0 = 0xf0;  //X�ͣ���Y
    IO_KeyDelay();
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //Y�ͣ���X
    IO_KeyDelay();
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //ȡ��
    
    if(j == IO_KeyState1)   //�������ζ����
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //�м�����
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //��һ�ΰ���
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1����ؼ�
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //������룬17~32
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}


