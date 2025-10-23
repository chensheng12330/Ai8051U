/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

LCM�ӿ�+DMA����Һ��������

8bit I8080ģʽ, P2�ڽ�D8~D15

sbit LCD_RS = P4^5;         //����/�����л�
sbit LCD_WR = P3^6;         //д����
sbit LCD_RD = P3^7;         //������
sbit LCD_CS = P0^5;         //Ƭѡ
sbit LCD_RESET = P4^7;      //��λ

LCMָ��ͨ���жϷ�ʽ�ȴ��������

DMA���ó���256�ֽڣ�ͨ���жϷ�ʽ�жϴ������

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include    "../../comm/AI8051U.h"
#include    "intrins.h"
#include    <stdio.h>
#include    "font.h"

#define     MAIN_Fosc       24000000L   //������ʱ��

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

sbit LCD_RS = P4^5;         //����/�����л�
sbit LCD_WR = P3^6;         //д����
sbit LCD_RD = P3^7;         //������
sbit LCD_CS = P0^5;         //Ƭѡ
sbit LCD_RESET = P4^7;      //��λ

//IO����
#define  LCD_DataPort P2    //8λ���ݿ�

#define  USR_LCM_IF     1   //1: use LCM Interface; 0: use IO mode

//֧�ֺ��������ٶ����л�
#define USE_HORIZONTAL  0   //����Һ������ʾ���� 	0-������1-����

#define LCMIFCR_OFFSET  0x90    //bit7:Enable interface, bit4:Write xram endian���޸�16λ���ݴ洢��XRAM��˳��


//������ɫ
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
#define BROWN            0XBC40 //��ɫ
#define BRRED            0XFC07 //�غ�ɫ
#define GRAY             0X8430 //��ɫ

u16 LCD_W;			//LCD ���
u16 LCD_H;			//LCD �߶�

u16 POINT_COLOR=0x0000;	//������ɫ

u16 index;
u16 xdata Buffer[8]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
u16 xdata Color[256];
bit DmaFlag;
bit LcmFlag;

void delay_ms(u16 ms);
void GPIO_Init(void);
void LCM_Config(void);
void DMA_Config(void);
void LCD_Init(void);
void Test_Color(void);
void LCD_WR_DATA_16Bit(u16 Data);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
u16 LCD_Read_ID(void);
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode);

void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���

	GPIO_Init();
#if USR_LCM_IF == 1	
	LCM_Config();
	DMA_Config();
	EA = 1;
#endif
	
	LCD_Init(); //LCM��ʼ��

	while(1)
	{
		Test_Color();
	}
}

void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)
{  	
	u16 i,j;			
	u16 width=ex-sx+1; 		//�õ����Ŀ��
	u16 height=ey-sy+1;		//�߶�
	LCD_SetWindows(sx,sy,ex,ey);//������ʾ����

#if USR_LCM_IF == 1
	
	for(j=0,i=0;i<256;i++)
	{
		Color[i] = color;
	}
	index = 600;
	LCD_CS=0;
	DMA_LCM_CR = 0xa0;	//Write dat
	
#else
	
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		LCD_WR_DATA_16Bit(color);	//д������ 	 
	}
	
#endif
}

//u16 LCD_Read_ID2(void)
//{
//    u16 val;

//    LCD_WR_REG(0x00);		//ID: Status Read (RS)
//    //LCM Read
//    LCD_CS = 0;
//    LCMIFCR |= 0x87;		//Enable interface, Read data
//    while(!LCD_CS);
//    val = (LCMIFDATL << 8);

//    LCD_CS = 0;
//    LCMIFCR |= 0x87;		//Enable interface, Read data
//    while(!LCD_CS);
//    val |= LCMIFDATL;

//    return (val);
//}


void Test_Color(void)
{
	u16 lcd_id;
	u8 buf[10] = {0};

	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	while(!LCD_CS);

	DMA_LCM_AMT = 0x01;		//Exe 2(n+1) bytes
	lcd_id = LCD_Read_ID();
	sprintf((char *)buf,"ID:0x%x",lcd_id);
	Show_Str(50,25,BLUE,YELLOW,buf,16,1);
	DMA_LCM_AMT = 0xff;		//Exe 256(n+1) bytes

//	lcd_id = LCD_Read_ID2();
//	sprintf((char *)buf,"ID:0x%x",lcd_id);
//	Show_Str(50,25,BLUE,YELLOW,buf,16,1);
    
	delay_ms(800);
	LCD_Fill(0,0,LCD_W,LCD_H,RED);
	delay_ms(800);
	LCD_Fill(0,0,LCD_W,LCD_H,GREEN);
	delay_ms(800);
	LCD_Fill(0,0,LCD_W,LCD_H,BLUE);
	delay_ms(800);
}

//========================================================================
// ����: void delay_ms(u16 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);   //6T per loop
    }while(--ms);
}

void Write_Cmd(unsigned char DH,unsigned char DL)
{
#if USR_LCM_IF == 1
	
	LCMIFDATL = DH;
	LCD_CS = 0;
	LcmFlag = 1;
	LCMIFCR = LCMIFCR_OFFSET | 0x04;    //write command out
	while(LcmFlag);
	
	LCMIFDATL = DL;
	LcmFlag = 1;
	LCMIFCR = LCMIFCR_OFFSET | 0x04;    //write command out
	while(LcmFlag);
	LCD_CS = 1 ;
	
#else
	
	LCD_CS=0;
	LCD_RS=0;
	LCD_WR=0;
	LCD_DataPort=DH;
	LCD_WR=1;
	_nop_();
	LCD_WR=0;
	LCD_DataPort=DL;
	LCD_WR=1;
	LCD_CS=1;
	
#endif
}

void Write_Data(unsigned char DH,unsigned char DL)
{
#if USR_LCM_IF == 1
	
	LCMIFDATL = DH;
	LCD_CS = 0;
	LcmFlag = 1;
	LCMIFCR = LCMIFCR_OFFSET | 0x05;		//write data out
	while(LcmFlag);
	
	LCMIFDATL = DL;
	LcmFlag = 1;
	LCMIFCR = LCMIFCR_OFFSET | 0x05;		//write data out
	while(LcmFlag);
	LCD_CS = 1 ;
	
#else
	
	LCD_CS=0;
	LCD_RS=1;
	LCD_WR=0;
	LCD_DataPort=DH;
	LCD_WR=1;
	_nop_();
	LCD_WR=0;
	LCD_DataPort=DL;	
	LCD_WR=1;
	LCD_CS=1;
	
#endif
}

void LCD_WR_REG(u8 Reg)	 
{	
#if USR_LCM_IF == 1
	
	LCMIFDATL = Reg;
	LCD_CS = 0;
	LcmFlag = 1;
	LCMIFCR = LCMIFCR_OFFSET | 0x04;		//write command out
	while(LcmFlag);
	LCD_CS = 1 ;
	
#else
	
	LCD_RS=0;
	LCD_CS=0;
	LCD_WR=0;
	LCD_DataPort = Reg;
	LCD_WR=1;
	LCD_CS=1;
	
#endif
}

u16 LCD_RD_DATA(void)
{
#if USR_LCM_IF == 1
	
	//DMA Read
	LCD_CS=0;
	DmaFlag = 1;
	DMA_LCM_CR = 0x88;	//Read data
	while(DmaFlag);
	LCD_CS=1;
	return (Buffer[0]);
	
#else
	
	u16 d;
	LCD_RS=1;
	LCD_CS = 0;
	LCD_RD = 0;
	_nop_();
	_nop_();
	_nop_();
	d = (LCD_DataPort << 8);
	LCD_RD = 1;
	_nop_();
	_nop_();
	_nop_();
	LCD_RD = 0;
	_nop_();
	_nop_();
	_nop_();
	d |= LCD_DataPort;
	LCD_RD = 1;
	LCD_CS = 1;
	return d;
	
#endif
}

void LCD_WR_REG16(u8 Reg, u16 dat)	 
{	
	Write_Cmd(0x00,Reg);
	Write_Data((dat>>8)&0xFF,dat&0xFF);
} 

void LCD_WR_DATA_16Bit(u16 Data)
{
	Write_Data((Data>>8)&0xFF,Data&0xFF);
}

/*****************************************************************************
 * @name       :u16 LCD_Read_ID(void)
 * @date       :2020-12-11 
 * @function   :Read ID
 * @parameters :None
 * @retvalue   :ID value
******************************************************************************/ 
u16 LCD_Read_ID(void)
{
	u16 val;

#if USR_LCM_IF == 1
	
	LCD_WR_REG(0x00);		//ID: Status Read (RS)
	val = LCD_RD_DATA();
	return (val);
	
#else
	
	LCD_WR_REG(0x00);		//ID: Status Read (RS)
	LCD_DataPort = 0xFF; //����P0
  //P0�����ó������
	P2M0=0x00;
	P2M1=0xFF;
	val = LCD_RD_DATA();
  //P0�����ó��������
	P2M0=0xFF;
	P2M1=0x00;
	LCD_DataPort = 0xFF; //����P0

	return (val);
	
#endif
}

/*****************************************************************************
 * @name       :void GPIO_Init(void)
 * @date       :2018-11-13 
 * @function   :Set the gpio to push-pull mode
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void GPIO_Init(void)
{
	//P0.5�����ó��������
	P0M0=0x20;
	P0M1=0x00;

    //P2�����ó�׼˫���
    P2M0=0x00;
    P2M1=0x00;

    //P3.3�����ó������
    //P3.7,P3.6,P3.4,P3.2�����ó��������
    P3M0=0xd4;
    P3M1=0x08;

    //P4.7,P4.5�����ó��������
    P4M0=0xa0;
    P4M1=0x00;

    //P5.3,P5.0�����ó������
    //P5.2�����ó��������
    P5M0=0x04;
    P5M1=0x09;
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
	LCMIFCFG = 0x80;	//bit7 1:Enable Interrupt, bit1 0:8bit mode; bit0 0:8080,1:6800
	LCMIFCFG2 = 0x29;	//RS:P45,E:P37,RW:P36; Setup Time=2,HOLD Time=1
	LCMIFSTA = 0x00;
}

/*****************************************************************************
 * @name       :void DMA_Config(void)
 * @date       :2020-12-09 
 * @function   :Config DMA
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void DMA_Config(void)
{
	DMA_LCM_AMT = 0xff;		//Exe 256(n+1) bytes
	DMA_LCM_TXAH = (u8)((u16)&Color >> 8);
	DMA_LCM_TXAL = (u8)((u16)&Color);
	DMA_LCM_RXAH = (u8)((u16)&Buffer >> 8);
	DMA_LCM_RXAL = (u8)((u16)&Buffer);
	DMA_LCM_STA = 0x00;
	DMA_LCM_CFG = 0x82;
	DMA_LCM_CR = 0x00;
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
	LCDReset(); //��ʼ��֮ǰ��λ
	delay_ms(150);                     //���ݲ�ͬ�����ٶȿ��Ե�����ʱ�������ȶ���ʾ
//*************2.4inch ILI9325��ʼ��**********//	
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

	//����LCD���Բ���
	#if USE_HORIZONTAL==1
	LCD_W = 320;
	LCD_H = 240;
	LCD_WR_REG16(0x0003, 0x1038); // set GRAM write direction and BGR=1.
	LCD_WR_REG16(0x0001, 0x0000); // set SS and SM bit
	#else
	LCD_W = 240;
	LCD_H = 320;
	LCD_WR_REG16(0x0003, 0x1030); // set GRAM write direction and BGR=1.
	LCD_WR_REG16(0x0001, 0x0100); // set SS and SM bit
	#endif 
}

#define WINDOW_XADDR_START	0x0050 // Horizontal Start Address Set
#define WINDOW_XADDR_END		0x0051 // Horizontal End Address Set
#define WINDOW_YADDR_START	0x0052 // Vertical Start Address Set
#define WINDOW_YADDR_END	0x0053 // Vertical End Address Set
#define GRAM_XADDR		    0x0020 // GRAM Horizontal Address Set
#define GRAM_YADDR		    0x0021 // GRAM Vertical Address Set
#define GRAMWR 			    0x0022 // memory write

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
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetWindows(x,y,x,y);//���ù��λ�� 
	LCD_WR_DATA_16Bit(POINT_COLOR); 	    
} 	 

/*****************************************************************************
 * @name       :void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display a single English character
 * @parameters :x:the beginning x coordinate of the Character display position
                y:the beginning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)
{  
	u8 temp;
	u8 pos,t;
	u16 colortemp=POINT_COLOR;

	num=num-' ';//�õ�ƫ�ƺ��ֵ
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//���õ���������ʾ����
	if(!mode) //�ǵ��ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
			{
				if(temp&0x01)LCD_WR_DATA_16Bit(fc); 
				else LCD_WR_DATA_16Bit(bc); 
				temp>>=1; 
			}
		}
	}
	else//���ӷ�ʽ
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//����1206����
			else temp=asc2_1608[num][pos];		 //����1608����
			for(t=0;t<size/2;t++)
			{
				POINT_COLOR=fc;
				if(temp&0x01)	LCD_DrawPoint(x+t,y+pos);//��һ����
				temp>>=1;
			}
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,LCD_W-1,LCD_H-1);//�ָ�����Ϊȫ��    	   	 	  
}

/*****************************************************************************
 * @name       :void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
 * @date       :2018-08-09 
 * @function   :Display Chinese and English strings
 * @parameters :x:the beginning x coordinate of the Chinese and English strings
                y:the beginning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/	   		   
void Show_Str(u16 x, u16 y, u16 fc, u16 bc, u8 *str,u8 size,u8 mode)
{					
	u16 x0=x;							  	  
	u8 bHz=0;     //�ַ��������� 
	while(*str!=0)//����δ����
	{ 
		if(!bHz)
		{
			if(x>(LCD_W-size/2)||y>(LCD_H-size)) 
			return;
			if(*str>0x80)	bHz=1;//���� 
			else              //�ַ�
			{
				if(*str==0x0D)//���з���
				{
					y+=size;
					x=x0;
					str++;
				}
				else
				{
					if(size>16)//�ֿ���û�м���12X24 16X32��Ӣ������,��8X16����
					{  
						LCD_ShowChar(x,y,fc,bc,*str,16,mode);
						x+=8; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
					else
					{
						LCD_ShowChar(x,y,fc,bc,*str,size,mode);
						x+=size/2; //�ַ�,Ϊȫ�ֵ�һ�� 
					}
				}
				str++;
			}
		}
		else//����
		{
//			if(x>(lcddev.width-size)||y>(lcddev.height-size))
//			return;
//			bHz=0;//�к��ֿ�
//			if(size==32)
//			GUI_DrawFont32(x,y,fc,bc,str,mode);
//			else if(size==24)
//			GUI_DrawFont24(x,y,fc,bc,str,mode);
//			else
//			GUI_DrawFont16(x,y,fc,bc,str,mode);

			str+=2;
			x+=size;//��һ������ƫ��
		}
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
	if(LCMIFSTA & 0x01)
	{
		LCMIFSTA = 0x00;
		LcmFlag = 0;
	}
	
	if(DMA_LCM_STA & 0x01)
	{
		if(DmaFlag)
		{
			DmaFlag = 0;
			DMA_LCM_CR = 0;
		}
		else
		{
			index--;
			if(index == 0)
			{
				DMA_LCM_CR = 0;
				LCD_CS=1;
			}
			else
			{
				DMA_LCM_CR = 0xa0;	//Write dat
			}
		}
		DMA_LCM_STA = 0;
	}
}
