/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

本示例说明了如何配置PWMA外围设备以生成不对称信号。
PWMA配置为在PWMA CH1,3上产生可编程相移信号产生不对称信号：

- 通道1配置在PWM2模式
- 通道2配置在不对称PWM2模式
- 通道3配置在PWM2模式
- 通道4配置在不对称PWM2模式
- 计数器模式是中心对齐模式
- 脉冲长度和相移是连续编程在PWMA_CCR2和PWMA_CCR1。


在不对称模式下，生成的两个中心对齐 PWM 信号间允许存在可编程相移。
频率由 PWMA_ARR 寄存器的值确定，而占空比和相移则由一对 PWMA_CCRx 寄存器确定。
两个寄存器分别控制递增计数和递减计数期间的 PWM，这样每半个 PWM 周期便会调节一次 PWM：
– OC1REFC（或 OC2REFC）由 PWMA_CCR1 和 PWMA_CCR2 控制
– OC3REFC（或 OC4REFC）由 PWMA_CCR3 和 PWMA_CCR4 控制

两个通道可以独立选择不对称 PWM 模式（每对 CCR 寄存器一个 OCx 输出），
只需向 PWMA_CCMRx 寄存器的 OCxM 位写入“1110”（不对称 PWM 模式 1）或“1111”（不对称 PWM 模式 2）。

注： 出于兼容性原因，OCxM[3:0] 位域分为两部分，最高有效位与最低有效的 3 位不相邻。
给定通道用作不对称 PWM 通道时，也可使用其互补通道。
例如，如果通道 1 上产生 OC1REFC 信号（不对称 PWM 模式 1），
则由于不对称 PWM 模式 1 的原因，通道 2 上可输出 OC2REF 信号或 OC2REFC 信号。
与死区发生器配合使用时，这可控制相移全桥直流到直流转换器。

******************************************/

#include "AI8051U.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define PWM1_0      0x00	//P:P1.0  N:P1.1
#define PWM1_1      0x01	//P:P2.0  N:P2.1
#define PWM1_2      0x02	//P:P6.0  N:P6.1

#define PWM2_0      0x00	//P:P1.2/P5.4  N:P1.3
#define PWM2_1      0x04	//P:P2.2  N:P2.3
#define PWM2_2      0x08	//P:P6.2  N:P6.3

#define PWM3_0      0x00	//P:P1.4  N:P1.5
#define PWM3_1      0x10	//P:P2.4  N:P2.5
#define PWM3_2      0x20	//P:P6.4  N:P6.5

#define PWM4_0      0x00	//P:P1.6  N:P1.7
#define PWM4_1      0x40	//P:P2.6  N:P2.7
#define PWM4_2      0x80	//P:P6.6  N:P6.7
#define PWM4_3      0xC0	//P:P3.4  N:P3.3

#define ENO1P       0x01
#define ENO1N       0x02
#define ENO2P       0x04
#define ENO2N       0x08
#define ENO3P       0x10
#define ENO3N       0x20
#define ENO4P       0x40
#define ENO4N       0x80

/*****************************************************************************/


/*************  本地常量声明    **************/

#define PWM_PERIOD  22000    //设置周期值

/*************  本地变量声明    **************/

u16 PWM1_Duty;
u16 PWM2_Duty;
u16 PWM3_Duty;
u16 PWM4_Duty;

void UpdatePwm(void);

/********************* 主函数 *************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0xff;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    PWM1_Duty = 15000;
    PWM2_Duty = 5000;
    PWM3_Duty = 5000+3000;
    PWM4_Duty = 5000+7000;

    PWMA_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCER2 = 0x00;

    PWMA_CCMR1X = 0x00;//通道1:PWM2模式
    PWMA_CCMR1 = 0x70;
    PWMA_CCMR2X = 0x01;//通道2:不对称PWM2模式
    PWMA_CCMR2 = 0x70;
    PWMA_CCMR3X = 0x00;//通道3:PWM2模式
    PWMA_CCMR3 = 0x70;
    PWMA_CCMR4X = 0x01;//通道4:不对称PWM2模式
    PWMA_CCMR4 = 0x70;

    PWMA_CCER1 = 0x55; //配置通道输出使能和极性
    PWMA_CCER2 = 0x55;

    PWMA_ARRH = (u8)(PWM_PERIOD >> 8); //设置周期时间
    PWMA_ARRL = (u8)PWM_PERIOD;

    PWMA_ENO = 0x00;
    PWMA_ENO |= ENO1P; //使能输出
    PWMA_ENO |= ENO1N; //使能输出
    PWMA_ENO |= ENO2P; //使能输出
    PWMA_ENO |= ENO2N; //使能输出
    PWMA_ENO |= ENO3P; //使能输出
    PWMA_ENO |= ENO3N; //使能输出
    PWMA_ENO |= ENO4P; //使能输出
    PWMA_ENO |= ENO4N; //使能输出

    PWMA_PS = 0x00;  //高级 PWM 通道输出脚选择位
    PWMA_PS |= PWM1_0; //选择 PWM1_0 通道
    PWMA_PS |= PWM2_0; //选择 PWM2_0 通道
    PWMA_PS |= PWM3_0; //选择 PWM3_0 通道
    PWMA_PS |= PWM4_0; //选择 PWM4_0 通道
    
    UpdatePwm();

    PWMA_BKR = 0x80;  //使能主输出
    PWMA_CR1 |= 0x21; //中心对齐模式1,开始计时

//    EA = 1;     //打开总中断

    while (1);
}

//========================================================================
// 函数: UpdatePwm(void)
// 描述: 更新PWM占空比. 
// 参数: none.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(void)
{
    PWMA_CCR1H = (u8)(PWM1_Duty >> 8); //设置占空比时间
    PWMA_CCR1L = (u8)(PWM1_Duty);
    PWMA_CCR2H = (u8)(PWM2_Duty >> 8); //设置占空比时间
    PWMA_CCR2L = (u8)(PWM2_Duty);
    PWMA_CCR3H = (u8)(PWM3_Duty >> 8); //设置占空比时间
    PWMA_CCR3L = (u8)(PWM3_Duty);
    PWMA_CCR4H = (u8)(PWM4_Duty >> 8); //设置占空比时间
    PWMA_CCR4L = (u8)(PWM4_Duty);
}
