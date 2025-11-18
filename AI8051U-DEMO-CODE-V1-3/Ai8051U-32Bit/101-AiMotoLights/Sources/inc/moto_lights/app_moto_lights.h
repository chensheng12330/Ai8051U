/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 主应用层 -----------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_MOTO_LIGHTS_H_
#define __APP_MOTO_LIGHTS_H_

#include "../type_def.h"
#include "moto_lights_config.h"

//========================================================================
//                          系统信息结构体
//========================================================================
typedef struct {
    SYSTEM_STATE system_state;      // 系统状态
    u8 error_code;                  // 错误码
    u32 run_time_seconds;           // 运行时间(秒)
    u8 status_led_state;            // 状态LED状态
    u16 status_led_blink_period;    // 状态LED闪烁周期
} SYSTEM_INFO;

//========================================================================
//                          全局变量声明
//========================================================================
extern SYSTEM_INFO system_info;

//========================================================================
//                          函数声明
//========================================================================

// 系统初始化
void MotoLights_Init(void);

// 主应用任务（每50ms调用）
void Sample_MotoLights_App(void);

// 按键事件处理
void MotoLights_KeyEventHandler(KEY_EVENT *event);

// 原车信号事件处理
void MotoLights_SignalEventHandler(VEHICLE_SIGNAL_TYPE signal, u8 new_state);

// 系统状态指示任务（每100ms调用）
void Sample_StatusIndicator(void);

// 错误处理
void MotoLights_SetError(u8 error_code);
void MotoLights_ClearError(void);

//========================================================================
//                          测试函数
//========================================================================
void MotoLights_Test_AllLights(void);
void MotoLights_Test_Sequence(void);

#endif  // __APP_MOTO_LIGHTS_H_

