/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_RTC.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Timer.h"
#include "AI8051U_Switch.h"

/*************    功能说明    **************

读写芯片内部集成的RTC模块.

使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

通过串口1（P3.0,P3.1）打印时间(年-月-日-时-分-秒).

通过按键调整时间:
P35: 小时+.
P34: 小时-.
P33: 分钟+.
P32: 分钟-.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************    本地常量声明    **************/

#define SleepModeSet  0     //0:不进休眠模式，使用数码管显示时不能进休眠; 1:使能休眠模式

/*************    本地变量声明    **************/

u8  KeyCode;    //给用户使用的键码
u8  KeyOld;     //上一次读取的键码

u16 Key_cnt;
bit Key_Flag;
bit Key_Function;

u8  hour,minute;    //RTC变量

/*************    本地函数声明    **************/

void KeyScan(void);
void WriteRTC(void);

/*************  外部函数和变量声明 *****************/

extern bit B_1S;
extern bit B_Alarm;
extern bit T0_1ms;

/******************** IO口配置 ********************/
void GPIO_config(void)
{
    P3_MODE_IO_PU(GPIO_Pin_All);    //P3 设置为准双向口
}

/************************ 定时器配置 ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;  //结构定义
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //指定工作模式,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //指定时钟模式,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //是否输出定时器时钟, ENABLE或DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 1000UL));    //中断频率, 1000次/秒
    TIM_InitStructure.TIM_PS        = 0;                    //8位预分频器(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //是否初始化后启动定时器, ENABLE或DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //初始化Timer0, Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/***************  串口初始化函数 *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure; //结构定义

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率, 一般 110 ~ 115200
//    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE或DISABLE
//    COMx_InitStructure.ParityMode  = PARITY_NONE;       //校验模式,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (使能校验位需要设置9位模式)
//    COMx_InitStructure.TimeOutEnable  = ENABLE;         //接收超时使能, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //超时中断使能, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
//    COMx_InitStructure.TimeOutTimer  = 32ul;            //超时时间, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  RTC初始化函数 *****************/
void RTC_config(void)
{
    RTC_InitTypeDef RTC_InitStructure;

    RTC_InitStructure.RTC_Clock  = RTC_X32KCR;      //RTC 时钟, RTC_IRC32KCR, RTC_X32KCR
    RTC_InitStructure.RTC_Enable = ENABLE;          //RTC 功能使能,   ENABLE, DISABLE
    RTC_InitStructure.RTC_Year   = 24;              //RTC 年, 00~99, 对应2000~2099年
    RTC_InitStructure.RTC_Month  = 12;              //RTC 月, 01~12
    RTC_InitStructure.RTC_Day    = 31;              //RTC 日, 01~31
    RTC_InitStructure.RTC_Hour   = 23;              //RTC 时, 00~23
    RTC_InitStructure.RTC_Min    = 59;              //RTC 分, 00~59
    RTC_InitStructure.RTC_Sec    = 55;              //RTC 秒, 00~59
    RTC_InitStructure.RTC_Ssec   = 00;              //RTC 1/128秒, 00~127

    RTC_InitStructure.RTC_ALAHour= 00;              //RTC 闹钟时, 00~23
    RTC_InitStructure.RTC_ALAMin = 00;              //RTC 闹钟分, 00~59
    RTC_InitStructure.RTC_ALASec = 00;              //RTC 闹钟秒, 00~59
    RTC_InitStructure.RTC_ALASsec= 00;              //RTC 闹钟1/128秒, 00~127
    RTC_Inilize(&RTC_InitStructure);
    NVIC_RTC_Init(RTC_ALARM_INT|RTC_SEC_INT,Priority_0);    //中断使能, RTC_ALARM_INT/RTC_DAY_INT/RTC_HOUR_INT/RTC_MIN_INT/RTC_SEC_INT/RTC_SEC2_INT/RTC_SEC8_INT/RTC_SEC32_INT/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/******************** task A **************************/
void main(void)
{
    WTST = 0;   //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXSFR();   //扩展SFR(XFR)访问使能 
    CKCON = 0;  //提高访问XRAM速度

#if(SleepModeSet == 1)
    SET_TPS();          //设置系统等待时间单元，用于控制EEPROM操作、SPI/I2C超时时间以及休眠唤醒等待时间
    IRC_Debounce(0x10); //设置IRC时钟从休眠唤醒恢复稳定需要等待的时钟数
#endif

    GPIO_config();
    Timer_config();
    UART_config();
    RTC_config();
    EA = 1;

    KeyCode = 0;    //给用户使用的键码
    Key_Function = 0;

    while (1)
    {
        if(B_1S)
        {
            B_1S = 0;
            printf("Year=2%03d,Month=%d,Day=%d,Hour=%d,Minute=%d,Second=%d\r\n",YEAR,MONTH,DAY,HOUR,MIN,SEC);
        }

        if(B_Alarm)
        {
            B_Alarm = 0;
            printf("RTC Alarm!\r\n");
        }

    #if(SleepModeSet == 1)
        _nop_();
        _nop_();
        PD = 1;
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    #else

        if(T0_1ms)
        {
            T0_1ms = 0;
            KeyScan();
            
            if((Key_Function) && (KeyCode != 0))    //有键按下
            {
                hour = HOUR;
                minute = MIN;

                if(KeyCode & 0x20)   //hour +1
                {
                    if(++hour >= 24)    hour = 0;
                }
                if(KeyCode & 0x10)   //hour -1
                {
                    if(--hour >= 24)    hour = 23;
                }
                if(KeyCode & 0x08)   //minute +1
                {
                    if(++minute >= 60)  minute = 0;
                }
                if(KeyCode & 0x04)   //minute -1
                {
                    if(--minute >= 60)  minute = 59;
                }

                WriteRTC();
                Key_Function = 0;
            }
        }
    #endif
    }
}

/********************** 写RTC函数 ************************/
void WriteRTC(void)
{
    INIYEAR = YEAR;   //继承当前年月日
    INIMONTH = MONTH;
    INIDAY = DAY;

    INIHOUR = hour;   //修改时分
    INIMIN = minute;
    INISEC = 0;       //秒清零
    INISSEC = 0;
    RTCCFG |= 0x01;   //触发RTC寄存器初始化
}

/********************* 按键扫描函数 **********************/
void KeyScan(void)
{
    KeyCode = ~P3 & 0x3C;   //读取P32~P35状态

    if((KeyCode != 0) && (KeyCode == KeyOld))
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)       //50ms防抖
            {
                Key_Flag = 1;       //设置按键状态，防止重复触发
                Key_Function = 1;
            }
        }
    }
    else
    {
        KeyOld = KeyCode;
        Key_cnt = 0;
        Key_Flag = 0;
    }
}
