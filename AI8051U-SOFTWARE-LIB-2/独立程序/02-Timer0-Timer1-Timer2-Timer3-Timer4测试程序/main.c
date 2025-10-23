/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_Timer.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"

/*************    ����˵��    **************

������ʾ6����ʱ����ʹ��, ��ʹ��16λ�Զ���װ.

��ʱ��0��16λ�Զ���װ, �ж�Ƶ��Ϊ100000Hz���жϺ�����P0.7ȡ�����50KHz�����ź�.

��ʱ��1��16λ�Զ���װ, �ж�Ƶ��Ϊ20000Hz���жϺ�����P0.6ȡ�����10KHz�����ź�.

��ʱ��2��16λ�Զ���װ, �ж�Ƶ��Ϊ10000Hz���жϺ�����P0.5ȡ�����5KHz�����ź�.

��ʱ��3��16λ�Զ���װ, �ж�Ƶ��Ϊ1000Hz���жϺ�����P0.4ȡ�����500Hz�����ź�.

��ʱ��4��16λ�Զ���װ, �ж�Ƶ��Ϊ500Hz���жϺ�����P0.3ȡ�����250Hz�����ź�.

��ʱ��11��16λ�Զ���װ, �ж�Ƶ��Ϊ(500/10)Hz���жϺ�����P0.2ȡ�����25Hz�����ź�.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/


/*************    ���غ�������    **************/



/*************  �ⲿ�����ͱ������� *****************/



/************************ IO������ ****************************/
void GPIO_config(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;     //�ṹ����

    GPIO_InitStructure.Pin  = GPIO_Pin_All;     //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);  //��ʼ��

    GPIO_InitStructure.Pin  = GPIO_Pin_0;       //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);  //��ʼ��
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef        TIM_InitStructure;               //�ṹ����

    //��ʱ��0��16λ�Զ���װ, �ж�Ƶ��Ϊ100000Hz.
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 100000UL));  //�ж�Ƶ��, 100000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //��ʼ��Timer0      Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer0_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    //��ʱ��1��16λ�Զ���װ, �ж�Ƶ��Ϊ20000Hz.
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��Դ, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     =  (u16)(65536UL - (MAIN_Fosc / 20000UL));  //�ж�Ƶ��, 20000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer1,&TIM_InitStructure);               //��ʼ��Timer1      Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer1_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    //��ʱ��2��16λ�Զ���װ, �ж�Ƶ��Ϊ10000Hz.
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 10000UL));   //�ж�Ƶ��, 10000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer2,&TIM_InitStructure);               //��ʼ��Timer2      Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer2_Init(ENABLE,NULL);        //�ж�ʹ��, ENABLE/DISABLE; �����ȼ�

    //��ʱ��3��16λ�Զ���װ, �ж�Ƶ��Ϊ1000Hz.
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_12T;        //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / (1000*12))); //�ж�Ƶ��, 1000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer3,&TIM_InitStructure);               //��ʼ��Timer3      Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer3_Init(ENABLE,NULL);        //�ж�ʹ��, ENABLE/DISABLE; �����ȼ�

    //��ʱ��4��16λ�Զ���װ, �ж�Ƶ��Ϊ500Hz.
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_12T;        //ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������������, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / (500*12))); //�ж�Ƶ��, 500��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer4,&TIM_InitStructure);               //��ʼ��Timer4      Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer4_Init(ENABLE,NULL);        //�ж�ʹ��, ENABLE/DISABLE; �����ȼ�

    //��ʱ��11��16λ�Զ���װ, �ж�Ƶ��Ϊ500Hz/(9+1).
    TIM_InitStructure.TIM_ClkSource = TIM_SOURCE_SYSCLK;    //ָ��ʱ��Դ, TIM_SOURCE_SYSCLK,TIM_SOURCE_HIRC,TIM_SOURCE_X32K,TIM_SOURCE_LIRC
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_12T;        //ָ��ʱ��ģʽ,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������ʱ��ʱ��, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / (500*12))); //�ж�Ƶ��, 500��/��
    TIM_InitStructure.TIM_PS        = 9;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer11,&TIM_InitStructure);              //��ʼ��Timer11, Timer0,Timer1,Timer2,Timer3,Timer4,Timer11
    NVIC_Timer11_Init(ENABLE,NULL);        //�ж�ʹ��, ENABLE/DISABLE; �����ȼ�
}

/******************** ������**************************/
void main(void)
{
    WTST = 0;        //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();        //��չSFR(XFR)����ʹ�� 
    CKCON = 0;      //��߷���XRAM�ٶ�

    GPIO_config();
    Timer_config();
    EA = 1;
    P40 = 0;        //��ʵ����LED��Դ

    while (1);
}
