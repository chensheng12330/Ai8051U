/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过添加MDU32库文件实现AI8051U硬件乘除法，替换标准算法库算法

串口1(115200,N,8,1)打印计算结果

可通过屏蔽"STC32_MDU32_V1.x.LIB"文件，用示波器测量IO口低电平时间，来对比AI8051U硬件乘除法单元与标准算法库的计算效率

测量计算时间时可将串口打印指令屏蔽，方便查看每条公式的计算时间

下载时, 默认时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

#define MAIN_Fosc        24000000UL

volatile  unsigned long int near uint1, uint2;
volatile unsigned long int near xuint;

volatile long int sint1, sint2;
volatile long int xsint;

unsigned long ultest;
long ltest;

/*****************************************************************************/

sbit TPIN  =  P4^2;

/*****************************************************************************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf 使用 UART1; 2:printf 使用 UART2

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

void delay(unsigned char ms)
{
     while(--ms);
}

/*****************************************************************************/
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

    UartInit();

    printf("AI8051U MDU32 Test.\r\n");
    
    TPIN = 0;  //计算开始同步信号
    delay(2);
    TPIN = 1;
    delay(2);
    TPIN = 0;
    delay(2);
    TPIN = 1;
    delay(2);
    
    ultest = 12345678UL;
    ltest = 12345678;
    ultest = ultest / 12;
    ltest = ltest / 12;

    sint1 = 0x31030F05;
    sint2 = 0x00401350;
    TPIN = 0;
    xsint = sint1 * sint2;
    TPIN = 1;
    printf("Result1=0x%lx\r\n",xsint);

    uint1 =  5;
    uint2 =  50;
    TPIN = 0;
    xuint = uint1 * uint2;
    TPIN = 1;
    printf("Result2=%d\r\n",xuint);

    uint1 = 654689;
    uint2 = 528;
    TPIN = 0;
    xuint = uint1 / uint2;
    TPIN = 1;
    printf("Result3=%u\r\n",xuint);

    sint1 = 2134135177;
    sint2 = 20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result4=0x%lx\r\n",xsint);

    sint1 = -2134135177;
    sint2 = -20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result5=0x%lx\r\n",xsint);

    sint1 = 2134135177;
    sint2 = -20000;
    TPIN = 0;
    xsint = sint1 / sint2;
    TPIN = 1;
    printf("Result6=0x%lx\r\n",xsint);

    while(1);
}
/*****************************************************************************/
