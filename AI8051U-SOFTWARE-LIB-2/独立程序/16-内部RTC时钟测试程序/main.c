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

/*************    ����˵��    **************

��доƬ�ڲ����ɵ�RTCģ��.

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������, �û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

ͨ������1��P3.0,P3.1����ӡʱ��(��-��-��-ʱ-��-��).

ͨ����������ʱ��:
P35: Сʱ+.
P34: Сʱ-.
P33: ����+.
P32: ����-.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/

#define SleepModeSet  0     //0:��������ģʽ��ʹ���������ʾʱ���ܽ�����; 1:ʹ������ģʽ

/*************    ���ر�������    **************/

u8  KeyCode;    //���û�ʹ�õļ���
u8  KeyOld;     //��һ�ζ�ȡ�ļ���

u16 Key_cnt;
bit Key_Flag;
bit Key_Function;

u8  hour,minute;    //RTC����

/*************    ���غ�������    **************/

void KeyScan(void);
void WriteRTC(void);

/*************  �ⲿ�����ͱ������� *****************/

extern bit B_1S;
extern bit B_Alarm;
extern bit T0_1ms;

/******************** IO������ ********************/
void GPIO_config(void)
{
    P3_MODE_IO_PU(GPIO_Pin_All);    //P3 ����Ϊ׼˫���
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;  //�ṹ����
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��ģʽ,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������ʱ��ʱ��, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 1000UL));    //�ж�Ƶ��, 1000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //��ʼ��Timer0, Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/***************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure; //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
//    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
//    COMx_InitStructure.ParityMode  = PARITY_NONE;       //У��ģʽ,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (ʹ��У��λ��Ҫ����9λģʽ)
//    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
//    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  RTC��ʼ������ *****************/
void RTC_config(void)
{
    RTC_InitTypeDef RTC_InitStructure;

    RTC_InitStructure.RTC_Clock  = RTC_X32KCR;      //RTC ʱ��, RTC_IRC32KCR, RTC_X32KCR
    RTC_InitStructure.RTC_Enable = ENABLE;          //RTC ����ʹ��,   ENABLE, DISABLE
    RTC_InitStructure.RTC_Year   = 24;              //RTC ��, 00~99, ��Ӧ2000~2099��
    RTC_InitStructure.RTC_Month  = 12;              //RTC ��, 01~12
    RTC_InitStructure.RTC_Day    = 31;              //RTC ��, 01~31
    RTC_InitStructure.RTC_Hour   = 23;              //RTC ʱ, 00~23
    RTC_InitStructure.RTC_Min    = 59;              //RTC ��, 00~59
    RTC_InitStructure.RTC_Sec    = 55;              //RTC ��, 00~59
    RTC_InitStructure.RTC_Ssec   = 00;              //RTC 1/128��, 00~127

    RTC_InitStructure.RTC_ALAHour= 00;              //RTC ����ʱ, 00~23
    RTC_InitStructure.RTC_ALAMin = 00;              //RTC ���ӷ�, 00~59
    RTC_InitStructure.RTC_ALASec = 00;              //RTC ������, 00~59
    RTC_InitStructure.RTC_ALASsec= 00;              //RTC ����1/128��, 00~127
    RTC_Inilize(&RTC_InitStructure);
    NVIC_RTC_Init(RTC_ALARM_INT|RTC_SEC_INT,Priority_0);    //�ж�ʹ��, RTC_ALARM_INT/RTC_DAY_INT/RTC_HOUR_INT/RTC_MIN_INT/RTC_SEC_INT/RTC_SEC2_INT/RTC_SEC8_INT/RTC_SEC32_INT/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/******************** task A **************************/
void main(void)
{
    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

#if(SleepModeSet == 1)
    SET_TPS();          //����ϵͳ�ȴ�ʱ�䵥Ԫ�����ڿ���EEPROM������SPI/I2C��ʱʱ���Լ����߻��ѵȴ�ʱ��
    IRC_Debounce(0x10); //����IRCʱ�Ӵ����߻��ѻָ��ȶ���Ҫ�ȴ���ʱ����
#endif

    GPIO_config();
    Timer_config();
    UART_config();
    RTC_config();
    EA = 1;

    KeyCode = 0;    //���û�ʹ�õļ���
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
            
            if((Key_Function) && (KeyCode != 0))    //�м�����
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

/********************** дRTC���� ************************/
void WriteRTC(void)
{
    INIYEAR = YEAR;   //�̳е�ǰ������
    INIMONTH = MONTH;
    INIDAY = DAY;

    INIHOUR = hour;   //�޸�ʱ��
    INIMIN = minute;
    INISEC = 0;       //������
    INISSEC = 0;
    RTCCFG |= 0x01;   //����RTC�Ĵ�����ʼ��
}

/********************* ����ɨ�躯�� **********************/
void KeyScan(void)
{
    KeyCode = ~P3 & 0x3C;   //��ȡP32~P35״̬

    if((KeyCode != 0) && (KeyCode == KeyOld))
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)       //50ms����
            {
                Key_Flag = 1;       //���ð���״̬����ֹ�ظ�����
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
