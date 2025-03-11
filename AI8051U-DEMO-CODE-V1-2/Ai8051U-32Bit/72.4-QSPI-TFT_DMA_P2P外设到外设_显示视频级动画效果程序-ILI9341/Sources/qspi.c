#include "config.h"
#include "qspi.h"

#define QSPI_SW     2

#if (QSPI_SW == 2)
    sbit    CS          =   P4^0;
    sbit    SDI_IO0     =   P4^1;
    sbit    SDO_IO1     =   P4^2;
    sbit    SCK         =   P4^3;
    sbit    WP_IO2      =   P5^2;
    sbit    HLD_IO3     =   P5^3;
#else
    sbit    CS          =   P1^4;
    sbit    SDI_IO0     =   P1^5;
    sbit    SDO_IO1     =   P1^6;
    sbit    SCK         =   P1^7;
    sbit    WP_IO2      =   P1^3;
    sbit    HLD_IO3     =   P1^2;
#endif


void QSPI_Init()
{
#if (QSPI_SW == 2)
    P4M0 |= 0x09;               //����CS,SCKΪǿ����ģʽ
    P4M1 &= ~0x09;
    P4SR &= ~0x0f;              //�������е�QSPI��Ϊ����ģʽ
    P5SR &= ~0x0c;
    P4PU |= 0x0f;               //ʹ�����е�QSPI�ڵ��ڲ�10K��������
    P5PU |= 0x0c;
    P4BP &= ~0x06;              //ʹ��QSPI��IO0~IO3����Ӳ���Զ����ö˿�ģʽ
    P5BP &= ~0x0c;
    P_SW4 = (P_SW4 & ~0x03) | 0x01;
                                //ʹ��QSPI�ĵ�2���л�(P4.0/P4.1/P4.2/P4.3/P5.2/P5.3)
#else
    P1M0 |= 0x90;               //����CS,SCKΪǿ����ģʽ
    P1M1 &= ~0x90;
    P1SR &= ~0xfc;              //�������е�QSPI��Ϊ����ģʽ
    P1PU |= 0xfc;               //ʹ�����е�QSPI�ڵ��ڲ�10K��������
    P1BP &= ~0x6c;              //ʹ��QSPI��IO0~IO3����Ӳ���Զ����ö˿�ģʽ
    P_SW4 &= ~0x03;             //ʹ��QSPI�ĵ�1���л�(PP1.2/P1.3/P1.4/P1.5/P1.6/P1.7)
#endif
    
    CS = 1;
    SCK = 1;
    SDI_IO0 = 1;
    SDO_IO1 = 1;
    WP_IO2 = 1;
    HLD_IO3 = 1;

    while (QSPI_CheckBusy());       //���æ״̬
    
    QSPI_HCR1 = 0x00;               //����READ_HOLDʱ��1(0x00)
    QSPI_HCR2 = 0x00;               //����READ_HOLDʱ��2(0x00)
    QSPI_SetFIFOLevel(31);          //����FIFO��ֵΪ(31+1)=32�ֽ�
    QSPI_CR3 = 0x00;                //�ر�QSPI�ж�
    QSPI_SetClockDivider(3);        //����QSPIʱ��Ϊϵͳʱ��/(3+1)
    QSPI_SetCSHold(1);              //����CS����ʱ��Ϊ(1+1)=2��QSPIʱ��
    QSPI_SetSCKNormalHigh();        //����ʱSCKΪ�ߵ�ƽ
    QSPI_SetFlashSize(25);          //����Flash��СΪ2^(25+1)=64M�ֽ�,
    QSPI_InstructionAlways();       //����ÿ�����������ָ��
    
    QSPI_Enable();                  //ʹ��QSPI
}

void QSPI_WRITE_INSTR(BYTE cmd)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_READ_INSTR_SDATA(BYTE cmd, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_WRITE_INSTR_SADDR8(BYTE cmd, BYTE addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(0);         //���õ�ַ���Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_WRITE_INSTR_SADDR16(BYTE cmd, WORD addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(1);         //���õ�ַ���Ϊ16λ(1+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_WRITE_INSTR_SADDR24(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_WRITE_INSTR_SADDR32(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_WRITE_INSTR_QADDR32(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    
    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_SDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressDualMode();         //���õ�ַΪ˫��ģʽ
    QSPI_AlternateDualMode();       //���ü���ֽ�Ϊ˫��ģʽ
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_DADDR24_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_AlternateDualMode();       //���ü���ֽ�Ϊ˫��ģʽ
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressDualMode();         //���õ�ַΪ˫��ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressDualMode();         //���õ�ַΪ˫��ģʽ
    QSPI_AlternateDualMode();       //���ü���ֽ�Ϊ˫��ģʽ
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_DADDR32_DALT8_DDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_AlternateDualMode();       //���ü���ֽ�Ϊ˫��ģʽ
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressDualMode();         //���õ�ַΪ˫��ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_DDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataDualMode();            //��������Ϊ˫��ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_SADDR32_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_INSTR_QADDR32_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //д���ݵ�FIFO��
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_DMA_WRITE_INSTR_SADDR24_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (datalen)
    {
        QSPI_WriteData(*pdat);      //д���ݵ�FIFO��
        pdat++;
        datalen--;
    }
    
    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_DMA_WRITE_INSTR_SADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //д���ݵ�FIFO��
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_DMA_WRITE_INSTR_SADDR32_SDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    while (datalen)
    {
        QSPI_WriteData(*pdat);      //д���ݵ�FIFO��
        pdat++;
        datalen--;
    }

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_DMA_WRITE_INSTR_SADDR32_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(3);         //���õ�ַ���Ϊ32λ(3+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressSingMode();         //���õ�ַΪ����ģʽ
    
    QSPI_DMA_WRITE(pdat, datalen);
}

void QSPI_WRITE_QINSTR(BYTE cmd)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_READ_QINSTR_QDATA(BYTE cmd, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_WRITE_QINSTR_QADDR8(BYTE cmd, BYTE addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(0);         //���õ�ַ���Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_QINSTR_QADDR24_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־

    while (datalen)
    {
        *pdat = QSPI_ReadData();    //��FIFO�ж�ȡ����
        pdat++;
        datalen--;
    }
    
    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ_QINSTR_QADDR24_QALT8_DUMMY_QDATA(BYTE cmd, DWORD addr, BYTE alt, BYTE dcyc, BYTE *pdat, WORD datalen)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetDataLength(datalen-1);  //�������ݳ���
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetAlternateSize(0);       //���ü���ֽڿ��Ϊ8λ(0+1�ֽ�)
    QSPI_SetDummyCycles(dcyc);      //����DUMMYʱ��
    QSPI_NoInstruction();           //������ָ��ģʽ(��ֹ�󴥷�)
    QSPI_NoAddress();               //�����޵�ַģʽ(��ֹ�󴥷�)
    QSPI_AlternateQuadMode();       //���ü���ֽ�Ϊ����ģʽ
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetAlternate(alt);         //���ü���ֽ�
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ

    QSPI_DMA_READ(pdat, datalen);
}

void QSPI_WRITE_QINSTR_QADDR24(BYTE cmd, DWORD addr)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetWriteMode();            //дģʽ
    QSPI_SetAddressSize(2);         //���õ�ַ���Ϊ24λ(2+1�ֽ�)
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_AddressQuadMode();         //���õ�ַΪ����ģʽ
    QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_NoData();                  //������
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetAddress(addr);          //���õ�ַ

    while (!QSPI_CheckTransfer());  //�ȵ����ݴ������
    QSPI_ClearTransfer();           //���������ɱ�־
}

void QSPI_READ_QINSTR_QADDR24_QDATA(BYTE cmd, DWORD addr, BYTE *pdat, WORD datalen)
{
    QSPI_READ_QINSTR_QADDR24_DUMMY_QDATA(cmd, addr, 0, pdat, datalen);
}

void QSPI_POLLING_READ_INSTR_SDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetPollingMask(mask);      //������ѯ״̬����λ
    QSPI_SetPollingMatch(match);    //������ѯ״̬ƥ��λ
    QSPI_SetPollingInterval(clks);  //������ѯ����
    QSPI_PollingMatchAND();         //������ѯƥ��ģʽ
    QSPI_PollingAutoStop();         //������ѯ��ƥ��ʱ�Զ�ֹͣ��ѯ
    QSPI_SetDataLength(0);          //�������ݳ���
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionSingMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataSingMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetPollingMode();          //��ѯģʽ

    while (!QSPI_CheckMatch());     //�ȵ���ѯ���
    QSPI_ClearMatch();              //�����ѯ��ɱ�־

    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_POLLING_READ_QINSTR_QDATA(BYTE cmd, BYTE mask, BYTE match, WORD clks)
{
    while (QSPI_CheckBusy());       //���æ״̬

    QSPI_SetReadMode();             //��ģʽ
    QSPI_SetPollingMask(mask);      //������ѯ״̬����λ
    QSPI_SetPollingMatch(match);    //������ѯ״̬ƥ��λ
    QSPI_SetPollingInterval(clks);  //������ѯ����
    QSPI_PollingMatchAND();         //������ѯƥ��ģʽ
    QSPI_PollingAutoStop();         //������ѯ��ƥ��ʱ�Զ�ֹͣ��ѯ
    QSPI_SetDataLength(0);          //�������ݳ���
    QSPI_SetDummyCycles(0);         //����DUMMYʱ��
    QSPI_InstructionQuadMode();     //����ָ��Ϊ����ģʽ
    QSPI_NoAddress();               //�޵�ַ�ֽ�
  	QSPI_NoAlternate();             //�޼���ֽ�
    QSPI_DataQuadMode();            //��������Ϊ����ģʽ
    QSPI_SetInstruction(cmd);       //����ָ��
    QSPI_SetPollingMode();          //��ѯģʽ

    while (!QSPI_CheckMatch());     //�ȵ���ѯ���
    QSPI_ClearMatch();              //�����ѯ��ɱ�־

    while (QSPI_CheckFIFOLevel())   //���FIFO
        QSPI_ReadData();
}

void QSPI_DMA_READ(BYTE *pdat, WORD datalen)
{
    DMA_QSPI_AMT = datalen-1;       //����DMA���ݳ���
    DMA_QSPI_AMTH = (datalen-1) >> 8;
    DMA_QSPI_RXAH = (WORD)pdat >> 8;//����DMA�Ĵ洢����ʼ��ַ
    DMA_QSPI_RXAL = (BYTE)pdat;     //����DMA�Ĵ洢����ʼ��ַ
    DMA_QSPI_STA = 0x00;            //���DMA״̬
    DMA_QSPI_CFG = 0x20;            //ʹ��DMA��ȡ����
    DMA_QSPI_CR = 0xa1;             //����DMA������QSPI������
    while (!(DMA_QSPI_STA & 0x01)); //�ȴ�DMA�������
    DMA_QSPI_STA = 0x00;            //���DMA״̬
    DMA_QSPI_CFG = 0x00;
    DMA_QSPI_CR = 0x00;
}

void QSPI_DMA_WRITE(BYTE *pdat, WORD datalen)
{
    DMA_QSPI_AMT = datalen-1;       //����DMA���ݳ���
    DMA_QSPI_AMTH = (datalen-1) >> 8;
    DMA_QSPI_TXAH = (WORD)pdat >> 8;//����DMA�Ĵ洢����ʼ��ַ
    DMA_QSPI_TXAL = (BYTE)pdat;     //����DMA�Ĵ洢����ʼ��ַ
    DMA_QSPI_STA = 0x00;            //���DMA״̬
    DMA_QSPI_CFG = 0x40;            //ʹ��DMAд����
    DMA_QSPI_CR = 0xc2;             //����DMA������QSPIд����
    while (!(DMA_QSPI_STA & 0x01)); //�ȴ�DMA�������
    DMA_QSPI_STA = 0x00;            //���DMA״̬
    DMA_QSPI_CFG = 0x00;
    DMA_QSPI_CR = 0x00;
}
