/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

USART1��SPIģʽ��ʹ��DMA�ͼĴ�����ʽ����ʵ�����ϵ����flash.

ͨ��USB-CDC���������д���. 

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#define PRINTF_HID            //printf���ֱ���ض���USB�ӿ�

#include "../comm/AI8051U.h"
#include "../comm/usb.h"
#include "stdio.h"

#define MAIN_Fosc   24000000UL                      //ϵͳ����Ƶ��
#define BAUD        (65536 - MAIN_Fosc/4/115200)    //���Դ��ڲ�����

//USB���Լ���λ���趨��
char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";                  //�����Զ���λ��ISP�����û��ӿ�����

sbit S1SS       =   P4^0;
sbit S1MOSI     =   P4^1;
sbit S1MISO     =   P4^2;
sbit S1SCLK     =   P4^3;

sbit KEY1       =   P3^2;
//�����������
bit Key_Flag;
bit Key_Function;
WORD Key_cnt;

BYTE xdata buffer1[256];                        //���建����
BYTE xdata buffer2[256];                        //ע��:�����Ҫʹ��DMA��������,�򻺳������붨����xdata������

void sys_init();
void usart1_spi_init();
void usart1_tx_dma(WORD size, BYTE xdata *pdat);
void usart1_rx_dma(WORD size, BYTE xdata *pdat);
BOOL flash_is_busy();
void flash_read_id();
void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_write_enable();
void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat);
void flash_erase_sector(DWORD addr);
void delay_ms(BYTE ms);
void KeyScan(void);

void main()
{
    int i;
    
    sys_init();                                 //ϵͳ��ʼ��
    usb_init();  //USB��ʼ��
    usart1_spi_init();                          //USART1ʹ��SPIģʽ��ʼ��
    EA = 1;

    while (1)
    {
        delay_ms(1);
        KeyScan();      //����ɨ��

        if(DeviceState != DEVSTATE_CONFIGURED)  //�ȴ�USB�������
            continue;
        
        if(Key_Function)
        {
            Key_Function = 0;

            printf("\r\nUSART_SPI_DMA test !\r\n");
            flash_read_id();
            flash_read_data(0x0000, 0x80, buffer1);     //����ʹ��USART1��SPIģʽ��ȡ���FLASH������
            flash_erase_sector(0x0000);                 //����ʹ��USART1��SPIģʽ�������FLASH��һ������
            flash_read_data(0x0000, 0x80, buffer1);
            for (i=0; i<128; i++)
                buffer2[i] = i;
            flash_write_data(0x0000, 0x80, buffer2);    //����ʹ��USART1��SPIģʽд���ݵ����FLASH
            flash_read_data(0x0000, 0x80, buffer1);
        }

        if (bUsbOutReady)
        {
//            USB_SendData(UsbOutBuffer,64);  //�������ݻ����������ȣ���������ԭ������, ���ڲ��ԣ�
            
            usb_OUT_done(); //����Ӧ�𣨹̶���ʽ��
        }
    }
}

void sys_init()
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

    P4SR = 0xf0;                  //P4.0~P4.3���ٷ�ת
}

void usart1_spi_init()
{
    S1SPI_S1 = 1;   //00: P1.4 P1.5 P1.6 P1.7, 01: P2.4 P2.5 P2.6 P2.7, 10: P4.0 P4.1 P4.2 P4.3, 11: P3.5 P3.4 P3.3 P3.2
    S1SPI_S0 = 0;                       //�л�S1SPI��P4.0/S1SS,P4.1/S1MOSI,P4.2/S1MISO,P4.3/S1SCLK
    SCON = 0x10;                        //ʹ�ܽ���,��������Ϊ����ģʽ0
    
    USARTCR1 = 0x10;                    //ʹ��USART1��SPIģʽ
//  USARTCR1 |= 0x40;                   //DORD=1
    USARTCR1 &= ~0x40;                  //DORD=0
//  USARTCR1 |= 0x04;                   //�ӻ�ģʽ
    USARTCR1 &= ~0x04;                  //����ģʽ
    USARTCR1 |= 0x00;                   //CPOL=0, CPHA=0
//  USARTCR1 |= 0x01;                   //CPOL=0, CPHA=1
//  USARTCR1 |= 0x02 ;                  //CPOL=1, CPHA=0
//  USARTCR1 |= 0x03;                   //CPOL=1, CPHA=1
//  USARTCR4 = 0x00;                    //SPI�ٶ�ΪSYSCLK/4
//  USARTCR4 = 0x01;                    //SPI�ٶ�ΪSYSCLK/8
    USARTCR4 = 0x02;                    //SPI�ٶ�ΪSYSCLK/16
//  USARTCR4 = 0x03;                    //SPI�ٶ�ΪSYSCLK/2
    USARTCR1 |= 0x08;                   //ʹ��SPI����
}

BYTE usart1_spi_shift(BYTE dat)
{
    TI = 0;
    SBUF = dat;                         //��������
    while (!TI);                        //TI��־������ģʽ����/����������ɱ�־
    
    return SBUF;                        //��ȡ���յ�����
}

BOOL flash_is_busy()
{
    BYTE dat;

    S1SS = 0;
    
    usart1_spi_shift(0x05);             //���Ͷ�ȡ״̬�Ĵ�������
    dat = usart1_spi_shift(0);          //��ȡ״̬�Ĵ���
    
    S1SS = 1;

    return (dat & 0x01);                //���FLASH��æ��־
}

void flash_read_id()
{
    BYTE id[3];
    
    S1SS = 0;
    
    usart1_spi_shift(0x9f);             //���Ͷ�ȡFLASH ID����
    id[0] = usart1_spi_shift(0);       //��ȡID1
    id[1] = usart1_spi_shift(0);       //��ȡID2
    id[2] = usart1_spi_shift(0);       //��ȡID3
    
    S1SS = 1;

    printf("ReadID : ");
    printf("%02bx", id[0]);
    printf("%02bx", id[1]);
    printf("%02bx\r\n", id[2]);
}

void flash_read_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;
    BYTE *ptr;

    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x03);             //���Ͷ�ȡFLASH��������
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));     //����Ŀ���ַ
    
//  sz = size;
//  ptr = pdat;
//  while (sz--)
//      *ptr++ = usart1_spi_shift(0);   //�Ĵ�����ʽ������
      
    usart1_rx_dma(size, pdat);          //DMA��ʽ������

    S1SS = 1;

    printf("ReadData : ");
    sz = size;
    ptr = pdat;
    for (sz=0; sz<size; sz++)
    {
        printf("%02bx ", *ptr++);        //�����������ݷ��͵�����,����ʹ��
        if ((sz % 16) == 15)
        {
            printf("\r\n           ");
        }
    }
    printf("\r\n");
}

void flash_write_enable()
{
    while (flash_is_busy());

    S1SS = 0;
    
    usart1_spi_shift(0x06);             //����дʹ������
    
    S1SS = 1;
}

void flash_write_data(DWORD addr, WORD size, BYTE xdata *pdat)
{
    WORD sz;

    sz = size;
    while (sz)
    {
        flash_write_enable();

        S1SS = 0;
        
        usart1_spi_shift(0x02);         //����д��������
        usart1_spi_shift((BYTE)(addr >> 16));
        usart1_spi_shift((BYTE)(addr >> 8));
        usart1_spi_shift((BYTE)(addr));
        
//      do
//      {
//          usart1_spi_shift(*pdat++);  //�Ĵ�����ʽд����
//          addr++;
//
//          if ((BYTE)(addr) == 0x00)
//              break;
//      } while (--sz);

        usart1_tx_dma(sz, pdat);        //DMA��ʽд����(ע��:���ݱ�����һ��page֮��)
        sz = 0;
        
        S1SS = 1;
    }

    printf("Program !\r\n");
}

void flash_erase_sector(DWORD addr)
{
    flash_write_enable();

    S1SS = 0;
    usart1_spi_shift(0x20);             //���Ͳ�������
    usart1_spi_shift((BYTE)(addr >> 16));
    usart1_spi_shift((BYTE)(addr >> 8));
    usart1_spi_shift((BYTE)(addr));
    S1SS = 1;

    printf("Erase Sector !\r\n");
}

void usart1_tx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA�����ֽ�����ʵ����1
    
    DMA_UR1T_CFG = 0x00;                //�ر�DMA�ж�
    DMA_UR1T_STA = 0x00;                //���DMA״̬
    DMA_UR1T_AMT = size;                //����DMA�����ֽ���
    DMA_UR1T_AMTH = size >> 8;
    DMA_UR1T_TXAL = (BYTE)pdat;         //���û�������ַ(ע��:������������xdata����)
    DMA_UR1T_TXAH = (WORD)pdat >> 8;
    DMA_UR1T_CR = 0xc0;                 //ʹ��DMA,��������1��������
    
    while (!(DMA_UR1T_STA & 0x01));     //�ȴ�DMA���ݴ������
    DMA_UR1T_STA = 0x00;                //���DMA״̬
    DMA_UR1T_CR = 0x00;                 //�ر�DMA
}

void usart1_rx_dma(WORD size, BYTE xdata *pdat)
{
    size--;                             //DMA�����ֽ�����ʵ����1
    
    DMA_UR1R_CFG = 0x00;                //�ر�DMA�ж�
    DMA_UR1R_STA = 0x00;                //���DMA״̬
    DMA_UR1R_AMT = size;                //����DMA�����ֽ���
    DMA_UR1R_AMTH = size >> 8;
    DMA_UR1R_RXAL = (BYTE)pdat;         //���û�������ַ(ע��:������������xdata����)
    DMA_UR1R_RXAH = (WORD)pdat >> 8;
    DMA_UR1R_CR = 0xa1;                 //ʹ��DMA,��ս���FIFO,��������1��������
    
                                        //!!!!!!!!!!!!!
    usart1_tx_dma(size+1, pdat);        //ע��:��������ʱ����ͬʱ��������DMA
                                        //!!!!!!!!!!!!!
    
    while (!(DMA_UR1R_STA & 0x01));     //�ȴ�DMA���ݴ������
    DMA_UR1R_STA = 0x00;                //���DMA״̬
    DMA_UR1R_CR = 0x00;                 //�ر�DMA
}

//========================================================================
// ����: void delay_ms(BYTE ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2021-3-9
// ��ע: 
//========================================================================
void delay_ms(BYTE ms)
{
     WORD i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);   //6T per loop
     }while(--ms);
}

//========================================================================
// ����: void KeyScan(void)
// ����: ����ɨ�躯����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-6-11
// ��ע: 
//========================================================================
void KeyScan(void)
{
    if(!P32)
    {
        if(!Key_Flag)
        {
            Key_cnt++;
            if(Key_cnt >= 50)		//����50ms��Ч������⣬����
            {
                Key_Flag = 1;		//���ð���״̬����ֹ�ظ�����
                Key_Function = 1;
            }
        }
    }
    else
    {
        Key_cnt = 0;
        Key_Flag = 0;
    }
}
