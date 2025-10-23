/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�߼�PWM��ʱ�� PWM5,PWM6,PWM7,PWM8 ÿ��ͨ�����ɶ���ʵ��PWM���.

4��ͨ��PWMͨ��P0�������ӵ�LED�ƣ��۲������Ч��.

PWM���ں�ռ�ձȿ����Զ������ã���߿ɴ�65535.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define PWM5_0      0x00	//P0.1
#define PWM5_1      0x01	//P1.1
#define PWM5_2      0x02	//P2.1
#define PWM5_3      0x03	//P5.0

#define PWM6_0      0x00	//P0.3
#define PWM6_1      0x04	//P1.3
#define PWM6_2      0x08	//P2.3
#define PWM6_3      0x0C	//P5.1

#define PWM7_0      0x00	//P0.5
#define PWM7_1      0x10	//P1.5
#define PWM7_2      0x20	//P2.5
#define PWM7_3      0x30	//P5.2

#define PWM8_0      0x00	//P0.7
#define PWM8_1      0x40	//P1.7
#define PWM8_2      0x80	//P2.7
#define PWM8_3      0xC0	//P5.3

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

/*****************************************************************************/


/*************  ���س�������    **************/

#define PWM_PERIOD  1023    //��������ֵ

/*************  ���ر�������    **************/

bit B_1ms;          //1ms��־

u16 PWM5_Duty;
u16 PWM6_Duty;
u16 PWM7_Duty;
u16 PWM8_Duty;

bit PWM5_Flag;
bit PWM6_Flag;
bit PWM7_Flag;
bit PWM8_Flag;

void UpdatePwm(void);

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

    PWM5_Flag = 0;
    PWM6_Flag = 0;
    PWM7_Flag = 0;
    PWM8_Flag = 0;
    
    PWM5_Duty = 0;
    PWM6_Duty = 256;
    PWM7_Duty = 512;
    PWM8_Duty = 1024;

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMB_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMB_CCER2 = 0x00;
    PWMB_CCMR1 = 0x60; //ͨ��ģʽ����
    PWMB_CCMR2 = 0x60;
    PWMB_CCMR3 = 0x60;
    PWMB_CCMR4 = 0x60;
    PWMB_CCER1 = 0x33; //����ͨ�����ʹ�ܺͼ���
    PWMB_CCER2 = 0x33;

    PWMB_CCMR1 |= 0x08; //����PWMB_CCR5Ԥת�ع���(��ҪCC5E=1�ſ�д)
    PWMB_CCMR2 |= 0x08;
    PWMB_CCMR3 |= 0x08;
    PWMB_CCMR4 |= 0x08;

    PWMB_ARRH = (u8)(PWM_PERIOD >> 8); //��������ʱ��
    PWMB_ARRL = (u8)PWM_PERIOD;

    PWMB_ENO = 0x00;
    PWMB_ENO |= ENO5P; //ʹ�����
    PWMB_ENO |= ENO6P; //ʹ�����
    PWMB_ENO |= ENO7P; //ʹ�����
    PWMB_ENO |= ENO8P; //ʹ�����

    PWMB_PS = 0x00;    //�߼� PWM ͨ�������ѡ��λ
    PWMB_PS |= PWM5_0; //ѡ�� PWM5_0 ͨ��
    PWMB_PS |= PWM6_0; //ѡ�� PWM6_0 ͨ��
    PWMB_PS |= PWM7_0; //ѡ�� PWM7_0 ͨ��
    PWMB_PS |= PWM8_0; //ѡ�� PWM8_0 ͨ��

    PWMB_BKR = 0x80;   //ʹ�������
    PWMB_CR1 |= 0x81;  //ʹ��ARRԤװ�أ���ʼ��ʱ

    P40 = 0;	//��LED����
    EA = 1;     //�����ж�

    while (1);
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
    if(!PWM5_Flag)
    {
        PWM5_Duty++;
        if(PWM5_Duty > PWM_PERIOD) PWM5_Flag = 1;
    }
    else
    {
        PWM5_Duty--;
        if(PWM5_Duty <= 0) PWM5_Flag = 0;
    }

    if(!PWM6_Flag)
    {
        PWM6_Duty++;
        if(PWM6_Duty > PWM_PERIOD) PWM6_Flag = 1;
    }
    else
    {
        PWM6_Duty--;
        if(PWM6_Duty <= 0) PWM6_Flag = 0;
    }

    if(!PWM7_Flag)
    {
        PWM7_Duty++;
        if(PWM7_Duty > PWM_PERIOD) PWM7_Flag = 1;
    }
    else
    {
        PWM7_Duty--;
        if(PWM7_Duty <= 0) PWM7_Flag = 0;
    }

    if(!PWM8_Flag)
    {
        PWM8_Duty++;
        if(PWM8_Duty > PWM_PERIOD) PWM8_Flag = 1;
    }
    else
    {
        PWM8_Duty--;
        if(PWM8_Duty <= 0) PWM8_Flag = 0;
    }
    
    UpdatePwm();
}

//========================================================================
// ����: UpdatePwm(void)
// ����: ����PWMռ�ձ�. 
// ����: none.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
    PWMB_CCR5H = (u8)(PWM5_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR5L = (u8)(PWM5_Duty);
    PWMB_CCR6H = (u8)(PWM6_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR6L = (u8)(PWM6_Duty);
    PWMB_CCR7H = (u8)(PWM7_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR7L = (u8)(PWM7_Duty);
    PWMB_CCR8H = (u8)(PWM8_Duty >> 8); //����ռ�ձ�ʱ��
    PWMB_CCR8L = (u8)(PWM8_Duty);
}

