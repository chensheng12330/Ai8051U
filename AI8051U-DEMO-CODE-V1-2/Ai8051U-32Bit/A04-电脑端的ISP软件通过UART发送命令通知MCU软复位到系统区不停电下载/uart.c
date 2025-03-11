/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "uart.h"

BYTE xdata UartTxBuffer[64];
BYTE xdata UartRxBuffer[64];
BYTE bUartRxStage;

void delay_ms(u8 ms);

void uart_init()
{
    S1_S0 = 0;
    S1_S1 = 0;
    SCON = 0x50;
    S1BRT = 1;
    T2x12 = 1;
    T2L = BAUD;
    T2H = BAUD >> 8;
    T2R = 1;
    ES = 1;
    
    bUartRxStage = 0;
}

void uart_isr() interrupt 4
{
    BYTE dat;

    if (TI)
    {
        TI = 0;
    }
    
    if (RI)
    {
        RI = 0;
        
        dat = SBUF;
        switch (bUartRxStage)
        {
        case 0:
L_CheckHeader:
            bUartRxStage = 0;
            if (dat == '@')
                bUartRxStage = 1;
            break;
        case 1:
            if (dat != 'S')
                goto L_CheckHeader;
            bUartRxStage = 2;
            break;
        case 2:
            if (dat != 'T')
                goto L_CheckHeader;
            bUartRxStage = 3;
            break;
        case 3:
            if (dat != 'C')
                goto L_CheckHeader;
            bUartRxStage = 4;
            break;
        case 4:
            if (dat != 'I')
                goto L_CheckHeader;
            bUartRxStage = 5;
            break;
        case 5:
            if (dat != 'S')
                goto L_CheckHeader;
            bUartRxStage = 6;
            break;
        case 6:
            if (dat != 'P')
                goto L_CheckHeader;
            bUartRxStage = 7;
            break;
        case 7:
            if (dat != '#')
                goto L_CheckHeader;
            bUartRxStage = 0;
            delay_ms(200);
            IAP_CONTR = 0x60;   //触发软件复位，从ISP开始执行
            while (1);
            break;
        default:
            bUartRxStage = 0;
            break;
        }
    }
}

void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);
     }while(--ms);
}
