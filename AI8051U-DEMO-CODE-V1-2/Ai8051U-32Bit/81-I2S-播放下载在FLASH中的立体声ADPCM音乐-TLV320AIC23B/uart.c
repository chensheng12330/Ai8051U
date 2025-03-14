/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/* 如果要在程序中使用此代码,请在程序中注明使用了STC的资料及程序        */
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "uart.h"
#include "usb.h"

BOOL RxFlag;
BYTE RxCount;
BYTE xdata RxBuffer[64];
BYTE xdata TxBuffer[1048];

void uart_init()
{
    RxFlag = 0;
    RxCount = 0;
}

void uart_send(int size)
{
    BYTE cnt;
    int addr;

    EUSB = 0;
    usb_write_reg(INDEX, 1);

    addr = 0;
    do
    {
        cnt = (size > 64) ? 64 : size;
        while (usb_read_reg(INCSR1) & INIPRDY);
        usb_bulk_intr_in(&TxBuffer[addr], cnt, 1);
        addr += cnt;
        size -= cnt;
    } while (cnt >= 64);

    EUSB = 1;
}

void uart_recv_done()
{
    EUSB = 0;

    RxFlag = 0;
    usb_write_reg(INDEX, 1);
    usb_write_reg(OUTCSR1, 0);

    EUSB = 1;
}
