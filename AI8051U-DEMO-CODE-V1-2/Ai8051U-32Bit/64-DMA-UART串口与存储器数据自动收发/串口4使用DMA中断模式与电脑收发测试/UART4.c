/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

����4ȫ˫���жϷ�ʽ�շ�ͨѶ����

ͨ��PC��MCU��������, MCU���յ��������Զ�����DMA�ռ�.

������DMA���������ݲ�����ʱ�жϺ�ͨ�����ڵ�DMA�Զ����͹��ܰѴ洢�ռ������ԭ������.

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc   22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�
#define Baudrate4   115200L

bit DmaTxFlag;
bit DmaRxFlag;
bit B_RX4_TimeOut;

u8 xdata DmaBuffer[256];

void UART4_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
void DMA_Config(void);

void UartPutc(unsigned char dat)
{
    S4BUF = dat; 
    while(S4TI == 0);
    S4TI = 0;
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
// ����: 2024-07-28
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

    for(i=0; i<256; i++)
    {
        DmaBuffer[i] = i;
    }

    UART4_config(0);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
    printf("AI8051U UART4 Test Programme!\r\n");  //UART4����һ���ַ���

    DMA_Config();
    EA = 1; //����ȫ���ж�

    DmaTxFlag = 0;
    DmaRxFlag = 0;
    
    while (1)
    {
        if(B_RX4_TimeOut)
        {
            B_RX4_TimeOut = 0;

            i = ((u16)DMA_UR4R_DONEH << 8) + DMA_UR4R_DONE;
            i--;
            DMA_UR4T_AMT = (u8)i;       //���ô������ֽ���(��8λ)��i+1
            DMA_UR4T_AMTH = (u8)(i>>8); //���ô������ֽ���(��8λ)��i+1

            //�رս���DMA���´ν��յ��������´������ʼ��ַλ�ã������´ν������ݼ����������š�ͬʱ����� DMA_UR4R_DONE
            DMA_UR4R_CR = 0;
//            printf("cnt=%u\r\n",i);
            DMA_UR4T_CR = 0xc0;         //bit7 1:ʹ�� UART4_DMA, bit6 1:��ʼ UART4_DMA �Զ�����
            DMA_UR4R_CR = 0xa1;         //bit7 1:ʹ�� UART4_DMA, bit5 1:��ʼ UART4_DMA �Զ�����, bit0 1:��� FIFO
        }

//        if((DmaTxFlag) && (DmaRxFlag))
//        {
//            DmaTxFlag = 0;
//            DMA_UR4T_CR = 0xc0;     //bit7 1:ʹ�� UART4_DMA, bit6 1:��ʼ UART4_DMA �Զ�����
//            DmaRxFlag = 0;
//            DMA_UR4R_CR = 0xa1;     //bit7 1:ʹ�� UART4_DMA, bit5 1:��ʼ UART4_DMA �Զ�����, bit0 1:��� FIFO
//        }
    }
}

//========================================================================
// ����: void DMA_Config(void)
// ����: UART DMA �������á�
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-07-28
// ��ע: 
//========================================================================
void DMA_Config(void)
{
    DMA_UR4T_CFG = 0x00;    //bit7 1:Enable Interrupt
    DMA_UR4T_STA = 0x00;
    DMA_UR4T_AMT = 0xff;    //���ô������ֽ���(��8λ)��n+1
    DMA_UR4T_AMTH = 0x00;   //���ô������ֽ���(��8λ)��n+1
    DMA_UR4T_TXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR4T_TXAL = (u8)((u16)&DmaBuffer);
    DMA_UR4T_CR = 0xc0;     //bit7 1:ʹ�� UART4_DMA, bit6 1:��ʼ UART4_DMA �Զ�����

    DMA_UR4R_CFG = 0x00;    //bit7 1:Enable Interrupt
    DMA_UR4R_STA = 0x00;
    DMA_UR4R_AMT = 0xff;    //���ô������ֽ���(��8λ)��n+1
    DMA_UR4R_AMTH = 0x00;   //���ô������ֽ���(��8λ)��n+1
    DMA_UR4R_RXAH = (u8)((u16)&DmaBuffer >> 8);
    DMA_UR4R_RXAL = (u8)((u16)&DmaBuffer);
    DMA_UR4R_CR = 0xa1;     //bit7 1:ʹ�� UART4_DMA, bit5 1:��ʼ UART4_DMA �Զ�����, bit0 1:��� FIFO
}

//========================================================================
// ����: SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2024-07-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // ʹ��Timer2��������.
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
// ����: void UART4_config(u8 brt)
// ����: UART4��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
// ����: none.
// �汾: VER1.0
// ����: 2024-07-28
// ��ע: 
//========================================================================
void UART4_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer4��������.
{
    if(brt == 2)
    {
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate4));
        S4CON = 0x10;       //8λ����, ʹ��Timer2�������ʷ�����, �������
    }
    else
    {
        T4R = 0;    //Timer stop
        S4CON = 0x50;   //8λ����, ʹ��Timer4�������ʷ�����, �������
        T4H = (65536UL - (MAIN_Fosc / 4) / Baudrate4) / 256;
        T4L = (65536UL - (MAIN_Fosc / 4) / Baudrate4) % 256;
        T4_CT = 0;  //Timer4 set As Timer
        T4x12 = 1;  //Timer4 set as 1T mode
        T4R = 1;    //Timer run enable
    }
//    ES4  = 1;       //����UART4�ж�
    S4_S = 0;       //UART4 switch bit2 to: 0: P0.2 P0.3, 1: P5.2 P5.3

    UR4TOCR = 0xc0; //bit7:ʹ�ܳ�ʱ���գ�bit6:ʹ�ܳ�ʱ�жϣ�bit5:��ʱʱ��ѡ�� 1:ϵͳʱ��  0:��������λ��(������)
    UR4TOTL = 0x40;
    UR4TOTH = 0x00; //0x5666 = 22118
    UR4TOTE = 0x00; //��Ҫд UR4TOTE ���µ�TOֵ�Ż���Ч
}

//========================================================================
// ����: void UART4_int (void) interrupt UART4_VECTOR
// ����: UART4�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2024-07-28
// ��ע: 
//========================================================================
void UART4_int (void) interrupt 18
{
//    if(S4RI)
//    {
//        S4RI = 0;    //Clear Rx flag
//        RX4_Buffer[RX4_Cnt] = S4BUF;
//        if(++RX4_Cnt >= UART4_BUF_LENGTH)   RX4_Cnt = 0;
//    }

//    if(S4TI)
//    {
//        S4TI = 0;    //Clear Tx flag
//        B_TX4_Busy = 0;
//    }

    if(UR4TOSR & 0x01)
    {
        B_RX4_TimeOut = 1;
        UR4TOSR = 0x80; //���� RTOCF �����ʱ��־λ TOIF
    }
}

//========================================================================
// ����: void UART4_DMA_Interrupt (void) interrupt 56/57
// ����: UART4 DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2024-07-28
// ��ע: 
//========================================================================
void UART4_DMA_Interrupt(void) interrupt 13
{
    if (DMA_UR4T_STA & 0x01)    //�������
    {
        DMA_UR4T_STA &= ~0x01;
        DmaTxFlag = 1;
    }
    if (DMA_UR4T_STA & 0x04)    //���ݸ���
    {
        DMA_UR4T_STA &= ~0x04;
    }
    
    if (DMA_UR4R_STA & 0x01)    //�������
    {
        DMA_UR4R_STA &= ~0x01;
        DmaRxFlag = 1;
    }
    if (DMA_UR4R_STA & 0x02)    //���ݶ���
    {
        DMA_UR4R_STA &= ~0x02;
    }
}
