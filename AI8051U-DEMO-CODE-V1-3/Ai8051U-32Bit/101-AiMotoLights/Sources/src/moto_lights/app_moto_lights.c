/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 主应用层 -----------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_moto_lights.h"
#include "moto_lights/app_vehicle_signal.h"
#include "moto_lights/app_ws2812b.h"
#include "moto_lights/app_pwm_output.h"
#include "moto_lights/app_light_scheduler.h"
#include "moto_lights/app_sensors.h"
#include "app_intKey.h"
#include "moto_lights/moto_lights_pinmap.h"

/*************** 功能说明 ****************

摩托车智能联动灯组系统 - 主应用层

功能:
1. 系统初始化和配置
2. 各模块协调管理
3. 事件处理（按键、信号）
4. 系统状态监控
5. 错误处理和恢复

设计架构:
- 事件驱动：按键和信号通过回调触发
- 优先级调度：场景按优先级自动切换
- 模块化设计：各模块独立运行，通过接口交互

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

SYSTEM_INFO system_info;

// 外部时间戳
extern volatile u32 system_tick_ms;

//========================================================================
//                          内部变量
//========================================================================

// 运行时间计数
static u32 last_second_tick = 0;

// 模式检测计数
static u16 mode_check_counter = 0;

//========================================================================
//                          内部函数声明
//========================================================================
static void UpdateSystemState(void);
static void ProcessVehicleSignals(void);
static void UpdateWorkMode(void);

//========================================================================
// 函数: MotoLights_Init
// 描述: 系统初始化
// 参数: None.
// 返回: None.
//========================================================================
void MotoLights_Init(void)
{
    // 初始化系统信息
    system_info.system_state = SYSTEM_STATE_INIT;
    system_info.error_code = 0;
    system_info.run_time_seconds = 0;
    system_info.status_led_state = 0;
    system_info.status_led_blink_period = STATUS_LED_BLINK_SLOW;
    
    printf("\r\n========================================\r\n");
    printf("Motorcycle Smart Light System\r\n");
    printf("Version: 1.0\r\n");
    printf("========================================\r\n\r\n");
    
    // 初始化各模块（顺序很重要）
    printf("Initializing modules...\r\n");
    
    // 1. 初始化传感器模块
    Sensors_Init();
    
    // 2. 初始化原车信号检测模块
    VehicleSignal_Init();
    
    // 3. 初始化WS2812B模块
    WS2812B_Init();
    
    // 4. 初始化PWM输出模块
    PWM_Output_Init();
    
    // 5. 初始化灯光调度器
    LightScheduler_Init();
    
    // 6. 初始化外部中断按键模块
    intKey_init();
    
    // 注册事件回调
    printf("Registering callbacks...\r\n");
    intKey_RegisterCallback(MotoLights_KeyEventHandler);
    VehicleSignal_RegisterCallback(MotoLights_SignalEventHandler);
    
    // 系统初始化完成
    system_info.system_state = SYSTEM_STATE_NORMAL;
    printf("\r\nSystem initialized successfully!\r\n");
    printf("System state: NORMAL\r\n\r\n");
    
    // 状态指示灯：蓝色常亮
    PIN_LED_STATUS_OK = LED_ON;
    PIN_LED_STATUS_ERR = LED_OFF;
    
    // 开机提示音
    PWM_Buzzer_Pattern(3, 100, 100);
}

//========================================================================
// 函数: Sample_MotoLights_App
// 描述: 主应用任务（每50ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_MotoLights_App(void)
{
    // 更新系统状态
    UpdateSystemState();
    
    // 处理原车信号
    ProcessVehicleSignals();
    
    // 更新工作模式（每500ms检查一次）
    mode_check_counter++;
    if(mode_check_counter >= 10)  // 50ms * 10 = 500ms
    {
        mode_check_counter = 0;
        UpdateWorkMode();
    }
    
    // 检查电池电压
    if(Sensors_IsBatteryLow())
    {
        if(system_info.system_state != SYSTEM_STATE_LOW_POWER)
        {
            printf("[WARNING] Battery voltage low: %.2fV\r\n", Sensors_GetBatteryVoltage());
            system_info.system_state = SYSTEM_STATE_LOW_POWER;
            system_info.status_led_blink_period = STATUS_LED_BLINK_FAST;
        }
    }
}

//========================================================================
// 函数: UpdateSystemState
// 描述: 更新系统状态
// 参数: None.
// 返回: None.
//========================================================================
static void UpdateSystemState(void)
{
    // 更新运行时间
    if(system_tick_ms - last_second_tick >= 1000)
    {
        last_second_tick = system_tick_ms;
        system_info.run_time_seconds++;
        
        // 每60秒打印一次系统状态
        if(system_info.run_time_seconds % 60 == 0)
        {
            printf("[INFO] System running: %lu seconds\r\n", system_info.run_time_seconds);
            printf("  Battery: %.2fV, Light: %d, Scene: %d\r\n", 
                   Sensors_GetBatteryVoltage(),
                   Sensors_GetLightIntensity(),
                   LightScheduler_GetActiveScene());
        }
    }
}

//========================================================================
// 函数: ProcessVehicleSignals
// 描述: 处理原车信号（由回调触发）
// 参数: None.
// 返回: None.
//========================================================================
static void ProcessVehicleSignals(void)
{
    // 信号处理已由回调函数MotoLights_SignalEventHandler完成
    // 这里可以添加额外的逻辑处理
}

//========================================================================
// 函数: UpdateWorkMode
// 描述: 更新工作模式（白天/夜晚）
// 参数: None.
// 返回: None.
//========================================================================
static void UpdateWorkMode(void)
{
    static WORK_MODE last_mode = MODE_DAY;
    WORK_MODE current_mode = Sensors_GetLightMode();
    
    if(current_mode != last_mode)
    {
        LightScheduler_SetWorkMode(current_mode);
        last_mode = current_mode;
    }
}

//========================================================================
// 函数: MotoLights_KeyEventHandler
// 描述: 按键事件处理回调
// 参数: event - 按键事件指针
// 返回: None.
//========================================================================
void MotoLights_KeyEventHandler(KEY_EVENT *event)
{
    printf("[KEY] %s %s\r\n", 
           intKey_GetKeyName(event->key_num),
           intKey_GetEventName(event->event_type));
    
    switch(event->key_num)
    {
        case KEY_MENU:  // 菜单按键 (INT0, P3.7)
            if(event->event_type == KEY_EVENT_SHORT)
            {
                // 短按：进入/退出测试模式
                if(system_info.system_state == SYSTEM_STATE_TEST)
                {
                    LightScheduler_ExitTestMode();
                    system_info.system_state = SYSTEM_STATE_NORMAL;
                    printf("[APP] Exit test mode\r\n");
                }
                else
                {
                    LightScheduler_EnterTestMode();
                    system_info.system_state = SYSTEM_STATE_TEST;
                    printf("[APP] Enter test mode\r\n");
                }
            }
            break;
            
        case KEY_TEST:  // 测试按键 (INT2, P3.6)
            if(event->event_type == KEY_EVENT_SHORT)
            {
                // 短按：测试所有灯光
                printf("[APP] Testing all lights\r\n");
                MotoLights_Test_AllLights();
            }
            else if(event->event_type == KEY_EVENT_LONG)
            {
                // 长按：顺序测试
                printf("[APP] Testing sequence\r\n");
                MotoLights_Test_Sequence();
            }
            break;
            
        case KEY_BRIGHTNESS:  // 亮度按键 (INT3, P3.2)
            if(event->event_type == KEY_EVENT_SHORT)
            {
                // 短按：循环调节亮度
                static u8 brightness_level = 2;
                brightness_level++;
                if(brightness_level > 3) brightness_level = 1;
                
                u8 brightness = (brightness_level == 1) ? 85 : 
                               (brightness_level == 2) ? 170 : 255;
                
                LightScheduler_SetGlobalBrightness(brightness);
                printf("[APP] Brightness set to %d%%\r\n", (brightness * 100) / 255);
                
                // 提示音
                PWM_Buzzer_Beep(50);
            }
            break;
    }
}

//========================================================================
// 函数: MotoLights_SignalEventHandler
// 描述: 原车信号事件处理回调
// 参数: signal - 信号类型, new_state - 新状态
// 返回: None.
//========================================================================
void MotoLights_SignalEventHandler(VEHICLE_SIGNAL_TYPE signal, u8 new_state)
{
    const char *signal_names[] = {
        "BRAKE", "FOG", "NEAR", "FAR", "TURN_L", "TURN_R", "HORN"
    };
    
    printf("[SIGNAL] %s: %s\r\n", signal_names[signal], new_state ? "ON" : "OFF");
    
    // 根据信号类型激活/失效场景
    switch(signal)
    {
        case SIGNAL_BRAKE:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_BRAKE, 0);  // 持续激活
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_BRAKE);
            }
            break;
            
        case SIGNAL_TURN_LEFT:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_TURN_LEFT, 0);
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_TURN_LEFT);
            }
            break;
            
        case SIGNAL_TURN_RIGHT:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_TURN_RIGHT, 0);
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_TURN_RIGHT);
            }
            break;
            
        case SIGNAL_FOG_LIGHT:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_POSITION_LIGHT, 0);
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_POSITION_LIGHT);
            }
            break;
            
        case SIGNAL_NEAR_LIGHT:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_NEAR_LIGHT, 0);
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_NEAR_LIGHT);
            }
            break;
            
        case SIGNAL_FAR_LIGHT:
            if(new_state == SIGNAL_ACTIVE)
            {
                LightScheduler_ActivateScene(SCENE_FAR_LIGHT, 0);
            }
            else
            {
                LightScheduler_DeactivateScene(SCENE_FAR_LIGHT);
            }
            break;
            
        case SIGNAL_HORN:
            if(new_state == SIGNAL_ACTIVE)
            {
                // 喇叭效果持续3秒
                LightScheduler_ActivateScene(SCENE_HORN_EFFECT, HORN_EFFECT_DURATION_MS);
            }
            break;
    }
}

//========================================================================
// 函数: Sample_StatusIndicator
// 描述: 系统状态指示任务（每100ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_StatusIndicator(void)
{
    static u16 blink_counter = 0;
    
    // 状态LED闪烁
    blink_counter += 100;  // 每次增加100ms
    if(blink_counter >= system_info.status_led_blink_period)
    {
        blink_counter = 0;
        system_info.status_led_state = !system_info.status_led_state;
        
        // 根据系统状态控制LED
        if(system_info.system_state == SYSTEM_STATE_NORMAL)
        {
            PIN_LED_STATUS_OK = LED_ON;   // 蓝色常亮
            PIN_LED_STATUS_ERR = LED_OFF;
        }
        else if(system_info.system_state == SYSTEM_STATE_ERROR)
        {
            PIN_LED_STATUS_OK = LED_OFF;
            PIN_LED_STATUS_ERR = system_info.status_led_state;  // 红色闪烁
        }
        else if(system_info.system_state == SYSTEM_STATE_LOW_POWER)
        {
            PIN_LED_STATUS_OK = system_info.status_led_state;   // 蓝色快闪
            PIN_LED_STATUS_ERR = LED_OFF;
        }
        else if(system_info.system_state == SYSTEM_STATE_TEST)
        {
            PIN_LED_STATUS_OK = system_info.status_led_state;   // 蓝色慢闪
            PIN_LED_STATUS_ERR = system_info.status_led_state;  // 红色慢闪
        }
    }
}

//========================================================================
//                          错误处理函数
//========================================================================

void MotoLights_SetError(u8 error_code)
{
    system_info.error_code = error_code;
    system_info.system_state = SYSTEM_STATE_ERROR;
    system_info.status_led_blink_period = STATUS_LED_BLINK_FAST;
    
    printf("[ERROR] System error: code %d\r\n", error_code);
}

void MotoLights_ClearError(void)
{
    system_info.error_code = 0;
    system_info.system_state = SYSTEM_STATE_NORMAL;
    system_info.status_led_blink_period = STATUS_LED_BLINK_SLOW;
    
    printf("[INFO] Error cleared\r\n");
}

//========================================================================
//                          测试函数
//========================================================================

void MotoLights_Test_AllLights(void)
{
    printf("Testing all lights...\r\n");
    
    // WS2812B全白
    WS2812B_SetAll(255, 255, 255);
    WS2812B_Update();
    
    // RGB灯全亮
    PWM_SetRGB(255, 255, 255);
    
    // 刹车灯亮
    PWM_BrakeLight_On(255);
    
    // 蜂鸣器响
    PWM_Buzzer_Beep(200);
    
    printf("Test completed\r\n");
}

void MotoLights_Test_Sequence(void)
{
    printf("Testing sequence...\r\n");
    
    // 测试各个区域
    u8 zones[] = {ZONE_FRONT_LEFT, ZONE_FRONT_RIGHT, ZONE_BODY_LEFT, 
                  ZONE_BODY_RIGHT, ZONE_REAR};
    u8 i;
    
    for(i = 0; i < 5; i++)
    {
        WS2812B_Clear();
        WS2812B_SetZone(zones[i], 255, 0, 0);
        WS2812B_Update();
        // 需要延时函数，这里省略
    }
    
    printf("Sequence test completed\r\n");
}

