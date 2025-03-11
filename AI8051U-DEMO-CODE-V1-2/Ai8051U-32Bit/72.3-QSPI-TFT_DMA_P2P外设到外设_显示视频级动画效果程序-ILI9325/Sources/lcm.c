#include "config.h"
#include "lcm.h"

void LCM_Init()
{
    P0M0 |= 0x20;       //CS设置成推挽输出
    P0M1 &= ~0x20;

    P2M0 = 0xff;        //DATA口设置成推挽输出
    P2M1 = 0x00;

    P3M0 |= 0xc0;       //WR,RD设置成推挽输出
    P3M1 &= ~0xc0;

    P4M0 |= 0xa0;       //RS,RST设置成推挽输出
    P4M1 &= ~0xa0;

    LCMIFCFG = 0x00;
    LCMIFCFG2 = 0x24;
    LCMIFSTA = 0x00;
}

void LCM_WriteCmd_CS(BYTE cmd)     
{
    LCM_CS = 0;
    LCMIFDATL = cmd;
    LCMIFCR = 0x84;
    while (!(LCMIFSTA & 0x01));
    LCMIFSTA = 0x00;
    LCM_CS = 1;
}

void LCM_WriteData_CS(BYTE dat)     
{
    LCM_CS = 0;
    LCM_WriteData(dat);
    LCM_CS = 1;
}

void LCM_WriteData(BYTE dat)     
{
    LCMIFDATL = dat;
    LCMIFCR = 0x85;
    while (!(LCMIFSTA & 0x01));
    LCMIFSTA = 0x00;
}
