/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - PWM输出控制模块 ----------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_PWM_OUTPUT_H_
#define __APP_PWM_OUTPUT_H_

#include "../type_def.h"
#include "moto_lights_config.h"

//========================================================================
//                          PWM通道定义
//========================================================================
typedef enum {
    PWM_CH_BRAKE_LED = 0,   // P1.0 尾箱刹车灯
    PWM_CH_RGB_RED,         // P1.1 RGB红色
    PWM_CH_RGB_GREEN,       // P1.2 RGB绿色
    PWM_CH_RGB_BLUE,        // P1.3 RGB蓝色
    PWM_CH_BUZZER,          // P1.5 蜂鸣器
    PWM_CH_COUNT = 5
} PWM_CHANNEL;

//========================================================================
//                          PWM模式定义
//========================================================================
typedef enum {
    PWM_MODE_STATIC = 0,    // 静态输出
    PWM_MODE_BREATHING,     // 呼吸效果
    PWM_MODE_FLASHING,      // 闪烁效果
    PWM_MODE_FADE           // 渐变效果
} PWM_MODE;

//========================================================================
//                          PWM通道配置结构体
//========================================================================
typedef struct {
    u8 duty_cycle;          // 占空比(0-255)
    u8 target_duty;         // 目标占空比(用于渐变)
    PWM_MODE mode;          // 工作模式
    u16 phase;              // 相位参数
    u16 speed;              // 速度参数
    u8 enabled;             // 使能标志
} PWM_CONFIG;

//========================================================================
//                          全局变量声明
//========================================================================
extern PWM_CONFIG pwm_channels[PWM_CH_COUNT];

//========================================================================
//                          函数声明
//========================================================================

// 初始化PWM输出模块
void PWM_Output_Init(void);

// PWM更新任务（每10ms调用）
void Sample_PWM_Output(void);

// 设置PWM占空比（立即生效）
void PWM_SetDuty(PWM_CHANNEL channel, u8 duty);

// 设置PWM目标占空比（渐变到目标值）
void PWM_SetTarget(PWM_CHANNEL channel, u8 target, u16 speed);

// 设置PWM模式
void PWM_SetMode(PWM_CHANNEL channel, PWM_MODE mode, u16 speed);

// 使能/禁用PWM通道
void PWM_Enable(PWM_CHANNEL channel, u8 enable);

// RGB灯控制函数
void PWM_SetRGB(u8 r, u8 g, u8 b);
void PWM_RGB_Breathing(u8 r, u8 g, u8 b, u16 speed);
void PWM_RGB_Flashing(u8 r, u8 g, u8 b, u16 speed);

// 刹车灯控制函数
void PWM_BrakeLight_On(u8 brightness);
void PWM_BrakeLight_Off(void);
void PWM_BrakeLight_Flash(u8 brightness, u16 period);

// 蜂鸣器控制函数
void PWM_Buzzer_Beep(u16 duration_ms);
void PWM_Buzzer_Pattern(u8 count, u16 on_time, u16 off_time);
void PWM_Buzzer_Stop(void);

#endif  // __APP_PWM_OUTPUT_H_

