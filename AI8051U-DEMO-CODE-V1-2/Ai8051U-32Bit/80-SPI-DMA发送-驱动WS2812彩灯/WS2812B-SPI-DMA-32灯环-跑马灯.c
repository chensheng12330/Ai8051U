
#define     MAIN_Fosc        25600000UL    //������ʱ��

#include    "AI8051U.h"


/*************    ����˵��    **************

���ȱ��޸ĳ���, ֱ������"SPI.hex"����, ����ʱѡ����Ƶ24MHz������25.6MHz. ʵ��20~30MHz��������������

ʹ��SPI-MOSI���ֱ������WS2812����ɫ�ʵ�, DMA���䣬��ռ��CPUʱ�䡣��������32���ƣ��ӳɻ�״��
����ʹ��USART1-SPIģʽ��P1.5-MOSI��������ź�ֱ������WS2812��
����ʹ��SPI����ģʽ����P1.6��P1.7��SPIռ�ã����������á�
DMA����ʱ�䣺@25.6MHz, DMA����һ���ֽں�ʱ67T(2.6172us)��һ����Ҫ����12���ֽں�ʱ31.5us��32K�ֽ�xdata���һ������2730����.

ÿ����3���ֽڣ��ֱ��Ӧ�̡��졢����MSB�ȷ�.
800KHz����, ����0(1/4ռ�ձ�): H=0.3125us  L=0.9375us, ����1(3/4ռ�ձ�): H=0.9375us  L=0.3125us, RESET>=50us.
�ߵ�ƽʱ��Ҫ��ȷ������Ҫ��ķ�Χ��, �͵�ƽʱ�䲻��Ҫ��ȷ����, ����Ҫ�����Сֵ��С��RES��50us����.

WS2812S�ı�׼ʱ������:
TH+TL = 1.25us��150ns, RES>50us
T0H = 0.25us��150ns = 0.10us - 0.40us
T0L = 1.00us��150ns = 0.85us - 1.15us
T1H = 1.00us��150ns = 0.85us - 1.15us
T1L = 0.25us��150ns = 0.10us - 0.40us
����λ����֮��ļ��ҪС��RES��50us.

SPI����:
����ʹ��SPIģʽ��P1.5-MOSI��������ź�ֱ������WS2812��
����ʹ��SPI����ģʽ����P1.6��P1.7��SPIռ�ã����������á�

��SPI����, �ٶ�3.0~3.5MHz����3.2MHzΪ���, MSB�ȷ�, ÿ���ֽڸ�4λ�͵�4λ�ֱ��Ӧһ��λ����, 1000Ϊ����0, 1110Ϊ����1.
SPI����λ       D7 D6 D5 D4    D3 D2 D1 D0
SPI����         1  0  0  0     1  1  1  0
               WS2812����0    WS2812����1
SPI���ݸ߰��ֽڶ�Ӧ��WS2812����0-->0x80, ����1-->0xe0,
SPI���ݵͰ��ֽڶ�Ӧ��WS2812����0-->0x08, ����1-->0x0e,
��Ƶ25.6MHz, SPI��Ƶ8 = 3.2MHz. ���.

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/


/*************    ���غ�������    **************/

#define COLOR    50                //���ȣ����255

#define LED_NUM    32              //LED�Ƹ���
#define SPI_NUM    (LED_NUM*12)    //LED�ƶ�ӦSPI�ֽ���

u8 xdata  led_RGB[LED_NUM][3];    //LED��Ӧ��RGB��led_buff[i][0]-->�̣�led_buff[i][1]-->�죬led_buff[i][0]-->��.
u8 xdata  led_SPI[SPI_NUM];       //LED�ƶ�ӦSPI�ֽ���

/*************  �ⲿ�����ͱ������� *****************/
void SPI_Config(u8 SPI_io, u8 SPI_speed);    //SPI��ʼ������, ����:  SPI_io: �л�����IO, 0: �л���P5.4 P1.3 P1.4 P1.5,  1: �л���P2.2 P2.3 P2.4 P2.5, 2: �л���P5.4 P4.0 P4.1 P4.3,  3: �л���P3.5 P3.4 P3.3 P3.2,
                                                //                        SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
void LoadSPI(void);
void SPI_DMA_TxTRIG(u8 xdata *TxBuf, u16 num);
void delay_ms(u16 ms);

/*************** ������ *******************************/

void main(void)
{
    u16    i,k;
    u8    xdata *px;

    EAXFR = 1;    //���������չ�Ĵ���
    WTST  = 0;
    CKCON = 0;
//    MCLKO47_DIV(100);    //��Ƶ��Ƶ��������ڲ�����Ƶ
    P1M0=0; P1M1 = 0;

    SPI_Config(0, 1);    //(SPI_io, SPI_speed), ����:     SPI_io: �л�IO(SS MOSI MISO SCLK), 0: �л���P1.4 P1.5 P1.6 P1.7,  1: �л���P2.4 P2.5 P2.6 P2.7, 2: �л���P4.0 P4.1 P4.2 P4.3,  3: �л���P3.5 P3.4 P3.3 P3.2,
                        //                                SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
    P1n_pure_input(Pin6);            // MISO-P1.6����Ϊ���裬�������������裬����SPI������ɺ�MOSI�ǵ͵�ƽ��
    PullUpDisable(P1PU, Pin6);       // ��ֹ�˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
    PullDownEnable(P1PD, Pin6);      // ����˿��ڲ���������   PxPD, Ҫ���õĶ˿ڶ�ӦλΪ1
    EA = 1;

    k = 0;

    while (1)
    {
        px = &led_RGB[0][0];    //����(��ɫ)�׵�ַ
        for(i=0; i<(LED_NUM*3); i++, px++)    *px = 0;    //������е���ɫ

        i = k;
        led_RGB[i][1] = COLOR;        //��ɫ
        if(++i >= LED_NUM)    i = 0;  //��һ����
        led_RGB[i][0] = COLOR;        //��ɫ
        if(++i >= LED_NUM)    i = 0;  //��һ����
        led_RGB[i][2] = COLOR;        //��ɫ

        LoadSPI();    //����ɫװ�ص�SPI����
        SPI_DMA_TxTRIG(led_SPI, SPI_NUM);    //u8 xdata *TxBuf, u16 num), ����SPI DMA, 720�ֽ�һ����ʱ2.08ms @25.6MHz

        if(++k >= LED_NUM)    k = 0;            //˳ʱ��
    //    if(--k >= LED_NUM)    k = LED_NUM-1;    //��ʱ��
        delay_ms(50);
    }
}

//================ N ms��ʱ���� ==============
void  delay_ms(u16 ms)
{
    u16 i;
    do
    {
        i = MAIN_Fosc / 6000;
        while(--i)    ;
    }while(--ms);
}

//================ ����ɫװ�ص�SPI���� ==============
void LoadSPI(void)
{
    u8    xdata *px;
    u16   i,j;
    u8    k;
    u8    dat;

    for(i=0; i<SPI_NUM; i++)    led_SPI[i] = 0;
    px = &led_RGB[0][0];    //�׵�ַ
    for(i=0, j=0; i<(LED_NUM*3); i++)
    {
        dat = *px;
        px++;
        for(k=0; k<4; k++)
        {
            if(dat & 0x80)  led_SPI[j]  = 0xE0;    //����1
            else            led_SPI[j]  = 0x80;    //����0
            if(dat & 0x40)  led_SPI[j] |= 0x0E;    //����1
            else            led_SPI[j] |= 0x08;    //����0
            dat <<= 2;
            j++;
        }
    }
}

//========================================================================
// ����: void SPI_Config(u8 io, u8 speed)
// ����: SPI��ʼ��������
// ����: io: �л�����IO,            SS  MOSI MISO SCLK
//                       0: �л��� P1.4 P1.5 P1.6 P1.7
//                       1: �л��� P2.4 P2.5 P2.6 P2.7
//                       2: �л��� P4.0 P4.1 P4.2 P4.3
//                       3: �л��� P3.5 P3.4 P3.3 P3.2
//       SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
// ����: none.
// �汾: VER1.0
// ����: 2018-6-13
// ��ע:
//========================================================================

void SPI_Config(u8 SPI_io, u8 SPI_speed)
{
    SPCTL = SPI_speed & 3;    //����SPI �ٶ�, ����ָ����ڵ�һλ, Bit7~Bit2��0
    SSIG = 1;    //1: ����SS�ţ���MSTRλ�����������Ǵӻ�  0: SS�����ھ����������Ǵӻ���
    SPEN = 1;    //1: ����SPI��                          0����ֹSPI������SPI�ܽž�Ϊ��ͨIO
    DORD = 0;    //1��LSB�ȷ���                          0��MSB�ȷ�
    MSTR = 1;    //1����Ϊ����                           0����Ϊ�ӻ�
    CPOL = 0;    //1: ����ʱSCLKΪ�ߵ�ƽ��               0������ʱSCLKΪ�͵�ƽ
    CPHA = 0;    //1: ������SCLKǰ������,���ز���.        0: ������SCLKǰ�ز���,��������.
    P_SW1 = (P_SW1 & ~0x0c) | ((SPI_io<<2) & 0x0c);      //�л�IO

    HSCLKDIV   = 1;        //HSCLKDIV��ʱ�ӷ�Ƶ
    SPI_CLKDIV = 1;        //SPI_CLKDIV��ʱ�ӷ�Ƶ
    if(SPI_io == 0)
    {
        P1n_standard(0xf0);             //�л��� P1.4(SS) P1.5(MOSI) P1.6(MISO) P1.7(SCLK), ����Ϊ׼˫���
        PullUpEnable(P1PU, 0xf0);       //������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
        P1n_push_pull(Pin7+Pin5);       //MOSI SCLK����Ϊ�������
        SlewRateHigh(P1SR, Pin7+Pin5);  //MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
    }
    else if(SPI_io == 1)
    {
        P2n_standard(0xf0);            //�л���P2.4(SS) P2.5(MOSI) P2.6(MISO) P2.7(SCLK), ����Ϊ׼˫���
        PullUpEnable(P2PU, 0xf0);       //������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
        P2n_push_pull(Pin7+Pin5);       //MOSI SCLK����Ϊ�������
        SlewRateHigh(P2SR, Pin7+Pin5);  //MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
    }
    else if(SPI_io == 2)
    {
        P4n_standard(0x0f);         //�л���P4.0(SS) P4.1(MOSI) P4.2(MISO) P4.3(SCLK), ����Ϊ׼˫���
        PullUpEnable(P4PU, 0x0f);   //������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
        P4n_push_pull(Pin3+Pin1);   //MOSI SCLK����Ϊ�������
        SlewRateHigh(P4SR, Pin3+Pin1);  //MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
    }
    else if(SPI_io == 3)
    {
        P3n_standard(0x3C);         //�л���P3.5(SS) P3.4(MOSI) P3.3(MISO) P3.2(SCLK), ����Ϊ׼˫���
        PullUpEnable(P3PU, 0x3c);   //������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
        P3n_push_pull(Pin4+Pin2);   //MOSI SCLK����Ϊ�������
        SlewRateHigh(P3SR, Pin4+Pin2);  //MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
    }
}


//DMA_SPI_CR     SPI_DMA���ƼĴ���
#define        DMA_ENSPI    (1<<7)    // SPI DMA����ʹ�ܿ���λ��    bit7, 0:��ֹSPI DMA���ܣ�  1������SPI DMA���ܡ�
#define        SPI_TRIG_M   (1<<6)    // SPI DMA����ģʽ��������λ��bit6, 0:д0��Ч��          1��д1��ʼSPI DMA����ģʽ������
#define        SPI_TRIG_S   (1<<5)    // SPI DMA�ӻ�ģʽ��������λ��bit5, 0:д0��Ч��          1��д1��ʼSPI DMA�ӻ�ģʽ������
#define        SPI_CLRFIFO       1    // ���SPI DMA����FIFO����λ��bit0, 0:д0��Ч��          1��д1��λFIFOָ�롣


//DMA_SPI_CFG     SPI_DMA���üĴ���
#define        DMA_SPIIE    (1<<7)    // SPI DMA�ж�ʹ�ܿ���λ��bit7, 0:��ֹSPI DMA�жϣ�     1�������жϡ�
#define        SPI_ACT_TX   (1<<6)    // SPI DMA�������ݿ���λ��bit6, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1�������͡�
#define        SPI_ACT_RX   (1<<5)    // SPI DMA�������ݿ���λ��bit5, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1��������ա�
#define        DMA_SPIIP    (0<<2)    // SPI DMA�ж����ȼ�����λ��bit3~bit2, (���)0~3(���).
#define        DMA_SPIPTY        0    // SPI DMA�������߷������ȼ�����λ��bit1~bit0, (���)0~3(���).

//DMA_SPI_CFG2     SPI_DMA���üĴ���2
#define        SPI_WRPSS    (0<<2)    // SPI DMA������ʹ��SS�ſ���λ��bit2, 0: SPI DMA������̲��Զ�����SS�š�  1���Զ�����SS�š�
#define        SPI_SSS           0    // SPI DMA�������Զ�����SS��ѡ��λ��bit1~bit0, 0: P1.4,  1��P2.4,  2: P4.0,  3:P3.5��

//DMA_SPI_STA     SPI_DMA״̬�Ĵ���
#define        SPI_TXOVW    (1<<2)    // SPI DMA���ݸ��Ǳ�־λ��bit2, �����0.
#define        SPI_RXLOSS   (1<<1)    // SPI DMA�������ݶ�����־λ��bit1, �����0.
#define        DMA_SPIIF         1    // SPI DMA�ж������־λ��bit0, �����0.

//HSSPI_CFG  ����SPI���üĴ���
#define        SS_HOLD      (0<<4)    //����ģʽʱSS�����źŵ�HOLDʱ�䣬 0~15, Ĭ��3. ��DMA�л�����N��ϵͳʱ�ӣ���SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.
#define        SS_SETUP          3    //����ģʽʱSS�����źŵ�SETUPʱ�䣬0~15, Ĭ��3. ��DMA�в�Ӱ��ʱ�䣬       ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.

//HSSPI_CFG2  ����SPI���üĴ���2
#define        SPI_IOSW     (1<<6)    //bit6:����MOSI��MISO��λ��0����������1������
#define        HSSPIEN      (1<<5)    //bit5:����SPIʹ��λ��0���رո���ģʽ��1��ʹ�ܸ���ģʽ
#define        FIFOEN       (1<<4)    //bit4:����SPI��FIFOģʽʹ��λ��0���ر�FIFOģʽ��1��ʹ��FIFOģʽ��ʹ��FIFOģʽ��DMA�м���13��ϵͳʱ�䡣
#define        SS_DACT           3    //bit3~0:����ģʽʱSS�����źŵ�DEACTIVEʱ�䣬0~15, Ĭ��3. ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.

/*****************************************************************************
 * @name       :void SPI_DMA_TxTRIG(u8 xdata *TxBuf, u16 num)
 * @date       :2024-1-5
 * @function   :Config SPI DMA
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void SPI_DMA_TxTRIG(u8 xdata *TxBuf, u16 num)
{
    u16 j;
    HSSPI_CFG  = SS_HOLD | SS_SETUP;    //SS_HOLD������N��ϵͳʱ��, SS_SETUPû������ʱ�ӡ�
    HSSPI_CFG2 = HSSPIEN | FIFOEN | SS_DACT;    //FIFOEN����FIFO���С13��ʱ��, 67T @8��Ƶ.

    j = (u16)TxBuf;        //ȡ�׵�ַ
    DMA_SPI_TXAH = (u8)(j >> 8);        //���͵�ַ�Ĵ������ֽ�
    DMA_SPI_TXAL = (u8)j;               //���͵�ַ�Ĵ������ֽ�
    DMA_SPI_AMTH = (u8)((num-1)/256);   //���ô������ֽ��� = n+1
    DMA_SPI_AMT  = (u8)((num-1)%256);   //���ô������ֽ��� = n+1
    DMA_SPI_ITVH = 0;                   //���ӵļ��ʱ�䣬N+1��ϵͳʱ��
    DMA_SPI_ITVL = 0;
    DMA_SPI_STA  = 0x00;
    DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_TX | DMA_SPIIP | DMA_SPIPTY;
    DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
    DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_CLRFIFO;
    P10 = 1;
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
    P10 = 0;
    DMA_SPI_STA = 0;        //����жϱ�־
}
