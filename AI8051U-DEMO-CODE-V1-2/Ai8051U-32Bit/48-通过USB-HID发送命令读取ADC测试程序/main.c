/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

上位机软通过USB向MCU发送读取AD值命令, MCU收到命令后将获取的AD值通过USB发送给上位机.

命令格式： AA 55 CMD ADH ADL GAPH GAPL CS
AA 55：起始码；
CMD：  01,上位机向设备获取数据；02，设备向上位机发送数据；
ADH：  测量AD值高8位；
ADL：  测量AD值低8位；
GAPH： 内部参考电压AD值高8位；
GAPL： 内部参考电压AD值低8位；
CS：   校验码(以上数据的累加值)。

下载时, 选择时钟 24MHZ (用户可自行修改频率).

******************************************/

#include "..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"
#include "USB.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

/****************************** 用户定义宏 ***********************************/
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒
/*****************************************************************************/

/*************  本地常量声明    **************/


/*************  本地变量声明    **************/
bit B_1ms;          //1ms标志
WORD msecond;
WORD Bandgap;

/*************  本地函数声明    **************/
void UsbInit();
BYTE ReadReg(BYTE addr);
void WriteReg(BYTE addr, BYTE dat);
BYTE ReadFifo(BYTE fifo, BYTE *pdat);
void WriteFifo(BYTE fifo, BYTE *pdat, BYTE cnt);

void ADC_Config(void);
void Timer0_Config(WORD tReload);
WORD Get_ADC12bitResult(BYTE channel); //channel = 0~15
BYTE GetCheckSum(BYTE *buf, BYTE len);

/********************* 主函数 *************************/
void main(void)
{
	BYTE i;
	WORD ADData;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x08;   P1M0 = 0x00;   //设置为准双向口,P1.3设置高阻输入
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x02;   //设置为准双向口,P5.1设置推挽输出
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

	IRC48MCR |= 0x80;    //使能内部48M高速IRC
	while((IRC48MCR & 0x01)==0);  //等待时钟稳定
	IRCBAND = (IRCBAND & 0x3f) | 0x80; //USB时钟选择IRC48M

	Timer0_Config((WORD)Timer0_Reload);
	ADC_Config();
	UsbInit();
	EA = 1;
	HidOutput[0]=0xaa;
	HidOutput[1]=0x55;
	HidOutput[2]=0x02;
	for(i=3;i<64;i++) HidOutput[i] = 0;
    P51 = 1;        //给NTC供电

	while (1)
	{
		if(B_1ms)   //1ms到
		{
			B_1ms = 0;
			if(++msecond >= 300)    //300ms到
			{
				msecond = 0;

                Get_ADC12bitResult(15);
				Bandgap = Get_ADC12bitResult(15);    //读内部基准ADC, 读15通道
                Get_ADC12bitResult(3);
				ADData = Get_ADC12bitResult(3);      //读外部电压ADC
				HidOutput[3] = (BYTE)(ADData >> 8);
				HidOutput[4] = (BYTE)(ADData);
				HidOutput[5] = (BYTE)(Bandgap >> 8);
				HidOutput[6] = (BYTE)(Bandgap);
				HidOutput[7] = GetCheckSum(HidOutput,7);
			}
		}
	}
}

//========================================================================
BYTE ReadReg(BYTE addr)
{
	BYTE dat;
	while (USBADR & 0x80);
	USBADR = addr | 0x80;
	while (USBADR & 0x80);
	dat = USBDAT;
	return dat;
}

void WriteReg(BYTE addr, BYTE dat)
{
	while (USBADR & 0x80);
	USBADR = addr & 0x7f;
	USBDAT = dat;
}

BYTE ReadFifo(BYTE fifo, BYTE *pdat)
{
	BYTE cnt;
	BYTE ret;
	ret = cnt = ReadReg(COUNT0);
	while (cnt--)
	{
		*pdat++ = ReadReg(fifo);
	}
	return ret;
}

void WriteFifo(BYTE fifo, BYTE *pdat, BYTE cnt)
{
	while (cnt--)
	{
		WriteReg(fifo, *pdat++);
	}
}

void UsbInit()
{
	USBCON = 0x90;
	PUSBH = 1;		//USB 中断优先级为 3 级（最高级）
	PUSB = 1;
	WriteReg(FADDR, 0x00);
	WriteReg(POWER, 0x08);
	WriteReg(INTRIN1E, 0x3f);
	WriteReg(INTROUT1E, 0x3f);
	WriteReg(INTRUSBE, 0x00);
	WriteReg(POWER, 0x01);
	Ep0Stage.bStage = EPIDLE;
}

void usb_isr() interrupt USB_VECTOR
{
	BYTE intrusb;
	BYTE intrin;
	BYTE introut;
	BYTE csr;
	BYTE cnt;
	WORD len;
    BYTE adrTemp;

    adrTemp = USBADR;     //USBADR 现场保存，避免主循环里写完 USBADR 后产生中断，在中断里修改了 USBADR 内容
	
	intrusb = ReadReg(INTRUSB);
	intrin = ReadReg(INTRIN1);
	introut = ReadReg(INTROUT1);
	if (intrusb & RSTIF)
	{
		WriteReg(INDEX, 1);
		WriteReg(INCSR1, INCLRDT);
		WriteReg(INDEX, 1);
		WriteReg(OUTCSR1, OUTCLRDT);
		Ep0Stage.bStage = EPIDLE;
	}
	if (intrin & EP0IF)
	{
		WriteReg(INDEX, 0);
		csr = ReadReg(CSR0);
		if (csr & STSTL)
		{
			WriteReg(CSR0, csr & ~STSTL);
			Ep0Stage.bStage = EPIDLE;
		}
		if (csr & SUEND)
		{
			WriteReg(CSR0, csr | SSUEND);
		}
		switch (Ep0Stage.bStage)
		{
			case EPIDLE:
			if (csr & OPRDY)
			{
				Ep0Stage.bStage = EPSTATUS;
				ReadFifo(FIFO0, (BYTE *)&Setup);
				((BYTE *)&Ep0Stage.wResidue)[0] = Setup.wLengthH;
				((BYTE *)&Ep0Stage.wResidue)[1]= Setup.wLengthL;
				switch (Setup.bmRequestType & REQUEST_MASK)
				{
					case STANDARD_REQUEST:
					switch (Setup.bRequest)
					{
						case SET_ADDRESS:
							WriteReg(FADDR, Setup.wValueL);
						break;
						
						case SET_CONFIG:
							WriteReg(INDEX, 1);
							WriteReg(INCSR2, INMODEIN);
							WriteReg(INMAXP, 8);
							WriteReg(INDEX, 1);
							WriteReg(INCSR2, INMODEOUT);
							WriteReg(OUTMAXP, 8);
							WriteReg(INDEX, 0);
						break;
						
						case GET_DESCRIPTOR:
							Ep0Stage.bStage = EPDATAIN;
							switch (Setup.wValueH)
							{
								case DESC_DEVICE:
									Ep0Stage.pData = DEVICEDESC;
									len = sizeof(DEVICEDESC);
								break;
								
								case DESC_CONFIG:
									Ep0Stage.pData = CONFIGDESC;
									len = sizeof(CONFIGDESC);
								break;
								
								case DESC_STRING:
								switch (Setup.wValueL)
								{
									case 0:
										Ep0Stage.pData = LANGIDDESC;
										len = sizeof(LANGIDDESC);
									break;
									
									case 1:
										Ep0Stage.pData = MANUFACTDESC;
										len = sizeof(MANUFACTDESC);
									break;
									
									case 2:
										Ep0Stage.pData = PRODUCTDESC;
										len = sizeof(PRODUCTDESC);
									break;
									
									default:
										Ep0Stage.bStage = EPSTALL;
									break;
								}
								break;
								
								case DESC_HIDREPORT:
									Ep0Stage.pData = HIDREPORTDESC;
									len = sizeof(HIDREPORTDESC);
								break;
								
								default:
									Ep0Stage.bStage = EPSTALL;
								break;
							}
							if (len < Ep0Stage.wResidue)
							{
								Ep0Stage.wResidue = len;
							}
						break;
							
						default:
							Ep0Stage.bStage = EPSTALL;
						break;
					}
					break;
					
					case CLASS_REQUEST:
						switch (Setup.bRequest)
						{
							case GET_REPORT:
								Ep0Stage.pData = HidFreature;
								Ep0Stage.bStage = EPDATAIN;
							break;
							
							case SET_REPORT:
								Ep0Stage.pData = HidFreature;
								Ep0Stage.bStage = EPDATAOUT;
							break;
							
							case SET_IDLE:
							break;
							
							case GET_IDLE:
							case GET_PROTOCOL:
							case SET_PROTOCOL:
							default:
								Ep0Stage.bStage = EPSTALL;
							break;
						}
					break;
						
					default:
						Ep0Stage.bStage = EPSTALL;
					break;
				}
				
				switch (Ep0Stage.bStage)
				{
					case EPDATAIN:
						WriteReg(CSR0, SOPRDY);
						goto L_Ep0SendData;
					break;
					
					case EPDATAOUT:
						WriteReg(CSR0, SOPRDY);
					break;
					
					case EPSTATUS:
						WriteReg(CSR0, SOPRDY | DATEND);
						Ep0Stage.bStage = EPIDLE;
					break;
					
					case EPSTALL:
						WriteReg(CSR0, SOPRDY | SDSTL);
						Ep0Stage.bStage = EPIDLE;
					break;
				}
			}
			break;
			
			case EPDATAIN:
				if (!(csr & IPRDY))
				{
					L_Ep0SendData:
					cnt = Ep0Stage.wResidue > 64 ? 64 : Ep0Stage.wResidue;
					WriteFifo(FIFO0, Ep0Stage.pData, cnt);
					Ep0Stage.wResidue -= cnt;
					Ep0Stage.pData += cnt;
					if (Ep0Stage.wResidue == 0)
					{
						WriteReg(CSR0, IPRDY | DATEND);
						Ep0Stage.bStage = EPIDLE;
					}
					else
					{
						WriteReg(CSR0, IPRDY);
					}
				}
			break;
			
			case EPDATAOUT:
			if (csr & OPRDY)
			{
				cnt = ReadFifo(FIFO0, Ep0Stage.pData);
				Ep0Stage.wResidue -= cnt;
				Ep0Stage.pData += cnt;
				if (Ep0Stage.wResidue == 0)
				{
					WriteReg(CSR0, SOPRDY | DATEND);
					Ep0Stage.bStage = EPIDLE;
				}
				else
				{
					WriteReg(CSR0, SOPRDY);
				}
			}
			break;
		}
	}
	
	if (intrin & EP1INIF)
	{
		WriteReg(INDEX, 1);
		csr = ReadReg(INCSR1);
		if (csr & INSTSTL)
		{
			WriteReg(INCSR1, INCLRDT);
		}
		if (csr & INUNDRUN)
		{
			WriteReg(INCSR1, 0);
		}
	}
	
	if (introut & EP1OUTIF)
	{
		WriteReg(INDEX, 1);
		csr = ReadReg(OUTCSR1);
		if (csr & OUTSTSTL)
		{
			WriteReg(OUTCSR1, OUTCLRDT);
		}
		if (csr & OUTOPRDY)
		{
			ReadFifo(FIFO1, HidInput);
			WriteReg(OUTCSR1, 0);
			
			if((HidInput[0]==0xaa) && (HidInput[1]==0x55) && (HidInput[2]==0x01))
			{
				WriteReg(INDEX, 1);
				WriteFifo(FIFO1, HidOutput, 64);
				WriteReg(INCSR1, INIPRDY);
			}
		}
	}

    USBADR = adrTemp;    //USBADR 现场恢复
}

char code DEVICEDESC[18] =
{
	0x12, //bLength(18);
	0x01, //bDescriptorType(Device);
	0x00,0x02, //bcdUSB(2.00);
	0x00, //bDeviceClass(0);
	0x00, //bDeviceSubClass0);
	0x00, //bDeviceProtocol(0);
	0x40, //bMaxPacketSize0(64);
	0xbf,0x34, //idVendor(34bf);
	0x03,0xf0, //idProduct(f003);
	0x00,0x01, //bcdDevice(1.00);
	0x01, //iManufacturer(1);
	0x02, //iProduct(2);
	0x00, //iSerialNumber(0);
	0x01, //bNumConfigurations(1);
};

char code CONFIGDESC[41] =
{
	0x09, //bLength(9);
	0x02, //bDescriptorType(Configuration);
	0x29,0x00, //wTotalLength(41);
	0x01, //bNumInterfaces(1);
	0x01, //bConfigurationValue(1);
	0x00, //iConfiguration(0);
	0x80, //bmAttributes(BUSPower);
	0x32, //MaxPower(100mA);
	0x09, //bLength(9);
	0x04, //bDescriptorType(Interface);
	0x00, //bInterfaceNumber(0);
	0x00, //bAlternateSetting(0);
	0x02, //bNumEndpoints(2);
	0x03, //bInterfaceClass(HID);
	0x00, //bInterfaceSubClass(0);
	0x00, //bInterfaceProtocol(0);
	0x00, //iInterface(0);
	0x09, //bLength(9);
	0x21, //bDescriptorType(HID);
	0x01,0x01, //bcdHID(1.01);
	0x00, //bCountryCode(0);
	0x01, //bNumDescriptors(1);
	0x22, //bDescriptorType(HID Report);
	0x1b,0x00, //wDescriptorLength(27);
	0x07, //bLength(7);
	0x05, //bDescriptorType(Endpoint);
	0x81, //bEndpointAddress(EndPoint1 as IN);
	0x03, //bmAttributes(Interrupt);
	0x40,0x00, //wMaxPacketSize(64);
	0x01, //bInterval(10ms);
	0x07, //bLength(7);
	0x05, //bDescriptorType(Endpoint);
	0x01, //bEndpointAddress(EndPoint1 as OUT);
	0x03, //bmAttributes(Interrupt);
	0x40,0x00, //wMaxPacketSize(64);
	0x01, //bInterval(10ms);
};

char code HIDREPORTDESC[27] =
{
	0x05,0x0c, //USAGE_PAGE(Consumer);
	0x09,0x01, //USAGE(Consumer Control);
	0xa1,0x01, //COLLECTION(Application);
	0x15,0x00, // LOGICAL_MINIMUM(0);
	0x25,0xff, // LOGICAL_MAXIMUM(255);
	0x75,0x08, // REPORT_SIZE(8);
	0x95,0x40, // REPORT_COUNT(64);
	0x09,0x01, // USAGE(Consumer Control);
	0xb1,0x02, // FEATURE(Data,Variable);
	0x09,0x01, // USAGE(Consumer Control);
	0x81,0x02, // INPUT(Data,Variable);
	0x09,0x01, // USAGE(Consumer Control);
	0x91,0x02, // OUTPUT(Data,Variable);
	0xc0, //END_COLLECTION;
};

char code LANGIDDESC[4] =
{
	0x04,0x03,
	0x09,0x04,
};

char code MANUFACTDESC[8] =
{
	0x08,0x03,
	'A',0,
	'I',0,
	'C',0,
};

char code PRODUCTDESC[30] =
{
	0x1e,0x03,
	'A',0,
	'I',0,
	'C',0,
	' ',0,
	'U',0,
	'S',0,
	'B',0,
	' ',0,
	'D',0,
	'e',0,
	'v',0,
	'i',0,
	'c',0,
	'e',0,
};

//========================================================================
// 函数: void ADC_Config(void)
// 描述: ADC设置.
// 参数: NULL.
// 返回: NULL.
// 版本: V1.0, 2012-10-22
//========================================================================
void ADC_Config(void)
{
	P1M1 |= 0x08;
	P1M0 &= 0xf7;   //设置 P1.3 为 ADC 输入口

	ADCTIM = 0x3f;		//设置 ADC 内部时序，ADC采样时间建议设最大值
	ADCCFG = 0x2f;		//设置 ADC 时钟为系统时钟/2/16/16
	ADC_CONTR = 0x80; //使能 ADC 模块
}

//========================================================================
// 函数: void Timer0_Config(u16 tReload)
// 描述: 定时器设置.
// 参数: tReload: 定时时间..
// 返回: NULL.
// 版本: V1.0, 2012-10-22
//========================================================================
void Timer0_Config(WORD tReload)
{
	AUXR |= 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
	TH0 = (BYTE)(tReload / 256);
	TL0 = (BYTE)(tReload % 256);
	ET0 = 1;    //Timer0 interrupt enable
	TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: WORD Get_ADC12bitResult(BYTE channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
WORD Get_ADC12bitResult(BYTE channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
    ADC_START = 1;//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //清除ADC结束标志
    return  (((WORD)ADC_RES << 8) | ADC_RESL);
}

/********************** Timer0 1ms中断函数 ************************/
void timer0 (void) interrupt TMR0_VECTOR
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: BYTE GetCheckSum(BYTE *buf, BYTE len)
// 描述: 获取校验码
// 参数: buf:  字符串指针, len: 字节数
// 返回: none.
// 版本: VER1.0
// 日期: 2020-7-6
// 备注: 
//========================================================================
BYTE GetCheckSum(BYTE *buf, BYTE len) //获取校验码
{
    BYTE i;
    BYTE cs=0;
    for (i = 0; i < len; i++)
    {
        cs += buf[i];
    }
    return cs;
}

