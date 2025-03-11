/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

����1ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU���յ��������Զ�����DMA�ռ䡣

��DMA�ռ�������ô�С�����ݺ�ͨ������1��DMA�Զ����͹��ܰѴ洢�ռ������ԭ�����ء�

���ô��ڳ�ʱ�жϣ�MCU���ڽ�����һ�����ݺ������������ʱ��û���յ������ݣ�������ʱ�жϣ����Ѿ���ȡ������ԭ��������

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�
#define Baudrate1       115200L

bit DmaTxFlag;
bit DmaRxFlag;
bit B_RX1_TimeOut;

u8 xdata DmaBuffer[1024];

void UART1_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
void DMA_Config(void);

void UartPutc(unsigned char dat)
{
    SBUF = dat; 
    while(TI == 0); 
    TI = 0;
}
 
char putchar(char c)
{
    UartPutc(c);
    return c;
}

//========================================================================
// ����: void main(void)
// ����: ��������
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void main(void)
{
    u16 i;
    
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

    for(i=0; i<512; i++)
    {
        DmaBuffer[i] = i;
    }

    UART1_config(1);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    printf("AI8051U UART1 TimeOut Test Programme!\r\n");  //UART1����һ���ַ���

    DMA_Config();
    EA = 1; //�������ж�

//    DmaTxFlag = 0;
//    DmaRxFlag = 0;

    while (1)
    {
        if(B_RX1_TimeOut)       //����һ�����ݽ�����������ʱ�ж�
        {
            B_RX1_TimeOut = 0;

            i = ((u16)DMA_UR1R_DONEH << 8) + DMA_UR1R_DONE; //��ȡ�ѽ����ֽڸ���
            TI = 0; //������ͱ�־��DMA�����겻���Զ������־λ��
            printf("cnt=%u\r\n",i);

            DMA_UR1R_CR = 0;
            i--;    //�����ֽ�����Ϊ(i-1)������
            DMA_UR1T_AMT = (u8)i;       //���ô������ֽ���(��8λ)��n+1
            DMA_UR1T_AMTH = (u8)(i>>8); //���ô������ֽ���(��8λ)��n+1

            DMA_UR1T_CR = 0xc0;         //bit7 1:ʹ�� UART1_DMA, bit6 1:��ʼ UART1_DMA �Զ�����
            DMA_UR1R_CR = 0xa1;         //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
        }

//        if((DmaTxFlag) && (DmaRxFlag))  //�շ����DMAָ���ֽ�����
//        {
//            DmaTxFlag = 0;
//            DMA_UR1T_CR = 0xc0;         //bit7 1:ʹ�� UART1_DMA, bit6 1:��ʼ UART1_DMA �Զ�����
//            DmaRxFlag = 0;
//            DMA_UR1R_CR = 0xa1;         //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
//        }
    }
}

//========================================================================
// ����: void DMA_Config(void)
// ����: UART DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2024-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_UR1T_CFG = 0x00;        //bit7 0:Disable Interrupt
    DMA_UR1T_STA = 0x00;
    DMA_UR1T_AMT = 0xff;        //���ô������ֽ���(��8λ)��n+1
    DMA_UR1T_AMTH = 0x01;       //���ô������ֽ���(��8λ)��n+1
    DMA_UR1T_TXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR1T_TXAL = (u8)((u16)&DmaBuffer);
    DMA_UR1T_CR = 0x80;         //bit7 1:ʹ�� UART1_DMA, bit6 1:��ʼ UART1_DMA �Զ�����

    DMA_UR1R_CFG = 0x00;        //bit7 0:Disable Interrupt
    DMA_UR1R_STA = 0x00;
    DMA_UR1R_AMT = 0xff;        //���ô������ֽ���(��8λ)��n+1
    DMA_UR1R_AMTH = 0x03;       //���ô������ֽ���(��8λ)��n+1
    DMA_UR1R_RXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR1R_RXAL = (u8)((u16)&DmaBuffer);
    DMA_UR1R_CR = 0xa1;         //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = dat / 256;
    T2L = dat % 256;
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;    //Timer run enable
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
        S1BRT = 1;      //S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate1));
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;      //S1 BRT Use Timer1;
        T1_CT = 0;      //Timer1 set As Timer
        T1x12 = 1;      //Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate1) % 256);
        ET1 = 0;        //��ֹ�ж�
        TR1  = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
//  ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;  //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    UR1TOCR = 0xc0; //ʹ�ܳ�ʱ���գ�ʹ�ܳ�ʱ�жϣ���ʱʱ��ѡ�� 1:ϵͳʱ��  0:��������λ��(������)
    UR1TOTL = 0x10;
    UR1TOTH = 0x00; //0x5666 = 22118
    UR1TOTE = 0x00; //��Ҫд UR1TOTE ���µ�TMֵ�Ż���Ч
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
//    if(RI)
//    {
//        RI = 0;
//        RX1_Buffer[RX1_Cnt] = SBUF;
//        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
//    }

//    if(TI)
//    {
//        TI = 0;
//        B_TX1_Busy = 0;
//    }
    
    if(UR1TOSR & 0x01)
    {
        P42 = !P42;
        B_RX1_TimeOut = 1;
        UR1TOSR = 0x80; //���� RTOCF �����ʱ��־λ TOIF
    }
}

//========================================================================
// ����: void UART1_DMA_Interrupt (void) interrupt 50/51
// ����: UART1 DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
//void UART1_DMA_Interrupt(void) interrupt 13
//{
//    if (DMA_UR1T_STA & 0x01)    //�������
//    {
//        DMA_UR1T_STA &= ~0x01;
//        DmaTxFlag = 1;
//    }
//    if (DMA_UR1T_STA & 0x04)    //���ݸ���
//    {
//        DMA_UR1T_STA &= ~0x04;
//    }
//    
//    if (DMA_UR1R_STA & 0x01)    //�������
//    {
//        DMA_UR1R_STA &= ~0x01;
//        DmaRxFlag = 1;
//    }
//    if (DMA_UR1R_STA & 0x02)    //���ݶ���
//    {
//        DMA_UR1R_STA &= ~0x02;
//    }
//}
