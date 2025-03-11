/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "system.h"
#include "intrins.h"
#include "SPI_Flash.h"
#include "stdio.h"
#include "uart.h"
#include "lcm.h"
#include "qspi.h"
#include "w25qxx.h"

/*************  ���س�������    **************/

/*************  ���ر�������    **************/

u32 Flash_addr;
u16 lcdIndex;
u16 spiIndex;

/*************  FLASH��ر�������   **************/
u8  B_FlashOK;                                //Flash״̬
u8  PM25LV040_ID, PM25LV040_ID1, PM25LV040_ID2;

bit SpiDmaFlag;

void FlashCheckID(void);

//========================================================================
// ����: void SPI_DMA_Config(void)
// ����: SPI DMA ��������.
// ����: none.
// ����: none.
// �汾: V1.0, 2021-5-6
//========================================================================
void SPI_DMA_Config(void)
{
    //�رս���DMA���´ν��յ��������´������ʼ��ַλ�ã������´ν������ݼ����������š�
    DMA_QSPI_CR = 0x00;        //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ Ubit0 1:��� FIFO

    DMA_QSPI_STA = 0x00;
    DMA_QSPI_CFG = 0x20;                //ʹ��DMA��ȡ����
    DMA_QSPI_AMT = (u8)(DMA_WR_LEN-1);         //���ô������ֽ���(��8λ)��n+1
    DMA_QSPI_AMTH = (u8)((DMA_WR_LEN-1) >> 8); //���ô������ֽ���(��8λ)��n+1
    DMA_QSPI_CR = 0x81;        //bit7 1:ʹ�� QSPI_DMA, bit6 1:��ʼ, bit0 1:��� SPI_DMA FIFO
}

/************************************************************************/
void SPI_init(void)
{
    QSPI_Init();

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
    
    if ((W25Q_ReadSR2_35() & 0x02) == 0)
    {
        W25Q_WriteEnableVSR_50();
        if ((W25Q_ReadJEDECID_9F() & 0xffff) == 0x4014)
        {
            W25Q_WriteSR12_01(0x0002);
        }
        else
        {
            W25Q_WriteSR2_31(0x02);
        }
    }
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
    u8 id;
    
    id = W25Q_ReadDeviceID_AB();
    PM25LV040_ID1 = id;
    PM25LV040_ID  = id;
    PM25LV040_ID2 = id;

//    printf("ID1=%x\r\n",PM25LV040_ID1);
//    printf("ID=%x\r\n",PM25LV040_ID);
//    printf("ID2=%x\r\n",PM25LV040_ID2);
    
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
    return (W25Q_ReadSR1_05() & 0x01);
}


/************************************************
������ƬFlash
��ڲ���: ��
���ڲ���: ��
************************************************/
void FlashChipErase(void)
{
    if(B_FlashOK)
    {
        W25Q_EraseChip_C7();            //����Ƭ��������
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
void SPI_Read_Nbytes(u32 addr, u16 len)
{
    if(len == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiDmaFlag);                  //DMAæ���

    while (QSPI_CheckBusy());           //���æ״̬
    QSPI_SetReadMode();                 //��ģʽ
    QSPI_SetDataLength(len-1);          //�������ݳ���
    QSPI_SetAddressSize(2);             //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(8);             //����DUMMYʱ��
    QSPI_NoInstruction();               //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();                   //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();                 //�޼���ֽ�
    QSPI_DataQuadMode();                //��������Ϊ����ģʽ
    QSPI_SetInstruction(0x6b);          //����ָ��(���߿��ٶ�ȡ����)
    QSPI_SetAddress(addr);              //���õ�ַ
    QSPI_InstructionSingMode();         //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();             //���õ�ַΪ����ģʽ

    DMA_P2P_CR1 = 0x87;                 //P2P_SRC_QSPIRX(0x80) | P2P_DEST_LCMTX(0x07);
    DMA_QSPI_CFG = 0xa0;                //ʹ��DMA��ȡ����
    DMA_QSPI_STA = 0x00;                //���DMA״̬
    DMA_QSPI_AMT = (len-1);             //����DMA���ݳ���
    DMA_QSPI_AMTH = (len-1) >> 8;
    DMA_LCM_CR = 0xa1;
    DMA_QSPI_CR = 0xa1;                 //����DMA������QSPI������

    SpiDmaFlag = 1;
}

/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
//void SPI_Write_Nbytes(u32 addr, u16 len)
//{
//}

//========================================================================
// ����: void QSPI_DMA_Interrupt (void) interrupt DMA_QSPI_VECTOR
// ����: QSPI DMA�жϺ���
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2021-5-8
// ��ע: 
//========================================================================
void QSPI_DMA_Interrupt(void) interrupt DMA_QSPI_VECTOR   //�ж������ų���31�����뱨��Ļ��谲װ�����̰���Ŀ¼�µģ�Keil�ж���������չ���
{
    DMA_QSPI_STA = 0;
    SpiDmaFlag = 0;
}
