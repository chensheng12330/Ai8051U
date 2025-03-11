/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过硬件QSPI接口1线、2线、4线模式对支持QSPI协议的Flash进行读写测试。

串口(P3.0,P3.1)打印访问结果，默认设置: 115200,8,N,1.

下载时, 选择时钟 40MHz (用户可自行修改频率).

******************************************/

#include <stdio.h>
#include <intrins.h>
#include "qspi.h"
#include "w25qxx.h"

/*****************************************************************************/
//#define FOSC                11059200UL
//#define FOSC                24000000UL
#define FOSC                40000000UL

#define BAUD                (65536 - FOSC/4/115200)
#define SIZE                30

/*****************************************************************************/
#define INIT_BUF()      for (i=0; i<SIZE; i++) buf[i] = 0;
#define SET_BUF()       for (i=0; i<SIZE; i++) buf[i] = (BYTE)(i + 0x00);
#define PRINT_BUF()     for (i=0; i<SIZE; i++)                              \
                        {                                                   \
                            printf("%02bx ", buf[i]);                       \
                            if ((i % 32) == 31)                             \
                                printf("\n                            ");   \
                        }                                                   \
                        printf("\n");

/*****************************************************************************/
BYTE xdata buf[1024];

/*****************************************************************************/
void delay(int n)
{
    int i;

    while (n--)
    for (i = 0; i < 1000; i++)
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

/*****************************************************************************/
void main()
{
    int i;
    int j;
    BYTE d1, d2;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    i = j = d1 = d2 = 0;

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x00; P5M1 = 0x00;

    P4M0 |= 0x09;               //设置CS/P4.0,SCK/P4.3为强推挽模式
    P4M1 &= ~0x09;
    P4SR &= ~0x0f;              //设置所有的QSPI口为快速模式
    P5SR &= ~0x0c;
    P4PU |= 0x0f;               //使能所有的QSPI口的内部10K上拉电阻
    P5PU |= 0x0c;
    P4BP &= ~0x06;              //使能QSPI的IO0~IO3数据硬件自动设置端口模式
    P5BP &= ~0x0c;

    SCON = 0x52;
    T2L = BAUD;
    T2H = BAUD >> 8;
    AUXR = 0x15;

    delay(1000);

    printf("AI8051U Test !\n");

    QSPI_Init();
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
    W25Q_Erase4K_20(0);
    
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

    while (1);
}
