/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用STC的MCU的IO方式控制74HC595驱动8位数码管。

显示效果为: 显示秒计数, 5秒后不喂狗, 等复位.

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

//==========================================================================

#define D_WDT_FLAG          (1<<7)
#define D_EN_WDT            (1<<5)
#define D_CLR_WDT           (1<<4)  //auto clear
#define D_IDLE_WDT          (1<<3)  //WDT counter when Idle
#define D_WDT_SCALE_2       0
#define D_WDT_SCALE_4       1
#define D_WDT_SCALE_8       2       //T=393216*N/fo
#define D_WDT_SCALE_16      3
#define D_WDT_SCALE_32      4
#define D_WDT_SCALE_64      5
#define D_WDT_SCALE_128     6
#define D_WDT_SCALE_256     7

//==========================================================================

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*************  本地常量声明    **************/
u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

/*************  IO口定义    **************/
sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  本地变量声明    **************/

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引

u16 ms_cnt;
u8  tes_cnt;    //测试用的计数变量

/*************  本地函数声明    **************/

void delay_ms(u8 ms);
void DisplayScan(void);

/****************  外部函数声明和外部变量声明 *****************/


/******************** 主函数 **************************/
void main(void)
{
    u8  i;
    
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

    RSTFLAG |= 0x04;   //设置看门狗复位需要检测P3.2的状态，否则看门狗复位后进入USB下载模式

    display_index = 0;
    for(i=0; i<8; i++)  LED8[i] = DIS_BLACK;    //全部消隐
    
    tes_cnt = 0;
    ms_cnt = 0;
    LED8[7] = ms_cnt;

    while(1)
    {
        delay_ms(1);    //延时1ms
        DisplayScan();
        if(tes_cnt <= 5)    //5秒后不喂狗, 将复位,
            WDT_CONTR = (D_EN_WDT + D_CLR_WDT + D_WDT_SCALE_16);    // 喂狗

        if(++ms_cnt >= 1000)
        {
            ms_cnt = 0;
            tes_cnt++;
            LED8[7] = tes_cnt;
        }
    }
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
