/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_Soft_I2C.h"

sbit    SDA = P0^1; //定义SDA
sbit    SCL = P0^0; //定义SCL

//========================================================================
// 函数: void I2C_Delay(void)
// 描述: I2C延时函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void I2C_Delay(void) //for normal MCS51, delay (2 * dly + 4) T, for STC12Cxxxx delay (4 * dly + 10) T
{
	u8  dly;
	dly = MAIN_Fosc / 2000000UL;        //按2us计算
	while(--dly);
}

//========================================================================
// I2C总线函数
//========================================================================
void I2C_Start(void)         //start the I2C, SDA High-to-low when SCL is high
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 0;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}       


void I2C_Stop(void)           //STOP the I2C, SDA Low-to-high when SCL is high
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SDA = 1;
	I2C_Delay();
}

void S_ACK(void)              //Send ACK (LOW)
{
	SDA = 0;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}

void S_NoACK(void)           //Send No ACK (High)
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	SCL = 0;
	I2C_Delay();
}
        
void I2C_Check_ACK(void)     //Check ACK, If F0=0, then right, if F0=1, then error
{
	SDA = 1;
	I2C_Delay();
	SCL = 1;
	I2C_Delay();
	F0  = SDA;
	SCL = 0;
	I2C_Delay();
}

//========================================================================
// 函数: void I2C_WriteAbyte(u8 dat)
// 描述: I2C发送一个字节数据函数.
// 参数: 发送的数据.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void I2C_WriteAbyte(u8 dat)     //write a byte to I2C
{
	u8 i;
	i = 8;
	do
	{
		if(dat & 0x80)  SDA = 1;
		else            SDA = 0;
		dat <<= 1;
		I2C_Delay();
		SCL = 1;
		I2C_Delay();
		SCL = 0;
		I2C_Delay();
	}
	while(--i);
}

//========================================================================
// 函数: u8 I2C_ReadAbyte(void)
// 描述: I2C读取一个字节数据函数.
// 参数: none.
// 返回: 读取数据.
// 版本: V1.0, 2020-09-15
//========================================================================
u8 I2C_ReadAbyte(void)          //read A byte from I2C
{
	u8 i,dat;
	i = 8;
	SDA = 1;
	do
	{
		SCL = 1;
		I2C_Delay();
		dat <<= 1;
		if(SDA)     dat++;
		SCL  = 0;
		I2C_Delay();
	}
	while(--i);
	return(dat);
}

//========================================================================
// 函数: void SI2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// 描述: I2C写入数据函数.
// 参数: dev_addr: 设备地址, mem_addr: 存储地址, *p写入数据存储位置, number写入数据个数.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SI2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth  */
{
	I2C_Start();
	I2C_WriteAbyte(dev_addr);
	I2C_Check_ACK();
	if(!F0)                                           //F0=0,right, F0=1,error
	{
		I2C_WriteAbyte(mem_addr);
		I2C_Check_ACK();
		if(!F0)
		{
			do
			{
				I2C_WriteAbyte(*p); p++;
				I2C_Check_ACK();
				if(F0)  break;
			}
			while(--number);
		}
	}
	I2C_Stop();
}

//========================================================================
// 函数: void SI2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)
// 描述: I2C读取数据函数.
// 参数: dev_addr: 设备地址, mem_addr: 存储地址, *p读取数据存储位置, number读取数据个数.
// 返回: none.
// 版本: V1.0, 2020-09-15
//========================================================================
void SI2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number)  /*  DeviceAddress,WordAddress,First Data Address,Byte lenth  */
{
	I2C_Start();
	I2C_WriteAbyte(dev_addr);
	I2C_Check_ACK();
	if(!F0)
	{
		I2C_WriteAbyte(mem_addr);
		I2C_Check_ACK();
		if(!F0)
		{
			I2C_Start();
			I2C_WriteAbyte(dev_addr|1);
			I2C_Check_ACK();
			if(!F0)
			{
				do
				{
					*p = I2C_ReadAbyte();   p++;
					if(number != 1)     S_ACK();    //send ACK
				}
				while(--number);
				S_NoACK();          //send no ACK
			}
		}
	}
	I2C_Stop();
}
