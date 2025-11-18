/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 配置头文件 ---------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __MOTO_LIGHTS_CONFIG_H_
#define __MOTO_LIGHTS_CONFIG_H_

#include "type_def.h"

//========================================================================
//                          功能模块使能开关
//========================================================================
#define ENABLE_WS2812B          1   // 启用WS2812B氛围灯
#define ENABLE_RGB_LIGHTS       1   // 启用RGB灯组
#define ENABLE_BRAKE_LIGHT      1   // 启用刹车灯
#define ENABLE_BUZZER           1   // 启用蜂鸣器
#define ENABLE_AUDIO_SPECTRUM   1   // 启用音频频谱功能
#define ENABLE_LIGHT_SENSE      1   // 启用环境光感应
#define ENABLE_BATTERY_MONITOR  1   // 启用电池电压监控

//========================================================================
//                          灯光效果优先级定义
//========================================================================
#define PRIORITY_BRAKE          100  // 刹车 - 最高优先级
#define PRIORITY_TURN_SIGNAL    90   // 转向
#define PRIORITY_POSITION_LIGHT 80   // 位置灯
#define PRIORITY_HEADLIGHT      70   // 远近光
#define PRIORITY_HORN_EFFECT    65   // 喇叭效果
#define PRIORITY_AUDIO_SPECTRUM 60   // 音频频谱
#define PRIORITY_AMBIENT        50   // 环境光效
#define PRIORITY_IDLE           0    // 空闲

//========================================================================
//                          定时器参数配置
//========================================================================
#define DEBOUNCE_TIME_MS        20   // 信号消抖时间(ms)
#define TURN_SIGNAL_BLINK_MS    500  // 转向灯闪烁周期(ms)
#define POSITION_BLINK_MS       2000 // 位置灯闪烁周期(ms)
#define STATUS_LED_BLINK_FAST   200  // 状态灯快闪周期(ms)
#define STATUS_LED_BLINK_SLOW   1000 // 状态灯慢闪周期(ms)
#define HORN_EFFECT_DURATION_MS 3000 // 喇叭效果持续时间(ms)

//========================================================================
//                          PWM参数配置
//========================================================================
#define PWM_FREQUENCY           1000  // PWM频率(Hz)
#define PWM_RESOLUTION          255   // PWM分辨率(8位)
#define PWM_DUTY_MIN            0     // 最小占空比
#define PWM_DUTY_MAX            255   // 最大占空比
#define PWM_DUTY_LOW            50    // 低亮度
#define PWM_DUTY_MED            128   // 中亮度
#define PWM_DUTY_HIGH           200   // 高亮度

//========================================================================
//                          WS2812B配置
//========================================================================
#define WS2812B_DATA_RATE       800000  // 800kHz数据速率
#define WS2812B_RESET_TIME_US   50      // 复位时间(us)
#define WS2812B_BRIGHTNESS_DAY  180     // 白天亮度(0-255)
#define WS2812B_BRIGHTNESS_NIGHT 255    // 夜晚亮度(0-255)

// WS2812B灯珠分区定义（总共90颗）
#define WS2812B_ZONE_FRONT_LEFT  0      // 前左区域起始索引
#define WS2812B_ZONE_FRONT_LEFT_LEN 15  // 前左区域灯珠数
#define WS2812B_ZONE_FRONT_RIGHT 15     // 前右区域起始索引
#define WS2812B_ZONE_FRONT_RIGHT_LEN 15 // 前右区域灯珠数
#define WS2812B_ZONE_BODY_LEFT   30     // 车身左侧起始索引
#define WS2812B_ZONE_BODY_LEFT_LEN 20   // 车身左侧灯珠数
#define WS2812B_ZONE_BODY_RIGHT  50     // 车身右侧起始索引
#define WS2812B_ZONE_BODY_RIGHT_LEN 20  // 车身右侧灯珠数
#define WS2812B_ZONE_REAR        70     // 尾部区域起始索引
#define WS2812B_ZONE_REAR_LEN    20     // 尾部区域灯珠数

//========================================================================
//                          颜色预设定义
//========================================================================
typedef struct {
    u8 r;
    u8 g;
    u8 b;
} RGB_COLOR;

// 预定义颜色
#define COLOR_RED       {255, 0, 0}
#define COLOR_GREEN     {0, 255, 0}
#define COLOR_BLUE      {0, 0, 255}
#define COLOR_WHITE     {255, 255, 255}
#define COLOR_YELLOW    {255, 255, 0}
#define COLOR_ORANGE    {255, 128, 0}
#define COLOR_PURPLE    {128, 0, 128}
#define COLOR_CYAN      {0, 255, 255}
#define COLOR_OFF       {0, 0, 0}

//========================================================================
//                          音频频谱配置
//========================================================================
#define AUDIO_SAMPLE_RATE       1000    // 音频采样率(Hz)
#define AUDIO_FFT_SIZE          64      // FFT大小（简化版）
#define AUDIO_BAND_COUNT        8       // 频谱带数量
#define AUDIO_THRESHOLD         100     // 音频检测阈值

//========================================================================
//                          系统状态定义
//========================================================================
typedef enum {
    SYSTEM_STATE_INIT = 0,       // 初始化状态
    SYSTEM_STATE_NORMAL,         // 正常运行状态
    SYSTEM_STATE_LOW_POWER,      // 低电量状态
    SYSTEM_STATE_ERROR,          // 错误状态
    SYSTEM_STATE_TEST            // 测试模式
} SYSTEM_STATE;

//========================================================================
//                          工作模式定义
//========================================================================
typedef enum {
    MODE_DAY = 0,                // 白天模式
    MODE_NIGHT                   // 夜晚模式
} WORK_MODE;

//========================================================================
//                          灯光效果类型定义
//========================================================================
typedef enum {
    EFFECT_NONE = 0,             // 无效果
    EFFECT_STATIC,               // 静态
    EFFECT_BREATHING,            // 呼吸
    EFFECT_FLASHING,             // 闪烁
    EFFECT_FLOWING,              // 流水
    EFFECT_RAINBOW,              // 彩虹
    EFFECT_SPECTRUM              // 频谱
} LIGHT_EFFECT;

#endif  // __MOTO_LIGHTS_CONFIG_H_

