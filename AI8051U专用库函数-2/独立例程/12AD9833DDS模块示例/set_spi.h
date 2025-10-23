#ifndef __SET_SPI_H__
#define __SET_SPI_H__

#include "AI8051U.H"
// SPIö�٣�����ָ����Ӧ��SPI����
typedef enum
{
    SPI0, // SPI����0
    SPI1, // SPI����1,ռ�ô���1��Դ����Ϊ����1�ĸ��ù���
    SPI2  // SPI����2,ռ�ô���2��Դ����Ϊ����2�ĸ��ù���
} spi_name;

// ���ڻ�����������󳤶�ö��ֵ
typedef enum
{
    _Spi0_Tx = 120, // SPI0���ͻ�����������󳤶�
    _Spi1_Tx = 60,  // SPI1���ͻ�����������󳤶�
    _Spi2_Tx = 60,  // SPI2���ͻ�����������󳤶�
    _Spi0_Rx = 120, // SPI0���ջ�����������󳤶�
    _Spi1_Rx = 60,  // SPI1���ջ�����������󳤶�
    _Spi2_Rx = 60,  // SPI2���ջ�����������󳤶�
} spi_len;

extern char xdata _spi0_rx_buff[_Spi0_Rx];
extern char xdata _spi1_rx_buff[_Spi1_Rx];
extern char xdata _spi2_rx_buff[_Spi2_Rx];

/*SPI���ý�����־*/
#define Spi_End "end"           // SPI�������

/*SPI����ģʽ��Ĭ��Ϊ��ģʽ*/
#define Spi_Master "\x01masl1" // SPI��ģʽ��Ĭ��Ϊ��ģʽ
// #define Spi_Slave "\x02masl0"   // SPI��ģʽ,��ʱ��ûд���ȴ���������

/*SPI����ʹ�ܣ�Ĭ��Ϊʹ��*/
#define Spi_Enable "\x01en_dis1"   // SPIʹ�ܣ�Ĭ��Ϊʹ��
#define Spi_Disable "\x01en_dis0" // SPI���ܽ���

/*SPI���Ž������ܣ�Ĭ��Ϊ������*/
#define NoSw_MOSI_MISO "\x01sw0"     // ������MOSI��MISO���ŵ�˳��Ĭ��Ϊ������
#define Sw_MOSI_MISO "\x01sw1"     // ����MOSI��MISO���ŵ�˳��

/*SPI�����л���Ĭ��ΪSpi_P14_5_6_7,Ŀǰֻ֧������ģʽ������SS��������ͨIO�ڣ��������п���*/
#define Spi_P14_5_6_7 "spi0" // ��ǰ����ֱ��ӦSS-MOSI-MISO-SCLK
#define Spi_P24_5_6_7 "spi1" // ��ǰ����ֱ��ӦSS-MOSI-MISO-SCLK
#define Spi_P40_1_2_3 "spi2" // ��ǰ����ֱ��ӦSS-MOSI-MISO-SCLK
#define Spi_P35_4_3_2 "spi3" // ��ǰ����ֱ��ӦSS-MOSI-MISO-SCLK

/*SPIʱ�ӷ�Ƶ���ã�Ĭ��ΪSpi_ClkDiv_16��16��Ƶ*/
#define Spi_ClkDiv_2 "div3"
#define Spi_ClkDiv_4 "div0"
#define Spi_ClkDiv_8 "div1"
#define Spi_ClkDiv_16 "div2"

/*SPI���ݸߵ�λ�����ã�Ĭ��ΪMSB����λ����*/
#define MSB "sign0"           // Most Significant Bit,�ȷ���/�������ݵĸ�λ
#define LSB "sign1"           // Least Significant Bit,�ȷ���/�������ݵĵ�λ

/*SPIʱ�Ӽ���(Ҳ��CPOL)���ã�Ĭ��ΪHigh_Falling��SCLK����Ϊ�ߵ�ƽ����һ����Ϊ�½���(CPOL=1)*/
#define Low_Rising "level0"    // SCLK ����ʱΪ�͵�ƽ��SCLK ��ǰʱ����Ϊ�����أ���ʱ����Ϊ�½���
#define High_Falling "level1" // SCLK ����ʱΪ�ߵ�ƽ��SCLK ��ǰʱ����Ϊ�½��أ���ʱ����Ϊ������
#define Cpol_0 "level0"    // ͬLow_Rising������
#define Cpol_1 "level1" // ͬHigh_Falling������

/*SPIʱ�ӱ���(Ҳ��CPHA)���ã�Ĭ��ΪOut_In�������� SCLK ��ǰʱ������������ʱ���ز���(CPHA=1)*/
#define Out_In "clock1"    //������ SCLK ��ǰʱ������������ʱ���ز���
#define In_Out "clock0"    //������ SCLK �ĺ�ʱ����������ǰʱ���ز���
#define Cpha_0 "level0"    // ͬOut_In������
#define Cpha_1 "level1" // ͬIn_Out������

// ��������SPI�ĸ��ֳ�ʼ�����������в�����֧��Ĭ��ֵ���������룬�����Ҫ����Spi_End������־
// �������Դ�����ֱ�Ӹ��ƣ�ͬһ��Ĳ���ֻ��ʹ��һ����ʹ�ö�������Ĳ�����Ч��˳��û��Ҫ��
// �ٸ����ӣ�����SPI1������ΪSpi_P14_5_6_7��ʱ�ӷ�ƵΪ2��Ƶ��SCLK����Ϊ��
// �������½��������������ز���������Ϊ��λ���ȣ�������MOSI��MISO���ţ�����ģʽ��ʹ��SPI
// set_spi_mode(SPI1, Spi_P14_5_6_7, Spi_ClkDiv_2, High_Falling, MSB, Out_In, Spi_Enable, Spi_Master, NoSw_MOSI_MISO, Spi_End);
// �������ʾ��������ǵ�Ч�ģ�set_spi_mode(SPI1, Spi_End);//ȫ��ʹ��Ĭ��ֵ
// Ҳ���԰������ set_spi_mode(SPI1, Spi_P24_5_6_7, Low_Rising, Spi_End);//ֻ���������ź���������
void set_spi_mode(spi_name spi, ...);

// ���ڻ�ȡSPI�ĵ�ǰ״̬����ΪSPI��ȫ˫���ģ����Ե�������ɣ�ͬʱҲ�ǽ�����ɣ���ʱ��ͻ᷵��1�����򷵻�0
char get_spi_state(spi_name spi);

#define Hex_Mode "\x01 hex"   // ���ֽ�ģʽ��ʹ���˷���ʾ��ascii�ַ���������ײ
#define Buff_Mode "\x02 buff" // ������ģʽ

// ���ڷ���SPI�����ݣ���ΪHex_Mode��Buff_Mode����ģʽ
// Hex_ModeΪ���ֽڷ��ͣ�������ֱ�Ӳ���SPI�����ݼĴ�������һ��byte������
// Buff_ModeΪ���������ͣ���Ҫ����һ��char���͵�����ָ�룬Ȼ���ٴ������跢�͵����ݳ���
void spi_printf(spi_name spi, ...);

#endif