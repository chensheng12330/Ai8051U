#ifndef __TASK_H_
#define __TASK_H_

#include "config.h"

//========================================================================
//                               本地变量声明
//========================================================================

typedef struct 
{
	u8 Run;               //任务状态：Run/Stop
	u16 TIMCount;         //定时计数器
	u16 TRITime;          //重载计数器
	void (*TaskHook) (void); //任务函数
} TASK_COMPONENTS;       

//========================================================================
//                             外部函数和变量声明
//========================================================================

void Task_Marks_Handler_Callback(void);
void Task_Pro_Handler_Callback(void);

#endif
