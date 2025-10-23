#include "AI8051U.H"
#include "ai_usb.h"
#include "set_io.h"
#include "set_spi.h"

/*
ʹ��˵����
ͨ��ʵ������LCD�ӿڸ��ò���MPU6050ģ�飬ʹ��ģ���ͺ�ΪGY-521
ģ���VCC����ʵ�����ϵ�DB6��GND����DB5��SCL����DB4��SDA����DB3��XDA~INT��Ҫ������������
������ʹ�öŰ��ߵĻ��Ͳ�Ҫ�ӣ���Ȼ���USB-CDC�ӿڼ��ɹ۲쵽��·���ٶȺͽ��ٶ����ݵ����
*/

#define AD9833_CS P11	// AD9833(DDSоƬ)��Ƭѡ
#define MCP41010_CS P47 // MCP41010(���ֵ�λ��оƬ)��Ƭѡ

#define TRI_WAVE 0 // ������ǲ�
#define SIN_WAVE 1 // ������Ҳ�
#define SQU_WAVE 2 // �������

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

// Freq��λHz
void AD9833_WaveSeting(float Freq, unsigned int Freq_SFR, unsigned int WaveMode, unsigned int Phase)
{
	int frequence_LSB, frequence_MSB, Phs_data;
	float frequence_DATA;
	long int frequence_hex;

	/*********************************����Ƶ�ʵ�16����ֵ***********************************/
	// �������㹫ʽ��1Hz/1e6*(2^28/25)=10.73741824 (25ΪAD9833��ʱ��Ƶ��)
	// ���ʱ��Ƶ�ʲ�Ϊ25MHZ���޸ĸô���Ƶ��ֵ����λMHz ��AD9833���֧��25MHz
	frequence_DATA = Freq;
	frequence_DATA = frequence_DATA * 10.737418;
	frequence_hex = frequence_DATA;			// ���frequence_hex��ֵ��32λ��һ���ܴ�����֣���Ҫ��ֳ�����14λ���д���
	frequence_LSB = frequence_hex;			// frequence_hex��16λ�͸�frequence_LSB
	frequence_LSB = frequence_LSB & 0x3fff; // ȥ�������λ��16λ����ȥ����λ������14λ
	frequence_MSB = frequence_hex >> 14;	// frequence_hex��16λ�͸�frequence_HSB
	frequence_MSB = frequence_MSB & 0x3fff; // ȥ�������λ��16λ����ȥ����λ������14λ

	Phs_data = Phase | 0xC000; // ��λֵ
	Send_16bit_Dat(0x0100);	   // ��λAD9833,��RESETλΪ1
	Send_16bit_Dat(0x2100);	   // ѡ������һ��д�룬B28λ��RESETλΪ1

	if (Freq_SFR == 0) // ���������õ�����Ƶ�ʼĴ���0
	{
		frequence_LSB = frequence_LSB | 0x4000;
		frequence_MSB = frequence_MSB | 0x4000;
		// ʹ��Ƶ�ʼĴ���0�������
		Send_16bit_Dat(frequence_LSB); // L14��ѡ��Ƶ�ʼĴ���0�ĵ�14λ��������
		Send_16bit_Dat(frequence_MSB); // H14 Ƶ�ʼĴ����ĸ�14λ��������
		Send_16bit_Dat(Phs_data);	   // ������λ
									   // AD9833_Write(0x2000); /**����FSELECTλΪ0��оƬ���빤��״̬,Ƶ�ʼĴ���0�������**/
	}
	if (Freq_SFR == 1) // ���������õ�����Ƶ�ʼĴ���1
	{
		frequence_LSB = frequence_LSB | 0x8000;
		frequence_MSB = frequence_MSB | 0x8000;
		// ʹ��Ƶ�ʼĴ���1�������
		Send_16bit_Dat(frequence_LSB); // L14��ѡ��Ƶ�ʼĴ���1�ĵ�14λ����
		Send_16bit_Dat(frequence_MSB); // H14 Ƶ�ʼĴ���1Ϊ
		Send_16bit_Dat(Phs_data);	   // ������λ
									   // AD9833_Write(0x2800); /**����FSELECTλΪ0������FSELECTλΪ1����ʹ��Ƶ�ʼĴ���1��ֵ��оƬ���빤��״̬,Ƶ�ʼĴ���1�������**/
	}

	if (WaveMode == TRI_WAVE) // ������ǲ�����
		Send_16bit_Dat(0x2002);
	if (WaveMode == SQU_WAVE) // �����������
		Send_16bit_Dat(0x2028);
	if (WaveMode == SIN_WAVE) // ������Ҳ���
		Send_16bit_Dat(0x2000);
}

void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	EA = 1; // �����ж�
	set_io_mode(pp_mode, Pin11, Pin47, Pin32, Pin33, Pin_End);
	set_spi_mode(SPI0, Spi_P35_4_3_2, Sw_MOSI_MISO, In_Out, Spi_End); // �����ã���������Ĭ��ֵ
	// AD9833_WaveSeting(1234.5,0,TRI_WAVE,0 );//1.2345KHz,Ƶ�ʼĴ���0�����ǲ���� ,����λ0
	// D9833_WaveSeting(5000,0,SQU_WAVE,90);	//5KHz,		Ƶ�ʼĴ���0��������� 	,����λ90
	AD9833_WaveSeting(1000.0, 0, SIN_WAVE, 0); // 1KHz,	Ƶ�ʼĴ���0�����Ҳ���� ,����λ0
	AD9833_AmpSet(200);						   // ���÷�ֵ����ֵ��� 255
	while (1)
		;
}
