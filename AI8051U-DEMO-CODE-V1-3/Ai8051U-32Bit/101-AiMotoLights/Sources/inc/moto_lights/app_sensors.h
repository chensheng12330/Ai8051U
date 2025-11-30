/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 传感器模块 ---------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_SENSORS_H_
#define __APP_SENSORS_H_

#include "../type_def.h"
#include "moto_lights_config.h"

//========================================================================
//                          传感器类型定义
//========================================================================
typedef enum {
    SENSOR_LIGHT = 0,       // 光强传感器
    SENSOR_BATTERY,         // 电池电压传感器
    SENSOR_AUDIO,           // 音频传感器
    SENSOR_COUNT
} SENSOR_TYPE;

//========================================================================
//                          传感器数据结构体
//========================================================================
typedef struct {
    u16 raw_value;          // 原始ADC值(0-4095)
    u16 filtered_value;     // 滤波后的值
    float physical_value;   // 物理值（光强lux、电压V、音量dB）
    u8 valid;               // 数据有效标志
} SENSOR_DATA;

//========================================================================
//                          音频频谱数据
//========================================================================
typedef struct {
    u8 bands[AUDIO_BAND_COUNT];     // 频谱带数据(0-255)
    u8 audio_detected;              // 音频检测标志
    u16 peak_level;                 // 峰值电平
} AUDIO_SPECTRUM_DATA;

//========================================================================
//                          全局变量声明
//========================================================================
extern SENSOR_DATA sensor_data[SENSOR_COUNT];
extern AUDIO_SPECTRUM_DATA audio_spectrum;

//========================================================================
//                          函数声明
//========================================================================

// 初始化传感器模块
void Sensors_Init(void);

// 传感器采样任务（每100ms调用）
void Sample_Sensors(void);

// 获取传感器物理值
float Sensors_GetPhysicalValue(SENSOR_TYPE sensor);

// 获取传感器原始值
u16 Sensors_GetRawValue(SENSOR_TYPE sensor);

// 光强传感器函数
WORK_MODE Sensors_GetLightMode(void);
u16 Sensors_GetLightIntensity(void);

// 电池电压函数
float Sensors_GetBatteryVoltage(void);
u8 Sensors_IsBatteryLow(void);

// 音频传感器函数
u8 Sensors_IsAudioDetected(void);
AUDIO_SPECTRUM_DATA* Sensors_GetAudioSpectrum(void);
void Sensors_UpdateAudioSpectrum(void);

#endif  // __APP_SENSORS_H_

