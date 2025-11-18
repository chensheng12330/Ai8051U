#ifndef __TASK_MONITOR_H_
#define __TASK_MONITOR_H_

#include "config.h"
/*============================================================================*/
/*                      任务监控插件 - 独立模块                                */
/*============================================================================*/
/* 
 * 版本: V1.0
 * 日期: 2025-10-21
 * 作者: AI Technical Analyst
 * 
 * 功能: 为任务调度系统提供性能监控功能
 * 
 * 特点:
 *   - 完全独立，不侵入原始代码
 *   - 通过钩子函数集成
 *   - 可配置监控级别
 *   - 可随时移除
 *   - 适用于任何基于任务调度的系统
 */

//========================================================================
//                              类型定义
//========================================================================

// 基础类型（如果项目没有，使用标准类型）
//#ifndef u8
//typedef unsigned char   u8;
//#endif

//#ifndef u16
//typedef unsigned int    u16;
//#endif

//#ifndef u32
//typedef unsigned long   u32;
//#endif




//========================================================================
//                            监控级别定义
//========================================================================

#define TASK_MONITOR_OFF    0  // 完全关闭
#define TASK_MONITOR_BASIC  1  // 基础监控：执行次数
#define TASK_MONITOR_TIME   2  // 时间监控：次数+时间+最大值
#define TASK_MONITOR_FULL   3  // 完整监控：TIME+超时检测

// 包含配置文件（用户自定义）
#include "TaskMonitor_Config.h"

// 默认配置（如果用户未定义）
#ifndef TASK_MONITOR_LEVEL
    #ifdef DEBUG
        #define TASK_MONITOR_LEVEL  TASK_MONITOR_TIME
    #else
        #define TASK_MONITOR_LEVEL  TASK_MONITOR_BASIC
    #endif
#endif

#ifndef TASK_MONITOR_MAX_TASKS
    #define TASK_MONITOR_MAX_TASKS  10  // 最多监控10个任务
#endif

//========================================================================
//                            数据结构
//========================================================================

// 任务监控数据结构
typedef struct {
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
    u32 exec_count;        // 执行次数
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
    u16 exec_time;         // 最近执行时间(μs)
    u16 max_exec_time;     // 历史最大时间(μs)
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
    u8  timeout_count;     // 超时次数
    u16 period;            // 任务周期(ms)
#endif
} TASK_MONITOR_DATA;

//========================================================================
//                            外部接口API
//========================================================================

// 初始化监控插件
void TaskMonitor_Init(void);

// 任务开始钩子（在任务执行前调用）
void TaskMonitor_TaskStart(u8 task_id);

// 任务结束钩子（在任务执行后调用）
void TaskMonitor_TaskEnd(u8 task_id);

// 设置任务周期（用于超时检测）
void TaskMonitor_SetPeriod(u8 task_id, u16 period_ms);

// 获取监控数据
TASK_MONITOR_DATA* TaskMonitor_GetData(u8 task_id);

// 打印性能报告
void TaskMonitor_PrintReport(void);

// 重置统计数据
void TaskMonitor_Reset(u8 task_id);
void TaskMonitor_ResetAll(void);

//========================================================================
//                            辅助宏定义
//========================================================================

#if (TASK_MONITOR_LEVEL > TASK_MONITOR_OFF)
    // 启用监控时的宏
    #define TASK_MONITOR_START(id)  TaskMonitor_TaskStart(id)
    #define TASK_MONITOR_END(id)    TaskMonitor_TaskEnd(id)
    #define TASK_MONITOR_REPORT()   TaskMonitor_PrintReport()
#else
    // 关闭监控时，宏定义为空（编译器会优化掉）
    #define TASK_MONITOR_START(id)  ((void)0)
    #define TASK_MONITOR_END(id)    ((void)0)
    #define TASK_MONITOR_REPORT()   ((void)0)
#endif

//========================================================================
//                            版本信息
//========================================================================

#define TASK_MONITOR_VERSION_MAJOR  1
#define TASK_MONITOR_VERSION_MINOR  0
#define TASK_MONITOR_VERSION_PATCH  0

const char* TaskMonitor_GetVersion(void);

#endif /* __TASK_MONITOR_H_ */

