#ifndef __I2S_H__
#define __I2S_H__

#define BUFFER_SIZE             (2048 / sizeof(WORD))
#define BUFFER_MASK             (BUFFER_SIZE - 1)
#define OVERRUN_POINT           (BUFFER_SIZE / 3 * 2)
#define UNDERRUN_POINT          (BUFFER_SIZE / 3)

void i2s_init();

extern BOOL WaveUpdate;                     //数据更新标志位
extern BOOL WavePlayEn;                     //播放使能
extern BOOL WaveOverrun;                    //数据上溢标志位
extern BOOL WaveUnderrun;                   //数据下溢标志位
extern BYTE WaveMute;                       //静音控制寄存器
extern WORD WavePreSamp;                    //I2S数据缓存
extern WORD WaveWritePtr;                   //队列环写指针
extern WORD WaveReadPtr;                    //队列环读指针
extern WORD WaveDumpSize;                   //缓存的声音数据
extern WORD xdata WaveBuffer[BUFFER_SIZE];  //声音数据缓冲区

extern bit	B_2ms;

#endif
