

#ifndef	__TLV320AIC23_H
#define	__TLV320AIC23_H


#define	R_LineInVolume_L		0
#define	R_LineInVolume_R		1
#define	R_HeadphoneVolume_L		2
#define	R_HeadphoneVolume_R		3
#define	R_AnalogAudioCtrl		4
#define	R_DigitalAudioCtrl		5
#define	R_PowerControl			6
#define	R_DigitalAudioFormat	7
#define	R_SampleRate			8
#define	R_DigitalInterfaceEn	9
#define	R_ResetRegister			0x0f

//	R_LineInVolume_L,	0x00
#define	LinVol_LRS		(1<<8)		//开启左右声道同步更新,0: 关闭， 1:开启
#define	LinVol_Mute		(0<<7)		//0 = Normal,  1 = Mute
#define	LinVol_Value	(31-8)		//31~0, 31->+12db, 0->-34.5db, -1.5db/step

//	R_LineInVolume_R,	0x01
#define	RinVol_RLS		(1<<8)		//开启左右声道同步更新,0: 关闭， 1:开启
#define	RinVol_Mute		(0<<7)		//0 = Normal,  1 = Mute
#define	RinVol_Value	(31-8)		//31~0, 31->+12db, 0->-34.5db, -1.5db/step

//	R_HeadphoneVolume_L,	0x02
#define	LhpVol_LRS		(1<<8)		//开启左右声道同步更新,0: 关闭， 1:开启
#define	LhpVol_LZC		(1<<7)
#define	LhpVol_Value	(127-12)		//127~47，-1db/step, 127->+6db, 48->-73db, <48 mute.

//	R_HeadphoneVolume_R,	0x03
#define	RhpVol_RLS		(1<<8)		//开启左右声道同步更新,0: 关闭， 1:开启
#define	RhpVol_RZC		(1<<7)
#define	RhpVol_Value	(127-12)		//127~47，-1db/step, 127->+6db, 48->-73db, <48 mute.

//	R_AnalogAudioCtrl,	0x04
#define	SidetoneAtten	(0<<6)		//0=-6db, 1=-9db, 2=-12db, 3=-15db
#define	SidetoneEnable	(0<<5)		//0 = Disable,  1 = Enable
#define	DAC_Select		(1<<4)		//0 = DAC off,  1 = DAC select
#define	Bypass			(1<<3)		//0 = Disable,  1 = Enable
#define	InputSelect		(0<<2)		//0 = Line in,  1 = Microphone
#define	MIC_Mute		(1<<1)		//0 = Normal,   1 = Mute
#define	MIC_Boost		1			//0 = 0db,      1 = 20db

//	R_DigitalAudioCtrl,	0x05
#define	DAC_Mute		(0<<3)		//DAC soft mute,0 = disable, 1=enable
#define	De_emphasis		(0<<1)		//0=disable, 1=32KHZ, 2=44.1KHZ, 3=48KHZ
#define	ADC_HighPassF	0			//ADC high pass filtter, 0=disable, 1=enable.

//	R_PowerControl,	0x06
#define	DevicePowerOff	(1<<7)
#define	PwoerClockOff	(1<<6)
#define	PowerOscOff		(1<<5)
#define	PowerOutputOff	(1<<4)
#define	PowerDacOff		(1<<3)
#define	PowerAdcOff		(1<<2)
#define	PowerMicOff		(1<<1)
#define	PowerLineInOff	1

//	R_DigitalAudioFormat,	0x07
#define	AIC_Master		(1<<6)	//0=Slave,   1=Master
#define	DAC_LR_Swap		(0<<5)	//0=disable, 1=enable
#define	DAC_LR_Phase	(0<<4)	//????
#define	AIC_DataLength	(0<<2)	//0=16bit, 1=20bit, 2=24bit, 3=32bit
#define	AIC_DataFormat		2	//0=MSB first, right aligned, 1=MSB first, left alihned, 2=I2S format, MSB first,left -1 aligned, 3=DSP format,

//	R_SampleRate,	0x08
#define	MclkOutputDiv	(0<<7)	//0=MCLK, 1=MCLK/2
#define	MclkInputDiv	(0<<6)	//0=MCLK, 1=MCLK/2
#define	AIC_SampleRate	(0<<2)	//48KHz at MCLK=12.288MHz = 24.576/2
#define	AIC_BOSR		(0<<1)	//USB mode: 0 = 250 fs, 1 = 272 fs.   Normal mode: 0 = 256 fs, 1 = 384 fs.
#define	AIC_ClockMode	0		//0 = Normal mode,  1 = USB mode

//	R_DigitalInterfaceEn,	0x09
#define	DigitalInterface	1	//0 = Inactive, 1 = Active.

//	R_ResetRegister,	0x0f
#define	AIC23_Reset		0


u8 		AIC23_WriteCmd(u16 RegAddr, u16 dat);
void	AIC23_Init(void);
void	AIC32_InitSet(void);




#endif

