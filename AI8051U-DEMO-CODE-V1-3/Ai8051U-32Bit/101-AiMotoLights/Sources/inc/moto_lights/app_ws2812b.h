/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - WS2812B控制模块 ----------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_WS2812B_H_
#define __APP_WS2812B_H_

#include "../type_def.h"
#include "moto_lights_config.h"

//========================================================================
//                          WS2812B颜色结构体
//========================================================================
typedef struct {
    u8 g;  // Green (注意：WS2812B顺序是GRB)
    u8 r;  // Red
    u8 b;  // Blue
} WS2812B_COLOR;

//========================================================================
//                          灯珠区域定义
//========================================================================
typedef enum {
    ZONE_FRONT_LEFT = 0,    // 前左区域
    ZONE_FRONT_RIGHT,       // 前右区域
    ZONE_BODY_LEFT,         // 车身左侧
    ZONE_BODY_RIGHT,        // 车身右侧
    ZONE_REAR,              // 尾部区域
    ZONE_ALL,               // 全部区域
    ZONE_COUNT = 6
} WS2812B_ZONE;

//========================================================================
//                          灯光效果参数结构体
//========================================================================
typedef struct {
    LIGHT_EFFECT effect_type;   // 效果类型
    WS2812B_COLOR color1;        // 主颜色
    WS2812B_COLOR color2;        // 副颜色（用于渐变等）
    u8 brightness;               // 亮度(0-255)
    u16 speed;                   // 速度参数(效果相关)
    u16 phase;                   // 相位参数(内部使用)
} EFFECT_PARAM;

//========================================================================
//                          全局变量声明
//========================================================================
extern WS2812B_COLOR ws2812b_buffer[WS2812B_LED_COUNT];
extern u8 ws2812b_brightness;

//========================================================================
//                          函数声明
//========================================================================

// 初始化WS2812B模块
void WS2812B_Init(void);

// 发送数据到WS2812B（更新显示）
void WS2812B_Update(void);

// 设置单个LED颜色
void WS2812B_SetPixel(u16 index, u8 r, u8 g, u8 b);

// 设置区域颜色
void WS2812B_SetZone(WS2812B_ZONE zone, u8 r, u8 g, u8 b);

// 设置全部LED颜色
void WS2812B_SetAll(u8 r, u8 g, u8 b);

// 清除全部LED
void WS2812B_Clear(void);

// 设置全局亮度
void WS2812B_SetBrightness(u8 brightness);

// 应用亮度到颜色
WS2812B_COLOR WS2812B_ApplyBrightness(WS2812B_COLOR color, u8 brightness);

//========================================================================
//                          灯光效果函数
//========================================================================

// 静态效果
void WS2812B_Effect_Static(WS2812B_ZONE zone, EFFECT_PARAM *param);

// 呼吸效果
void WS2812B_Effect_Breathing(WS2812B_ZONE zone, EFFECT_PARAM *param);

// 流水效果（转向灯）
void WS2812B_Effect_Flowing(WS2812B_ZONE zone, EFFECT_PARAM *param);

// 闪烁效果
void WS2812B_Effect_Flashing(WS2812B_ZONE zone, EFFECT_PARAM *param);

// 彩虹效果
void WS2812B_Effect_Rainbow(WS2812B_ZONE zone, EFFECT_PARAM *param);

// 频谱效果
void WS2812B_Effect_Spectrum(u8 *spectrum_data, u8 band_count);

//========================================================================
//                          辅助函数
//========================================================================

// 获取区域起始索引
u16 WS2812B_GetZoneStart(WS2812B_ZONE zone);

// 获取区域长度
u16 WS2812B_GetZoneLength(WS2812B_ZONE zone);

// HSV转RGB
WS2812B_COLOR WS2812B_HSV2RGB(u16 hue, u8 sat, u8 val);

#endif  // __APP_WS2812B_H_

