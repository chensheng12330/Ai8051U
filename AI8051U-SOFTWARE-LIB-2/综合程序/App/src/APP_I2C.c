/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_I2C.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Delay.h"
#include "AI8051U_Switch.h"

/*************	功能说明	**************

通过硬件I2C接口读取AT24C02前8个字节数据，通过串口打印读取结果.

将读取的数据加1后写回AT24C02前8个字节.

重新读取AT24C02前8个字节数据，通过串口打印读取结果.

MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

串口配置UART1(P3.0,P3.1): 115200,N,8,1.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/

//========================================================================
//                               本地常量声明	
//========================================================================

#define SLAW        0xA0
#define SLAR        0xA1

//========================================================================
//                               本地变量声明
//========================================================================


//========================================================================
//                               本地函数声明
//========================================================================


//========================================================================
//                            外部函数和变量声明
//========================================================================


//========================================================================
// 函数: I2C_PS_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-25
//========================================================================
void I2C_24C02_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;
    COMx_InitDefine COMx_InitStructure; //结构定义

	P3_MODE_IO_PU(GPIO_Pin_LOW);	    //P3.0~P3.3 设置为准双向口
	I2C_SW(I2C_P32_P33);				//I2C_P24_P23,I2C_P15_P14,I2C_P32_P33

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C功能使能,   ENABLE, DISABLE
    I2C_InitStructure.I2C_MS_WDTA   = DISABLE;          //主机使能自动发送,  ENABLE, DISABLE
    I2C_InitStructure.I2C_Speed     = 16;               //总线速度=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
    NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);  //主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1/BRT_Timer2
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE或DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

//========================================================================
// 函数: Sample_I2C_PS
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-25
//========================================================================
void Sample_I2C_24C02(void)
{
    u8  i;
    u8  tmp[8];

    I2C_ReadNbyte(SLAW, 0, tmp, 8);
    printf("Read = ");      //打印读取数值
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
        tmp[i]++;           //读取数值+1
    }
    printf("\r\n");

    I2C_WriteNbyte(SLAW, 0, tmp, 8);  //写入新的数值
}
