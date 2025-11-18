/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "app.h"

//========================================================================
// 函数: APP_config
// 描述: 用户应用程序初始化.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-24
//========================================================================
void APP_config(void)
{
	Display_init();
	//RTC_init();
	//adcKey_init();

#if ENABLE_INT_KEY
	intKey_init();  // 初始化外部中断按键模块
#endif

	// 初始化摩托车智能灯组系统
	Motorcycle_Light_System_Init();

	// 更新GPIO配置
	GPIO_Config_Update();
}

//========================================================================
// 函数: Sample_USB_Debug
// 描述: USB HID 调试任务，通过 USB 接口输出调试信息
// 参数: None.
// 返回: None.
// 版本: V1.0, 2024-01-01
//========================================================================
void Sample_USB_Debug(void)
{
    static u16 debug_counter = 0;

    debug_counter++;

    // 每1000次执行（约1秒）输出一次调试信息
    if (debug_counter >= 1000)
    {
        debug_counter = 0;

        // 输出系统运行状态信息
        printf("=== System Debug Info ===\n");
        printf("System running at 48MHz\n");
        printf("USB HID Debug enabled\n");
        printf("Task scheduler active\n");

        // 输出一些示例数据
        printf("Sample data: 0x%08lX\n", 0x12345678UL);
        printf("Counter: %u\n", debug_counter);

        printf("========================\n\n");
    }
}
