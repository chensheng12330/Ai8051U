/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP.h"

//========================================================================
//                               IO口定义    
//========================================================================

sbit P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

//========================================================================
//                               本地常量声明    
//========================================================================

u8 code t_display[]={                       //标准字库
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
// black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //位码

u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

//========================================================================
//                               本地变量声明
//========================================================================

u8  LED8[8];        //显示缓冲
u8  display_index;  //显示位索引

u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;    //行列键盘变量
u8  KeyHoldCnt; //键按下计时
u8  KeyCode;    //给用户使用的键码
u8  cnt50ms;

u8  hour,minute,second; //RTC变量
u16 msecond;

u8  Key1_cnt;
u8  Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;

//========================================================================
// 函数: APP_config
// 描述: 用户应用程序初始化.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-24
//========================================================================
void APP_config(void)
{
//    USB_Fun_init();
//    Lamp_init();
//    ADtoUART_init();
//    INTtoUART_init();
    GPIO_INTtoUART_init();
//    RTC_init();
//    I2C_24C02_init();
//    SPI_Flash_init();
//    EEPROM_init();
//    WDT_init();
//    PWMA_Output_init();
//    PWMB_Output_init();
//    DMA_AD_init();
//    DMA_M2M_init();
//    DMA_UART_init();
//    DMA_SPI_Flash_init();
//    DMA_LCM_init();
//    DMA_I2C_init();
//    USART_LIN_init();
//    USART2_LIN_init();
//    HSSPI_init();
//    HSPWM_init();
//    QSPI_Flash_init();
}

/**************** 向HC595发送一个字节函数 ******************/
void Send_595(u8 dat)
{
    u8  i;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        P_HC595_SER   = CY;
        P_HC595_SRCLK = 1;
        P_HC595_SRCLK = 0;
    }
}

//========================================================================
// 函数: DisplayScan
// 描述: 显示扫描函数.
// 参数: None.
// 返回: None.
// 版本: V1.0, 2020-09-25
//========================================================================
void DisplayScan(void)
{   
    Send_595(t_display[LED8[display_index]]);   //输出段码
    Send_595(~T_COM[display_index]);            //输出位码

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8) display_index = 0; //8位结束回0
}
