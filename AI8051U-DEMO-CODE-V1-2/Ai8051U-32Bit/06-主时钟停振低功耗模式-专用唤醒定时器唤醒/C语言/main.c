/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

显示效果为: 上电后跑马灯显示2秒, 然后睡眠2秒, 醒来再跑马灯显示2秒,一直重复.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

/*************  本地常量声明    **************/

#define MAIN_Fosc        24000000UL

u8 code ledNum[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*************  本地变量声明    **************/

u8 ledIndex;
u16 msecond;        //1000ms计数

/*************  本地函数声明    **************/

void delay_ms(u8 ms);
void SetWakeUpTime(u16 SetTime);

/****************  外部函数声明和外部变量声明 *****************/


/******************** 主函数 **************************/
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

    SetWakeUpTime(2000);    //2秒后醒来
    P40 = 0;        //LED Power On

    while(1)
    {
        delay_ms(100);    //延时100ms

        //跑马灯指示工作状态
        P0 = ~ledNum[ledIndex];    //输出低驱动
        ledIndex++;
        if(ledIndex > 7)
        {
            ledIndex = 0;
        }

        //2秒后MCU进入休眠状态
        if(++msecond >= 20)
        {
            msecond = 0;    //清计数
            P0 = 0xff;      //先关闭显示，省电

            PD = 1;         //Sleep
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
        }
    }
}

//========================================================================
// 函数: void SetWakeUpTime(u16 SetTime)
// 描述: 唤醒定时器设置时间值函数。
// 参数: SetTime: 要设置的时间值(睡眠的时间), 单位为ms.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-7-15
// 备注: 
//========================================================================
void SetWakeUpTime(u16 SetTime)
{
    SetTime = (u16)((32768UL * (u32)SetTime) / 16000);  //重装值 = Fwkt/16 * SetTime/1000 = Fwkt * SetTime / 16000
    if(SetTime > 0) SetTime--;
    WKTCL = (u8)SetTime;
    WKTCH = (u8)(SetTime >> 8) | 0x80;
}

//========================================================================
// 函数: void delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2023-4-1
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}
