#ifndef __APP_RTC_H_
#define __APP_RTC_H_

#include "config.h"

extern u8 usrHour;
extern u8 usrMinute;
extern u8 usrSecond; //RTC±‰¡ø

void RTC_init(void);
void Sample_RTC(void);
void DisplayRTC(void);

#endif

