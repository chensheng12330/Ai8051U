/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "clock.h"
#include "i2s.h"

#define P5n_push_pull(bitn)			P5M1 &= ~(bitn),	P5M0 |=  (bitn)

BYTE ClockStable;

void clock_init()
{
                                    //ѡ�����У׼��36.864MHzƵ��
                                    //Ȼ��ʹ���Զ�׷Ƶ��Ƶ��׷��24.567M����
                                    //����ڲ���WAVEʱ��IRC����ʵʱ΢��
    CLKDIV = 0x04;
    IRTRIM = T36M_ADDR;
    VRTRIM = VRT44M_ADDR;
    IRCBAND &= ~0x03;
    IRCBAND |= 0x03;
    CLKDIV = 0;

    X32KCR = 0x80;                  //�����ⲿ32K������Ϊ�Զ�׷Ƶʱ��Դ
    while (!(X32KCR & 0x01));

    CRECNTH = CNT >> 8;             //�����Զ�׷Ƶ
    CRECNTL = CNT;
    CRERES = RES;
    CRECR = 0x90;

	P5n_push_pull(0x10);	        //P5.4����Ϊ�������, ���I2S-MCLKʱ.
	MCLKOCR = 0x00 + 100;		        //��ʱ��������ƼĴ���, 0x00:��P5.4���,  0x80:��P1.6���. +0:�����ʱ��, +1~127:���1~127��Ƶ.

    ClockStable = 0;
}

void clock_trim()
{
    if (WaveUpdate)                 //USB ISO���ݴ������ڹ̶�Ϊ1ms
    {
        WaveUpdate = 0;

        if (ClockStable)            //�ȴ�΢��IRCʱ��Ƶ���ȶ�
        {
            ClockStable--;
        }
        else
        {
            if (WavePlayEn)
            {
                if (WaveDumpSize < UNDERRUN_POINT)      //��������ƫ��
                {
                    IRTRIM--;                           //��Ҫ��IRCƵ������΢��
                    ClockStable = 50;
                }
                else if (WaveDumpSize > OVERRUN_POINT)  //��������ƫ��
                {
                    IRTRIM++;                           //��Ҫ��IRCƵ������΢��
                    ClockStable = 50;
                }
            }
        }
    }
}

