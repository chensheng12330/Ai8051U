/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

显示效果为: 上电后跑马灯显示2秒, 然后进入睡眠模式.

按板上的P32、P33、P34、P35按键唤醒, 继续显示2秒后再进入睡眠模式.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

//==========================================================================

#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf 使用 UART1; 2:printf 使用 UART2

/*************  本地常量声明    **************/

u8 code ledNum[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

/*************  本地变量声明    **************/

u8 ledIndex;
u16 msecond;    //1000ms计数
u8 ioIndex; 

/*************  本地函数声明    **************/

void delay_ms(u8 ms);
void UartInit(void);

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

    P40 = 0;    //LED Power On

    //PnIM1,PnIM0  00:下降沿中断; 01:上升沿中断; 10:低电平中断; 11:高电平中断
//    P0IM0 = 0xff;  //高电平中断
//    P0IM1 = 0xff; 
//    P0INTE = 0xff; //使能 P0 口中断
//    P0WKUE = 0xff; //使能 P0 口中断唤醒

//    P1IM0 = 0xff;  //上升沿中断
//    P1IM1 = 0x00; 
//    P1INTE = 0xff; //使能 P1 口中断
//    P1WKUE = 0xff; //使能 P1 口中断唤醒

//    P2IM0 = 0x00;  //低电平中断
//    P2IM1 = 0xff; 
//    P2INTE = 0xff; //使能 P2 口中断
//    P2WKUE = 0xff; //使能 P2 口中断唤醒

    P3IM0 = 0x00;  //下降沿中断
    P3IM1 = 0x00; 
    P3INTE = 0x3c; //使能 P3.2~P3.5 口中断
    P3WKUE = 0x3c; //使能 P3.2~P3.5 口中断唤醒

//    P4IM0 = 0x00;  //下降沿中断
//    P4IM1 = 0x00; 
//    P4INTE = 0xff; //使能 P4 口中断
//    P4WKUE = 0xff; //使能 P4 口中断唤醒

//    P5IM0 = 0x00;  //下降沿中断
//    P5IM1 = 0x00; 
//    P5INTE = 0xff; //使能 P5 口中断
//    P5WKUE = 0xff; //使能 P5 口中断唤醒

    UartInit();
    
//    IRCDB = 0x10;

    P0INTF = 0;     //清中断标志
    P1INTF = 0;
    P2INTF = 0;
    P3INTF = 0;
    P4INTF = 0;
    P5INTF = 0;
    EA = 1;     //允许总中断

    while(1)
    {
        delay_ms(100);      //延时100ms
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
            printf("MCU Sleep.\r\n");

            PD = 1;         //Sleep
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            _nop_();
            printf("MCU wakeup from P%02X.\r\n", ioIndex);
        }
    }
}

//========================================================================
//由于中断向量大于 31，在 KEIL 中无法直接编译
//借用第 13 号中断入口地址，需添加"isr.asm"文件到项目
//========================================================================
void common_isr() interrupt 13 
{ 
    u8 intf; 

    intf = P0INTF; //P0 口中断
    if (intf) 
    { 
        P0INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x00;
        if (intf & 0x02) ioIndex = 0x01;
        if (intf & 0x04) ioIndex = 0x02;
        if (intf & 0x08) ioIndex = 0x03;
        if (intf & 0x10) ioIndex = 0x04;
        if (intf & 0x20) ioIndex = 0x05;
        if (intf & 0x40) ioIndex = 0x06;
        if (intf & 0x80) ioIndex = 0x07;
    } 

    intf = P1INTF; //P1 口中断
    if (intf) 
    { 
        P1INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x10;
        if (intf & 0x02) ioIndex = 0x11;
        if (intf & 0x04) ioIndex = 0x12;
        if (intf & 0x08) ioIndex = 0x13;
        if (intf & 0x10) ioIndex = 0x14;
        if (intf & 0x20) ioIndex = 0x15;
        if (intf & 0x40) ioIndex = 0x16;
        if (intf & 0x80) ioIndex = 0x17;
    } 

    intf = P2INTF; //P2 口中断
    if (intf) 
    { 
        P2INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x20;
        if (intf & 0x02) ioIndex = 0x21;
        if (intf & 0x04) ioIndex = 0x22;
        if (intf & 0x08) ioIndex = 0x23;
        if (intf & 0x10) ioIndex = 0x24;
        if (intf & 0x20) ioIndex = 0x25;
        if (intf & 0x40) ioIndex = 0x26;
        if (intf & 0x80) ioIndex = 0x27;
    } 

    intf = P3INTF; //P3 口中断
    if (intf) 
    { 
        P3INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x30;
        if (intf & 0x02) ioIndex = 0x31;
        if (intf & 0x04) ioIndex = 0x32;
        if (intf & 0x08) ioIndex = 0x33;
        if (intf & 0x10) ioIndex = 0x34;
        if (intf & 0x20) ioIndex = 0x35;
        if (intf & 0x40) ioIndex = 0x36;
        if (intf & 0x80) ioIndex = 0x37;
    } 

    intf = P4INTF; //P4 口中断
    if (intf) 
    { 
        P4INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x40;
        if (intf & 0x02) ioIndex = 0x41;
        if (intf & 0x04) ioIndex = 0x42;
        if (intf & 0x08) ioIndex = 0x43;
        if (intf & 0x10) ioIndex = 0x44;
        if (intf & 0x20) ioIndex = 0x45;
        if (intf & 0x40) ioIndex = 0x46;
        if (intf & 0x80) ioIndex = 0x47;
    } 

    intf = P5INTF; //P5 口中断
    if (intf) 
    { 
        P5INTF = 0x00; 
        if (intf & 0x01) ioIndex = 0x50;
        if (intf & 0x02) ioIndex = 0x51;
        if (intf & 0x04) ioIndex = 0x52;
        if (intf & 0x08) ioIndex = 0x53;
        if (intf & 0x10) ioIndex = 0x54;
        if (intf & 0x20) ioIndex = 0x55;
        if (intf & 0x40) ioIndex = 0x56;
        if (intf & 0x80) ioIndex = 0x57;
    } 
}

//========================================================================
// 函数: void delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2025-1-10
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

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //定时器时钟1T模式
    S1BRT = 0;      //串口1选择定时器1为波特率发生器
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //定时器1开始计时

//    SCON = (SCON & 0x3f) | 0x40; 
//    T2L  = TM;
//    T2H  = TM>>8;
//    AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
    S2_S = 1;       //UART2 switch to: 0: P1.0 P1.1,  1: P4.6 P4.7
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
    S2CON = (S2CON & 0x3f) | 0x40; 
    T2L  = TM;
    T2H  = TM>>8;
    AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
    SBUF = dat; 
    while(TI==0);
    TI = 0;
#else
    S2BUF  = dat; 
    while(S2TI == 0);
    S2TI = 0;       //Clear Tx flag
#endif
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}
