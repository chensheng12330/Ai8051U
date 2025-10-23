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

//void LCD_WR_REG16(u8 Reg, u16 dat)     
//{    
//    LCD_WR_REG(Reg);
//    LCD_WR_DATA((u8)(dat>>8));
//    LCD_WR_DATA((u8)dat);
//} 

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u8 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u8 LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/    
void LCD_WriteRAM_Prepare(void)
{
     LCD_WR_REG((u8)lcddev.wramcmd);      
}

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
    DMA_LCM_TXAH = (u8)((u16)&DmaBuffer1 >> 8);
    DMA_LCM_TXAL = (u8)((u16)&DmaBuffer1);
//    DMA_LCM_RXAH = (u8)((u16)&Buffer >> 8);
//    DMA_LCM_RXAL = (u8)((u16)&Buffer);
    DMA_LCM_STA = 0x00;
    DMA_LCM_CFG = 0x82;
    DMA_LCM_CR = 0x80;
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

//*************2.4inch ILI9341初始化**********//
    LCD_WR_REG(0xCF);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0xD9); //0xC1 
    LCD_WR_DATA(0X30); 
    LCD_WR_REG(0xED);  
    LCD_WR_DATA(0x64); 
    LCD_WR_DATA(0x03); 
    LCD_WR_DATA(0X12); 
    LCD_WR_DATA(0X81); 
    LCD_WR_REG(0xE8);  
    LCD_WR_DATA(0x85); 
    LCD_WR_DATA(0x10); 
    LCD_WR_DATA(0x7A); 
    LCD_WR_REG(0xCB);  
    LCD_WR_DATA(0x39); 
    LCD_WR_DATA(0x2C); 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x34); 
    LCD_WR_DATA(0x02); 
    LCD_WR_REG(0xF7);  
    LCD_WR_DATA(0x20); 
    LCD_WR_REG(0xEA);  
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0xC0);    //Power control 
    LCD_WR_DATA(0x1B);   //VRH[5:0] 
    LCD_WR_REG(0xC1);    //Power control 
    LCD_WR_DATA(0x12);   //SAP[2:0];BT[3:0] 0x01
    LCD_WR_REG(0xC5);    //VCM control 
    LCD_WR_DATA(0x08);      //30
    LCD_WR_DATA(0x26);      //30
    LCD_WR_REG(0xC7);    //VCM control2 
    LCD_WR_DATA(0XB7); 
    LCD_WR_REG(0x36);    // Memory Access Control 
    LCD_WR_DATA(0x08);
    LCD_WR_REG(0x3A);   
    LCD_WR_DATA(0x55); 
    LCD_WR_REG(0xB1);   
    LCD_WR_DATA(0x00);   
    LCD_WR_DATA(0x1A); 
    LCD_WR_REG(0xB6);    // Display Function Control 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0xA2); 
    LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
    LCD_WR_DATA(0x00); 
    LCD_WR_REG(0x26);    //Gamma curve selected 
    LCD_WR_DATA(0x01); 
    LCD_WR_REG(0xE0);    //Set Gamma 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x1D); 
    LCD_WR_DATA(0x1A); 
    LCD_WR_DATA(0x0A); 
    LCD_WR_DATA(0x0D); 
    LCD_WR_DATA(0x07); 
    LCD_WR_DATA(0x49); 
    LCD_WR_DATA(0X66); 
    LCD_WR_DATA(0x3B); 
    LCD_WR_DATA(0x07); 
    LCD_WR_DATA(0x11); 
    LCD_WR_DATA(0x01); 
    LCD_WR_DATA(0x09); 
    LCD_WR_DATA(0x05); 
    LCD_WR_DATA(0x04);          
    LCD_WR_REG(0XE1);    //Set Gamma 
    LCD_WR_DATA(0x00); 
    LCD_WR_DATA(0x18); 
    LCD_WR_DATA(0x1D); 
    LCD_WR_DATA(0x02); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x04); 
    LCD_WR_DATA(0x36); 
    LCD_WR_DATA(0x13); 
    LCD_WR_DATA(0x4C); 
    LCD_WR_DATA(0x07); 
    LCD_WR_DATA(0x13); 
    LCD_WR_DATA(0x0F); 
    LCD_WR_DATA(0x2E); 
    LCD_WR_DATA(0x2F); 
    LCD_WR_DATA(0x05); 
    LCD_WR_REG(0x2B); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A); 
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);     
    LCD_WR_REG(0x11); //Exit Sleep
    delay_ms(120);
    LCD_WR_REG(0x29); //display on    

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
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{    
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA((u8)(xStar>>8));
    LCD_WR_DATA(0x00FF&xStar);
    LCD_WR_DATA((u8)(xEnd>>8));
    LCD_WR_DATA(0x00FF&xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA((u8)(yStar>>8));
    LCD_WR_DATA(0x00FF&yStar);
    LCD_WR_DATA((u8)(yEnd>>8));
    LCD_WR_DATA(0x00FF&yEnd);    

    LCD_WriteRAM_Prepare();    //开始写入GRAM
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
    lcddev.setxcmd=0x2A;
    lcddev.setycmd=0x2B;
    lcddev.wramcmd=0x2C;
    lcddev.rramcmd=0x2E;
    switch(direction){
        case 0:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;
            LCD_WriteReg(0x36,(1<<3));
        break;
        case 1:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(1<<5)|(1<<6));
        break;
        case 2:
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;    
            LCD_WriteReg(0x36,(1<<3)|(1<<4)|(1<<6)|(1<<7));
        break;
        case 3:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5)|(1<<4));
        break;
        default:break;
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

        lcdIndex--;
        if(lcdIndex == 0)
        {
            DMA_LCM_CR = 0;
            LCD_CS=1;
        }

        DMA_LCM_STA = 0;
    }
}

#if 0
/*****************************************************************************
 * @name       :void LCD_WR_DATA_16Bit(u16 dat)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/     
void LCD_WR_DATA_16Bit(u16 dat)
{
    LCD_WR_DATA((u8)(dat>>8));
    LCD_WR_DATA((u8)dat);
}

void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{      
    u16 i,j;            
    u16 width=ex-sx+1;         //得到填充的宽度
    u16 height=ey-sy+1;        //高度
    LCD_SetWindows(sx,sy,ex,ey);//设置显示窗口

    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        LCD_WR_DATA_16Bit(color);    //写入数据      
    }
}

void Test_Color(void)
{
    u8 buf[10] = {0};

    LCD_Fill(0,0,lcddev.width,lcddev.height,WHITE);delay_ms(800);
    LCD_Fill(0,0,lcddev.width,lcddev.height,RED);delay_ms(800);
    LCD_Fill(0,0,lcddev.width,lcddev.height,GREEN);delay_ms(800);
    LCD_Fill(0,0,lcddev.width,lcddev.height,BLUE);delay_ms(800);
}
#endif
