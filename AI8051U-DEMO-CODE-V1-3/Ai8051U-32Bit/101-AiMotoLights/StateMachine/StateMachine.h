#ifndef __STATE_MACHINE_H_
#define __STATE_MACHINE_H_

/*============================================================================*/
/*                      状态机辅助工具 - 独立模块                              */
/*============================================================================*/
/* 
 * 版本: V1.0
 * 日期: 2025-10-21
 * 作者: AI Technical Analyst
 * 
 * 功能: 简化状态机编写，提供时间延迟、状态转换等辅助功能
 * 
 * 特点:
 *   - 轻量级宏定义，无函数调用开销
 *   - 基于全局1ms时基（system_tick_ms）
 *   - 不强制使用，仅作为辅助工具
 *   - Flash占用: 0字节（仅宏定义）
 *   - 易于理解和使用
 * 
 * 依赖:
 *   - 需要全局时间戳: system_tick_ms (volatile u32)
 *   - 在Timer0中断中每1ms递增
 */

//========================================================================
//                              类型定义
//========================================================================

#ifndef _TYPE_DEF_DEFINED_
typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;
typedef unsigned char   bit;
#endif

//========================================================================
//                          全局时间戳声明
//========================================================================

// 全局毫秒计数器（需要在Timer0中断中递增）
// 如果使用ENABLE_INT_KEY，已在app_intKey.c中定义
// 否则需要在项目中定义
extern volatile u32 system_tick_ms;

//========================================================================
//                          时间延迟辅助宏
//========================================================================

// 获取当前时间戳
#define SM_GET_TICK()          (system_tick_ms)

// 计算时间差（自动处理溢出）
#define SM_ELAPSED(start_tick) (system_tick_ms - (start_tick))

// 检查延时是否到期
#define SM_DELAY_DONE(start_tick, delay_ms) \
    (SM_ELAPSED(start_tick) >= (delay_ms))

// 开始延时（记录起始时间戳）
#define SM_DELAY_START(timestamp_var) \
    do { timestamp_var = SM_GET_TICK(); } while(0)

// 检查延时
#define SM_DELAY_CHECK(timestamp_var, delay_ms) \
    SM_DELAY_DONE(timestamp_var, delay_ms)

//========================================================================
//                          状态定义辅助宏
//========================================================================

// 定义状态枚举（可选，使代码更清晰）
#define SM_STATE_ENUM(name) \
    enum name##_States

// 状态定义示例：
// SM_STATE_ENUM(MySensor) {
//     STATE_IDLE = 0,
//     STATE_INIT,
//     STATE_WAIT,
//     STATE_READ,
//     STATE_DONE
// };

//========================================================================
//                          状态转换辅助宏
//========================================================================

// 状态转换（带可选动作）
#define SM_GOTO_STATE(state_var, new_state) \
    do { state_var = new_state; } while(0)

// 状态转换并记录时间戳
#define SM_GOTO_STATE_WITH_DELAY(state_var, new_state, timestamp_var) \
    do { \
        state_var = new_state; \
        timestamp_var = SM_GET_TICK(); \
    } while(0)

//========================================================================
//                          超时检测辅助宏
//========================================================================

// 超时检测（返回1表示超时）
#define SM_TIMEOUT(start_tick, timeout_ms) \
    (SM_ELAPSED(start_tick) > (timeout_ms))

// 带超时的延时检查
#define SM_DELAY_WITH_TIMEOUT(start_tick, delay_ms, timeout_ms, timeout_flag) \
    do { \
        if(SM_TIMEOUT(start_tick, timeout_ms)) { \
            timeout_flag = 1; \
        } \
    } while(0)

//========================================================================
//                          状态机调试辅助
//========================================================================

#ifdef STATE_MACHINE_DEBUG
    #include <stdio.h>
    
    // 状态转换打印
    #define SM_DEBUG_STATE(state_name) \
        printf("[SM] State: %s\r\n", #state_name)
    
    // 延时打印
    #define SM_DEBUG_DELAY(delay_ms) \
        printf("[SM] Delay: %ums\r\n", delay_ms)
    
    // 超时打印
    #define SM_DEBUG_TIMEOUT() \
        printf("[SM] Timeout!\r\n")
#else
    #define SM_DEBUG_STATE(state_name)  ((void)0)
    #define SM_DEBUG_DELAY(delay_ms)    ((void)0)
    #define SM_DEBUG_TIMEOUT()          ((void)0)
#endif

//========================================================================
//                          计数器辅助宏
//========================================================================

// 基于任务周期的计数延时（适合粗延时）
#define SM_COUNT_DELAY_START(counter_var) \
    do { counter_var = 0; } while(0)

#define SM_COUNT_DELAY_CHECK(counter_var, count_max) \
    ((++counter_var) >= (count_max))

//========================================================================
//                          常用状态机模板
//========================================================================

/* 
 * 模板1：简单延时状态机
 * 
 * void MyTask(void)
 * {
 *     static u8 state = 0;
 *     static u32 timestamp = 0;
 *     
 *     switch(state)
 *     {
 *         case 0:  // 初始化
 *             init_something();
 *             SM_DELAY_START(timestamp);
 *             SM_GOTO_STATE(state, 1);
 *             break;
 *             
 *         case 1:  // 等待100ms
 *             if(SM_DELAY_CHECK(timestamp, 100))
 *             {
 *                 SM_GOTO_STATE(state, 2);
 *             }
 *             break;
 *             
 *         case 2:  // 读取数据
 *             read_data();
 *             SM_GOTO_STATE(state, 0);
 *             break;
 *     }
 * }
 * 
 * 模板2：带超时检测
 * 
 * void MyTask_WithTimeout(void)
 * {
 *     static u8 state = 0;
 *     static u32 timestamp = 0;
 *     static bit timeout_flag = 0;
 *     
 *     switch(state)
 *     {
 *         case 0:
 *             send_command();
 *             SM_DELAY_START(timestamp);
 *             timeout_flag = 0;
 *             SM_GOTO_STATE(state, 1);
 *             break;
 *             
 *         case 1:  // 等待响应，最长500ms
 *             if(data_ready())
 *             {
 *                 SM_GOTO_STATE(state, 2);
 *             }
 *             else if(SM_TIMEOUT(timestamp, 500))
 *             {
 *                 SM_DEBUG_TIMEOUT();
 *                 SM_GOTO_STATE(state, 3);  // 超时处理
 *             }
 *             break;
 *             
 *         case 2:  // 正常处理
 *             process_data();
 *             SM_GOTO_STATE(state, 0);
 *             break;
 *             
 *         case 3:  // 超时处理
 *             handle_timeout();
 *             SM_GOTO_STATE(state, 0);
 *             break;
 *     }
 * }
 * 
 * 模板3：基于计数的简单延时
 * 
 * void MyTask_CountDelay(void)  // 假设10ms周期
 * {
 *     static u8 state = 0;
 *     static u8 delay_count = 0;
 *     
 *     switch(state)
 *     {
 *         case 0:
 *             init();
 *             SM_COUNT_DELAY_START(delay_count);
 *             SM_GOTO_STATE(state, 1);
 *             break;
 *             
 *         case 1:  // 等待100ms = 10次×10ms
 *             if(SM_COUNT_DELAY_CHECK(delay_count, 10))
 *             {
 *                 SM_GOTO_STATE(state, 2);
 *             }
 *             break;
 *             
 *         case 2:
 *             process();
 *             SM_GOTO_STATE(state, 0);
 *             break;
 *     }
 * }
 */

//========================================================================
//                          版本信息
//========================================================================

#define STATE_MACHINE_VERSION_MAJOR  1
#define STATE_MACHINE_VERSION_MINOR  0
#define STATE_MACHINE_VERSION_PATCH  0

#define STATE_MACHINE_VERSION  "StateMachine v1.0.0"

#endif /* __STATE_MACHINE_H_ */

