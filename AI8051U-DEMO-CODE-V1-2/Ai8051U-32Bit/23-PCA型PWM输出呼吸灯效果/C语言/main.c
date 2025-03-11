/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************	功能说明	**************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

程序演示芯片PCA输出呼吸灯效果PWM信号.

PCA0从P4.2口输出8位PWM信号，可通过实验箱LED11查看效果.

PCA1从P4.3口输出10位PWM信号，可通过实验箱LED10查看效果.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

#define     MAIN_Fosc       24000000UL  //定义主时钟
#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 12 / 400))       //Timer 0 中断频率, 400次/秒

#define	PCA0			0
#define	PCA1			1
#define	PCA2			2

bit	B_PWM0_Dir;	//方向, 0为+, 1为-.
bit	B_PWM1_Dir;	//方向, 0为+, 1为-.
u16	pwm0;
u16	pwm1;

void Timer0_config(void);
void PWM_config(void);
void UpdatePwm(u8 PCA_id, u16 pwm_value);

void main()
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

    Timer0_config();
    PWM_config();
    EA = 1;      //开启总中断

    //初始化变量
    pwm0 = 128;
    B_PWM0_Dir = 0;
    UpdatePwm(PCA0,pwm0);

    pwm1 = 128;
    B_PWM1_Dir = 0;
    UpdatePwm(PCA1,pwm1);

    while (1)
    {
    }
}

//========================================================================
// 函数: void Timer0_config(void)
// 描述: Timer0初始化函数。
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2020-6-10
//========================================================================
void Timer0_config(void)
{
    AUXR &= 0x7F;   //Timer0 set as 12T, 16 bits timer auto-reload
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;        //Timer0 interrupt enable
    TR0 = 1;        //Tiner0 run
}

//========================================================================
// 函数: void PWM_config(void)
// 描述: PWM初始化函数。
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2020-6-10
//========================================================================
void PWM_config(void)
{
    //bit[6:5]=0: ECI/P1.2, CCP0/P1.3, CCP1/P1.4, CCP2/P1.1
    //bit[6:5]=1: ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    //bit[6:5]=2: ECI/P2.3, CCP0/P2.0, CCP1/P2.1, CCP2/P2.2
    CMOD = (CMOD & 0x9f) | (1<<5);

    CCON = 0x00;
    CMOD |= 0x0e;   //PCA 时钟为系统时钟/8
    CL = 0x00;
    CH = 0x00;

    CCAPM0 = 0x42;  //PCA 模块 0 为 PWM 工作模式
    PCA_PWM0 = 0x00;//PCA 模块 0 输出 8 位 PWM
    CCAP0L = 0x80;
    CCAP0H = 0x80;  //设置 PWM 初始占空比

    CCAPM1 = 0x42;  //PCA 模块 1 为 PWM 工作模式
    PCA_PWM1 = 0xc0;//PCA 模块 1 输出 10 位 PWM
    CCAP1L = 0x40;
    CCAP1H = 0x40;  //设置 PWM 初始占空比

    CCAPM2 = 0x42;  //PCA 模块 2 为 PWM 工作模式
    PCA_PWM2 = 0x80;//PCA 模块 2 输出 6 位 PWM
    CCAP2L = 0x20;
    CCAP2H = 0x20;  //设置 PWM 初始占空比

    CCON |= 0x40;   //启动 PCA 计时器
}

//========================================================================
// 函数: UpdatePwm(u8 PCA_id, u16 pwm_value)
// 描述: 更新PWM值. 
// 参数: PCA_id: PCA序号. 取值 PCA0,PCA1,PCA2,PCA_Counter
//		 pwm_value: pwm值, 这个值是输出低电平的时间.
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void UpdatePwm(u8 PCA_id, u16 pwm_value)
{
    if(PCA_id == PCA0)
    {
        PCA_PWM0 = (PCA_PWM0 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP0H = (u8)pwm_value;
    }
    else if(PCA_id == PCA1)
    {
        PCA_PWM1 = (PCA_PWM1 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP1H = (u8)pwm_value;
    }
    else if(PCA_id == PCA2)
    {
        PCA_PWM2 = (PCA_PWM2 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
        CCAP2H = (u8)pwm_value;
    }
}

/********************** Timer0 中断函数 ************************/
void timer0(void) interrupt 1
{
    if(B_PWM0_Dir)
    {
        if(--pwm0 <= 1)	B_PWM0_Dir = 0;
    }
    else if(++pwm0 >= 255)	B_PWM0_Dir = 1;

    UpdatePwm(PCA0,pwm0);   //调节 PCA0 占空比

    if(B_PWM1_Dir)
    {
        if(--pwm1 <= 1)	B_PWM1_Dir = 0;
    }
    else if(++pwm1 >= 1023)	B_PWM1_Dir = 1;

    UpdatePwm(PCA1,pwm1);   //调节 PCA1 占空比
}
