#ifndef __CPU_MONITOR_H_
#define __CPU_MONITOR_H_

/*============================================================================*/
/*                      CPU占用率监控插件 - 独立模块                          */
/*============================================================================*/
/* 
 * 版本: V1.0
 * 日期: 2025-10-21
 * 作者: AI Technical Analyst
 * 
 * 功能: 实时监控CPU占用率
 * 
 * 原理:
 *   - 在主循环中计数空闲循环次数
 *   - 定期计算: CPU占用率 = (1 - 空闲时间/总时间) × 100%
 * 
 * 特点:
 *   - 完全独立，不依赖项目结构
 *   - 通过钩子函数集成
 *   - 可随时启用/禁用
 *   - 支持多种计算方式
 *   - Flash占用<200字节
 */

//========================================================================
//                              类型定义
//========================================================================

// 基础类型（如果项目没有，使用标准类型）
#ifndef _TYPE_DEF_DEFINED_
typedef unsigned char   u8;
typedef unsigned int    u16;
typedef unsigned long   u32;
#endif

//========================================================================
//                            包含配置文件
//========================================================================

#include "CPUMonitor_Config.h"

//========================================================================
//                            数据结构
//========================================================================

// CPU监控数据结构
typedef struct {
    u32 idle_count;        // 空闲循环计数
    u32 last_idle_count;   // 上次空闲计数
    u32 total_loops;       // 总循环次数
    u8  cpu_usage;         // CPU占用率(%)
    u8  cpu_usage_max;     // 历史最大CPU占用率
    u8  cpu_usage_min;     // 历史最小CPU占用率
    u32 sample_count;      // 采样次数
} CPU_MONITOR_DATA;

//========================================================================
//                            外部接口API
//========================================================================

// 初始化CPU监控
void CPUMonitor_Init(void);

// 空闲计数（在主循环中调用）
void CPUMonitor_IdleTick(void);

// 计算CPU占用率（定时调用，如每秒）
void CPUMonitor_Calculate(void);

// 获取CPU占用率
u8 CPUMonitor_GetUsage(void);

// 获取监控数据
CPU_MONITOR_DATA* CPUMonitor_GetData(void);

// 打印CPU使用报告
void CPUMonitor_PrintReport(void);

// 重置统计数据
void CPUMonitor_Reset(void);

//========================================================================
//                            辅助宏定义
//========================================================================

#if CPU_MONITOR_ENABLE
    // 启用监控时的宏
    #define CPU_MONITOR_IDLE_TICK()    CPUMonitor_IdleTick()
    #define CPU_MONITOR_CALCULATE()    CPUMonitor_Calculate()
    #define CPU_MONITOR_PRINT()        CPUMonitor_PrintReport()
#else
    // 关闭监控时，宏定义为空
    #define CPU_MONITOR_IDLE_TICK()    ((void)0)
    #define CPU_MONITOR_CALCULATE()    ((void)0)
    #define CPU_MONITOR_PRINT()        ((void)0)
#endif

//========================================================================
//                            版本信息
//========================================================================

#define CPU_MONITOR_VERSION_MAJOR  1
#define CPU_MONITOR_VERSION_MINOR  0
#define CPU_MONITOR_VERSION_PATCH  0

const char* CPUMonitor_GetVersion(void);

#endif /* __CPU_MONITOR_H_ */

