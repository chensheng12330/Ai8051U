/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_UART.h"
#include "AI8051U_USART_LIN.h"

//========================================================================
//                               ���ر�������
//========================================================================

//========================================================================
// ����: void UsartLinSendByte(u8 USARTx, u8 dat)
// ����: ����һ���ֽں�����
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: dat: ���͵�����.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendByte(u8 USARTx, u8 dat)
{
    if(USARTx == USART1)
    {
        COM1.B_TX_busy = 1;
        SBUF = dat;
        while(COM1.B_TX_busy);
    }
    else if(USARTx == USART2)
    {
        COM2.B_TX_busy = 1;
        S2BUF = dat;
        while(COM2.B_TX_busy);
    }
}

//========================================================================
// ����: void UsartLinSendData(u8 USARTx, u8 *pdat, u8 len)
// ����: Lin�������ݺ�����
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: *pdat: �������ݻ�����
// ����: len: ���ݳ���.
// ����: Lin ID.
// �汾: VER1.0
// ����: 2023-4-15
// ��ע: 
//========================================================================
void UsartLinSendData(u8 USARTx, u8 *pdat, u8 len)
{
    u8 i;

    if(len > 8) return; 
    for(i=0;i<len;i++)
    {
        UsartLinSendByte(USARTx,pdat[i]);
    }
}

//========================================================================
// ����: void UsartLinSendChecksum(u8 USARTx, u8 *dat, u8 len)
// ����: ����У���벢���͡�
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: *dat: ���ݳ����������
// ����: len: ���ݳ���.
// ����: none.
// �汾: VER1.0
// ����: 2023-4-15
// ��ע: 
//========================================================================
void UsartLinSendChecksum(u8 USARTx, u8 *dat, u8 len)
{
    u16 sum = 0;
    u8 i;

    if(len > 8) return; 
    for(i = 0; i < len; i++)
    {
        sum += dat[i];
        if(sum & 0xFF00)
        {
            sum = (sum & 0x00FF) + 1;
        }
    }
    sum ^= 0x00FF;
    UsartLinSendByte(USARTx,(u8)sum);
}

//========================================================================
// ����: void UsartSendBreak(u8 USARTx)
// ����: ��ģʽ����Lin����Break������
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartSendBreak(u8 USARTx)
{
    if(USARTx == USART1)
    {
        USARTCR5 |= 0x04;   //��ģʽ Send Break
    }
    else if(USARTx == USART2)
    {
        USART2CR5 |= 0x04;  //��ģʽ Send Break
    }
    UsartLinSendByte(USARTx,0x00);
}

//========================================================================
// ����: void UsartLinSendPID(u8 USARTx, u8 id)
// ����: ID�����У�����ת��PID�벢���͡�
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: ID��.
// ����: none.
// �汾: VER1.0
// ����: 2020-12-2
// ��ע: 
//========================================================================
void UsartLinSendPID(u8 USARTx, u8 id)
{
    u8 P0 ;
    u8 P1 ;
    
    P0 = (((id)^(id>>1)^(id>>2)^(id>>4))&0x01)<<6 ;
    P1 = ((~((id>>1)^(id>>3)^(id>>4)^(id>>5)))&0x01)<<7 ;
    
    UsartLinSendByte(USARTx,id|P0|P1);
}

//========================================================================
// ����: void UsartLinSendHeader(u8 USARTx, u8 lid)
// ����: Lin��������֡ͷ������
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: ID��.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendHeader(u8 USARTx, u8 lid)
{
    UsartSendBreak(USARTx);         //Send Break
    UsartLinSendByte(USARTx,0x55);  //Send Sync Field
    UsartLinSendPID(USARTx,lid);    //��������ID
}

//========================================================================
// ����: void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat, u8 len)
// ����: Lin������������֡������
// ����: USARTx: USART��ţ�ȡֵ��USART1����USART2
// ����: lid: Lin ID
// ����: *pdat: �������ݻ�����
// ����: len: ���ݳ���
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinSendFrame(u8 USARTx, u8 lid, u8 *pdat, u8 len)
{
    UsartSendBreak(USARTx);         //Send Break
    UsartLinSendByte(USARTx,0x55);  //Send Sync Field

    UsartLinSendPID(USARTx,lid);    //��������ID
    UsartLinSendData(USARTx,pdat,len);
    UsartLinSendChecksum(USARTx,pdat,len);
}

//========================================================================
// ����: void UsartLinBaudrate(u8 USARTx, u16 brt)
// ����: Lin���߲��������ú�����
// ����: brt: ������.
// ����: none.
// �汾: VER1.0
// ����: 2021-10-28
// ��ע: 
//========================================================================
void UsartLinBaudrate(u8 USARTx, u16 brt)
{
    u16 tmp;
    
    tmp = (MAIN_Fosc >> 4) / brt;
    if(USARTx == USART1)
    {
        USARTBRH = (u8)(tmp>>8);
        USARTBRL = (u8)tmp;
    }
    else if(USARTx == USART2)
    {
        USART2BRH = (u8)(tmp>>8);
        USART2BRL = (u8)tmp;
    }
}

//========================================================================
// ����: UASRT_LIN_Configuration
// ����: USART LIN��ʼ������.
// ����: USARTx: UART���, USART LIN�ṹ����,��ο�AI8051U_USART_LIN.h��Ķ���.
// ����: none.
// �汾: V1.0, 2022-03-30
//========================================================================
u8 UASRT_LIN_Configuration(u8 USARTx, USARTx_LIN_InitDefine *USART)
{
    if(USARTx == USART1)
    {
        SCON = (SCON & 0x3f) | 0x40;    //USART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
        SMOD = 1;
        TI = 0;
        REN = 1;    //�������
        ES  = 1;    //�����ж�

        if(USART->LIN_Enable == ENABLE)     USARTCR1 |= 0x80;   //ʹ��LINģ��
        else                                USARTCR1 &= ~0x80;  //�ر�LINģ��
        if(USART->LIN_Mode == LinSlaveMode) USARTCR5 |= 0x20;   //LINģ��ӻ�ģʽ
        else                                USARTCR5 &= ~0x20;  //LINģ������ģʽ
        if(USART->LIN_AutoSync == ENABLE)   USARTCR5 |= 0x10;   //ʹ���Զ�ͬ��
        else                                USARTCR5 &= ~0x10;  //�ر��Զ�ͬ��

        UsartLinBaudrate(USART1,USART->LIN_Baudrate);           //���ò�����

        if(USART->TimeOutEnable == ENABLE)   UR1TOCR |= 0x80;   //���ڽ��ճ�ʱ����ʹ��
        else                                 UR1TOCR &= ~0x80;  //���ڽ��ճ�ʱ���ܽ�ֹ

        if(USART->TimeOutINTEnable == ENABLE) UR1TOCR |= 0x40;  //���ڽ��ճ�ʱ�ж�ʹ��
        else                                  UR1TOCR &= ~0x40; //���ڽ��ճ�ʱ�жϽ�ֹ

        if(USART->TimeOutScale == TO_SCALE_SYSCLK)  UR1TOCR |= 0x20;  //��ʱ����ʱ��Դ��ϵͳʱ��
        else                                        UR1TOCR &= ~0x20; //��ʱ����ʱ��Դ����������λ��(������)

        if((USART->TimeOutTimer > 0) && (USART->TimeOutTimer <= 0xffffff))
        {
            UR1TOTL = (u8)USART->TimeOutTimer;
            UR1TOTH = (u8)(USART->TimeOutTimer>>8);
            UR1TOTE = (u8)(USART->TimeOutTimer>>16); //д URxTOTE ���µ�TMֵ�Ż���Ч
        }
        else return FAIL;//��ʱʱ�����

        return SUCCESS;
    }

    if(USARTx == USART2)
    {
        S2CON = (S2CON & 0x3f) | 0x50;
        T2x12 = 1;   //��ʱ��2ʱ��1Tģʽ
        T2R = 1;     //��ʼ��ʱ
        ES2 = 1;     //�����ж�
        S2CFG |= 0x80;  //S2MOD = 1

        if(USART->LIN_Enable == ENABLE)     USART2CR1 |= 0x80;  //ʹ��LINģ��
        else                                USART2CR1 &= ~0x80; //�ر�LINģ��
        if(USART->LIN_Mode == LinSlaveMode) USART2CR5 |= 0x20;  //LINģ��ӻ�ģʽ
        else                                USART2CR5 &= ~0x20; //LINģ������ģʽ
        if(USART->LIN_AutoSync == ENABLE)   USART2CR5 |= 0x10;  //ʹ���Զ�ͬ��
        else                                USART2CR5 &= ~0x10; //�ر��Զ�ͬ��

        UsartLinBaudrate(USART2,USART->LIN_Baudrate);           //���ò�����

        if(USART->TimeOutEnable == ENABLE)  UR2TOCR |= 0x80;    //���ڽ��ճ�ʱ����ʹ��
        else                                UR2TOCR &= ~0x80;   //���ڽ��ճ�ʱ���ܽ�ֹ

        if(USART->TimeOutINTEnable == ENABLE) UR2TOCR |= 0x40;  //���ڽ��ճ�ʱ�ж�ʹ��
        else                                  UR2TOCR &= ~0x40; //���ڽ��ճ�ʱ�жϽ�ֹ

        if(USART->TimeOutScale == TO_SCALE_SYSCLK)  UR2TOCR |= 0x20;  //��ʱ����ʱ��Դ��ϵͳʱ��
        else                                        UR2TOCR &= ~0x20; //��ʱ����ʱ��Դ����������λ��(������)

        if((USART->TimeOutTimer > 0) && (USART->TimeOutTimer <= 0xffffff))
        {
            UR2TOTL = (u8)USART->TimeOutTimer;
            UR2TOTH = (u8)(USART->TimeOutTimer>>8);
            UR2TOTE = (u8)(USART->TimeOutTimer>>16); //д URxTOTE ���µ�TMֵ�Ż���Ч
        }
        else return FAIL;   //��ʱʱ�����

        return SUCCESS;
    }
    return FAIL;    //����
}

/*********************************************************/
