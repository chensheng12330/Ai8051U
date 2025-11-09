#ifndef __SET_PWM_H__
#define __SET_PWM_H__

#include "AI8051U.H"

// PWM 设备枚举
typedef enum
{
    Pwm1,Pwm2,Pwm3,Pwm4, //PWMA组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
    Pwm5,Pwm6,Pwm7,Pwm8  //PWMB组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
} pwm_name;

typedef enum
{
    Pwm1_Pwm2,Pwm3_Pwm4, //PWMA组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
    Pwm5_Pwm6,Pwm7_Pwm8  //PWMB组，设置的周期必须一致，设置多个不同的周期按照最后一个生效
} pwm_inx;

typedef struct
{
    char sync_pwm;//同步捕获标志位
    unsigned int cycl;//计算得到的周期
    unsigned int dat;//捕获数据数据流
}cap_dat;

// 宏定义部分
#define Pwm_End "end"       // 结束标志

#define Pwm_Out_Mode "mode0"//配置pwm为输出模式，默认值
#define Pwm_In_Mode "mode1"//配置pwm为输入模式，同一组可以既输出也输入
//例如pwm1配置为输出，pwm2配置为输入这样是可以的
//但是同一个pwm不能即配置为输出也配置为输入，会按照最后一个的配置生效

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

/*以下是PWM通道配置在输出模式下可以设定的配置*/
#define En_Out_P "out1" //只打开PwmxP输出，默认值，（x范围1~8）
#define En_Out_N "out2" //只打开PwmxN输出，(Pwm5~8没有PwmxN输出，打开也是无效的)
#define En_Out_PN "out3" //打开PwmxP和PwmxN输出,互补输出
#define Dis_Out "out0" //关闭PwmxP和PwmxN输出

#define Out_CPWM "pol0" //设置输出极性为PwmxP通道高电平，PwmxN通道低电平有效，即正常互补PWM，默认值
#define Out_IPWM "pol1" //设置PwmxN输出极性为跟随PwmxP极性，即PwmxN输出跟PwmxP一样，即同相PWM，但是带有死区
//输出极性设定通常适用于互补信号的极性设定，在PWMB组（PWM5~8）这样不支持互补信号输出的情况，互补或者同相设定无效

#define Self_Capture "ic1" //设置输入捕获为自己对应的Pwm通道，默认值
#define Cross_Capture "ic2" //设置输入捕获为旁边的Pwm通道
//PWM1~8中，两两一组，例如PWM1和PWM2为同一组，PWM1设置自身捕获通道对应到PWM1管脚，设置交叉通道则对应到PWM2管脚
//而PWM2设置自身捕获通道为PWM2管脚，设置交叉通道则对应到PWM1管脚，其他的依次类推
//将两个捕获外设设置到同一个管脚，就可以实现同时捕获一个信号的上升沿和下降沿，从而实现捕获占空比信号
/*以上是PWM通道配置在输出模式下可以设定的配置*/


/*以下是PWM通道配置在输入模式下可以设定的配置*/
#define En_In_P "in1" //输入只允许从PwmxP输入，PwmxN无法捕获，配置为输入时的默认值
#define Dis_In "in0" //关闭Pwmx输入

#define Cap_In_Rising_Edge "cap0" //设置输入捕获为上升沿捕获，默认值
#define Cap_In_Falling_Edge "cap1" //设置输入捕获为下降沿捕获

/*以上是PWM通道配置在输入模式下可以设定的配置*/


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

extern unsigned int pwma_max, pwmb_max;//pwma组和pwmb组的最大值，ARR寄存器值

// PWM捕获返回值单位枚举
typedef enum
{
    khz = 0,//频率单位，千赫兹
    hz,//频率单位，赫兹
    ms,//时间单位，毫秒
    us//时间单位，微秒
} dat_unit;//返回数据单位

//计算捕获周期，第一个传入参数是Pwm1~Pwm8，对应通道应该在前面初始化为输入模式，否则出现异常情况会返回-1
//第二个参数可以通过上面的参数来指定单位，khz和hz是周期模式，返回的float就是对应单位的值
//需要注意的是，如果查询的时候发现捕获尚未完成或者捕获溢出，则会返回-1，注意处理异常情况
//第二个参数还可以指定为时间单位，可以指定为ms和us，分别表示毫秒和微秒，返回的float就是对应单位的值
float get_pwm_period(pwm_name pwm, dat_unit unit);


// 计算捕获占空比，传入参数是Pwm1_Pwm2~Pwm7_Pwm8，对应通道应该在前面初始化为输入模式，否则出现异常情况会返回-1
// 捕获占空比需要占用两个相邻的Pwm外设，会自动计算出占空比，返回范围0~100%
// 需要注意的是，如果查询的时候发现捕获尚未完成或者捕获溢出，则会返回-1，注意处理异常情况
// 示例：duty = get_pwm_duty(Pwm1_Pwm2);//计算pwm1捕获和pwm2捕获时的占空比，自动根据捕获数据大小确定占空比
float get_pwm_duty(pwm_inx pwm);


//用于设置PWM部分的时钟源频率，fosc为时钟频率，单位为Hz。
//不使用这个函数的情况下，主频是自动获取，用户可以不用关心这个问题。
void set_pwm_fosc(long fosc);
#endif