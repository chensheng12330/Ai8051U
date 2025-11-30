#ifndef __APP_H_
#define __APP_H_

//========================================================================
//                                头文件
//========================================================================

// 原有模块（演示用，可根据需要保留或移除）
// #include "app_Display.h"
// #include "app_MatrixKey.h"
// #include "app_adcKey.h"
// #include "app_ntc.h"
// #include "app_rtc.h"

// 外部中断按键模块（必需）
#if ENABLE_INT_KEY
#include "app_intKey.h"
#endif

//========================================================================
//                     摩托车智能联动灯组系统模块
//========================================================================
#include "moto_lights/moto_lights_config.h"
#include "moto_lights/moto_lights_pinmap.h"
#include "moto_lights/app_moto_lights.h"
#include "moto_lights/app_vehicle_signal.h"
#include "moto_lights/app_ws2812b.h"
#include "moto_lights/app_pwm_output.h"
#include "moto_lights/app_light_scheduler.h"
#include "moto_lights/app_sensors.h"

// USB HID 调试任务
void Sample_USB_Debug(void);

void APP_config(void);

#endif
