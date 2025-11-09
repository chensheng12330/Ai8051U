/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_I2C.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Timer.h"
#include "AI8051U_Switch.h"

/*************    功能说明    **************

通过硬件I2C接口读取AT24C02前8个字节数据，通过串口打印读取结果.

将读取的数据加1后写回AT24C02前8个字节.

重新读取AT24C02前8个字节数据，通过串口打印读取结果.

MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

串口配置UART1(P3.0,P3.1): 115200,N,8,1.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/

/*************    本地常量声明    **************/

#define SLAW        0xA0
#define SLAR        0xA1

/*************    本地变量声明    **************/


/*************    本地函数声明    **************/


/*************  外部函数和变量声明 *****************/

extern bit T0_Flag;

/******************** IO口配置 ********************/
void GPIO_config(void)
{
	P3_MODE_IO_PU(GPIO_Pin_LOW);	    //P3.0~P3.3 设置为准双向口
}

/************************ 定时器配置 ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;          //结构定义
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //指定工作模式,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_12T;        //指定时钟模式,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //是否输出定时器时钟, ENABLE或DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / (12*100UL)));    //中断频率, 100次/秒
    TIM_InitStructure.TIM_PS        = 100;                  //8位预分频器(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //是否初始化后启动定时器, ENABLE或DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //初始化Timer0, Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  串口初始化函数 *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure; //结构定义

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率, 一般 110 ~ 115200
//    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE 或 DISABLE
//    COMx_InitStructure.ParityMode  = PARITY_NONE;       //校验模式,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (使能校验位需要设置9位模式)
//    COMx_InitStructure.TimeOutEnable  = ENABLE;         //接收超时使能, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //超时中断使能, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
//    COMx_InitStructure.TimeOutTimer  = 32ul;            //超时时间, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);         //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/****************  I2C初始化函数 *****************/
void I2C_config(void)
{
    I2C_InitTypeDef I2C_InitStructure;

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C功能使能,   ENABLE, DISABLE
    I2C_InitStructure.I2C_MS_WDTA   = DISABLE;          //主机使能自动发送,  ENABLE, DISABLE
    I2C_InitStructure.I2C_Speed     = 16;               //总线速度=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
    NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);  //主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    I2C_SW(I2C_P32_P33);    //I2C_P24_P23,I2C_P15_P14,I2C_P32_P33
}

/******************** task A **************************/
void main(void)
{
    u8  i;
    u8  tmp[8];
    
    WTST = 0;   //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXSFR();   //扩展SFR(XFR)访问使能 
    CKCON = 0;  //提高访问XRAM速度

    GPIO_config();
    Timer_config();
    UART_config();
    I2C_config();
    EA = 1;
    
    printf("AI8051U I2C主机模式访问AT24C02程序\r\n");

    while (1)
    {
        if(T0_Flag)     //定时器每1秒钟设置一次中断标志
        {
            T0_Flag = 0;
            
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
    }
}
