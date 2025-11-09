/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "ai_usb.h"
#include "AI8051U_USART_LIN.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Delay.h"
#include "AI8051U_Switch.h"

/*************   功能说明   ***************

Lin主机模式收发测试：
按一下P32口按键, 主机发送完整一帧数据.
按一下P33口按键, 主机发送帧头并获取从机应答数据（合并成一串完整的帧），USB-CDC串口打印应答数据.

Lin从机模式收发测试：
收到一个非本机应答的完整帧后通过USB-CDC串口输出.
收到一个本机应答的帧头后(例如：ID=0x12), 发送缓存数据进行应答.

默认传输速率：9600波特率, 用户可自行修改.

下载时, 选择时钟 40MHz (用户可在"config.h"修改频率).

******************************************/

sbit SLP_N  = P5^0;     //0: Sleep

/*************	本地常量声明	**************/

#define	LIN_MASTER_MODE		1    //0: 从机模式; 1: 主机模式

/*************	本地变量声明	**************/

bit Key1_Flag;
bit Key2_Flag;

u8 Key1_cnt;
u8 Key2_cnt;

u8 U2Lin_ID;
u8 USART2_BUF[8];

/*************	本地函数声明	**************/


/*************  外部函数和变量声明 *****************/

extern bit B_ULinRX2_Flag;

/******************** IO口配置 ********************/
void GPIO_config(void)
{
	P4_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3); //P4.2,P4.3 设置为准双向口
	P5_MODE_IO_PU(GPIO_Pin_0);		//P5.0 设置为准双向口
    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2,P3.3 使能内部上拉
}

/******************** LIN 配置 ********************/
void LIN_config(void)
{
	USARTx_LIN_InitDefine LIN_InitStructure;            //结构定义

#if(LIN_MASTER_MODE==1)
	LIN_InitStructure.LIN_Mode = LinMasterMode;         //LIN总线模式  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = DISABLE;           //自动同步使能  	ENABLE,DISABLE
#else
	LIN_InitStructure.LIN_Mode = LinSlaveMode;          //LIN总线模式  	LinMasterMode,LinSlaveMode
	LIN_InitStructure.LIN_AutoSync = ENABLE;            //自动同步使能  	ENABLE,DISABLE
#endif
	LIN_InitStructure.LIN_Enable   = ENABLE;		    //LIN功能使能  	ENABLE,DISABLE
	LIN_InitStructure.LIN_Baudrate = 9600;			    //LIN波特率
    LIN_InitStructure.TimeOutEnable  = ENABLE;          //接收超时使能, ENABLE,DISABLE
    LIN_InitStructure.TimeOutINTEnable  = ENABLE;       //超时中断使能, ENABLE,DISABLE
    LIN_InitStructure.TimeOutScale  = TO_SCALE_BRT;     //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
    LIN_InitStructure.TimeOutTimer  = 32ul;             //超时时间, 1 ~ 0xffffff
	UASRT_LIN_Configuration(USART2,&LIN_InitStructure); //LIN 初始化

	NVIC_UART2_Init(ENABLE,Priority_1);     //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    UART2_SW(UART2_SW_P42_P43);     //UART2_SW_P12_P13,UART2_SW_P42_P43
}

/**********************************************/
void main(void)
{
	u8 i;

	WTST = 0;		//设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
	EAXSFR();		//扩展SFR(XFR)访问使能 
	CKCON = 0;      //提高访问XRAM速度

	GPIO_config();
	LIN_config();
    usb_init();     //USB CDC 接口配置
	EA = 1;
	//====初始化数据=====
	SLP_N = 1;
	U2Lin_ID = 0x32;
	USART2_BUF[0] = 0x81;
	USART2_BUF[1] = 0x22;
	USART2_BUF[2] = 0x33;
	USART2_BUF[3] = 0x44;
	USART2_BUF[4] = 0x55;
	USART2_BUF[5] = 0x66;
	USART2_BUF[6] = 0x77;
	USART2_BUF[7] = 0x88;

	while (1)
	{
		delay_ms(1);
#if(LIN_MASTER_MODE==1)
		if(!P32)
		{
			if(!Key1_Flag)
			{
				Key1_cnt++;
				if(Key1_cnt > 50)
				{
					Key1_Flag = 1;
					UsartLinSendFrame(USART2,U2Lin_ID, USART2_BUF, FRAME_LEN);  //发送一串完整数据
				}
			}
		}
		else
		{
			Key1_cnt = 0;
			Key1_Flag = 0;
		}

		if(!P33)
		{
			if(!Key2_Flag)
			{
				Key2_cnt++;
				if(Key2_cnt > 50)
				{
					Key2_Flag = 1;
					UsartLinSendHeader(USART2,0x13);  //发送帧头，获取数据帧，组成一个完整的帧
				}
			}
		}
		else
		{
			Key2_cnt = 0;
			Key2_Flag = 0;
		}
#else
		if((B_ULinRX2_Flag) && (COM2.RX_Cnt >= 2))
		{
			B_ULinRX2_Flag = 0;

			if((RX2_Buffer[0] == 0x55) && ((RX2_Buffer[1] & 0x3f) == 0x12)) //PID -> ID
			{
				UsartLinSendData(USART2,USART2_BUF,FRAME_LEN);
				UsartLinSendChecksum(USART2,USART2_BUF,FRAME_LEN);
			}
		}
#endif

        if(COM2.RX_TimeOut)     //超时计数
        {
            COM2.RX_TimeOut = 0;

            printf_usb("Read Cnt = %d.\r\n",COM2.RX_Cnt);
            for(i=0; i<COM2.RX_Cnt; i++)    printf_usb("0x%02x ",RX2_Buffer[i]);    //从串口输出收到的从机数据
            COM2.RX_Cnt  = 0;   //清除字节数
            printf_usb("\r\n");
		}

        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,OutNumber);   //发送数据缓冲区，长度（接收数据原样返回, 用于测试）
            
            usb_OUT_done();
        }
	}
}
