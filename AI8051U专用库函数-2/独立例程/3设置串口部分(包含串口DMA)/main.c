#include "AI8051U.h"
#include "io_int.h"	  //设置IO独立中断的库函数，依赖于set_io.h
#include "set_io.h"	  //设置IO模式的库函数，可以阅读H文件中的详细说明，或者直接看以下例程的使用
#include "set_uart.h" //设置串口的库函数，丰富的使用方法详见H文件中的详细说明
#include "stdio.h"

/*
使用说明：
按下P32，串口1打印hello world。需要使用USB转串口接在TyepC下载口上，波特率是115200bps
使用串口1发送“cnt:123”(英文符号，引号不包括在内)，应该返回send num:123。
即发送什么数字，解析后返回什么数字。
*/

char tmp_str[5] = {'a', 'b', 'c', 'd', 'e'};
int cnt_dat = 0;
void main(void)
{
	EAXFR = 1; // 允许访问扩展寄存器
	WTST = 0;
	CKCON = 0;
	// AI8051U试验箱所需配置
	// 最后一个参数必须是Pin_End，否则函数无法停止解析，Pin_End是结束标志
	// 变长函数，可以设定任意数量的引脚
	set_io_mode(en_pur, Pin32, Pin_End); // 设置P32的上拉电阻开启

	// io_int.h 函数使用例程
	set_ioint_mode(falling_edge_mode, Pin32, Pin_End); // 设置P32的下降沿中断
	set_ioint_mode(en_int, Pin32, Pin_End);			// 设置P32的中断使能

	// set_uart.h 函数使用例程
	set_uart_mode(Uart1, Uart_End);
	// 设置串口1为默认模式，115200bps，64byte停止中断，默认timer2，Uart1_P30_1
	set_uart_mode(Uart4, "9600bps", Use_Timerx, Uart4_P02_3, Uart_End);
	// 设置串口4为9600bps，32byte停止中断，使用定时3（对应串口号的定时器），切换为Uart4_P02_3
	// 注意：如果使用了不同的波特率，则必须使用Use_Timerx。如果相同波特率，则可以共用默认的Timer2
	set_io_mode(pu_mode, Pin02, Pin03, Pin_End);
	// 配置对应串口前记得使用set_io_mode函数配置引脚,否则对应引脚为高阻模式无法输出数据
	EA = 1; // 打开总中断
	while (1)
	{
		if (get_uart_state(Uart1))
		{
			// 注意：使用sscanf需要引入stdio.h
			sscanf(_uart1_rx_buff, "cnt:%d", &cnt_dat); // 缓冲区可以查看set_uart.h中缓冲区的定义
			// sscanf用法，第一个参数是缓冲区，第二个参数是格式化字符串，第三个参数是变量地址
			uart_printf(Uart1, "send num:%d\r\n", (int)cnt_dat); // 串口1打印解析到的数据并显示
		}
		if (get_ioint_state(Pin32)) // 按下P32，串口1打印hello world
		{
			uart_printf(Uart1, "hello world!\r\n");
			// 普通printf用法，内嵌printf函数，可以通过第一个参数实现打印串口的选择
			// 本printf自带长度校验和串口忙标志，超过长度会不打印，请到set_uart.h中改变Uart_Tmp_Max
			// 如果连续调用printf，在第一个printf没有完成发送的情况下，后续的pritnf会被丢弃
			// 如果想要知道对应的串口发送是否忙，可以使用tx_state[Uart1]这样子来查询（这个是串口1的）

			// uart_printf(Uart1,Hex_Mode,0x0f);//输出0x0f单字节，类似直接给SBUF值
			// uart_printf(Uart1,Buff_Mode,tmp_str,5);//输出字符串tmp_str，5个字节
		}
	}
}