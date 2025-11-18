/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 灯光效果调度器 -----------------------*/
/*---------------------------------------------------------------------*/

#ifndef __APP_LIGHT_SCHEDULER_H_
#define __APP_LIGHT_SCHEDULER_H_

#include "../type_def.h"
#include "moto_lights_config.h"
#include "app_vehicle_signal.h"

//========================================================================
//                          灯光场景类型定义
//========================================================================
typedef enum {
    SCENE_IDLE = 0,         // 空闲场景
    SCENE_BRAKE,            // 刹车场景
    SCENE_TURN_LEFT,        // 左转向场景
    SCENE_TURN_RIGHT,       // 右转向场景
    SCENE_POSITION_LIGHT,   // 位置灯场景
    SCENE_NEAR_LIGHT,       // 近光灯场景
    SCENE_FAR_LIGHT,        // 远光灯场景
    SCENE_HORN_EFFECT,      // 喇叭效果场景
    SCENE_AUDIO_SPECTRUM,   // 音频频谱场景
    SCENE_TEST_MODE,        // 测试模式场景
    SCENE_COUNT
} LIGHT_SCENE;

//========================================================================
//                          场景优先级定义
//========================================================================
typedef struct {
    LIGHT_SCENE scene;      // 场景类型
    u8 priority;            // 优先级(0-100)
    u8 active;              // 激活状态
    u32 start_time;         // 场景开始时间(ms)
    u32 duration;           // 持续时间(ms, 0=无限)
} SCENE_INFO;

//========================================================================
//                          调度器状态结构体
//========================================================================
typedef struct {
    LIGHT_SCENE current_scene;      // 当前激活场景
    WORK_MODE work_mode;             // 工作模式(白天/夜晚)
    u8 global_brightness;            // 全局亮度(0-255)
    u8 test_mode_active;             // 测试模式激活标志
    u8 scene_changed;                // 场景变化标志
} SCHEDULER_STATE;

//========================================================================
//                          全局变量声明
//========================================================================
extern SCHEDULER_STATE scheduler_state;
extern SCENE_INFO active_scenes[SCENE_COUNT];

//========================================================================
//                          函数声明
//========================================================================

// 初始化灯光调度器
void LightScheduler_Init(void);

// 调度器主任务（每20ms调用）
void Sample_LightScheduler(void);

// 场景控制函数
void LightScheduler_ActivateScene(LIGHT_SCENE scene, u32 duration_ms);
void LightScheduler_DeactivateScene(LIGHT_SCENE scene);
void LightScheduler_ClearAllScenes(void);

// 获取当前激活场景
LIGHT_SCENE LightScheduler_GetActiveScene(void);

// 设置工作模式
void LightScheduler_SetWorkMode(WORK_MODE mode);

// 设置全局亮度
void LightScheduler_SetGlobalBrightness(u8 brightness);

// 测试模式控制
void LightScheduler_EnterTestMode(void);
void LightScheduler_ExitTestMode(void);

//========================================================================
//                          场景渲染函数声明
//========================================================================
void Scene_Render_Idle(void);
void Scene_Render_Brake(void);
void Scene_Render_TurnLeft(void);
void Scene_Render_TurnRight(void);
void Scene_Render_PositionLight(void);
void Scene_Render_NearLight(void);
void Scene_Render_FarLight(void);
void Scene_Render_HornEffect(void);
void Scene_Render_AudioSpectrum(void);
void Scene_Render_TestMode(void);

#endif  // __APP_LIGHT_SCHEDULER_H_

