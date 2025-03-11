/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

����2���洢���ռ䣬һ������һ�����գ��ֱ��ʼ��2���洢���ռ����ݡ�

����M2M DMA���ϵ���Զ������ʹ洢�������д�뵽���մ洢���ռ�.

���ݲ�ͬ�Ķ�ȡ˳��д��˳�򣬽��յ���ͬ�����ݽ��.

ͨ������1(P3.0 P3.1)��ӡ���մ洢������(�ϵ��ӡһ��).

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

#define MAIN_Fosc     22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�
#define Baudrate      115200L
#define TM            (65536 -(MAIN_Fosc/Baudrate/4))

bit DmaFlag;

u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

void DMA_Config(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
    S1_S1 = 0;      //UART1 switch to, 00: P3.0 P3.1, 01: P3.6 P3.7, 10: P1.6 P1.7, 11: P4.3 P4.4
    S1_S0 = 0;
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //��ʱ��ʱ��1Tģʽ
    S1BRT = 0;      //����1ѡ��ʱ��1Ϊ�����ʷ�����
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //��ʱ��1��ʼ��ʱ
}

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

    for(i=0; i<256; i++)
    {
        DmaTxBuffer[i] = i;
        DmaRxBuffer[i] = 0;
    }

    UartInit();
    printf("AI8051U Memory to Memory DMA Test Programme!\r\n");  //UART1����һ���ַ���

    DMA_Config();
    EA = 1;     //�������ж�

    DmaFlag = 0;

    while (1)
    {
        if(DmaFlag)
        {
            DmaFlag = 0;

            for(i=0; i<256; i++)
            {
                printf("%02X ", DmaRxBuffer[i]);
                if((i & 0x0f) == 0x0f)
                    printf("\r\n");
            }
        }
    }
}

//========================================================================
// ����: void DMA_Config(void)
// ����: UART DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_M2M_CFG = 0x80;     //r++ = t++
    DMA_M2M_STA = 0x00;
    DMA_M2M_AMT = 0x7f;     //���ô������ֽ�����n+1
    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//    DMA_M2M_CFG = 0xa0;     //r++ = t--
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //���ô������ֽ�����n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer);

//    DMA_M2M_CFG = 0x90;     //r-- = t++
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //���ô������ֽ�����n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);

//    DMA_M2M_CFG = 0xb0;     //r-- = t--
//    DMA_M2M_STA = 0x00;
//    DMA_M2M_AMT = 0x7f;     //���ô������ֽ�����n+1
//    DMA_M2M_TXAH = (u8)((u16)&DmaTxBuffer[255] >> 8);
//    DMA_M2M_TXAL = (u8)((u16)&DmaTxBuffer[255]);
//    DMA_M2M_RXAH = (u8)((u16)&DmaRxBuffer[255] >> 8);
//    DMA_M2M_RXAL = (u8)((u16)&DmaRxBuffer[255]);
    
//    DMA_M2M_CFG |= 0x0c;    //�����ж����ȼ�
    DMA_M2M_CR = 0xc0;        //bit7 1:ʹ�� M2M_DMA, bit6 1:��ʼ M2M_DMA �Զ�����
}

//========================================================================
// ����: void M2M_DMA_Interrupt (void) interrupt 47
// ����: M2M DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void M2M_DMA_Interrupt(void) interrupt 13
{
    if (DMA_M2M_STA & 0x01)    //�������
    {
        DMA_M2M_STA &= ~0x01;
        DmaFlag = 1;
    }
}
