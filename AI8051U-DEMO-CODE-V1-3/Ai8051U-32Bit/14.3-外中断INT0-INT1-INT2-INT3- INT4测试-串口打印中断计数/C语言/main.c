/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试.

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用串口打印INT中断计数结果（INT0与数码管控制接口复用，不能用数码管显示）。

串口(P30,P31)默认设置：115200,N,8,1，使用文本模式打印.

由于按键是机械按键, 按下有抖动, 而本例程没有去抖动处理, 所以按一次有多个计数也是正常的.

INT2, INT3, INT4 实验板上没有引出测试按键，供需要时参考使用.

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
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf 使用 UART1; 2:printf 使用 UART2

/*************  本地变量声明    **************/

bit int0Flag;
bit int1Flag;
bit int2Flag;
bit int3Flag;
bit int4Flag;
u8  INT0_cnt, INT1_cnt; //测试用的计数变量
u8  INT2_cnt, INT3_cnt, INT4_cnt; //测试用的计数变量

/******************** 串口打印函数 ********************/
void UartInit(void)
{
#if(PrintUart == 1)
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
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
    S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
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
    S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}

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
    
    P3PU = 0x0c;    //P3.2,P3.3使能内部上拉

    UartInit();
    
    INT0_cnt = 0;
    INT1_cnt = 0;

    IE1  = 0;   //外中断1标志位
    IE0  = 0;   //外中断0标志位
    EX1 = 1;    //INT1 Enable
    EX0 = 1;    //INT0 Enable

    IT0 = 1;    //INT0 下降沿中断
//  IT0 = 0;    //INT0 上升,下降沿中断  
    IT1 = 1;    //INT1 下降沿中断
//  IT1 = 0;    //INT1 上升,下降沿中断  

    //INT2, INT3, INT4 实验板上没有引出测试按键，供需要时参考使用
    EX2 = 1;    //使能 INT2 下降沿中断
    EX3 = 1;    //使能 INT3 下降沿中断
    EX4 = 1;    //使能 INT4 下降沿中断

    EA = 1;     //允许总中断

    while(1)
    {
        if(int0Flag)
        {
            int0Flag = 0;
            printf("int0 cnt=%bu\r\n",INT0_cnt);
        }

        if(int1Flag)
        {
            int1Flag = 0;
            printf("int1 cnt=%bu\r\n",INT1_cnt);
        }
    }
}

/********************* INT0中断函数 *************************/
void INT0_int (void) interrupt 0      //进中断时已经清除标志
{
    INT0_cnt++; //中断+1
    int0Flag = 1;
}

/********************* INT1中断函数 *************************/
void INT1_int (void) interrupt 2      //进中断时已经清除标志
{
    INT1_cnt++; //中断+1
    int1Flag = 1;
}

/********************* INT2中断函数 *************************/
void INT2_int (void) interrupt 10     //进中断时已经清除标志
{
    INT2_cnt++; //中断+1
    int2Flag = 1;
}

/********************* INT3中断函数 *************************/
void INT3_int (void) interrupt 11     //进中断时已经清除标志
{
    INT3_cnt++; //中断+1
    int3Flag = 1;
}

/********************* INT4中断函数 *************************/
void INT4_int (void) interrupt 16     //进中断时已经清除标志
{
    INT4_cnt++; //中断+1
    int4Flag = 1;
}
