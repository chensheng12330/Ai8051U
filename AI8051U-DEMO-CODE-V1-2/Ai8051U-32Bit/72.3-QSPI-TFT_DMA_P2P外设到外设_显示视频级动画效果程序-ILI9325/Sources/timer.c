#include "config.h"
#include "timer.h"


#define T1MS                (65536 - FOSC/1000)


BOOL f100ms;
static BYTE cnt100;

void TIMER0_Init()
{
    AUXR |= 0x80;
    TMOD &= ~0x0f;
    TL0 = T1MS;
    TH0 = T1MS >> 8;
    TR0 = 1;
    ET0 = 1;
}

void TIMER0_Isr() interrupt TMR0_VECTOR
{
    if (++cnt100 >= 100)
    {
        cnt100 = 0;
        f100ms = 1;
    }
}
