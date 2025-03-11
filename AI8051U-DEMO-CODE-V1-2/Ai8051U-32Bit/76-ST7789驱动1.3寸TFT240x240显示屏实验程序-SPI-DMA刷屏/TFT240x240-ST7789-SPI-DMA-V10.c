
/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ����V1.1�汾���б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

��ɫSPI�ӿ�TFT LCD240x240����ʾ����ͨ��SPI DMA��3200�ֽڵ�ͼƬ�����͵�����������ʱ��ռ��CPUʱ�䡣

��ʾͼ�Σ����֣�Ӣ�ģ�����. TFT LCD240X240ʹ���о�԰��Һ����������IC�ͺ�ΪST7789V3��

����ͼ����ʾ��������ʹ��SPI��ѯ��ʽ(11�ֽ�)��ͼƬ����ʹ��SPI DMA����������������40MHz, ÿ��SPI DMA������ʱ��1.52ms. ����ˢ��55mms.

��Ҫ��ʾ�����ݷ���1024�ֽڵ��Դ��У�����DMA���伴�ɡ�

����ʱ, ѡ��ʱ�� 40MHz (�û��������޸�Ƶ�ʺ����±��뼴��).

******************************************/

	#define	FOSC	40000000UL

	#include	"AI8051U.h"
	#include 	"LCD.h"
	#include	"lcdfont.h"
	#include 	"pic.h"

//-----------------LCD�˿ڶ���----------------
/*	����ӿ�	*/
							//GND	AI8051Uʵ���� V1.1
							//VCC	3~5V
sbit P_LCD_CLK	=  	P3^2;	//D0	SPI or II2 ��ʱ�ӽ�
sbit P_LCD_SDA	=   P3^3;	//D1	SPI or II2 �����ݽ�
sbit P_LCD_RST	=  	P4^7;	//RES	��λ��, �͵�ƽ��λ
sbit P_LCD_DC	=  	P1^1;	//DC	���ݻ������
sbit P_LCD_CS	=	P3^5;	//CS	Ƭѡ��


// ��ʾ����
#define USE_HORIZONTAL 3 //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����
#define LCD_W 240
#define LCD_H 240


//-----------------��������----------------
bit	B_SPI_DMA_busy;		//SPI DMAæ��־�� 1��־SPI-DMAæ��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0
u16	SPI_TxAddr;			//SPI DMAҪ�������ݵ��׵�ַ
u8	xdata DisTmp[3200];	//��ʾ���壬��Ҫ��ʾ�����ݷ����Դ������DMA����. ����LCM DMA��4�ֽڶ������⣬�������ﶨλ�Ե�ַΪ4�ı���


//N ms��ʱ����
void delay_ms(u16 ms)	// 1~65535 ms
{
	u16 i;
	do
	{
		i = FOSC / 6000;	//STC32ϵ��
		while(--i)	;
	}while(--ms);
}


//========================================================================
// ����: void  SPI_Config(u8 SPI_io, u8 SPI_speed)
// ����: SPI��ʼ��������
// ����: io: �л�����IO,            SS  MOSI MISO SCLK
//                       0: �л��� P1.4 P1.5 P1.6 P1.7
//                       1: �л��� P2.4 P2.5 P2.6 P2.7
//                       2: �л��� P4.0 P4.1 P4.2 P4.3
//                       3: �л��� P3.5 P3.4 P3.3 P3.2
//       SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
// ����: none.
// �汾: VER1.0
// ����: 2024-8-13
// ��ע:
//========================================================================
void  SPI_Config(u8 SPI_io, u8 SPI_speed)
{
	SPI_io &= 3;

	SPCTL = SPI_speed & 3;	//����SPI �ٶ�, ����ָ����ִ��, ˳��Bit7~Bit2��0
	SSIG = 1;	//1: ����SS�ţ���MSTRλ�����������Ǵӻ�		0: SS�����ھ����������Ǵӻ���
	SPEN = 1;	//1: ����SPI��								0����ֹSPI������SPI�ܽž�Ϊ��ͨIO
	DORD = 0;	//1��LSB�ȷ���								0��MSB�ȷ�
	MSTR = 1;	//1����Ϊ����								0����Ϊ�ӻ�
	CPOL = 1;	//1: ����ʱSCLKΪ�ߵ�ƽ��					0������ʱSCLKΪ�͵�ƽ
	CPHA = 1;	//1: ������SCLKǰ������,���ز���.			0: ������SCLKǰ�ز���,��������.
//	SPR1 = 0;	//SPR1,SPR0   00: fosc/4,     01: fosc/8
//	SPR0 = 0;	//            10: fosc/16,    11: fosc/2
	P_SW1 = (P_SW1 & ~0x0c) | ((SPI_io<<2) & 0x0c);		//�л�IO

	HSCLKDIV   = 1;					//HSCLKDIV��ʱ�ӷ�Ƶ
	SPI_CLKDIV = 1;					//SPI_CLKDIV��ʱ�ӷ�Ƶ
	SPSTAT = 0x80 + 0x40;			//��0 SPIF��WCOL��־

	if(SPI_io == 0)
	{
		P1n_standard(0xf0);			//�л��� P1.4(SS) P1.5(MOSI) P1.6(MISO) P1.7(SCLK), ����Ϊ׼˫���
		PullUpEnable(P1PU, 0xf0);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P1n_push_pull(Pin7+Pin5);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P1SR, Pin7+Pin5);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 1)
	{
		P2n_standard(0xf0);			//�л���P2.4(SS) P2.5(MOSI) P2.6(MISO) P2.7(SCLK), ����Ϊ׼˫���
		PullUpEnable(P2PU, 0xf0);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P2n_push_pull(Pin7+Pin5);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P2SR, Pin7+Pin5);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 2)
	{
		P4n_standard(0x0f);			//�л���P4.0(SS) P4.1(MOSI) P4.2(MISO) P4.3(SCLK), ����Ϊ׼˫���
		PullUpEnable(P4PU, 0x0f);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P4n_push_pull(Pin3+Pin1);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P4SR, Pin3+Pin1);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 3)
	{
		P3n_standard(0x3C);		//�л���P3.5(SS) P3.4(MOSI) P3.3(MISO) P3.2(SCLK), ����Ϊ׼˫���
		PullUpEnable(P3PU, 0x3c);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P3n_push_pull(Pin4+Pin2);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P3SR, Pin4+Pin2);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
}


//========================================================================
// ����: дSPIһ���ֽں���
// ����: ��.
// ����: ��
//========================================================================

void	LCD_SendByte(u8 dat)
{
	SPDAT = dat;	//����һ���ֽ�
	while(SPIF == 0)	;			//�ȴ��������
	SPSTAT = 0x80 + 0x40;			//��0 SPIF��WCOL��־
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	P_LCD_CS = 0;
	LCD_SendByte(dat);
	P_LCD_CS = 1;
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA16(u16 dat)
{
	P_LCD_CS = 0;
	LCD_SendByte((u8)(dat>>8));
	LCD_SendByte((u8)dat);
	P_LCD_CS = 1;
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	P_LCD_DC = 0;//д����
	P_LCD_CS = 0;
	LCD_SendByte(dat);
	P_LCD_CS = 1;
	P_LCD_DC = 1;//д����
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL == 1)
	{
		y1 += 80;
		y2 += 80;
	}
	else if(USE_HORIZONTAL == 3)
	{
		x1 += 80;
		x2 += 80;
	}

	LCD_WR_REG(0x2a);//�е�ַ����
	LCD_WR_DATA16(x1);
	LCD_WR_DATA16(x2);
	LCD_WR_REG(0x2b);//�е�ַ����
	LCD_WR_DATA16(y1);
	LCD_WR_DATA16(y2);
	LCD_WR_REG(0x2c);//������д
}


/******************************************************************************
      ����˵����LCD��ʼ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Init(void)
{
	P_LCD_CLK = 1;//SCLK
	P_LCD_SDA = 1;//MOSI
	P_LCD_RST = 1;//RES
	P_LCD_DC  = 1;//DC
	P_LCD_CS  = 1;//CS

	P_LCD_RST = 0;
	delay_ms(100);
	P_LCD_RST = 1;
	delay_ms(100);

	LCD_WR_REG(0x11); 	//Sleep out �˳�˯��
	delay_ms(120);		//Delay 120ms
	LCD_WR_REG(0x36);	//�Դ���ʿ���
		 if(USE_HORIZONTAL == 0)	LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL == 1)	LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL == 2)	LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0);

	LCD_WR_REG(0x3A);	//�ӿڸ�ʽ
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA8(0x35);

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x20);   //2b

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x18);   //VDV, 0x20:0v

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x13);   //0x13:60Hz

	LCD_WR_REG(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6);
	LCD_WR_DATA8(0xA1);   //sleep in��gate���ΪGND

	//---------------ST7789V gamma setting-------------//
	LCD_WR_REG(0xE0);	//Set Gamma
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x3C);
	LCD_WR_DATA8(0x36);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x29);
	LCD_WR_DATA8(0x30);

	LCD_WR_REG(0xE1);	//Set Gamma
	LCD_WR_DATA8(0xF0);
	LCD_WR_DATA8(0x02);
	LCD_WR_DATA8(0x04);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x25);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x3B);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x12);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x27);
	LCD_WR_DATA8(0x31);

	LCD_WR_REG(0xE4);
	LCD_WR_DATA8(0x1D);   //ʹ��240��gate  (N+1)*8
	LCD_WR_DATA8(0x00);   //�趨gate���λ��
	LCD_WR_DATA8(0x00);   //��gateû������ʱ��bit4(TMG)��Ϊ0

	LCD_WR_REG(0x21);

	LCD_WR_REG(0x29);	//������ʾ
}

/******************************************************************************
      ����˵������ָ�����������ɫ
      ������ݣ�xsta,ysta   ��ʼ����
                xend,yend   ��ֹ����
								color       Ҫ������ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
	u16 i,j;
	LCD_Address_Set(xsta,ysta,xend-1,yend-1);//������ʾ��Χ
	for(i=ysta;i<yend;i++)
	{
		for(j=xsta;j<xend;j++)
		{
			LCD_WR_DATA16(color);
		}
	}
}

/******************************************************************************
      ����˵������ָ��λ�û���
      ������ݣ�x,y ��������
                color �����ɫ
      ����ֵ��  ��
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//���ù��λ��
	LCD_WR_DATA16(color);
}

/******************************************************************************
      ����˵������ʾ����12x12����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;

	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++)
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont12[k].Msk[i]&(0x01<<j))		LCD_WR_DATA16(fc);
						else LCD_WR_DATA16(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
}

/******************************************************************************
      ����˵������ʾ����16x16����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++)
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_WR_DATA16(fc);
						else LCD_WR_DATA16(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
}


/******************************************************************************
      ����˵������ʾ����24x24����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++)
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_WR_DATA16(fc);
						else LCD_WR_DATA16(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
}

/******************************************************************************
      ����˵������ʾ����32x32����
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//ͳ�ƺ�����Ŀ
	for(k=0;k<HZnum;k++)
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{
					if(!mode)//�ǵ��ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_WR_DATA16(fc);
						else LCD_WR_DATA16(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//���ӷ�ʽ
					{
						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//��һ����
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
}


/******************************************************************************
      ����˵������ʾ�����ַ�
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ���ַ�
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	u16 x0=x;
	sizex=sizey/2;
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //�õ�ƫ�ƺ��ֵ
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //���ù��λ��
	for(i=0;i<TypefaceNum;i++)
	{
		if(sizey==12)temp=ascii_1206[num][i];		       //����6x12����
		else if(sizey==16)temp=ascii_1608[num][i];		 //����8x16����
		else if(sizey==24)temp=ascii_2412[num][i];		 //����12x24����
		else if(sizey==32)temp=ascii_3216[num][i];		 //����16x32����
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//�ǵ���ģʽ
			{
				if(temp&(0x01<<t))	LCD_WR_DATA16(fc);
				else LCD_WR_DATA16(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//����ģʽ
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//��һ����
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}
}


/******************************************************************************
      ����˵������ʾ���ִ�
      ������ݣ�x,y��ʾ����
                *s Ҫ��ʾ�ĺ��ִ�
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ� ��ѡ 16 24 32
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
			 if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;
		x+=sizey;
	}
}

/******************************************************************************
      ����˵������ʾ�ַ���
      ������ݣ�x,y��ʾ����
                *p Ҫ��ʾ���ַ���
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
                mode:  0�ǵ���ģʽ  1����ģʽ
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*p!='\0')
	{
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;
		p++;
	}
}


/******************************************************************************
      ����˵������ʾ����
      ������ݣ�m������nָ��
      ����ֵ��  ��
******************************************************************************/
u32 mypow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;
	return result;
}


/******************************************************************************
      ����˵������ʾ��������
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾ��������
                len Ҫ��ʾ��λ��
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowIntNum(u16 x,u16 y,u16 num,u8 len,u16 fc,u16 bc,u8 sizey)
{
	u8 t,temp;
	u8 enshow=0;
	u8 sizex=sizey/2;
	for(t=0;t<len;t++)
	{
		temp = (num/mypow(10, (u8)(len-t-1)))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
				continue;
			}else enshow=1;

		}
	 	LCD_ShowChar(x+t*sizex,y, (u8)(temp+48),fc,bc,sizey,0);
	}
}


/******************************************************************************
      ����˵������ʾ��λС������
      ������ݣ�x,y��ʾ����
                num Ҫ��ʾС������
                len Ҫ��ʾ��λ��
                fc �ֵ���ɫ
                bc �ֵı���ɫ
                sizey �ֺ�
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowFloatNum1(u16 x,u16 y,float num,u8 len,u16 fc,u16 bc,u8 sizey)
{
	u8 t,temp,sizex;
	u16 num1;
	sizex=sizey/2;
	num1=num*100;
	for(t=0;t<len;t++)
	{
		temp=(num1/mypow(10, (u8)(len-t-1)))%10;
		if(t==(len-2))
		{
			LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
			t++;
			len+=1;
		}
	 	LCD_ShowChar(x+t*sizex,y,(u8)(temp+48),fc,bc,sizey,0);
	}
}


/******************************************************************************
      ����˵������ʾͼƬ
      ������ݣ�x,y�������
                length ͼƬ����
                width  ͼƬ���
                pic[]  ͼƬ����
      ����ֵ��  ��
******************************************************************************/
void LCD_ShowPicture(u16 x,u16 y,u16 length,u16 width, u8 xdata *pic)
{
	LCD_Address_Set(x,y,x+length-1,y+width-1);
	SPI_DMA_TRIG(pic);	//����SPI DAM����һ��ͼƬ
	while(B_SPI_DMA_busy);	//�ȴ�ͼƬ�������
}


//DMA_SPI_CR 	SPI_DMA���ƼĴ���
#define		DMA_ENSPI		(1<<7)	// SPI DMA����ʹ�ܿ���λ��    bit7, 0:��ֹSPI DMA���ܣ�  1������SPI DMA���ܡ�
#define		SPI_TRIG_M		(1<<6)	// SPI DMA����ģʽ��������λ��bit6, 0:д0��Ч��          1��д1��ʼSPI DMA����ģʽ������
#define		SPI_TRIG_S		(0<<5)	// SPI DMA�ӻ�ģʽ��������λ��bit5, 0:д0��Ч��          1��д1��ʼSPI DMA�ӻ�ģʽ������
#define		SPI_CLRFIFO		1		// ���SPI DMA����FIFO����λ��bit0, 0:д0��Ч��          1��д1��λFIFOָ�롣


//DMA_SPI_CFG 	SPI_DMA���üĴ���
#define		DMA_SPIIE	(1<<7)	// SPI DMA�ж�ʹ�ܿ���λ��bit7, 0:��ֹSPI DMA�жϣ�     1�������жϡ�
#define		SPI_ACT_TX	(1<<6)	// SPI DMA�������ݿ���λ��bit6, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1�������͡�
#define		SPI_ACT_RX	(0<<5)	// SPI DMA�������ݿ���λ��bit5, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1��������ա�
#define		DMA_SPIIP	(0<<2)	// SPI DMA�ж����ȼ�����λ��bit3~bit2, (���)0~3(���).
#define		DMA_SPIPTY	0		// SPI DMA�������߷������ȼ�����λ��bit1~bit0, (���)0~3(���).

//DMA_SPI_CFG2 	SPI_DMA���üĴ���2
#define		SPI_WRPSS	(0<<2)	// SPI DMA������ʹ��SS�ſ���λ��bit2, 0: SPI DMA������̲��Զ�����SS�š�  1���Զ�����SS�š�
#define		SPI_SSS	    3		// SPI DMA�������Զ�����SS��ѡ��λ��bit1~bit0, 0: P1.4,  1��P2.4,  2: P4.0,  3:P3.5��

//DMA_SPI_STA 	SPI_DMA״̬�Ĵ���
#define		SPI_TXOVW	(1<<2)	// SPI DMA���ݸ��Ǳ�־λ��bit2, �����0.
#define		SPI_RXLOSS	(1<<1)	// SPI DMA�������ݶ�����־λ��bit1, �����0.
#define		DMA_SPIIF	1		// SPI DMA�ж������־λ��bit0, �����0.

//HSSPI_CFG  ����SPI���üĴ���
#define		SS_HOLD		(0<<4)	//����ģʽʱSS�����źŵ�HOLDʱ�䣬 0~15, Ĭ��3. ��DMA�л�����N��ϵͳʱ�ӣ���SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.
#define		SS_SETUP		3	//����ģʽʱSS�����źŵ�SETUPʱ�䣬0~15, Ĭ��3. ��DMA�в�Ӱ��ʱ�䣬       ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.

//HSSPI_CFG2  ����SPI���üĴ���2
#define		SPI_IOSW	(1<<6)	//bit6:����MOSI��MISO��λ��0����������1������
#define		HSSPIEN		(0<<5)	//bit5:����SPIʹ��λ��0���رո���ģʽ��1��ʹ�ܸ���ģʽ
#define		FIFOEN		(1<<4)	//bit4:����SPI��FIFOģʽʹ��λ��0���ر�FIFOģʽ��1��ʹ��FIFOģʽ��ʹ��FIFOģʽ��DMA�м���13��ϵͳʱ�䡣
#define		SS_DACT			3	//bit3~0:����ģʽʱSS�����źŵ�DEACTIVEʱ�䣬0~15, Ĭ��3, ��Ӱ��DMAʱ��.  ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.


void	SPI_DMA_TRIG(u8 xdata *TxBuf)
{
				//@40MHz, Fosc/4, 200�ֽ�258us��100�ֽ�  130us��50�ֽ�66us��N���ֽں�ʱ N*1.280+2 us, 51Tһ���ֽڣ�����״̬��19T, �����ʱ32T.
				//@40MHz, Fosc/2, 200�ֽ�177us��100�ֽ� 89.5us��50�ֽ�46us��N���ֽں�ʱ N*0.875+2 us, 35Tһ���ֽڣ�����״̬��19T, �����ʱ16T.
				//@40MHz, Fosc/2, SPI DMA����һ���ֽ�, FIFO=1, HOLD=0����ʱ16+3=19T(0.475us), HOLD=3����ʱ16+6=22T(0.55us).
				//@40MHz, Fosc/4, SPI DMA����һ���ֽ�, FIFO=1, HOLD=0����ʱ32+3=35T(0.875us), HOLD=3����ʱ32+6=38T(0.95us).
	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD������N��ϵͳʱ��, SS_SETUPû������ʱ�ӡ�����OLED 40MHzʱSS_HOLD��������Ϊ0��
	HSSPI_CFG2 = SPI_IOSW | FIFOEN | SS_DACT;	//FIFOEN����FIFO���С13��ʱ��.

	P_LCD_DC  = 1;	//д����
	P_LCD_CS  = 0;	//Ƭѡ
	B_SPI_DMA_busy = 1;	//��־SPI-DMAæ��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0

	SPI_TxAddr   = (u16)TxBuf;		//Ҫ�������ݵ��׵�ַ
	DMA_SPI_TXAH = (u8)(SPI_TxAddr >> 8);		//���͵�ַ�Ĵ������ֽ�
	DMA_SPI_TXAL = (u8)SPI_TxAddr;				//���͵�ַ�Ĵ������ֽ�
	DMA_SPI_AMTH = (u8)((3200-1)/256);		//���ô������ֽ���(��8λ),	���ô������ֽ��� = N+1
	DMA_SPI_AMT  = (u8)(3200-1);			//���ô������ֽ���(��8λ).
	DMA_SPI_ITVH = 0;
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_TX | SPI_ACT_RX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
	DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;	//����SPI DMA��������
}


//========================================================================
// ����: void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
// ����:  SPI_DMA�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2024-1-5
//========================================================================
void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
{
	DMA_SPI_CR = 0;			//�ر�SPI DMA
	B_SPI_DMA_busy = 0;		//���SPI-DMAæ��־��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0
	SPSTAT = 0x80 + 0x40;	//��0 SPIF��WCOL��־
	HSSPI_CFG2 = SPI_IOSW | SS_DACT;	//ʹ��SPI��ѯ���жϷ�ʽʱ��Ҫ��ֹFIFO
	P_LCD_CS = 1;
	DMA_SPI_STA = 0;		//����жϱ�־
}




void main(void)
{
	u16 i;
	u8	j;
	float t=0;

	EAXFR = 1;	//���������չ�Ĵ���
	WTST  = 0;
	CKCON = 0;

	P0M1 = 0;	P0M0 = 0;	//����Ϊ׼˫���
	P1M1 = 0;	P1M0 = 0;	//����Ϊ׼˫���
	P2M1 = 0;	P2M0 = 0;	//����Ϊ׼˫���
	P3M1 = 0;	P3M0 = 0;	//����Ϊ׼˫���
	P4M1 = 0;	P4M0 = 0;	//����Ϊ׼˫���
	P5M1 = 0;	P5M0 = 0;	//����Ϊ׼˫���
	P6M1 = 0;	P6M0 = 0;	//����Ϊ׼˫���
	P7M1 = 0;	P7M0 = 0;	//����Ϊ׼˫���

	//==================== SPI��ʼ�� ==================================
	SPI_Config(3, 3);	//(SPI_io, SPI_speed), ����: 	SPI_io: �л�IO(SS MOSI MISO SCLK), 0: �л���P1.4 P1.5 P1.6 P1.7,  1: �л���P2.4 P2.5 P2.6 P2.7, 2: �л���P4.0 P4.1 P4.2 P4.3,  3: �л���P3.5 P3.4 P3.3 P3.2,
						//								SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
	HSSPI_CFG2 = 0x40;	//����MOSI MISO, P3.3��MOSI

	P1n_standard(Pin1);			//SPI��������Ϊ׼˫���, SPI�Ϳ����ź�
	PullUpEnable(P1PU, Pin1);	// ����˿��ڲ���������     PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	P3n_standard(0x2c);			//SPI��������Ϊ׼˫���, SPI�Ϳ����ź�
	PullUpEnable(P3PU, 0x2c);	// ����˿��ڲ���������     PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	P4n_standard(Pin7);			//SPI��������Ϊ׼˫���, SPI�Ϳ����ź�
	PullUpEnable(P4PU, Pin7);	// ����˿��ڲ���������     PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	//=================================================================

	LCD_Init();//LCD��ʼ��
	LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
	EA = 1;

	while(1)
	{
		LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
		LCD_ShowChinese(0,0,"���ڹ�о�˹�����",RED,WHITE,24,0);
		LCD_ShowString(0,40,"LCD_W:",RED,WHITE,16,0);
		LCD_ShowIntNum(48,40,LCD_W,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowIntNum(128,40,LCD_H,3,RED,WHITE,16);
		LCD_ShowString(80,40,"LCD_H:",RED,WHITE,16,0);
		LCD_ShowString(0,70,"Increaseing Nun:",RED,WHITE,16,0);
		LCD_ShowFloatNum1(128,70,t,4,RED,WHITE,16);
		t+=0.11;
		delay_ms(3000);	// 1~65535 ms

		for(i=0; i<3200; i++)	DisTmp[i] = Image_1[i];	//��ͼƬװ�ص��Դ�
		for(j=0; j<6; j++)		//6��ͼƬ, ����36��ͼƬ @40MHz FIFOEN=1, SS_HOLD=0ʱ55ms @2T
		{
			for(i=0; i<6; i++)	//һ��6��ͼƬ
			{
				LCD_ShowPicture(40*i, j*40, 40, 40, DisTmp);	//����SPI DMA��ʾһ��ͼƬ, 3200�ֽ� 1.52ms @40MHz
			}
		}
		delay_ms(3000);	// 1~65535 ms

		for(i=0; i<3200; i++)	DisTmp[i] = Image_2[i];	//��ͼƬװ�ص��Դ�
		for(j=0; j<6; j++)		//6��ͼƬ, ����36��ͼƬ @40MHz FIFOEN=1, SS_HOLD=0ʱ55ms @2T
		{
			for(i=0; i<6; i++)	//һ��6��ͼƬ
			{
				LCD_ShowPicture(40*i, j*40, 40, 40, DisTmp);	//����SPI DMA��ʾһ��ͼƬ, 3200�ֽ� 1.52ms @40MHz
			}
		}
		delay_ms(3000);
	}
}









