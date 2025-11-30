/*============================================================================*/
/*                      CPU占用率监控插件 - 实现文件                           */
/*============================================================================*/

#include "CPUMonitor.h"

//========================================================================
//                          本地变量
//========================================================================

// CPU监控数据
static CPU_MONITOR_DATA cpu_data;

// 校准标志
#if CPU_MONITOR_AUTO_CALIBRATE
static u8 calibrated = 0;
static u32 calibrate_loops = 0;
#endif

//========================================================================
// 函数: CPUMonitor_Init
// 描述: 初始化CPU监控插件
//========================================================================
void CPUMonitor_Init(void)
{
    cpu_data.idle_count = 0;
    cpu_data.last_idle_count = 0;
    cpu_data.total_loops = 0;
    cpu_data.cpu_usage = 0;
    cpu_data.cpu_usage_max = 0;
    cpu_data.cpu_usage_min = 100;
    cpu_data.sample_count = 0;
    
#if CPU_MONITOR_AUTO_CALIBRATE
    calibrated = 0;
    calibrate_loops = 0;
#endif
    
#if CPU_MONITOR_DEBUG
    CPU_MONITOR_PRINTF("CPUMonitor initialized\r\n");
    CPU_MONITOR_PRINTF("Expected idle loops/sec: %lu\r\n", 
                       CPU_MONITOR_IDLE_LOOPS_PER_SEC);
#endif
}

//========================================================================
// 函数: CPUMonitor_IdleTick
// 描述: 空闲计数（在主循环中调用）
// 开销: 1条指令 (~20ns @ 48MHz)
//========================================================================
void CPUMonitor_IdleTick(void)
{
    cpu_data.idle_count++;
    cpu_data.total_loops++;
    
#if CPU_MONITOR_AUTO_CALIBRATE
    // 自动校准模式：记录空闲时的循环次数
    if(!calibrated)
    {
        calibrate_loops++;
    }
#endif
}

//========================================================================
// 函数: CPUMonitor_Calculate
// 描述: 计算CPU占用率（定时调用，如每秒）
//========================================================================
void CPUMonitor_Calculate(void)
{
    u32 idle_delta;
    u32 expected_idle;
    u8  cpu_usage_raw;
    
#if CPU_MONITOR_AUTO_CALIBRATE
    // 首次调用时进行校准
    if(!calibrated)
    {
        //CPU_MONITOR_IDLE_LOOPS_PER_SEC = calibrate_loops;
        calibrated = 1;
        
#if CPU_MONITOR_DEBUG
        CPU_MONITOR_PRINTF("Auto-calibrated: %lu loops/sec\r\n", 
                           calibrate_loops);
#endif
        
        // 重置计数器
        cpu_data.idle_count = 0;
        cpu_data.last_idle_count = 0;
        return;
    }
#endif
    
    // 计算本周期的空闲循环次数
    idle_delta = cpu_data.idle_count - cpu_data.last_idle_count;
    cpu_data.last_idle_count = cpu_data.idle_count;
    
    // 期望的空闲循环次数（100% CPU空闲时）
    expected_idle = CPU_MONITOR_IDLE_LOOPS_PER_SEC * 
                    CPU_MONITOR_SAMPLE_PERIOD_MS / 1000;
    
    // 计算CPU占用率
    if(idle_delta >= expected_idle)
    {
        cpu_usage_raw = 0;  // 完全空闲
    }
    else
    {
        // CPU占用 = (1 - 实际空闲/期望空闲) × 100
        cpu_usage_raw = (u8)(100 - ((u32)idle_delta * 100 / expected_idle));
        
        // 限幅
        if(cpu_usage_raw > 100)
            cpu_usage_raw = 100;
    }
    
#if CPU_MONITOR_USE_FILTER
    // 使用滤波器平滑数据
    // 新值 = 旧值×(1-α) + 新测量值×α
    cpu_data.cpu_usage = (u8)((cpu_data.cpu_usage * (100 - CPU_MONITOR_FILTER_ALPHA) +
                               cpu_usage_raw * CPU_MONITOR_FILTER_ALPHA) / 100);
#else
    cpu_data.cpu_usage = cpu_usage_raw;
#endif
    
    // 更新历史最大/最小值
#if CPU_MONITOR_KEEP_HISTORY
    if(cpu_data.cpu_usage > cpu_data.cpu_usage_max)
    {
        cpu_data.cpu_usage_max = cpu_data.cpu_usage;
    }
    
    if(cpu_data.cpu_usage < cpu_data.cpu_usage_min)
    {
        cpu_data.cpu_usage_min = cpu_data.cpu_usage;
    }
#endif
    
    cpu_data.sample_count++;
    
#if CPU_MONITOR_DEBUG
    CPU_MONITOR_PRINTF("Idle delta: %lu, Expected: %lu, CPU: %u%%\r\n",
                       idle_delta, expected_idle, cpu_data.cpu_usage);
#endif
}

//========================================================================
// 函数: CPUMonitor_GetUsage
// 描述: 获取当前CPU占用率
// 返回: CPU占用率(%)
//========================================================================
u8 CPUMonitor_GetUsage(void)
{
    return cpu_data.cpu_usage;
}

//========================================================================
// 函数: CPUMonitor_GetData
// 描述: 获取完整监控数据
//========================================================================
CPU_MONITOR_DATA* CPUMonitor_GetData(void)
{
    return &cpu_data;
}

//========================================================================
// 函数: CPUMonitor_PrintReport
// 描述: 打印CPU使用报告
//========================================================================
void CPUMonitor_PrintReport(void)
{
    CPU_MONITOR_PRINTF("\r\n=== CPU Usage Report ===\r\n");
    CPU_MONITOR_PRINTF("Current:    %u%%\r\n", cpu_data.cpu_usage);
    
#if CPU_MONITOR_KEEP_HISTORY
    CPU_MONITOR_PRINTF("Max:        %u%%\r\n", cpu_data.cpu_usage_max);
    CPU_MONITOR_PRINTF("Min:        %u%%\r\n", cpu_data.cpu_usage_min);
#endif
    
    CPU_MONITOR_PRINTF("Idle Count: %lu\r\n", cpu_data.idle_count);
    CPU_MONITOR_PRINTF("Total Loops:%lu\r\n", cpu_data.total_loops);
    CPU_MONITOR_PRINTF("Samples:    %lu\r\n", cpu_data.sample_count);
    CPU_MONITOR_PRINTF("========================\r\n");
}

//========================================================================
// 函数: CPUMonitor_Reset
// 描述: 重置监控数据
//========================================================================
void CPUMonitor_Reset(void)
{
    cpu_data.idle_count = 0;
    cpu_data.last_idle_count = 0;
    cpu_data.total_loops = 0;
    cpu_data.cpu_usage = 0;
    cpu_data.cpu_usage_max = 0;
    cpu_data.cpu_usage_min = 100;
    cpu_data.sample_count = 0;
    
#if CPU_MONITOR_DEBUG
    CPU_MONITOR_PRINTF("CPU Monitor reset\r\n");
#endif
}

//========================================================================
// 函数: CPUMonitor_GetVersion
// 描述: 获取插件版本
//========================================================================
const char* CPUMonitor_GetVersion(void)
{
    return "CPUMonitor v1.0.0";
}

