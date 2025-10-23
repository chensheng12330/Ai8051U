/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_PWM.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Timer.h"

/*************    ����˵��    **************

�߼�PWM��ʱ�� PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N ÿ��ͨ�����ɶ���ʵ��PWM������������������Գ����.

8��ͨ��PWM���ö�ӦP0��8���˿�.

ͨ��P0�������ӵ�8��LED�ƣ�����PWMʵ�ֺ�����Ч��.

PWM���ں�ռ�ձȿ��Ը�����Ҫ�������ã���߿ɴ�65535.

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/

PWMx_Duty PWMA_Duty;
bit PWM1_Flag;
bit PWM2_Flag;
bit PWM3_Flag;
bit PWM4_Flag;

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
    
    PWMA_Duty.PWM1_Duty = 128;
    PWMA_Duty.PWM2_Duty = 256;
    PWMA_Duty.PWM3_Duty = 512;
    PWMA_Duty.PWM4_Duty = 1024;

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ, CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMA_Duty.PWM1_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO1P | ENO1N;       //���ͨ��ѡ��,  ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM1, &PWMx_InitStructure);           //��ʼ��PWM1

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ, CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMA_Duty.PWM2_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO2P | ENO2N;       //���ͨ��ѡ��,  ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM2, &PWMx_InitStructure);           //��ʼ��PWM2

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ, CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMA_Duty.PWM3_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO3P | ENO3N;       //���ͨ��ѡ��,  ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM3, &PWMx_InitStructure);           //��ʼ��PWM3

    PWMx_InitStructure.PWM_Mode    = CCMRn_PWM_MODE1;       //ģʽ, CCMRn_FREEZE,CCMRn_MATCH_VALID,CCMRn_MATCH_INVALID,CCMRn_ROLLOVER,CCMRn_FORCE_INVALID,CCMRn_FORCE_VALID,CCMRn_PWM_MODE1,CCMRn_PWM_MODE2
    PWMx_InitStructure.PWM_Duty    = PWMA_Duty.PWM4_Duty;   //PWMռ�ձ�ʱ��, 0~Period
    PWMx_InitStructure.PWM_EnoSelect = ENO4P | ENO4N;       //���ͨ��ѡ��,  ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWM_Configuration(PWM4, &PWMx_InitStructure);           //��ʼ��PWM4

    PWMx_InitStructure.PWM_Period   = 2047;                 //����ʱ��,   0~65535
    PWMx_InitStructure.PWM_DeadTime = 0;                    //��������������, 0~255
    PWMx_InitStructure.PWM_MainOutEnable= ENABLE;           //�����ʹ��, ENABLE,DISABLE
    PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;           //ʹ�ܼ�����, ENABLE,DISABLE
    PWM_Configuration(PWMA, &PWMx_InitStructure);           //��ʼ��PWMͨ�üĴ���,  PWMA,PWMB
    
//    PWMA_CC1P_HighValid();
//    PWMA_CC1NP_HighValid();
    PWMA_CC1P_LowValid();
    PWMA_CC1NP_LowValid();

    PWM1_USE_P00P01();
    PWM2_USE_P02P03();
    PWM3_USE_P04P05();
    PWM4_USE_P06P07();

    NVIC_PWM_Init(PWMA,DISABLE,Priority_0);
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
            
            if(!PWM1_Flag)
            {
                PWMA_Duty.PWM1_Duty++;
                if(PWMA_Duty.PWM1_Duty >= 2047) PWM1_Flag = 1;
            }
            else
            {
                PWMA_Duty.PWM1_Duty--;
                if(PWMA_Duty.PWM1_Duty <= 0) PWM1_Flag = 0;
            }

            if(!PWM2_Flag)
            {
                PWMA_Duty.PWM2_Duty++;
                if(PWMA_Duty.PWM2_Duty >= 2047) PWM2_Flag = 1;
            }
            else
            {
                PWMA_Duty.PWM2_Duty--;
                if(PWMA_Duty.PWM2_Duty <= 0) PWM2_Flag = 0;
            }

            if(!PWM3_Flag)
            {
                PWMA_Duty.PWM3_Duty++;
                if(PWMA_Duty.PWM3_Duty >= 2047) PWM3_Flag = 1;
            }
            else
            {
                PWMA_Duty.PWM3_Duty--;
                if(PWMA_Duty.PWM3_Duty <= 0) PWM3_Flag = 0;
            }

            if(!PWM4_Flag)
            {
                PWMA_Duty.PWM4_Duty++;
                if(PWMA_Duty.PWM4_Duty >= 2047) PWM4_Flag = 1;
            }
            else
            {
                PWMA_Duty.PWM4_Duty--;
                if(PWMA_Duty.PWM4_Duty <= 0) PWM4_Flag = 0;
            }
            
            UpdatePwm(PWMA, &PWMA_Duty);
        }
    }
}
