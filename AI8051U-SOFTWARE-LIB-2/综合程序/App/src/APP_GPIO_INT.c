/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_GPIO_INT.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_Delay.h"
#include "AI8051U_NVIC.h"

/*************    功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

演示所有普通IO口都可以产生中断，并且可以将MCU从休眠唤醒的功能.

从串口输出唤醒源跟芯片状态，115200,N,8,1.

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 24MHz (用户可在"config.h"修改频率).

******************************************/

//========================================================================
//                               本地常量声明    
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================


//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================

extern u8 code ledNum[];
extern u8 ledIndex;
extern u16 msecond;

//========================================================================
// 函数: GPIO_INTtoUART_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-02-11
//========================================================================
void GPIO_INTtoUART_init(void)
{
    COMx_InitDefine  COMx_InitStructure;            //结构定义
    GPIO_Int_InitTypeDef  GPIO_Int_InitStructure;   //结构定义

    P0_MODE_IO_PU(GPIO_Pin_All);            //P0 设置为准双向口
    P1_MODE_IO_PU(GPIO_Pin_All);            //P1 设置为准双向口
    P2_MODE_IO_PU(GPIO_Pin_All);            //P2 设置为准双向口
    P3_MODE_IO_PU(GPIO_Pin_All);            //P3 设置为准双向口
    P4_MODE_IO_PU(GPIO_Pin_All);            //P4 设置为准双向口
    P5_MODE_IO_PU(GPIO_Pin_All);            //P5 设置为准双向口

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;     //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;         //选择波特率发生器, BRT_Timer1,BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 115200ul;           //波特率,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = DISABLE;            //接收禁止,   ENABLE 或 DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);         //初始化串口  UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1); //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    //------------------------------------------------
    P0IntClrFlag();     //清中断标志
    P1IntClrFlag();     //清中断标志
    P2IntClrFlag();     //清中断标志
    P3IntClrFlag();     //清中断标志
    P4IntClrFlag();     //清中断标志
    P5IntClrFlag();     //清中断标志

    GPIO_Int_InitStructure.Mode = GPIO_FALLING_EDGE;    //IO模式, GPIO_FALLING_EDGE,GPIO_RISING_EDGE,GPIO_LOW_LEVEL,GPIO_HIGH_LEVEL
    GPIO_Int_InitStructure.Pin  = GPIO_Pin_All;
    GPIO_Int_InitStructure.WakeUpEn  = ENABLE;          //唤醒使能状态, ENABLE/DISABLE
    GPIO_Int_InitStructure.Priority  = Priority_0;      //中断优先级, Priority_0,Priority_1,Priority_2,Priority_3
    GPIO_Int_InitStructure.IntEnable = ENABLE;          //中断使能状态, ENABLE/DISABLE
    GPIO_INT_Inilize(GPIO_P0, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P1, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P2, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7
    GPIO_INT_Inilize(GPIO_P5, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7

    GPIO_Int_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_HIGH;  //P3.2~P3.7
    GPIO_INT_Inilize(GPIO_P3, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7

    GPIO_Int_InitStructure.Mode = GPIO_RISING_EDGE;     //IO模式, GPIO_FALLING_EDGE,GPIO_RISING_EDGE,GPIO_LOW_LEVEL,GPIO_HIGH_LEVEL
    GPIO_Int_InitStructure.Pin  = GPIO_Pin_7;
    GPIO_INT_Inilize(GPIO_P4, &GPIO_Int_InitStructure); //初始化IO中断  GPIO_P0~GPIO_P7

//    IRC_Debounce(0x10); //设置IRC时钟从休眠唤醒恢复稳定需要等待的时钟数
    P40 = 0;    //LED Power On
    printf("普通IO口中断/唤醒测试");
}

//========================================================================
// 函数: Sample_GPIO_INTtoUART
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-24
//========================================================================
void Sample_GPIO_INTtoUART(void)
{
    //跑马灯指示工作状态
    P0 = ~ledNum[ledIndex];    //输出低驱动
    ledIndex++;
    if(ledIndex > 7)
    {
        ledIndex = 0;
    }

    //1秒后MCU进入休眠状态
    if(++msecond >= 10)
    {
        msecond = 0;    //清计数

        P0 = 0xff;      //先关闭显示，省电
        printf("MCU Sleep.\r\n");

        _nop_();
        _nop_();
        _nop_();
        PD = 1;         //Sleep
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        printf("MCU wakeup from P%02X.\r\n", ioIndex);
    }
}
