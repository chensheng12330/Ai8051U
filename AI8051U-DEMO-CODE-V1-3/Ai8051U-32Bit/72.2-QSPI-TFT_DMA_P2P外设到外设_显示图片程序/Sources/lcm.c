/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "system.h"
#include "stdio.h"
#include "intrins.h"
#include "lcm.h"
#include "spi_flash.h"

bit LcmDmaFlag;
bit LcmFlag;

typedef struct  
{                                            
    u16 width;      //LCD 宽度
    u16 height;     //LCD 高度
    u16 id;         //LCD ID
    u8  dir;        //横屏还是竖屏控制：0，竖屏；1，横屏。    
    u8 wramcmd;     //开始写gram指令
    u8 rramcmd;     //开始读gram指令
    u8 setxcmd;     //设置x坐标指令
    u8 setycmd;     //设置y坐标指令     
}_lcd_dev;     

_lcd_dev lcddev;

void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_direction(u8 direction);


void LCD_Display(void)
{
	LCD_SetWindows(0,0,lcddev.width,lcddev.height);//设置显示窗口
    LCD_CS=0;
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 Reg)    
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 Reg)     
{
    LCMIFDATL = Reg;
    LCD_CS=0;
    LCMIFCR = 0x84;        //Enable interface, write command out
    while(!LCMIFSTA);
    LCMIFSTA = 0x00;
    LCD_CS=1;
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 Data)
{
    LCMIFDATL = Data;
    LCD_CS=0;
    LCMIFCR = 0x85;        //Enable interface, write data out
    while(!LCMIFSTA);
    LCMIFSTA = 0x00;
    LCD_CS=1;
}

void LCD_WR_REG16(u8 Reg, u16 dat)     
{    
    LCD_WR_REG(Reg);
    LCD_WR_DATA((u8)(dat>>8));
    LCD_WR_DATA((u8)dat);
} 

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/    
//void LCD_WriteRAM_Prepare(void)
//{
//     LCD_WR_REG((u8)lcddev.wramcmd);      
//}

/*****************************************************************************
 * @name       :void LCM_Config(void)
 * @date       :2018-11-13 
 * @function   :Config LCM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/    
void LCM_Config(void)
{
    LCMIFCFG = 0x00;    //bit7 1:Enable Interrupt, bit1 0:8bit mode; bit0 0:8080,1:6800
    LCMIFCFG2 = 0x24;    //RS:P45,E:P37,RW:P36; Setup Time=1,HOLD Time=0
    LCMIFSTA = 0x00;
    LCMIFCR = 0x80;         //{ ENLCMIF, -, -, -, -, CMD[2:0] }
}

/*****************************************************************************
 * @name       :void LCM_DMA_Config(void)
 * @date       :2020-12-09 
 * @function   :Config DMA
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCM_DMA_Config(void)
{
    DMA_LCM_CR = 0x00;
    DMA_LCM_AMT = (u8)(DMA_AMT_LEN-1);            //设置传输总字节数(低8位)：n+1
    DMA_LCM_AMTH = (u8)((DMA_AMT_LEN-1)>>8);    //设置传输总字节数(高8位)：n+1
    DMA_LCM_STA = 0x00;
    DMA_LCM_CFG = 0x02;
    DMA_LCM_CR = 0x80;
    
//    DMA_LCM_CR = 0xa0;    //LCM DMA Write dat    
}

/*****************************************************************************
 * @name       :void LCDReset(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/    
void LCDReset(void)
{
    LCD_CS=1;
    delay_ms(50);    
    LCD_RESET=0;
    delay_ms(150);
    LCD_RESET=1;
    delay_ms(50);
}

/*****************************************************************************
 * @name       :void LCD_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/          
void LCD_Init(void)
{
    LCDReset(); //初始化之前复位
//    delay_ms(150);                     //根据不同晶振速度可以调整延时，保障稳定显示

//*************2.4inch ILI9325???**********//    
    LCD_WR_REG16(0x00E5,0x78F0); 
    LCD_WR_REG16(0x0001,0x0100); 
    LCD_WR_REG16(0x0002,0x0700); 
    LCD_WR_REG16(0x0003,0x1030); 
    LCD_WR_REG16(0x0004,0x0000); 
    LCD_WR_REG16(0x0008,0x0202);  
    LCD_WR_REG16(0x0009,0x0000);
    LCD_WR_REG16(0x000A,0x0000); 
    LCD_WR_REG16(0x000C,0x0000); 
    LCD_WR_REG16(0x000D,0x0000);
    LCD_WR_REG16(0x000F,0x0000);
    //power on sequence VGHVGL
    LCD_WR_REG16(0x0010,0x0000);   
    LCD_WR_REG16(0x0011,0x0007);  
    LCD_WR_REG16(0x0012,0x0000);  
    LCD_WR_REG16(0x0013,0x0000); 
    LCD_WR_REG16(0x0007,0x0000); 
    //vgh 
    LCD_WR_REG16(0x0010,0x1690);   
    LCD_WR_REG16(0x0011,0x0227);
    delay_ms(10);
    //vregiout 
    LCD_WR_REG16(0x0012,0x009D); //0x001b
    delay_ms(10); 
    //vom amplitude
    LCD_WR_REG16(0x0013,0x1900);
    delay_ms(10); 
    //vom H
    LCD_WR_REG16(0x0029,0x0025); 
    LCD_WR_REG16(0x002B,0x000D); 
    //gamma
    LCD_WR_REG16(0x0030,0x0007);
    LCD_WR_REG16(0x0031,0x0303);
    LCD_WR_REG16(0x0032,0x0003); //0006
    LCD_WR_REG16(0x0035,0x0206);
    LCD_WR_REG16(0x0036,0x0008);
    LCD_WR_REG16(0x0037,0x0406); 
    LCD_WR_REG16(0x0038,0x0304); //0200
    LCD_WR_REG16(0x0039,0x0007); 
    LCD_WR_REG16(0x003C,0x0602); //0504
    LCD_WR_REG16(0x003D,0x0008); 
    //ram
    LCD_WR_REG16(0x0050,0x0000); 
    LCD_WR_REG16(0x0051,0x00EF);
    LCD_WR_REG16(0x0052,0x0000); 
    LCD_WR_REG16(0x0053,0x013F);  
    LCD_WR_REG16(0x0060,0xA700); 
    LCD_WR_REG16(0x0061,0x0001); 
    LCD_WR_REG16(0x006A,0x0000); 
    //
    LCD_WR_REG16(0x0080,0x0000); 
    LCD_WR_REG16(0x0081,0x0000); 
    LCD_WR_REG16(0x0082,0x0000); 
    LCD_WR_REG16(0x0083,0x0000); 
    LCD_WR_REG16(0x0084,0x0000); 
    LCD_WR_REG16(0x0085,0x0000); 
    //
    LCD_WR_REG16(0x0090,0x0010); 
    LCD_WR_REG16(0x0092,0x0600); 
    
    LCD_WR_REG16(0x0007,0x0133);
    LCD_WR_REG16(0x0000,0x0022);

    //设置LCD属性参数
    LCD_direction(USE_HORIZONTAL);//设置LCD显示方向 
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
                                yStar:the bebinning y coordinate of the LCD display window
                                xEnd:the endning x coordinate of the LCD display window
                                yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar0, u16 yStar0,u16 xEnd0,u16 yEnd0)
{    
    u16 xStar; u16 yStar;u16 xEnd;u16 yEnd;
    #if USE_HORIZONTAL==1
    xStar = yStar0;
    xEnd = yEnd0;
    yStar = xStar0;
    yEnd = xEnd0;
    #else
    xStar = xStar0;
    xEnd = xEnd0;
    yStar = yStar0;
    yEnd = yEnd0;
    #endif 

    LCD_WR_REG16(WINDOW_XADDR_START,xStar);
    LCD_WR_REG16(WINDOW_XADDR_END,xEnd);
    LCD_WR_REG16(WINDOW_YADDR_START,yStar);
    LCD_WR_REG16(WINDOW_YADDR_END,yEnd);
    LCD_WR_REG16(GRAM_XADDR,xStar);
    LCD_WR_REG16(GRAM_YADDR,yStar);
    LCD_WR_REG(GRAMWR);
}   

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
                                                    2-180 degree
                                                    3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{
    lcddev.setxcmd=0x50;
    lcddev.setycmd=0x52;
    lcddev.wramcmd=0x22;
    lcddev.rramcmd=0x22;
    switch(direction){          
        case 0:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;
            LCD_WR_REG16(0x0003, 0x1030); // set GRAM write direction and BGR=1.
            LCD_WR_REG16(0x0001, 0x0100); // set SS and SM bit
        break;
        case 1:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;    
            LCD_WR_REG16(0x0003, 0x1038); // set GRAM write direction and BGR=1.
            LCD_WR_REG16(0x0001, 0x0000); // set SS and SM bit
        break;
        default:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;
            LCD_WR_REG16(0x0003, 0x1030); // set GRAM write direction and BGR=1.
            LCD_WR_REG16(0x0001, 0x0100); // set SS and SM bit
        break;
    }
}

/*****************************************************************************
 * @name       :void LCM_Interrupt(void)
 * @date       :2018-11-13 
 * @function   :None
 * @parameters :None
 * @retvalue   :
******************************************************************************/ 
void LCMIF_DMA_Interrupt(void) interrupt 13
{
//    if(LCMIFSTA & 0x01)
//    {
//        LCMIFSTA = 0x00;
//        LcmFlag = 0;
//    }
    
    if(DMA_LCM_STA & 0x01)
    {
        LcmDmaFlag = 0;
        DMA_LCM_STA = 0;
    }
}
