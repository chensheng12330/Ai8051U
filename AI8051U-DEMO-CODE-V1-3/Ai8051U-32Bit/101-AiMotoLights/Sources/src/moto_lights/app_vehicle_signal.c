/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - 原车信号检测模块 ---------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_vehicle_signal.h"
#include "moto_lights/moto_lights_pinmap.h"

/*************** 功能说明 ****************

原车信号检测模块

功能:
1. 检测7路原车信号输入（刹车、雾灯、近光、远光、左转、右转、喇叭）
2. 提供软件消抖（20ms）
3. 边沿检测（上升沿/下降沿）
4. 信号激活时间统计
5. 状态变化回调通知

信号特性:
- 所有信号为上升沿触发
- 高电平有效（SIGNAL_ACTIVE = 1）
- 支持长时间激活状态

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

// 信号状态数组
SIGNAL_STATE vehicle_signals[SIGNAL_COUNT];

// 信号变化回调函数指针
static SignalCallback signal_change_callback = NULL;

// 外部时间戳（从Timer0中断获取）
extern volatile u32 system_tick_ms;

//========================================================================
//                          内部函数声明
//========================================================================
static u8 ReadSignalPin(VEHICLE_SIGNAL_TYPE signal);
static void ProcessSignal(VEHICLE_SIGNAL_TYPE signal);

//========================================================================
// 函数: VehicleSignal_Init
// 描述: 初始化原车信号检测模块
// 参数: None.
// 返回: None.
//========================================================================
void VehicleSignal_Init(void)
{
    u8 i;
    
    // 初始化所有信号状态
    for(i = 0; i < SIGNAL_COUNT; i++)
    {
        vehicle_signals[i].current_state = 0;
        vehicle_signals[i].last_state = 0;
        vehicle_signals[i].debounce_count = 0;
        vehicle_signals[i].edge_detected = 0;
        vehicle_signals[i].active_time = 0;
        vehicle_signals[i].last_trigger_time = 0;
    }
    
    // GPIO已在System_init中配置，P0口默认为准双向口（可输入）
    // 确保P0口为输入模式（高阻或准双向）
    // P0M1 已配置，P0口作为输入
    
    printf("VehicleSignal module initialized\r\n");
}

//========================================================================
// 函数: Sample_VehicleSignal
// 描述: 原车信号扫描任务（每10ms调用）
// 参数: None.
// 返回: None.
//========================================================================
void Sample_VehicleSignal(void)
{
    u8 i;
    
    // 扫描所有信号
    for(i = 0; i < SIGNAL_COUNT; i++)
    {
        ProcessSignal((VEHICLE_SIGNAL_TYPE)i);
    }
}

//========================================================================
// 函数: ReadSignalPin
// 描述: 读取信号引脚状态
// 参数: signal - 信号类型
// 返回: 引脚状态(0/1)
//========================================================================
static u8 ReadSignalPin(VEHICLE_SIGNAL_TYPE signal)
{
    switch(signal)
    {
        case SIGNAL_BRAKE:      return PIN_IN_BRAKE;
        case SIGNAL_FOG_LIGHT:  return PIN_IN_FOG_LIGHT;
        case SIGNAL_NEAR_LIGHT: return PIN_IN_NEAR_LIGHT;
        case SIGNAL_FAR_LIGHT:  return PIN_IN_FAR_LIGHT;
        case SIGNAL_TURN_LEFT:  return PIN_IN_TURN_LEFT;
        case SIGNAL_TURN_RIGHT: return PIN_IN_TURN_RIGHT;
        case SIGNAL_HORN:       return 0;  // 喇叭信号通过音频检测，暂返回0
        default: return 0;
    }
}

//========================================================================
// 函数: ProcessSignal
// 描述: 处理单个信号的消抖和边沿检测
// 参数: signal - 信号类型
// 返回: None.
//========================================================================
static void ProcessSignal(VEHICLE_SIGNAL_TYPE signal)
{
    SIGNAL_STATE *sig = &vehicle_signals[signal];
    u8 pin_state = ReadSignalPin(signal);
    
    // 清除本次边沿检测标志
    sig->edge_detected = 0;
    
    // 消抖处理
    if(pin_state != sig->last_state)
    {
        // 状态变化，开始消抖
        sig->debounce_count++;
        
        if(sig->debounce_count >= (DEBOUNCE_TIME_MS / 10))  // 20ms / 10ms = 2次
        {
            // 消抖完成，确认状态变化
            sig->last_state = pin_state;
            sig->debounce_count = 0;
            
            // 检测边沿
            if(pin_state == SIGNAL_ACTIVE && sig->current_state == SIGNAL_INACTIVE)
            {
                // 上升沿
                sig->edge_detected = 1;
                sig->current_state = SIGNAL_ACTIVE;
                sig->active_time = 0;
                sig->last_trigger_time = system_tick_ms;
                
                // 触发回调
                if(signal_change_callback != NULL)
                {
                    signal_change_callback(signal, sig->current_state);
                }
            }
            else if(pin_state == SIGNAL_INACTIVE && sig->current_state == SIGNAL_ACTIVE)
            {
                // 下降沿
                sig->edge_detected = 2;
                sig->current_state = SIGNAL_INACTIVE;
                
                // 触发回调
                if(signal_change_callback != NULL)
                {
                    signal_change_callback(signal, sig->current_state);
                }
            }
        }
    }
    else
    {
        // 状态稳定，重置消抖计数器
        sig->debounce_count = 0;
    }
    
    // 更新激活时间
    if(sig->current_state == SIGNAL_ACTIVE)
    {
        sig->active_time += 10;  // 每10ms累加
    }
}

//========================================================================
// 函数: VehicleSignal_GetState
// 描述: 获取信号当前状态
// 参数: signal - 信号类型
// 返回: 当前状态(0/1)
//========================================================================
u8 VehicleSignal_GetState(VEHICLE_SIGNAL_TYPE signal)
{
    if(signal >= SIGNAL_COUNT) return 0;
    return vehicle_signals[signal].current_state;
}

//========================================================================
// 函数: VehicleSignal_IsRisingEdge
// 描述: 检测信号上升沿
// 参数: signal - 信号类型
// 返回: 1=上升沿, 0=无
//========================================================================
u8 VehicleSignal_IsRisingEdge(VEHICLE_SIGNAL_TYPE signal)
{
    if(signal >= SIGNAL_COUNT) return 0;
    return (vehicle_signals[signal].edge_detected == 1) ? 1 : 0;
}

//========================================================================
// 函数: VehicleSignal_IsFallingEdge
// 描述: 检测信号下降沿
// 参数: signal - 信号类型
// 返回: 1=下降沿, 0=无
//========================================================================
u8 VehicleSignal_IsFallingEdge(VEHICLE_SIGNAL_TYPE signal)
{
    if(signal >= SIGNAL_COUNT) return 0;
    return (vehicle_signals[signal].edge_detected == 2) ? 1 : 0;
}

//========================================================================
// 函数: VehicleSignal_GetActiveTime
// 描述: 获取信号激活时间
// 参数: signal - 信号类型
// 返回: 激活时间(ms)
//========================================================================
u16 VehicleSignal_GetActiveTime(VEHICLE_SIGNAL_TYPE signal)
{
    if(signal >= SIGNAL_COUNT) return 0;
    return vehicle_signals[signal].active_time;
}

//========================================================================
// 函数: VehicleSignal_ClearEdge
// 描述: 清除边沿检测标志
// 参数: signal - 信号类型
// 返回: None.
//========================================================================
void VehicleSignal_ClearEdge(VEHICLE_SIGNAL_TYPE signal)
{
    if(signal >= SIGNAL_COUNT) return;
    vehicle_signals[signal].edge_detected = 0;
}

//========================================================================
// 函数: VehicleSignal_RegisterCallback
// 描述: 注册信号状态变化回调函数
// 参数: callback - 回调函数指针
// 返回: None.
//========================================================================
void VehicleSignal_RegisterCallback(SignalCallback callback)
{
    signal_change_callback = callback;
    if(callback != NULL)
    {
        printf("VehicleSignal callback registered\r\n");
    }
}

