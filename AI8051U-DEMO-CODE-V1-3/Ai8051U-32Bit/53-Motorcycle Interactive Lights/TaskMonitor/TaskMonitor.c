/*============================================================================*/
/*                      任务监控插件 - 实现文件                                */
/*============================================================================*/

#include "TaskMonitor.h"

//========================================================================
//                          本地变量
//========================================================================

// 监控数据数组
static TASK_MONITOR_DATA monitor_data[TASK_MONITOR_MAX_TASKS];

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
// 开始时间记录（每个任务）
static u16 task_start_time[TASK_MONITOR_MAX_TASKS];
#endif

#if ENABLE_SAMPLE_MONITOR
// 采样计数器
static u8 sample_counter = 0;
#endif

//========================================================================
//                          内部函数
//========================================================================

// 读取Timer0当前值
static u16 ReadTimer0(void)
{
#if (TASK_MONITOR_TIME_SOURCE == TIME_SOURCE_TIMER0)
    // 读取Timer0计数器（需要原子操作）
    u8 th, tl;
    
    // 读取两次确保一致性
    do {
        th = TH0;
        tl = TL0;
    } while(th != TH0);
    
    return ((u16)th << 8) | tl;
#else
    return 0;
#endif
}

//========================================================================
// 函数: TaskMonitor_Init
// 描述: 初始化任务监控插件
//========================================================================
void TaskMonitor_Init(void)
{
    u8 i;
    
    // 清零所有监控数据
    for(i=0; i<TASK_MONITOR_MAX_TASKS; i++)
    {
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
        monitor_data[i].exec_count = 0;
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
        monitor_data[i].exec_time = 0;
        monitor_data[i].max_exec_time = 0;
        task_start_time[i] = 0;
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
        monitor_data[i].timeout_count = 0;
        monitor_data[i].period = 0;
#endif
    }
    
#if ENABLE_MONITOR_DEBUG
    MONITOR_PRINTF("TaskMonitor initialized, Level=%d\r\n", TASK_MONITOR_LEVEL);
#endif
}

//========================================================================
// 函数: TaskMonitor_TaskStart
// 描述: 任务开始钩子（在任务执行前调用）
// 参数: task_id - 任务ID (0-9)
//========================================================================
void TaskMonitor_TaskStart(u8 task_id)
{
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
    if(task_id < TASK_MONITOR_MAX_TASKS)
    {
#if ENABLE_SAMPLE_MONITOR
        // 采样监控：每N次监控一次
        if(sample_counter == 0)
        {
            task_start_time[task_id] = ReadTimer0();
        }
#else
        // 每次都监控
        task_start_time[task_id] = ReadTimer0();
#endif
    }
#endif
}

//========================================================================
// 函数: TaskMonitor_TaskEnd
// 描述: 任务结束钩子（在任务执行后调用）
// 参数: task_id - 任务ID (0-9)
//========================================================================
void TaskMonitor_TaskEnd(u8 task_id)
{
    if(task_id >= TASK_MONITOR_MAX_TASKS)
        return;
    
#if ENABLE_SAMPLE_MONITOR
    u8 do_monitor;
    
    sample_counter++;
    do_monitor = (sample_counter >= SAMPLE_RATE);
    if(do_monitor)
        sample_counter = 0;
#else
    #define do_monitor  1  // 总是监控
#endif
    
    //====================================================================
    // BASIC级别：统计执行次数
    //====================================================================
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
    monitor_data[task_id].exec_count++;
#endif

    //====================================================================
    // TIME级别：记录执行时间
    //====================================================================
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
    if(do_monitor)
    {
        u16 end_time = ReadTimer0();
        u16 exec_time = end_time - task_start_time[task_id];
        
        monitor_data[task_id].exec_time = exec_time;
        
        // 更新最大执行时间
        if(exec_time > monitor_data[task_id].max_exec_time)
        {
            monitor_data[task_id].max_exec_time = exec_time;
        }
    }
#endif

    //====================================================================
    // FULL级别：超时检测
    //====================================================================
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
    if(do_monitor && monitor_data[task_id].period > 0)
    {
        u16 exec_time = monitor_data[task_id].exec_time;
        u16 threshold = monitor_data[task_id].period * TASK_MONITOR_TIMEOUT_THRESHOLD / 100;
        
        if(exec_time > threshold)
        {
            monitor_data[task_id].timeout_count++;
            
#if ENABLE_TIMEOUT_WARNING
            MONITOR_PRINTF("WARN: Task%d timeout! %u/%u us\r\n", 
                          task_id, exec_time, monitor_data[task_id].period);
#endif
        }
    }
#endif

#if ENABLE_SAMPLE_MONITOR
    #undef do_monitor
#endif
}

//========================================================================
// 函数: TaskMonitor_SetPeriod
// 描述: 设置任务周期（用于超时检测）
//========================================================================
void TaskMonitor_SetPeriod(u8 task_id, u16 period_ms)
{
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
    if(task_id < TASK_MONITOR_MAX_TASKS)
    {
        monitor_data[task_id].period = period_ms;
    }
#endif
}

//========================================================================
// 函数: TaskMonitor_GetData
// 描述: 获取任务的监控数据
//========================================================================
TASK_MONITOR_DATA* TaskMonitor_GetData(u8 task_id)
{
    if(task_id < TASK_MONITOR_MAX_TASKS)
    {
        return &monitor_data[task_id];
    }
    return (TASK_MONITOR_DATA*)0;
}

//========================================================================
// 函数: TaskMonitor_Reset
// 描述: 重置指定任务的监控数据
//========================================================================
void TaskMonitor_Reset(u8 task_id)
{
    if(task_id < TASK_MONITOR_MAX_TASKS)
    {
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
        monitor_data[task_id].exec_count = 0;
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
        monitor_data[task_id].exec_time = 0;
        monitor_data[task_id].max_exec_time = 0;
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
        monitor_data[task_id].timeout_count = 0;
#endif
    }
}

//========================================================================
// 函数: TaskMonitor_ResetAll
// 描述: 重置所有任务的监控数据
//========================================================================
void TaskMonitor_ResetAll(void)
{
    u8 i;
    for(i=0; i<TASK_MONITOR_MAX_TASKS; i++)
    {
        TaskMonitor_Reset(i);
    }
}

//========================================================================
// 函数: TaskMonitor_PrintReport
// 描述: 打印性能报告
//========================================================================
void TaskMonitor_PrintReport(void)
{
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
    u8 i;
    
    MONITOR_PRINTF("\r\n=== Task Performance Report ===\r\n");
    
    for(i=0; i<TASK_MONITOR_MAX_TASKS; i++)
    {
        // 跳过从未执行的任务
        if(monitor_data[i].exec_count == 0)
            continue;
        
        MONITOR_PRINTF("Task%d: ", i);
        
#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_BASIC)
        MONITOR_PRINTF("Count=%lu", monitor_data[i].exec_count);
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_TIME)
        MONITOR_PRINTF(", Last=%uus, Max=%uus", 
                      monitor_data[i].exec_time,
                      monitor_data[i].max_exec_time);
#endif

#if (TASK_MONITOR_LEVEL >= TASK_MONITOR_FULL)
        if(monitor_data[i].period > 0)
        {
            u16 duty = monitor_data[i].max_exec_time * 100 / monitor_data[i].period;
            MONITOR_PRINTF(", Duty=%u%%, Timeout=%u", 
                          duty,
                          monitor_data[i].timeout_count);
        }
#endif
        
        MONITOR_PRINTF("\r\n");
    }
    
    MONITOR_PRINTF("===============================\r\n");
#endif
}

//========================================================================
// 函数: TaskMonitor_GetVersion
// 描述: 获取插件版本号
//========================================================================
const char* TaskMonitor_GetVersion(void)
{
    return "TaskMonitor v1.0.0";
}

