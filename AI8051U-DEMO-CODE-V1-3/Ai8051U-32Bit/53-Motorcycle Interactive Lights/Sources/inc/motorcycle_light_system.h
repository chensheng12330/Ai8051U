/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __MOTORCYCLE_LIGHT_SYSTEM_H_
#define __MOTORCYCLE_LIGHT_SYSTEM_H_

//========================================================================
//                                头文件
//========================================================================

#include "type_def.h"
#include "AI8051U.h"
#include "config.h"

//========================================================================
//                               系统常量定义
//========================================================================

// 中断标志宏定义 (兼容AI8051U.h)
#define INT2_FLAG       INT2IF
#define INT3_FLAG       INT3IF

// DMA完成标志
#define DMA_SPI_DONE    DMA_SPI_CR & 0x80  // SPI DMA完成标志
#define DMA_ADC_DONE    DMA_ADC_CR & 0x80  // ADC DMA完成标志

// DMA中断向量
#define DMA_SPI_VECTOR  32  // SPI DMA中断向量
#define DMA_ADC_VECTOR  33  // ADC DMA中断向量

// 硬件配置
#define WS2812_COUNT        90      // WS2812灯珠数量
#define ADC_AUDIO_CHANNEL   6       // 音频输入ADC通道 (P0.6)
#define ADC_BATTERY_CHANNEL 7       // 电瓶电压ADC通道 (P0.7)
#define ADC_SAMPLE_COUNT    128     // ADC采样点数
#define ADC_SAMPLE_RATE     8000    // ADC采样率

// 系统阈值
#define LOW_VOLTAGE_THRESHOLD   10500   // 低电压阈值 (10.5V)
#define RECOVER_VOLTAGE_THRESHOLD 11500 // 恢复电压阈值 (11.5V)

// 超时设置 (ms)
#define TURN_SIGNAL_TIMEOUT     3000    // 转向信号超时
#define MUSIC_MODE_TIMEOUT      10000   // 音乐模式超时

//========================================================================
//                               数据类型定义
//========================================================================

// RGB颜色结构体
typedef struct {
    u8 r, g, b;
} RGB_COLOR;

// HSV颜色结构体
typedef struct {
    u16 h;     // 色相 0-360
    u8 s;      // 饱和度 0-255
    u8 v;      // 明度 0-255
} HSV_COLOR;

// 车辆信号结构体
typedef struct {
    u8 brake_signal;        // 刹车信号
    u8 fog_light;          // 雾灯信号
    u8 headlight_low;      // 近光灯信号
    u8 headlight_high;     // 远光灯信号
    u8 turn_left;          // 左转向信号
    u8 turn_right;         // 右转向信号
    u8 horn_pressed;       // 喇叭按下信号
} VEHICLE_SIGNALS;

// 传感器数据结构体
typedef struct {
    s16 accel_x, accel_y, accel_z;    // 加速度计数据
    u16 light_level;                  // 环境光强度
    u16 audio_level;                  // 音频电平
    u16 battery_voltage;              // 电瓶电压
} SENSOR_DATA;

// PWM占空比结构体
typedef struct {
    u16 red_duty;      // 红色通道占空比
    u16 green_duty;    // 绿色通道占空比
    u16 blue_duty;     // 蓝色通道占空比
    u16 brake_duty;    // 刹车灯占空比
    u16 buzzer_duty;   // 蜂鸣器占空比
} PWM_DUTIES;

// 系统状态枚举
typedef enum {
    SYS_INIT,           // 系统初始化
    SYS_NORMAL,         // 正常运行
    SYS_LOW_POWER,      // 低功耗模式
    SYS_ERROR,          // 错误状态
    SYS_TEST_MODE       // 测试模式
} SYSTEM_STATE;

// 灯效模式枚举
typedef enum {
    LIGHT_OFF,          // 关灯模式
    LIGHT_POSITION,     // 位置灯模式
    LIGHT_TURN_LEFT,    // 左转向模式
    LIGHT_TURN_RIGHT,   // 右转向模式
    LIGHT_BRAKE,        // 刹车模式
    LIGHT_HIGH_BEAM,    // 远光灯模式
    LIGHT_MUSIC,        // 音乐频谱模式
    LIGHT_AMBIENT       // 环境适应模式
} LIGHT_MODE;

// 任务优先级枚举
typedef enum {
    PRIORITY_BRAKE = 0,    // 刹车优先级 (最高)
    PRIORITY_TURN,         // 转向优先级
    PRIORITY_POSITION,     // 位置灯优先级
    PRIORITY_HEADLIGHT,    // 远近光优先级
    PRIORITY_MUSIC,        // 音乐优先级
    PRIORITY_AMBIENT       // 环境适应优先级 (最低)
} TASK_PRIORITY;

//========================================================================
//                               全局变量声明
//========================================================================

// 系统状态
extern SYSTEM_STATE system_state;
extern LIGHT_MODE current_mode;
extern LIGHT_MODE previous_mode;

// 数据结构体
extern VEHICLE_SIGNALS vehicle_signals;
extern SENSOR_DATA sensor_data;
extern PWM_DUTIES pwm_duties;

// WS2812数据缓冲区
extern RGB_COLOR xdata led_colors[WS2812_COUNT];
extern u8 xdata ws2812_buffer[WS2812_COUNT * 24];

// 音频数据缓冲区
extern u16 xdata audio_samples[ADC_SAMPLE_COUNT];
extern u8 xdata frequency_bands[8];

// ADC全局变量 (与现有ADC模块兼容)
extern u16 adc_audio_avg;
extern u16 adc_battery_avg;

// 模式计时器
extern u16 xdata mode_timers[8];
extern u8 xdata active_modes[8];
extern u8 active_mode_count;

//========================================================================
//                               函数声明
//========================================================================

// 系统初始化
void Motorcycle_Light_System_Init(void);
void GPIO_Config_Update(void);

// 硬件初始化
void WS2812_Init(void);
void PWM_Multi_Channel_Init(void);
void ADC_DMA_Config_Multi(void);
void ADC_Start_Multi_DMA(void);

// 任务函数
void Sample_Vehicle_Signal_Process(void);
void Sample_WS2812_DMA_Control(void);
void Sample_Button_Process(void);
void Sample_Sensor_Read(void);
void Sample_Light_Effect_Calculate(void);
void Sample_Audio_Process(void);
void Sample_Status_Display(void);
void Sample_Debug_Output(void);

// 信号处理
u8 Brake_Signal_Active(void);
u8 Turn_Left_Signal_Active(void);
u8 Turn_Right_Signal_Active(void);
u8 High_Beam_Active(void);
u8 Audio_Signal_Detected(void);

// 灯效控制
void Set_Light_Mode(LIGHT_MODE mode, u8 enable);
void Apply_Light_Effect(LIGHT_MODE mode);
void WS2812_Send_DMA(void);
void WS2812_Encode_Buffer(void);
u8 Get_Light_Mode_Priority(LIGHT_MODE mode);

// 状态机
void System_State_Machine(void);
void Light_Mode_State_Machine(void);
void Check_Mode_Timeouts(void);

// 传感器接口
void MPU6050_Read_Accel(s16 *x, s16 *y, s16 *z);
u16 BH1750_Read_Light(void);

// 音频处理
void Audio_FFT_Process(u16 *samples);
void Calculate_Frequency_Bands(void);

// 工具函数
RGB_COLOR HSV_to_RGB(u16 h, u8 s, u8 v);
void Update_PWM_Outputs(void);

// 调试函数
void Print_System_Status(void);
void Print_Light_Status(void);

#endif
