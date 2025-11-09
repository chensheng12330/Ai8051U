/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

在组合 PWM 模式下，生成的两个边沿或中心对齐 PWM 信号的各个脉冲间允许存在可编程延时和相移。
频率由 PWMA_ARR 寄存器的值确定，而占空比和延时则由两个 PWMA_CCRx 寄存器确定。
产生的信号 OCxREFC 由两个参考 PWM 的逻辑或运算或者逻辑与运算组合组成。
– OC1REFC（或 OC2REFC）由 PWMA_CCR1 和 PWMA_CCR2 控制
– OC3REFC（或 OC4REFC）由 PWMA_CCR3 和 PWMA_CCR4 控制
两个通道可以独立选择组合 PWM 模式（每对 CCR 寄存器一个 OCx 输出），
只需向 PWMA_CCMRx 寄存器的 OCxM 位写入“1100”（组合 PWM 模式 1）或“1101”（组合 PWM 模式 2）。

当给定通道用作组合 PWM 通道时，其互补通道必须在相反的 PWM 模式下配置
（例如，一 通道在组合 PWM 模式 1 下配置，另一个通道在组合 PWM 模式 2 下配置）。

注： 出于兼容性原因，OCxM[3:0] 位域分为两部分，最高有效位与最低有效的 3 位不相邻。
通过以下配置可获得这些信号：
– 通道 1 在组合 PWM 模式 2 下配置。
– 通道 2 在 PWM 模式 1 下配置。
– 通道 3 在组合 PWM 模式 2 下配置。
– 通道 4 在 PWM 模式 1 下配置。

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

    //组合模式测试-1
    PWMA_CCMR1X = 0x01; //通道1:组合模式2
    PWMA_CCMR1 = 0x50;
    PWMA_CCMR2X = 0x00; //通道2:PWM模式2
    PWMA_CCMR2 = 0x70;
    PWMA_CCMR3X = 0x01; //通道3:组合模式2
    PWMA_CCMR3 = 0x50;
    PWMA_CCMR4X = 0x00; //通道4:PWM模式1
    PWMA_CCMR4 = 0x60;

    //组合模式测试-2
//    PWMA_CCMR1X = 0x01; //通道1:组合模式1
//    PWMA_CCMR1 = 0x40;
//    PWMA_CCMR2X = 0x00; //通道2:PWM模式1
//    PWMA_CCMR2 = 0x60;
//    PWMA_CCMR3X = 0x01; //通道3:组合模式2
//    PWMA_CCMR3 = 0x50;
//    PWMA_CCMR4X = 0x00; //通道4:PWM模式2
//    PWMA_CCMR4 = 0x70;

    //组合模式测试-3
//    PWMA_CCMR1X = 0x00; //通道1:PWM模式1
//    PWMA_CCMR1 = 0x60;
//    PWMA_CCMR2X = 0x01; //通道2:不对称模式1
//    PWMA_CCMR2 = 0x60;
//    PWMA_CCMR3X = 0x00; //通道3:PWM模式1
//    PWMA_CCMR3 = 0x60;
//    PWMA_CCMR4X = 0x01; //通道4:不对称模式1
//    PWMA_CCMR4 = 0x60;
//    PWMA_CR1 |= 0x30; //中心对齐模式1，向下计数

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
    PWMA_CR1 |= 0x01; //开始计时

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
