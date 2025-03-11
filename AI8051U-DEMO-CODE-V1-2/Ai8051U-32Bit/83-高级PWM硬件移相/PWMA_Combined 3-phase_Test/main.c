/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

��������� PWM ģʽ�£�������һ���������Ķ��� PWM �ź���һ���ɱ���źż������������м�����߼������㡣
OC5REF �ź����ڶ������������źš�
ƾ�� PWMA_CCR5 �е� 3 λ GC5C[3:1]������ѡ�� OC5REF ���ĸ��ο��ź���ϡ�
�������ź� OCxREFC �������ο� PWM ���߼������������ɡ�
�C ��� GC5C1 �� 1���� OC1REFC �� PWMA_CCR1 �� PWMA_CCR5 ����
�C ��� GC5C2 �� 1���� OC2REFC �� PWMA_CCR2 �� PWMA_CCR5 ����
�C ��� GC5C3 �� 1���� OC3REFC �� PWMA_CCR3 �� PWMA_CCR5 ����
ͨ�� 1 ��ͨ�� 3 �ɶ���ѡ��������� PWM ģʽ��ֻ�轫 3 λ GC5C[3:1] �е�����һλ�� 1��

******************************************/

#include "AI8051U.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** �û������ ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define PWM1_0      0x00	//P:P1.0  N:P1.1
#define PWM1_1      0x01	//P:P2.0  N:P2.1
#define PWM1_2      0x02	//P:P6.0  N:P6.1

#define PWM2_0      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_1      0x04	//P:P2.2  N:P2.3
#define PWM2_2      0x08	//P:P6.2  N:P6.3

#define PWM3_0      0x00	//P:P1.4  N:P1.5
#define PWM3_1      0x10	//P:P2.4  N:P2.5
#define PWM3_2      0x20	//P:P6.4  N:P6.5

#define PWM4_0      0x00	//P:P1.6  N:P1.7
#define PWM4_1      0x40	//P:P2.6  N:P2.7
#define PWM4_2      0x80	//P:P6.6  N:P6.7
#define PWM4_3      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

/*****************************************************************************/


/*************  ���س�������    **************/

#define PWM_PERIOD  0x1000    //��������ֵ

/*************  ���ر�������    **************/

u16 PWM1_Duty;
u16 PWM2_Duty;
u16 PWM3_Duty;
u16 PWM4_Duty;

void UpdatePwm(void);

/********************* ������ *************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0xff;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

    PWM1_Duty = 0x07ff;
    PWM2_Duty = 0x05ff;
    PWM3_Duty = 0x03ff;
    PWM4_Duty = 0;

    PWMA_CCER1 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCER2 = 0x00;

    PWMA_CCMR1X = 0x00; //ͨ��1:PWMģʽ1
    PWMA_CCMR1 = 0x60;
    PWMA_CCMR2X = 0x00; //ͨ��2:PWMģʽ1
    PWMA_CCMR2 = 0x60;
    PWMA_CCMR3X = 0x00; //ͨ��3:PWMģʽ1
    PWMA_CCMR3 = 0x60;
    PWMA_CCMR4X = 0x00; //ͨ��4:����
    PWMA_CCMR4 = 0x00;
    PWMA_CCMR5 = 0x70;  //ͨ��1��2��3�ֱ������ͨ��5���߼���������

    PWMA_CCR5X = 0xe0;
    PWMA_CCR5H = (u8)(0xfe >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR5L = (u8)(0xfe);

    PWMA_OISR = 0x15;

    PWMA_CCER1 = 0x11; //����ͨ�����ʹ�ܺͼ���
    PWMA_CCER2 = 0x01;

    PWMA_ARRH = (u8)(PWM_PERIOD >> 8); //��������ʱ��
    PWMA_ARRL = (u8)PWM_PERIOD;

    PWMA_ENO = 0x00;
    PWMA_ENO |= ENO1P; //ʹ�����
    PWMA_ENO |= ENO1N; //ʹ�����
    PWMA_ENO |= ENO2P; //ʹ�����
    PWMA_ENO |= ENO2N; //ʹ�����
    PWMA_ENO |= ENO3P; //ʹ�����
    PWMA_ENO |= ENO3N; //ʹ�����
    PWMA_ENO |= ENO4P; //ʹ�����
    PWMA_ENO |= ENO4N; //ʹ�����

    PWMA_PS = 0x00;  //�߼� PWM ͨ�������ѡ��λ
    PWMA_PS |= PWM1_0; //ѡ�� PWM1_0 ͨ��
    PWMA_PS |= PWM2_0; //ѡ�� PWM2_0 ͨ��
    PWMA_PS |= PWM3_0; //ѡ�� PWM3_0 ͨ��
    PWMA_PS |= PWM4_0; //ѡ�� PWM4_0 ͨ��
    
    UpdatePwm();

    PWMA_BKR = 0x80;  //ʹ�������
    PWMA_CR1 |= 0x31; //���Ķ���ģʽ1�����¼�������ʼ��ʱ

//    EA = 1;     //�����ж�

    while (1);
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
    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR1L = (u8)(PWM1_Duty);
    PWMA_CCR2H = (u8)(PWM2_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR2L = (u8)(PWM2_Duty);
    PWMA_CCR3H = (u8)(PWM3_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR3L = (u8)(PWM3_Duty);
    PWMA_CCR4H = (u8)(PWM4_Duty >> 8); //����ռ�ձ�ʱ��
    PWMA_CCR4L = (u8)(PWM4_Duty);
}
