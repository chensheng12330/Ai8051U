/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

测试外挂的32K xdata程序。
测试方式: 
1: 写入0x55, 并读出判断是否全部是0x55.
2: 写入0xaa, 并读出判断是否全部是0xaa.
3: 写入32768字节的汉字字库(类似于随机数据), 并读出比较.

通过串口2发送单个字符x或X, 开始测试, 并返回相关的测试结果.

串口设置115200bps, 8, N, 1. 切换到P4.6 P4.7.

下载时, 选择时钟 22.1184MHz (用户可自行修改频率).

******************************************/

#include "..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/****************************** 用户定义宏 ***********************************/

#define     ExternalRAM_enable()        EXTRAM = 1      //允许外部XRAM
#define     InternalRAM_enable()        EXTRAM = 0      //禁止外部XRAM

/*****************************************************************************/

extern unsigned char code hz[];

/*************  本地常量声明    **************/

#define MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）

#define Baudrate1       (65536 - MAIN_Fosc / 115200 / 4)

#define XDATA_LENTH     32768   //xdata长度
#define HZK_LENTH       32768   //字库长度

#define BUS_SPEED_SET(x)  BUS_SPEED = x

#define UART1_BUF_LENGTH    64  //串口缓冲长度

/*************  本地变量声明    **************/
u8  RX1_TimeOut;
u8  RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志

u8  RX1_Buffer[UART1_BUF_LENGTH];   //接收缓冲
u16 temp;

/*************  本地函数声明    **************/
void delay_ms(u8 ms);
u8   TestXRAM(void);
void Xdata_Test(void);

void delay_ms(u8 ms);
void RX1_Check(void);
void UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void PrintString1(u8 *puts);
void UART1_TxByte(u8 dat);

/********************* 主函数 *************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x30;   P1M0 = 0x00;   //设置为准双向口，P1.4,P1.5设置高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    P0BP = 0x00;    //设置硬件自动控制P0口模式（默认由P0M1/P0M0控制）
    
    delay_ms(10);
    UART1_config(1);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    EA = 1;     //打开总中断
    
    P41 = 0;    //扩展SRAM使能
    P27 = 0;    //最高地址位置零

    PrintString1("AI8051U xdata test programme!\r\n"); //UART1发送一个字符串
    PrintString1("\r\n串口发送单个字符x或X, 开始测试.\r\n");

    BUS_SPEED_SET(3);       //0~7  3V@22MHZ用1T会访问错误
    BUS_SPEED |= 0x40;      //选择 P3.7为RD，P3.6为WR
    ExternalRAM_enable();   //允许外部XDATA
//  InternalRAM_enable();   //允许内部XDATA

    while(1)
    {
        delay_ms(1);
        if(RX1_TimeOut > 0)     //超时计数
        {
            if(--RX1_TimeOut == 0)  //串口通讯结束
            {
                if(RX1_Cnt > 0)     //收到数据字节数
                {
                    if(RX1_Cnt == 1)    Xdata_Test();   //单字节命令
                }
                RX1_Cnt = 0;
            }
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

/*************  发送错误地址函数 *****************/
void TxErrorAddress(u16 i)
{
    PrintString1("错误地址 = ");
    if(i >= 10000)  UART1_TxByte((u8)(i/10000+'0')),  i %= 10000;
    UART1_TxByte((u8)(i/1000+'0')),   i %= 1000;
    UART1_TxByte((u8)(i/100+'0')),    i %= 100;
    UART1_TxByte((u8)(i/10+'0'));
    UART1_TxByte((u8)(i%10+'0'));
    UART1_TxByte(0x0d);
    UART1_TxByte(0x0a);
}

/*************  测试xdata函数 *****************/

#define EXRAM           ((unsigned char volatile far *)0x7f0000)

u8  TestXRAM(void)
{
    u16 ptc;
    u16 ptx;
    u16 i,j;

    for(ptx=0; ptx<XDATA_LENTH; ptx++)  EXRAM[ptx] = 0x55;    //测试是否有位短路
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        if(EXRAM[ptx] != 0x55)
        {
            TxErrorAddress(ptx);
            return 1;   //测试0x55错误
        }
    }

    for(ptx=0; ptx<XDATA_LENTH; ptx++)  EXRAM[ptx] = 0xaa;    //测试是否有位短路
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    if(EXRAM[ptx] != 0xaa)
    {
        TxErrorAddress(ptx);
        return 2;   //测试0xaa错误
    }

    i = 0;
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        EXRAM[ptx] = (u8)(i >> 8);
        ptx++;
        EXRAM[ptx] = (u8)i;
        i++;
    }
    i = 0;
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        j = EXRAM[ptx];
        ptx++;
        j = (j << 8) + EXRAM[ptx];
        if(i != j)
        {
            TxErrorAddress(ptx);
            return 3;   //写连续数字错误
        }
        i++;
    }

    ptx = 0;
    for(ptc=0; ptc<HZK_LENTH; ptc++)    {EXRAM[ptx] = hz[ptc];    ptx++;}
    ptx = 0;
    for(ptc=0; ptc<HZK_LENTH; ptc++)
    {
        if(EXRAM[ptx] != hz[ptc])
        {
            TxErrorAddress(ptc);
            return 4;   //写字库错误
        }
        ptx++;
    }

    return 0;
}

/*************  xdata测试返回信息函数 *****************/
void Xdata_Test(void)
{
    u8  i;
    if((RX1_Buffer[0] == 'x') || (RX1_Buffer[0] == 'X'))
    {
        PrintString1("测试 xdata 中, 请稍候......\r\n");
        i = TestXRAM();
             if(i == 0) PrintString1("测试 xdata 结果OK!\r\n");
        else if(i == 1) PrintString1("测试 xdata 写入0x55错误!  ");
        else if(i == 2) PrintString1("测试 xdata 写入0xaa错误!  ");
        else if(i == 3) PrintString1("测试 xdata 连续写入错误!  ");
        else if(i == 4) PrintString1("测试 xdata 写入字库错误!  ");
    }
}

//========================================================================
// 函数: void UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2024-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    SBUF = dat;
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}

//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-07
// 备注: 
//========================================================================
void PrintString1(u8 *puts) //发送一个字符串
{
    for (; *puts != 0;  puts++)     UART1_TxByte(*puts);    //遇到停止符0结束
}

//========================================================================
// 函数: void SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-07
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;    //Timer run enable
}

//========================================================================
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-07
// 备注: 
//========================================================================
void UART1_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
    /*********** 波特率使用定时器2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)Baudrate1);
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)(Baudrate1 / 256);
        TL1 = (u8)(Baudrate1 % 256);
        ET1 = 0;    //禁止中断
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    RX1_Cnt = 0;
}

//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2024-08-07
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;     //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;     //Clear Tx flag
        B_TX1_Busy = 0;
    }
}
