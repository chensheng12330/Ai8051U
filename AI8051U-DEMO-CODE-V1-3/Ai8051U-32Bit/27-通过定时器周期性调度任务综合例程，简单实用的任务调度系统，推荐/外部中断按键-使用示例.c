/*---------------------------------------------------------------------*/
/* 外部中断按键模块 - 使用示例代码                                      */
/* 演示如何使用方案C（事件结构体回调机制）                              */
/*---------------------------------------------------------------------*/

#include "config.h"
#include "app_intKey.h"
#include "app_rtc.h"      // 用于时间调整示例
#include "app_display.h"  // 用于显示更新示例

//========================================================================
//                            使用方式1：注册回调函数
//========================================================================

// 步骤1：定义您的回调函数
void MyKeyEventHandler(KEY_EVENT *event)
{
    // 打印事件信息（调试用）
    printf("[%s] %s, Type:%s, Time:%dms\r\n",
           intKey_GetKeyName(event->key_num),
           event->pin_state ? "Released" : "Pressed",
           intKey_GetEventName(event->event_type),
           event->hold_time);
    
    //========================================================================
    // 根据事件类型处理
    //========================================================================
    
    if(event->event_type == KEY_EVENT_SHORT)
    {
        // 短按事件处理
        switch(event->key_num)
        {
            case KEY_INT0:  // 按键1：小时+
                if(++usrHour >= 24) usrHour = 0;
                DisplayRTC();
                printf("Hour = %d\r\n", usrHour);
                break;
                
            case KEY_INT1:  // 按键2：小时-
                if(--usrHour >= 24) usrHour = 23;
                DisplayRTC();
                printf("Hour = %d\r\n", usrHour);
                break;
                
            case KEY_INT2:  // 按键3：分钟+
                usrSecond = 0;
                if(++usrMinute >= 60) usrMinute = 0;
                DisplayRTC();
                printf("Minute = %d\r\n", usrMinute);
                break;
                
            case KEY_INT3:  // 按键4：分钟-
                usrSecond = 0;
                if(--usrMinute >= 60) usrMinute = 59;
                DisplayRTC();
                printf("Minute = %d\r\n", usrMinute);
                break;
        }
    }
    else if(event->event_type == KEY_EVENT_LONG)
    {
        // 长按事件处理（首次触发）
        printf("Long press detected on Key%d\r\n", event->key_num);
        
        // 示例：长按复位时间
        if(event->key_num == KEY_INT0)
        {
            usrHour = 12;
            usrMinute = 0;
            usrSecond = 0;
            DisplayRTC();
            printf("Time RESET to 12:00:00\r\n");
        }
    }
    else if(event->event_type == KEY_EVENT_REPEAT)
    {
        // 长按重复事件（每200ms触发）
        switch(event->key_num)
        {
            case KEY_INT0:  // 长按快速加小时
                if(++usrHour >= 24) usrHour = 0;
                DisplayRTC();
                break;
                
            case KEY_INT1:  // 长按快速减小时
                if(--usrHour >= 24) usrHour = 23;
                DisplayRTC();
                break;
                
            case KEY_INT2:  // 长按快速加分钟
                if(++usrMinute >= 60) usrMinute = 0;
                DisplayRTC();
                break;
                
            case KEY_INT3:  // 长按快速减分钟
                if(--usrMinute >= 60) usrMinute = 59;
                DisplayRTC();
                break;
        }
    }
}

// 步骤2：在初始化时注册回调（在APP_config()或main()中）
void MyApp_Init(void)
{
    // ... 其他初始化 ...
    
    intKey_init();  // 初始化按键模块
    
    intKey_RegisterCallback(MyKeyEventHandler);  // 注册回调函数
    
    printf("Key callback installed\r\n");
}

//========================================================================
//                        使用方式2：直接修改默认处理函数
//========================================================================

// 如果不想注册回调，可以直接修改app_intKey.c中的Default_KeyEventHandler()函数
// 该函数会在未注册回调时自动调用

//========================================================================
//                        使用方式3：获取事件信息
//========================================================================

void Advanced_KeyHandler(KEY_EVENT *event)
{
    // 使用所有事件信息
    printf("=== Key Event ===\r\n");
    printf("Key:       %s\r\n", intKey_GetKeyName(event->key_num));
    printf("Event:     %s\r\n", intKey_GetEventName(event->event_type));
    printf("Hold Time: %dms\r\n", event->hold_time);
    printf("Pin State: %s\r\n", event->pin_state ? "HIGH" : "LOW");
    printf("Timestamp: %ldms\r\n", event->timestamp);
    printf("=================\r\n");
    
    // 根据持续时间做不同处理
    if(event->event_type == KEY_EVENT_SHORT)
    {
        if(event->hold_time < 100)
        {
            printf("Very quick press\r\n");
        }
        else if(event->hold_time < 300)
        {
            printf("Normal press\r\n");
        }
        else
        {
            printf("Slow press\r\n");
        }
    }
}

//========================================================================
//                        使用方式4：运行时切换回调
//========================================================================

KeyEventCallback mode1_handler = MyKeyEventHandler;
KeyEventCallback mode2_handler = Advanced_KeyHandler;

void SwitchKeyMode(u8 mode)
{
    if(mode == 1)
    {
        intKey_RegisterCallback(mode1_handler);
        printf("Switched to Mode 1\r\n");
    }
    else if(mode == 2)
    {
        intKey_RegisterCallback(mode2_handler);
        printf("Switched to Mode 2\r\n");
    }
    else
    {
        intKey_RegisterCallback(NULL);  // 使用默认处理
        printf("Switched to Default Mode\r\n");
    }
}

//========================================================================
//                        性能测试代码
//========================================================================

void Test_Callback_Performance(void)
{
    u32 start_time, end_time;
    u16 elapsed;
    u8 i;
    
    printf("\r\n=== 回调性能测试 ===\r\n");
    
    // 模拟100次按键事件
    KEY_EVENT test_event = {
        .key_num = KEY_INT0,
        .event_type = KEY_EVENT_SHORT,
        .hold_time = 100,
        .pin_state = 1,
        .timestamp = 0
    };
    
    start_time = system_tick_ms;
    
    for(i=0; i<100; i++)
    {
        if(key_event_callback != NULL)
        {
            key_event_callback(&test_event);
        }
    }
    
    end_time = system_tick_ms;
    elapsed = end_time - start_time;
    
    printf("100次回调耗时: %dms\r\n", elapsed);
    printf("平均每次: %d.%dus\r\n", elapsed * 10, elapsed % 10);
    printf("===================\r\n");
}

//========================================================================
//                        完整使用示例（main.c中）
//========================================================================

/*

// 在main.c中的使用方法：

#include "app_intKey.h"

// 定义回调函数
void My_Key_Handler(KEY_EVENT *event)
{
    if(event->event_type == KEY_EVENT_SHORT)
    {
        printf("Key %d pressed\r\n", event->key_num);
        // 您的功能代码
    }
}

void main(void)
{
    // ... 系统初始化 ...
    
    SYS_Init();
    
    #if ENABLE_INT_KEY
    // 注册按键回调
    intKey_RegisterCallback(My_Key_Handler);
    #endif
    
    // ... 启动看门狗 ...
    
    while(1)
    {
        // ... 任务调度 ...
    }
}

*/

