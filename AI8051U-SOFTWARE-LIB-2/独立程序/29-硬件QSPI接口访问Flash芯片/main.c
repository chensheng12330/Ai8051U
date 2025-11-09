/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "w25qxx.h"
#include "AI8051U_QSPI.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_UART.h"
#include "AI8051U_Switch.h"

/*************   功能说明   ***************

通过硬件QSPI接口1线、2线、4线模式对支持QSPI协议的Flash进行读写测试。

串口(P3.0,P3.1)打印访问结果，默认设置: 115200,8,N,1.

下载时, 选择时钟 40MHz (可以在配置文件"config.h"中修改).

******************************************/


/*************    本地常量声明    **************/


/******************* 引脚定义 ************************/

sbit    QSPI_CS     =   P4^0;
sbit    QSPI_IO0    =   P4^1;
sbit    QSPI_IO1    =   P4^2;
sbit    QSPI_SCK    =   P4^3;
sbit    QSPI_IO2    =   P5^2;
sbit    QSPI_IO3    =   P5^3;

/*************    本地变量声明    **************/

int i;
BYTE xdata buf[1024];

/*************    本地函数声明    **************/

#define SIZE            30

#define INIT_BUF()      for (i=0; i<SIZE; i++) buf[i] = 0;
#define SET_BUF()       for (i=0; i<SIZE; i++) buf[i] = (BYTE)(i + 0x00);
#define PRINT_BUF()     for (i=0; i<SIZE; i++)                              \
                        {                                                   \
                            printf("%02bx ", buf[i]);                       \
                            if ((i % 32) == 31)                             \
                                printf("\n                            ");   \
                        }                                                   \
                        printf("\n");

/*************  外部函数和变量声明 *****************/


/******************** IO口配置 ********************/
void GPIO_config(void)
{
    P4_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_3);    //P4.0, P4.3 设置为推挽输出
    P4_SPEED_HIGH(GPIO_Pin_LOW);                //P4.0~P4.3 设置快速电平转换
    P5_SPEED_HIGH(GPIO_Pin_2 | GPIO_Pin_3);     //P5.2, P5.3 设置快速电平转换
    P4_PULL_UP_ENABLE(GPIO_Pin_LOW);            //P4.0~P4.3 设置上拉使能
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2, P5.3 设置上拉使能
    P4_BP_ENABLE(GPIO_Pin_1 | GPIO_Pin_2);      //P4.1, P4.2 硬件自动设置端口模式
    P5_BP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3);      //P5.2, P5.3 硬件自动设置端口模式
    QSPI_SW(QSPI_P40_P41_P42_P52_P53_P43);      //QSPI_P14_P15_P16_P13_P12_P17,QSPI_P40_P41_P42_P52_P53_P43,QSPI_P47_P25_P26_P46_P45_P27

    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2 设置上拉使能
    
    QSPI_CS = 1;
    QSPI_SCK = 1;
    QSPI_IO0 = 1;
    QSPI_IO1 = 1;
    QSPI_IO2 = 1;
    QSPI_IO3 = 1;
}

/******************** SPI 配置 ********************/
void QSPI_config(void)
{
    QSPI_InitTypeDef QSPI_InitStructure;    //结构定义

    NVIC_QSPI_Init(DISABLE,Priority_0);     //中断使能设置, QSPI_SMIE/QSPI_FTIE/QSPI_TCIE/QSPI_TEIE/DISABLE; 优先级(低到高) Priority_0~Priority_3
    QSPI_InitStructure.FIFOLevel  = 31;     //设置FIFO阈值, 0~31
    QSPI_InitStructure.ClockDiv   = 3;      //设置QSPI时钟 = 系统时钟/(n+1), 0~255
    QSPI_InitStructure.CSHold     = 1;      //设置CS保持时间为(n+1)个QSPI时钟, 0~7
    QSPI_InitStructure.CKMode     = 1;      //设置空闲时CLK电平, 0/1
    QSPI_InitStructure.FlashSize  = 25;     //设置Flash大小为2^(25+1)=64M字节, 0~31
    QSPI_InitStructure.SIOO       = DISABLE;//发送一次指令模式, ENABLE(仅第一条事务发送指令)/DISABLE(每个事务均发送指令)
    QSPI_InitStructure.QSPI_EN    = ENABLE; //QSPI使能, ENABLE/DISABLE
    QSPI_Inilize(&QSPI_InitStructure);      //初始化
}

/******************** UART 配置 ********************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;     //结构定义

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //选择波特率发生器, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE 或 DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
}

/**********************************************/
void main(void)
{
    WTST = 0;        //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXSFR();        //扩展SFR(XFR)访问使能 
    CKCON = 0;      //提高访问XRAM速度

    GPIO_config();
    UART_config();
    QSPI_config();
    EA = 1;

    printf("QSPI Test !\n");
    printf("W25Q_ReadJEDECID_9F         %08lx\n", W25Q_ReadJEDECID_9F());
    printf("SR1: %02bx\n", W25Q_ReadSR1_05());
    printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    printf("SR3: %02bx\n", W25Q_ReadSR3_15());
    
    if ((W25Q_ReadSR2_35() & 0x02) == 0)
    {
        W25Q_WriteEnableVSR_50();
        if ((W25Q_ReadJEDECID_9F() & 0xffff) == 0x4014) //W25Q80写SR2方法不同
        {
            W25Q_WriteSR12_01(0x0002);
        }
        else
        {
            W25Q_WriteSR2_31(0x02);
        }
        printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    }
    
    printf("W25Q_Erase4K_20\n");
    W25Q_Erase4K_20(0, TRUE);
    
    printf("W25Q_ReadData_03            ");
    INIT_BUF();
    W25Q_ReadData_03(0, buf, SIZE);
    PRINT_BUF();

    printf("W25Q_PageProgram_02\n");
    SET_BUF();
    W25Q_PageProgram_02(0, buf, SIZE);
    
    printf("W25Q_ReadData_03            ");
    INIT_BUF();
    W25Q_ReadData_03(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_0B            ");
    INIT_BUF();
    W25Q_FastRead_0B(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_3B            ");
    INIT_BUF();
    W25Q_FastRead_3B(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_6B            ");
    INIT_BUF();
    W25Q_FastRead_6B(0, buf, SIZE);
    PRINT_BUF();

    while (1)
    {
    }
}
