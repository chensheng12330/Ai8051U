/*---------------------------------------------------------------------*/
/* --- STC AI Limited -------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "TLV320AIC23.h"

#define AIC23_WR	0x34
#define AIC23_RD	0x35

/*
sbit	P_AIC23_SCL = P2^4;		//2024-6-18
sbit	P_AIC23_SDA = P2^3;
sbit	P_AIC23_CS  = P1^7;
*/
sbit	P_AIC23_SCL = P5^1;		//2024-7-21
sbit	P_AIC23_SDA = P5^0;


/****************************/

void	AIC23_Delay(void)
{
	u16	dly;
	dly = 10;		//取1对应SCL H为0.5us,L为1.5us,读写仍正常.为了可靠,取8
	while(--dly)	;
}


/****************************/
void AIC23_Start(void)               //start the I2C, SDA High-to-low when SCL is high
{
	P_AIC23_SDA = 1;	//SDA = 1;
	AIC23_Delay();
	P_AIC23_SCL = 1;	//SCL = 1;
	AIC23_Delay();
	AIC23_Delay();
	AIC23_Delay();
	AIC23_Delay();
	P_AIC23_SDA = 0;	//SDA = 0;
	AIC23_Delay();
	AIC23_Delay();
	AIC23_Delay();
	P_AIC23_SCL = 0;	//SCL = 0;
	AIC23_Delay();
}


void AIC23_Stop(void)					//STOP the I2C, SDA Low-to-high when SCL is high
{
	P_AIC23_SCL = 0;	//SCL = 0;
	AIC23_Delay();
	P_AIC23_SDA = 0;	//SDA = 0;
	AIC23_Delay();
	P_AIC23_SCL = 1;	//SCL = 1;
	AIC23_Delay();
	AIC23_Delay();
	AIC23_Delay();
	P_AIC23_SDA = 1;	//SDA = 1;
	AIC23_Delay();
}

u8 AIC23_Check_ACK(void)         //Check ACK, If status=0: right, if status=1:  error
{
	u8	status;
	P_AIC23_SDA = 1;	//SDA = 1;
	AIC23_Delay();
	P_AIC23_SCL = 1;	//SCL = 1;
	AIC23_Delay();
	status = 0;
	if(P_AIC23_SDA)	status = 1;
	P_AIC23_SCL = 0;	//SCL = 0;
	AIC23_Delay();
	return	status;
}

/****************************/
void AIC23_WriteAbyte(u8 dat)		//write a byte to I2C
{
	u8 i;
	for(i=0; i<8; i++)
	{
		if(dat & 0x80)	P_AIC23_SDA = 1;	//SDA = 1;
		else			P_AIC23_SDA = 0;	//SDA = 0;
		AIC23_Delay();
		P_AIC23_SCL = 1;	//SCL = 1;
		dat <<= 1;
		AIC23_Delay();
		P_AIC23_SCL = 0;	//SCL = 0;
		AIC23_Delay();
	}
}

/****************************/
u8	AIC23_RegWrite(u8 RegAddr, u8 dat)	//返回0: 正确, >0: 错误
{
	u8	status;

	status = 1;
	AIC23_Start();
	AIC23_WriteAbyte(AIC23_WR);		//device address + WR
	if(AIC23_Check_ACK() == 0)
	{
		AIC23_WriteAbyte(RegAddr);	//register address
		if(AIC23_Check_ACK() == 0)
		{
			AIC23_WriteAbyte(dat);	//data
			if(AIC23_Check_ACK() == 0)	//data
					status = 0;
		}
	}
	AIC23_Stop();
	return	status;
}

u8	AIC23_WriteCmd(u16 RegAddr, u16 dat)
{
	u8	status;

	status = 1;
	AIC23_Start();
	AIC23_WriteAbyte(AIC23_WR);		//device address + WR
	if(AIC23_Check_ACK() == 0)
	{
		dat = (RegAddr << 9) | dat;
		AIC23_WriteAbyte((u8)(dat >> 8));
		if(AIC23_Check_ACK() == 0)
		{
			AIC23_WriteAbyte((u8)dat);
			if(AIC23_Check_ACK() == 0)	status = 0;
		}
	}
	AIC23_Stop();
	return	status;
}


void	AIC23_Init(void)
{
/*	P_AIC23_CS = 0;	//CS = 0, 2024-6-18，对于2024-7-12和2024-7-21版本，CS直接接地了
	P2n_standard(Pin4+Pin3);	//SDA SCL设置为准双向口
	P1n_standard(Pin7);			//CS设置为准双向口
*/
	P5n_standard(Pin1+Pin0);	//SDA SCL设置为准双向口
	P5PU = 0x02;

	AIC23_WriteCmd(R_ResetRegister, AIC23_Reset);	//Reset
}


void	AIC32_InitSet(void)
{
	AIC23_WriteCmd(R_LineInVolume_L,		(LinVol_LRS | LinVol_Mute | LinVol_Value));
	AIC23_WriteCmd(R_LineInVolume_R,		(RinVol_RLS | RinVol_Mute | RinVol_Value));
	AIC23_WriteCmd(R_HeadphoneVolume_L,		(LhpVol_LRS | LhpVol_LZC  | LhpVol_Value));
	AIC23_WriteCmd(R_HeadphoneVolume_R,		(RhpVol_RLS | RhpVol_RZC  | RhpVol_Value));
//	AIC23_WriteCmd(R_AnalogAudioCtrl,		(SidetoneAtten | SidetoneEnable | DAC_Select | Bypass | InputSelect | MIC_Mute | MIC_Boost));
	AIC23_WriteCmd(R_DigitalAudioCtrl,		(DAC_Mute | De_emphasis | ADC_HighPassF));
	AIC23_WriteCmd(R_PowerControl, 			0);
//	AIC23_WriteCmd(R_DigitalAudioFormat,	(             DAC_LR_Swap | DAC_LR_Phase | AIC_DataLength | AIC_DataFormat));

#if defined (I2S_MASTER_TRANSMITTER)
	AIC23_WriteCmd(R_AnalogAudioCtrl,		(SidetoneAtten | SidetoneEnable | DAC_Select | Bypass | InputSelect | MIC_Mute | MIC_Boost));
	AIC23_WriteCmd(R_DigitalAudioFormat,	(             DAC_LR_Swap | DAC_LR_Phase | AIC_DataLength | AIC_DataFormat));

#elif defined (I2S_SLAVE_RECEIVER)
	AIC23_WriteCmd(R_AnalogAudioCtrl,		(SidetoneAtten | SidetoneEnable |              Bypass | InputSelect | MIC_Mute | MIC_Boost));
	AIC23_WriteCmd(R_DigitalAudioFormat,    (AIC_Master | DAC_LR_Swap | DAC_LR_Phase | AIC_DataLength | AIC_DataFormat));
#endif

	AIC23_WriteCmd(R_SampleRate,			(MclkOutputDiv | MclkInputDiv | AIC_SampleRate | AIC_BOSR | AIC_ClockMode));
	AIC23_WriteCmd(R_DigitalInterfaceEn,	DigitalInterface);
}












