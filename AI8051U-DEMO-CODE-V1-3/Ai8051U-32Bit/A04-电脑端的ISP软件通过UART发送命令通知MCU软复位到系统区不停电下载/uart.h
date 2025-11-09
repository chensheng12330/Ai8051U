#ifndef __UART_H__
#define __UART_H__

void uart_init();
void delay_ms(u8 ms);

extern BYTE xdata UartTxBuffer[64];
extern BYTE xdata UartRxBuffer[64];

#endif


