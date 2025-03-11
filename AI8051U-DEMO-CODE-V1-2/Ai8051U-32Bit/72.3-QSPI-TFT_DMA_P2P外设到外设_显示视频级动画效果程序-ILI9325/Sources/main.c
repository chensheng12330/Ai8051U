/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

QSPI DMA + LCM DMA����Һ��������

��סP3.3���ϵ磬оƬ����FLASH����ģʽ��ʹ��6.94P���ISP��������еġ�����Flash�������

���߽����ͼƬ���ص�ʵ�����Flash�У�Ȼ�������ϵ磨P3.3��Ҫ���������ɽ��붯����ʾģʽ

��ʾʱQSPI ��ȡFlashͼƬ���ݣ�ͨ��P2P DMAֱ�ӷ������ݵ�LCM��ʾͼƬ��

QSPI��ȡFlash��LCM��ʾDMA�����ݳ�������51200�ֽڡ�

8bit I8080ģʽ, P2�ڽ�������

LCD_RS = P4^5;         //����/�����л�
LCD_WR = P3^6;         //д����
LCD_RD = P3^7;         //������
LCD_CS = P0^5;//P5^3;  //Ƭѡ
LCD_RESET = P4^7;      //��λ

QSPI Flash �ӿڣ�
sbit    QSPI_CS          =   P4^0;

sbit    QSPI_SDI_IO0     =   P4^1;

sbit    QSPI_SDO_IO1     =   P4^2;

sbit    QSPI_SCK         =   P4^3;

sbit    QSPI_WP_IO2      =   P5^2;

sbit    QSPI_HLD_IO3     =   P5^3;

����ʱ, ѡ��ʱ�� 40MHz (Ƶ�ʶ��������system.h�޸�).

Ver6.94P�°����˵����

  1. ���Ӵ���Flash��̹���
     ѡ�� USB-HID/CDC�������֣���� �����ļ����ڣ�
     ���½�����Flash��̹��ܰ�ť
     ֧�ֶ��ļ�ͬʱ����, ֧���Զ������ļ����������
     �͵��Դ�������ͨ�ŵ�MCUҪ�ж�Flash��̵Ķ�Ӧ����
     �����׵ķ�������ɹ��ο���
     �û��ɸ��ݶ�Ӧ��Flash�޸���Ӧ����
  2. ͼƬȡģ����֧�ֶ�gif�ļ���ʽ��֧��
     ���߲˵� �е� ͼƬȡģ���� ����֧��ת��gif�ļ�
     ֧���Զ�ת����֡ͼƬ���������ݣ�
     ��������Ƶ���Ķ�����ʾ

******************************************/

#include "config.h"
#include "qspi.h"
#include "w25qxx.h"
#include "lcm.h"
#include "tft.h"
#include "usbcdc.h"
#include "timer.h"

#define IMG_SIZE            (320UL * 240 * 2)
#define DMA_AMT_LEN         (51200UL)
#define DMA_CNT             (IMG_SIZE / DMA_AMT_LEN)

void QSPI2TFT_Start();
void QSPI2TFT_Next();

typedef struct
{
    char strSign[4];
    DWORD dwCount;
    DWORD dwAddress[62];
} FAT;

FAT     Fat;                            //�ļ������
DWORD   dwOffset;                       //ͼƬ���ݵ�ƫ�Ƶ�ַ
int     nIndex;                         //ͼƬ����
int     nCount;                         //ͼƬ����װ�ش���
BOOL    fLoading;                       //װ�����ݱ�־

void main()
{
    P_SW2 = 0x80;
    WTST = 0x00;
    CKCON = 0x00;

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x00; P5M1 = 0x00;
    
    TIMER0_Init();
    QSPI_Init();
    LCM_Init();
    TFT_Init();
    CDC_Init();
    
    EA = 1;

    W25Q_Enable_QE();                   //ʹ��QSPI FLASH��4�߶�дģʽ
    CDC_WaitStable();                   //�ȴ�USB-CDC�������
    
    if (!P33)                           //��סP3.3��λ,����ͼƬ��������ģʽ
    {
        while (1)
        {
            CDC_Process();              //CDC�ӿڴ���ͼƬ����
        }
    }
    
    W25Q_FastRead_6B(0, (BYTE *)&Fat, 256);     //��FLASH��һҳ��ȡFAT
    
    nIndex = 0;                         //�ӵ�һ��ͼƬ��ʼ��ʾ
    while (1)
    {
        if (f100ms)
        {
            f100ms = 0;
            
            QSPI2TFT_Start();           //ÿ��100ms�Զ���ʾ��һ��ͼƬ
        }
    }
}

void QSPI_DMA_Isr() interrupt DMA_QSPI_VECTOR
{
    DMA_QSPI_STA = 0x00;
    QSPI2TFT_Next();                    //DMA������һ������
}

void QSPI2TFT_Start()
{
    if (fLoading)                       //�������װ��ͼƬ,���˳�
        return;

    if (nIndex >= Fat.dwCount)          //���ͼƬ�����ﵽ���ֵ
        nIndex = 0;                     //��ӵ�һ��ͼƬ��ʼѭ��

    dwOffset = Fat.dwAddress[nIndex++]; //��ȡ��ǰͼƬ��ƫ�Ƶ�ַ
    nCount = 0;                         //��ʼ��ͼƬ����װ�ش���
    
    while (QSPI_CheckBusy());           //���æ״̬
    QSPI_SetReadMode();                 //��ģʽ
    QSPI_SetDataLength(DMA_AMT_LEN-1);  //�������ݳ���
    QSPI_SetAddressSize(2);             //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(8);             //����DUMMYʱ��
    QSPI_NoInstruction();               //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();                   //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();                 //�޼���ֽ�
    QSPI_DataQuadMode();                //��������Ϊ����ģʽ
    QSPI_SetInstruction(0x6B);          //����ָ��
    QSPI_InstructionSingMode();         //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();                   //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_SetAddress(dwOffset);          //���õ�ַ
    QSPI_AddressSingMode();             //���õ�ַΪ����ģʽ
                
    TFT_ShowStart();                    //��ʼTFT������ʾ

    DMA_P2P_CR1 = 0x87;                 //P2P_SRC_QSPIRX(0x80) | P2P_DEST_LCMTX(0x07);
    DMA_QSPI_CFG = 0xa0;                //ʹ��DMA��ȡ����
    DMA_QSPI_STA = 0x00;                //���DMA״̬
    DMA_QSPI_AMT = (DMA_AMT_LEN-1);     //����DMA���ݳ���
    DMA_QSPI_AMTH = (DMA_AMT_LEN-1) >> 8;
    DMA_LCM_CR = 0xa0;
    DMA_QSPI_CR = 0xa1;                 //����DMA������QSPI������
    
    fLoading = TRUE;
}

void QSPI2TFT_Next()
{
    dwOffset += DMA_AMT_LEN;            //����ƫ�Ƶ�ַ
    nCount++;                           //װ�ش���+1
    
    if (nCount < DMA_CNT)               //�ж�ͼƬ����װ�ش���
    {
        QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
        QSPI_SetAddress(dwOffset);      //���õ�ַ
        QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
        
        DMA_QSPI_CR = 0xa1;             //����DMA������QSPI������
    }
    else
    {
        fLoading = FALSE;               //ͼƬ����װ�����
        DMA_QSPI_CR = 0x00;             //ֹͣQSPI_DMA
        DMA_QSPI_CFG = 0x00;
        DMA_LCM_CR = 0x00;
        DMA_P2P_CR1 = 0x00;
        
        TFT_ShowEnd();                  //����TFT������ʾ��������
    }
}

void delay_ms(WORD ms)
{
    WORD i;
    
    do
    {
        i = FOSC / 6000;
        while(--i);
    } while(--ms);
}
