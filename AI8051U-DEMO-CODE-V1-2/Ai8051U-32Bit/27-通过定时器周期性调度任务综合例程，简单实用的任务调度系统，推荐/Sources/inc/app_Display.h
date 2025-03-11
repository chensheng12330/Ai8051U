#ifndef __APP_DISPLAY_H_
#define __APP_DISPLAY_H_

#include "config.h"

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

extern u8  LED8[8];        //œ‘ æª∫≥Â

void Display_init(void);
void Sample_Display(void);

#endif

