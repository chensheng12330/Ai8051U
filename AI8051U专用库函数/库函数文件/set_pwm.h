#ifndef __SET_PWM_H__
#define __SET_PWM_H__

#include "AI8051U.H"

// PWM 设备枚举
typedef enum
{
    Pwm1,Pwm2,Pwm3,Pwm4, //PWMA组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
    Pwm5,Pwm6,Pwm7,Pwm8  //PWMB组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
} pwm_name;

// 宏定义部分
#define Pwm_End "end"       // 结束标志

#define Pwm_Out_Mode "mode0"//配置pwm为输出模式，默认值
// #define Pwm_In_Mode "mode1"//暂时还不支持PWM输入捕获模式，等待后续支持

extern unsigned long pscr, arr;

//前面的是PWMxP，后面的是PWMxN，只有一个的就是PWMxP（x范围1~8）
#define Pwm1_P10_11 "pwm10"//PWM1的引脚默认值
#define Pwm1_P00_01 "pwm11"
#define Pwm1_P20_21 "pwm12"
#define Pwm2_P12_13 "pwm20"//PWM2的引脚默认值
#define Pwm2_P02_03 "pwm21"
#define Pwm2_P22_23 "pwm22"
#define Pwm3_P14_15 "pwm30"//PWM3的引脚默认值
#define Pwm3_P04_05 "pwm31"
#define Pwm3_P24_25 "pwm32"
#define Pwm4_P16_17 "pwm40"//PWM4的引脚默认值
#define Pwm4_P06_07 "pwm41"
#define Pwm4_P26_27 "pwm42"
#define Pwm5_P01 "pwm50"//PWM5的引脚默认值
#define Pwm5_P11 "pwm51"
#define Pwm5_P21 "pwm52"
#define Pwm5_P50 "pwm53"
#define Pwm6_P03 "pwm60"//PWM6的引脚默认值
#define Pwm6_P13 "pwm61"
#define Pwm6_P23 "pwm62"
#define Pwm6_P51 "pwm63"
#define Pwm7_P05 "pwm70"//PWM7的引脚默认值
#define Pwm7_P15 "pwm71"
#define Pwm7_P25 "pwm72"
#define Pwm7_P52 "pwm73"
#define Pwm8_P07 "pwm80"//PWM8的引脚默认值
#define Pwm8_P17 "pwm81"
#define Pwm8_P27 "pwm82"
#define Pwm8_P53 "pwm83"

#define En_Out_P "out1" //只打开PwmxP输出，默认值，（x范围1~8）
#define En_Out_N "out2" //只打开PwmxN输出，(Pwm5~8没有PwmxN输出，打开也是无效的)
#define En_Out_PN "out3" //打开PwmxP和PwmxN输出,互补输出
#define Dis_Out "out0" //关闭PwmxP和PwmxN输出

//可以设置Pwm1~Pwm8的周期和占空比，周期支持khz和hz单位，占空比支持0%~100%，占空比支持小数点
//例如占空比为25.7%这种也是允许的。这里的占空比是初始化的占空比，后续如果还需要改变占空比
//请使用set_pwm_duty函数，否则本函数重新计算并解析各个参数会较为耗费时间。
//注意，Pwm1~4为PWMA组，Pwm5~8为PWMB组，设置其中一路Pwm通道的周期后，该组的其他通道设定时可以不用再重复设置周期。
//如果设置了多次周期，按最后一次的设置的周期为准进行生效。
//支持死区的设置为0~1000clk，clk为Pwm的输入时钟，例如当前Pwm输入时钟40Mhz，那么设置100clk的死区
//死区的时间就是1/40Mhz*100=2.5us，需要注意的是，死区设置在超过127clk后便不是非常精准的时钟数了。
//因为原死区寄存器是8位的，在超过127的部分做了阶段性缩放，所以后面只能近似的进行设置死区时间。
//下面进行一个pwm通道的设置举例：(周期的默认值为1khz，占空比默认值为50%， 死区默认值为10clk)
//set_pwm_mode(Pwm1, Pwm_Out_Mode, Pwm1_P10_11, En_Out_P, "1khz", "50%", "10clk", Pwm_End);
//上面的程序代表的是Pwm1输出1khz的50%占空比，死区为10clk。并且只有P通道输出，N通道不输出，切换到到P10_P11输出
//当然，上面的这段设置程序也可以使用全默认值来设置：set_pwm_mode(Pwm1, Pwm_End);
//可以根据自己想要设置的部分来进行设置，其他部分可以保留默认值
void set_pwm_mode(pwm_name pwm, ...);

// 设置占空比，第一个参数是Pwm1~Pwm8，第二个参数是占空比，支持小数点，范围0%~100%
// 例如设置24.5%的占空比，可以这么写：set_pwm_duty(Pwm1, 24.5);
void set_pwm_duty(pwm_name pwm, float duty);

#endif