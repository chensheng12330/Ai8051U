/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef    __AI8051U_GPIO_H
#define    __AI8051U_GPIO_H

#include "config.h"

//========================================================================
//                            端口模式设置
//========================================================================
//准双向口
#define  P0_MODE_IO_PU(Pin)         {P0M1 &= ~(Pin), P0M0 &= ~(Pin);}
#define  P1_MODE_IO_PU(Pin)         {P1M1 &= ~(Pin), P1M0 &= ~(Pin);}
#define  P2_MODE_IO_PU(Pin)         {P2M1 &= ~(Pin), P2M0 &= ~(Pin);}
#define  P3_MODE_IO_PU(Pin)         {P3M1 &= ~(Pin), P3M0 &= ~(Pin);}
#define  P4_MODE_IO_PU(Pin)         {P4M1 &= ~(Pin), P4M0 &= ~(Pin);}
#define  P5_MODE_IO_PU(Pin)         {P5M1 &= ~(Pin), P5M0 &= ~(Pin);}
#define  P6_MODE_IO_PU(Pin)         {P6M1 &= ~(Pin), P6M0 &= ~(Pin);}
#define  P7_MODE_IO_PU(Pin)         {P7M1 &= ~(Pin), P7M0 &= ~(Pin);}
//高阻输入
#define  P0_MODE_IN_HIZ(Pin)        {P0M1 |= (Pin), P0M0 &= ~(Pin);}
#define  P1_MODE_IN_HIZ(Pin)        {P1M1 |= (Pin), P1M0 &= ~(Pin);}
#define  P2_MODE_IN_HIZ(Pin)        {P2M1 |= (Pin), P2M0 &= ~(Pin);}
#define  P3_MODE_IN_HIZ(Pin)        {P3M1 |= (Pin), P3M0 &= ~(Pin);}
#define  P4_MODE_IN_HIZ(Pin)        {P4M1 |= (Pin), P4M0 &= ~(Pin);}
#define  P5_MODE_IN_HIZ(Pin)        {P5M1 |= (Pin), P5M0 &= ~(Pin);}
#define  P6_MODE_IN_HIZ(Pin)        {P6M1 |= (Pin), P6M0 &= ~(Pin);}
#define  P7_MODE_IN_HIZ(Pin)        {P7M1 |= (Pin), P7M0 &= ~(Pin);}
//漏极开路
#define  P0_MODE_OUT_OD(Pin)        {P0M1 |= (Pin), P0M0 |= (Pin);}
#define  P1_MODE_OUT_OD(Pin)        {P1M1 |= (Pin), P1M0 |= (Pin);}
#define  P2_MODE_OUT_OD(Pin)        {P2M1 |= (Pin), P2M0 |= (Pin);}
#define  P3_MODE_OUT_OD(Pin)        {P3M1 |= (Pin), P3M0 |= (Pin);}
#define  P4_MODE_OUT_OD(Pin)        {P4M1 |= (Pin), P4M0 |= (Pin);}
#define  P5_MODE_OUT_OD(Pin)        {P5M1 |= (Pin), P5M0 |= (Pin);}
#define  P6_MODE_OUT_OD(Pin)        {P6M1 |= (Pin), P6M0 |= (Pin);}
#define  P7_MODE_OUT_OD(Pin)        {P7M1 |= (Pin), P7M0 |= (Pin);}
//推挽输出
#define  P0_MODE_OUT_PP(Pin)        {P0M1 &= ~(Pin), P0M0 |= (Pin);}
#define  P1_MODE_OUT_PP(Pin)        {P1M1 &= ~(Pin), P1M0 |= (Pin);}
#define  P2_MODE_OUT_PP(Pin)        {P2M1 &= ~(Pin), P2M0 |= (Pin);}
#define  P3_MODE_OUT_PP(Pin)        {P3M1 &= ~(Pin), P3M0 |= (Pin);}
#define  P4_MODE_OUT_PP(Pin)        {P4M1 &= ~(Pin), P4M0 |= (Pin);}
#define  P5_MODE_OUT_PP(Pin)        {P5M1 &= ~(Pin), P5M0 |= (Pin);}
#define  P6_MODE_OUT_PP(Pin)        {P6M1 &= ~(Pin), P6M0 |= (Pin);}
#define  P7_MODE_OUT_PP(Pin)        {P7M1 &= ~(Pin), P7M0 |= (Pin);}

//========================================================================
//                          端口内部上拉设置
//========================================================================
//上拉使能
#define P0_PULL_UP_ENABLE(Pin)      P0PU |= (Pin)
#define P1_PULL_UP_ENABLE(Pin)      P1PU |= (Pin)
#define P2_PULL_UP_ENABLE(Pin)      P2PU |= (Pin)
#define P3_PULL_UP_ENABLE(Pin)      P3PU |= (Pin)
#define P4_PULL_UP_ENABLE(Pin)      P4PU |= (Pin)
#define P5_PULL_UP_ENABLE(Pin)      P5PU |= (Pin)
#define P6_PULL_UP_ENABLE(Pin)      P6PU |= (Pin)
#define P7_PULL_UP_ENABLE(Pin)      P7PU |= (Pin)
//上拉禁止
#define P0_PULL_UP_DISABLE(Pin)     P0PU &= ~(Pin)
#define P1_PULL_UP_DISABLE(Pin)     P1PU &= ~(Pin)
#define P2_PULL_UP_DISABLE(Pin)     P2PU &= ~(Pin)
#define P3_PULL_UP_DISABLE(Pin)     P3PU &= ~(Pin)
#define P4_PULL_UP_DISABLE(Pin)     P4PU &= ~(Pin)
#define P5_PULL_UP_DISABLE(Pin)     P5PU &= ~(Pin)
#define P6_PULL_UP_DISABLE(Pin)     P6PU &= ~(Pin)
#define P7_PULL_UP_DISABLE(Pin)     P7PU &= ~(Pin)

//========================================================================
//                          端口内部下拉设置
//========================================================================
//下拉使能
#define P0_PULL_DOWN_ENABLE(Pin)      P0PD |= (Pin)
#define P1_PULL_DOWN_ENABLE(Pin)      P1PD |= (Pin)
#define P2_PULL_DOWN_ENABLE(Pin)      P2PD |= (Pin)
#define P3_PULL_DOWN_ENABLE(Pin)      P3PD |= (Pin)
#define P4_PULL_DOWN_ENABLE(Pin)      P4PD |= (Pin)
#define P5_PULL_DOWN_ENABLE(Pin)      P5PD |= (Pin)
#define P6_PULL_DOWN_ENABLE(Pin)      P6PD |= (Pin)
#define P7_PULL_DOWN_ENABLE(Pin)      P7PD |= (Pin)
//下拉禁止
#define P0_PULL_DOWN_DISABLE(Pin)     P0PD &= ~(Pin)
#define P1_PULL_DOWN_DISABLE(Pin)     P1PD &= ~(Pin)
#define P2_PULL_DOWN_DISABLE(Pin)     P2PD &= ~(Pin)
#define P3_PULL_DOWN_DISABLE(Pin)     P3PD &= ~(Pin)
#define P4_PULL_DOWN_DISABLE(Pin)     P4PD &= ~(Pin)
#define P5_PULL_DOWN_DISABLE(Pin)     P5PD &= ~(Pin)
#define P6_PULL_DOWN_DISABLE(Pin)     P6PD &= ~(Pin)
#define P7_PULL_DOWN_DISABLE(Pin)     P7PD &= ~(Pin)

//========================================================================
//                           端口施密特触发设置
//========================================================================
//施密特触发使能
#define P0_ST_ENABLE(Pin)           P0NCS &= ~(Pin)
#define P1_ST_ENABLE(Pin)           P1NCS &= ~(Pin)
#define P2_ST_ENABLE(Pin)           P2NCS &= ~(Pin)
#define P3_ST_ENABLE(Pin)           P3NCS &= ~(Pin)
#define P4_ST_ENABLE(Pin)           P4NCS &= ~(Pin)
#define P5_ST_ENABLE(Pin)           P5NCS &= ~(Pin)
#define P6_ST_ENABLE(Pin)           P6NCS &= ~(Pin)
#define P7_ST_ENABLE(Pin)           P7NCS &= ~(Pin)
//施密特触发禁止
#define P0_ST_DISABLE(Pin)          P0NCS |= (Pin)
#define P1_ST_DISABLE(Pin)          P1NCS |= (Pin)
#define P2_ST_DISABLE(Pin)          P2NCS |= (Pin)
#define P3_ST_DISABLE(Pin)          P3NCS |= (Pin)
#define P4_ST_DISABLE(Pin)          P4NCS |= (Pin)
#define P5_ST_DISABLE(Pin)          P5NCS |= (Pin)
#define P6_ST_DISABLE(Pin)          P6NCS |= (Pin)
#define P7_ST_DISABLE(Pin)          P7NCS |= (Pin)

//========================================================================
//                          端口电平转换速度设置
//========================================================================
//电平转换慢速，相应的上下冲比较小
#define P0_SPEED_LOW(Pin)           P0SR |= (Pin) 
#define P1_SPEED_LOW(Pin)           P1SR |= (Pin) 
#define P2_SPEED_LOW(Pin)           P2SR |= (Pin) 
#define P3_SPEED_LOW(Pin)           P3SR |= (Pin) 
#define P4_SPEED_LOW(Pin)           P4SR |= (Pin) 
#define P5_SPEED_LOW(Pin)           P5SR |= (Pin) 
#define P6_SPEED_LOW(Pin)           P6SR |= (Pin) 
#define P7_SPEED_LOW(Pin)           P7SR |= (Pin) 
//电平转换快速，相应的上下冲比较大
#define P0_SPEED_HIGH(Pin)          P0SR &= ~(Pin) 
#define P1_SPEED_HIGH(Pin)          P1SR &= ~(Pin) 
#define P2_SPEED_HIGH(Pin)          P2SR &= ~(Pin) 
#define P3_SPEED_HIGH(Pin)          P3SR &= ~(Pin) 
#define P4_SPEED_HIGH(Pin)          P4SR &= ~(Pin) 
#define P5_SPEED_HIGH(Pin)          P5SR &= ~(Pin) 
#define P6_SPEED_HIGH(Pin)          P6SR &= ~(Pin) 
#define P7_SPEED_HIGH(Pin)          P7SR &= ~(Pin) 

//========================================================================
//                          端口驱动电流控制设置
//========================================================================
//一般驱动能力
#define P0_DRIVE_MEDIUM(Pin)        P0DR |= (Pin)
#define P1_DRIVE_MEDIUM(Pin)        P1DR |= (Pin)
#define P2_DRIVE_MEDIUM(Pin)        P2DR |= (Pin)
#define P3_DRIVE_MEDIUM(Pin)        P3DR |= (Pin)
#define P4_DRIVE_MEDIUM(Pin)        P4DR |= (Pin)
#define P5_DRIVE_MEDIUM(Pin)        P5DR |= (Pin)
#define P6_DRIVE_MEDIUM(Pin)        P6DR |= (Pin)
#define P7_DRIVE_MEDIUM(Pin)        P7DR |= (Pin)
//增强驱动能力
#define P0_DRIVE_HIGH(Pin)          P0DR &= ~(Pin)
#define P1_DRIVE_HIGH(Pin)          P1DR &= ~(Pin)
#define P2_DRIVE_HIGH(Pin)          P2DR &= ~(Pin)
#define P3_DRIVE_HIGH(Pin)          P3DR &= ~(Pin)
#define P4_DRIVE_HIGH(Pin)          P4DR &= ~(Pin)
#define P5_DRIVE_HIGH(Pin)          P5DR &= ~(Pin)
#define P6_DRIVE_HIGH(Pin)          P6DR &= ~(Pin)
#define P7_DRIVE_HIGH(Pin)          P7DR &= ~(Pin)

//========================================================================
//                          端口数字信号输入使能
//========================================================================
//使能数字信号输入
#define P0_DIGIT_IN_ENABLE(Pin)     P0IE |= (Pin)
#define P1_DIGIT_IN_ENABLE(Pin)     P1IE |= (Pin)
#define P2_DIGIT_IN_ENABLE(Pin)     P2IE |= (Pin)
#define P3_DIGIT_IN_ENABLE(Pin)     P3IE |= (Pin)
#define P4_DIGIT_IN_ENABLE(Pin)     P4IE |= (Pin)
#define P5_DIGIT_IN_ENABLE(Pin)     P5IE |= (Pin)
#define P6_DIGIT_IN_ENABLE(Pin)     P6IE |= (Pin)
#define P7_DIGIT_IN_ENABLE(Pin)     P7IE |= (Pin)
//禁止数字信号输入
#define P0_DIGIT_IN_DISABLE(Pin)    P0IE &= ~(Pin)
#define P1_DIGIT_IN_DISABLE(Pin)    P1IE &= ~(Pin)
#define P2_DIGIT_IN_DISABLE(Pin)    P2IE &= ~(Pin)
#define P3_DIGIT_IN_DISABLE(Pin)    P3IE &= ~(Pin)
#define P4_DIGIT_IN_DISABLE(Pin)    P4IE &= ~(Pin)
#define P5_DIGIT_IN_DISABLE(Pin)    P5IE &= ~(Pin)
#define P6_DIGIT_IN_DISABLE(Pin)    P6IE &= ~(Pin)
#define P7_DIGIT_IN_DISABLE(Pin)    P7IE &= ~(Pin)

//========================================================================
//                       端口模式用户控制寄存器设置
//========================================================================
//外设模块对所用I/O模式进行自动配置，忽略用户使用 PxM1/PxM0 对相应I/O进行的配置
#define P0_BP_ENABLE(Pin)           P0BP &= ~(Pin)
#define P1_BP_ENABLE(Pin)           P1BP &= ~(Pin)
#define P2_BP_ENABLE(Pin)           P2BP &= ~(Pin)
#define P3_BP_ENABLE(Pin)           P3BP &= ~(Pin)
#define P4_BP_ENABLE(Pin)           P4BP &= ~(Pin)
#define P5_BP_ENABLE(Pin)           P5BP &= ~(Pin)
#define P6_BP_ENABLE(Pin)           P6BP &= ~(Pin)
#define P7_BP_ENABLE(Pin)           P7BP &= ~(Pin)
//外设模块对所用I/O模式不进行自动配置，需要用户使用 PxM1/PxM0 对相应I/O进行配置
#define P0_BP_DISABLE(Pin)          P0BP |= (Pin)
#define P1_BP_DISABLE(Pin)          P1BP |= (Pin)
#define P2_BP_DISABLE(Pin)          P2BP |= (Pin)
#define P3_BP_DISABLE(Pin)          P3BP |= (Pin)
#define P4_BP_DISABLE(Pin)          P4BP |= (Pin)
#define P5_BP_DISABLE(Pin)          P5BP |= (Pin)
#define P6_BP_DISABLE(Pin)          P6BP |= (Pin)
#define P7_BP_DISABLE(Pin)          P7BP |= (Pin)

//========================================================================
//                       端口模式用户控制寄存器设置
//========================================================================

#define P0IntClrFlag()         P0INTF = 0    /* 清除 P0 中断标志 */
#define P1IntClrFlag()         P1INTF = 0    /* 清除 P1 中断标志 */
#define P2IntClrFlag()         P2INTF = 0    /* 清除 P2 中断标志 */
#define P3IntClrFlag()         P3INTF = 0    /* 清除 P3 中断标志 */
#define P4IntClrFlag()         P4INTF = 0    /* 清除 P4 中断标志 */
#define P5IntClrFlag()         P5INTF = 0    /* 清除 P5 中断标志 */
#define P6IntClrFlag()         P6INTF = 0    /* 清除 P6 中断标志 */
#define P7IntClrFlag()         P7INTF = 0    /* 清除 P7 中断标志 */

//========================================================================
//                              定义声明
//========================================================================

#define    GPIO_PullUp      0       //上拉准双向口
#define    GPIO_HighZ       1       //浮空输入
#define    GPIO_OUT_OD      2       //开漏输出
#define    GPIO_OUT_PP      3       //推挽输出

#define    GPIO_Pin_0       0x01    //IO引脚 Px.0
#define    GPIO_Pin_1       0x02    //IO引脚 Px.1
#define    GPIO_Pin_2       0x04    //IO引脚 Px.2
#define    GPIO_Pin_3       0x08    //IO引脚 Px.3
#define    GPIO_Pin_4       0x10    //IO引脚 Px.4
#define    GPIO_Pin_5       0x20    //IO引脚 Px.5
#define    GPIO_Pin_6       0x40    //IO引脚 Px.6
#define    GPIO_Pin_7       0x80    //IO引脚 Px.7
#define    GPIO_Pin_LOW     0x0F    //IO低4位引脚
#define    GPIO_Pin_HIGH    0xF0    //IO高4位引脚
#define    GPIO_Pin_All     0xFF    //IO所有引脚

#define    GPIO_P0          0
#define    GPIO_P1          1
#define    GPIO_P2          2
#define    GPIO_P3          3
#define    GPIO_P4          4
#define    GPIO_P5          5
#define    GPIO_P6          6
#define    GPIO_P7          7

#define    GPIO_FALLING_EDGE        1    //产生下降沿中断
#define    GPIO_RISING_EDGE         2    //产生上升沿中断
#define    GPIO_LOW_LEVEL           3    //产生低电平中断
#define    GPIO_HIGH_LEVEL          4    //产生高电平中断

typedef struct
{
    u8    Mode;       //IO模式, GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    u8    Pin;        //要设置的端口
} GPIO_InitTypeDef;

typedef struct
{
    u8    Mode;       //IO模式, GPIO_FALLING_EDGE,GPIO_RISING_EDGE,GPIO_LOW_LEVEL,GPIO_HIGH_LEVEL
    u8    Pin;        //要设置的端口
    u8    IntEnable;  //中断使能状态, ENABLE/DISABLE
    u8    Priority;   //中断优先级, Priority_0,Priority_1,Priority_2,Priority_3
    u8    WakeUpEn;   //唤醒使能状态, ENABLE/DISABLE
} GPIO_Int_InitTypeDef;

extern u8 ioIndex;

u8 GPIO_Inilize(u8 GPIO, GPIO_InitTypeDef *GPIOx);
u8 GPIO_INT_Inilize(u8 GPIO, GPIO_Int_InitTypeDef *GPIOx);

#endif
