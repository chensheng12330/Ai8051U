/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过一个IO口获取一线制温度传感器 DS18B20 温度值.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

通过数码管显示测量的温度值，同时通过串口1(P3.0,P3.1)打印温度值.

下载时, 选择时钟 24MHz (用户可自行修改频率).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf 使用 UART1; 2:printf 使用 UART2

/****************************** 用户定义宏 ***********************************/
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#if (MAIN_Fosc >= 40000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 36000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 30000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 24000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 20000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 18000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 12000000L)
    #define        usrNOP()    _nop_();_nop_();_nop_()
#elif (MAIN_Fosc >= 6000000L)
    #define        usrNOP()    _nop_();_nop_()
#else
    #define        usrNOP()    _nop_()
#endif

/*****************************************************************************/
#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*************  IO口定义    **************/
sbit P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

sbit DQ = P3^3;                   //DS18B20的数据口

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
u8 MinusFlag;       //负数标志，0：正数，1：负数

/*************  本地函数声明    **************/
void DS18B20_Reset();
void DS18B20_WriteByte(u8 dat);
u8 DS18B20_ReadByte();

void delay_us(u8 us);
u16 ReadTemperature();
void DisplayTemperature(u16 temp);
void UartInit(void);

/********************* 主函数 *************************/
void main(void)
{
    u8  i;
    u16 Temp;

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

    display_index = 0;
    AUXR |= 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    EA = 1;     //打开总中断
    
    UartInit();
    
    for(i=0; i<8; i++)  LED8[i] = 0x10; //上电消隐

    while(1)
    {
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++msecond >= 300)    //300ms到
            {
                msecond = 0;
                Temp = ReadTemperature();
                printf("Temp = %0.1f\r\n",(float)Temp/10);  //串口打印温度值
                DisplayTemperature(Temp);   //数码管显示温度值
            }
        }
    }
}

//========================================================================
// 函数: u16 ReadTemperature()
// 描述: 读取温度函数。
// 参数: none.
// 返回: 温度值.
// 版本: VER1.0
// 日期: 2020-7-30
// 备注: 
//========================================================================
u16 ReadTemperature()
{
    u16 TempH, TempL, Temperature;
    
    DS18B20_Reset();                //设备复位
    DS18B20_WriteByte(0xCC);        //跳过ROM命令
    DS18B20_WriteByte(0x44);        //开始转换命令
    while (!DQ);                    //等待转换完成

    DS18B20_Reset();                //设备复位
    DS18B20_WriteByte(0xCC);        //跳过ROM命令
    DS18B20_WriteByte(0xBE);        //读暂存存储器命令
    TempL = DS18B20_ReadByte();     //读温度低字节
    TempH = DS18B20_ReadByte();     //读温度高字节
    
    if(TempH & 0xf8)    //判断是否位负数
    {
        MinusFlag = 1;  //设置负数标志
        Temperature = (TempH<<8) | TempL;
        Temperature = ~Temperature + 1;
        Temperature *= 0.625;       //0.0625 * 10，保留1位小数点
    }
    else
    {
        MinusFlag = 0;  //清除负数标志
        Temperature = (((TempH<<8) | TempL) * 0.625); //0.0625 * 10，保留1位小数点
    }

    return Temperature;
}

//========================================================================
// 函数: void DisplayTemperature(u16 temp)
// 描述: 显示温度函数。
// 参数: none.
// 返回: 温度值.
// 版本: VER1.0
// 日期: 2020-7-30
// 备注: 
//========================================================================
void DisplayTemperature(u16 temp)
{
    if(MinusFlag)
    {
        if(temp > 999)
        {
            LED8[3] = DIS_;
            LED8[4] = temp / 1000;
        }
        else
        {
            LED8[3] = DIS_BLACK;
            LED8[4] = DIS_;
        }
    }
    else
    {
        LED8[3] = DIS_BLACK;
        if(temp > 999)
        {
            LED8[4] = temp / 1000;
        }
        else
        {
            LED8[4] = DIS_BLACK;
        }
    }
    LED8[5] = (temp % 1000) / 100;
    LED8[6] = ((temp % 100) / 10) + DIS_DOT;
    LED8[7] = temp % 10;
}

//========================================================================
// 函数: void  delay_us(u8 us)
// 描述: 延时函数。
// 参数: us,要延时的us数, 这里只支持1~255us. 
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_us(u8 us)
{
    do{
        usrNOP();
        usrNOP();
    }while(--us);
}

/**************************************
复位DS18B20,并检测设备是否存在
**************************************/
void DS18B20_Reset()
{
    CY = 1;
    while (CY)
    {
        DQ = 0;                     //送出低电平复位信号
        delay_us(240);              //延时至少480us
        delay_us(240);
        DQ = 1;                     //释放数据线
        delay_us(60);               //等待60us
        CY = DQ;                    //检测存在脉冲
        delay_us(240);              //等待设备释放数据线
        delay_us(180);
    }
}

/**************************************
从DS18B20读1字节数据
**************************************/
u8 DS18B20_ReadByte()
{
    u8 i;
    u8 dat = 0;

    for (i=0; i<8; i++)             //8位计数器
    {
        dat >>= 1;
        DQ = 0;                     //开始时间片
        delay_us(1);                //延时等待
        DQ = 1;                     //准备接收
        delay_us(1);                //接收延时
        if (DQ) dat |= 0x80;        //读取数据
        delay_us(60);               //等待时间片结束
    }

    return dat;
}

/**************************************
向DS18B20写1字节数据
**************************************/
void DS18B20_WriteByte(u8 dat)
{
    char i;

    for (i=0; i<8; i++)             //8位计数器
    {
        DQ = 0;                     //开始时间片
        delay_us(1);                //延时等待
        dat >>= 1;                  //送出数据
        DQ = CY;
        delay_us(60);               //等待时间片结束
        DQ = 1;                     //恢复数据线
        delay_us(1);                //恢复延时
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
    if(++display_index >= 8) display_index = 0; //8位结束回0
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
