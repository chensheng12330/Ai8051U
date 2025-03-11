/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "i2s.h"

BOOL WaveUpdate;                        //数据更新标志位
BOOL WavePlayEn;                        //播放使能
BOOL WaveOverrun;                       //数据上溢标志位
BOOL WaveUnderrun;                      //数据下溢标志位
BYTE WaveMute;                          //静音控制寄存器
WORD WavePreSamp;                       //I2S数据缓存
WORD WaveWritePtr;                      //队列环写指针
WORD WaveReadPtr;                       //队列环读指针
WORD WaveDumpSize;                      //缓存的声音数据
WORD xdata WaveBuffer[BUFFER_SIZE];     //声音数据缓冲区

u16	cnt_2ms = 0;
bit	B_2ms;

//====================== I2S初始化函数 ==================================================
#define	MCKOE		1		//I2S主时钟输出控制, 0:禁止I2S主时钟输出, 1:允许I2S主时钟输出

#define	I2SEN		0x04	//I2S模块使能, 0x00:禁止, 0x04:允许
#define	I2S_MODE	2		//I2S模式, 0:从机发送模式, 1:从机接收模式, 2:主机发送模式, 3:主机接收模式,

#define	PCMSYNC		0		//PCM帧同步, 0: 短帧同步, 1: 长帧同步
#define	STD_MODE	0		//I2S标准选择, 0: I2S飞利浦标准, 1: MSB左对齐标准, 2:LSB右对齐标准, 3:PCM标准, CS4334、CS4344使用0:I2S飞利浦标准，PT8211使用1: MSB左对齐标准。
#define	CKPOL		0		//I2S稳态时钟极性, 0:时钟稳定状态为低电平, 1:时钟稳定状态为高电平
#define	DATLEN		0		//数据长度, 0:16位, 1:24位, 2:32位, 3:保留
#define	CHLEN		0		//通道长度(每个音频通道的位数), 0:16位, 1: 32位

#define I2S_MCLKDIV		(FOSC/(8*16*2*S_SAMPFREQ))	//MCLK分频系数, 对于双声道16bit.
#define I2S_BCLKDIV		(FOSC/(16*2*S_SAMPFREQ))	//BCLK分频系数, 对于双声道16bit.

void i2s_init()
{
	I2SMD = 0xff;					//内部保留字节,需设置为FFH
	I2SSR = 0x00;					//状态寄存器清0
	I2SCR = 0x80+0x00;				//使能发送缓冲区空中断(0x80), +0x00:Motorola格式, +0x10:TI格式
	HSCLKDIV   = 1;					//HSCLKDIV主时钟分频
	I2S_CLKDIV = 1;					//I2S主时钟分频
	I2SMCKDIV  = I2S_MCLKDIV;					//I2S时钟分频，I2SMCLK = 主频/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV,  或I2SMCLK = PLLCLK/2/I2S_CLKDIV/HSCLKDIV/I2SMCKDIV
	I2SPRH = (MCKOE << 1) + (I2S_BCLKDIV & 1);	//设置I2S_BMCLK分频系数的bit0, 并允许或禁止输出MCLK。
	I2SPRL = I2S_BCLKDIV/2;						//设置I2S_BMCLK分频系数的bit8~bit1
	I2SCFGH = I2S_MODE;				//设置I2S模式为主机发送模式
	I2SCFGL = (PCMSYNC << 7) + (STD_MODE << 4) + (CKPOL << 3) + (DATLEN << 1) + CHLEN;
//	P_SW3 = (P_SW3 & 0x3f) | (3<<6);	//I2S端口切换, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-6-18
	P_SW3 = (P_SW3 & 0x3f) | (1<<6);	//I2S端口切换, 0: P3.2(BCLK) P3.3(MCLK) P3.4(SD) P3.5(WS),	2024-7-21
										//             1: P1.7(BCLK) P1.6(MCLK) P1.5(SD) P1.4(WS),
										//             2: P2.3(BCLK) P2.2(MCLK) P2.1(SD) P2.0(WS),
										//             3: P4.3(BCLK) P1.6(MCLK) P4.1(SD) P4.0(WS),
	I2SCFGH |= I2SEN;                   //使能I2S模块

	IP3  |= 0x08;	                    //将I2S中断设置为最高优先级
	IP3H |= 0x08;

    WavePlayEn = 0;
    WaveMute = 1;
    WaveOverrun = 0;
    WaveUnderrun = 0;
    WavePreSamp = 0;
    WaveWritePtr = 0;
    WaveReadPtr = 0;
    WaveDumpSize = 0;
}

void i2s_isr(void) interrupt 62
{
    if (I2SSR & 0x02)	//发送缓冲区空
    {
        I2SDRH = WavePreSamp >> 8;      //填充准备的声音数据,左右声道自动切换
        I2SDRL = WavePreSamp;           //注意：I2S数据寄存器必须先写高字节，后写低字节

        if (WaveMute)
        {
            WavePreSamp = 0;            //静音时,准备声音数据0
        }
        else
        {
            if (WavePlayEn)
            {
                if (WaveDumpSize)       //判断是否有足够的数据
            	{
                    WavePreSamp = WaveBuffer[WaveReadPtr++];
                    WaveReadPtr &= BUFFER_MASK;
                    WaveDumpSize--;     //当前中断为最高优先级,不需要对临界变量WaveDumpSize进行保护
            	}
                else
                {
                    WaveUnderrun = 1;   //数据下溢
                    WavePlayEn = 0;
                }
            }
        }
    }
    if(++cnt_2ms == (96*2))
    {
		cnt_2ms = 0;
		B_2ms = 1;
	}
}

