/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "system.h"
#include "stdio.h"
#include "uart.h"
#include "spi_flash.h"

/*************  ���س�������    **************/

#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart     1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

/*************  ���ر�������    **************/

bit    UartDmaTxFlag;
bit    UartDmaRxFlag;
bit    UartTimeoutFlag;

/******************** ���ڴ�ӡ���� ********************/

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
    SBUF = dat; 
    while(TI==0);
    TI = 0;
#else
    S2BUF  = dat; 
    while((S2CON & 2) == 0);
    S2CON &= ~2;    //Clear Tx flag
#endif
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}

//========================================================================
// ����: void UART1_DMA_Config(void)
// ����: UART1 DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void UART1_DMA_Config(void)
{
//    DMA_UR1T_CR = 0x00;            //bit7 1:ʹ�� UART1_DMA, bit6 1:��ʼ UART1_DMA �Զ�����
//    DMA_UR1T_CFG = 0x80;        //bit7 1:Enable Interrupt
//    DMA_UR1T_STA = 0x00;
//    DMA_UR1T_AMT = (u8)(DMA_WR_LEN-1);         //���ô������ֽ���(��8λ)��n+1
//    DMA_UR1T_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //���ô������ֽ���(��8λ)��n+1
//    DMA_UR1T_TXAH = (u8)((u16)&DmaBuffer1 >> 8);
//    DMA_UR1T_TXAL = (u8)((u16)&DmaBuffer1);
//    DMA_UR1T_CR = 0x80;            //bit7 1:ʹ�� UART1_DMA, bit6 1:��ʼ UART1_DMA �Զ�����

    //�رս���DMA���´ν��յ��������´������ʼ��ַλ�ã������´ν������ݼ����������š�
    DMA_UR1R_CR = 0x00;            //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
    DMA_UR1R_CFG = 0x80;        //bit7 1:Enable Interrupt
    DMA_UR1R_STA = 0x00;
    DMA_UR1R_AMT = (u8)(DMA_WR_LEN-1);         //���ô������ֽ���(��8λ)��n+1
    DMA_UR1R_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //���ô������ֽ���(��8λ)��n+1
    DMA_UR1_ITVH = 0x00;    //DMA������ʱ��
    DMA_UR1_ITVL = 0x00;
    DMA_UR1R_CR = 0x81;            //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
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
    T2_CT = 0;    //Timer2 set As Timer
    T2x12 = 1;    //Timer2 set as 1T mode
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
        S1BRT = 1;    //S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)(65536UL - (MAIN_Fosc / 4) / Baudrate));
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;        //S1 BRT Use Timer1;
        T1_CT = 0;        //Timer1 set As Timer
        T1x12 = 1;        //Timer1 set as 1T mode
        TMOD &= ~0x30;  //Timer1_16bitAutoReload;
        TH1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) / 256);
        TL1 = (u8)((65536UL - (MAIN_Fosc / 4) / Baudrate) % 256);
        ET1 = 0;    //��ֹ�ж�
        TR1  = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
//    ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

//    UR1TOCR = 0xC0; //������ʱ�жϣ�SCALE(bit5) = 0:ʹ�ò�����bitʱ�����ڣ� 1:ʹ��ϵͳʱ������
//    UR1TOTL = 0x80; //���ó�ʱʱ�䣺128��λʱ������=(1000000/115200)*128us
//    UR1TOTH = 0x00;
//    UR1TOTE = 0x00; //д��TE��ʱʱ��Ż���Ч

//    UartTimeoutFlag = 0;
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
        RI = 0;
    }

//    if(TI)
//    {
//        TI = 0;
//    }

//    if(UR1TOSR & 0x01)  //�ж��Ƿ������ʱ�ж�
//    {
//        UartTimeoutFlag = 1;
//        UR1TOSR = 0x80; //���� RTOCF �����ʱ��־λ TOIF
//    }
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
void UART1T_DMA_Interrupt(void) interrupt 50   //�ж������ų���31�����뱨��Ļ��谲װ�����̰���Ŀ¼�µģ�Keil�ж���������չ���
{
    if (DMA_UR1T_STA & 0x01)    //�������
    {
        DMA_UR1T_STA &= ~0x01;
        UartDmaTxFlag = 1;
    }
    if (DMA_UR1T_STA & 0x04)    //���ݸ���
    {
        DMA_UR1T_STA &= ~0x04;
    }
}

void UART1R_DMA_Interrupt(void) interrupt 51   //�ж������ų���31�����뱨��Ļ��谲װ�����̰���Ŀ¼�µģ�Keil�ж���������չ���
{
    if (DMA_UR1R_STA & 0x01)    //�������
    {
        DMA_UR1R_STA &= ~0x01;
        UartDmaRxFlag = 1;
        SpiDmaFlag = 0;
    }
    if (DMA_UR1R_STA & 0x02)    //���ݶ���
    {
        DMA_UR1R_STA &= ~0x02;
    }
}