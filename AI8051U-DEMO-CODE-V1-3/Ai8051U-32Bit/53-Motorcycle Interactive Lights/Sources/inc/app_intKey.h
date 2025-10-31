#ifndef __APP_INTKEY_H_
#define __APP_INTKEY_H_

#include "type_def.h"

//========================================================================
//                               按键状态定义
//========================================================================

#define KEY_STATE_IDLE      0  // 空闲状态
#define KEY_STATE_PRESS     1  // 按下状态
#define KEY_STATE_HOLD      2  // 长按状态
#define KEY_STATE_RELEASE   3  // 释放状态

//========================================================================
//                               按键事件定义
//========================================================================

#define KEY_EVENT_NONE       0  // 无事件
#define KEY_EVENT_SHORT      1  // 短按事件
#define KEY_EVENT_LONG       2  // 长按事件
#define KEY_EVENT_REPEAT     3  // 长按重复事件

//========================================================================
//                               按键编号定义
//========================================================================

#define KEY_INT0    1  // 对应INT0（P3.2）
#define KEY_INT1    2  // 对应INT1（P3.3）
#define KEY_INT2    3  // 对应INT2（P3.6）
#define KEY_INT3    4  // 对应INT3（P3.7）

//========================================================================
//                            事件结构体定义
//========================================================================

// 按键事件信息结构体（方案C）
typedef struct {
    u8  key_num;       // 按键编号: 1-4对应INT0-INT3
    u8  event_type;    // 事件类型: SHORT/LONG/REPEAT
    u16 hold_time;     // 按键持续时间(ms)
    u8  pin_state;     // 当前按键IO状态: 0=按下, 1=释放
    u32 timestamp;     // 事件发生时的时间戳(ms)
} KEY_EVENT;

// 按键事件回调函数类型定义
typedef void (*KeyEventCallback)(KEY_EVENT *event);

//========================================================================
//                            外部函数和变量声明
//========================================================================

// 兼容性变量（保留，供不使用回调的代码使用）
extern u8 intKeyCode;     // 按键码: 1-4对应INT0-INT3, 0表示无按键
extern u8 intKeyEvent;    // 按键事件类型
extern u8 intKeyState;    // 当前按键状态

// 时间戳支持（需要在Timer0中递增）
extern volatile u32 system_tick_ms;

// 初始化和任务函数
void intKey_init(void);
void Sample_intKey(void);

// 回调函数注册
void intKey_RegisterCallback(KeyEventCallback callback);

// 辅助函数
const char* intKey_GetEventName(u8 event_type);
const char* intKey_GetKeyName(u8 key_num);

#endif

