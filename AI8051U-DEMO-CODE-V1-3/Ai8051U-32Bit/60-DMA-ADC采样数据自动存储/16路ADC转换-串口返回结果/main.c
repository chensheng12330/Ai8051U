/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************    功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

演示15路ADC和bandgap查询采样，串口1(P3.0 P3.1)发送给上位机，波特率115200,N,8,1.

0~7通道对应P1.0~P1.7, 8~14通道对应P0.0~P0.6, 15通道为内部1.19V基准电压做输入的ADC值.

初始化时先把要ADC转换的引脚设置为高阻输入.

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc     22118400L  //定义主时钟
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

/*************    本地常量声明    **************/

#define    ADC_SPEED    15        /* 0~15, ADC转换时间(CPU时钟数) = (n+1)*32  ADCCFG */
#define    RES_FMT      (1<<5)    /* ADC结果格式 0: 左对齐, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
                                  /* ADCCFG      1: 右对齐, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

/*************    本地变量声明    **************/


/*************    本地函数声明    **************/

void delay_ms(u8 ms);
void ADC_convert(u8 chn);    //chn=0~7对应P1.0~P1.7, chn=8~14对应P0.0~P0.6, chn=15对应BandGap电压
u16  Get_ADC12bitResult(u8 channel);

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
    u8    i;

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

    ADCTIM = 0x3f;  //设置通道选择时间、保持时间、采样时间
    ADCCFG = RES_FMT + ADC_SPEED;
    //ADC模块电源打开后，需等待1ms，MCU内部ADC电源稳定后再进行AD转换
    ADC_CONTR = 0x80 + 0;    //ADC on + channel

    UartInit();
    EA = 1;
    printf("AI8051U系列ADC测试程序!\r\n");

    while (1)
    {
        for(i=0; i<16; i++)
        {
            delay_ms(200);
            //ADC_convert(3);        //发送固定通道AD值

            ADC_convert(i);        //发送轮询通道AD值
            if((i & 7) == 7)    //分两行打印
            {
                printf("\r\n");
            }
        }
    }
}


//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel))    //channel = 0~15
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC, 0~15.
// 返回: 12位ADC结果.
// 版本: V1.0, 2016-4-28
//========================================================================
u16 Get_ADC12bitResult(u8 channel)    //channel = 0~15
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
    return (((u16)ADC_RES << 8) | ADC_RESL);
}

#define SUM_LENGTH    16    /* 平均值采样次数 最大值16 */

/***********************************
查询方式做一次ADC, chn为通道号, chn=0~7对应P1.0~P1.7, chn=8~14对应P0.0~P0.6, chn=15对应BandGap电压.
***********************************/
void ADC_convert(u8 chn)
{
    u16 j;
    u8  k;        //平均值滤波时使用

    Get_ADC12bitResult(chn);    //参数i=0~15,查询方式做一次ADC, 切换通道后第一次转换结果丢弃. 避免采样电容的残存电压影响.
    Get_ADC12bitResult(chn);    //参数i=0~15,查询方式做一次ADC, 切换通道后第二次转换结果丢弃. 避免采样电容的残存电压影响.
    for(k=0, j=0; k<SUM_LENGTH; k++)    j += Get_ADC12bitResult(chn);    // 采样累加和, 参数0~15,查询方式做一次ADC, 返回值就是结果
    j = j / SUM_LENGTH;        // 求平均

    if(chn == 15)    printf("Bandgap=%04d  ",j);    //内基准 1.19V
    else        //ADC0~ADC14
    {
        printf("ADC%02d=%04d  ",chn,j);
    }
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
