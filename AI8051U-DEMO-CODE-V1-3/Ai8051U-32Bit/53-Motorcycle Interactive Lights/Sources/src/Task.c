/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "Task.h"
#include "app.h"

//========================================================================
// 集成独立监控插件（可选，需要时才包含）
//========================================================================
#include "TaskMonitor/TaskMonitor.h"   // 任务性能监控插件
#include "CPUMonitor/CPUMonitor.h"     // CPU占用率监控插件

//========================================================================
//                               本地变量声明
//========================================================================
void Sample_NTC(void)
{

}

void Sample_RTC(void)
{

}

void Sample_MatrixKey(void)
{

}

static TASK_COMPONENTS Task_Comps[]=
{
//状态  计数  周期  函数
    {0, 1,   1,   Sample_Display},              /* task 1 Period： 1ms */
    {0, 10,  10,  Sample_MatrixKey},            /* task 2 Period： 10ms */
    {0, 10,  10,  Sample_adcKey},               /* task 3 Period： 10ms */
    {0, 300, 300, Sample_NTC},                  /* task 4 Period： 300ms */
    {0, 500, 500, Sample_RTC},                  /* task 5 Period： 500ms */
#if ENABLE_INT_KEY
    {0, 10,  10,  Sample_intKey},               /* task 6 Period： 10ms - 外部中断按键 */
#endif
    //========================================================================
    // 摩托车智能灯组系统任务 (新增)
    //========================================================================
    {0, 1,   1,   Sample_Vehicle_Signal_Process},   /* task 7 Period： 1ms - 原车信号处理 */
    {0, 2,   2,   Sample_WS2812_DMA_Control},       /* task 8 Period： 2ms - WS2812 DMA控制 */
    {0, 5,   5,   Sample_Button_Process},           /* task 9 Period： 5ms - 按键处理 */
    {0, 10,  10,  Sample_Sensor_Read},              /* task 10 Period： 10ms - 传感器读取 */
    {0, 20,  20,  Sample_Light_Effect_Calculate},   /* task 11 Period： 20ms - 灯效算法 */
    {0, 50,  50,  Sample_Audio_Process},            /* task 12 Period： 50ms - 音频处理 */
    {0, 100, 100, Sample_Status_Display},           /* task 13 Period： 100ms - 状态指示 */
    {0, 500, 500, Sample_Debug_Output},             /* task 14 Period： 500ms - 调试输出 */

    //========================================================================
    // 系统状态机任务 (新增)
    //========================================================================
    {0, 100, 100, System_State_Machine},            /* task 15 Period： 100ms - 系统状态机 */

    //========================================================================
    // 监控插件任务（可选）
    //========================================================================
    {0, 1000,1000,CPUMonitor_Calculate},            /* CPU监控：每秒计算占用率 */
    {0, 5000,5000,TaskMonitor_PrintReport},         /* 任务监控：每5秒打印报告 */
    {0, 5000,5000,CPUMonitor_PrintReport},          /* CPU监控：每5秒打印报告 */

    //========================================================================
    // USB HID 调试任务
    //========================================================================
    {0, 1,   1,   Sample_USB_Debug},                /* USB调试：每1ms执行一次 */

    /* Add new task here */
};

u8 Tasks_Max = sizeof(Task_Comps)/sizeof(Task_Comps[0]);

//========================================================================
// 函数: Task_Handler_Callback
// 描述: 任务标记回调函数.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2012-10-22
//========================================================================
void Task_Marks_Handler_Callback(void)
{
    u8 i;
    for(i=0; i<Tasks_Max; i++)
    {
        if(Task_Comps[i].TIMCount)      /* If the time is not 0 */
        {
            Task_Comps[i].TIMCount--;   /* Time counter decrement */
            if(Task_Comps[i].TIMCount == 0) /* If time arrives */
            {
                /*Resume the timer value and try again */
                Task_Comps[i].TIMCount = Task_Comps[i].TRITime;  
                Task_Comps[i].Run = 1;      /* The task can be run */
            }
        }
    }
}

//========================================================================
// 函数: Task_Pro_Handler_Callback
// 描述: 任务处理回调函数.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2012-10-22
//========================================================================
void Task_Pro_Handler_Callback(void)
{
    u8 i;
    for(i=0; i<Tasks_Max; i++)
    {
        if(Task_Comps[i].Run) /* If task can be run */
        {
            //================================================================
            // 优化4：TaskMonitor插件钩子 - 任务开始
            //================================================================
            TASK_MONITOR_START(i);
            
            Task_Comps[i].Run = 0;      /* Flag clear 0 */
            Task_Comps[i].TaskHook();   /* Run task */
            
            //================================================================
            // 优化4：TaskMonitor插件钩子 - 任务结束
            //================================================================
            TASK_MONITOR_END(i);
        }
    }
}
