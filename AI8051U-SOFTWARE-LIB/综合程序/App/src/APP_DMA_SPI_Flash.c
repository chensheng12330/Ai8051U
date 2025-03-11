/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_DMA_SPI_Flash.h"
#include "APP_SPI_Flash.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_SPI.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_DMA.h"
#include "AI8051U_Switch.h"

/*************	����˵��	**************

ͨ�����ڶ�SPI Flash���ж�д���ԡ�

��FLASH������������д�롢�����Ĳ���������ָ����ַ��

����(P3.0,P3.1)Ĭ�ϲ�����:  115200,8,N,1.

������������: (��ĸ�����ִ�Сд)
    E 0x001234              --> ����������ָ��ʮ�����Ƶ�ַ.
    W 0x001234 1234567890   --> д�������ָ��ʮ�����Ƶ�ַ������Ϊд������.
    R 0x001234 10           --> ����������ָ��ʮ�����Ƶ�ַ������Ϊ�����ֽ���. 
    C                       --> �����ⲻ��SPI Flash, ����Cǿ���������.

ע�⣺Ϊ��ͨ�ã�����ʶ���ַ�Ƿ���Ч���û��Լ����ݾ�����ͺ���������

����д���������ݷ���SPI����DMA�ռ䣬Ȼ������SPI_DMA���з���.
������������ͨ��SPI��ȡ�����DMA���տռ䣬�ɴ��ڽ��д�ӡ��ʾ.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

//========================================================================
//                               ���س�������	
//========================================================================

sbit SPI_CE  = P4^0;     //PIN1
sbit SPI_SO  = P4^2;     //PIN2
sbit SPI_SI  = P4^1;     //PIN5
sbit SPI_SCK = P4^3;     //PIN6

#define DMA_BUF_LENGTH   255

//========================================================================
//                               ���ر�������
//========================================================================


//========================================================================
//                               ���غ�������
//========================================================================

void Command_Check(void);

//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: DMA_SPI_PS_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void DMA_SPI_Flash_init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    COMx_InitDefine COMx_InitStructure;     //�ṹ����
    DMA_SPI_InitTypeDef DMA_SPI_InitStructure;

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

    SPI_InitStructure.SPI_Enable    = ENABLE;           //SPI����    ENABLE, DISABLE
    SPI_InitStructure.SPI_SSIG      = ENABLE;           //Ƭѡλ     ENABLE, DISABLE
    SPI_InitStructure.SPI_FirstBit  = SPI_MSB;          //��λ����   SPI_MSB, SPI_LSB
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;  //����ѡ��   SPI_Mode_Master, SPI_Mode_Slave
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High;    //ʱ����λ   SPI_CPOL_Low,    SPI_CPOL_High
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge;   //���ݱ���   SPI_CPHA_1Edge,  SPI_CPHA_2Edge
    SPI_InitStructure.SPI_Speed     = SPI_Speed_16;     //SPI�ٶ�    SPI_Speed_4, SPI_Speed_8, SPI_Speed_16, SPI_Speed_2
    SPI_Init(&SPI_InitStructure);
    NVIC_SPI_Init(DISABLE,Priority_0);      //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    SPI_ClearFlag();    //���SPIF��WCOL��־

    SPI_SW(SPI_P40_P41_P42_P43);            //SPI_P14_P15_P16_P17,SPI_P24_P25_P26_P27,SPI_P40_P41_P42_P43,SPI_P35_P34_P33_P32
    P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1); //P3.0,P3.1 ����Ϊ׼˫���
    P4_MODE_IO_PU(GPIO_Pin_LOW);            //P40~P43 ����Ϊ׼˫���
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2,P5.3 �ڲ�����ʹ��

    SPI_SCK = 0;    // set clock to low initial state
    SPI_SI = 1;

	//----------------------------------------------
	DMA_SPI_InitStructure.DMA_Enable = ENABLE;				//DMAʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Tx_Enable = ENABLE;			//DMA��������ʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Rx_Enable = ENABLE;			//DMA��������ʹ��  	ENABLE,DISABLE
	DMA_SPI_InitStructure.DMA_Length = DMA_BUF_LENGTH;		//DMA�������ֽ���  	(0~65535) + 1
	DMA_SPI_InitStructure.DMA_Tx_Buffer = (u16)DmaTxBuffer;	//�������ݴ洢��ַ
	DMA_SPI_InitStructure.DMA_Rx_Buffer = (u16)DmaRxBuffer;	//�������ݴ洢��ַ
	DMA_SPI_InitStructure.DMA_SS_Sel = SPI_SS_P40;			//�Զ�����SS��ѡ�� 	SPI_SS_P14,SPI_SS_P24,SPI_SS_P40,SPI_SS_P35
	DMA_SPI_InitStructure.DMA_AUTO_SS = DISABLE;			//�Զ�����SS��ʹ��  	ENABLE,DISABLE
	DMA_SPI_Inilize(&DMA_SPI_InitStructure);		        //��ʼ��
	SET_DMA_SPI_CR(DMA_ENABLE | CLR_FIFO);	                //bit7 1:ʹ�� SPI_DMA, bit5 1:��ʼ SPI_DMA �ӻ�ģʽ, bit0 1:��� SPI_DMA FIFO
	NVIC_DMA_SPI_Init(ENABLE,Priority_0,Priority_0);		//�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3

	//----------------------------------------------
    printf("��������:\r\n");
    printf("E 0x001234            --> ��������  ʮ�����Ƶ�ַ\r\n");
    printf("W 0x001234 1234567890 --> д�����  ʮ�����Ƶ�ַ  д������\r\n");
    printf("R 0x001234 10         --> ��������  ʮ�����Ƶ�ַ  �����ֽ�\r\n");
    printf("C                     --> �����ⲻ��SPI Flash, ����Cǿ���������.\r\n\r\n");

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
}

//========================================================================
// ����: Sample_DMA_SPI_PS
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void Sample_DMA_SPI_Flash(void)
{
    if(COM1.RX_TimeOut)
    {
        COM1.RX_TimeOut = 0;

        if(COM1.RX_Cnt > 0)
        {
            Command_Check();        //�����������
        }
        COM1.RX_Cnt = 0;
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
void SPI_DMA_Read_Nbytes(u32 addr, u16 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiTxFlag);                     //DMAæ���
    while(CheckFlashBusy() > 0);        //Flashæ���

    SPI_CE  = 0;                        //enable device
    SPI_WriteByte(SFC_READ);            //read command

    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiTxFlag = 1;
    SET_SPI_DMA_LEN(size-1);    //���ô������ֽ�����n+1
    DMA_SPI_TRIG_M();           //��ʼSPI_DMA��ģʽ����
}

/************************************************************************
����n���ֽ�,��ָ�������ݽ��бȽ�, ���󷵻�1,��ȷ����0
************************************************************************/
u8 SPI_DMA_Read_Compare(u16 size)
{
    u8  j=0;
    if(size == 0)   return 2;
    if(!B_FlashOK)  return 2;
    while(SpiTxFlag);                         //DMAæ���

    do
    {
        if(DmaRxBuffer[j] != DmaTxBuffer[j])       //receive byte and store at buffer
        {
            return 1;
        }
        j++;
    }while(--size);         //read until no_bytes is reached
    return 0;
}

/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void SPI_DMA_Write_Nbytes(u32 addr, u8 size)
{
    if(size == 0)   return;
    if(!B_FlashOK)  return;
    while(SpiTxFlag);                     //DMAæ���
    while(CheckFlashBusy() > 0);        //Flashæ���

    FlashWriteEnable();                 //ʹ��Flashд����

    SPI_CE  = 0;                        //enable device
    SPI_WriteByte(SFC_PAGEPROG);        // ����ҳ�������
    SPI_WriteByte(((u8 *)&addr)[1]);    //������ʼ��ַ
    SPI_WriteByte(((u8 *)&addr)[2]);
    SPI_WriteByte(((u8 *)&addr)[3]);

    SpiTxFlag = 1;
    SET_SPI_DMA_LEN(size-1);    //���ô������ֽ�����n+1
    DMA_SPI_TRIG_M();           //��ʼSPI_DMA��ģʽ����
}

void Command_Check(void)
{
    u8  i,j;

    if((COM1.RX_Cnt == 1) && (RX1_Buffer[0] == 'C'))    //����Cǿ���������
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
                printf("�Ѳ���һ����������!\r\n");
                F0 = 1;
            }

            else if((RX1_Buffer[0] == 'W') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //д��N���ֽ�
            {
                j = COM1.RX_Cnt - 11;
                for(i=0; i<j; i++)  DmaTxBuffer[i] = 0xff;      //���Ҫд��Ŀռ��Ƿ�Ϊ��
                SPI_DMA_Read_Nbytes(Flash_addr,j);
                i = SPI_DMA_Read_Compare(j);
                if(i > 0)
                {
                    printf("Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n");
                }
                else
                {
                    for(i=0; i<j; i++)  DmaTxBuffer[i] = RX1_Buffer[i+11];
                    SPI_DMA_Write_Nbytes(Flash_addr,j);     //дN���ֽ� 
                    SPI_DMA_Read_Nbytes(Flash_addr,j);
                    i = SPI_DMA_Read_Compare(j); //�Ƚ�д�������
                    if(i == 0)
                    {
                        printf("��д��%d�ֽ�����!\r\n",j);
                    }
                    else printf("д�����!\r\n");
                }
                F0 = 1;
            }
            else if((RX1_Buffer[0] == 'R') && (COM1.RX_Cnt >= 12) && (RX1_Buffer[10] == ' '))   //����N���ֽ�
            {
                j = GetDataLength();
                if((j > 0) && (j < DMA_BUF_LENGTH))
                {
                    SPI_DMA_Read_Nbytes(Flash_addr,j);
                    printf("����%d���ֽ��������£�\r\n",j);
                    for(i=0; i<j; i++) printf("%c", DmaRxBuffer[i]);
                    printf("\r\n");
                    F0 = 1;
                }
            }
        }
    }
    if(!F0) printf("�������!\r\n");
}
