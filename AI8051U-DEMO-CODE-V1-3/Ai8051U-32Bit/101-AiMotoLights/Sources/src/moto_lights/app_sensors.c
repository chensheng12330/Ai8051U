/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 传感器模块 ---------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_sensors.h"
#include "moto_lights/moto_lights_pinmap.h"
#include "../adc.h"

/*************** 功能说明 ****************

传感器模块

功能:
1. 光强传感器 - 检测环境光强度，自动切换白天/夜晚模式
2. 电池电压传感器 - 监控电瓶电压，低电压报警
3. 音频传感器 - 检测音频信号，实现频谱显示

采样方式:
- 使用ADC采样（12位，0-4095）
- 移动平均滤波（降噪）
- 物理值转换

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

SENSOR_DATA sensor_data[SENSOR_COUNT];
AUDIO_SPECTRUM_DATA audio_spectrum;

//========================================================================
//                          内部变量
//========================================================================

// 滤波缓冲区（移动平均）
#define FILTER_SIZE 8
static u16 filter_buffer[SENSOR_COUNT][FILTER_SIZE];
static u8 filter_index = 0;

// 音频采样缓冲区
#define AUDIO_BUFFER_SIZE 64
static u16 audio_buffer[AUDIO_BUFFER_SIZE];
static u8 audio_buffer_index = 0;

//========================================================================
//                          内部函数声明
//========================================================================
static u16 FilterValue(SENSOR_TYPE sensor, u16 raw_value);
static float ConvertToPhysical(SENSOR_TYPE sensor, u16 adc_value);
static void SampleAudio(void);

//========================================================================
// 函数: Sensors_Init
// 描述: 初始化传感器模块
// 参数: None.
// 返回: None.
//========================================================================
void Sensors_Init(void)
{
    u8 i, j;
    
    // 初始化传感器数据
    for(i = 0; i < SENSOR_COUNT; i++)
    {
        sensor_data[i].raw_value = 0;
        sensor_data[i].filtered_value = 0;
        sensor_data[i].physical_value = 0.0f;
        sensor_data[i].valid = 0;
        
        // 初始化滤波缓冲区
        for(j = 0; j < FILTER_SIZE; j++)
        {
            filter_buffer[i][j] = 0;
        }
    }
    
    // 初始化音频频谱
    for(i = 0; i < AUDIO_BAND_COUNT; i++)
    {
        audio_spectrum.bands[i] = 0;
    }
    audio_spectrum.audio_detected = 0;
    audio_spectrum.peak_level = 0;
    
    filter_index = 0;
    audio_buffer_index = 0;
    
    printf("Sensors module initialized\r\n");
}

//========================================================================
// 函数: Sample_Sensors
// 描述: 传感器采样任务（每100ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_Sensors(void)
{
    u16 adc_value;
    
    // 采样光强传感器
    // 注意：原P1.5已用作蜂鸣器，光强传感器需连接到其他ADC通道
    // 当前ADC_CH_LIGHT_SENSE=15（P1.5）会导致冲突，请根据实际硬件连接修改
    // 如果未连接光强传感器，此采样值将无效，但不影响其他功能
    adc_value = Get_ADC12bitResult(ADC_CH_LIGHT_SENSE);
    sensor_data[SENSOR_LIGHT].raw_value = adc_value;
    sensor_data[SENSOR_LIGHT].filtered_value = FilterValue(SENSOR_LIGHT, adc_value);
    sensor_data[SENSOR_LIGHT].physical_value = ConvertToPhysical(SENSOR_LIGHT, 
                                                    sensor_data[SENSOR_LIGHT].filtered_value);
    sensor_data[SENSOR_LIGHT].valid = 1;
    
    // 采样电池电压 (ADC通道7, P0.7)
    adc_value = Get_ADC12bitResult(ADC_CH_BATTERY);
    sensor_data[SENSOR_BATTERY].raw_value = adc_value;
    sensor_data[SENSOR_BATTERY].filtered_value = FilterValue(SENSOR_BATTERY, adc_value);
    sensor_data[SENSOR_BATTERY].physical_value = ConvertToPhysical(SENSOR_BATTERY, 
                                                    sensor_data[SENSOR_BATTERY].filtered_value);
    sensor_data[SENSOR_BATTERY].valid = 1;
    
    // 采样音频信号（快速采样）
    SampleAudio();
    
    // 更新滤波索引
    filter_index++;
    if(filter_index >= FILTER_SIZE) filter_index = 0;
}

//========================================================================
// 函数: FilterValue
// 描述: 移动平均滤波
// 参数: sensor - 传感器类型, raw_value - 原始值
// 返回: 滤波后的值
//========================================================================
static u16 FilterValue(SENSOR_TYPE sensor, u16 raw_value)
{
    u8 i;
    u32 sum = 0;
    
    // 更新缓冲区
    filter_buffer[sensor][filter_index] = raw_value;
    
    // 计算平均值
    for(i = 0; i < FILTER_SIZE; i++)
    {
        sum += filter_buffer[sensor][i];
    }
    
    return (u16)(sum / FILTER_SIZE);
}

//========================================================================
// 函数: ConvertToPhysical
// 描述: ADC值转换为物理值
// 参数: sensor - 传感器类型, adc_value - ADC值
// 返回: 物理值
//========================================================================
static float ConvertToPhysical(SENSOR_TYPE sensor, u16 adc_value)
{
    float result = 0.0f;
    
    switch(sensor)
    {
        case SENSOR_LIGHT:
            // 光强传感器：ADC值直接对应光强（简化）
            // 实际应根据传感器特性曲线转换
            result = (float)adc_value;
            break;
            
        case SENSOR_BATTERY:
            // 电池电压：ADC值转电压
            // 假设：ADC参考电压5V，分压比1:1
            // 实际应根据分压电路调整
            result = ((float)adc_value / 4096.0f) * 5.0f * 3.0f;  // *3为分压比
            break;
            
        case SENSOR_AUDIO:
            // 音频：ADC值转音量（简化为原始值）
            result = (float)adc_value;
            break;
    }
    
    return result;
}

//========================================================================
// 函数: SampleAudio
// 描述: 音频快速采样
// 参数: None.
// 返回: None.
//========================================================================
static void SampleAudio(void)
{
    u16 adc_value;
    u8 i;
    
    // 采样音频ADC
    adc_value = Get_ADC12bitResult(ADC_CH_AUDIO);
    
    // 保存到缓冲区
    audio_buffer[audio_buffer_index++] = adc_value;
    if(audio_buffer_index >= AUDIO_BUFFER_SIZE)
    {
        audio_buffer_index = 0;
        
        // 缓冲区满，更新频谱
        Sensors_UpdateAudioSpectrum();
    }
    
    // 更新传感器数据
    sensor_data[SENSOR_AUDIO].raw_value = adc_value;
    sensor_data[SENSOR_AUDIO].filtered_value = FilterValue(SENSOR_AUDIO, adc_value);
    
    // 检测音频峰值
    if(adc_value > audio_spectrum.peak_level)
    {
        audio_spectrum.peak_level = adc_value;
    }
    
    // 音频检测：超过阈值
    if(adc_value > AUDIO_THRESHOLD)
    {
        audio_spectrum.audio_detected = 1;
    }
    else
    {
        audio_spectrum.audio_detected = 0;
    }
}

//========================================================================
// 函数: Sensors_UpdateAudioSpectrum
// 描述: 更新音频频谱（简化FFT）
// 参数: None.
// 返回: None.
//========================================================================
void Sensors_UpdateAudioSpectrum(void)
{
    u8 i, j;
    u16 band_sum;
    u8 samples_per_band = AUDIO_BUFFER_SIZE / AUDIO_BAND_COUNT;
    
    // 简化版频谱分析：将采样点分组求平均
    // 注意：这不是真正的FFT，仅作演示
    for(i = 0; i < AUDIO_BAND_COUNT; i++)
    {
        band_sum = 0;
        for(j = 0; j < samples_per_band; j++)
        {
            band_sum += audio_buffer[i * samples_per_band + j];
        }
        
        // 转换为0-255范围
        audio_spectrum.bands[i] = (u8)((band_sum / samples_per_band) >> 4);
    }
}

//========================================================================
//                          外部接口函数
//========================================================================

float Sensors_GetPhysicalValue(SENSOR_TYPE sensor)
{
    if(sensor >= SENSOR_COUNT) return 0.0f;
    return sensor_data[sensor].physical_value;
}

u16 Sensors_GetRawValue(SENSOR_TYPE sensor)
{
    if(sensor >= SENSOR_COUNT) return 0;
    return sensor_data[sensor].raw_value;
}

// 光强传感器函数
WORK_MODE Sensors_GetLightMode(void)
{
    u16 light_intensity = sensor_data[SENSOR_LIGHT].filtered_value;
    
    // 滞回判断（防抖）
    static WORK_MODE last_mode = MODE_DAY;
    
    if(light_intensity > LIGHT_THRESHOLD_DAY)
    {
        last_mode = MODE_DAY;
    }
    else if(light_intensity < LIGHT_THRESHOLD_NIGHT)
    {
        last_mode = MODE_NIGHT;
    }
    // 中间区域保持上次状态
    
    return last_mode;
}

u16 Sensors_GetLightIntensity(void)
{
    return sensor_data[SENSOR_LIGHT].filtered_value;
}

// 电池电压函数
float Sensors_GetBatteryVoltage(void)
{
    return sensor_data[SENSOR_BATTERY].physical_value;
}

u8 Sensors_IsBatteryLow(void)
{
    float voltage = Sensors_GetBatteryVoltage();
    return (voltage < BATTERY_LOW_VOLTAGE) ? 1 : 0;
}

// 音频传感器函数
u8 Sensors_IsAudioDetected(void)
{
    return audio_spectrum.audio_detected;
}

AUDIO_SPECTRUM_DATA* Sensors_GetAudioSpectrum(void)
{
    return &audio_spectrum;
}

