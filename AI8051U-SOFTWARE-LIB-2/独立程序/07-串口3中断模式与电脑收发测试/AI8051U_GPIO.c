/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_GPIO.h"

//========================================================================
// 函数: u8 GPIO_Inilize(u8 GPIO, GPIO_InitTypeDef *GPIOx)
// 描述: 初始化IO口.
// 参数: GPIOx: 结构参数,请参考头文件里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 GPIO_Inilize(u8 GPIO, GPIO_InitTypeDef *GPIOx)
{
    if(GPIO > GPIO_P7)               return FAIL;    //错误
    if(GPIOx->Mode > GPIO_OUT_PP)    return FAIL;    //错误
    if(GPIO == GPIO_P0)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P0M1 &= ~GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P0M1 |=  GPIOx->Pin,    P0M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P0M1 |=  GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P0M1 &= ~GPIOx->Pin,    P0M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P1)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P1M1 &= ~GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P1M1 |=  GPIOx->Pin,    P1M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P1M1 |=  GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P1M1 &= ~GPIOx->Pin,    P1M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P2)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P2M1 &= ~GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P2M1 |=  GPIOx->Pin,    P2M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P2M1 |=  GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P2M1 &= ~GPIOx->Pin,    P2M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P3)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P3M1 &= ~GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P3M1 |=  GPIOx->Pin,    P3M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P3M1 |=  GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P3M1 &= ~GPIOx->Pin,    P3M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P4)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P4M1 &= ~GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P4M1 |=  GPIOx->Pin,    P4M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P4M1 |=  GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P4M1 &= ~GPIOx->Pin,    P4M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P5)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P5M1 &= ~GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P5M1 |=  GPIOx->Pin,    P5M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P5M1 |=  GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P5M1 &= ~GPIOx->Pin,    P5M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P6)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P6M1 &= ~GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P6M1 |=  GPIOx->Pin,    P6M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P6M1 |=  GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P6M1 &= ~GPIOx->Pin,    P6M0 |=  GPIOx->Pin;     //推挽输出
    }
    if(GPIO == GPIO_P7)
    {
        if(GPIOx->Mode == GPIO_PullUp)        P7M1 &= ~GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //上拉准双向口
        if(GPIOx->Mode == GPIO_HighZ)         P7M1 |=  GPIOx->Pin,    P7M0 &= ~GPIOx->Pin;     //浮空输入
        if(GPIOx->Mode == GPIO_OUT_OD)        P7M1 |=  GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //开漏输出
        if(GPIOx->Mode == GPIO_OUT_PP)        P7M1 &= ~GPIOx->Pin,    P7M0 |=  GPIOx->Pin;     //推挽输出
    }
    return SUCCESS;    //成功
}

//========================================================================
// 函数: u8 GPIO_INT_Inilize(u8 GPIO, GPIO_Int_InitTypeDef *GPIOx)
// 描述: 初始化IO口中断.
// 参数: GPIOx: 结构参数,请参考头文件里的定义.
// 返回: 成功返回 SUCCESS, 错误返回 FAIL.
// 版本: V1.0, 2025-02-11
//========================================================================
u8 GPIO_INT_Inilize(u8 GPIO, GPIO_Int_InitTypeDef *GPIOx)
{
    if(GPIO > GPIO_P7)                   return FAIL;    //错误
    if(GPIOx->Mode > GPIO_HIGH_LEVEL)    return FAIL;    //错误

    if(GPIO == GPIO_P0)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P0IM1 &= ~GPIOx->Pin,    P0IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P0IM1 &= ~GPIOx->Pin,    P0IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P0IM1 |=  GPIOx->Pin,    P0IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P0IM1 |=  GPIOx->Pin,    P0IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P0WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P0WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P0INTE |=  GPIOx->Pin;     //中断使能
        else                                    P0INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P1)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P1IM1 &= ~GPIOx->Pin,    P1IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P1IM1 &= ~GPIOx->Pin,    P1IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P1IM1 |=  GPIOx->Pin,    P1IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P1IM1 |=  GPIOx->Pin,    P1IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P1WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P1WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P1INTE |=  GPIOx->Pin;     //中断使能
        else                                    P1INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P2)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P2IM1 &= ~GPIOx->Pin,    P2IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P2IM1 &= ~GPIOx->Pin,    P2IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P2IM1 |=  GPIOx->Pin,    P2IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P2IM1 |=  GPIOx->Pin,    P2IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P2WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P2WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P2INTE |=  GPIOx->Pin;     //中断使能
        else                                    P2INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P3)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P3IM1 &= ~GPIOx->Pin,    P3IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P3IM1 &= ~GPIOx->Pin,    P3IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P3IM1 |=  GPIOx->Pin,    P3IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P3IM1 |=  GPIOx->Pin,    P3IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P3WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P3WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P3INTE |=  GPIOx->Pin;     //中断使能
        else                                    P3INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P4)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P4IM1 &= ~GPIOx->Pin,    P4IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P4IM1 &= ~GPIOx->Pin,    P4IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P4IM1 |=  GPIOx->Pin,    P4IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P4IM1 |=  GPIOx->Pin,    P4IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P4WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P4WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P4INTE |=  GPIOx->Pin;     //中断使能
        else                                    P4INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P5)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P5IM1 &= ~GPIOx->Pin,    P5IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P5IM1 &= ~GPIOx->Pin,    P5IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P5IM1 |=  GPIOx->Pin,    P5IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P5IM1 |=  GPIOx->Pin,    P5IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P5WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P5WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P5INTE |=  GPIOx->Pin;     //中断使能
        else                                    P5INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P6)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P6IM1 &= ~GPIOx->Pin,    P6IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P6IM1 &= ~GPIOx->Pin,    P6IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P6IM1 |=  GPIOx->Pin,    P6IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P6IM1 |=  GPIOx->Pin,    P6IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P6WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P6WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P6INTE |=  GPIOx->Pin;     //中断使能
        else                                    P6INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIO == GPIO_P7)
    {
        if(GPIOx->Mode == GPIO_FALLING_EDGE)    P7IM1 &= ~GPIOx->Pin,    P7IM0 &= ~GPIOx->Pin;     //下降沿中断
        if(GPIOx->Mode == GPIO_RISING_EDGE)     P7IM1 &= ~GPIOx->Pin,    P7IM0 |=  GPIOx->Pin;     //上升沿中断
        if(GPIOx->Mode == GPIO_LOW_LEVEL)       P7IM1 |=  GPIOx->Pin,    P7IM0 &= ~GPIOx->Pin;     //低电平中断
        if(GPIOx->Mode == GPIO_HIGH_LEVEL)      P7IM1 |=  GPIOx->Pin,    P7IM0 |=  GPIOx->Pin;     //高电平中断

        if(GPIOx->WakeUpEn == ENABLE)           P7WKUE |=  GPIOx->Pin;     //掉电唤醒使能
        else                                    P7WKUE &=  ~GPIOx->Pin;    //掉电唤醒禁能

        if(GPIOx->IntEnable == ENABLE)          P7INTE |=  GPIOx->Pin;     //中断使能
        else                                    P7INTE &=  ~GPIOx->Pin;    //中断禁能
    }

    if(GPIOx->Priority == Priority_0)        PINIPH &= ~(1<<GPIO),    PINIPL &= ~(1<<GPIO);     //中断优先级为0
    if(GPIOx->Priority == Priority_1)        PINIPH &= ~(1<<GPIO),    PINIPL |=  (1<<GPIO);     //中断优先级为1
    if(GPIOx->Priority == Priority_2)        PINIPH |=  (1<<GPIO),    PINIPL &= ~(1<<GPIO);     //中断优先级为2
    if(GPIOx->Priority == Priority_3)        PINIPH |=  (1<<GPIO),    PINIPL |=  (1<<GPIO);     //中断优先级为3

    return SUCCESS;    //成功
}
