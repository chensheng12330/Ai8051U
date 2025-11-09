#include "AI8051U.H"
#include "ai_usb.h"
#include "set_io.h"
#include "set_spi.h"

/*
使用说明：
通过实验箱上LCD接口复用插入MPU6050模块，使用模块型号为GY-521
模块的VCC连接实验箱上的DB6，GND连接DB5，SCL连接DB4，SDA连接DB3，XDA~INT不要焊接排针连接
（或者使用杜邦线的话就不要接），然后打开USB-CDC接口即可观察到三路加速度和角速度数据的输出
*/

#define AD9833_CS P11	// AD9833(DDS芯片)的片选
#define MCP41010_CS P47 // MCP41010(数字电位器芯片)的片选

#define TRI_WAVE 0 // 输出三角波
#define SIN_WAVE 1 // 输出正弦波
#define SQU_WAVE 2 // 输出方波

char spi_dat[2] = {0};

void Send_16bit_Dat(unsigned int dat)
{
	AD9833_CS = 0;
	spi_dat[0] = dat >> 8;
	spi_dat[1] = dat & 0xff;
	spi_printf(SPI0, Buff_Mode, spi_dat, 2);
	while (!get_spi_state(SPI0))
		;
	AD9833_CS = 1;
}

void AD9833_AmpSet(unsigned char Amp)
{
	MCP41010_CS = 0;
	spi_dat[0] = 0x11;
	spi_dat[1] = Amp;
	spi_printf(SPI0, Buff_Mode, spi_dat, 2);
	while (!get_spi_state(SPI0))
		;
	MCP41010_CS = 1;
}

// Freq单位Hz
void AD9833_WaveSeting(float Freq, unsigned int Freq_SFR, unsigned int WaveMode, unsigned int Phase)
{
	int frequence_LSB, frequence_MSB, Phs_data;
	float frequence_DATA;
	long int frequence_hex;

	/*********************************计算频率的16进制值***********************************/
	// 常量计算公式：1Hz/1e6*(2^28/25)=10.73741824 (25为AD9833的时钟频率)
	// 如果时钟频率不为25MHZ，修改该处的频率值，单位MHz ，AD9833最大支持25MHz
	frequence_DATA = Freq;
	frequence_DATA = frequence_DATA * 10.737418;
	frequence_hex = frequence_DATA;			// 这个frequence_hex的值是32位的一个很大的数字，需要拆分成两个14位进行处理；
	frequence_LSB = frequence_hex;			// frequence_hex低16位送给frequence_LSB
	frequence_LSB = frequence_LSB & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位
	frequence_MSB = frequence_hex >> 14;	// frequence_hex高16位送给frequence_HSB
	frequence_MSB = frequence_MSB & 0x3fff; // 去除最高两位，16位数换去掉高位后变成了14位

	Phs_data = Phase | 0xC000; // 相位值
	Send_16bit_Dat(0x0100);	   // 复位AD9833,即RESET位为1
	Send_16bit_Dat(0x2100);	   // 选择数据一次写入，B28位和RESET位为1

	if (Freq_SFR == 0) // 把数据设置到设置频率寄存器0
	{
		frequence_LSB = frequence_LSB | 0x4000;
		frequence_MSB = frequence_MSB | 0x4000;
		// 使用频率寄存器0输出波形
		Send_16bit_Dat(frequence_LSB); // L14，选择频率寄存器0的低14位数据输入
		Send_16bit_Dat(frequence_MSB); // H14 频率寄存器的高14位数据输入
		Send_16bit_Dat(Phs_data);	   // 设置相位
									   // AD9833_Write(0x2000); /**设置FSELECT位为0，芯片进入工作状态,频率寄存器0输出波形**/
	}
	if (Freq_SFR == 1) // 把数据设置到设置频率寄存器1
	{
		frequence_LSB = frequence_LSB | 0x8000;
		frequence_MSB = frequence_MSB | 0x8000;
		// 使用频率寄存器1输出波形
		Send_16bit_Dat(frequence_LSB); // L14，选择频率寄存器1的低14位输入
		Send_16bit_Dat(frequence_MSB); // H14 频率寄存器1为
		Send_16bit_Dat(Phs_data);	   // 设置相位
									   // AD9833_Write(0x2800); /**设置FSELECT位为0，设置FSELECT位为1，即使用频率寄存器1的值，芯片进入工作状态,频率寄存器1输出波形**/
	}

	if (WaveMode == TRI_WAVE) // 输出三角波波形
		Send_16bit_Dat(0x2002);
	if (WaveMode == SQU_WAVE) // 输出方波波形
		Send_16bit_Dat(0x2028);
	if (WaveMode == SIN_WAVE) // 输出正弦波形
		Send_16bit_Dat(0x2000);
}

void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	usb_init();
	EA = 1; // 打开总中断
	set_io_mode(pp_mode, Pin11, Pin47, Pin32, Pin33, Pin_End);
	set_spi_mode(SPI0, Spi_P35_4_3_2, Sw_MOSI_MISO, In_Out, Spi_End); // 简单配置，其他采用默认值
	// AD9833_WaveSeting(1234.5,0,TRI_WAVE,0 );//1.2345KHz,频率寄存器0，三角波输出 ,初相位0
	// D9833_WaveSeting(5000,0,SQU_WAVE,90);	//5KHz,		频率寄存器0，方波输出 	,初相位90
	AD9833_WaveSeting(1000.0, 0, SIN_WAVE, 0); // 1KHz,	频率寄存器0，正弦波输出 ,初相位0
	AD9833_AmpSet(200);						   // 设置幅值，幅值最大 255
	while (1)
		;
}
