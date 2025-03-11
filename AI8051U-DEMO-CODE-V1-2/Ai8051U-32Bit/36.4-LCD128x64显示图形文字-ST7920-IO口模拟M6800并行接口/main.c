/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  功能说明    **************

本例程基于AI8051U为主控芯片的实验箱9.6版本进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

128*64的LCD显示程序

显示图形，汉字，英文，数字

下载时, 选择时钟 12MHz (用户可自行修改频率).

******************************************/

#include "../comm/AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        12000000UL

/****************************** 用户定义宏 ***********************************/
sbit    LCD_RS = P4^5;  //定义引脚
sbit    LCD_RW = P3^6;  //9.5版本之前实验箱需要将P42与P44对调才能正常使用
sbit    LCD_E  = P3^7;
//sbit    PSB    = P3^5;      //PSB脚为12864的串、并通讯功能切换，我们使用8位并行接口，PSB=1
sbit    LCD_RESET   =   P4^7;   //  17---RESET  L-->Enable
#define LCD_Data P2

#define Busy    0x80 //用于检测LCD状态字中的Busy标识
/*****************************************************************************/

/*************  本地常量声明    **************/
u8  code uctech[] = {"深圳国芯人工智能"};
u8  code net[]    = {" www.stcai.com "};
u8  code mcu[]    = {"专业设计51单片机"};
u8  code qq[]     = {"AI8051U LQFP48 "};

    //128*64点阵图形数据
u8 code gImage_gxw[1024] = { /* 0X10,0X01,0X00,0X80,0X00,0X40, */
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x3F,0x03,0xF0,0x3F,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x7F,0xF0,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0xFF,0xF0,0x41,0x00,0x7F,0xC0,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0x08,0x10,0x41,0x00,0x40,0x40,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x01,0x08,0x10,0x41,0x00,0x40,0x40,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF9,0x1F,0x90,0x41,0x00,0x7F,0xC0,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF9,0x31,0x10,0x7F,0xF0,0x40,0x41,0xFF,0xFC,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x4A,0x10,0x40,0x00,0x40,0x40,0x02,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x04,0x10,0x40,0x00,0x7F,0xC0,0x02,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x1B,0x10,0x40,0x00,0x11,0x00,0x22,0x20,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0xE0,0xF0,0x7F,0x81,0x11,0x10,0x22,0x10,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x0C,0x10,0x40,0x80,0x91,0x10,0x42,0x08,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x02,0x10,0x40,0x80,0x51,0x20,0x82,0x04,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x18,0x10,0x40,0x80,0x51,0x41,0x02,0x04,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x06,0x10,0x80,0x80,0x11,0x00,0x0A,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0xFF,0xF0,0x80,0x83,0xFF,0xF8,0x04,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x00,0x11,0x00,0x80,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x63,0x07,0x87,0x83,0xCF,0xC1,0x8C,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x67,0x83,0x0C,0xC6,0x6C,0x03,0x8C,0xD8,0x00,0x00,0x00,0x00,0x00,0x3E,0x00,0x00,
    0x6C,0xC3,0x0C,0xC6,0xEC,0x0F,0x8C,0xD8,0x00,0x3C,0x00,0x00,0x00,0xFF,0x80,0x00,
    0x6C,0xC3,0x0E,0xC6,0xEC,0x01,0x8C,0xD8,0x00,0xFF,0x80,0x00,0x03,0xFF,0xE0,0x00,
    0x6C,0xC3,0x07,0x86,0x6F,0x81,0x8C,0xD8,0x03,0xFF,0xE0,0x00,0x07,0xFF,0xF0,0x00,
    0x6F,0xC3,0x0D,0xC7,0x60,0xC1,0x8C,0xD8,0x07,0x80,0xF0,0x00,0x0F,0xFF,0xF8,0x00,
    0x6C,0xC3,0x0C,0xC7,0x60,0xC1,0x8C,0xD8,0x0E,0x18,0x38,0x00,0x1F,0xFF,0xFC,0x00,
    0x6C,0xC3,0x0C,0xC6,0x61,0x81,0x8C,0xD8,0x1C,0x18,0x1C,0x00,0x1F,0xFF,0xFC,0x00,
    0x6C,0xC7,0x87,0x83,0xCF,0x01,0x87,0x98,0x38,0x18,0x0E,0x00,0x3F,0xFF,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x30,0x18,0x06,0x00,0x3F,0xFF,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x70,0x18,0x07,0x00,0x7C,0x3E,0x1F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x18,0x03,0x00,0x7C,0x3E,0x1F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x18,0x03,0x00,0x7E,0x7F,0x3F,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF0,0x18,0x07,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF8,0x1F,0xCF,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF8,0x1F,0xCF,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0xF0,0x00,0x07,0x80,0x7F,0xFF,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x60,0x00,0x03,0x00,0x7F,0x80,0xFF,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x70,0x00,0x07,0x00,0x3F,0x80,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x30,0x00,0x06,0x00,0x3F,0xC1,0xFE,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x38,0x00,0x0E,0x00,0x1F,0xE3,0xFC,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x1C,0x00,0x1C,0x00,0x0F,0xFF,0xF8,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x0E,0x18,0x38,0x00,0x07,0xFF,0xF0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x07,0xBC,0xF0,0x00,0x03,0xFF,0xE0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x03,0xFF,0xE0,0x00,0x01,0xFF,0xC0,0x00,
    0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x00,0xFF,0x80,0x00,0x00,0x3E,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,
    0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x21,0x02,0x10,0x21,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x3F,0x03,0xF0,0x3F,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


/*************  本地变量声明    **************/


/*************  本地函数声明    **************/
void    delay_ms(u16 ms);
void    WriteDataLCD(u8 WDLCD);
void    WriteCommandLCD(u8 WCLCD,u8 BuysC);
u8      ReadDataLCD(void);
u8      ReadStatusLCD(void);
void    LCDInit(void);
void    LCDClear(void);
void    LCDFlash(void);
void    DisplayOneChar(u8 X, u8 Y, u8 DData);
void    DisplayListChar(u8 X, u8 Y, u8 code *DData);
void    DisplayImage (u8 code *DData);

/********************* 主函数 *************************/
void main(void)
{
    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0xc0;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0xa0;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    delay_ms(100); //启动等待，等LCD讲入工作状态
    LCDInit(); //LCM初始化
    delay_ms(5); //延时片刻(可不要)

    while(1)
    {
        LCDClear();
        DisplayImage(gImage_gxw);//显示图形
        delay_ms(3000);
        LCDClear();
        DisplayListChar(0,1,uctech);    //显示字库中的中文数字
        DisplayListChar(0,2,net);       //显示字库中的中文数字
        DisplayListChar(0,3,mcu);       //显示字库中的中文
        DisplayListChar(0,4,qq);        //显示字库中的中文数字
        delay_ms(3000);
    }
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
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

//******************************************
void LCD_delay(void)
{
    NOP(30);    //电压降低，IO口翻转速度变慢，驱动信号需要增加延时时间
    NOP(30);
}

//写数据
void WriteDataLCD(u8 WDLCD)
{
    ReadStatusLCD(); //检测忙 
    LCD_RS = 1;
    LCD_delay();
    LCD_RW = 0;
    LCD_Data = WDLCD;
    LCD_delay();
    LCD_E = 1;
    LCD_delay();
    LCD_E = 0;
}

//写指令
void WriteCommandLCD(u8 WCLCD,u8 BuysC) //BuysC为0时忽略忙检测
{
    if (BuysC) ReadStatusLCD(); //根据需要检测忙 
    LCD_RS = 0;
    LCD_delay();
    LCD_RW = 0; 
    LCD_Data = WCLCD;
    LCD_delay();
    LCD_E = 1; 
    LCD_delay();
    LCD_E = 0;  
}

//读状态
u8 ReadStatusLCD(void)
{
    LCD_Data = 0xFF; 

    LCD_RS = 0;
    LCD_delay();
    LCD_RW = 1; 
    LCD_delay();
    LCD_E = 1;
    LCD_delay();
    while (LCD_Data & Busy); //检测忙信号
    LCD_E = 0;

    return(LCD_Data);
}

void LCDInit(void) //LCM初始化
{
//  PSB = 1;    //并口
//  PSB = 0;    //SPI口
    delay_ms(10);
    LCD_RESET = 0;
    delay_ms(10);
    LCD_RESET = 1;
    delay_ms(100);
    
    WriteCommandLCD(0x30,1); //显示模式设置,开始要求每次检测忙信号
    WriteCommandLCD(0x01,1); //显示清屏
    WriteCommandLCD(0x06,1); // 显示光标移动设置
    WriteCommandLCD(0x0C,1); // 显示开及光标设置
}

void LCDClear(void) //清屏
{
    WriteCommandLCD(0x01,1); //显示清屏
    WriteCommandLCD(0x34,1); // 显示光标移动设置
    WriteCommandLCD(0x30,1); // 显示开及光标设置
}


//按指定位置显示一串字符
void DisplayListChar(u8 X, u8 Y, u8 code *DData)
{
    u8 ListLength,X2;
    ListLength = 0;
    X2 = X;
    if(Y < 1)   Y=1;
    if(Y > 4)   Y=4;
    X &= 0x0F; //限制X不能大于16，Y在1-4之内
    switch(Y)
    {
        case 1: X2 |= 0X80; break;  //根据行数来选择相应地址
        case 2: X2 |= 0X90; break;
        case 3: X2 |= 0X88; break;
        case 4: X2 |= 0X98; break;
    }
    WriteCommandLCD(X2, 1); //发送地址码
    while (DData[ListLength] >= 0x20) //若到达字串尾则退出
    {
        if (X <= 0x0F) //X坐标应小于0xF
        {
            WriteDataLCD(DData[ListLength]); //
            ListLength++;
            X++;
        }
    }
}

//图形显示122*32
void DisplayImage (u8 code *DData)
{
    u8 x,y,i;
    unsigned int tmp=0;
    for(i=0;i<9;)       //分两屏，上半屏和下半屏，因为起始地址不同，需要分开
    {
        for(x=0;x<32;x++)   //32行
        {
            WriteCommandLCD(0x34,1);
            WriteCommandLCD((u8)(0x80+x),1);//列地址
            WriteCommandLCD((u8)(0x80+i),1);    //行地址，下半屏，即第三行地址0X88
            WriteCommandLCD(0x30,1);        
            for(y=0;y<16;y++)   
                WriteDataLCD(DData[tmp+y]);//读取数据写入LCD
            tmp+=16;        
        }
        i+=8;
    }
    WriteCommandLCD(0x36,1);    //扩充功能设定
    WriteCommandLCD(0x30,1);
}

