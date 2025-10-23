/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ��������˵��  **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

���ڷ�ָ��ͨ��I2C DMA��дAT24C02����.

����(P3.0,P3.1)Ĭ������: 115200,8,N,1. 

������������: (��ĸ�����ִ�Сд)
    W 0x10 12345678 --> д�����  ʮ�����Ƶ�ַ  д������.
    R 0x10 8        --> ��������  ʮ�����Ƶ�ַ  �����ֽ���.

24CXX����д����ֽ�����ͬ, 1\2Kbit 8-Byte/Page, 4\8\16Kbit 16-Byte/Page.
����д��Ļ�����ַ+���ݳ��Ȳ�Ҫ����һ��PAGE��Χ.

����ʱ, ѡ��ʱ�� 22.1184MHz (�û��������޸�Ƶ��).

******************************************/

#include "../comm/AI8051U.h"
#include "intrins.h"
#include "stdio.h"

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** �û������ ***********************************/

#define MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))

/*****************************************************************************/


/*************  ���س�������    **************/

#define EE_BUF_LENGTH       255          //
#define UART1_BUF_LENGTH    (EE_BUF_LENGTH+7)   //���ڻ��峤��

#define SLAW    0xA0
#define SLAR    0xA1

/*************  ���ر�������    **************/

u8 EEPROM_addr;
u8 xdata DmaTxBuffer[256];
u8 xdata DmaRxBuffer[256];

u8  RX1_TimeOut;
u16 RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־
bit	DmaTxFlag=0;
bit	DmaRxFlag=0;

u8  RX1_Buffer[UART1_BUF_LENGTH];   //���ջ���

/*************  ���غ�������    **************/

void I2C_init(void);
void WriteNbyte(u8 addr, u8 number);
void ReadNbyte( u8 addr, u8 number);
void delay_ms(u8 ms);
void RX1_Check(void);
void DMA_Config(void);

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
    SCON = (SCON & 0x3f) | 0x40; 
    T1x12 = 1;      //��ʱ��ʱ��1Tģʽ
    S1BRT = 0;      //����1ѡ��ʱ��1Ϊ�����ʷ�����
    TL1  = TM;
    TH1  = TM>>8;
    TR1 = 1;        //��ʱ��1��ʼ��ʱ

//    SCON = (SCON & 0x3f) | 0x40; 
//    T2L  = TM;
//    T2H  = TM>>8;
//    AUXR |= 0x15; //����1ѡ��ʱ��2Ϊ�����ʷ�����

    REN = 1;        //�������
    ES = 1;         //ʹ�ܴ���1�ж�
}

void UartPutc(unsigned char dat)
{
    SBUF = dat; 
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}
 
char putchar(char c)
{
    UartPutc(c);
    return c;
}

/**********************************************/
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
    
    I2C_init();
    UartInit();
    DMA_Config();
    EA = 1;     //�������ж�

    printf("��������:\r\n");
    printf("W 0x10 12345678 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x10 8        --> ��������  ʮ�����Ƶ�ַ  �����ֽ�����\r\n");

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
// ����: void delay_ms(u8 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2021-3-9
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
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

/**************** ��ȡд���ַ ****************************/
u8 GetAddress(void)
{
    u8 address;
    u8  i,j;
    
    address = 0;
    if((RX1_Buffer[2] == '0') && (RX1_Buffer[3] == 'X'))
    {
        for(i=4; i<6; i++)
        {
            j = CheckData(RX1_Buffer[i]);
            if(j >= 0x10)   return 0;  //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0; //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8  GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=7; i<RX1_Cnt; i++)
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

    F0 = 0;
    if((RX1_Cnt >= 8) && (RX1_Buffer[1] == ' '))   //�������Ϊ8���ֽ�
    {
        for(i=0; i<6; i++)
        {
            if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';//Сдת��д
        }
        EEPROM_addr = GetAddress();
        if(EEPROM_addr <= 255)
        {
            if((RX1_Buffer[0] == 'W') && (RX1_Cnt >= 8) && (RX1_Buffer[6] == ' '))   //д��N���ֽ�
            {
                j = RX1_Cnt - 7;
                
                for(i=0; i<j; i++)  DmaTxBuffer[i+2] = RX1_Buffer[i+7];
                WriteNbyte(EEPROM_addr, j);     //дN���ֽ� 
                printf("��д��%d�ֽ�����!\r\n",j);
                delay_ms(5);

                ReadNbyte(EEPROM_addr, j);
                printf("����%d���ֽ��������£�\r\n",j);
                for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                printf("\r\n");

                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (RX1_Cnt >= 8) && (RX1_Buffer[6] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j <= EE_BUF_LENGTH))
                {
                    ReadNbyte(EEPROM_addr, j);
                    printf("����%d���ֽ��������£�\r\n",j);
                    for(i=0; i<j; i++)    printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("�������!\r\n");
}

//========================================================================
// ����: void DMA_Config(void)
// ����: I2C DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void DMA_Config(void)
{
    DMA_I2CT_STA = 0x00;
    DMA_I2CT_CFG = 0x80;	//bit7 1:Enable Interrupt
    DMA_I2CT_AMT = 0xff;	//���ô������ֽ���(��8λ)��n+1
    DMA_I2CT_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
    DMA_I2CT_TXAH = (u8)((u16)&DmaTxBuffer >> 8);	//I2C�������ݴ洢��ַ
    DMA_I2CT_TXAL = (u8)((u16)&DmaTxBuffer);
    DMA_I2CT_CR = 0x80;		//bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

    DMA_I2CR_STA = 0x00;
    DMA_I2CR_CFG = 0x80;	//bit7 1:Enable Interrupt
    DMA_I2CR_AMT = 0xff;	//���ô������ֽ���(��8λ)��n+1
    DMA_I2CR_AMTH = 0x00;	//���ô������ֽ���(��8λ)��n+1
    DMA_I2CR_RXAH = (u8)((u16)&DmaRxBuffer >> 8);	//I2C�������ݴ洢��ַ
    DMA_I2CR_RXAL = (u8)((u16)&DmaRxBuffer);
    DMA_I2CR_CR = 0x81;		//bit7 1:ʹ�� I2CT_DMA, bit5 1:��ʼ I2CT_DMA, bit0 1:��� FIFO

    DMA_I2C_ST1 = 0xff;		//������Ҫ�����ֽ���(��8λ)��n+1
    DMA_I2C_ST2 = 0x00;		//������Ҫ�����ֽ���(��8λ)��n+1
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
        TI = 0;    //Clear Tx flag
        B_TX1_Busy = 0;
    }
}

/********************** I2C���� ************************/
void I2C_init(void)
{
    P_SW2 = (P_SW2 & ~(3<<4)) | (3<<4); //IO���л�. 0: P1.4 P1.5, 1: P2.4 P2.5, 3: P3.3 P3.2
    I2CCFG = 0xe0;              //ʹ��I2C����ģʽ
    I2CMSST = 0x00;
}

void WriteNbyte(u8 addr, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);  //���I2C������æµ״̬

    DmaTxFlag = 1;
    DmaTxBuffer[0] = SLAW;
    DmaTxBuffer[1] = addr;

    I2CMSST = 0x00;
    I2CMSCR = 0x89;             //set cmd //write_start_combo
    DMA_I2C_CR = 0x01;
    DMA_I2CT_AMT = number+1;    //���ô������ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2CT_AMTH = 0x00;       //���ô������ֽ���(��8λ)��n+1
    DMA_I2C_ST1 = number+1;     //������Ҫ�����ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2C_ST2 = 0x00;         //������Ҫ�����ֽ���(��8λ)��n+1
    DMA_I2CT_CR |= 0x40;        //bit7 1:ʹ�� I2CT_DMA, bit6 1:��ʼ I2CT_DMA

    while(DmaTxFlag);           //DMAæ���
    DMA_I2C_CR = 0x00;
}

void ReadNbyte(u8 addr, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    while(I2CMSST & 0x80);    //���I2C������æµ״̬
    DMA_I2C_CR = 0x00;
    I2CMSST = 0x00;

    //������ʼ�ź�+�豸��ַ+д�ź�
    I2CTXD = SLAW;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    //���ʹ洢����ַ
    I2CTXD = addr;
    I2CMSCR = 0x0a;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;
    
    //������ʼ�ź�+�豸��ַ+���ź�
    I2CTXD = SLAR;
    I2CMSCR = 0x09;
    while ((I2CMSST & 0x40) == 0);
    I2CMSST = 0x00;

    DmaRxFlag = 1;
    //�������ݶ�ȡ����
    I2CMSCR = 0x8b;
    DMA_I2C_CR = 0x01;

    DMA_I2CR_AMT = number-1;    //���ô������ֽ���(��8λ)��n+1
    DMA_I2CR_AMTH = 0x00;       //���ô������ֽ���(��8λ)��n+1
    DMA_I2C_ST1 = number-1;     //������Ҫ�����ֽ���(��8λ)��number + �豸��ַ + �洢��ַ
    DMA_I2C_ST2 = 0x00;         //������Ҫ�����ֽ���(��8λ)��n+1
    DMA_I2CR_CR |= 0x40;        //bit7 1:ʹ�� I2CT_DMA, bit5 1:��ʼ I2CT_DMA, bit0 1:��� FIFO
    while(DmaRxFlag);           //DMAæ���
    DMA_I2C_CR = 0x00;
}

//========================================================================
// ����: void I2C_DMA_Interrupt (void) interrupt 60/61
// ����: I2C DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void I2C_DMA_Interrupt(void) interrupt 13
{
    if(DMA_I2CT_STA & 0x01)     //�������
    {
        DMA_I2CT_STA &= ~0x01;  //�����־λ
        DmaTxFlag = 0;
    }
    if(DMA_I2CT_STA & 0x04)     //���ݸ���
    {
        DMA_I2CT_STA &= ~0x04;  //�����־λ
    }

    if(DMA_I2CR_STA & 0x01)     //�������
    {
        DMA_I2CR_STA &= ~0x01;  //�����־λ
        DmaRxFlag = 0;
    }
    if(DMA_I2CR_STA & 0x02)     //���ݶ���
    {
        DMA_I2CR_STA &= ~0x02;  //�����־λ
    }
}

//========================================================================
// ����: void I2C_Interrupt (void) interrupt 24
// ����: I2C �жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-3-18
// ��ע: 
//========================================================================
void I2C_Interrupt() interrupt 24
{
	I2CMSST &= ~0x40;       //I2Cָ������״̬���

	if(DMA_I2C_CR & 0x04)   //ACKERR
	{
		DMA_I2C_CR &= ~0x04;  //�����ݺ��յ�NAK
	}
}

