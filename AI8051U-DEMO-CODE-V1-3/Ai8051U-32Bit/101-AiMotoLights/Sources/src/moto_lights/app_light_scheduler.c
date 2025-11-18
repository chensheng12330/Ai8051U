/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 灯光效果调度器 -----------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_light_scheduler.h"
#include "moto_lights/app_ws2812b.h"
#include "moto_lights/app_pwm_output.h"
#include "moto_lights/app_vehicle_signal.h"
#include "moto_lights/moto_lights_pinmap.h"

/*************** 功能说明 ****************

灯光效果调度器

功能:
1. 优先级管理（刹车 > 转向 > 位置灯 > 远近光 > 音频频谱 > 其他）
2. 场景渲染（10种场景）
3. 工作模式切换（白天/夜晚）
4. 全局亮度控制
5. 场景自动切换和超时管理

调度策略:
- 根据优先级选择最高优先级的激活场景
- 场景可设置持续时间，超时自动失效
- 支持场景叠加（不同输出设备）

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

SCHEDULER_STATE scheduler_state;
SCENE_INFO active_scenes[SCENE_COUNT];

// 外部时间戳
extern volatile u32 system_tick_ms;

//========================================================================
//                          场景优先级映射表
//========================================================================
static const u8 scene_priority_map[SCENE_COUNT] = {
    PRIORITY_IDLE,              // SCENE_IDLE
    PRIORITY_BRAKE,             // SCENE_BRAKE
    PRIORITY_TURN_SIGNAL,       // SCENE_TURN_LEFT
    PRIORITY_TURN_SIGNAL,       // SCENE_TURN_RIGHT
    PRIORITY_POSITION_LIGHT,    // SCENE_POSITION_LIGHT
    PRIORITY_HEADLIGHT,         // SCENE_NEAR_LIGHT
    PRIORITY_HEADLIGHT,         // SCENE_FAR_LIGHT
    PRIORITY_HORN_EFFECT,       // SCENE_HORN_EFFECT
    PRIORITY_AUDIO_SPECTRUM,    // SCENE_AUDIO_SPECTRUM
    100                         // SCENE_TEST_MODE (最高)
};

//========================================================================
//                          内部函数声明
//========================================================================
static void UpdateActiveScenes(void);
static void RenderCurrentScene(void);
static LIGHT_SCENE SelectHighestPriorityScene(void);

//========================================================================
// 函数: LightScheduler_Init
// 描述: 初始化灯光调度器
// 参数: None.
// 返回: None.
//========================================================================
void LightScheduler_Init(void)
{
    u8 i;
    
    // 初始化调度器状态
    scheduler_state.current_scene = SCENE_IDLE;
    scheduler_state.work_mode = MODE_DAY;
    scheduler_state.global_brightness = 255;
    scheduler_state.test_mode_active = 0;
    scheduler_state.scene_changed = 0;
    
    // 初始化场景信息
    for(i = 0; i < SCENE_COUNT; i++)
    {
        active_scenes[i].scene = (LIGHT_SCENE)i;
        active_scenes[i].priority = scene_priority_map[i];
        active_scenes[i].active = 0;
        active_scenes[i].start_time = 0;
        active_scenes[i].duration = 0;
    }
    
    // 激活空闲场景
    active_scenes[SCENE_IDLE].active = 1;
    
    printf("LightScheduler initialized\r\n");
}

//========================================================================
// 函数: Sample_LightScheduler
// 描述: 调度器主任务（每20ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_LightScheduler(void)
{
    LIGHT_SCENE new_scene;
    
    // 更新激活场景状态
    UpdateActiveScenes();
    
    // 选择最高优先级场景
    new_scene = SelectHighestPriorityScene();
    
    // 检测场景变化
    if(new_scene != scheduler_state.current_scene)
    {
        scheduler_state.current_scene = new_scene;
        scheduler_state.scene_changed = 1;
        
        printf("Scene changed to: %d\r\n", new_scene);
    }
    
    // 渲染当前场景
    RenderCurrentScene();
}

//========================================================================
// 函数: UpdateActiveScenes
// 描述: 更新激活场景状态（检查超时）
// 参数: None.
// 返回: None.
//========================================================================
static void UpdateActiveScenes(void)
{
    u8 i;
    
    for(i = 0; i < SCENE_COUNT; i++)
    {
        if(active_scenes[i].active && active_scenes[i].duration > 0)
        {
            // 检查是否超时
            u32 elapsed = system_tick_ms - active_scenes[i].start_time;
            if(elapsed >= active_scenes[i].duration)
            {
                // 场景超时，失效
                active_scenes[i].active = 0;
                printf("Scene %d timeout\r\n", i);
            }
        }
    }
    
    // 确保至少有一个场景激活（空闲场景）
    u8 any_active = 0;
    for(i = 0; i < SCENE_COUNT; i++)
    {
        if(active_scenes[i].active)
        {
            any_active = 1;
            break;
        }
    }
    
    if(!any_active)
    {
        active_scenes[SCENE_IDLE].active = 1;
    }
}

//========================================================================
// 函数: SelectHighestPriorityScene
// 描述: 选择最高优先级的激活场景
// 参数: None.
// 返回: 场景类型
//========================================================================
static LIGHT_SCENE SelectHighestPriorityScene(void)
{
    u8 i;
    u8 highest_priority = 0;
    LIGHT_SCENE selected_scene = SCENE_IDLE;
    
    for(i = 0; i < SCENE_COUNT; i++)
    {
        if(active_scenes[i].active && active_scenes[i].priority > highest_priority)
        {
            highest_priority = active_scenes[i].priority;
            selected_scene = active_scenes[i].scene;
        }
    }
    
    return selected_scene;
}

//========================================================================
// 函数: RenderCurrentScene
// 描述: 渲染当前场景
// 参数: None.
// 返回: None.
//========================================================================
static void RenderCurrentScene(void)
{
    switch(scheduler_state.current_scene)
    {
        case SCENE_IDLE:
            Scene_Render_Idle();
            break;
        case SCENE_BRAKE:
            Scene_Render_Brake();
            break;
        case SCENE_TURN_LEFT:
            Scene_Render_TurnLeft();
            break;
        case SCENE_TURN_RIGHT:
            Scene_Render_TurnRight();
            break;
        case SCENE_POSITION_LIGHT:
            Scene_Render_PositionLight();
            break;
        case SCENE_NEAR_LIGHT:
            Scene_Render_NearLight();
            break;
        case SCENE_FAR_LIGHT:
            Scene_Render_FarLight();
            break;
        case SCENE_HORN_EFFECT:
            Scene_Render_HornEffect();
            break;
        case SCENE_AUDIO_SPECTRUM:
            Scene_Render_AudioSpectrum();
            break;
        case SCENE_TEST_MODE:
            Scene_Render_TestMode();
            break;
        default:
            Scene_Render_Idle();
            break;
    }
    
    // 更新WS2812B显示（每次渲染后）
    WS2812B_Update();
}

//========================================================================
//                          场景控制函数实现
//========================================================================

void LightScheduler_ActivateScene(LIGHT_SCENE scene, u32 duration_ms)
{
    if(scene >= SCENE_COUNT) return;
    
    active_scenes[scene].active = 1;
    active_scenes[scene].start_time = system_tick_ms;
    active_scenes[scene].duration = duration_ms;
    
    printf("Scene %d activated (duration: %lu ms)\r\n", scene, duration_ms);
}

void LightScheduler_DeactivateScene(LIGHT_SCENE scene)
{
    if(scene >= SCENE_COUNT) return;
    
    active_scenes[scene].active = 0;
}

void LightScheduler_ClearAllScenes(void)
{
    u8 i;
    for(i = 0; i < SCENE_COUNT; i++)
    {
        active_scenes[i].active = 0;
    }
    
    // 激活空闲场景
    active_scenes[SCENE_IDLE].active = 1;
}

LIGHT_SCENE LightScheduler_GetActiveScene(void)
{
    return scheduler_state.current_scene;
}

void LightScheduler_SetWorkMode(WORK_MODE mode)
{
    scheduler_state.work_mode = mode;
    
    // 根据工作模式调整亮度
    if(mode == MODE_DAY)
    {
        WS2812B_SetBrightness(WS2812B_BRIGHTNESS_DAY);
    }
    else
    {
        WS2812B_SetBrightness(WS2812B_BRIGHTNESS_NIGHT);
    }
    
    printf("Work mode changed to: %s\r\n", (mode == MODE_DAY) ? "DAY" : "NIGHT");
}

void LightScheduler_SetGlobalBrightness(u8 brightness)
{
    scheduler_state.global_brightness = brightness;
    WS2812B_SetBrightness(brightness);
}

void LightScheduler_EnterTestMode(void)
{
    scheduler_state.test_mode_active = 1;
    LightScheduler_ActivateScene(SCENE_TEST_MODE, 0);  // 无限持续
}

void LightScheduler_ExitTestMode(void)
{
    scheduler_state.test_mode_active = 0;
    LightScheduler_DeactivateScene(SCENE_TEST_MODE);
}

//========================================================================
//                          场景渲染函数实现
//========================================================================

// 空闲场景
void Scene_Render_Idle(void)
{
    // 关闭所有灯光
    WS2812B_Clear();
    PWM_SetRGB(0, 0, 0);
    PWM_BrakeLight_Off();
}

// 刹车场景
void Scene_Render_Brake(void)
{
    // 尾箱刹车灯高亮
    PWM_BrakeLight_On(255);
    
    // 车身氛围灯后部高亮红色
    WS2812B_SetZone(ZONE_REAR, 255, 0, 0);
    
    // 共用RGB灯组红色
    PWM_SetRGB(255, 0, 0);
}

// 左转向场景
void Scene_Render_TurnLeft(void)
{
    static EFFECT_PARAM effect = {EFFECT_FLOWING, {255, 150, 0}, {0, 0, 0}, 255, 10, 0};
    
    // 左侧流水效果（橙色）
    WS2812B_Effect_Flowing(ZONE_FRONT_LEFT, &effect);
    WS2812B_Effect_Flowing(ZONE_BODY_LEFT, &effect);
    
    // 右侧保持暗
    WS2812B_SetZone(ZONE_FRONT_RIGHT, 0, 0, 0);
    WS2812B_SetZone(ZONE_BODY_RIGHT, 0, 0, 0);
}

// 右转向场景
void Scene_Render_TurnRight(void)
{
    static EFFECT_PARAM effect = {EFFECT_FLOWING, {255, 150, 0}, {0, 0, 0}, 255, 10, 0};
    
    // 右侧流水效果（橙色）
    WS2812B_Effect_Flowing(ZONE_FRONT_RIGHT, &effect);
    WS2812B_Effect_Flowing(ZONE_BODY_RIGHT, &effect);
    
    // 左侧保持暗
    WS2812B_SetZone(ZONE_FRONT_LEFT, 0, 0, 0);
    WS2812B_SetZone(ZONE_BODY_LEFT, 0, 0, 0);
}

// 位置灯场景
void Scene_Render_PositionLight(void)
{
    static u16 blink_counter = 0;
    
    // 所有灯低亮度
    WS2812B_SetAll(50, 50, 50);
    PWM_SetRGB(50, 50, 50);
    
    // 尾箱刹车灯2秒一次闪烁
    blink_counter++;
    if(blink_counter >= 100)  // 20ms * 100 = 2s
    {
        blink_counter = 0;
        PWM_BrakeLight_Flash(100, 200);
    }
}

// 近光灯场景
void Scene_Render_NearLight(void)
{
    // 整体中等亮度白色
    WS2812B_SetAll(128, 128, 128);
    PWM_SetRGB(128, 128, 128);
}

// 远光灯场景
void Scene_Render_FarLight(void)
{
    static EFFECT_PARAM effect = {EFFECT_FLASHING, {255, 255, 255}, {0, 0, 0}, 255, 5, 0};
    
    // 快闪白色
    WS2812B_Effect_Flashing(ZONE_ALL, &effect);
    PWM_RGB_Flashing(255, 255, 255, 5);
}

// 喇叭效果场景
void Scene_Render_HornEffect(void)
{
    static EFFECT_PARAM effect = {EFFECT_FLASHING, {255, 0, 0}, {0, 0, 0}, 255, 3, 0};
    
    // 前轮和车底RGB强提示效果（红色快闪）
    PWM_RGB_Flashing(255, 0, 0, 3);
    
    // 前部氛围灯快闪
    WS2812B_Effect_Flashing(ZONE_FRONT_LEFT, &effect);
    WS2812B_Effect_Flashing(ZONE_FRONT_RIGHT, &effect);
}

// 音频频谱场景
void Scene_Render_AudioSpectrum(void)
{
    // TODO: 实现音频频谱效果
    // 需要音频采样和FFT处理
    WS2812B_SetAll(0, 128, 255);  // 临时：蓝色
}

// 测试模式场景
void Scene_Render_TestMode(void)
{
    static u16 test_phase = 0;
    static EFFECT_PARAM rainbow_effect = {EFFECT_RAINBOW, {0, 0, 0}, {0, 0, 0}, 255, 5, 0};
    
    test_phase++;
    
    // 彩虹效果
    WS2812B_Effect_Rainbow(ZONE_ALL, &rainbow_effect);
    
    // RGB灯组循环变色
    u8 r = (test_phase >> 0) & 0xFF;
    u8 g = (test_phase >> 3) & 0xFF;
    u8 b = (test_phase >> 6) & 0xFF;
    PWM_SetRGB(r, g, b);
    
    // 刹车灯闪烁
    if((test_phase & 0x20) == 0)
        PWM_BrakeLight_On(255);
    else
        PWM_BrakeLight_Off();
    
    // 蜂鸣器短鸣（每2秒）
    if(test_phase % 100 == 0)
    {
        PWM_Buzzer_Beep(100);
    }
}

