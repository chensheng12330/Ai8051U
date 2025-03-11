/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

������ҵ�32K xdata����
���Է�ʽ: 
1: д��0x55, �������ж��Ƿ�ȫ����0x55.
2: д��0xaa, �������ж��Ƿ�ȫ����0xaa.
3: д��32768�ֽڵĺ����ֿ�(�������������), �������Ƚ�.

ͨ������2���͵����ַ�x��X, ��ʼ����, ��������صĲ��Խ��.

��������115200bps, 8, N, 1. �л���P4.6 P4.7.

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/****************************** �û������ ***********************************/

#define     ExternalRAM_enable()        EXTRAM = 1      //�����ⲿXRAM
#define     InternalRAM_enable()        EXTRAM = 0      //��ֹ�ⲿXRAM

/*****************************************************************************/

extern unsigned char code hz[];

/*************  ���س�������    **************/

#define MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

#define Baudrate1       (65536 - MAIN_Fosc / 115200 / 4)

#define XDATA_LENTH     32768   //xdata����
#define HZK_LENTH       32768   //�ֿⳤ��

#define BUS_SPEED_SET(x)  BUS_SPEED = x

#define UART1_BUF_LENGTH    64  //���ڻ��峤��

/*************  ���ر�������    **************/
u8  RX1_TimeOut;
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־

u8  RX1_Buffer[UART1_BUF_LENGTH];   //���ջ���
u16 temp;

/*************  ���غ�������    **************/
void delay_ms(u8 ms);
u8   TestXRAM(void);
void Xdata_Test(void);

void delay_ms(u8 ms);
void RX1_Check(void);
void UART1_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void PrintString1(u8 *puts);
void UART1_TxByte(u8 dat);

/********************* ������ *************************/
void main(void)
{
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x30;   P1M0 = 0x00;   //����Ϊ׼˫��ڣ�P1.4,P1.5���ø�������
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
    P0BP = 0x00;    //����Ӳ���Զ�����P0��ģʽ��Ĭ����P0M1/P0M0���ƣ�
    
    delay_ms(10);
    UART1_config(1);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    EA = 1;     //�����ж�
    
    P41 = 0;    //��չSRAMʹ��
    P27 = 0;    //��ߵ�ַλ����

    PrintString1("AI8051U xdata test programme!\r\n"); //UART1����һ���ַ���
    PrintString1("\r\n���ڷ��͵����ַ�x��X, ��ʼ����.\r\n");

    BUS_SPEED_SET(3);       //0~7  3V@22MHZ��1T����ʴ���
    BUS_SPEED |= 0x40;      //ѡ�� P3.7ΪRD��P3.6ΪWR
    ExternalRAM_enable();   //�����ⲿXDATA
//  InternalRAM_enable();   //�����ڲ�XDATA

    while(1)
    {
        delay_ms(1);
        if(RX1_TimeOut > 0)     //��ʱ����
        {
            if(--RX1_TimeOut == 0)  //����ͨѶ����
            {
                if(RX1_Cnt > 0)     //�յ������ֽ���
                {
                    if(RX1_Cnt == 1)    Xdata_Test();   //���ֽ�����
                }
                RX1_Cnt = 0;
            }
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

/*************  ���ʹ����ַ���� *****************/
void TxErrorAddress(u16 i)
{
    PrintString1("�����ַ = ");
    if(i >= 10000)  UART1_TxByte((u8)(i/10000+'0')),  i %= 10000;
    UART1_TxByte((u8)(i/1000+'0')),   i %= 1000;
    UART1_TxByte((u8)(i/100+'0')),    i %= 100;
    UART1_TxByte((u8)(i/10+'0'));
    UART1_TxByte((u8)(i%10+'0'));
    UART1_TxByte(0x0d);
    UART1_TxByte(0x0a);
}

/*************  ����xdata���� *****************/

#define EXRAM           ((unsigned char volatile far *)0x7f0000)

u8  TestXRAM(void)
{
    u16 ptc;
    u16 ptx;
    u16 i,j;

    for(ptx=0; ptx<XDATA_LENTH; ptx++)  EXRAM[ptx] = 0x55;    //�����Ƿ���λ��·
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        if(EXRAM[ptx] != 0x55)
        {
            TxErrorAddress(ptx);
            return 1;   //����0x55����
        }
    }

    for(ptx=0; ptx<XDATA_LENTH; ptx++)  EXRAM[ptx] = 0xaa;    //�����Ƿ���λ��·
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    if(EXRAM[ptx] != 0xaa)
    {
        TxErrorAddress(ptx);
        return 2;   //����0xaa����
    }

    i = 0;
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        EXRAM[ptx] = (u8)(i >> 8);
        ptx++;
        EXRAM[ptx] = (u8)i;
        i++;
    }
    i = 0;
    for(ptx=0; ptx<XDATA_LENTH; ptx++)
    {
        j = EXRAM[ptx];
        ptx++;
        j = (j << 8) + EXRAM[ptx];
        if(i != j)
        {
            TxErrorAddress(ptx);
            return 3;   //д�������ִ���
        }
        i++;
    }

    ptx = 0;
    for(ptc=0; ptc<HZK_LENTH; ptc++)    {EXRAM[ptx] = hz[ptc];    ptx++;}
    ptx = 0;
    for(ptc=0; ptc<HZK_LENTH; ptc++)
    {
        if(EXRAM[ptx] != hz[ptc])
        {
            TxErrorAddress(ptc);
            return 4;   //д�ֿ����
        }
        ptx++;
    }

    return 0;
}

/*************  xdata���Է�����Ϣ���� *****************/
void Xdata_Test(void)
{
    u8  i;
    if((RX1_Buffer[0] == 'x') || (RX1_Buffer[0] == 'X'))
    {
        PrintString1("���� xdata ��, ���Ժ�......\r\n");
        i = TestXRAM();
             if(i == 0) PrintString1("���� xdata ���OK!\r\n");
        else if(i == 1) PrintString1("���� xdata д��0x55����!  ");
        else if(i == 2) PrintString1("���� xdata д��0xaa����!  ");
        else if(i == 3) PrintString1("���� xdata ����д�����!  ");
        else if(i == 4) PrintString1("���� xdata д���ֿ����!  ");
    }
}

//========================================================================
// ����: void UART1_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2024-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    SBUF = dat;
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}

//========================================================================
// ����: void PrintString1(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2024-08-07
// ��ע: 
//========================================================================
void PrintString1(u8 *puts) //����һ���ַ���
{
    for (; *puts != 0;  puts++)     UART1_TxByte(*puts);    //����ֹͣ��0����
}

//========================================================================
// ����: void SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2024-08-07
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;    //Timer run enable
}

//========================================================================
// ����: void UART1_config(u8 brt)
// ����: UART1��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
// ����: none.
// �汾: VER1.0
// ����: 2024-08-07
// ��ע: 
//========================================================================
void UART1_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
    /*********** ������ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)Baudrate1);
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)(Baudrate1 / 256);
        TL1 = (u8)(Baudrate1 % 256);
        ET1 = 0;    //��ֹ�ж�
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
    ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    RX1_Cnt = 0;
}

//========================================================================
// ����: void UART1_int (void) interrupt UART1_VECTOR
// ����: UART1�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2024-08-07
// ��ע: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;     //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;     //Clear Tx flag
        B_TX1_Busy = 0;
    }
}
