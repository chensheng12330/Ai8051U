/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ�����ڶ�SPI Flash���ж�д���ԡ�

��FLASH������������д�롢�����Ĳ���������ָ����ַ��

����1(P3.0 P3.1)Ĭ�ϲ�����:  115200,8,N,1.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ����������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ����������ָ��ʮ�����Ƶ�ַ������Ϊ�����ֽ���. 
    C                       --> �����ⲻ��Flash, ����Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef     unsigned char    u8;
typedef     unsigned int     u16;
typedef     unsigned long    u32;

/****************************** �û������ ***********************************/

#define MAIN_Fosc           24000000L   //������ʱ��

#define Baudrate            115200L
#define TM                  (65536 -(MAIN_Fosc/Baudrate/4))

#define EE_BUF_LENGTH       50
#define UART1_BUF_LENGTH    (EE_BUF_LENGTH+9)   //���ڻ��峤��

/*****************************************************************************/


/*************  ���س�������    **************/

#define HSCK_MCLK       0
#define HSCK_PLL        1
#define HSCK_SEL        HSCK_PLL

#define PLL_96M         0
#define PLL_144M        1
#define PLL_SEL         PLL_96M

#define CKMS            0x80
#define HSIOCK          0x40
#define MCK2SEL_MSK     0x0c
#define MCK2SEL_SEL1    0x00
#define MCK2SEL_PLL     0x04
#define MCK2SEL_PLLD2   0x08
#define MCK2SEL_IRC48   0x0c
#define MCKSEL_MSK      0x03
#define MCKSEL_HIRC     0x00
#define MCKSEL_XOSC     0x01
#define MCKSEL_X32K     0x02
#define MCKSEL_IRC32K   0x03

#define ENCKM           0x80
#define PCKI_MSK        0x60
#define PCKI_D1         0x00
#define PCKI_D2         0x20
#define PCKI_D4         0x40
#define PCKI_D8         0x60

/*************  ���ر�������    **************/

u8  tmp[EE_BUF_LENGTH];
u8  sst_byte;
u32 Flash_addr;

/*************  FLASH��ر�������   **************/
sbit    SPI_CE  = P4^0;     //PIN1
sbit    SPI_SO  = P4^2;     //PIN2
sbit    SPI_SI  = P4^1;     //PIN5
sbit    SPI_SCK = P4^3;     //PIN6

u8  B_FlashOK;                                //Flash״̬
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

u8  RX1_TimeOut;
u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־

u8  RX1_Buffer[UART1_BUF_LENGTH];   //���ջ���

/*************  ���غ�������    **************/
void delay_ms(u8 ms);
void RX1_Check(void);

void SPI_init(void);
void FlashCheckID(void);
u8   CheckFlashBusy(void);
void FlashWriteEnable(void);
void FlashChipErase(void);
void FlashSectorErase(u32 addr);
void SPI_Read_Nbytes( u32 addr, u8 *buffer, u16 size);
u8   SPI_Read_Compare(u32 addr, u8 *buffer, u16 size);
void SPI_Write_Nbytes(u32 addr, u8 *buffer,  u8 size);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //��ʱ��ʱ��1Tģʽ
    S1BRT = 0;      //����1ѡ��ʱ��1Ϊ�����ʷ�����
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //��ʱ��1��ʼ��ʱ
    REN = 1;        //�������
    ES  = 1;        //�����ж�

//  SCON = (SCON & 0x3f) | 0x40; 
//  T2L  = TM;
//  T2H  = TM>>8;
//  AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
}

void UartPutc(unsigned char dat)
{
    B_TX1_Busy = 1;
    SBUF = dat; 
    while(B_TX1_Busy);
}

char putchar(char c)
{
    UartPutc(c);
    return c;
}

/********************* ������ *************************/
void main(void)
{
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

    P4SR = 0xf0;        //��ƽת���ٶȿ죨���IO�ڷ�ת�ٶȣ�
    UartInit();
    EA = 1;             //�����ж�
    
    printf("��������:\r\n");
    printf("E 0x001234            --> ��������  ʮ�����Ƶ�ַ\r\n");
    printf("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�����\r\n");
    printf("C                     --> �����ⲻ��SPI Flash, ����Cǿ���������.\r\n\r\n");

    SPI_init();
    FlashCheckID();
    FlashCheckID();
    
    if(!B_FlashOK)  printf("δ��⵽PM25LV040/W25X40CL/W25Q80BV/W25Q128FV!\r\n");
    else
    {
        if(B_FlashOK == 1)
        {
            printf("��⵽PM25LV040!\r\n");
        }
        else if(B_FlashOK == 2)
        {
            printf("��⵽W25X40CL!\r\n");
        }
        else if(B_FlashOK == 3)
        {
            printf("��⵽W25Q80BV!\r\n");
        }
        else if(B_FlashOK == 4)
        {
            printf("��⵽W25Q128FV!\r\n");
        }
    }
    printf("������ID1 = 0x%02X",PM25LV040_ID1);
    printf("\r\n      ID2 = 0x%02X",PM25LV040_ID2);
    printf("\r\n   �豸ID = 0x%02X\r\n",PM25LV040_ID);

    while(1)
    {
        delay_ms(1);

        if(RX1_TimeOut > 0)
        {
            if(--RX1_TimeOut == 0)  //��ʱ,�򴮿ڽ��ս���
            {
                if(RX1_Cnt > 0)
                {
                    RX1_Check();    //����1��������
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
// ����: 2013-4-1
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

/**************** ASCII��תBIN ****************************/
u8  CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** ��ȡд���ַ ****************************/
u32 GetAddress(void)
{
    u32 address;
    u8  i,j;
    
    address = 0;
    if((RX1_Buffer[2] == '0') && (RX1_Buffer[3] == 'X'))
    {
        for(i=4; i<10; i++)
        {
            j = CheckData(RX1_Buffer[i]);
            if(j >= 0x10)   return 0x80000000;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0x80000000; //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=11; i<RX1_Cnt; i++)
    {
        if(CheckData(RX1_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX1_Buffer[i]);
    }
    return (length);
}

/**************** ���ڴ����� ****************************/

void RX1_Check(void)
{
    u8  i,j;

    if((RX1_Cnt == 1) && (RX1_Buffer[0] == 'C'))    //����Cǿ���������
    {
        B_FlashOK = 1;
        printf("ǿ���������FLASH!\r\n");
    }

    if(!B_FlashOK)
    {
        printf("PM25LV040/W25X40CL/W25Q80BV������, ���ܲ���FLASH!\r\n");
        return;
    }
    
    F0 = 0;
    if((RX1_Cnt >= 10) && (RX1_Buffer[1] == ' '))   //�������Ϊ10���ֽ�
    {
        for(i=0; i<10; i++)
        {
            if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';//Сдת��д
        }
        Flash_addr = GetAddress();
        if(Flash_addr < 0x80000000)
        {
            if(RX1_Buffer[0] == 'E')    //����
            {
                FlashSectorErase(Flash_addr);
                printf("�Ѳ���һ����������!\r\n");
                F0 = 1;
            }

            else if((RX1_Buffer[0] == 'W') && (RX1_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //д��N���ֽ�
            {
                j = RX1_Cnt - 11;
                for(i=0; i<j; i++)  tmp[i] = 0xff;      //���Ҫд��Ŀռ��Ƿ�Ϊ��
                i = SPI_Read_Compare(Flash_addr,tmp,j);
                if(i > 0)
                {
                    printf("Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n");
                }
                else
                {
                    SPI_Write_Nbytes(Flash_addr,&RX1_Buffer[11],j);     //дN���ֽ� 
                    i = SPI_Read_Compare(Flash_addr,&RX1_Buffer[11],j); //�Ƚ�д�������
                    if(i == 0)
                    {
                        printf("��д��");
                        if(j >= 100)    {UartPutc((u8)(j/100+'0'));   j = j % 100;}
                        if(j >= 10)     {UartPutc((u8)(j/10+'0'));    j = j % 10;}
                        UartPutc((u8)(j%10+'0'));
                        printf("�ֽ�����!\r\n");
                    }
                    else        printf("д�����!\r\n");
                }
                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (RX1_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j < EE_BUF_LENGTH))
                {
                    SPI_Read_Nbytes(Flash_addr,tmp,j);
                    printf("����");
                    if(j>=100)  UartPutc((u8)(j/100+'0'));
                    UartPutc((u8)(j%100/10+'0'));
                    UartPutc((u8)(j%10+'0'));
                    printf("���ֽ��������£�\r\n");
                    for(i=0; i<j; i++)  UartPutc(tmp[i]);
                    UartPutc(0x0d);
                    UartPutc(0x0a);
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("�������!\r\n");
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
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;
        B_TX1_Busy = 0;
    }
}

/******************* FLASH��س��� ************************/
#define SFC_WREN        0x06        //����Flash���
#define SFC_WRDI        0x04
#define SFC_RDSR        0x05
#define SFC_WRSR        0x01
#define SFC_READ        0x03
#define SFC_FASTREAD    0x0B
#define SFC_RDID        0xAB
#define SFC_PAGEPROG    0x02
#define SFC_RDCR        0xA1
#define SFC_WRCR        0xF1
#define SFC_SECTORER1   0xD7        //PM25LV040 ��������ָ��
#define SFC_SECTORER2   0x20        //W25Xxx ��������ָ��
#define SFC_BLOCKER     0xD8
#define SFC_CHIPER      0xC7

#define SPI_CE_High()   SPI_CE  = 1     // set CE high
#define SPI_CE_Low()    SPI_CE  = 0     // clear CE low

/************************************************************************/
void SPI_init(void)
{
    //ѡ��PLL���ʱ��
#if (PLL_SEL == PLL_96M)
    CLKSEL &= ~CKMS;            //ѡ��PLL��96M��ΪPLL�����ʱ��
#elif (PLL_SEL == PLL_144M)
    CLKSEL |= CKMS;             //ѡ��PLL��144M��ΪPLL�����ʱ��
#else
    CLKSEL &= ~CKMS;            //Ĭ��ѡ��PLL��96M��ΪPLL�����ʱ��
#endif

    //ѡ��PLL����ʱ�ӷ�Ƶ,��֤����ʱ��Ϊ12M
    USBCLK &= ~PCKI_MSK;
#if (MAIN_Fosc == 12000000UL)
    USBCLK |= PCKI_D1;          //PLL����ʱ��1��Ƶ
#elif (MAIN_Fosc == 24000000UL)
    USBCLK |= PCKI_D2;          //PLL����ʱ��2��Ƶ
#elif (MAIN_Fosc == 48000000UL)
    USBCLK |= PCKI_D4;          //PLL����ʱ��4��Ƶ
#elif (MAIN_Fosc == 96000000UL)
    USBCLK |= PCKI_D8;          //PLL����ʱ��8��Ƶ
#else
    USBCLK |= PCKI_D1;          //Ĭ��PLL����ʱ��1��Ƶ
#endif

    //����PLL
    USBCLK |= ENCKM;            //ʹ��PLL��Ƶ
    delay_ms(1);                //�ȴ�PLL��Ƶ

    //ѡ��HSPWM/HSSPIʱ��
#if (HSCK_SEL == HSCK_MCLK)
    CLKSEL &= ~HSIOCK;          //HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#elif (HSCK_SEL == HSCK_PLL)
    CLKSEL |= HSIOCK;           //HSPWM/HSSPIѡ��PLL���ʱ��Ϊʱ��Դ
#else
    CLKSEL &= ~HSIOCK;          //Ĭ��HSPWM/HSSPIѡ����ʱ��Ϊʱ��Դ
#endif

    HSCLKDIV = 8;               //HSPWM/HSSPIʱ��Դ4��Ƶ

    SSIG = 1; //���� SS ���Ź��ܣ�ʹ�� MSTR ȷ���������������Ǵӻ�
    SPEN = 1; //ʹ�� SPI ����
    DORD = 0; //�ȷ���/�������ݵĸ�λ�� MSB��
    MSTR = 1; //��������ģʽ
    CPOL = 1; //SCLK ����ʱΪ�ߵ�ƽ��SCLK ��ǰʱ����Ϊ�½��أ���ʱ����Ϊ������
    CPHA = 1; //������ SCLK ǰʱ������������ʱ���ز���
    SPCTL = (SPCTL & ~3) | 2;   //SPI ʱ��Ƶ��ѡ��, 0: 4T, 1: 8T,  2: 16T,  3: 2T
    SPI_S1 = 1;     //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    SPI_S0 = 0;

    SPI_SCK = 0;    // set clock to low initial state
    SPI_SI = 1;
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־

    HSSPI_CFG2 |= 0x20;         //ʹ��SPI����ģʽ
}

/************************************************************************/
void SPI_WriteByte(u8 out)
{
    SPDAT = out;
    while(SPIF == 0) ;
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    while(SPIF == 0) ;
    SPIF = 1;   //��SPIF��־
    WCOL = 1;   //��WCOL��־
    return (SPDAT);
}

/************************************************
���Flash�Ƿ�׼������
��ڲ���: ��
���ڲ���:
    0 : û�м�⵽��ȷ��Flash
    1 : Flash׼������
************************************************/
void FlashCheckID(void)
{
    SPI_CE_Low();
    SPI_WriteByte(SFC_RDID);        //���Ͷ�ȡID����
    SPI_WriteByte(0x00);            //�ն�3���ֽ�
    SPI_WriteByte(0x00);
    SPI_WriteByte(0x00);
    PM25LV040_ID1 = SPI_ReadByte();         //��ȡ������ID1
    PM25LV040_ID  = SPI_ReadByte();         //��ȡ�豸ID
    PM25LV040_ID2 = SPI_ReadByte();         //��ȡ������ID2
    SPI_CE_High();

//    UartPutc(PM25LV040_ID1);
//    UartPutc(PM25LV040_ID);
//    UartPutc(PM25LV040_ID2);
    
    if((PM25LV040_ID1 == 0x9d) && (PM25LV040_ID2 == 0x7f))  B_FlashOK = 1;  //����Ƿ�ΪPM25LVxxϵ�е�Flash
    else if(PM25LV040_ID == 0x12)  B_FlashOK = 2;                           //����Ƿ�ΪW25X4xϵ�е�Flash
    else if(PM25LV040_ID == 0x13)  B_FlashOK = 3;                           //����Ƿ�ΪW25X8xϵ�е�Flash
    else if(PM25LV040_ID == 0x17)  B_FlashOK = 4;                           //����Ƿ�ΪW25X128ϵ�е�Flash
    else                                                    B_FlashOK = 0;
}

/************************************************
���Flash��æ״̬
��ڲ���: ��
���ڲ���:
    0 : Flash���ڿ���״̬
    1 : Flash����æ״̬
************************************************/
u8 CheckFlashBusy(void)
{
    u8  dat;

    SPI_CE_Low();
    SPI_WriteByte(SFC_RDSR);        //���Ͷ�ȡ״̬����
    dat = SPI_ReadByte();           //��ȡ״̬
    SPI_CE_High();

    return (dat);                   //״ֵ̬��Bit0��Ϊæ��־
}

/************************************************
ʹ��Flashд����
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashWriteEnable(void)
{
    while(CheckFlashBusy() > 0);    //Flashæ���
    SPI_CE_Low();
    SPI_WriteByte(SFC_WREN);        //����дʹ������
    SPI_CE_High();
}

/************************************************
������ƬFlash
��ڲ���: ��
���ڲ���: ��
************************************************/
/*
void FlashChipErase(void)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        SPI_WriteByte(SFC_CHIPER);      //����Ƭ��������
        SPI_CE_High();
    }
}
*/

/************************************************
��������, һ������4KB
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashSectorErase(u32 addr)
{
    if(B_FlashOK)
    {
        FlashWriteEnable();             //ʹ��Flashд����
        SPI_CE_Low();
        if(B_FlashOK == 1)
        {
            SPI_WriteByte(SFC_SECTORER1);    //����������������
        }
        else
        {
            SPI_WriteByte(SFC_SECTORER2);    //����������������
        }
        SPI_WriteByte(((u8 *)&addr)[1]);           //������ʼ��ַ
        SPI_WriteByte(((u8 *)&addr)[2]);
        SPI_WriteByte(((u8 *)&addr)[3]);
        SPI_CE_High();
    }
}

/************************************************
��Flash�ж�ȡ����
��ڲ���:
    addr   : ��ַ����
    buffer : �����Flash�ж�ȡ������
    size   : ���ݿ��С
���ڲ���:
    ��
************************************************/
void SPI_Read_Nbytes(u32 addr, u8 *buffer, u16 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���

    SPI_CE_Low();                       //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    do{
        *buffer = SPI_ReadByte();       //receive byte and store at buffer
        buffer++;
    }while(--size);                     //read until no_bytes is reached
    SPI_CE_High();                      //disable device
}

/************************************************************************
����n���ֽ�,��ָ�������ݽ��бȽ�, ���󷵻�1,��ȷ����0
************************************************************************/
u8 SPI_Read_Compare(u32 addr, u8 *buffer, u16 size)
{
    u8  j;
    if(size == 0)   return 2;
    if(!B_FlashOK)  return 2;
    while(CheckFlashBusy() > 0);            //Flashæ���

    j = 0;
    SPI_CE_Low();                           //enable device
    SPI_WriteByte(SFC_READ);                //read command
    SPI_WriteByte(((u8 *)&addr)[1]);        //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do
    {
        if(*buffer != SPI_ReadByte())       //receive byte and store at buffer
        {
            j = 1;
            break;
        }
        buffer++;
    }while(--size);         //read until no_bytes is reached
    SPI_CE_High();          //disable device
    return j;
}

/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    buffer : ������Ҫд��Flash������
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void SPI_Write_Nbytes(u32 addr, u8 *buffer, u8 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(CheckFlashBusy() > 0);        //Flashæ���


    FlashWriteEnable();                 //ʹ��Flashд����

    SPI_CE_Low();                       // enable device
    SPI_WriteByte(SFC_PAGEPROG);        // ����ҳ�������
    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);
    do{
        SPI_WriteByte(*buffer++);       //����ҳ��д
        addr++;
        if ((addr & 0xff) == 0) break;
    }while(--size);
    SPI_CE_High();                      // disable device
}
