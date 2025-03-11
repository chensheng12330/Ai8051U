/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_USB.h"
#include "stc32_stc8_usb.h"

/*************   功能说明   ***************

USB功能测试用例，使用USB数据线连接电脑与MCU。

根据需要添加“stc_usb_cdc_32.lib”或者“stc_usb_hid_32.lib”库文件到项目。

使用USB接口打印"printf"信息的话，需要开启 "stc32_stc8_usb.h" 头文件里的"PRINTF_USB"定义。

注意：不要在其它程序里修改USB接口（P3.0/D-,P3.1/D+），以及USB相关寄存器的配置。

下载时, 选择时钟 40MHz (用户可在"config.h"修改频率).

******************************************/

//========================================================================
//                               本地常量声明	
//========================================================================


//========================================================================
//                               本地变量声明
//========================================================================

char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";

//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================


//========================================================================
// 函数: USB_Fun_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-02-07
//========================================================================
void USB_Fun_init(void)
{
    usb_init();     //USB 接口配置
}

//========================================================================
// 函数: Sample_USB
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2025-02-07
//========================================================================
void Sample_USB(void)
{
    if(DeviceState == DEVSTATE_CONFIGURED)  //USB完成配置后才能执行USB通信程序
    {
        if (bUsbOutReady)
        {
            printf("OutNumber=%bd\r\n",OutNumber);  //使用 printf 函数打印接收数据长度
            USB_SendData(UsbOutBuffer,OutNumber);   //发送数据缓冲区，长度（接收数据原样返回, 用于测试）
            
            usb_OUT_done();
        }
    }
}
