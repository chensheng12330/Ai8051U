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
                                    //选择出厂校准的36.864MHz频率
                                    //然后使用自动追频将频率追到24.567M附近
                                    //最后在播放WAVE时对IRC进行实时微调
    CLKDIV = 0x04;
    IRTRIM = T36M_ADDR;
    VRTRIM = VRT44M_ADDR;
    IRCBAND &= ~0x03;
    IRCBAND |= 0x03;
    CLKDIV = 0;

    X32KCR = 0x80;                  //启动外部32K振荡器作为自动追频时钟源
    while (!(X32KCR & 0x01));

    CRECNTH = CNT >> 8;             //设置自动追频
    CRECNTL = CNT;
    CRERES = RES;
    CRECR = 0x90;

	P5n_push_pull(0x10);	        //P5.4设置为推挽输出, 输出I2S-MCLK时.
	MCLKOCR = 0x00 + 100;		        //主时钟输出控制寄存器, 0x00:从P5.4输出,  0x80:从P1.6输出. +0:不输出时钟, +1~127:输出1~127分频.

    ClockStable = 0;
}

void clock_trim()
{
    if (WaveUpdate)                 //USB ISO数据传输周期固定为1ms
    {
        WaveUpdate = 0;

        if (ClockStable)            //等待微调IRC时钟频率稳定
        {
            ClockStable--;
        }
        else
        {
            if (WavePlayEn)
            {
                if (WaveDumpSize < UNDERRUN_POINT)      //缓存数据偏少
                {
                    IRTRIM--;                           //需要将IRC频率向下微调
                    ClockStable = 50;
                }
                else if (WaveDumpSize > OVERRUN_POINT)  //缓存数据偏多
                {
                    IRTRIM++;                           //需要将IRC频率向上微调
                    ClockStable = 50;
                }
            }
        }
    }
}

