#ifndef __CLOCK_H__
#define __CLOCK_H__

#define CNT                 ((16 * FOSC) / 32768)   //Ŀ��׷Ƶֵ
#define RES                 (CNT * 5 / 1000)        //׷Ƶ��� 0.5%

void clock_init();
void clock_trim();

#endif
