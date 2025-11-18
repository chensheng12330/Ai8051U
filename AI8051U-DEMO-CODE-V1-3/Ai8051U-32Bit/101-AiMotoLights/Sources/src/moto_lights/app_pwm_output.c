/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - PWM输出控制模块 ----------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_pwm_output.h"
#include "moto_lights/moto_lights_pinmap.h"

/*************** 功能说明 ****************

PWM输出控制模块

功能:
1. 控制5路PWM输出（刹车灯、RGB三色、蜂鸣器）
2. 支持多种输出模式（静态、呼吸、闪烁、渐变）
3. 软件PWM实现（基于定时任务）
4. RGB灯组控制
5. 蜂鸣器音效控制

PWM频率: 约100Hz（10ms更新周期）
PWM分辨率: 8位（0-255）

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

// PWM通道配置数组
PWM_CONFIG pwm_channels[PWM_CH_COUNT];

//========================================================================
//                          内部变量
//========================================================================

// PWM计数器（0-255）
static u8 pwm_counter = 0;

// 蜂鸣器控制变量
static u16 buzzer_timer = 0;
static u8 buzzer_pattern_count = 0;
static u16 buzzer_on_time = 0;
static u16 buzzer_off_time = 0;
static u8 buzzer_state = 0;

//========================================================================
//                          内部函数声明
//========================================================================
static void PWM_UpdateOutput(void);
static void PWM_ProcessEffects(void);
static void PWM_UpdateBuzzer(void);

//========================================================================
// 函数: PWM_Output_Init
// 描述: 初始化PWM输出模块
// 参数: None.
// 返回: None.
//========================================================================
void PWM_Output_Init(void)
{
    u8 i;
    
    // 初始化所有PWM通道
    for(i = 0; i < PWM_CH_COUNT; i++)
    {
        pwm_channels[i].duty_cycle = 0;
        pwm_channels[i].target_duty = 0;
        pwm_channels[i].mode = PWM_MODE_STATIC;
        pwm_channels[i].phase = 0;
        pwm_channels[i].speed = 10;
        pwm_channels[i].enabled = 1;
    }
    
    // 初始化输出引脚为低电平
    PIN_OUT_BRAKE_LED = 0;
    PIN_OUT_RGB_RED = 0;
    PIN_OUT_RGB_GREEN = 0;
    PIN_OUT_RGB_BLUE = 0;
    PIN_OUT_BUZZER = 0;
    
    pwm_counter = 0;
    
    printf("PWM Output module initialized\r\n");
}

//========================================================================
// 函数: Sample_PWM_Output
// 描述: PWM更新任务（每10ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_PWM_Output(void)
{
    // 更新PWM计数器
    pwm_counter++;
    if(pwm_counter >= 255) pwm_counter = 0;
    
    // 处理效果
    PWM_ProcessEffects();
    
    // 更新输出
    PWM_UpdateOutput();
    
    // 更新蜂鸣器
    PWM_UpdateBuzzer();
}

//========================================================================
// 函数: PWM_UpdateOutput
// 描述: 更新PWM输出引脚
// 参数: None.
// 返回: None.
//========================================================================
static void PWM_UpdateOutput(void)
{
    // 刹车灯PWM
    if(pwm_channels[PWM_CH_BRAKE_LED].enabled)
    {
        PIN_OUT_BRAKE_LED = (pwm_counter < pwm_channels[PWM_CH_BRAKE_LED].duty_cycle) ? 1 : 0;
    }
    else
    {
        PIN_OUT_BRAKE_LED = 0;
    }
    
    // RGB红色PWM
    if(pwm_channels[PWM_CH_RGB_RED].enabled)
    {
        PIN_OUT_RGB_RED = (pwm_counter < pwm_channels[PWM_CH_RGB_RED].duty_cycle) ? 1 : 0;
    }
    else
    {
        PIN_OUT_RGB_RED = 0;
    }
    
    // RGB绿色PWM
    if(pwm_channels[PWM_CH_RGB_GREEN].enabled)
    {
        PIN_OUT_RGB_GREEN = (pwm_counter < pwm_channels[PWM_CH_RGB_GREEN].duty_cycle) ? 1 : 0;
    }
    else
    {
        PIN_OUT_RGB_GREEN = 0;
    }
    
    // RGB蓝色PWM
    if(pwm_channels[PWM_CH_RGB_BLUE].enabled)
    {
        PIN_OUT_RGB_BLUE = (pwm_counter < pwm_channels[PWM_CH_RGB_BLUE].duty_cycle) ? 1 : 0;
    }
    else
    {
        PIN_OUT_RGB_BLUE = 0;
    }
    
    // 蜂鸣器PWM（由UpdateBuzzer控制）
}

//========================================================================
// 函数: PWM_ProcessEffects
// 描述: 处理PWM效果（呼吸、闪烁等）
// 参数: None.
// 返回: None.
//========================================================================
static void PWM_ProcessEffects(void)
{
    u8 i;
    
    for(i = 0; i < PWM_CH_COUNT; i++)
    {
        PWM_CONFIG *ch = &pwm_channels[i];
        
        if(!ch->enabled) continue;
        
        switch(ch->mode)
        {
            case PWM_MODE_STATIC:
                // 静态模式，无需处理
                break;
                
            case PWM_MODE_BREATHING:
                // 呼吸效果（三角波）
                ch->phase += ch->speed;
                if(ch->phase >= 1000) ch->phase = 0;
                
                if(ch->phase < 500)
                    ch->duty_cycle = (ch->phase * ch->target_duty) / 500;
                else
                    ch->duty_cycle = ((1000 - ch->phase) * ch->target_duty) / 500;
                break;
                
            case PWM_MODE_FLASHING:
                // 闪烁效果
                ch->phase += ch->speed;
                if(ch->phase >= 1000) ch->phase = 0;
                
                ch->duty_cycle = (ch->phase < 500) ? ch->target_duty : 0;
                break;
                
            case PWM_MODE_FADE:
                // 渐变到目标值
                if(ch->duty_cycle < ch->target_duty)
                {
                    ch->duty_cycle += ch->speed;
                    if(ch->duty_cycle > ch->target_duty)
                        ch->duty_cycle = ch->target_duty;
                }
                else if(ch->duty_cycle > ch->target_duty)
                {
                    if(ch->duty_cycle >= ch->speed)
                        ch->duty_cycle -= ch->speed;
                    else
                        ch->duty_cycle = ch->target_duty;
                }
                break;
        }
    }
}

//========================================================================
// 函数: PWM_UpdateBuzzer
// 描述: 更新蜂鸣器状态
// 参数: None.
// 返回: None.
//========================================================================
static void PWM_UpdateBuzzer(void)
{
    if(buzzer_timer > 0)
    {
        buzzer_timer -= 10;  // 每次减10ms
        
        if(buzzer_pattern_count > 0)
        {
            // 模式播放
            if(buzzer_state == 0)  // OFF状态
            {
                if(buzzer_timer == 0)
                {
                    // 切换到ON
                    buzzer_state = 1;
                    buzzer_timer = buzzer_on_time;
                    pwm_channels[PWM_CH_BUZZER].duty_cycle = 128;  // 50%占空比
                }
            }
            else  // ON状态
            {
                if(buzzer_timer == 0)
                {
                    // 切换到OFF
                    buzzer_state = 0;
                    buzzer_timer = buzzer_off_time;
                    pwm_channels[PWM_CH_BUZZER].duty_cycle = 0;
                    buzzer_pattern_count--;
                }
            }
        }
        else
        {
            // 单次播放
            if(buzzer_timer == 0)
            {
                pwm_channels[PWM_CH_BUZZER].duty_cycle = 0;
            }
        }
    }
    
    // 更新蜂鸣器输出
    if(pwm_channels[PWM_CH_BUZZER].enabled)
    {
        PIN_OUT_BUZZER = (pwm_counter < pwm_channels[PWM_CH_BUZZER].duty_cycle) ? 1 : 0;
    }
    else
    {
        PIN_OUT_BUZZER = 0;
    }
}

//========================================================================
//                          外部接口函数
//========================================================================

// 设置PWM占空比
void PWM_SetDuty(PWM_CHANNEL channel, u8 duty)
{
    if(channel >= PWM_CH_COUNT) return;
    
    pwm_channels[channel].duty_cycle = duty;
    pwm_channels[channel].target_duty = duty;
    pwm_channels[channel].mode = PWM_MODE_STATIC;
}

// 设置PWM目标占空比
void PWM_SetTarget(PWM_CHANNEL channel, u8 target, u16 speed)
{
    if(channel >= PWM_CH_COUNT) return;
    
    pwm_channels[channel].target_duty = target;
    pwm_channels[channel].speed = speed;
    pwm_channels[channel].mode = PWM_MODE_FADE;
}

// 设置PWM模式
void PWM_SetMode(PWM_CHANNEL channel, PWM_MODE mode, u16 speed)
{
    if(channel >= PWM_CH_COUNT) return;
    
    pwm_channels[channel].mode = mode;
    pwm_channels[channel].speed = speed;
    pwm_channels[channel].phase = 0;
}

// 使能/禁用PWM通道
void PWM_Enable(PWM_CHANNEL channel, u8 enable)
{
    if(channel >= PWM_CH_COUNT) return;
    
    pwm_channels[channel].enabled = enable;
    
    if(!enable)
    {
        pwm_channels[channel].duty_cycle = 0;
    }
}

//========================================================================
//                          RGB灯控制函数
//========================================================================

void PWM_SetRGB(u8 r, u8 g, u8 b)
{
    PWM_SetDuty(PWM_CH_RGB_RED, r);
    PWM_SetDuty(PWM_CH_RGB_GREEN, g);
    PWM_SetDuty(PWM_CH_RGB_BLUE, b);
}

void PWM_RGB_Breathing(u8 r, u8 g, u8 b, u16 speed)
{
    pwm_channels[PWM_CH_RGB_RED].target_duty = r;
    pwm_channels[PWM_CH_RGB_GREEN].target_duty = g;
    pwm_channels[PWM_CH_RGB_BLUE].target_duty = b;
    
    PWM_SetMode(PWM_CH_RGB_RED, PWM_MODE_BREATHING, speed);
    PWM_SetMode(PWM_CH_RGB_GREEN, PWM_MODE_BREATHING, speed);
    PWM_SetMode(PWM_CH_RGB_BLUE, PWM_MODE_BREATHING, speed);
}

void PWM_RGB_Flashing(u8 r, u8 g, u8 b, u16 speed)
{
    pwm_channels[PWM_CH_RGB_RED].target_duty = r;
    pwm_channels[PWM_CH_RGB_GREEN].target_duty = g;
    pwm_channels[PWM_CH_RGB_BLUE].target_duty = b;
    
    PWM_SetMode(PWM_CH_RGB_RED, PWM_MODE_FLASHING, speed);
    PWM_SetMode(PWM_CH_RGB_GREEN, PWM_MODE_FLASHING, speed);
    PWM_SetMode(PWM_CH_RGB_BLUE, PWM_MODE_FLASHING, speed);
}

//========================================================================
//                          刹车灯控制函数
//========================================================================

void PWM_BrakeLight_On(u8 brightness)
{
    PWM_SetDuty(PWM_CH_BRAKE_LED, brightness);
}

void PWM_BrakeLight_Off(void)
{
    PWM_SetDuty(PWM_CH_BRAKE_LED, 0);
}

void PWM_BrakeLight_Flash(u8 brightness, u16 period)
{
    pwm_channels[PWM_CH_BRAKE_LED].target_duty = brightness;
    PWM_SetMode(PWM_CH_BRAKE_LED, PWM_MODE_FLASHING, period / 100);
}

//========================================================================
//                          蜂鸣器控制函数
//========================================================================

void PWM_Buzzer_Beep(u16 duration_ms)
{
    buzzer_timer = duration_ms;
    buzzer_pattern_count = 0;
    pwm_channels[PWM_CH_BUZZER].duty_cycle = 128;  // 50%占空比
}

void PWM_Buzzer_Pattern(u8 count, u16 on_time, u16 off_time)
{
    buzzer_pattern_count = count;
    buzzer_on_time = on_time;
    buzzer_off_time = off_time;
    buzzer_state = 0;  // 从OFF开始
    buzzer_timer = 1;  // 立即开始
}

void PWM_Buzzer_Stop(void)
{
    buzzer_timer = 0;
    buzzer_pattern_count = 0;
    pwm_channels[PWM_CH_BUZZER].duty_cycle = 0;
}

