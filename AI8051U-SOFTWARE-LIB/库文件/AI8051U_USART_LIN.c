/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_UART.h"
#include "AI8051U_USART_LIN.h"

//========================================================================
//                               本地变量声明
//========================================================================

//========================================================================
// 函数: void UsartLinSendByte(u8 USARTx, u8 dat)
// 描述: 发送一个字节函数。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: dat: 发送的数据.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendByte(u8 USARTx, u8 dat)
{
    if(USARTx == USART1)
    {
        COM1.B_TX_busy = 1;
        SBUF = dat;
        while(COM1.B_TX_busy);
    }
    else if(USARTx == USART2)
    {
        COM2.B_TX_busy = 1;
        S2BUF = dat;
        while(COM2.B_TX_busy);
    }
}

//========================================================================
// 函数: void UsartLinSendData(u8 USARTx, u8 *pdat, u8 len)
// 描述: Lin发送数据函数。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: *pdat: 发生数据缓冲区
// 参数: len: 数据长度.
// 返回: Lin ID.
// 版本: VER1.0
// 日期: 2023-4-15
// 备注: 
//========================================================================
void UsartLinSendData(u8 USARTx, u8 *pdat, u8 len)
{
    u8 i;

    if(len > 8) return; 
    for(i=0;i<len;i++)
    {
        UsartLinSendByte(USARTx,pdat[i]);
    }
}

//========================================================================
// 函数: void UsartLinSendChecksum(u8 USARTx, u8 *dat, u8 len)
// 描述: 计算校验码并发送。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: *dat: 数据场传输的数据
// 参数: len: 数据长度.
// 返回: none.
// 版本: VER1.0
// 日期: 2023-4-15
// 备注: 
//========================================================================
void UsartLinSendChecksum(u8 USARTx, u8 *dat, u8 len)
{
    u16 sum = 0;
    u8 i;

    if(len > 8) return; 
    for(i = 0; i < len; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
    UsartLinSendByte(USARTx,(u8)sum);
}

//========================================================================
// 函数: void UsartSendBreak(u8 USARTx)
// 描述: 主模式发送Lin总线Break函数。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartSendBreak(u8 USARTx)
{
    if(USARTx == USART1)
    {
        USARTCR5 |= 0x04;   //主模式 Send Break
    }
    else if(USARTx == USART2)
    {
        USART2CR5 |= 0x04;  //主模式 Send Break
    }
    UsartLinSendByte(USARTx,0x00);
}

//========================================================================
// 函数: void UsartLinSendPID(u8 USARTx, u8 id)
// 描述: ID码加上校验符，转成PID码并发送。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2020-12-2
// 备注: 
//========================================================================
void UsartLinSendPID(u8 USARTx, u8 id)
{
    u8 P0 ;
    u8 P1 ;
    
    P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
    P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
    
    UsartLinSendByte(USARTx,id|P0|P1);
}

//========================================================================
// 函数: void UsartLinSendHeader(u8 USARTx, u8 lid)
// 描述: Lin主机发送帧头函数。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: ID码.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendHeader(u8 USARTx, u8 lid)
{
    UsartSendBreak(USARTx);         //Send Break
    UsartLinSendByte(USARTx,0x55);  //Send Sync Field
    UsartLinSendPID(USARTx,lid);    //设置总线ID
}

//========================================================================
// 函数: void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat, u8 len)
// 描述: Lin主机发送完整帧函数。
// 参数: USARTx: USART组号，取值：USART1或者USART2
// 参数: lid: Lin ID
// 参数: *pdat: 发送数据缓冲区
// 参数: len: 数据长度
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat, u8 len)
{
    UsartSendBreak(USARTx);         //Send Break
    UsartLinSendByte(USARTx,0x55);  //Send Sync Field

    UsartLinSendPID(USARTx,lid);    //设置总线ID
    UsartLinSendData(USARTx,pdat,len);
    UsartLinSendChecksum(USARTx,pdat,len);
}

//========================================================================
// 函数: void UsartLinBaudrate(u8 USARTx, u16 brt)
// 描述: Lin总线波特率设置函数。
// 参数: brt: 波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-10-28
// 备注: 
//========================================================================
void UsartLinBaudrate(u8 USARTx, u16 brt)
{
    u16 tmp;
    
    tmp = (MAIN_Fosc >> 4) / brt;
    if(USARTx == USART1)
    {
        USARTBRH = (u8)(tmp>>8);
        USARTBRL = (u8)tmp;
    }
    else if(USARTx == USART2)
    {
        USART2BRH = (u8)(tmp>>8);
        USART2BRL = (u8)tmp;
    }
}

//========================================================================
// 函数: UASRT_LIN_Configuration
// 描述: USART LIN初始化程序.
// 参数: USARTx: UART组号, USART LIN结构参数,请参考AI8051U_USART_LIN.h里的定义.
// 返回: none.
// 版本: V1.0, 2022-03-30
//========================================================================
u8 UASRT_LIN_Configuration(u8 USARTx, USARTx_LIN_InitDefine *USART)
{
    if(USARTx == USART1)
    {
        SCON = (SCON & 0x3f) | 0x40;    //USART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
        SMOD = 1;
        TI = 0;
        REN = 1;    //允许接收
        ES  = 1;    //允许中断

        if(USART->LIN_Enable == ENABLE)     USARTCR1 |= 0x80;   //使能LIN模块
        else                                USARTCR1 &= ~0x80;  //关闭LIN模块
        if(USART->LIN_Mode == LinSlaveMode) USARTCR5 |= 0x20;   //LIN模块从机模式
        else                                USARTCR5 &= ~0x20;  //LIN模块主机模式
        if(USART->LIN_AutoSync == ENABLE)   USARTCR5 |= 0x10;   //使能自动同步
        else                                USARTCR5 &= ~0x10;  //关闭自动同步

        UsartLinBaudrate(USART1,USART->LIN_Baudrate);           //设置波特率

        if(USART->TimeOutEnable == ENABLE)   UR1TOCR |= 0x80;   //串口接收超时功能使能
        else                                 UR1TOCR &= ~0x80;  //串口接收超时功能禁止

        if(USART->TimeOutINTEnable == ENABLE) UR1TOCR |= 0x40;  //串口接收超时中断使能
        else                                  UR1TOCR &= ~0x40; //串口接收超时中断禁止

        if(USART->TimeOutScale == TO_SCALE_SYSCLK)  UR1TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR1TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((USART->TimeOutTimer > 0) && (USART->TimeOutTimer <= 0xffffff))
        {
            UR1TOTL = (u8)USART->TimeOutTimer;
            UR1TOTH = (u8)(USART->TimeOutTimer>>8);
            UR1TOTE = (u8)(USART->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;//超时时间错误

        return SUCCESS;
    }

    if(USARTx == USART2)
    {
        S2CON = (S2CON & 0x3f) | 0x50;
        T2x12 = 1;   //定时器2时钟1T模式
        T2R = 1;     //开始计时
        ES2 = 1;     //允许中断
        S2CFG |= 0x80;  //S2MOD = 1

        if(USART->LIN_Enable == ENABLE)     USART2CR1 |= 0x80;  //使能LIN模块
        else                                USART2CR1 &= ~0x80; //关闭LIN模块
        if(USART->LIN_Mode == LinSlaveMode) USART2CR5 |= 0x20;  //LIN模块从机模式
        else                                USART2CR5 &= ~0x20; //LIN模块主机模式
        if(USART->LIN_AutoSync == ENABLE)   USART2CR5 |= 0x10;  //使能自动同步
        else                                USART2CR5 &= ~0x10; //关闭自动同步

        UsartLinBaudrate(USART2,USART->LIN_Baudrate);           //设置波特率

        if(USART->TimeOutEnable == ENABLE)  UR2TOCR |= 0x80;    //串口接收超时功能使能
        else                                UR2TOCR &= ~0x80;   //串口接收超时功能禁止

        if(USART->TimeOutINTEnable == ENABLE) UR2TOCR |= 0x40;  //串口接收超时中断使能
        else                                  UR2TOCR &= ~0x40; //串口接收超时中断禁止

        if(USART->TimeOutScale == TO_SCALE_SYSCLK)  UR2TOCR |= 0x20;  //超时计数时钟源：系统时钟
        else                                        UR2TOCR &= ~0x20; //超时计数时钟源：串口数据位率(波特率)

        if((USART->TimeOutTimer > 0) && (USART->TimeOutTimer <= 0xffffff))
        {
            UR2TOTL = (u8)USART->TimeOutTimer;
            UR2TOTH = (u8)(USART->TimeOutTimer>>8);
            UR2TOTE = (u8)(USART->TimeOutTimer>>16); //写 URxTOTE 后，新的TM值才会生效
        }
        else return FAIL;   //超时时间错误

        return SUCCESS;
    }
    return FAIL;    //错误
}

/*********************************************************/
