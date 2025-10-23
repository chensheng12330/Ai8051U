/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************    ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

0~7ͨ����ӦP1.0~P1.7, 8~14ͨ����ӦP0.0~P0.6, 15ͨ��Ϊ�ڲ�1.19V��׼��ѹ�������ADCֵ.

��ʼ��ʱ�Ȱ�ҪADCת������������Ϊ��������.

����������������(DMA)���ܣ�����ͨ��һ��ѭ���ɼ��������Զ���ŵ�DMA�����xdata�ռ�.

ͨ������1(P3.0 P3.1)��DMA�����xdata�ռ����յ������ݷ��͸���λ����������115200,N,8,1

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char    u8;
typedef     unsigned int    u16;
typedef     unsigned long    u32;

/*************    ���س�������    **************/

#define MAIN_Fosc   22118400L    //������ʱ��
#define Baudrate    115200L
#define TM          (65536 -(MAIN_Fosc/Baudrate/4))

#define ADC_SPEED   15      /* 0~15, ADCת��ʱ��(CPUʱ����) = (n+1)*32  ADCCFG */
#define RES_FMT     (1<<5)  /* ADC�����ʽ 0: �����, ADC_RES: D11 D10 D9 D8 D7 D6 D5 D4, ADC_RESL: D3 D2 D1 D0 0 0 0 0 */
                            /* ADCCFG     1: �Ҷ���, ADC_RES: 0 0 0 0 D11 D10 D9 D8, ADC_RESL: D7 D6 D5 D4 D3 D2 D1 D0 */

#define ADC_CH      16      /* 1~16, ADCת��ͨ����, ��ͬ���޸� DMA_ADC_CHSW ת��ͨ�� */
#define ADC_DATA    12      /* 6~n, ÿ��ͨ��ADCת����������, 2*ת������+4, ��ͬ���޸� DMA_ADC_CFG2 ת������ */
#define DMA_ADDR    0x800   /* DMA���ݴ�ŵ�ַ */

/*************    ���ر�������    **************/

bit DmaFlag;

u8 xdata DmaBuffer[ADC_CH][ADC_DATA] _at_ DMA_ADDR;

/*************    ���غ�������    **************/

void delay_ms(u8 ms);
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

/**********************************************/
void main(void)
{
    u8 i,n;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    P0M1 = 0x7f;   P0M0 = 0x00;   //����Ϊ��������
    P1M1 = 0xff;   P1M0 = 0x00;   //����Ϊ��������
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
    ADCTIM = 0x3f;  //����ͨ��ѡ��ʱ�䡢����ʱ�䡢����ʱ��
    ADCCFG = RES_FMT + ADC_SPEED;
    //ADCģ���Դ�򿪺���ȴ�1ms��MCU�ڲ�ADC��Դ�ȶ����ٽ���ADת��
    ADC_CONTR = 0x80 + 0;    //ADC on + channel

    UartInit();
    DMA_Config();
    EA = 1;
    printf("AI8051Uϵ��ADC DMA���Գ���!\r\n");

    while (1)
    {
        delay_ms(200);
        if(DmaFlag)
        {
            DmaFlag = 0;
            for(i=0; i<ADC_CH; i++)
            {
                for(n=0; n<ADC_DATA; n++)
                {
                    printf("0x%02x ",DmaBuffer[i][n]);
                }
                printf("\r\n");
            }
            printf("\r\n");
            DMA_ADC_CR = 0xc0;  //bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
        }
    }
}

//========================================================================
// ����: void DMA_Config(void)
// ����: ADC DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_ADC_STA = 0x00;
    DMA_ADC_CFG = 0x80;     //bit7 1:Enable Interrupt
    DMA_ADC_RXAH = (u8)(DMA_ADDR >> 8);    //ADCת�����ݴ洢��ַ
    DMA_ADC_RXAL = (u8)DMA_ADDR;
    DMA_ADC_CFG2 = 0x09;    //ÿ��ͨ��ADCת������:4
    DMA_ADC_CHSW0 = 0xff;   //ADCͨ��ʹ�ܼĴ��� ADC7~ADC0
    DMA_ADC_CHSW1 = 0xff;   //ADCͨ��ʹ�ܼĴ��� ADC15~ADC8
    DMA_ADC_CR = 0xc0;      //bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
}

//========================================================================
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u8 ms)
{
    u16 i;
    do
    {
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

//========================================================================
// ����: void ADC_DMA_Interrupt (void) interrupt 48
// ����: ADC DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void ADC_DMA_Interrupt(void) interrupt 13
{
    DMA_ADC_STA = 0;
    DmaFlag = 1;
}
