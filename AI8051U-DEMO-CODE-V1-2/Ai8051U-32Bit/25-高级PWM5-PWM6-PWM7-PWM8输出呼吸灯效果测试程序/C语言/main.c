/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

高级PWM定时器 PWM5,PWM6,PWM7,PWM8 每个通道都可独立实现PWM输出.

4个通道PWM通过P0口上连接的LED灯，观察呼吸灯效果.

PWM周期和占空比可以自定义设置，最高可达65535.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define PWM5_0      0x00	//P0.1
#define PWM5_1      0x01	//P1.1
#define PWM5_2      0x02	//P2.1
#define PWM5_3      0x03	//P5.0

#define PWM6_0      0x00	//P0.3
#define PWM6_1      0x04	//P1.3
#define PWM6_2      0x08	//P2.3
#define PWM6_3      0x0C	//P5.1

#define PWM7_0      0x00	//P0.5
#define PWM7_1      0x10	//P1.5
#define PWM7_2      0x20	//P2.5
#define PWM7_3      0x30	//P5.2

#define PWM8_0      0x00	//P0.7
#define PWM8_1      0x40	//P1.7
#define PWM8_2      0x80	//P2.7
#define PWM8_3      0xC0	//P5.3

#define ENO5P       0x01
#define ENO6P       0x04
#define ENO7P       0x10
#define ENO8P       0x40

/*****************************************************************************/


/*************  本地常量声明    **************/

#define PWM_PERIOD  1023    //设置周期值

/*************  本地变量声明    **************/

bit B_1ms;          //1ms标志

u16 PWM5_Duty;
u16 PWM6_Duty;
u16 PWM7_Duty;
u16 PWM8_Duty;

bit PWM5_Flag;
bit PWM6_Flag;
bit PWM7_Flag;
bit PWM8_Flag;

void UpdatePwm(void);

/********************* 主函数 *************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    PWM5_Flag = 0;
    PWM6_Flag = 0;
    PWM7_Flag = 0;
    PWM8_Flag = 0;
    
    PWM5_Duty = 0;
    PWM6_Duty = 256;
    PWM7_Duty = 512;
    PWM8_Duty = 1024;

    //  Timer0初始化
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    PWMB_CCER1 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMB_CCER2 = 0x00;
    PWMB_CCMR1 = 0x60; //通道模式配置
    PWMB_CCMR2 = 0x60;
    PWMB_CCMR3 = 0x60;
    PWMB_CCMR4 = 0x60;
    PWMB_CCER1 = 0x33; //配置通道输出使能和极性
    PWMB_CCER2 = 0x33;

    PWMB_CCMR1 |= 0x08; //开启PWMB_CCR5预转载功能(需要CC5E=1才可写)
    PWMB_CCMR2 |= 0x08;
    PWMB_CCMR3 |= 0x08;
    PWMB_CCMR4 |= 0x08;

    PWMB_ARRH = (u8)(PWM_PERIOD >> 8); //设置周期时间
    PWMB_ARRL = (u8)PWM_PERIOD;

    PWMB_ENO = 0x00;
    PWMB_ENO |= ENO5P; //使能输出
    PWMB_ENO |= ENO6P; //使能输出
    PWMB_ENO |= ENO7P; //使能输出
    PWMB_ENO |= ENO8P; //使能输出

    PWMB_PS = 0x00;    //高级 PWM 通道输出脚选择位
    PWMB_PS |= PWM5_0; //选择 PWM5_0 通道
    PWMB_PS |= PWM6_0; //选择 PWM6_0 通道
    PWMB_PS |= PWM7_0; //选择 PWM7_0 通道
    PWMB_PS |= PWM8_0; //选择 PWM8_0 通道

    PWMB_BKR = 0x80;   //使能主输出
    PWMB_CR1 |= 0x81;  //使能ARR预装载，开始计时

    P40 = 0;	//给LED供电
    EA = 1;     //打开总中断

    while (1);
}

/********************** Timer0 1ms中断函数 ************************/
void timer0(void) interrupt 1
{
    if(!PWM5_Flag)
    {
        PWM5_Duty++;
        if(PWM5_Duty > PWM_PERIOD) PWM5_Flag = 1;
    }
    else
    {
        PWM5_Duty--;
        if(PWM5_Duty <= 0) PWM5_Flag = 0;
    }

    if(!PWM6_Flag)
    {
        PWM6_Duty++;
        if(PWM6_Duty > PWM_PERIOD) PWM6_Flag = 1;
    }
    else
    {
        PWM6_Duty--;
        if(PWM6_Duty <= 0) PWM6_Flag = 0;
    }

    if(!PWM7_Flag)
    {
        PWM7_Duty++;
        if(PWM7_Duty > PWM_PERIOD) PWM7_Flag = 1;
    }
    else
    {
        PWM7_Duty--;
        if(PWM7_Duty <= 0) PWM7_Flag = 0;
    }

    if(!PWM8_Flag)
    {
        PWM8_Duty++;
        if(PWM8_Duty > PWM_PERIOD) PWM8_Flag = 1;
    }
    else
    {
        PWM8_Duty--;
        if(PWM8_Duty <= 0) PWM8_Flag = 0;
    }
    
    UpdatePwm();
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
    PWMB_CCR5H = (u8)(PWM5_Duty >> 8); //设置占空比时间
    PWMB_CCR5L = (u8)(PWM5_Duty);
    PWMB_CCR6H = (u8)(PWM6_Duty >> 8); //设置占空比时间
    PWMB_CCR6L = (u8)(PWM6_Duty);
    PWMB_CCR7H = (u8)(PWM7_Duty >> 8); //设置占空比时间
    PWMB_CCR7L = (u8)(PWM7_Duty);
    PWMB_CCR8H = (u8)(PWM8_Duty >> 8); //设置占空比时间
    PWMB_CCR8L = (u8)(PWM8_Duty);
}

