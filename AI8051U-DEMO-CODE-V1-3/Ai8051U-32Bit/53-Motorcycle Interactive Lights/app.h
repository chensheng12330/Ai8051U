#ifndef __APP_H_
#define __APP_H_

//========================================================================
//                                头文件
//========================================================================

#include "app_Display.h"
//#include "app_MatrixKey.h"
#include "app_adcKey.h"
//#include "app_ntc.h"
//#include "app_rtc.h"
#include "motorcycle_light_system.h"

// 外部中断按键模块（可配置启用/禁用）
#if ENABLE_INT_KEY
#include "app_intKey.h"
#endif

// USB HID 调试任务
void Sample_USB_Debug(void);

void APP_config(void);

#endif
