/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明  **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过加载"usb_hid_32.lib"库文件，简便的实现使用USB接口打印数据信息，可用于程序调试.

P3.2口按键演示"printf"函数输出数据;

P3.3口按键演示"ShowLong"函数输出长整型数据;

P3.4口按键演示"ShowFloat"函数输出浮点数数据;

P3.5口按键演示"ShowCode"函数输出8字节数组数据;

在"config.h"文件里选择"PRINTF_SEGLED"定义，printf输出数据格式重定向到ISP下载软件中的7段数码管

在"config.h"文件里选择"PRINTF_HID"定义，printf输出直接重定向到USB HID接口

下载时, 选择时钟 24MHz (可在"config.h"文件里修改).

******************************************/

#include "config.h"
#include "../comm/AI8051U.h"
#include "../comm/usb.h"
#include <stdio.h>

void sys_init();

//USB调试及复位所需定义
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                      //设置自动复位到ISP区的用户接口命令

BYTE xdata cod[8];

void main()
{
    sys_init();
    usb_init();  //USB初始化
    EA = 1;
    
    while (1)
    {
        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,64);  //发送数据缓冲区，长度（接收数据原样返回, 用于测试）
            
            usb_OUT_done(); //接收应答（固定格式）
        }
        
        if (!P32)
        {
            while (!P32);
            printf("%08lx", 0x1234abcdL);  //使用printf函数重定向到USB输出数据
        }
        else if (!P33)
        {
            while (!P33);
            SEG7_ShowLong(0x98765432, 16);  //输出数码管长整型数据
        }
        else if (!P34)
        {
            while (!P34);
            SEG7_ShowFloat(3.1415);  //输出数码管浮点数数据
        }
        else if (!P35)
        {
            cod[0] = 0x3f;  //数码管段码
            cod[1] = 0x06;
            cod[2] = 0x5b;
            cod[3] = 0x4f;
            cod[4] = 0x66;
            cod[5] = 0x6d;
            cod[6] = 0x7d;
            cod[7] = 0x27;
            while (!P35);
            SEG7_ShowCode(cod);  //输出数码管码值
        }
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
}

