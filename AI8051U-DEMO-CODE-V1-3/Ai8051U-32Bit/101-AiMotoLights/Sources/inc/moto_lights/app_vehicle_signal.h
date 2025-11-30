/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 原车信号检测模块 ---------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_VEHICLE_SIGNAL_H_
#define __APP_VEHICLE_SIGNAL_H_

#include "../type_def.h"
#include "moto_lights_config.h"

//========================================================================
//                          信号类型定义
//========================================================================
typedef enum {
    SIGNAL_BRAKE = 0,       // 刹车信号
    SIGNAL_FOG_LIGHT,       // 雾灯信号
    SIGNAL_NEAR_LIGHT,      // 近光灯信号
    SIGNAL_FAR_LIGHT,       // 远光灯信号
    SIGNAL_TURN_LEFT,       // 左转向信号
    SIGNAL_TURN_RIGHT,      // 右转向信号
    SIGNAL_HORN,            // 喇叭信号(虚拟，通过音频检测)
    SIGNAL_COUNT            // 信号总数
} VEHICLE_SIGNAL_TYPE;

//========================================================================
//                          信号状态结构体
//========================================================================
typedef struct {
    u8  current_state;      // 当前状态(0/1)
    u8  last_state;         // 上一次状态(0/1)
    u8  debounce_count;     // 消抖计数器
    u8  edge_detected;      // 边沿检测标志: 0=无, 1=上升沿, 2=下降沿
    u16 active_time;        // 信号激活持续时间(ms)
    u32 last_trigger_time;  // 上次触发时间戳(ms)
} SIGNAL_STATE;

//========================================================================
//                          全局变量声明
//========================================================================
extern SIGNAL_STATE vehicle_signals[SIGNAL_COUNT];

//========================================================================
//                          函数声明
//========================================================================

// 初始化原车信号检测模块
void VehicleSignal_Init(void);

// 原车信号扫描任务（每10ms调用一次）
void Sample_VehicleSignal(void);

// 获取信号当前状态（带消抖）
u8 VehicleSignal_GetState(VEHICLE_SIGNAL_TYPE signal);

// 检测信号上升沿
u8 VehicleSignal_IsRisingEdge(VEHICLE_SIGNAL_TYPE signal);

// 检测信号下降沿
u8 VehicleSignal_IsFallingEdge(VEHICLE_SIGNAL_TYPE signal);

// 获取信号激活时间
u16 VehicleSignal_GetActiveTime(VEHICLE_SIGNAL_TYPE signal);

// 清除边沿检测标志
void VehicleSignal_ClearEdge(VEHICLE_SIGNAL_TYPE signal);

// 信号状态变化回调函数（用户可注册）
typedef void (*SignalCallback)(VEHICLE_SIGNAL_TYPE signal, u8 new_state);
void VehicleSignal_RegisterCallback(SignalCallback callback);

#endif  // __APP_VEHICLE_SIGNAL_H_

