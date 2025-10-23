/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_SPI.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_UART.h"
#include "AI8051U_Switch.h"

/*************   ����˵��   ***************

ͨ������SPI��PM25LV040/W25X40CL/W25Q80BV���ж�д���ԡ�

���ڷ�ָ���FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,8,N,1. ������"���ڳ�ʼ������"���޸�.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ����������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ����������ָ��ʮ�����Ƶ�ַ������Ϊ�����ֽ�. 
    C                       --> �����ⲻ��ָ��SPI Flash, ����Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/


/*************    ���س�������    **************/

#define BUF_LENGTH          107     //n+1
#define EE_BUF_LENGTH       50      //

/******************* FLASH��ض��� ************************/
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

sbit SPI_CE  = P4^0;     //PIN1
sbit SPI_SO  = P4^2;     //PIN2
sbit SPI_SI  = P4^1;     //PIN5
sbit SPI_SCK = P4^3;     //PIN6

#define SPI_CE_High()   SPI_CE  = 1     // SPIƬѡ�ø�
#define SPI_CE_Low()    SPI_CE  = 0     // SPIƬѡ�õ�

/*************    ���ر�������    **************/

u8  B_FlashOK;          //Flash״̬
u8  FLASH_ID, FLASH_ID1, FLASH_ID2;
u32 Flash_addr;

/*************    ���غ�������    **************/

void FlashCheckID(void);
void RX1_Check(void);
u8   CheckFlashBusy(void);
void FlashWriteEnable(void);
void FlashChipErase(void);
void FlashSectorErase(u32 addr);
void SPI_Read_Nbytes( u32 addr, u8 *buffer, u16 size);
u8   SPI_Read_Compare(u32 addr, u8 *buffer, u16 size);
void SPI_Write_Nbytes(u32 addr, u8 *buffer,  u8 size);

/*************  �ⲿ�����ͱ������� *****************/


/******************** IO������ ********************/
void GPIO_config(void)
{
    P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1); //P3.0,P3.1 ����Ϊ׼˫���
    P4_MODE_IO_PU(GPIO_Pin_LOW);            //P40~P43 ����Ϊ׼˫���
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2,P5.3 �ڲ�����ʹ��

    SPI_SCK = 0;    //����SPI�ӿڳ�ʼ����ƽ
    SPI_SI = 1;
    SPI_CE = 1;
}

/******************** SPI ���� ********************/
void SPI_config(void)
{
    SPI_InitTypeDef SPI_InitStructure;

    SPI_InitStructure.SPI_Enable    = ENABLE;           //SPI����    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = ENABLE;           //Ƭѡλ     ENABLE, DISABLE
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;          //��λ����   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;  //����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;    //ʱ����λ   SPI_CPOL_Low,    SPI_CPOL_High
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;   //���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_16;     //SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(DISABLE,Priority_0);      //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    SPI_SW(SPI_P40_P41_P42_P43);            //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32
    SPI_ClearFlag();   //��� SPIF��WCOL ��־
}

/******************** UART ���� ********************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;     //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE��DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);     //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

u8 Hex2Ascii(u8 dat)
{
    dat &= 0x0f;
    if(dat < 10)    return (dat+'0');
    return (dat-10+'A');
}

/**********************************************/
void main(void)
{
    WTST = 0;        //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();        //��չSFR(XFR)����ʹ�� 
    CKCON = 0;      //��߷���XRAM�ٶ�

    GPIO_config();
    UART_config();
    SPI_config();
    EA = 1;

    printf("��������:\r\n");
    printf("E 0x001234            --> ��������  ʮ�����Ƶ�ַ\r\n");
    printf("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�\r\n");
    printf("C                     --> �����ⲻ��ָ��SPI Flash, ����Cǿ���������.\r\n\r\n");

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
    printf("������ID1 = 0x%02X",FLASH_ID1);
    printf("\r\n      ID2 = 0x%02X",FLASH_ID2);
    printf("\r\n   �豸ID = 0x%02X\r\n",FLASH_ID);

    while (1)
    {
        if(COM1.RX_TimeOut)
        {
            COM1.RX_TimeOut = 0;

            if(COM1.RX_Cnt > 0)
            {
                RX1_Check();        //����1��������
            }
            COM1.RX_Cnt = 0;
        }
    }
}

/**************** ASCII��תBIN ****************************/
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9')) return (dat-'0');
    if((dat >= 'A') && (dat <= 'F')) return (dat-'A'+10);
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
u8 GetDataLength(void)
{
    u8 i;
    u8 length;
    
    length = 0;
    for(i=11; i<COM1.RX_Cnt; i++)
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
    u8  temp[EE_BUF_LENGTH];

    if((COM1.RX_Cnt == 1) && (RX1_Buffer[0] == 'C'))    //����Cǿ���������
    {
        B_FlashOK = 1;
        printf("ǿ���������FLASH!\r\n");
    }

    if(!B_FlashOK)
    {
        printf("PM25LV040/W25X40CL/W25Q80BV/W25Q128FV������, ���ܲ���FLASH!\r\n");
        return;
    }
    
    F0 = 0;
    if((COM1.RX_Cnt >= 10) && (RX1_Buffer[1] == ' '))   //�������Ϊ10���ֽ�
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
                printf("�Ѳ�����ǰ��������!\r\n");
                F0 = 1;
            }

            else if((RX1_Buffer[0] == 'W') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //д��N���ֽ�
            {
                j = COM1.RX_Cnt - 11;
                for(i=0; i<j; i++)  temp[i] = 0xff;      //���Ҫд��Ŀռ��Ƿ�Ϊ��
                i = SPI_Read_Compare(Flash_addr,temp,j);
                if(i > 0)
                {
                    printf("Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n");
                }
                else
                {
                    SPI_Write_Nbytes(Flash_addr,&RX1_Buffer[11],j);     //дN���ֽ� 
                    i = SPI_Read_Compare(Flash_addr,&RX1_Buffer[11],j); //�Ƚ�д�������
                    if(i == 0)
                         printf("��д��%u�ֽ�����!\r\n",j);
                    else printf("д�����!\r\n");
                }
                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j < EE_BUF_LENGTH))
                {
                    SPI_Read_Nbytes(Flash_addr,temp,j);
                    printf("����%u���ֽ��������£�\r\n",j);
                    for(i=0; i<j; i++)  printf("%c",temp[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("�������!\r\n");
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
    FLASH_ID1 = SPI_ReadByte();     //��ȡ������ID1
    FLASH_ID  = SPI_ReadByte();     //��ȡ�豸ID
    FLASH_ID2 = SPI_ReadByte();     //��ȡ������ID2
    SPI_CE_High();

//    printf("ID1=0x%02x,ID=0x%02x,ID2=0x%02x\r\n",FLASH_ID1,FLASH_ID,FLASH_ID2);
    
    if((FLASH_ID1 == 0x9d) && (FLASH_ID2 == 0x7f))  B_FlashOK = 1;  //����Ƿ�ΪPM25LVxxϵ�е�Flash
    else if(FLASH_ID == 0x12)  B_FlashOK = 2;                       //����Ƿ�ΪW25X4xϵ�е�Flash
    else if(FLASH_ID == 0x13)  B_FlashOK = 3;                       //����Ƿ�ΪW25X8xϵ�е�Flash
    else if(FLASH_ID == 0x17)  B_FlashOK = 4;                       //����Ƿ�ΪW25X128ϵ�е�Flash
    else                                            B_FlashOK = 0;
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
