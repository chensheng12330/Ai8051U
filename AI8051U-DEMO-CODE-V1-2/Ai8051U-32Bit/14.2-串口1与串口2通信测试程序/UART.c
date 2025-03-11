/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

��ʵ�����ϵ�S2�������ز���"ͨ"���в���.

����1(P3.6,P3.7)��ʱ����һ�����ݸ�����2(P4.2,P4.3).

����2(P4.2,P4.3)����Ӵ���1(P3.6,P3.7)���յ�������.

����ʱ, ѡ��ʱ�� 22.1184MHZ (�û��������޸�Ƶ��).

******************************************/

#include    "..\comm\AI8051U.h"
#include    "intrins.h"

#define     MAIN_Fosc       22118400L   //������ʱ��

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** �û������ ***********************************/

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define Baudrate2   115200UL
#define Baudrate1   115200UL

#define UART2_BUF_LENGTH    128
#define UART1_BUF_LENGTH    128

/*****************************************************************************/

/*************  ���ر�������    **************/
bit B_1ms;      //1ms��־
u16 Sec_Cnt;    //1�����

u8  TX2_Cnt;    //���ͼ���
u8  RX2_Cnt;    //���ռ���
u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX2_Busy; //����æ��־
bit B_TX1_Busy; //����æ��־
u8 RX2_TimeOut;
u8 RX1_TimeOut;

u8  xdata RX2_Buffer[UART2_BUF_LENGTH]; //���ջ���
u8  xdata RX1_Buffer[UART1_BUF_LENGTH]; //���ջ���

void UART2_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void UART1_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
void UART2_TxByte(u8 dat);
void UART1_TxByte(u8 dat);
void PrintString2(u8 *puts);
void PrintString1(u8 *puts);
/******************** ������ **************************/
void main(void)
{
    u8 i;
	
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

    //  Timer0��ʼ��
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    UART2_config(2);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
    UART1_config(1);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    EA = 1;     //�����ж�

    PrintString2("AI8051U UART2 Test Programme!\r\n");  //UART2����һ���ַ���
    PrintString1("AI8051U UART1 Test Programme!\r\n");  //UART1����һ���ַ���

    while (1)
    {
        if(B_1ms)
        {
            B_1ms = 0;
            Sec_Cnt++;
            if(Sec_Cnt>=1000)
            {
                Sec_Cnt = 0;
                PrintString1("AI8051U UART1-UART2 Test Programme!\r\n");  //UART1����һ���ַ���
            }
//            if(RX1_TimeOut > 0)     //��ʱ����
//            {
//                if(--RX1_TimeOut == 0)
//                {
//                    for(i=0; i<RX1_Cnt; i++)    UART1_TxByte(RX1_Buffer[i]);    //����3���յ�������ԭ������
//                    RX1_Cnt  = 0;   //����ֽ���
//                }
//            }

            if(RX2_TimeOut > 0)     //��ʱ����
            {
                if(--RX2_TimeOut == 0)
                {
                    for(i=0; i<RX2_Cnt; i++)    UART2_TxByte(RX2_Buffer[i]);    //���յ�������ͨ������2���
                    RX2_Cnt  = 0;   //����ֽ���
                }
            }
        }
    }
}


/********************** Timer0 1ms�жϺ��� ************************/
void timer0(void) interrupt 1
{
	B_1ms = 1;
}

//========================================================================
// ����: void UART2_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
void UART2_TxByte(u8 dat)
{
    B_TX2_Busy = 1;
    S2BUF = dat;
    while(B_TX2_Busy);
}

//========================================================================
// ����: void UART1_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    B_TX1_Busy = 1;
    SBUF = dat;
    while(B_TX1_Busy);
}

//========================================================================
// ����: void PrintString2(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString2(u8 *puts)
{
    for (; *puts != 0;  puts++)     //����ֹͣ��0����
    {
        UART2_TxByte(*puts);
    }
}

//========================================================================
// ����: void PrintString1(u8 *puts)
// ����: ����3�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString1(u8 *puts)
{
    for (; *puts != 0;  puts++)     //����ֹͣ��0����
    {
        UART1_TxByte(*puts);
    }
}

//========================================================================
// ����: SetTimer2Baudraye(u32 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u32 dat)
{
    T2R = 0;		//Timer stop
    T2_CT = 0;	//Timer2 set As Timer
    T2x12 = 1;	//Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;		//Timer run enable
}

//========================================================================
// ����: void UART1_config(u8 brt)
// ����: UART1��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART1_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
    /*********** ������ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate1);
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;    //��ֹ�ж�
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
    ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x40;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;

}

//========================================================================
// ����: void UART2_config(u8 brt)
// ����: UART2��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    if(brt == 2)
    {
        SetTimer2Baudraye(65536UL - (MAIN_Fosc / 4) / Baudrate2);

        S2CFG |= 0x01;     //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
        S2CON = (S2CON & 0x3f) | 0x40;    //UART2ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
        ES2   = 1;         //�����ж�
        S2REN = 1;         //�������
        S2_S  = 1;         //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3

        B_TX2_Busy = 0;
        TX2_Cnt = 0;
        RX2_Cnt = 0;
    }
}

//========================================================================
// ����: void UART2_int (void) interrupt UART2_VECTOR
// ����: UART2�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART2_int (void) interrupt 8
{
    if(S2RI)
    {
        S2RI = 0;    //Clear Rx flag
        if(RX2_Cnt >= UART2_BUF_LENGTH)   RX2_Cnt = 0;
        RX2_Buffer[RX2_Cnt] = S2BUF;
        RX2_Cnt++;
        RX2_TimeOut = 5;
    }

    if(S2TI)
    {
        S2TI = 0;    //Clear Tx flag
        B_TX2_Busy = 0;
    }
}

//========================================================================
// ����: void UART1_int (void) interrupt UART1_VECTOR
// ����: UART1�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;    //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;   //Clear Tx flag
        B_TX1_Busy = 0;
    }
}
