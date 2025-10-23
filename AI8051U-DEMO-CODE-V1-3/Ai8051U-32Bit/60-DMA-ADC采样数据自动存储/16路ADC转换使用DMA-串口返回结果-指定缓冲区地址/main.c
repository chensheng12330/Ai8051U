/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************    功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

0~7通道对应P1.0~P1.7, 8~14通道对应P0.0~P0.6, 15通道为内部1.19V基准电压做输入的ADC值.

初始化时先把要ADC转换的引脚设置为高阻输入.

设置数据批量传输(DMA)功能，所有通道一次循环采集的数据自动存放到DMA定义的xdata空间.

通过串口1(P3.0 P3.1)将DMA定义的xdata空间所收到的数据发送给上位机，波特率115200,N,8,1

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

/*************    本地常量声明    **************/

#define MAIN_Fosc   22118400L    //定义主时钟
#define Baudrate    115200L
#define TM          (65536 -(MAIN_Fosc/Baudrate/4))

#define ADC_SPEED   15      /* 0~15, ADC转换时间(CPU时钟数) = (n+1)*32  ADCCFG */
#define RES_FMT     (1<<5)  /* ADC结果格式 0: 左对齐, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
                            /* ADCCFG     1: 右对齐, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

#define ADC_CH      16      /* 1~16, ADC转换通道数, 需同步修改 DMA_ADC_CHSW 转换通道 */
#define ADC_DATA    12      /* 6~n, 每个通道ADC转换数据总数, 2*转换次数+4, 需同步修改 DMA_ADC_CFG2 转换次数 */
#define DMA_ADDR    0x800   /* DMA数据存放地址 */

/*************    本地变量声明    **************/

bit DmaFlag;

u8 xdata DmaBuffer[ADC_CH][ADC_DATA] _at_ DMA_ADDR;

/*************    本地函数声明    **************/

void delay_ms(u8 ms);
void DMA_Config(void);

/******************** 串口打印函数 ********************/
void UartInit(void)
{
    S1_S1 = 0;      //UART1 switch to, 00: P3.0 P3.1, 01: P3.6 P3.7, 10: P1.6 P1.7, 11: P4.3 P4.4
    S1_S0 = 0;
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //定时器时钟1T模式
    S1BRT = 0;      //串口1选择定时器1为波特率发生器
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //定时器1开始计时
}

void UartPutc(unsigned char dat)
{
    SBUF = dat; 
    while(TI == 0);
    TI = 0;
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}

/**********************************************/
void main(void)
{
    u8 i,n;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x7f;   P0M0 = 0x00;   //设置为高阻输入
    P1M1 = 0xff;   P1M0 = 0x00;   //设置为高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    ADCTIM = 0x3f;  //设置通道选择时间、保持时间、采样时间
    ADCCFG = RES_FMT + ADC_SPEED;
    //ADC模块电源打开后，需等待1ms，MCU内部ADC电源稳定后再进行AD转换
    ADC_CONTR = 0x80 + 0;    //ADC on + channel

    UartInit();
    DMA_Config();
    EA = 1;
    printf("AI8051U系列ADC DMA测试程序!\r\n");

    while (1)
    {
        delay_ms(200);
        if(DmaFlag)
        {
            DmaFlag = 0;
            for(i=0; i<ADC_CH; i++)
            {
                for(n=0; n<ADC_DATA; n++)
                {
                    printf("0x%02x ",DmaBuffer[i][n]);
                }
                printf("\r\n");
            }
            printf("\r\n");
            DMA_ADC_CR = 0xc0;  //bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
        }
    }
}

//========================================================================
// 函数: void DMA_Config(void)
// 描述: ADC DMA 功能配置.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_ADC_STA = 0x00;
    DMA_ADC_CFG = 0x80;     //bit7 1:Enable Interrupt
    DMA_ADC_RXAH = (u8)(DMA_ADDR >> 8);    //ADC转换数据存储地址
    DMA_ADC_RXAL = (u8)DMA_ADDR;
    DMA_ADC_CFG2 = 0x09;    //每个通道ADC转换次数:4
    DMA_ADC_CHSW0 = 0xff;   //ADC通道使能寄存器 ADC7~ADC0
    DMA_ADC_CHSW1 = 0xff;   //ADC通道使能寄存器 ADC15~ADC8
    DMA_ADC_CR = 0xc0;      //bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do
    {
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

//========================================================================
// 函数: void ADC_DMA_Interrupt (void) interrupt 48
// 描述: ADC DMA中断函数
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-5-8
// 备注: 
//========================================================================
void ADC_DMA_Interrupt(void) interrupt 13
{
    DMA_ADC_STA = 0;
    DmaFlag = 1;
}
