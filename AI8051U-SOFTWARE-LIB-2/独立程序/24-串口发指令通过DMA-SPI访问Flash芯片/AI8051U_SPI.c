/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_SPI.h"

bit B_SPI_Busy; //����æ��־
bit SPI_RxTimerOut;
bit SPI_TimerOutEn;
u8 	SPI_RxCnt;
u8  SPI_BUF_type SPI_RxBuffer[SPI_BUF_LENTH];

//========================================================================
// ����: void SPI_Init(SPI_InitTypeDef *SPIx)
// ����: SPI��ʼ������.
// ����: SPIx: �ṹ����,��ο�spi.h��Ķ���.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void SPI_Init(SPI_InitTypeDef *SPIx)
{
    if(SPIx->SPI_SSIG == ENABLE) SSIG = 1;     //conform Master or Slave by SPI_Mode(ignore SS)
    else                         SSIG = 0;     //conform Master or Slave by SS pin.
    SPI_Start(SPIx->SPI_Enable);
    SPI_FirstBit_Set(SPIx->SPI_FirstBit);
    SPI_Mode_Set(SPIx->SPI_Mode);
    SPI_CPOL_Set(SPIx->SPI_CPOL);
    SPI_CPHA_Set(SPIx->SPI_CPHA);
    SPI_Clock_Select(SPIx->SPI_Speed);

    if(SPIx->TimeOutScale == TO_SCALE_SYSCLK)   SPITOCR |= 0x20;  //��ʱ����ʱ��Դ��ϵͳʱ��
    else
    {
        SET_TPS();          //����ϵͳ�ȴ�ʱ�䵥Ԫ�����ڿ���EEPROM������SPI/I2C��ʱʱ���Լ����߻��ѵȴ�ʱ��
        SPITOCR &= ~0x20;   //��ʱ����ʱ��Դ��1usʱ��(1MHzʱ��)
    }

    if((SPIx->TimeOutTimer > 0) && (SPIx->TimeOutTimer <= 0xffffff))
    {
        SPITOTL = (u8)SPIx->TimeOutTimer;
        SPITOTH = (u8)(SPIx->TimeOutTimer>>8);
        SPITOTE = (u8)(SPIx->TimeOutTimer>>16); //д SPITOTE ���µ�TMֵ�Ż���Ч
    }
    
    if((SPIx->SPI_Mode == SPI_Mode_Slave) && (SPIx->TimeOutEnable == ENABLE))
    {
        SPI_TimerOutEn = 1;
        SPITOCR |= 0x80;    //�ӻ����ճ�ʱ����ʹ��
    }
    else
    {
        SPI_TimerOutEn = 0;
        SPITOCR &= ~0x80;   //�ӻ����ճ�ʱ���ܽ�ֹ
    }

    if(SPIx->TimeOutINTEnable == ENABLE) SPITOCR |= 0x40;   //�ӻ����ճ�ʱ�ж�ʹ��
    else                                 SPITOCR &= ~0x40;  //�ӻ����ճ�ʱ�жϽ�ֹ

    SPI_RxTimerOut = 0;
    B_SPI_Busy = 0;
    SPI_RxCnt = 0;
}

//========================================================================
// ����: void SPI_SetMode(u8 mode)
// ����: SPI��������ģʽ����.
// ����: mode: ָ��ģʽ, ȡֵ SPI_Mode_Master �� SPI_Mode_Slave.
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void SPI_SetMode(u8 mode)
{
    if(mode == SPI_Mode_Slave)
    {
        MSTR = 0;     //��������Ϊ�ӻ�����
        SSIG = 0;     //SS����ȷ������
        if(SPI_TimerOutEn)
        {
            SPI_TOIFClear();
            SPITOCR |= 0x80;    //�ӻ����ճ�ʱ����ʹ��
        }
    }
    else
    {
        SPITOCR &= ~0x80;   //�ӻ����ճ�ʱ���ܽ�ֹ
        MSTR = 1;     //ʹ�� SPI ����ģʽ
        SSIG = 1;     //����SS���Ź���
    }
}

//========================================================================
// ����: void SPI_WriteByte(u8 dat)
// ����: SPI����һ���ֽ�����.
// ����: dat: Ҫ���͵�����.
// ����: none.
// �汾: V1.0, 2020-09-14
//========================================================================
void SPI_WriteByte(u8 dat)  //SPI����һ���ֽ�����
{
    if(ESPI)
    {
        B_SPI_Busy = 1;
        SPDAT = dat;
        while(B_SPI_Busy);  //�ж�ģʽ
    }
    else
    {
        SPDAT = dat;
        while(SPIF == 0);   //��ѯģʽ
        SPI_ClearFlag();    //���SPIF��WCOL��־
    }
}

//========================================================================
// ����: void SPI_ReadByte(u8 dat)
// ����: SPI��ѯģʽ��ȡһ���ֽ�����.
// ����: none.
// ����: ��ȡ������.
// �汾: V1.0, 2020-09-14
//========================================================================
u8 SPI_ReadByte(void)
{
    SPDAT = 0xff;
    if(ESPI)
    {
        B_SPI_Busy = 1;
        while(B_SPI_Busy);  //�ж�ģʽ
    }
    else
    {
        while(SPIF == 0);   //��ѯģʽ
        SPI_ClearFlag();    //���SPIF��WCOL��־
    }
    return (SPDAT);
}
