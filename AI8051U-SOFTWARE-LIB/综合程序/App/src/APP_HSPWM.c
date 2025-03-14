/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_HSPWM.h"
#include "AI8051U_Clock.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_PWM.h"
#include "AI8051U_NVIC.h"

/*************    功能说明    **************

高速高级PWM定时器 PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N 每个通道都可独立实现PWM输出，或者两两互补对称输出.

8个通道PWM设置对应P0的8个端口.

高级PWM定时器 PWM5,PWM6,PWM7,PWM8 每个通道都可独立实现PWM输出.

通过P0口上连接的8个LED灯，利用PWM实现呼吸灯效果.

PWM周期和占空比可以根据需要自行设置，最高可达65535.

下载时, 选择时钟 24MHz (可以在配置文件"config.h"中修改).

******************************************/

//========================================================================
//                               本地常量声明    
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================


//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================

extern PWMx_Duty PWMA_Duty;
extern bit PWM1_Flag;
extern bit PWM2_Flag;
extern bit PWM3_Flag;
extern bit PWM4_Flag;

extern PWMx_Duty PWMB_Duty;
extern bit PWM5_Flag;
extern bit PWM6_Flag;
extern bit PWM7_Flag;
extern bit PWM8_Flag;

//========================================================================
// 函数: HSPWM_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void HSPWM_init(void)
{
    HSPWMx_InitDefine PWMx_InitStructure;

    PWMA_Duty.PWM1_Duty = 128;
    PWMA_Duty.PWM2_Duty = 256;
    PWMA_Duty.PWM3_Duty = 512;
    PWMA_Duty.PWM4_Duty = 1024;

    PWMB_Duty.PWM5_Duty = 128;
    PWMB_Duty.PWM6_Duty = 256;
    PWMB_Duty.PWM7_Duty = 512;
    PWMB_Duty.PWM8_Duty = 1024;

    HSPllClkConfig(MCKSEL_HIRC,PLL_96M,0);    //系统时钟选择,PLL时钟选择,时钟分频系数

    PWMx_InitStructure.PWM_EnoSelect= ENO1P|ENO2P|ENO3P|ENO4P;  //输出通道选择,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    PWMx_InitStructure.PWM_Period   = 2047;                     //周期时间,   0~65535
    PWMx_InitStructure.PWM_DeadTime = 0;                        //死区发生器设置, 0~255
    PWMx_InitStructure.PWM_MainOutEnable= ENABLE;               //主输出使能, ENABLE,DISABLE
    PWMx_InitStructure.PWM_CEN_Enable   = ENABLE;               //使能计数器, ENABLE,DISABLE
    HSPWM_Configuration(PWMA, &PWMx_InitStructure, &PWMA_Duty); //初始化PWM通用寄存器,  PWMA,PWMB
    PWMx_InitStructure.PWM_EnoSelect= ENO5P|ENO6P|ENO7P|ENO8P;  //输出通道选择,    ENO1P,ENO1N,ENO2P,ENO2N,ENO3P,ENO3N,ENO4P,ENO4N / ENO5P,ENO6P,ENO7P,ENO8P
    HSPWM_Configuration(PWMB, &PWMx_InitStructure, &PWMB_Duty); //初始化PWM通用寄存器,  PWMA,PWMB

    PWM1_USE_P00P01();
    PWM2_USE_P02P03();
    PWM3_USE_P04P05();
    PWM4_USE_P06P07();

    PWM5_USE_P01();
    PWM6_USE_P03();
    PWM7_USE_P05();
    PWM8_USE_P07();
    
    P0_MODE_OUT_PP(GPIO_Pin_All);   //P0 设置为推挽输出
    P4_MODE_IO_PU(GPIO_Pin_0);      //P4.0 设置为准双向口
    NVIC_PWM_Init(PWMA,DISABLE,Priority_0);
    NVIC_PWM_Init(PWMB,DISABLE,Priority_0);
    P40 = 0;    //打开实验箱LED电源
}

//========================================================================
// 函数: Sample_HSSPI
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void Sample_HSPWM(void)
{
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
    
    UpdateHSPwm(PWMA, &PWMA_Duty);
    UpdateHSPwm(PWMB, &PWMB_Duty);
}
