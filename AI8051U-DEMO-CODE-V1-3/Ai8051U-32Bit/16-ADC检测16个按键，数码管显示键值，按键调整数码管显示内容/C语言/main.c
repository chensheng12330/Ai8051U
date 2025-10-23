/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用STC的MCU的IO方式控制74HC595驱动8位数码管。

显示效果为: 数码时钟.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下,用户修改MCU主时钟频率时,自动定时于1ms.

左边4位LED显示时间(小时,分钟), 右边最后两位显示按键值.

ADC按键键码为1~16.

按键只支持单键按下, 不支持多键同时按下, 那样将会有不可预知的结果.

键按下超过1秒后,将以10键/秒的速度提供重键输出. 用户只需要检测KeyCode是否非0来判断键是否按下.

调整时间键:
键码1: 小时+.
键码2: 小时-.
键码3: 分钟+.
键码4: 分钟-.

注意：实验箱需要提供4V以上电源，才能保证MCU-VCC与SYS-VCC一致，才能采集到正常的ADC按键值.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

//==========================================================================

#define MAIN_Fosc       24000000UL
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

//========================================================================

sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

//========================================================================

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引
bit B_1ms;          //1ms标志

u8  ADC_KeyState,ADC_KeyState1,ADC_KeyState2,ADC_KeyState3; //键状态
u8  ADC_KeyHoldCnt; //键按下计时
u8  KeyCode;    //给用户使用的键码, 1~16有效
u8  cnt10ms;

u8  hour,minute,second; //RTC变量
u16 msecond;

void CalculateAdcKey(u16 adc);
u16  Get_ADC12bitResult(u8 channel); //channel = 0~15
void DisplayRTC(void);
void RTC(void);

/**********************************************/
void main(void)
{
    u8  i;
    u16 j;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x01;   P1M0 = 0x00;   //设置为准双向口, P1.0高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    display_index = 0;
    
    ADCTIM = 0x3f;		//设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;		//设置 ADC 时钟为系统时钟/2/16/16
    ADC_CONTR = 0x80;   //使能 ADC 模块

    AUXR |= 0x80;       //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;            //Timer0 interrupt enable
    TR0 = 1;            //Tiner0 run
    EA = 1;             //打开总中断
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //上电消隐

    hour   = 12;        //初始化时间值
    minute = 0;
    second = 0;
    DisplayRTC();

    ADC_KeyState  = 0;
    ADC_KeyState1 = 0;
    ADC_KeyState2 = 0;
    ADC_KeyState3 = 0;  //键状态
    ADC_KeyHoldCnt = 0; //键按下计时
    KeyCode = 0;        //给用户使用的键码, 1~16有效
    cnt10ms = 0;

    while(1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++msecond >= 1000)   //1秒到
            {
                msecond = 0;
                RTC();
                DisplayRTC();
            }
            if(msecond == 500)  DisplayRTC();   //小时后的小数点做秒闪

            if(++cnt10ms >= 10) //10ms读一次ADC
            {
                cnt10ms = 0;
                j = Get_ADC12bitResult(0);  //参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
                if(j < 4096)    CalculateAdcKey(j); //计算按键
            }

            if(KeyCode > 0)     //有键按下
            {
                LED8[6] = KeyCode / 10; //显示键码
                LED8[7] = KeyCode % 10; //显示键码

                if(KeyCode == 1)    //hour +1
                {
                    if(++hour >= 24)    hour = 0;
                    DisplayRTC();
                }
                if(KeyCode == 2)    //hour -1
                {
                    if(--hour >= 24)    hour = 23;
                    DisplayRTC();
                }
                if(KeyCode == 3)    //minute +1
                {
                    second = 0;
                    if(++minute >= 60)  minute = 0;
                    DisplayRTC();
                }
                if(KeyCode == 4)    //minute -1
                {
                    second = 0;
                    if(--minute >= 60)  minute = 59;
                    DisplayRTC();
                }
                KeyCode = 0;
            }
        }
    }
} 
/**********************************************/


/********************** 显示时钟函数 ************************/
void DisplayRTC(void)
{
    if(hour >= 10)  LED8[0] = hour / 10;
    else            LED8[0] = DIS_BLACK;
    LED8[1] = hour % 10;
    LED8[2] = minute / 10;
    LED8[3] = minute % 10;
    if(msecond >= 500)      LED8[1] |= DIS_DOT; //小时后的小数点做秒闪
}

/********************** RTC演示函数 ************************/
void RTC(void)
{
    if(++second >= 60)
    {
        second = 0;
        if(++minute >= 60)
        {
            minute = 0;
            if(++hour >= 24)    hour = 0;
        }
    }
}

//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
    ADC_START = 1;//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

/***************** ADC键盘计算键码 *****************************
电路和软件算法设计: Coody
本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
16个键,理论上各个键对应的ADC值为 (4096 / 16) * k = 256 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是4095.
但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (256*k-ADC_OFFSET) 与 (256*k+ADC_OFFSET)之间为键有效.
间隔一定的时间,就采样一次ADC,比如10ms.
为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
以上算法, 能保证读键非常可靠.
**********************************************/
#define ADC_OFFSET  64
void CalculateAdcKey(u16 adc)
{
    u8  i;
    u16 j;
    
    if(adc < (256-ADC_OFFSET))
    {
        ADC_KeyState = 0;   //键状态归0
        ADC_KeyHoldCnt = 0;
    }
    j = 256;
    for(i=1; i<=16; i++)
    {
        if((adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET)))  break;  //判断是否在偏差范围内
        j += 256;
    }
    ADC_KeyState3 = ADC_KeyState2;
    ADC_KeyState2 = ADC_KeyState1;
    if(i > 16)  ADC_KeyState1 = 0;  //键无效
    else                        //键有效
    {
        ADC_KeyState1 = i;
        if((ADC_KeyState3 == ADC_KeyState2) && (ADC_KeyState2 == ADC_KeyState1) &&
           (ADC_KeyState3 > 0) && (ADC_KeyState2 > 0) && (ADC_KeyState1 > 0))
        {
            if(ADC_KeyState == 0)   //第一次检测到
            {
                KeyCode  = i;   //保存键码
                ADC_KeyState = i;   //保存键状态
                ADC_KeyHoldCnt = 0;
            }
            if(ADC_KeyState == i)   //连续检测到同一键按着
            {
                if(++ADC_KeyHoldCnt >= 100) //按下1秒后,以10次每秒的速度Repeat Key
                {
                    ADC_KeyHoldCnt = 90;
                    KeyCode  = i;   //保存键码
                }
            }
            else ADC_KeyHoldCnt = 0; //按下时间计数归0
        }
    }
}

/**************** 向HC595发送一个字节函数 ******************/
void Send_595(u8 dat)
{
    u8  i;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        P_HC595_SER   = CY;
        P_HC595_SRCLK = 1;
        P_HC595_SRCLK = 0;
    }
}

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{   
    Send_595(t_display[LED8[display_index]]);   //输出段码
    Send_595(~T_COM[display_index]);            //输出位码

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8)    display_index = 0;  //8位结束回0
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1ms扫描显示一位
    B_1ms = 1;      //1ms标志
}
