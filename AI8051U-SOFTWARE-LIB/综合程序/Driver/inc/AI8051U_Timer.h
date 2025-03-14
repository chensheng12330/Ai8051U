/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_TIMER_H
#define __AI8051U_TIMER_H

#include "config.h"

//========================================================================
//                             定时器0设置
//========================================================================

/* 0,1: 定时器0用做定时器, 12T/1T; 2: 定时器0用做计数器    */
#define Timer0_Mode_Select(n)   do{if(n == 0) T0x12 = 0, T0_CT = 0; \
                                   if(n == 1) T0x12 = 1, T0_CT = 0; \
                                   if(n == 2) T0_CT = 1; \
                                }while(0)

#define Timer0_CLK_Output(n)    T0CLKO = n                  /* T0 溢出脉冲在T0脚输出使能 */
#define Timer0_Run(n)           (n==0?(TR0 = 0):(TR0 = 1))  /* 定时器0计数使能 */
#define Timer0_Stop()           TR0 = 0                     /* 禁止定时器0计数 */
#define T0_Load(n)              TH0 = (n) / 256, TL0 = (n) % 256

//========================================================================
//                             定时器1设置
//========================================================================

/* 0,1: 定时器1用做定时器, 12T/1T; 2: 定时器1用做计数器    */
#define Timer1_Mode_Select(n)   do{if(n == 0) T1x12 = 0, T1_CT = 0; \
                                   if(n == 1) T1x12 = 1, T1_CT = 0; \
                                   if(n == 2) T1_CT = 1; \
                                }while(0)

#define Timer1_CLK_Output(n)    T1CLKO = n                  /* T1 溢出脉冲在T1脚输出使能 */
#define Timer1_Run(n)           (n==0?(TR1 = 0):(TR1 = 1))  /* 定时器1计数使能 */
#define Timer1_Stop()           TR1 = 0                     /* 禁止定时器1计数 */
#define T1_Load(n)              TH1 = (n) / 256, TL1 = (n) % 256

//========================================================================
//                             定时器2设置
//========================================================================

/* 0,1: 定时器2用做定时器, 12T/1T; 2: 定时器2用做计数器    */
#define Timer2_Mode_Select(n)   do{if(n == 0) T2x12 = 0, T2_CT = 0; \
                                   if(n == 1) T2x12 = 1, T2_CT = 0; \
                                   if(n == 2) T2_CT = 1; \
                                }while(0)

#define Timer2_CLK_Output(n)    T2CLKO = n                  /* T2 溢出脉冲在T2脚输出使能 */
#define Timer2_Run(n)           (n==0?(T2R = 0):(T2R = 1))  /* 定时器2计数使能 */
#define Timer2_Stop()           T2R = 0                     /* 禁止定时器2计数 */
#define T2_Load(n)              T2H = (n) / 256, T2L = (n) % 256

//========================================================================
//                             定时器3设置
//========================================================================

/* 0,1: 定时器3用做定时器, 12T/1T; 2: 定时器3用做计数器    */
#define Timer3_Mode_Select(n)   do{if(n == 0) T3x12 = 0, T3_CT = 0; \
                                   if(n == 1) T3x12 = 1, T3_CT = 0; \
                                   if(n == 2) T3_CT = 1; \
                                }while(0)

#define Timer3_CLK_Output(n)    T3CLKO = n                  /* T3 溢出脉冲在T3脚输出使能 */
#define Timer3_Run(n)           (n==0?(T3R = 0):(T3R = 1))  /* 定时器3计数使能 */
#define Timer3_Stop()           T3R = 0                     /* 禁止定时器3计数 */
#define T3_Load(n)              T3H = (n) / 256, T3L = (n) % 256

//========================================================================
//                             定时器4设置
//========================================================================

/* 0,1: 定时器4用做定时器, 12T/1T; 2: 定时器4用做计数器    */
#define Timer4_Mode_Select(n)   do{if(n == 0) T4x12 = 0, T4_CT = 0; \
                                   if(n == 1) T4x12 = 1, T4_CT = 0; \
                                   if(n == 2) T4_CT = 1; \
                                }while(0)

#define Timer4_CLK_Output(n)    T4CLKO = n                  /* T4 溢出脉冲在T4脚输出使能 */
#define Timer4_Run(n)           (n==0?(T4R = 0):(T4R = 1))  /* 定时器4计数使能 */
#define Timer4_Stop()           T4R = 0                     /* 禁止定时器4计数 */
#define T4_Load(n)              T4H = (n) / 256,    T4L = (n) % 256

//========================================================================
//                             定时器11设置
//========================================================================

/* 0,1: 定时器11用做定时器, 12T/1T; 2: 定时器11用做计数器    */
#define Timer11_Mode_Select(n)  do{if(n == 0) T11CR &= ~0x50; \
                                   if(n == 1) T11CR |= 0x10, T11CR &= ~0x40; \
                                   if(n == 2) T11CR |= 0x50; \
                                }while(0)

#define Timer11_CLK_Select(n)   do{if(n == 0) T11CR &= ~0x0c; \
                                   if(n == 1) T11CR |= 0x04, T11CR &= ~0x08; \
                                   if(n == 2) T11CR |= 0x08, T11CR &= ~0x04; \
                                   if(n == 3) T11CR |= 0x0c; \
                                }while(0)

#define Timer11_CLK_Output(n)   (n==0?(T11CR &= ~0x20):(T11CR |= 0x20)) /* T11 溢出脉冲在T11脚输出使能 */
#define Timer11_Run(n)          (n==0?(T11CR &= ~0x80):(T11CR |= 0x80)) /* 定时器1计数使能 */
#define Timer11_Stop()          T11CR &= ~0x80                          /* 禁止定时器4计数 */
#define T11_Load(n)             T11H = (n) / 256, T11L = (n) % 256

//========================================================================
//                              定义声明
//========================================================================

#define Timer0                        0
#define Timer1                        1
#define Timer2                        2
#define Timer3                        3
#define Timer4                        4
#define Timer11                       11

#define TIM_16BitAutoReload           0
#define TIM_16Bit                     1
#define TIM_8BitAutoReload            2
#define TIM_16BitAutoReloadNoMask     3
#define TIM_T1Stop                    3

#define TIM_CLOCK_12T                 0
#define TIM_CLOCK_1T                  1
#define TIM_CLOCK_Ext                 2

#define TIM_SOURCE_SYSCLK             0
#define TIM_SOURCE_HIRC               1
#define TIM_SOURCE_X32K               2
#define TIM_SOURCE_LIRC               3

typedef struct
{
    u8    TIM_Mode;       //工作模式,      TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    u8    TIM_ClkMode;    //模式选择       TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    u8    TIM_ClkSource;  //时钟源         TIM_SOURCE_SYSCLK,TIM_SOURCE_HIRC,TIM_SOURCE_X32K,TIM_SOURCE_LIRC
    u8    TIM_ClkOut;     //定时器时钟输出, ENABLE,DISABLE
    u16   TIM_Value;      //装载初值
    u8    TIM_PS;         //8位预分频器
    u8    TIM_Run;        //是否运行        ENABLE,DISABLE
} TIM_InitTypeDef;

u8 Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx);

#endif
