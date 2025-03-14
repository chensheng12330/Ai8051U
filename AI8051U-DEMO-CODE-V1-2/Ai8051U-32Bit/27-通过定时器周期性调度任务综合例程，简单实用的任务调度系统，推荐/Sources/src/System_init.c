/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "app.h"

//========================================================================
//                                IO口配置
//========================================================================
void GPIO_config(void)
{
    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x09;   P1M0 = 0x00;   //设置为准双向口, P1.0,P1.3高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x02;   //设置为准双向口, P5.1推挽输出
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    P51 = 1;    //给NTC供电
}

//========================================================================
//                               定时器配置
//========================================================================
void Timer_config(void)
{
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;        //Timer0 interrupt enable
    TR0 = 1;        //Tiner0 run
}

//========================================================================
//                              ADC初始化
//========================================================================
void ADC_config(void)
{
    ADCTIM = 0x3f;      //设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;      //设置 ADC 时钟为系统时钟/2/16/16
    ADC_CONTR = 0x80;   //使能 ADC 模块
}

//========================================================================
//                              PWM初始化
//========================================================================
void PWM_config(void)
{
    PWMB_CCER1 = 0x00;  //写 CCMRx 前必须先清零 CCxE 关闭通道
    PWMB_CCMR1 = 0x30;  //通道模式配置，翻转模式
    PWMB_CCER1 = 0x01;  //配置通道输出使能和极性

    PWMB_ARRH = 0x07;   //设置周期时间
    PWMB_ARRL = 0xff;

    PWMB_ENO = 0x00;    //PWM5关闭输出
//    PWMB_ENO = 0x01;    //PWM5使能输出
    PWMB_PS = 0x03;     //高级 PWM 通道输出脚选择位, P50

    PWMB_BKR = 0x80;    //使能主输出
    PWMB_CR1 |= 0x01;   //开始计时
}

//========================================================================
//                              UART初始化
//========================================================================
void UartInit(void)
{
#if(PrintUart == 1)
    S1_S1 = 0;      //UART1 switch to, 00: P3.0 P3.1, 01: P3.6 P3.7, 10: P1.6 P1.7, 11: P4.3 P4.4
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
    S2CON = (SCON & 0x3f) | 0x40; 
    T2L  = TM;
    T2H  = TM>>8;
    AUXR |= 0x14;   //定时器2时钟1T模式,开始计时
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
    SBUF = dat; 
    while(TI == 0);
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

//========================================================================
//                                系统初始化
//========================================================================
void SYS_Init(void)
{
    GPIO_config();
    Timer_config();
    ADC_config();
    PWM_config();
    UartInit();
    EA = 1;

    APP_config();
}
