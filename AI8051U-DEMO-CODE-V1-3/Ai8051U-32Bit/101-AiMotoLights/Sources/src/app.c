/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "app.h"
#include "../../USBHID/usb.h"

//========================================================================
// 函数: APP_config
// 描述: 用户应用程序初始化.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-24
//       V2.0, 2025-11-17 - 改为摩托车智能联动灯组系统
//========================================================================
void APP_config(void)
{
	// 原有演示模块（已注释，可根据需要保留）
	// Display_init();
	// RTC_init();
	// adcKey_init();

	//========================================================================
	// 摩托车智能联动灯组系统初始化
	//========================================================================
	MotoLights_Init();  // 初始化摩托车灯组系统（包含所有子模块）
	
	// 注意：外部中断按键模块在MotoLights_Init()中已初始化
}

//========================================================================
// 函数: Sample_USB_Debug
// 描述: USB HID 调试任务，通过 USB 接口输出系统运行信息
// 参数: None.
// 返回: None.
// 版本: V1.0, 2024-01-01
//       V2.0, 2025-11-17 - 输出摩托车灯组系统信息
//========================================================================
void Sample_USB_Debug(void)
{
    static u16 debug_counter = 0;
    extern SYSTEM_INFO system_info;

    debug_counter++;

    // 每5000次执行（约5秒）输出一次调试信息
    if (debug_counter >= 5000)
    {
        debug_counter = 0;

        // 输出摩托车灯组系统状态
        printf("=== Moto Lights System Status ===\n");
        printf("Runtime: %lu s\n", system_info.run_time_seconds);
        printf("State: %d, Scene: %d\n", 
               system_info.system_state,
               LightScheduler_GetActiveScene());
        printf("Battery: %.2fV, Light: %d\n", 
               Sensors_GetBatteryVoltage(),
               Sensors_GetLightIntensity());
        printf("==================================\n\n");
    }
}
