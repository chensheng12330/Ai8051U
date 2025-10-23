#ifndef __SET_UART_H__
#define __SET_UART_H__

#include "AI8051U.H"

// UART枚举，用于指定相应的串口
typedef enum
{
    Uart1 = 0,
    Uart2,
    Uart3,
    Uart4
} uart_name;

// 串口缓冲区数组最大长度枚举值
typedef enum
{
    Uart1_Tx = 120, // 串口1发送缓冲区数组最大长度
    Uart2_Tx = 60,  // 串口2发送缓冲区数组最大长度
    Uart3_Tx = 60,  // 串口3发送缓冲区数组最大长度
    Uart4_Tx = 60,  // 串口4发送缓冲区数组最大长度
    Uart1_Rx = 120, // 串口1接收缓冲区数组最大长度
    Uart2_Rx = 60,  // 串口2接收缓冲区数组最大长度
    Uart3_Rx = 60,  // 串口3接收缓冲区数组最大长度
    Uart4_Rx = 60   // 串口4接收缓冲区数组最大长度
} uart_len;

// 宏定义部分
#define Uart_End "end"       // 结束标志
#define Uart1_P30_1 "uart10" // 引脚切换宏定义,前面的是RXD，后面的是TXD
#define Uart1_P36_7 "uart11"
#define Uart1_P16_7 "uart12"
#define Uart1_P43_4 "uart13"
#define Uart2_P12_3 "uart20"
#define Uart2_P42_3 "uart21"
#define Uart3_P00_1 "uart30"
#define Uart3_P50_1 "uart31"
#define Uart4_P02_3 "uart40"
#define Uart4_P52_3 "uart41"

#define Hex_Mode "\x01 hex"   // 单字节模式，使用了非显示的ascii字符来避免碰撞
#define Buff_Mode "\x02 buff" // 缓冲区模式

#define Use_Timer2 "\x03timer" // 使用定时器2作为串口波特率发生器，默认选择定时器2
#define Use_Timerx "\x04timer" // 使用串口对应的定时器作为波特率，例如串口1使用定时器1
//这里的x是固定参数，不可以填写为Use_Timer1这样子。在串口使用不同波特率的时候，就需要每个串口都使用自己对应的定时器。
//如果没有设置，默认都为定时器2，这样波特率会被后面设置的给覆盖掉。

#define Base_8b "\x050len0" // 8位数据位,无校验位，默认模式
#define Odd_9b "\x051len1"  // 9位数据位，奇校验位，需要注意的是，自动奇偶校验只有串口1/2拥有
#define Even_9b "\x051len0" // 9位数据位，偶校验位，串口3/4的奇偶校验需要自行设置S3TB8、S4TB8

// 可以跨文件调用的缓存数组
extern char xdata _uart1_rx_buff[Uart1_Rx];
extern char xdata _uart2_rx_buff[Uart2_Rx];
extern char xdata _uart3_rx_buff[Uart3_Rx];
extern char xdata _uart4_rx_buff[Uart4_Rx];
// 可以跨文件调用的接收数据长度，调用get_uart_state()获取串口状态时候可以同时使用
extern int rx_cnt[4];
// 这个是用于查询串口发送的忙标志，tx_state[Uart1]为1则标识发送忙，为0则标识发送空闲
extern char tx_state[4];

// 设置串口模式，默认配置为115200波特率，8位数据位，1位停止位.
// 变长参数，可变参数为波特率、超时中断数据位，串口切换引脚，最后需要使用Uart_End结束。
// 举个例子：set_uart_mode(Uart1, "9600bps", Uart1_P36_7, Uart_End);
// 这个的意思是设置串口1为9600波特率，8位数据位，1位停止位，并切换引脚为P36和P37上，超时中断为64byte
// 超时中断的作用是对数据自动分包，64byte就是没数据64个字节（根据波特率），就会自动中断，然后进行数据分包。
// 再举个例子：set_uart_mode(Uart1, "32byte", "115200bps", Uart_End);
// 这个的意思是设置串口1为115200波特率，并切换引脚为P30和P31上（默认引脚），超时中断为32byte。
// 变长参数部分支持乱序输入，波特率和超时中断需要带上单位bps和byte，中间不要有空格。
// 如果不输入变长参数，例如：set_uart_mode(Uart1, Uart_End);//则代表115200波特率，64byte，P30P31（UART1下）
// 即不输入的选项拥有默认值，不设置也可以的。
void set_uart_mode(uart_name uart, ...);
// 默认值为115200波特率，超时中断64byte，引脚切换每个串口的第一个（参见上面的宏定义）
// uart1是P30和P31，uart2是P12和P13，uart3是P00和P01，uart4是P02和P03

char get_uart_state(uart_name uart);
//  获取串口状态，返回值为0代表串口没有接收到数据，1代表串口接收到了数据，并且数据触发了超时中断。

//  可以对指定串口发出数据，分为三种模式：普通字符串打印模式、hex数据单字节输出模式、buff缓冲区输出模式。
//  普通字符串打印模式：uart_printf(Uart1, "num:%d\r\n",cnt);//可以像正常printf一样使用
//  hex数据单字节输出模式：uart_printf(Uart1, Hex_Mode, 0x12);//输出0x12一个字节的数据
//  buff缓冲区输出模式：uart_printf(Uart1, Buff_Mode, dat, 20);//输出20个字节的数据，从dat数组0地址开始。
void uart_printf(uart_name uart, ...);

// 用于设置串口的时钟频率，传入的是系统时钟频率，单位是Hz
// 一般来说不用管，设置的时候会自动设置
void set_uart_fosc(long fosc);

#endif