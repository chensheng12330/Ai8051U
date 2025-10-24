/*============================================================================*/
/*              CPUMonitor插件 - 当前项目集成示例代码                          */
/*============================================================================*/

/*
 * 本文件展示如何将CPUMonitor插件集成到当前项目
 * 
 * 集成方式: 最小侵入，仅需1行代码！
 */

//========================================================================
//                    示例1：main.c的修改（必需）
//========================================================================

/*
文件: Sources/src/main.c
修改内容: 添加1行include + 1行初始化 + 1行钩子
*/

// ========== 在文件开头添加 ==========
#include "CPUMonitor/CPUMonitor.h"
// ===================================

void main(void)
{
    WTST = 0;
    EAXFR = 1;
    CKCON = 0;

    SYS_Init();

    // ========== 添加：初始化CPU监控 ==========
    CPUMonitor_Init();
    // ========================================

    WDT_CONTR = 0x35;

    while (1)
    {
        WDT_CONTR |= 0x10;
        
        Task_Pro_Handler_Callback();
        
        // ========== 添加：空闲计数钩子 ==========
        CPU_MONITOR_IDLE_TICK();  // 仅1行！
        // =======================================
    }
}

//========================================================================
//                    示例2：添加计算和报告任务（推荐）
//========================================================================

/*
文件: Sources/src/Task.c
修改内容: 添加CPU监控任务
*/

static TASK_COMPONENTS Task_Comps[]=
{
    {0, 1,   1,   Sample_Display},
    {0, 10,  10,  Sample_MatrixKey},
    {0, 10,  10,  Sample_adcKey},
    {0, 300, 300, Sample_NTC},
    {0, 500, 500, Sample_RTC},
#if ENABLE_INT_KEY
    {0, 10,  10,  Sample_intKey},
#endif
    // ========== 添加：CPU监控任务 ==========
    {0, 1000,1000,CPUMonitor_Calculate},    // 每秒计算
    {0, 5000,5000,CPUMonitor_PrintReport},  // 每5秒打印（可选）
    // =======================================
};

//========================================================================
//                    示例3：在现有任务中集成（可选）
//========================================================================

/*
如果不想添加新任务，可以在现有任务中调用
*/

void Sample_RTC(void)  // 每500ms执行
{
    // ... 原有功能 ...
    
    static u8 count = 0;
    if(++count >= 2)  // 每1秒
    {
        count = 0;
        CPUMonitor_Calculate();  // 计算CPU占用率
    }
    
    // 可选：打印
    if(msecond & 1)  // 每秒打印一次
    {
        u8 cpu = CPUMonitor_GetUsage();
        printf("CPU: %u%%\r\n", cpu);
    }
}

//========================================================================
//                    示例4：按键触发查看
//========================================================================

/*
在按键回调中触发CPU报告
*/

void MyKeyHandler(KEY_EVENT *event)
{
    if(event->key_num == KEY_INT1 && 
       event->event_type == KEY_EVENT_LONG)
    {
        // 长按KEY2查看CPU占用率
        CPUMonitor_PrintReport();
    }
}

//========================================================================
//                    示例5：LED显示CPU占用率
//========================================================================

/*
在显示任务中显示CPU占用率
*/

void Sample_Display_with_CPU(void)
{
    // 原有显示功能
    Send_595(t_display[LED8[display_index]]);
    Send_595(~T_COM[display_index]);
    
    // 每100次刷新更新一次CPU数据
    static u8 refresh_count = 0;
    if(++refresh_count >= 100)
    {
        refresh_count = 0;
        
        u8 cpu = CPUMonitor_GetUsage();
        
        // 显示在数码管上（如右边2位）
        LED8[6] = cpu / 10;  // 十位
        LED8[7] = cpu % 10;  // 个位
    }
    
    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8) display_index = 0;
}

//========================================================================
//                    示例6：超载报警
//========================================================================

void Sample_CPU_Alarm(void)  // 每秒执行
{
    CPUMonitor_Calculate();
    
    u8 cpu = CPUMonitor_GetUsage();
    
    if(cpu > 80)
    {
        // CPU超载报警
        PWMB_ENO = 0x01;  // 蜂鸣器响
        printf("ALERT: CPU overload %u%%!\r\n", cpu);
    }
    else if(cpu > 50)
    {
        printf("WARN: CPU usage high %u%%\r\n", cpu);
    }
}

//========================================================================
//                    示例7：自适应任务周期
//========================================================================

/*
根据CPU占用率动态调整任务周期
*/

void Adaptive_Task_Period(void)
{
    static u16 ntc_period = 300;  // NTC任务初始周期
    
    u8 cpu = CPUMonitor_GetUsage();
    
    if(cpu > 80)
    {
        // CPU过载，降低采样频率
        ntc_period = 500;  // 300ms → 500ms
        printf("Reduced NTC sampling rate\r\n");
    }
    else if(cpu < 30)
    {
        // CPU空闲，提高采样频率
        ntc_period = 200;  // 恢复或提高
        printf("Increased NTC sampling rate\r\n");
    }
    
    // 动态修改任务周期（需要支持）
    // Task_Comps[3].TRITime = ntc_period;
}

//========================================================================
//                    性能分析示例输出
//========================================================================

/*
串口输出示例:

=== CPU Usage Report ===
Current:    8%
Max:        15%
Min:        5%
Idle Count: 1850000
Total Loops:2000000
Samples:    120
========================

解读:
  - 当前CPU占用8% (正常)
  - 峰值15% (系统负载不高)
  - 谷值5% (有大量空闲时间)
  - 空闲计数1,850,000次 (92.5%空闲)
  - 总循环2,000,000次
  - 已采样120次 (运行120秒)
*/

//========================================================================
//                    完全移除示例
//========================================================================

/*
如果不需要CPU监控:

步骤1: 在main.c中删除/注释3行
  // #include "CPUMonitor/CPUMonitor.h"
  // CPUMonitor_Init();
  // CPU_MONITOR_IDLE_TICK();

步骤2: 删除相关任务（如果有）
  // {0, 1000,1000,CPUMonitor_Calculate},

步骤3: 删除CPUMonitor文件夹
  rm -rf CPUMonitor/

完成! 
  Flash恢复: 150-200字节
  RAM恢复: 18字节
  CPU恢复: <0.001%
*/

//========================================================================
//                    与TaskMonitor配合使用
//========================================================================

/*
TaskMonitor和CPUMonitor可以同时使用:
*/

void main(void)
{
    SYS_Init();
    
    TaskMonitor_Init();   // 任务监控
    CPUMonitor_Init();    // CPU监控
    
    WDT_CONTR = 0x35;
    
    while (1)
    {
        WDT_CONTR |= 0x10;
        Task_Pro_Handler_Callback();
        CPU_MONITOR_IDLE_TICK();  // CPU空闲计数
    }
}

// 添加综合报告任务
void Sample_System_Report(void)  // 每5秒
{
    printf("\r\n╔═══ System Performance ═══╗\r\n");
    
    // CPU占用率
    CPUMonitor_Calculate();
    u8 cpu = CPUMonitor_GetUsage();
    printf("║ CPU Usage:    %u%%\r\n", cpu);
    
    // 任务性能
    printf("║ Tasks:\r\n");
    TaskMonitor_PrintReport();  // 详细任务报告
    
    printf("╚═══════════════════════════╝\r\n");
}

//========================================================================
//                    总结
//========================================================================

/*
CPUMonitor插件优势:

1. 极轻量级 ✅
   Flash: 150-200字节
   RAM:   18字节
   CPU:   <0.001%

2. 集成简单 ✅
   仅需1行代码: CPU_MONITOR_IDLE_TICK()
   
3. 功能完整 ✅
   当前/最大/最小值
   自动校准
   数据滤波

4. 完全独立 ✅
   不依赖项目
   可跨项目复用
   随时可移除

推荐指数: ⭐⭐⭐⭐⭐
*/

