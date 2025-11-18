/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "app_intKey.h"

/*************** 功能说明 ****************

外部中断按键模块 (INT0/INT1/INT2/INT3)

支持功能:
1. 短按检测（按下并释放<500ms）
2. 长按检测（按下持续>500ms）
3. 长按重复（长按1秒后，每200ms触发一次）
4. 硬件消抖（下降沿中断）
5. 软件消抖（20ms确认）

按键接线:
  INT0 - P3.2 (按键1) - GND
  INT1 - P3.3 (按键2) - GND
  INT2 - P3.6 (按键3) - GND
  INT3 - P3.7 (按键4) - GND
  
注意: 按键需外接上拉电阻(10K)或使能IO内部上拉

******************************************/

//========================================================================
//                               全局变量
//========================================================================

// 全局时间戳（需要在Timer0中断中递增）
// 注意：需要在Timer_Isr.c中定义和递增
volatile u32 system_tick_ms = 0;

//========================================================================
//                               本地变量声明
//========================================================================

// 兼容性变量（保留）
u8  intKeyCode = 0;      // 按键码: 1-4对应INT0-INT3, 0表示无按键
u8  intKeyEvent = 0;     // 按键事件: 0=无, 1=短按, 2=长按, 3=重复
u8  intKeyState = 0;     // 当前状态: 0=空闲, 1=按下, 2=长按, 3=释放

// 内部变量
u8  Key_PressFlag = 0;   // 按键按下标志（中断中设置）
u8  Key_Which = 0;       // 哪个按键被按下（1-4）
u16 Key_HoldTime = 0;    // 按键持续时间（ms）
u8  Key_DebounceCount = 0; // 消抖计数器

// 回调函数指针（方案C）
KeyEventCallback key_event_callback = NULL;

//========================================================================
//                               本地常量定义
//========================================================================

#define KEY_DEBOUNCE_TIME    20    // 消抖时间: 20ms
#define KEY_LONG_PRESS_TIME  500   // 长按判定时间: 500ms
#define KEY_REPEAT_TIME      200   // 长按重复间隔: 200ms

//========================================================================
//                               本地函数声明
//========================================================================

void Default_KeyEventHandler(KEY_EVENT *event);

//========================================================================
// 函数: intKey_init
// 描述: 外部中断按键初始化
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-10-21
//========================================================================
void intKey_init(void)
{
    // 初始化变量
    intKeyCode = 0;
    intKeyEvent = 0;
    intKeyState = KEY_STATE_IDLE;
    Key_PressFlag = 0;
    Key_Which = 0;
    Key_HoldTime = 0;
    Key_DebounceCount = 0;
    
    //========================================================================
    // 配置外部中断（INT0/INT1/INT2/INT3）
    //========================================================================
    
    // 使能IO口内部上拉电阻（按键需要上拉）
    // P3M1 &= ~0xCC; P3M0 &= ~0xCC;  // 确保P3.2,3,6,7为准双向口模式（已在GPIO_config配置）
    
    // INT0配置（P3.2）
    IT0 = 1;   // INT0下降沿触发
    EX0 = 1;   // 使能INT0中断
    PX0 = 0;   // INT0优先级0（低，稍后在优化3中配置）
    
    // INT1配置（P3.3）
    IT1 = 1;   // INT1下降沿触发
    EX1 = 1;   // 使能INT1中断
    PX1 = 0;   // INT1优先级0（低）
    
    // INT2配置（P3.6）
    INTCLKO &= ~0x10;  // INT2下降沿触发
    INTCLKO |= 0x04;   // 使能INT2中断
    // INT2优先级在IP2中设置，保持默认
    
    // INT3配置（P3.7）
    INTCLKO &= ~0x20;  // INT3下降沿触发
    INTCLKO |= 0x08;   // 使能INT3中断
    // INT3优先级在IP2中设置，保持默认
}

//========================================================================
// 函数: INT0_ISR_Handler
// 描述: INT0外部中断服务程序
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-10-21
//========================================================================
void INT0_ISR_Handler(void) interrupt INT0_VECTOR
{
    // 下降沿触发，按键按下
    if(Key_PressFlag == 0)  // 避免重复触发
    {
        Key_PressFlag = 1;
        Key_Which = KEY_INT0;
        Key_HoldTime = 0;
        Key_DebounceCount = 0;
    }
}

//========================================================================
// 函数: INT1_ISR_Handler
// 描述: INT1外部中断服务程序
//========================================================================
void INT1_ISR_Handler(void) interrupt INT1_VECTOR
{
    if(Key_PressFlag == 0)
    {
        Key_PressFlag = 1;
        Key_Which = KEY_INT1;
        Key_HoldTime = 0;
        Key_DebounceCount = 0;
    }
}

//========================================================================
// 函数: INT2_ISR_Handler
// 描述: INT2外部中断服务程序
//========================================================================
void INT2_ISR_Handler(void) interrupt INT2_VECTOR
{
    if(Key_PressFlag == 0)
    {
        Key_PressFlag = 1;
        Key_Which = KEY_INT2;
        Key_HoldTime = 0;
        Key_DebounceCount = 0;
    }
}

//========================================================================
// 函数: INT3_ISR_Handler
// 描述: INT3外部中断服务程序
//========================================================================
void INT3_ISR_Handler(void) interrupt INT3_VECTOR
{
    if(Key_PressFlag == 0)
    {
        Key_PressFlag = 1;
        Key_Which = KEY_INT3;
        Key_HoldTime = 0;
        Key_DebounceCount = 0;
    }
}

//========================================================================
// 函数: Sample_intKey
// 描述: 外部中断按键处理任务（每10ms执行一次）
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-10-21
//========================================================================
void Sample_intKey(void)
{
    u8 key_pin_state = 1;  // 默认高电平（未按下状态）
    
    // 清除上次的事件
    intKeyEvent = KEY_EVENT_NONE;
    
    // 如果有按键按下标志
    if(Key_PressFlag)
    {
        // 读取按键当前状态（检测是否仍按下）
        switch(Key_Which)
        {
            case KEY_INT0: key_pin_state = P32; break;
            case KEY_INT1: key_pin_state = P33; break;
            case KEY_INT2: key_pin_state = P36; break;
            case KEY_INT3: key_pin_state = P37; break;
            default: key_pin_state = 1; break;
        }
        
        // 按键仍然按下（低电平）
        if(key_pin_state == 0)
        {
            // 消抖处理（需要连续20ms确认）
            if(Key_DebounceCount < (KEY_DEBOUNCE_TIME / 10))
            {
                Key_DebounceCount++;
                return;  // 消抖期间，不处理
            }
            
            // 消抖完成，确认按键有效
            if(intKeyState == KEY_STATE_IDLE)
            {
                // 第一次确认按下
                intKeyState = KEY_STATE_PRESS;
                intKeyCode = Key_Which;
                Key_HoldTime = 0;
            }
            else if(intKeyState == KEY_STATE_PRESS)
            {
                // 按键持续按下，累计时间
                Key_HoldTime += 10;  // 每10ms累加
                
                // 检测是否达到长按时间
                if(Key_HoldTime >= KEY_LONG_PRESS_TIME)
                {
                    intKeyState = KEY_STATE_HOLD;
                    intKeyEvent = KEY_EVENT_LONG;  // 触发长按事件
                    Key_HoldTime = 0;  // 重置，用于重复计时
                }
            }
            else if(intKeyState == KEY_STATE_HOLD)
            {
                // 长按状态，检测重复触发
                Key_HoldTime += 10;
                
                if(Key_HoldTime >= KEY_REPEAT_TIME)
                {
                    intKeyEvent = KEY_EVENT_REPEAT;  // 触发重复事件
                    Key_HoldTime = 0;  // 重置，继续重复
                }
            }
        }
        else  // 按键已释放（高电平）
        {
            // 按键释放处理
            if(intKeyState == KEY_STATE_PRESS)
            {
                // 短按：按下时间<500ms
                intKeyEvent = KEY_EVENT_SHORT;
                intKeyCode = Key_Which;
            }
            
            // 清除状态
            intKeyState = KEY_STATE_IDLE;
            Key_PressFlag = 0;
            Key_Which = 0;
            Key_HoldTime = 0;
            Key_DebounceCount = 0;
        }
    }
    
    //========================================================================
    // 方案C：事件结构体回调机制
    //========================================================================
    if(intKeyEvent != KEY_EVENT_NONE)
    {
        // 构建事件结构体
        KEY_EVENT event;
        event.key_num = intKeyCode;
        event.event_type = intKeyEvent;
        event.hold_time = Key_HoldTime;
        event.pin_state = key_pin_state;
        event.timestamp = system_tick_ms;
        
        // 调用用户注册的回调函数
        if(key_event_callback != NULL)
        {
            key_event_callback(&event);  // 触发回调
        }
        else
        {
            // 如果未注册回调，使用默认处理（兼容模式）
            Default_KeyEventHandler(&event);
        }
        
        // 清除按键码和事件
        intKeyCode = 0;
        intKeyEvent = KEY_EVENT_NONE;
    }
}

//========================================================================
// 函数: Default_KeyEventHandler
// 描述: 默认按键事件处理函数（示例）
// 参数: event - 事件信息指针
// 返回: None.
// 版本: V1.0, 2025-10-21
//========================================================================
void Default_KeyEventHandler(KEY_EVENT *event)
{
    //========================================================================
    // 默认处理示例（未注册回调时使用）
    // 用户可以修改此函数，或注册自己的回调函数
    //========================================================================
    
    if(event->event_type == KEY_EVENT_SHORT)
    {
        // 短按事件
        printf("[Default] Key%d Short Press (held %dms)\r\n", 
               event->key_num, event->hold_time);
        
        switch(event->key_num)
        {
            case KEY_INT0:  // 按键1短按
                printf("  -> Key1 function\r\n");
                // TODO: 添加您的功能
                break;
                
            case KEY_INT1:  // 按键2短按
                printf("  -> Key2 function\r\n");
                break;
                
            case KEY_INT2:  // 按键3短按
                printf("  -> Key3 function\r\n");
                break;
                
            case KEY_INT3:  // 按键4短按
                printf("  -> Key4 function\r\n");
                break;
        }
    }
    else if(event->event_type == KEY_EVENT_LONG)
    {
        // 长按事件（首次触发）
        printf("[Default] Key%d Long Press START\r\n", event->key_num);
        
        // TODO: 添加长按功能
    }
    else if(event->event_type == KEY_EVENT_REPEAT)
    {
        // 长按重复事件
        printf("[Default] Key%d REPEAT (held %dms)\r\n", 
               event->key_num, event->hold_time);
        
        // TODO: 添加重复功能
    }
}

//========================================================================
// 函数: intKey_RegisterCallback
// 描述: 注册按键事件回调函数
// 参数: callback - 回调函数指针
// 返回: None.
// 版本: V1.0, 2025-10-21
//========================================================================
void intKey_RegisterCallback(KeyEventCallback callback)
{
    key_event_callback = callback;
    
    if(callback != NULL)
    {
        printf("Key callback registered\r\n");
    }
    else
    {
        printf("Key callback unregistered, using default handler\r\n");
    }
}

//========================================================================
// 函数: intKey_GetEventName
// 描述: 获取事件类型名称（辅助函数）
// 参数: event_type - 事件类型
// 返回: 事件名称字符串
// 版本: V1.0, 2025-10-21
//========================================================================
const char* intKey_GetEventName(u8 event_type)
{
    switch(event_type)
    {
        case KEY_EVENT_NONE:   return "NONE";
        case KEY_EVENT_SHORT:  return "SHORT";
        case KEY_EVENT_LONG:   return "LONG";
        case KEY_EVENT_REPEAT: return "REPEAT";
        default:               return "UNKNOWN";
    }
}

//========================================================================
// 函数: intKey_GetKeyName
// 描述: 获取按键名称（辅助函数）
// 参数: key_num - 按键编号
// 返回: 按键名称字符串
// 版本: V1.0, 2025-10-21
//========================================================================
const char* intKey_GetKeyName(u8 key_num)
{
    switch(key_num)
    {
        case KEY_INT0: return "KEY1(INT0)";
        case KEY_INT1: return "KEY2(INT1)";
        case KEY_INT2: return "KEY3(INT2)";
        case KEY_INT3: return "KEY4(INT3)";
        default:       return "UNKNOWN";
    }
}

