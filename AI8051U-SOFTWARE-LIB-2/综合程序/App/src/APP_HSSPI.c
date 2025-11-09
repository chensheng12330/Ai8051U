/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_HSSPI.h"
#include "APP_SPI_Flash.h"
#include "AI8051U_Clock.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_SPI.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Switch.h"

/*************    功能说明    **************

通过高速SPI对Flash进行读写测试。

串口发指令对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

默认波特率:  115200,8,N,1. 可以在"串口初始化函数"里修改.

串口命令设置: (字母不区分大小写)
    E 0x001234              --> 扇区擦除，指定十六进制地址.
    W 0x001234 1234567890   --> 写入操作，指定十六进制地址，后面为写入内容.
    R 0x001234 10           --> 读出操作，指定十六进制地址，后面为读出字节. 
    C                       --> 如果检测不到指定SPI Flash，发送C强制允许操作.

注意：为了通用，程序不识别地址是否有效，用户自己根据具体的型号来决定。

下载时, 选择时钟 24MHz (PLL输入时钟为12M，建议用12M整数倍频率).

******************************************/

//========================================================================
//                               本地常量声明    
//========================================================================

sbit SPI_CE  = P4^0;     //PIN1
sbit SPI_SO  = P4^2;     //PIN2
sbit SPI_SI  = P4^1;     //PIN5
sbit SPI_SCK = P4^3;     //PIN6

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
// 函数: HSSPI_init
// 描述: 用户初始化程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void HSSPI_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    COMx_InitDefine COMx_InitStructure;     //结构定义

    //----------------------------------------------
    P4_MODE_IO_PU(GPIO_Pin_LOW);    //P40~P43 设置为准双向口
    P4_SPEED_HIGH(GPIO_Pin_LOW);    //电平转换速度快（提高IO口翻转速度）
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2,P5.3 内部上拉使能
    SPI_SW(SPI_P40_P41_P42_P43);    //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32

    SPI_SCK = 0;
    SPI_SI = 1;

    //----------------------------------------------
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //波特率, 一般 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //接收允许,   ENABLE或DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //接收超时使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //超时中断使能, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //超时时钟源选择, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //超时时间, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //初始化串口1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

    printf("命令设置:\r\n");
    printf("E 0x001234            --> 扇区擦除\xfd  十六进制地址\r\n");
    printf("W 0x001234 1234567890 --> 写入操作  十六进制地址  写入内容\r\n");
    printf("R 0x001234 10         --> 读出操作  十六进制地址  读出字节\r\n");
    printf("C                     --> 如果检测不到指定SPI Flash, 发送C强制允许操作.\r\n\r\n");

    //----------------------------------------------
    SPI_InitStructure.SPI_Enable    = ENABLE;               //SPI启动    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = ENABLE;               //片选位     ENABLE(忽略SS引脚功能), DISABLE(SS确定主机从机)
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;              //移位方向   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;      //主从选择   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;        //时钟相位   SPI_CPOL_High,   SPI_CPOL_Low
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;       //数据边沿   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_4;          //SPI速度    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(DISABLE,Priority_0);      //中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3
    SPI_ClearFlag();   //清除 SPIF和WCOL 标志

    HSPllClkConfig(MCKSEL_HIRC,PLL_96M,4);  //系统时钟选择,PLL时钟选择,时钟分频系数
    HSSPI_Enable();

    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  printf("未检测到PM25LV040/W25X40CL/W25Q80BV/W25Q128FV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            printf("检测到PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            printf("检测到W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            printf("检测到W25Q80BV!\r\n");
        }
        else if(B_FlashOK == 4)
        {
            printf("检测到W25Q128FV!\r\n");
        }
    }
    printf("制造商ID1 = 0x%02X",FLASH_ID1);
    printf("\r\n      ID2 = 0x%02X",FLASH_ID2);
    printf("\r\n   设备ID = 0x%02X\r\n",FLASH_ID);
}

//========================================================================
// 函数: Sample_HSSPI
// 描述: 用户应用程序.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2021-05-27
//========================================================================
void Sample_HSSPI(void)
{
    if(COM1.RX_TimeOut)
    {
        COM1.RX_TimeOut = 0;

        if(COM1.RX_Cnt > 0)
        {
            RX1_Check();    //处理命令数据
        }
        COM1.RX_Cnt = 0;
    }
}
