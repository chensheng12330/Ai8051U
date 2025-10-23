/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************	����˵��	**************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

������ʾоƬPCA���������Ч��PWM�ź�.

PCA0��P4.2�����8λPWM�źţ���ͨ��ʵ����LED11�鿴Ч��.

PCA1��P4.3�����10λPWM�źţ���ͨ��ʵ����LED10�鿴Ч��.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

#define     MAIN_Fosc       24000000UL  //������ʱ��
#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 12 / 400))       //Timer 0 �ж�Ƶ��, 400��/��

#define	PCA0			0
#define	PCA1			1
#define	PCA2			2

bit	B_PWM0_Dir;	//����, 0Ϊ+, 1Ϊ-.
bit	B_PWM1_Dir;	//����, 0Ϊ+, 1Ϊ-.
u16	pwm0;
u16	pwm1;

void Timer0_config(void);
void PWM_config(void);
void UpdatePwm(u8 PCA_id, u16 pwm_value);

void main()
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

    Timer0_config();
    PWM_config();
    EA = 1;      //�������ж�

    //��ʼ������
    pwm0 = 128;
    B_PWM0_Dir = 0;
    UpdatePwm(PCA0,pwm0);

    pwm1 = 128;
    B_PWM1_Dir = 0;
    UpdatePwm(PCA1,pwm1);

    while (1)
    {
    }
}

//========================================================================
// ����: void Timer0_config(void)
// ����: Timer0��ʼ��������
// ����: ��.
// ����: ��.
// �汾: V1.0, 2020-6-10
//========================================================================
void Timer0_config(void)
{
    AUXR &= 0x7F;   //Timer0 set as 12T, 16 bits timer auto-reload
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;        //Timer0 interrupt enable
    TR0 = 1;        //Tiner0 run
}

//========================================================================
// ����: void PWM_config(void)
// ����: PWM��ʼ��������
// ����: ��.
// ����: ��.
// �汾: V1.0, 2020-6-10
//========================================================================
void PWM_config(void)
{
    //bit[6:5]=0: ECI/P1.2, CCP0/P1.3, CCP1/P1.4, CCP2/P1.1
    //bit[6:5]=1: ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    //bit[6:5]=2: ECI/P2.3, CCP0/P2.0, CCP1/P2.1, CCP2/P2.2
    CMOD = (CMOD & 0x9f) | (1<<5);

    CCON = 0x00;
    CMOD |= 0x0e;   //PCA ʱ��Ϊϵͳʱ��/8
    CL = 0x00;
    CH = 0x00;

    CCAPM0 = 0x42;  //PCA ģ�� 0 Ϊ PWM ����ģʽ
    PCA_PWM0 = 0x00;//PCA ģ�� 0 ��� 8 λ PWM
    CCAP0L = 0x80;
    CCAP0H = 0x80;  //���� PWM ��ʼռ�ձ�

    CCAPM1 = 0x42;  //PCA ģ�� 1 Ϊ PWM ����ģʽ
    PCA_PWM1 = 0xc0;//PCA ģ�� 1 ��� 10 λ PWM
    CCAP1L = 0x40;
    CCAP1H = 0x40;  //���� PWM ��ʼռ�ձ�

    CCAPM2 = 0x42;  //PCA ģ�� 2 Ϊ PWM ����ģʽ
    PCA_PWM2 = 0x80;//PCA ģ�� 2 ��� 6 λ PWM
    CCAP2L = 0x20;
    CCAP2H = 0x20;  //���� PWM ��ʼռ�ձ�

    CCON |= 0x40;   //���� PCA ��ʱ��
}

//========================================================================
// ����: UpdatePwm(u8 PCA_id, u16 pwm_value)
// ����: ����PWMֵ. 
// ����: PCA_id: PCA���. ȡֵ PCA0,PCA1,PCA2,PCA_Counter
//		 pwm_value: pwmֵ, ���ֵ������͵�ƽ��ʱ��.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(u8 PCA_id, u16 pwm_value)
{
    if(PCA_id == PCA0)
    {
        PCA_PWM0 = (PCA_PWM0 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP0H = (u8)pwm_value;
    }
    else if(PCA_id == PCA1)
    {
        PCA_PWM1 = (PCA_PWM1 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP1H = (u8)pwm_value;
    }
    else if(PCA_id == PCA2)
    {
        PCA_PWM2 = (PCA_PWM2 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP2H = (u8)pwm_value;
    }
}

/********************** Timer0 �жϺ��� ************************/
void timer0(void) interrupt 1
{
    if(B_PWM0_Dir)
    {
        if(--pwm0 <= 1)	B_PWM0_Dir = 0;
    }
    else if(++pwm0 >= 255)	B_PWM0_Dir = 1;

    UpdatePwm(PCA0,pwm0);   //���� PCA0 ռ�ձ�

    if(B_PWM1_Dir)
    {
        if(--pwm1 <= 1)	B_PWM1_Dir = 0;
    }
    else if(++pwm1 >= 1023)	B_PWM1_Dir = 1;

    UpdatePwm(PCA1,pwm1);   //���� PCA1 ռ�ձ�
}
