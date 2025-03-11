/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

用户可以在宏定义中改变MCU主时钟频率. 范围 8MHZ ~ 33MHZ.

红外接收程序。模拟市场上用量最大的NEC的编码。

用户可以在宏定义中指定用户码.

使用PWM4产生38KHZ载波, 1/3占空比, 每个38KHZ周期发射管发射9us,关闭16.3us.

使用开发板上的16个IO扫描按键, MCU不睡眠, 连续扫描按键.

当键按下, 第一帧为数据, 后面的帧为重复帧,不带数据, 具体定义请自行参考NEC的编码资料.

键释放后, 停止发送.

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

/*****************************************************************************/


/*************  本地常量声明    **************/


/*************  IO键盘变量声明  **************/

u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;   //行列键盘变量
u8  KeyHoldCnt; //键按下计时
u8  KeyCode;    //给用户使用的键码, 1~16有效

/*************  红外发送相关变量    **************/

#define User_code   0xFF00      //定义红外用户码

sbit    P_IR_TX   = P2^7;   //定义红外发送端口
#define IR_TX_ON    0
#define IR_TX_OFF   1

static u16 tx_cnt;     //发送或空闲的脉冲计数(等于38KHZ的脉冲数，对应时间), 红外频率为38KHZ, 周期26.3us
u8 TxTime;     //发送时间

/************* 本地函数声明 **************/

void delay_ms(u8 ms);
void IO_KeyScan(void);
void PWM_config(void);
void IR_TxPulse(u16 pulse);
void IR_TxSpace(u16 pulse);
void IR_TxByte(u8 dat);
void UartInit(void);

/********************* 主函数 *************************/
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
    PWM_config();
    P_IR_TX = IR_TX_OFF;
    EA = 1;                     //打开总中断
    
    while(1)
    {
        delay_ms(30);       //30ms
        IO_KeyScan();

        if(KeyCode != 0)        //检测到键码
        {
            TxTime = 0;
                                //一帧数据最小长度 = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
                                //一帧数据最大长度 = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
            IR_TxPulse(342);    //对应9ms，同步头       9ms
            IR_TxSpace(171);    //对应4.5ms，同步头间隔 4.5ms
            IR_TxPulse(21);     //开始发送数据          0.5625ms

            IR_TxByte(User_code%256);   //发用户码低字节
            IR_TxByte(User_code/256);   //发用户码高字节
            IR_TxByte(KeyCode);         //发数据
            IR_TxByte(~KeyCode);        //发数据反码
            
            if(TxTime < 56)     //一帧按最大77ms发送, 不够的话,补偿时间     108ms
            {
                TxTime = 56 - TxTime;
                TxTime = TxTime + TxTime / 8;
                delay_ms(TxTime);
            }
            delay_ms(31);

            while(IO_KeyState != 0) //键未释放
            {
                IR_TxPulse(342);    //对应9ms，   同步头        9ms
                IR_TxSpace(86);     //对应2.25ms，同步头间隔    2.25ms
                IR_TxPulse(21);     //开始发送数据              0.5625ms
                delay_ms(96);
                IO_KeyScan();
            }

            printf("KeyCode = %u\r\n",KeyCode);
            KeyCode = 0;
        }
    }
}

//========================================================================
// 函数: void delay_ms(unsigned char ms)
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
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/*****************************************************
    行列键扫描程序
    使用XY查找4x4键的方法，只能单键，速度快

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/


u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
    u8 i;
    i = 60;
    while(--i)  ;
}

void IO_KeyScan(void)    //50ms call
{
    u8  j;

    j = IO_KeyState1;   //保存上一次状态

    P0 = 0xf0;  //X低，读Y
    IO_KeyDelay();
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //Y低，读X
    IO_KeyDelay();
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //取反
    
    if(j == IO_KeyState1)   //连续两次读相等
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //有键按下
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //第一次按下
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1秒后重键
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //计算键码，17~32
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}


/************* 发送脉冲函数 **************/
void IR_TxPulse(u16 pulse)
{
    tx_cnt = pulse;
    PWMA_CCER2 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR4 = 0x60; //设置 PWM4 模式1 输出
    PWMA_CCER2 = 0x70; //使能 CC4NE 通道, 低电平有效
    PWMA_IER = 0x10;   //使能捕获/比较 4 中断
    while(tx_cnt);
}

/************* 发送空闲函数 **************/
void IR_TxSpace(u16 pulse)
{
    tx_cnt = pulse;
    PWMA_CCER2 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR4 = 0x40; //设置 PWM4 强制为无效电平
    PWMA_CCER2 = 0x70; //使能 CC4NE 通道, 低电平有效
    PWMA_IER = 0x10;   //使能捕获/比较 4 中断
    while(tx_cnt);
}


/************* 发送一个字节函数 **************/
void IR_TxByte(u8 dat)
{
    u8 i;
    for(i=0; i<8; i++)
    {
        if(dat & 1)     IR_TxSpace(63), TxTime += 2;    //数据1对应 1.6875 + 0.5625 ms 
        else            IR_TxSpace(21), TxTime++;       //数据0对应 0.5625 + 0.5625 ms
        IR_TxPulse(21);         //脉冲都是0.5625ms
        dat >>= 1;              //下一个位
    }
}

//========================================================================
// 函数: void   PWM_config(void)
// 描述: PCA配置函数.
// 参数: None
// 返回: none.
// 版本: V1.0, 2012-11-22
//========================================================================
void PWM_config(void)
{
    PWMA_CCER2 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMA_CCMR4 = 0x60; //设置 PWM4 模式1 输出
    //PWMA_CCER2 = 0xB0; //使能 CC4E 通道, 低电平有效

    PWMA_ARRH = 0x02; //设置周期时间
    PWMA_ARRL = 0x77;
    PWMA_CCR4H = 0;
    PWMA_CCR4L = 210; //设置占空比时间

    PWMA_PS = 0x80;  //高级 PWM 通道 4N 输出脚选择位, 0x00:P1.7, 0x40:P0.7, 0x80:P2.7
//  PWMA_PS = 0x80;  //高级 PWM 通道 4P 输出脚选择位, 0x00:P1.6, 0x40:P0.6, 0x80:P2.6
    PWMA_ENO = 0x80; //使能 PWM4N 输出
//  PWMA_ENO = 0x40; //使能 PWM4P 输出
    PWMA_BKR = 0x80; //使能主输出
//    PWMA_IER = 0x10; //使能中断
    PWMA_CR1 |= 0x81;  //使能ARR预装载，开始计时
}

/******************* PWM中断函数 ********************/
void PWMA_ISR() interrupt PWMA_VECTOR
{
    if(PWMA_SR1 & 0X10)
    {
        PWMA_SR1 &=~0X10;
        //PWMA_SR1 = 0;
        if(--tx_cnt == 0)
        {
            PWMA_CCER2 = 0x00; //写 CCMRx 前必须先清零 CCxE 关闭通道
            PWMA_CCMR4 = 0x40; //设置 PWM4 强制为无效电平
            PWMA_CCER2 = 0x70; //使能 CC4NE 通道, 低电平有效
            PWMA_IER = 0x00;   // 关闭中断
        }
    }
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
