/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

UART DMA + SPI DMA + LCM DMA驱动液晶屏程序

下载图片时通过串口(P3.0,P3.1)DMA接收图片数据，SPI DMA保存到Flash里。

显示时SPI DMA读取Flash图片数据，LCM DMA显示图片。

由于使用的SPI Flash一次最多只能写入256字节数据，UART接收SPI写入Flash的DMA数据长度只设置256字节。

SPI读取Flash与LCM显示DMA的数据长度设置15360字节。

发送DMA缓冲区与接收DMA缓冲区交替使用。

8bit I8080模式, P2口接数据线

LCD_RS = P4^5;         //数据/命令切换
LCD_WR = P3^6;         //写控制
LCD_RD = P3^7;         //读控制
LCD_CS = P0^5;//P5^3;  //片选
LCD_RESET = P4^7;      //复位

SPI Flash 接口：
SPI_SS = P4^0;
SPI_MOSI = P4^1;
SPI_MISO = P4^2;
SPI_SCLK = P4^3;

UART 接口：
RX = P3^0
TX = P3^1

下载时, 选择时钟 35MHz (频率定义参数在system.h修改).

******************************************/

#include "system.h"
#include "spi_flash.h"
#include "uart.h"
#include "lcm.h"
#include "stdio.h"
#include "iap_eeprom.h"

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒

#define KEY_TIMER 30        //按键持续检测时间(ms)

sbit KEY1 = P3^2;
sbit KEY2 = P3^3;

u16 Key1_cnt;
u16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key1_Short_Flag;
bit Key2_Short_Flag;

bit B_1ms;          //1ms标志
bit Mode_Flag;
bit AutoDisplayFlag;
u32 Max_addr;

u16 MSecond;

u16 count;

void GPIO_Init(void);
void Timer0_Init(void);
void KeyScan(void);
void UART_SPI_DMA_BufSwitch(void);
void LCM_SPI_DMA_BufSwitch(void);

void main(void)
{
    u8 temp[4];
    
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    GPIO_Init();
    Timer0_Init();
    
    LCM_Config();
    LCM_DMA_Config();
    
    UART1_config(1);  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
//    UART1_DMA_Config();
    EA = 1;

    SPI_init();
    SPI_DMA_Config();
    
    LCD_Init(); //LCM初始化

    EEPROM_read_n(EE_ADDRESS,temp,4);    //读出4字节
    Max_addr = ((u32)temp[0] << 24) | ((u32)temp[1] << 16) | ((u32)temp[2] << 8) | (u32)temp[3];
    if(Max_addr == 0xffffffff)
    {
        Max_addr = 0;
    }
    
    while(1)
    {
        if(Mode_Flag == 0)
        {
            if(!SpiDmaFlag && !LcmDmaFlag && (lcdIndex > 0))
            {
                LCM_SPI_DMA_BufSwitch();
                LcmDmaFlag = 1;
                DMA_LCM_CR = 0xa0;    //LCM DMA Write dat

                Flash_addr += DMA_AMT_LEN;
                SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);

    //            printf("LCD index=%u\r\n",lcdIndex);
            }
        }
        else if(Mode_Flag == 1)
        {
            if(!SpiDmaFlag && UartDmaRxFlag)
            {
                UartDmaRxFlag = 0;
                UART_SPI_DMA_BufSwitch();
                SPI_Write_Nbytes(Flash_addr,DMA_WR_LEN);
                Flash_addr += DMA_WR_LEN;
                DMA_UR1R_CR = 0xa0;            //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
            }
        }
        
        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            KeyScan();
            
            if(AutoDisplayFlag)
            {
                MSecond++;
                if(MSecond >= 1000)
                {
                    MSecond = 0;

                    Mode_Flag = 0;
                    if(Flash_addr >= Max_addr)
                    {
                        Flash_addr = 0;
                        DmaBufferSW = 0;
                    }
                    
                    lcdIndex = 10;  //10 * 15360 = 320 * 240 * 2
                    LCD_Display();
                    SPI_DMA_Config();
                    LCM_DMA_Config();

                    SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
                }
            }
            
            if(RX1_TimeOut > 0)     //超时计数
            {
                if(--RX1_TimeOut == 0)
                {
                    UART_SPI_DMA_BufSwitch();
                    SPI_Write_Nbytes(Flash_addr,(((u16)DMA_UR1R_DONEH<<8)+DMA_UR1R_DONE));
                    Flash_addr += (((u16)DMA_UR1R_DONEH<<8)+DMA_UR1R_DONE);
                    Max_addr = Flash_addr;
                    
                    //关闭接收DMA，下次接收的数据重新存放在起始地址位置，否则下次接收数据继续往后面存放。
                    DMA_UR1R_CR = 0x00;            //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO

                    temp[0] = (u8)(Max_addr >> 24);
                    temp[1] = (u8)(Max_addr >> 16);
                    temp[2] = (u8)(Max_addr >> 8);
                    temp[3] = (u8)Max_addr;
                    EEPROM_SectorErase(EE_ADDRESS); //擦除扇区
                    EEPROM_write_n(EE_ADDRESS,temp,4);
                    printf("\r\nUART1 Timeout receive end!\r\n");  //UART1发送一个字符串
                }
            }
        }
    }
}

//========================================================================
// 函数: void UART_SPI_DMA_BufSwitch(void)
// 描述: UART与SPI的DMA缓冲区切换函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-8-26
// 备注: 
//========================================================================
void UART_SPI_DMA_BufSwitch(void)
{
    if(DmaBufferSW)
    {
        DmaBufferSW = 0;
        DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer1 >> 8);
        DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer1);
        DMA_SPI_TXAH = (u8)((u16)&DmaBuffer2 >> 8);    //SPI发送数据存储地址
        DMA_SPI_TXAL = (u8)((u16)&DmaBuffer2);
    }
    else
    {
        DmaBufferSW = 1;
        DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer2 >> 8);
        DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer2);
        DMA_SPI_TXAH = (u8)((u16)&DmaBuffer1 >> 8);    //SPI发送数据存储地址
        DMA_SPI_TXAL = (u8)((u16)&DmaBuffer1);
    }
}

//========================================================================
// 函数: void LCM_SPI_DMA_BufSwitch(void)
// 描述: LCM与SPI的DMA缓冲区切换函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-8-26
// 备注: 
//========================================================================
void LCM_SPI_DMA_BufSwitch(void)
{
    if(DmaBufferSW)
    {
        DmaBufferSW = 0;
        DMA_LCM_TXAH = (u8)((u16)&DmaBuffer2 >> 8);
        DMA_LCM_TXAL = (u8)((u16)&DmaBuffer2);
        DMA_SPI_RXAH = (u8)((u16)&DmaBuffer1 >> 8);    //SPI接收数据存储地址
        DMA_SPI_RXAL = (u8)((u16)&DmaBuffer1);
    }
    else
    {
        DmaBufferSW = 1;
        DMA_LCM_TXAH = (u8)((u16)&DmaBuffer1 >> 8);
        DMA_LCM_TXAL = (u8)((u16)&DmaBuffer1);
        DMA_SPI_RXAH = (u8)((u16)&DmaBuffer2 >> 8);    //SPI接收数据存储地址
        DMA_SPI_RXAL = (u8)((u16)&DmaBuffer2);
    }
}

//========================================================================
// 函数: void delay_ms(u16 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

//========================================================================
// 函数: void GPIO_Init(void)
// 描述: IO口设置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-8-24
// 备注: 
//========================================================================
void GPIO_Init(void)
{
    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口
    
    //P0.5口设置成推挽输出
    P0M0=0x20;
    P0M1=0x00;

    //P2口设置成推挽输出
    P2M0=0xff;
    P2M1=0x00;

    //P3.3,P3.2口设置成输入口
    //P3.7,P3.6口设置成推挽输出
    P3M0=0xc0;
    P3M1=0x0c;

    //P4.7,P4.5口设置成推挽输出
    P4M0=0xa0;
    P4M1=0x00;

    //P5.3,P5.2口设置成推挽输出
    P5M0=0x0c;
    P5M1=0x00;
    
    P3PU |= 0x0c;   //P3.3,P3.2口内部上拉使能
}

//========================================================================
// 函数: void Timer0_Init(void)
// 描述: 定时器0设置函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2022-8-24
// 备注: 
//========================================================================
void Timer0_Init(void)
{
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// 函数: void timer0_Interrupt(void) interrupt 1
// 描述: 定时器0中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void timer0_Interrupt(void) interrupt 1
{
    B_1ms = 1;      //1ms标志
}

//========================================================================
// 函数: void KeyScan(void)
// 描述: 按键扫描函数。
// 参数: none.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================
void KeyScan(void)
{
    if(!KEY1)
    {
        if(!Key1_Flag)
        {
            Key1_cnt++;
            if(Key1_cnt >= 1000)        //长按1s
            {
                Key1_Short_Flag = 0;    //清除短按标志
                Key1_Flag = 1;            //设置按键状态，防止重复触发

                printf("LCM auto display...\r\n");

                DMA_SPI_CR = 0x00;        //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
                DMA_SPI_STA = 0x00;
                DMA_UR1R_CR = 0x00;            //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
                DMA_UR1R_STA = 0x00;
                
                AutoDisplayFlag = 1;
                Flash_addr = 0;
                DmaBufferSW = 0;

                lcdIndex = 10;  //10 * 15360 = 320 * 240 * 2
                LCD_Display();
//                printf("Start LCD display...\r\n");
                SPI_DMA_Config();
                LCM_DMA_Config();

                SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
            }
            else if(Key1_cnt >= KEY_TIMER)    //30ms防抖
            {
                Key1_Short_Flag = 1;        //设置短按标志
            }
        }
    }
    else
    {
        if(Key1_Short_Flag)            //判断是否短按
        {
            Key1_Short_Flag = 0;    //清除短按标志

            DMA_SPI_CR = 0x00;        //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
            DMA_SPI_STA = 0x00;
            DMA_UR1R_CR = 0x00;            //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
            DMA_UR1R_STA = 0x00;
            
            Mode_Flag = 0;
            AutoDisplayFlag = 0;
            
            if(Flash_addr >= Max_addr)
            {
                Flash_addr = 0;
                DmaBufferSW = 0;
            }
            printf("Max_addr = %lu, Flash_addr = %lu\r\n",Max_addr,Flash_addr);
            
            lcdIndex = 10;  //10 * 15360 = 320 * 240 * 2
            LCD_Display();
//          printf("Start LCD display...\r\n");
            SPI_DMA_Config();
            LCM_DMA_Config();

            SPI_Read_Nbytes(Flash_addr,DMA_AMT_LEN);
            
//            delay_ms(100);
//            printf("Read Flash:\r\n");
//            for(count=0;count<DMA_AMT_LEN;count++)
//            {
//                printf("%02x ",DmaBuffer1[count]);
//            }
//            printf("\r\n");
            
        }
        Key1_cnt = 0;
        Key1_Flag = 0;
    }

    if(!KEY2)
    {
        if(!Key2_Flag)
        {
            Key2_cnt++;

            if(Key2_cnt >= 1000)        //长按1s
            {
                Key2_Short_Flag = 0;    //清除短按标志
                Key2_Flag = 1;            //设置按键状态，防止重复触发

                DMA_LCM_CR = 0x00;
                DMA_LCM_STA = 0x00;
                DMA_SPI_CR = 0x00;        //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
                DMA_SPI_STA = 0x00;
                DMA_UR1R_CR = 0x00;        //bit7 1:使能 UART1_DMA, bit5 1:开始 UART1_DMA 自动接收, bit0 1:清除 FIFO
                DMA_UR1R_STA = 0x00;
                
                Mode_Flag = 1;
                DmaBufferSW = 0;
                Flash_addr = 0;
                Max_addr = 0;
                printf("Start Flash Chip Erase...\r\n");
                FlashChipErase();
                printf("Flash Chip Erase OK!\r\n");
            }
            else if(Key2_cnt >= KEY_TIMER)    //30ms防抖
            {
                Key2_Short_Flag = 1;        //设置短按标志
            }
        }
    }
    else
    {
        if(Key2_Short_Flag)            //判断是否短按
        {
            Key2_Short_Flag = 0;    //清除短按标志

            Mode_Flag = 1;
            Flash_addr = Max_addr;  //继续添加图片
            printf("Ready to receive data...\r\n");
            UART1_DMA_Config();
            SPI_DMA_Config();
        }
        Key2_cnt = 0;
        Key2_Flag = 0;    //按键释放
    }
}