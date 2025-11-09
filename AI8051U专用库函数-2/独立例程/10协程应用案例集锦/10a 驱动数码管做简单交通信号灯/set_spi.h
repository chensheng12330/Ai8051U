#ifndef __SET_SPI_H__
#define __SET_SPI_H__

#include "AI8051U.H"
// SPI枚举，用于指定相应的SPI部分
typedef enum
{
    SPI0, // SPI外设0
    SPI1, // SPI外设1,占用串口1资源，作为串口1的复用功能
    SPI2  // SPI外设2,占用串口2资源，作为串口2的复用功能
} spi_name;

// 串口缓冲区数组最大长度枚举值
typedef enum
{
    _Spi0_Tx = 120, // SPI0发送缓冲区数组最大长度
    _Spi1_Tx = 60,  // SPI1发送缓冲区数组最大长度
    _Spi2_Tx = 60,  // SPI2发送缓冲区数组最大长度
    _Spi0_Rx = 120, // SPI0接收缓冲区数组最大长度
    _Spi1_Rx = 60,  // SPI1接收缓冲区数组最大长度
    _Spi2_Rx = 60,  // SPI2接收缓冲区数组最大长度
} spi_len;

extern char xdata _spi0_rx_buff[_Spi0_Rx];
extern char xdata _spi1_rx_buff[_Spi1_Rx];
extern char xdata _spi2_rx_buff[_Spi2_Rx];

/*SPI设置结束标志*/
#define Spi_End "end"           // SPI结束标记

/*SPI主从模式，默认为主模式*/
#define Spi_Master "\x01masl1" // SPI主模式，默认为主模式
// #define Spi_Slave "\x02masl0"   // SPI从模式,暂时还没写，等待补充完善

/*SPI功能使能，默认为使能*/
#define Spi_Enable "\x01en_dis1"   // SPI使能，默认为使能
#define Spi_Disable "\x01en_dis0" // SPI功能禁用

/*SPI引脚交换功能，默认为不交换*/
#define NoSw_MOSI_MISO "\x01sw0"     // 不交换MOSI和MISO引脚的顺序，默认为不交换
#define Sw_MOSI_MISO "\x01sw1"     // 交换MOSI和MISO引脚的顺序

/*SPI引脚切换，默认为Spi_P14_5_6_7,目前只支持主机模式，所以SS引脚是普通IO口，可以自行控制*/
#define Spi_P14_5_6_7 "spi0" // 从前往后分别对应SS-MOSI-MISO-SCLK
#define Spi_P24_5_6_7 "spi1" // 从前往后分别对应SS-MOSI-MISO-SCLK
#define Spi_P40_1_2_3 "spi2" // 从前往后分别对应SS-MOSI-MISO-SCLK
#define Spi_P35_4_3_2 "spi3" // 从前往后分别对应SS-MOSI-MISO-SCLK

/*SPI时钟分频设置，默认为Spi_ClkDiv_16，16分频*/
#define Spi_ClkDiv_2 "div3"
#define Spi_ClkDiv_4 "div0"
#define Spi_ClkDiv_8 "div1"
#define Spi_ClkDiv_16 "div2"

/*SPI数据高低位置设置，默认为MSB，高位优先*/
#define MSB "sign0"           // Most Significant Bit,先发送/接收数据的高位
#define LSB "sign1"           // Least Significant Bit,先发送/接收数据的低位

/*SPI时钟极性(也叫CPOL)设置，默认为High_Falling，SCLK空闲为高电平，第一个沿为下降沿(CPOL=1)*/
#define Low_Rising "level0"    // SCLK 空闲时为低电平，SCLK 的前时钟沿为上升沿，后时钟沿为下降沿
#define High_Falling "level1" // SCLK 空闲时为高电平，SCLK 的前时钟沿为下降沿，后时钟沿为上升沿
#define Cpol_0 "level0"    // 同Low_Rising，别名
#define Cpol_1 "level1" // 同High_Falling，别名

/*SPI时钟边沿(也叫CPHA)设置，默认为Out_In，数据在 SCLK 的前时钟沿驱动，后时钟沿采样(CPHA=1)*/
#define Out_In "clock1"    //数据在 SCLK 的前时钟沿驱动，后时钟沿采样
#define In_Out "clock0"    //数据在 SCLK 的后时钟沿驱动，前时钟沿采样
#define Cpha_0 "level0"    // 同Out_In，别名
#define Cpha_1 "level1" // 同In_Out，别名

// 用于设置SPI的各种初始化参数，所有参数都支持默认值和乱序输入，最后需要带上Spi_End结束标志
// 参数可以从上面直接复制，同一组的参数只能使用一个，使用多个是最后的参数生效，顺序没有要求
// 举个例子：设置SPI1的引脚为Spi_P14_5_6_7，时钟分频为2分频，SCLK空闲为高
// 数据在下降沿驱动，上升沿采样，数据为高位优先，不交换MOSI和MISO引脚，主机模式并使能SPI
// set_spi_mode(SPI1, Spi_P14_5_6_7, Spi_ClkDiv_2, High_Falling, MSB, Out_In, Spi_Enable, Spi_Master, NoSw_MOSI_MISO, Spi_End);
// 上面这个示例跟这个是等效的：set_spi_mode(SPI1, Spi_End);//全部使用默认值
// 也可以按需更改 set_spi_mode(SPI1, Spi_P24_5_6_7, Low_Rising, Spi_End);//只更改了引脚和驱动极性
void set_spi_mode(spi_name spi, ...);

// 用于获取SPI的当前状态，因为SPI是全双工的，所以当发送完成（同时也是接收完成）的时候就会返回1，否则返回0
char get_spi_state(spi_name spi);

#define Hex_Mode "\x01 hex"   // 单字节模式，使用了非显示的ascii字符来避免碰撞
#define Buff_Mode "\x02 buff" // 缓冲区模式

// 用于发送SPI的数据，分为Hex_Mode和Buff_Mode两种模式
// Hex_Mode为单字节发送，类似于直接操作SPI的数据寄存器发送一个byte的数据
// Buff_Mode为缓冲区发送，需要传入一个char类型的数组指针，然后再传入所需发送的数据长度
void spi_printf(spi_name spi, ...);

#endif