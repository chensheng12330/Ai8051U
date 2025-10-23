/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "adc.h"
#include "app_rtc.h"
#include "app_adcKey.h"

/*************** 功能说明 ****************

ADC按键键码为1~16.

按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测adcKeyCode是否非0来判断键是否按下.

调整时间键:
键码1: 小时+.
键码2: 小时-.
键码3: 分钟+.
键码4: 分钟-.

******************************************/


//========================================================================
//                               本地常量声明    
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================

u8  ADC_KeyState,ADC_KeyState1,ADC_KeyState2,ADC_KeyState3; //键状态
u8  ADC_KeyHoldCnt; //键按下计时
u8  adcKeyCode;    //给用户使用的键码, 1~16有效

//========================================================================
//                               本地函数声明
//========================================================================

void CalculateAdcKey(u16 adc);

//========================================================================
//                            外部函数和变量声明
//========================================================================


//========================================================================
// 函数: adcKey_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-05-26
//========================================================================
void adcKey_init(void)
{
    ADC_KeyState  = 0;
    ADC_KeyState1 = 0;
    ADC_KeyState2 = 0;
    ADC_KeyState3 = 0;  //键状态
    ADC_KeyHoldCnt = 0; //键按下计时
    adcKeyCode = 0;    //给用户使用的键码, 1~16有效
}

//========================================================================
// 函数: Sample_adcKey
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2022-05-26
//========================================================================
void Sample_adcKey(void)
{
    u16 j;

    j = Get_ADC12bitResult(0);  //参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
    if(j < 4096)    CalculateAdcKey(j); //计算按键

    if(adcKeyCode > 0)     //有键按下
    {
        if(adcKeyCode == 1)    //usrHour +1
        {
            if(++usrHour >= 24)    usrHour = 0;
            DisplayRTC();
        }
        if(adcKeyCode == 2)    //usrHour -1
        {
            if(--usrHour >= 24)    usrHour = 23;
            DisplayRTC();
        }
        if(adcKeyCode == 3)    //usrMinute +1
        {
            usrSecond = 0;
            if(++usrMinute >= 60)  usrMinute = 0;
            DisplayRTC();
        }
        if(adcKeyCode == 4)    //usrMinute -1
        {
            usrSecond = 0;
            if(--usrMinute >= 60)  usrMinute = 59;
            DisplayRTC();
        }
        adcKeyCode = 0;
    }
}

/***************** ADC键盘计算键码 *****************************
电路和软件算法设计: Coody
本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
16个键,理论上各个键对应的ADC值为 (4096 / 16) * k = 256 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是4095.
但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (256*k-ADC_OFFSET) 与 (256*k+ADC_OFFSET)之间为键有效.
间隔一定的时间,就采样一次ADC,比如10ms.
为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
以上算法, 能保证读键非常可靠.
**********************************************/
#define ADC_OFFSET  64
void CalculateAdcKey(u16 adc)
{
    u8  i;
    u16 j;
    
    if(adc < (256-ADC_OFFSET))
    {
        ADC_KeyState = 0;   //键状态归0
        ADC_KeyHoldCnt = 0;
    }
    j = 256;
    for(i=1; i<=16; i++)
    {
        if((adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET)))  break;  //判断是否在偏差范围内
        j += 256;
    }
    ADC_KeyState3 = ADC_KeyState2;
    ADC_KeyState2 = ADC_KeyState1;
    if(i > 16)  ADC_KeyState1 = 0;  //键无效
    else                        //键有效
    {
        ADC_KeyState1 = i;
        if((ADC_KeyState3 == ADC_KeyState2) && (ADC_KeyState2 == ADC_KeyState1) &&
           (ADC_KeyState3 > 0) && (ADC_KeyState2 > 0) && (ADC_KeyState1 > 0))
        {
            if(ADC_KeyState == 0)   //第一次检测到
            {
                adcKeyCode  = i;    //保存键码
                ADC_KeyState = i;   //保存键状态
                ADC_KeyHoldCnt = 0;
            }
            if(ADC_KeyState == i)   //连续检测到同一键按着
            {
                if(++ADC_KeyHoldCnt >= 100) //按下1秒后,以10次每秒的速度Repeat Key
                {
                    ADC_KeyHoldCnt = 90;
                    adcKeyCode  = i; //保存键码
                }
            }
            else ADC_KeyHoldCnt = 0; //按下时间计数归0
        }
    }
}
