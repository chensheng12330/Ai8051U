/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_PWM.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Timer.h"

/*************    ����˵��    **************

�߼�PWM��ʱ�� PWM5,PWM6,PWM7,PWM8 ÿ��ͨ�����ɶ���ʵ��PWM���.

4��ͨ��PWM������Ҫ���ö�Ӧ�����.

ͨ��P0�������ӵ�LED�ƣ�����PWMʵ�ֺ�����Ч��.

PWM���ں�ռ�ձȿ����Զ������ã���߿ɴ�65535.

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/

PWMx_Duty PWMB_Duty;
bit PWM5_Flag;
bit PWM6_Flag;
bit PWM7_Flag;
bit PWM8_Flag;

/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/

extern bit T0_1ms;

/************************ IO������ ****************************/
void GPIO_config(void)
{
    P0_MODE_IO_PU(GPIO_Pin_All);    //P0 ����Ϊ׼˫���
    P4_MODE_IO_PU(GPIO_Pin_0);      //P4.0 ����Ϊ׼˫���
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;  //�ṹ����
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��ģʽ,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������ʱ��ʱ��, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 1000UL));    //�ж�Ƶ��, 1000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //��ʼ��Timer0, Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/***************  PWM��ʼ������ *****************/
void PWM_config(void)
{
    PWMx_InitDefine PWMx_InitStructure;
    
    PWMB_Duty.PWM5_Duty = 128;
    PWMB_Duty.PWM6_Duty = 256;
    PWMB_Duty.PWM7_Duty = 512;
    PWMB_Duty.PWM8_Duty = 1024;

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ,   CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMB_Duty.PWM5_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO5P;               //���ͨ��ѡ��,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM5, &PWMx_InitStructure);           //��ʼ��PWM,  PWMA,PWMB

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ,  CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMB_Duty.PWM6_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO6P;               //���ͨ��ѡ��,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM6, &PWMx_InitStructure);           //��ʼ��PWM,  PWMA,PWMB

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ,CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMB_Duty.PWM7_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO7P;               //���ͨ��ѡ��,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM7, &PWMx_InitStructure);           //��ʼ��PWM,  PWMA,PWMB

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ,CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMB_Duty.PWM8_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO8P;               //���ͨ��ѡ��,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM8, &PWMx_InitStructure);           //��ʼ��PWM,  PWMA,PWMB

    PWMx_InitStructure.PWM_Period   = 2047;                 //����ʱ��,   0~65535
    PWMx_InitStructure.PWM_DeadTime = 0;                    //��������������, 0~255
    PWMx_InitStructure.PWM_MainOutEnable= ENABLE;           //�����ʹ��, ENABLE,DISABLE
    PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;           //ʹ�ܼ�����, ENABLE,DISABLE
    PWM_Configuration(PWMB, &PWMx_InitStructure);           //��ʼ��PWMͨ�üĴ���,  PWMA,PWMB

    PWM5_USE_P01();
    PWM6_USE_P03();
    PWM7_USE_P05();
    PWM8_USE_P07();

    NVIC_PWM_Init(PWMB,DISABLE,Priority_0);
}

/******************** ������**************************/
void main(void)
{
    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

    GPIO_config();
    Timer_config();
    PWM_config();
    EA = 1;
    
    P40 = 0;    //��LED��Դ

    while (1)
    {
        if(T0_1ms)
        {
            T0_1ms = 0;
            
            if(!PWM5_Flag)
            {
                PWMB_Duty.PWM5_Duty++;
                if(PWMB_Duty.PWM5_Duty >= 2047) PWM5_Flag = 1;
            }
            else
            {
                PWMB_Duty.PWM5_Duty--;
                if(PWMB_Duty.PWM5_Duty <= 0) PWM5_Flag = 0;
            }

            if(!PWM6_Flag)
            {
                PWMB_Duty.PWM6_Duty++;
                if(PWMB_Duty.PWM6_Duty >= 2047) PWM6_Flag = 1;
            }
            else
            {
                PWMB_Duty.PWM6_Duty--;
                if(PWMB_Duty.PWM6_Duty <= 0) PWM6_Flag = 0;
            }

            if(!PWM7_Flag)
            {
                PWMB_Duty.PWM7_Duty++;
                if(PWMB_Duty.PWM7_Duty >= 2047) PWM7_Flag = 1;
            }
            else
            {
                PWMB_Duty.PWM7_Duty--;
                if(PWMB_Duty.PWM7_Duty <= 0) PWM7_Flag = 0;
            }

            if(!PWM8_Flag)
            {
                PWMB_Duty.PWM8_Duty++;
                if(PWMB_Duty.PWM8_Duty >= 2047) PWM8_Flag = 1;
            }
            else
            {
                PWMB_Duty.PWM8_Duty--;
                if(PWMB_Duty.PWM8_Duty <= 0) PWM8_Flag = 0;
            }
            
            UpdatePwm(PWMB, &PWMB_Duty);
        }
    }
}
