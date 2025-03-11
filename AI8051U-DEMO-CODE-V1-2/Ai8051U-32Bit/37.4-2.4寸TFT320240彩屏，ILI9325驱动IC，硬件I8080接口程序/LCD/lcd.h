#ifndef __LCD_H
#define __LCD_H		
#include "sys.h"

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换
#define USE_HORIZONTAL  	  1   //定义液晶屏显示方向 	0-竖屏，1-横屏
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////	  
//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 320

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END		0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

//IO连接
#define  LCD_DataPort P2    //8位数据口
sbit LCD_RS = P4^5;         //数据/命令切换
sbit LCD_WR = P3^6;         //写控制
sbit LCD_RD = P3^7;         //读控制
//sbit LCD_CS = P5^3;         //片选
sbit LCD_CS = P0^5;         //片选
sbit LCD_RESET = P4^7;      //复位

//LCD的画笔颜色和背景色
extern u16  POINT_COLOR;//默认红色
extern u16  BACK_COLOR; //背景颜色.默认为白色
//LCD重要参数集
typedef struct
{
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;					//LCD ID
	u8  dir;				//横屏还是竖屏控制：0，竖屏；1，横屏。	
	u16	 wramcmd;		//开始写gram指令
	u16  rramcmd;   //开始读gram指令
	u16  setxcmd;		//设置x坐标指令
	u16  setycmd;		//设置y坐标指令
}_lcd_dev;

//LCD参数
extern _lcd_dev lcddev;	//管理LCD重要参数
void LCD_Init(void); 
void LCD_Clear(u16 Color);
void LCD_write(u8 VAL);
u16 LCD_read(void);
void LCD_WR_DATA(u8 Data);
u16 LCD_RD_DATA(void);
//void LCD_WR_REG(u8 Reg);
void LCD_SetCursor(u16 Xpos, u16 Ypos);//设置光标位置
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);//设置显示窗口
void LCD_DrawPoint(u16 x,u16 y);//画点
u16 LCD_ReadPoint(u16 x,u16 y);
void LCD_direction(u8 direction );
void LCD_WR_DATA_16Bit(u16 Data);
u16 Lcd_RD_DATA_16Bit(void);
//void LCD_WriteReg(u8 LCD_Reg, u8 LCD_RegValue);
void LCD_ReadReg(u8 LCD_Reg,u8 *Rval,int n);
u16 LCD_Read_ID(void);
u16 Color_To_565(u8 r, u8 g, u8 b);
void LCM_Config(void);

//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE             0x001F  
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN            0XBC40 //棕色
#define BRRED            0XFC07 //棕红色
#define GRAY             0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY            0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


					  		 
#endif  
	 
	 



