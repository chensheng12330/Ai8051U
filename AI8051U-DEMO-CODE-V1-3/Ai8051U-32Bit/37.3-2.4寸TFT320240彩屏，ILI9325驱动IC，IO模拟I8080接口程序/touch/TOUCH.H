#ifndef __TOUCH_H
#define __TOUCH_H	
	
/* touch panel interface define */
#define  CMD_RDX  0xD0	 //触摸IC读坐标积存器
#define  CMD_RDY  0x90

//IO连接
sfr   P5    = 0xC8;
sbit DCLK   =    P3^2;   
sbit TCS    =    P5^2;
sbit DIN    =    P3^4;  //MOSI
sbit DOUT   =    P3^3;  //MISO
sbit Penirq =    P5^0;  //检测触摸屏响应信号

struct tp_pix_
{
	u16 x;
	u16 y;
};
struct tp_pixu32_
{
	u32 x;
	u32 y;
};
extern struct tp_pix_  tp_pixad,tp_pixlcd;	 //当前触控坐标的AD值,前触控坐标的像素值   
extern u16 vx,vy;  //比例因子，此值除以1000之后表示多少个AD值代表一个像素点
extern u16 chx,chy;//默认像素点坐标为0时的AD起始值
u8 tpstate(void);
void spistar(void);  
void Touch_Adjust(void);
void point(void); //绘图函数
u16 ReadFromCharFrom7843();         //SPI 读数据
void TP_Draw_Big_Point(u16 x,u16 y,u16 color);
u8 Convert_Pos(void);
#endif  
	 
	 



