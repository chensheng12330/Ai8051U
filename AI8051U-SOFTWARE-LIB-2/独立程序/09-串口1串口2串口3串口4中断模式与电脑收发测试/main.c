/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    功能说明    **************

例程为双串口全双工中断方式收发通讯程序。

通过PC向MCU发送数据, MCU收到后通过串口把收到的数据原样返回.

串口1默认波特率：115200,N,8,1.

串口2默认波特率：9600,N,8,1.

串口3默认波特率：1000000,N,8,1.

串口4默认波特率：2000000,N,8,1.

波特率相同的串口，可共用定时器2作为波特率发生器。

通过开启 "AI8051U_UART.h" 头文件里面的 UART1~UART4 定义，启动不同通道的串口通信。

用定时器做波特率发生器，建议使用1T模式(除非低波特率用12T)，并选择可被波特率整除的时钟频率，以提高精度。

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************    本地常量声明    **************/


/*************    本地变量声明    **************/


/*************    本地函数声明    **************/


/*************  外部函数和变量声明 *****************/


/******************* IO配置函数 *******************/
void GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;        //结构定义

    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;  //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);  //初始化

    GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;  //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);  //初始化

    GPIO_InitStructure.Pin  = GPIO_Pin_LOW;     //指定要初始化的IO, GPIO_Pin_0 ~ GPIO_Pin_7
    GPIO_InitStructure.Mode = GPIO_PullUp;      //指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);  //初始化
}

/***************  串口初始化函数 *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;                 //结构定义

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE或DISABLE
    COMx_InitStructure.ParityMode  = PARITY_NONE;       //校验模式,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (使能校验位需要设置9位模式)
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //接收超时使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //超时中断使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //超时时间, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4

    //相同的参数不需要重复设置，只需重新设置不同的参数。
//    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2, 所以不用选择)
    COMx_InitStructure.UART_BaudRate  = 9600ul;         //波特率, 一般 110 ~ 115200
    UART_Configuration(UART2, &COMx_InitStructure);     //初始化串口2 UART1,UART2,UART3,UART4

    COMx_InitStructure.UART_BRT_Use   = BRT_Timer3;     //选择波特率发生器, BRT_Timer3, BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 1000000ul;      //波特率, 一般 110 ~ 115200
    UART_Configuration(UART3, &COMx_InitStructure);     //初始化串口3 UART1,UART2,UART3,UART4

    COMx_InitStructure.UART_BRT_Use   = BRT_Timer4;     //选择波特率发生器, BRT_Timer4, BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 2000000ul;      //波特率, 一般 110 ~ 115200
    UART_Configuration(UART4, &COMx_InitStructure);     //初始化串口4 UART1,UART2,UART3,UART4

    NVIC_UART1_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART2_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART3_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    NVIC_UART4_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);         //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
    UART2_SW(UART2_SW_P42_P43);         //UART2_SW_P12_P13,UART2_SW_P42_P43
    UART3_SW(UART3_SW_P00_P01);         //UART3_SW_P00_P01,UART3_SW_P50_P51
    UART4_SW(UART4_SW_P02_P03);         //UART4_SW_P02_P03,UART4_SW_P52_P53
}

/***********************************************/
void main(void)
{
    u8 i;

    WTST = 0;   //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXSFR();   //扩展SFR(XFR)访问使能 
    CKCON = 0;  //提高访问XRAM速度

    GPIO_config();
    UART_config();
    EA = 1;

    PrintString1("AI8051U UART1 Test Programme!\r\n");    //UART1发送一个字符串
    PrintString2("AI8051U UART2 Test Programme!\r\n");    //UART2发送一个字符串
    PrintString3("AI8051U UART3 Test Programme!\r\n");    //UART3发送一个字符串
    PrintString4("AI8051U UART4 Test Programme!\r\n");    //UART4发送一个字符串

    while (1)
    {
        if(COM1.RX_TimeOut)             //串口接收超时标志
        {
            COM1.RX_TimeOut = 0;

            if(COM1.RX_Cnt > 0)
            {
                for(i=0; i<COM1.RX_Cnt; i++)    TX1_write2buff(RX1_Buffer[i]);    //收到的数据原样返回
            }
            COM1.RX_Cnt = 0;
        }

        if(COM2.RX_TimeOut)             //串口接收超时标志
        {
            COM2.RX_TimeOut = 0;

            if(COM2.RX_Cnt > 0)
            {
                for(i=0; i<COM2.RX_Cnt; i++)    TX2_write2buff(RX2_Buffer[i]);    //收到的数据原样返回
            }
            COM2.RX_Cnt = 0;
        }

        if(COM3.RX_TimeOut)             //串口接收超时标志
        {
            COM3.RX_TimeOut = 0;

            if(COM3.RX_Cnt > 0)
            {
                for(i=0; i<COM3.RX_Cnt; i++)    TX3_write2buff(RX3_Buffer[i]);    //收到的数据原样返回
            }
            COM3.RX_Cnt = 0;
        }

        if(COM4.RX_TimeOut)             //串口接收超时标志
        {
            COM4.RX_TimeOut = 0;

            if(COM4.RX_Cnt > 0)
            {
                for(i=0; i<COM4.RX_Cnt; i++)    TX4_write2buff(RX4_Buffer[i]);    //收到的数据原样返回
            }
            COM4.RX_Cnt = 0;
        }
    }
}
