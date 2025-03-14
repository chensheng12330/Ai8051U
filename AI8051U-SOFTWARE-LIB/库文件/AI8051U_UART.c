/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_UART.h"

//========================================================================
//                               本地变量声明
//========================================================================

#ifdef UART1
COMx_Define    COM1;
u8 UART_BUF_type TX1_Buffer[COM_TX1_Lenth];    //发送缓冲
u8 UART_BUF_type RX1_Buffer[COM_RX1_Lenth];    //接收缓冲
#endif
#ifdef UART2
COMx_Define    COM2;
u8 UART_BUF_type TX2_Buffer[COM_TX2_Lenth];    //发送缓冲
u8 UART_BUF_type RX2_Buffer[COM_RX2_Lenth];    //接收缓冲
#endif
#ifdef UART3
COMx_Define    COM3;
u8 UART_BUF_type TX3_Buffer[COM_TX3_Lenth];    //发送缓冲
u8 UART_BUF_type RX3_Buffer[COM_RX3_Lenth];    //接收缓冲
#endif
#ifdef UART4
COMx_Define    COM4;
u8 UART_BUF_type TX4_Buffer[COM_TX4_Lenth];    //发送缓冲
u8 UART_BUF_type RX4_Buffer[COM_RX4_Lenth];    //接收缓冲
#endif

//========================================================================
// 函数: UART_Configuration
// 描述: UART初始化程序.
// 参数: UARTx: UART组号, COMx结构参数,请参考UART.h里的定义.
// 返回: none.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 UART_Configuration(u8 UARTx, COMx_InitDefine *COMx)
{
#if defined( UART1 ) || defined( UART2 ) || defined( UART3 ) || defined( UART4 )
    u16    i;
    u32    j;
#else
    UARTx = NULL;
    COMx = NULL;
#endif

#ifdef UART1
    if(UARTx == UART1)
    {
        COM1.TX_send    = 0;
        COM1.TX_write   = 0;
        COM1.B_TX_busy  = 0;
        COM1.RX_Cnt     = 0;
        COM1.RX_TimeOut = 0;
        
        for(i=0; i<COM_TX1_Lenth; i++)    TX1_Buffer[i] = 0;
        for(i=0; i<COM_RX1_Lenth; i++)    RX1_Buffer[i] = 0;

        SCON = (SCON & 0x3f) | COMx->UART_Mode;    //模式设置
        if((COMx->UART_Mode == UART_9bit_BRTx) || (COMx->UART_Mode == UART_8bit_BRTx))  //可变波特率
        {
            j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;  //按1T计算
            if(j >= 65536UL)    return FAIL;    //错误
            j = 65536UL - j;
            if(COMx->UART_BRT_Use == BRT_Timer2)
            {
                T2R = 0;        //Timer stop
                S1BRT = 1;      //S1 BRT Use Timer2;
                T2_CT = 0;      //Timer2 set As Timer
                T2x12 = 1;      //Timer2 set as 1T mode
                T2H = (u8)(j>>8);
                T2L = (u8)j;
                T2R = 1;        //Timer run enable
            }
            else
            {
                TR1 = 0;
                S1BRT = 0;      //S1 BRT Use Timer1;
                T1_CT = 0;      //Timer1 set As Timer
                TMOD &= ~0x30;  //Timer1_16bitAutoReload;
                T1x12 = 1;      //Timer1 set as 1T mode
                TH1 = (u8)(j>>8);
                TL1 = (u8)j;
                TR1  = 1;
            }
        }
        else if(COMx->UART_Mode == UART_ShiftRight)
        {
            if(COMx->BaudRateDouble == ENABLE)  S1M0x6 = 1;    //固定波特率SysClk/2
            else                                S1M0x6 = 0;    //固定波特率SysClk/12
        }
        else if(COMx->UART_Mode == UART_9bit)    //固定波特率SysClk*2^SMOD/64
        {
            if(COMx->BaudRateDouble == ENABLE)  SMOD = 1;    //固定波特率SysClk/32
            else                                SMOD = 0;    //固定波特率SysClk/64
        }
        else return FAIL;//模式错误
        UART1_RxEnable(COMx->UART_RxEnable);    //UART接收使能
        
        USARTCR2 &= ~0x06;     //清除校验模式
        USARTCR2 |= COMx->ParityMode;  //设置校验模式
        
        if(COMx->TimeOutEnable == ENABLE)   UR1TOCR |= 0x80;    //串口接收超时功能使能
        else                                UR1TOCR &= ~0x80;   //串口接收超时功能禁止

        if(COMx->TimeOutINTEnable == ENABLE) UR1TOCR |= 0x40;   //串口接收超时中断使能
        else                                 UR1TOCR &= ~0x40;  //串口接收超时中断禁止

        if(COMx->TimeOutScale == TO_SCALE_SYSCLK)   UR1TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR1TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((COMx->TimeOutTimer > 0) && (COMx->TimeOutTimer <= 0xffffff))
        {
            UR1TOTL = (u8)COMx->TimeOutTimer;
            UR1TOTH = (u8)(COMx->TimeOutTimer>>8);
            UR1TOTE = (u8)(COMx->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;//超时时间错误

        return SUCCESS;
    }
#endif
#ifdef UART2
    if(UARTx == UART2)
    {
        COM2.TX_send    = 0;
        COM2.TX_write   = 0;
        COM2.B_TX_busy  = 0;
        COM2.RX_Cnt     = 0;
        COM2.RX_TimeOut = 0;

        for(i=0; i<COM_TX2_Lenth; i++)    TX2_Buffer[i] = 0;
        for(i=0; i<COM_RX2_Lenth; i++)    RX2_Buffer[i] = 0;

        S2CON = (S2CON & 0x3f) | COMx->UART_Mode;       //模式设置
        if((COMx->UART_Mode == UART_9bit_BRTx) ||(COMx->UART_Mode == UART_8bit_BRTx))   //可变波特率
        {
            j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;  //按1T计算
            if(j >= 65536UL)    return FAIL;    //错误
            j = 65536UL - j;
            T2R = 0;        //Timer stop
            T2_CT = 0;      //Timer2 set As Timer
            T2x12 = 1;      //Timer2 set as 1T mode
            T2H = (u8)(j>>8);
            T2L = (u8)j;
            T2R = 1;        //Timer run enable
        }
        else if(COMx->UART_Mode == UART_ShiftRight)
        {
            if(COMx->BaudRateDouble == ENABLE)  S2CFG |= 0x20;  //S2M0x6=1,固定波特率SysClk/2
            else                                S2CFG &= ~0x20; //S2M0x6=0,固定波特率SysClk/12
        }
        else if(COMx->UART_Mode == UART_9bit)    //固定波特率SysClk*2^S2MOD/64
        {
            if(COMx->BaudRateDouble == ENABLE)  S2CFG |= 0x80;  //S2MOD=1,固定波特率SysClk/32
            else                                S2CFG &= ~0x80; //S2MOD=0,固定波特率SysClk/64
        }
        else return FAIL;//模式错误
        UART2_RxEnable(COMx->UART_RxEnable);    //UART接收使能
        
        USART2CR2 &= ~0x06;     //清除校验模式
        USART2CR2 |= COMx->ParityMode;  //设置校验模式
        
        if(COMx->TimeOutEnable == ENABLE)   UR2TOCR |= 0x80;    //串口接收超时功能使能
        else                                UR2TOCR &= ~0x80;   //串口接收超时功能禁止

        if(COMx->TimeOutINTEnable == ENABLE) UR2TOCR |= 0x40;   //串口接收超时中断使能
        else                                 UR2TOCR &= ~0x40;  //串口接收超时中断禁止

        if(COMx->TimeOutScale == TO_SCALE_SYSCLK)   UR2TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR2TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((COMx->TimeOutTimer > 0) && (COMx->TimeOutTimer <= 0xffffff))
        {
            UR2TOTL = (u8)COMx->TimeOutTimer;
            UR2TOTH = (u8)(COMx->TimeOutTimer>>8);
            UR2TOTE = (u8)(COMx->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;//超时时间错误

        return SUCCESS;
    }
#endif
#ifdef UART3
    if(UARTx == UART3)
    {
        COM3.TX_send    = 0;
        COM3.TX_write   = 0;
        COM3.B_TX_busy  = 0;
        COM3.RX_Cnt     = 0;
        COM3.RX_TimeOut = 0;
        for(i=0; i<COM_TX3_Lenth; i++)    TX3_Buffer[i] = 0;
        for(i=0; i<COM_RX3_Lenth; i++)    RX3_Buffer[i] = 0;

        if((COMx->UART_Mode == UART_9bit_BRTx) || (COMx->UART_Mode == UART_8bit_BRTx))  //可变波特率
        {
            if(COMx->UART_Mode == UART_9bit_BRTx)   S3_9bit();  //9bit
            else                                    S3_8bit();  //8bit
            j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;    //按1T计算
            if(j >= 65536UL)    return FAIL;    //错误
            j = 65536UL - j;
            if(COMx->UART_BRT_Use == BRT_Timer2)
            {
                T2R = 0;    //Timer stop
                S3_BRT_UseTimer2();    //S3 BRT Use Timer2;
                T2_CT = 0;  //Timer2 set As Timer
                T2x12 = 1;  //Timer2 set as 1T mode
                T2H = (u8)(j>>8);
                T2L = (u8)j;
                T2R = 1;    //Timer run enable
            }
            else
            {
                T3R = 0;    //Timer stop
                S3_BRT_UseTimer3(); //S3 BRT Use Timer3;
                T3H = (u8)(j>>8);
                T3L = (u8)j;
                T3_CT = 0;  //Timer3 set As Timer
                T3x12 = 1;  //Timer3 set as 1T mode
                T3R = 1;    //Timer run enable
            }
        }
        else return FAIL;    //模式错误
        UART3_RxEnable(COMx->UART_RxEnable);    //UART接收使能
        
        if(COMx->TimeOutEnable == ENABLE)   UR3TOCR |= 0x80;    //串口接收超时功能使能
        else                                UR3TOCR &= ~0x80;   //串口接收超时功能禁止

        if(COMx->TimeOutINTEnable == ENABLE) UR3TOCR |= 0x40;   //串口接收超时中断使能
        else                                 UR3TOCR &= ~0x40;  //串口接收超时中断禁止

        if(COMx->TimeOutScale == TO_SCALE_SYSCLK)   UR3TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR3TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((COMx->TimeOutTimer > 0) && (COMx->TimeOutTimer <= 0xffffff))
        {
            UR3TOTL = (u8)COMx->TimeOutTimer;
            UR3TOTH = (u8)(COMx->TimeOutTimer>>8);
            UR3TOTE = (u8)(COMx->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;//超时时间错误

        return SUCCESS;
    }
#endif
#ifdef UART4
    if(UARTx == UART4)
    {
        COM4.TX_send    = 0;
        COM4.TX_write   = 0;
        COM4.B_TX_busy  = 0;
        COM4.RX_Cnt     = 0;
        COM4.RX_TimeOut = 0;
        for(i=0; i<COM_TX4_Lenth; i++)    TX4_Buffer[i] = 0;
        for(i=0; i<COM_RX4_Lenth; i++)    RX4_Buffer[i] = 0;

        if((COMx->UART_Mode == UART_9bit_BRTx) || (COMx->UART_Mode == UART_8bit_BRTx))    //可变波特率
        {
            if(COMx->UART_Mode == UART_9bit_BRTx)   S4_9bit();  //9bit
            else                                    S4_8bit();  //8bit
            j = (MAIN_Fosc / 4) / COMx->UART_BaudRate;    //按1T计算
            if(j >= 65536UL)    return FAIL;    //错误
            j = 65536UL - j;
            if(COMx->UART_BRT_Use == BRT_Timer2)
            {
                T2R = 0;        //Timer stop
                S4_BRT_UseTimer2(); //S4 BRT Use Timer2;
                T2_CT = 0;      //Timer2 set As Timer
                T2x12 = 1;      //Timer2 set as 1T mode
                T2H = (u8)(j>>8);
                T2L = (u8)j;
                T2R = 1;        //Timer run enable
            }
            else
            {
                T4R = 0;        //Timer stop
                S4_BRT_UseTimer4(); //S4 BRT Use Timer4;
                T4H = (u8)(j>>8);
                T4L = (u8)j;
                T4_CT = 0;      //Timer4 set As Timer
                T4x12 = 1;      //Timer4 set as 1T mode
                T4R = 1;        //Timer run enable
            }
        }
        else return FAIL;    //模式错误
        UART4_RxEnable(COMx->UART_RxEnable);    //UART接收使能
        
        if(COMx->TimeOutEnable == ENABLE)   UR4TOCR |= 0x80;    //串口接收超时功能使能
        else                                UR4TOCR &= ~0x80;   //串口接收超时功能禁止

        if(COMx->TimeOutINTEnable == ENABLE) UR4TOCR |= 0x40;   //串口接收超时中断使能
        else                                 UR4TOCR &= ~0x40;  //串口接收超时中断禁止

        if(COMx->TimeOutScale == TO_SCALE_SYSCLK)   UR4TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR4TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((COMx->TimeOutTimer > 0) && (COMx->TimeOutTimer <= 0xffffff))
        {
            UR4TOTL = (u8)COMx->TimeOutTimer;
            UR4TOTH = (u8)(COMx->TimeOutTimer>>8);
            UR4TOTE = (u8)(COMx->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;//超时时间错误
        
        return SUCCESS;
    }
#endif
    return FAIL;    //错误
}

/*********************************************************/

/********************* UART1 函数 ************************/
#ifdef UART1
void TX1_write2buff(u8 dat)     //串口1发送函数
{
    #if(UART_QUEUE_MODE == 1)
    TX1_Buffer[COM1.TX_write] = dat;    //装发送缓冲，使用队列式数据发送，一次性发送数据长度不要超过缓冲区大小（COM_TXn_Lenth）
    if(++COM1.TX_write >= COM_TX1_Lenth)    COM1.TX_write = 0;

    if(COM1.B_TX_busy == 0)     //空闲
    {  
        COM1.B_TX_busy = 1;     //标志忙
        TI = 1;                 //触发发送中断
    }
    #else
    //以下是阻塞方式发送方法
    SBUF = dat;
    COM1.B_TX_busy = 1;         //标志忙
    while(COM1.B_TX_busy);
    #endif
}

void PrintString1(u8 *puts)
{
    for (; *puts != 0; puts++)  TX1_write2buff(*puts);   //遇到停止符0结束
}

#endif

/********************* UART2 函数 ************************/
#ifdef UART2
void TX2_write2buff(u8 dat)     //串口2发送函数
{
    #if(UART_QUEUE_MODE == 1)
    TX2_Buffer[COM2.TX_write] = dat;    //装发送缓冲，使用队列式数据发送，一次性发送数据长度不要超过缓冲区大小（COM_TXn_Lenth）
    if(++COM2.TX_write >= COM_TX2_Lenth)    COM2.TX_write = 0;

    if(COM2.B_TX_busy == 0)     //空闲
    {  
        COM2.B_TX_busy = 1;     //标志忙
        S2TI = 1;               //触发发送中断
    }
    #else
    //以下是阻塞方式发送方法
    S2BUF = dat;
    COM2.B_TX_busy = 1;         //标志忙
    while(COM2.B_TX_busy);
    #endif
}

void PrintString2(u8 *puts)
{
    for (; *puts != 0; puts++)  TX2_write2buff(*puts);   //遇到停止符0结束
}

#endif

/********************* UART3 函数 ************************/
#ifdef UART3
void TX3_write2buff(u8 dat)     //串口3发送函数
{
    #if(UART_QUEUE_MODE == 1)
    TX3_Buffer[COM3.TX_write] = dat;    //装发送缓冲，使用队列式数据发送，一次性发送数据长度不要超过缓冲区大小（COM_TXn_Lenth）
    if(++COM3.TX_write >= COM_TX3_Lenth)    COM3.TX_write = 0;

    if(COM3.B_TX_busy == 0)     //空闲
    {  
        COM3.B_TX_busy = 1;     //标志忙
        S3TI = 1;               //触发发送中断
    }
    #else
    //以下是阻塞方式发送方法
    S3BUF = dat;
    COM3.B_TX_busy = 1;         //标志忙
    while(COM3.B_TX_busy);
    #endif
}

void PrintString3(u8 *puts)
{
    for (; *puts != 0; puts++)  TX3_write2buff(*puts);  //遇到停止符0结束
}

#endif

/********************* UART4 函数 ************************/
#ifdef UART4
void TX4_write2buff(u8 dat)     //串口4发送函数
{
    #if(UART_QUEUE_MODE == 1)
    TX4_Buffer[COM4.TX_write] = dat;    //装发送缓冲，使用队列式数据发送，一次性发送数据长度不要超过缓冲区大小（COM_TXn_Lenth）
    if(++COM4.TX_write >= COM_TX4_Lenth)    COM4.TX_write = 0;

    if(COM4.B_TX_busy == 0)     //空闲
    {  
        COM4.B_TX_busy = 1;     //标志忙
        S4TI = 1;               //触发发送中断
    }
    #else
    //以下是阻塞方式发送方法
    S4BUF = dat;
    COM4.B_TX_busy = 1;         //标志忙
    while(COM4.B_TX_busy);
    #endif
}

void PrintString4(u8 *puts)
{
    for (; *puts != 0; puts++)  TX4_write2buff(*puts);   //遇到停止符0结束
}

#endif

/*********************************************************/
/*
void COMx_write2buff(u8 UARTx, u8 dat)  //UART1/UART2/UART3/UART4
{
    if(UARTx == UART1)    TX1_write2buff(dat);
    if(UARTx == UART2)    TX2_write2buff(dat);
    if(UARTx == UART3)    TX3_write2buff(dat);
    if(UARTx == UART4)    TX4_write2buff(dat);
}

void PrintString(u8 UARTx, u8 *puts)
{
    for (; *puts != 0; puts++)  COMx_write2buff(UARTx,*puts);   //遇到停止符0结束
}
*/

/********************* Printf 函数 ************************/
#if(PRINTF_SELECT == 1)

char putchar(char c)
{
    TX1_write2buff(c);
    return c;
}

#elif(PRINTF_SELECT == 2)

char putchar(char c)
{
    TX2_write2buff(c);
    return c;
}

#elif(PRINTF_SELECT == 3)

char putchar(char c)
{
    TX3_write2buff(c);
    return c;
}

#elif(PRINTF_SELECT == 4)

char putchar(char c)
{
    TX4_write2buff(c);
    return c;
}

#endif
