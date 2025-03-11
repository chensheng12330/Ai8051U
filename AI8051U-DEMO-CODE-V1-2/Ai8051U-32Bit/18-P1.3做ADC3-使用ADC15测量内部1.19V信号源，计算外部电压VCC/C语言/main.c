/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

读ADC测量外部电压，使用内部基准计算电压.

用STC的MCU的IO方式控制74HC595驱动8位数码管。

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

右边4位数码管显示测量的电压值.

串口1(P3.0,P3.1)配置：115200,N,8,1，使用文本模式打印电压值.

外部电压从板上测温电阻两端输入, 输入电压0~Vref, 不要超过Vref或低于0V. 

实际项目使用请串一个1K的电阻到ADC输入口, ADC输入口再并一个102~103电容到地.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/****************************** 用户定义宏 ***********************************/
#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf 使用 UART1; 2:printf 使用 UART2
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

/*****************************************************************************/
#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*****************************************************************************/

    #define Cal_MODE    0   //每次测量只读1次ADC. 分辨率0.01V
//  #define Cal_MODE    1   //每次测量连续读16次ADC 再平均计算. 分辨率0.01V
	
/*****************************************************************************/

/*************  IO口定义    **************/
sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  本地常量声明    **************/
u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

/*************  本地变量声明    **************/
u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引
bit B_1ms;          //1ms标志

u16 msecond;
u16 Bandgap;    //

/*************  本地函数声明    **************/
u16 Get_ADC12bitResult(u8 channel); //channel = 0~15
void UartInit(void);

/********************* 主函数 *************************/
void main(void)
{
    u8  i;
    u16 j;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x08;   P1M0 = 0x00;   //设置为准双向口,P1.3设置高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x02;   //设置为准双向口,P5.1设置推挽输出
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    display_index = 0;

    ADCTIM = 0x3f;      //设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;      //设置 ADC 时钟为系统时钟/2/16
    ADC_CONTR = 0x80;   //使能 ADC 模块

    AUXR |= 0x80;   //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;        //Timer0 interrupt enable
    TR0 = 1;        //Tiner0 run

    UartInit();
    EA = 1;         //打开总中断
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //上电消隐
    P51 = 1;        //给NTC供电

    while(1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++msecond >= 300)    //300ms到
            {
                msecond = 0;

            #if (Cal_MODE == 0)
            //=================== 只读1次ADC, 12bit ADC. 分辨率0.01V ===============================
                Get_ADC12bitResult(15);  //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
                Bandgap = Get_ADC12bitResult(15);    //读内部基准ADC, 读15通道
                Get_ADC12bitResult(3);   //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
                j = Get_ADC12bitResult(3);  //读外部电压ADC
                j = (u16)((u32)j * 119 / Bandgap);  //计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
            #endif
            //==========================================================================

            //===== 连续读16次ADC 再平均计算. 分辨率0.01V =========
            #if (Cal_MODE == 1)
                Get_ADC12bitResult(15);  //先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
                for(j=0, i=0; i<16; i++)
                {
                    j += Get_ADC12bitResult(15); //读内部基准ADC, 读15通道
                }
                Bandgap = j >> 4;   //16次平均
                for(j=0, i=0; i<16; i++)
                {
                    j += Get_ADC12bitResult(3); //读外部电压ADC
                }
                j = j >> 4; //16次平均
                j = (u16)((u32)j * 119 / Bandgap);  //计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
            #endif
            //==========================================================================

                printf("VCC=%0.2fV\r\n",(float)j/100);

                LED8[5] = j / 100 + DIS_DOT;    //显示外部电压值
                LED8[6] = (j % 100) / 10;
                LED8[7] = j % 10;
/*
                j = Bandgap;
                LED8[0] = j / 1000;     //显示Bandgap ADC值
                LED8[1] = (j % 1000) / 100;
                LED8[2] = (j % 100) / 10;
                LED8[3] = j % 10;
*/
            }
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

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //串口1选择定时器2为波特率发生器
#else
	S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //使用串口2时，W1位必需设置为1，否则可能会产生不可预期的错误
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //定时器2时钟1T模式,开始计时
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
