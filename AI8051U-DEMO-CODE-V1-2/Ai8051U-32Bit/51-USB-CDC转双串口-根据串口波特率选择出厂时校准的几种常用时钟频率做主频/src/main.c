/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试.

使用USB线连接核心板USB接口与电脑;

MCU通过USB CDC(Communication Device Class)协议识别为2个串口设备;

其中CDC1虚拟串口，通过TXD_2(P3.7), RXD_2(P3.6)接口收发; CDC2虚拟串口，通过TXD2_2(P4.3), RXD2_2(P4.2)接口收发。
将实验箱上的S2拨动开关拨到“通”位置即可进行CDC1与CDC2双串口间相互通信。
或者通过J6接口引线与外部串口设备进行通信。

可以通过“config.h”里面的“Dynamic_Frequency”定义启动根据串口通信波特率动态调整IRC主频，减少波特率计算误差。
使能后可根据串口波特率动态调整主频以提高串口通信精确度；
不过一个通道波特率调整引发主频变化后，另一个通道的波特率计算就会受影响；
不需要此功能可屏蔽这个定义。

串口通信最高波特率可达10Mbps.

下载时, 选择时钟 24MHZ (用户可自行修改频率)。

******************************************/

#include "stc.h"
#include "usb.h"
#include "uart.h"

unsigned long MAIN_Fosc = 24000000L;    //定义默认主时钟

bit Key_Flag;
bit Pwr_Flag;
u8  Key_cnt;

void sys_init();

void main()
{
    sys_init();
    uart_init();
    usb_init();
    EA = 1;
    
    while (1)
    {
        uart_polling();
    }
}

void sys_init()
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    S1_S1 = 0;
    S1_S0 = 1;      //UART1(RxD1_2/P3.6, TxD1_2/P3.7)
    S2_S = 1;       //UART2(RxD2_2/P4.2, TxD2_2/P4.3)
}
