#ifndef __I2S_H__
#define __I2S_H__

#define BUFFER_SIZE             (2048 / sizeof(WORD))
#define BUFFER_MASK             (BUFFER_SIZE - 1)
#define OVERRUN_POINT           (BUFFER_SIZE / 3 * 2)
#define UNDERRUN_POINT          (BUFFER_SIZE / 3)

void i2s_init();

extern BOOL WaveUpdate;                     //���ݸ��±�־λ
extern BOOL WavePlayEn;                     //����ʹ��
extern BOOL WaveOverrun;                    //���������־λ
extern BOOL WaveUnderrun;                   //���������־λ
extern BYTE WaveMute;                       //�������ƼĴ���
extern WORD WavePreSamp;                    //I2S���ݻ���
extern WORD WaveWritePtr;                   //���л�дָ��
extern WORD WaveReadPtr;                    //���л���ָ��
extern WORD WaveDumpSize;                   //�������������
extern WORD xdata WaveBuffer[BUFFER_SIZE];  //�������ݻ�����

extern bit	B_2ms;

#endif
