/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_SWITCH_H
#define __AI8051U_SWITCH_H

#include "config.h"

//========================================================================
//                            功能脚切换设置
//========================================================================

#define  UART1_SW(Pin)               P_SW1 = (P_SW1 & 0x3F) | (Pin << 6)
#define  CAN1_SW(Pin)                P_SW1 = (P_SW1 & 0xCF) | (Pin << 4)
#define  CAN2_SW(Pin)                P_SW3 = (P_SW3 & 0xFC) | (Pin)
#define  LIN_SW(Pin)                 P_SW1 = (P_SW1 & 0xFC) | (Pin)
#define  SPI_SW(Pin)                 P_SW1 = (P_SW1 & 0xF3) | (Pin << 2)
#define  I2C_SW(Pin)                 P_SW2 = (P_SW2 & 0xCF) | (Pin << 4)
#define  COMP_SW(Pin)                P_SW2 = (P_SW2 & 0xF7) | (Pin << 3)
#define  UART4_SW(Pin)               P_SW2 = (P_SW2 & 0xFB) | (Pin << 2)
#define  UART3_SW(Pin)               P_SW2 = (P_SW2 & 0xFD) | (Pin << 1)
#define  UART2_SW(Pin)               P_SW2 = (P_SW2 & 0xFE) | (Pin)

#define  MCLKO_SW(Pin)               MCLKOCR = (MCLKOCR & 0x7F) | (Pin << 7)
#define  T3T4SEL_SW(Pin)             T3T4PIN = (T3T4PIN & 0xFE) | (Pin)

#define  LCM_CTRL_SW(Pin)            LCMIFCFG2 = (LCMIFCFG2 & ~0x60) | (Pin << 5)
#define  LCM_DATA_SW(Pin)            LCMIFCFG = (LCMIFCFG & ~0x0C) | (Pin << 2)

#define  PWM1_SW(Pin)                PWMA_PS = (PWMA_PS & 0xFC) | (Pin)
#define  PWM2_SW(Pin)                PWMA_PS = (PWMA_PS & 0xF3) | (Pin << 2)
#define  PWM3_SW(Pin)                PWMA_PS = (PWMA_PS & 0xCF) | (Pin << 4)
#define  PWM4_SW(Pin)                PWMA_PS = (PWMA_PS & 0x3F) | (Pin << 6)

#define  PWM5_SW(Pin)                PWMB_PS = (PWMB_PS & 0xFC) | (Pin)
#define  PWM6_SW(Pin)                PWMB_PS = (PWMB_PS & 0xF3) | (Pin << 2)
#define  PWM7_SW(Pin)                PWMB_PS = (PWMB_PS & 0xCF) | (Pin << 4)
#define  PWM8_SW(Pin)                PWMB_PS = (PWMB_PS & 0x3F) | (Pin << 6)

#define  PWMA_ETR_SW(Pin)            PWMA_ETRPS = (PWMA_ETRPS & 0xFC) | (Pin)
#define  PWMB_ETR_SW(Pin)            PWMB_ETRPS = (PWMB_ETRPS & 0xFC) | (Pin)

#define  PWMA_BRK_SW(Pin)            PWMA_ETRPS = (PWMA_ETRPS & 0xFB) | (Pin << 2)
#define  PWMB_BRK_SW(Pin)            PWMB_ETRPS = (PWMB_ETRPS & 0xFB) | (Pin << 2)

#define  QSPI_SW(Pin)                P_SW4 = (P_SW4 & 0xFC) | (Pin)

//========================================================================
//                              定义声明
//========================================================================

#define    LCM_CTRL_P45_P44_P42       0
#define    LCM_CTRL_P45_P37_P36       1
#define    LCM_CTRL_P40_P44_P42       2
#define    LCM_CTRL_P40_P37_P36       3

#define    LCM_D8_NA_P2               0   //0/2

#define    LCM_D16_P2_P0              0
#define    LCM_D16_P2_P0P4            2

#define    UART1_SW_P30_P31           0
#define    UART1_SW_P36_P37           1
#define    UART1_SW_P16_P17           2
#define    UART1_SW_P43_P44           3

#define    UART2_SW_P12_P13           0
#define    UART2_SW_P42_P43           1
#define    UART3_SW_P00_P01           0
#define    UART3_SW_P50_P51           1
#define    UART4_SW_P02_P03           0
#define    UART4_SW_P52_P53           1

#define    I2C_P24_P23                0
#define    I2C_P15_P14                1
#define    I2C_P32_P33                3

#define    CMP_OUT_P45                0
#define    CMP_OUT_P41                1

#define    CAN1_P00_P01               0
#define    CAN1_P50_P51               1
#define    CAN1_P42_P45               2
#define    CAN1_P70_P71               3

#define    CAN2_P02_P03               0
#define    CAN2_P52_P53               1
#define    CAN2_P46_P47               2
#define    CAN2_P72_P73               3

#define    SPI_P14_P15_P16_P17        0
#define    SPI_P24_P25_P26_P27        1
#define    SPI_P40_P41_P42_P43        2
#define    SPI_P35_P34_P33_P32        3

#define    I2S_P32_P33_P34_P35        0
#define    I2S_P17_P16_P15_P14        1
#define    I2S_P23_P22_P21_P20        2
#define    I2S_P43_P16_P41_P40        3

#define    T3T4_P04_P05_P06_P07       0
#define    T3T4_P00_P01_P02_P03       1

#define    MCLKO_SW_P47               0
#define    MCLKO_SW_P56               1

#define    LIN_P02_P03                0
#define    LIN_P52_P53                1
#define    LIN_P46_P47                2
#define    LIN_P72_P73                3

#define    PWM1_SW_P10_P11            0
#define    PWM1_SW_P00_P01            1
#define    PWM1_SW_P20_P21            2

#define    PWM2_SW_P12_P13            0
#define    PWM2_SW_P02_P03            1
#define    PWM2_SW_P22_P23            2

#define    PWM3_SW_P14_P15            0
#define    PWM3_SW_P04_P05            1
#define    PWM3_SW_P24_P25            2

#define    PWM4_SW_P16_P17            0
#define    PWM4_SW_P06_P07            1
#define    PWM4_SW_P26_P27            2

#define    QSPI_P14_P15_P16_P13_P12_P17    0
#define    QSPI_P40_P41_P42_P52_P53_P43    1
#define    QSPI_P47_P25_P26_P46_P45_P27    2

#define    PCA_P12_P13_P14_P11        0
#define    PCA_P41_P42_P43_P44        1
#define    PCA_P23_P20_P21_P22        2

#define    PWM5_SW_P00                0
#define    PWM5_SW_P11                1
#define    PWM5_SW_P21                2
#define    PWM5_SW_P50                3

#define    PWM6_SW_P03                0
#define    PWM6_SW_P13                1
#define    PWM6_SW_P23                2
#define    PWM6_SW_P51                3

#define    PWM7_SW_P05                0
#define    PWM7_SW_P15                1
#define    PWM7_SW_P25                2
#define    PWM7_SW_P52                3

#define    PWM8_SW_P07                0
#define    PWM8_SW_P17                1
#define    PWM8_SW_P27                2
#define    PWM8_SW_P53                3

#define    PWMA_ETR_P32               0
#define    PWMA_ETR_P41               1
#define    PWMA_ETR_P23               2
#define    PWMA_ETR_P12               3

#define    PWMB_ETR_P32               0
#define    PWMB_ETR_P41               1
#define    PWMB_ETR_P23               2
#define    PWMB_ETR_P12               3

#define    PWMA_BRK_P35               0
#define    PWMA_BRK_CMP               1
#define    PWMA_BRK_P06               2

#define    PWMB_BRK_P35               0
#define    PWMB_BRK_CMP               1
#define    PWMB_BRK_P06               2

#define    ADC_ETR_P47                0
#define    ADC_ETR_P40                1
#define    ADC_ETR_P20                2

#endif
