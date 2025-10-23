/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

��STC��MCU��IO��ʽ����74HC595����8λ����ܡ�

��ʾЧ��Ϊ: ��ʾ�����, 5���ι��, �ȸ�λ.

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

#define MAIN_Fosc        24000000UL

//==========================================================================

#define D_WDT_FLAG          (1<<7)
#define D_EN_WDT            (1<<5)
#define D_CLR_WDT           (1<<4)  //auto clear
#define D_IDLE_WDT          (1<<3)  //WDT counter when Idle
#define D_WDT_SCALE_2       0
#define D_WDT_SCALE_4       1
#define D_WDT_SCALE_8       2       //T=393216*N/fo
#define D_WDT_SCALE_16      3
#define D_WDT_SCALE_32      4
#define D_WDT_SCALE_64      5
#define D_WDT_SCALE_128     6
#define D_WDT_SCALE_256     7

//==========================================================================

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*************  ���س�������    **************/
u8 code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��

/*************  IO�ڶ���    **************/
sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  ���ر�������    **************/

u8  LED8[8];        //��ʾ����
u8  display_index;  //��ʾλ����

u16 ms_cnt;
u8  tes_cnt;    //�����õļ�������

/*************  ���غ�������    **************/

void delay_ms(u8 ms);
void DisplayScan(void);

/****************  �ⲿ�����������ⲿ�������� *****************/


/******************** ������ **************************/
void main(void)
{
    u8  i;
    
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

    RSTFLAG |= 0x04;   //���ÿ��Ź���λ��Ҫ���P3.2��״̬�������Ź���λ�����USB����ģʽ

    display_index = 0;
    for(i=0; i<8; i++)  LED8[i] = DIS_BLACK;    //ȫ������
    
    tes_cnt = 0;
    ms_cnt = 0;
    LED8[7] = ms_cnt;

    while(1)
    {
        delay_ms(1);    //��ʱ1ms
        DisplayScan();
        if(tes_cnt <= 5)    //5���ι��, ����λ,
            WDT_CONTR = (D_EN_WDT + D_CLR_WDT + D_WDT_SCALE_16);    // ι��

        if(++ms_cnt >= 1000)
        {
            ms_cnt = 0;
            tes_cnt++;
            LED8[7] = tes_cnt;
        }
    }
}

//========================================================================
// ����: void delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2023-4-1
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

/**************** ��HC595����һ���ֽں��� ******************/
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

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{
    Send_595(t_display[LED8[display_index]]);   //�������
    Send_595(~T_COM[display_index]);            //���λ��

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}
