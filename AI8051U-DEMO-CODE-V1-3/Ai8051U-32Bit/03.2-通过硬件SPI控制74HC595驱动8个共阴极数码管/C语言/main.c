/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试.

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用STC的MCU的SPI接口控制74HC595驱动8位数码管。

显示效果为: 数码时钟.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

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

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

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
sbit P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  本地变量声明    **************/

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引
bit B_1ms;          //1ms标志

u8  hour,minute,second;
u16 msecond;


/*************  本地函数声明    **************/

void SPI_init(void);

/****************  外部函数声明和外部变量声明 *****************/


/********************** 显示时钟函数 ************************/
void DisplayRTC(void)
{
    if(hour >= 10)  LED8[0] = hour / 10;
    else            LED8[0] = DIS_BLACK;
    LED8[1] = hour % 10;
    LED8[2] = DIS_;
    LED8[3] = minute / 10;
    LED8[4] = minute % 10;
    LED8[5] = DIS_;
    LED8[6] = second / 10;
    LED8[7] = second % 10;
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

    SPI_init();
    
    T0x12 = 1;  //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    EA = 1;     //打开总中断
    
    display_index = 0;
    hour   = 11;    //初始化时间值
    minute = 59;
    second = 58;
    RTC();
    DisplayRTC();
    
//  for(i=0; i<8; i++)  LED8[i] = DIS_BLACK;    //上电消隐
    for(i=0; i<8; i++)  LED8[i] = i;    //显示01234567

    while(1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++msecond >= 1000)   //1秒到
            {
//                P42 = !P42;
                msecond = 0;
                RTC();
                DisplayRTC();
            }
        }
    }
}

/**************** SPI初始化程序 ******************/
void SPI_init(void)
{
    SSIG = 1; //忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
    SPEN = 1; //使能 SPI 功能
    DORD = 0; //先发送/接收数据的高位（ MSB）
    MSTR = 1; //设置主机模式
    CPOL = 1; //SCLK 空闲时为高电平，SCLK 的前时钟沿为下降沿，后时钟沿为上升沿
    CPHA = 1; //数据在 SCLK 前时钟沿驱动，后时钟沿采样
    //采用漏极开路+上拉电阻方式驱动，需要调低SPI频率才能正常与Flash通信
    SPCTL = (SPCTL & ~3) | 2;   //SPI 时钟频率选择, 0: 4T, 1: 8T,  2: 16T,  3: 2T
    SPI_S1 = 1;     //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    SPI_S0 = 1;

    HSCLKDIV = 0x01;    //高速时钟1分频，默认2分频
    
    P_HC595_SRCLK = 0;  // set clock to low initial state
    P_HC595_SER = 1;
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
}

/**************** 向HC595发送一个字节函数 ******************/
void Send_595(u8 dat)
{
    P_HC595_RCLK = 0;
    SPDAT = dat;
    while(SPIF == 0) ;
    P_HC595_RCLK = 1;
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
}

/********************** 显示扫描函数 ************************/
void DisplayScan(void)
{
    Send_595(t_display[LED8[display_index]]);   //输出段码
    Send_595(~T_COM[display_index]);            //输出位码

    if(++display_index >= 8)    display_index = 0;  //8位结束回0
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt 1
{
    DisplayScan();  //1ms扫描显示一位
    B_1ms = 1;      //1ms标志

}
