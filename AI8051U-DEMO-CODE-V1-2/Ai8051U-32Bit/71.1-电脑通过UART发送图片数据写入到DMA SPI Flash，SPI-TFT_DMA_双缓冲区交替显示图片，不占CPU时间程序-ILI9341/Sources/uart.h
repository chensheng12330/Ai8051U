#ifndef	__UART_H
#define	__UART_H

extern u8  RX1_TimeOut;
extern bit UartDmaRxFlag;

//void UartInit(void);
void UART1_config(u8 brt);
void UART1_DMA_Config(void);

#endif
