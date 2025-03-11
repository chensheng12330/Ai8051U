/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ��������˵��  **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ������1(P3.0 P3.1)��STC�ڲ��Դ���EEPROM(FLASH)���ж�д���ԡ�

��FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,8,N,1. 
Ĭ����ʱ��:  22.1184MHz.

������������: (������ĸ�����ִ�Сд)
    W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.
    R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����. 

ע�⣺����ʱ�����ؽ���"Ӳ��ѡ��"�������û�EEPROM��С��EEPROM��С�����û�ϵͳ��
������Ҫ�û�ϵͳ��4K��EEPROM��8K����ô�û�EEPROM��С��Ҫ����12K��

��ȷ�����������еĵ�ַ��EEPROM���õĴ�С��Χ֮�ڡ�

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"

#define     MAIN_Fosc       22118400L   //������ʱ�ӣ���ȷ����115200�����ʣ�

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

#define Baudrate1   (65536 - MAIN_Fosc / 115200 / 4)
#define Tmp_Length  100      //��дEEPROM���峤��

#define UART1_BUF_LENGTH    (Tmp_Length+11)  //���ڻ��峤��

u8  RX1_TimeOut;
u8  TX1_Cnt;    //���ͼ���
u8  RX1_Cnt;    //���ռ���
bit B_TX1_Busy; //����æ��־

u8  RX1_Buffer[UART1_BUF_LENGTH];   //���ջ���
u8  tmp[Tmp_Length];        //EEPROM��������


void    UART1_config(u8 brt);   // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
void    PrintString1(u8 *puts);
void    UART1_TxByte(u8 dat);
void    delay_ms(u8 ms);
u8      CheckData(u8 dat);
u32     GetAddress(void);
u8      GetDataLength(void);
void    EEPROM_SectorErase(u32 EE_address);
void    EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 length);
u8      EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length);


/********************* ������ *************************/
void main(void)
{
    u8  i,j;
    u32 addr;
    u8  status;

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

    UART1_config(1);    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    EA = 1; //�������ж�

    PrintString1("AI8051Uϵ�е�Ƭ��EEPROM���Գ��򣬴���������������:\r\n");    //UART1����һ���ַ���
    PrintString1("W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.\r\n");   //UART1����һ���ַ���
    PrintString1("R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����.\r\n"); //UART1����һ���ַ���

    while(1)
    {
        delay_ms(1);
        if(RX1_TimeOut > 0)     //��ʱ����
        {
            if(--RX1_TimeOut == 0)
            {
              for(i=0; i<RX1_Cnt; i++)    UART1_TxByte(RX1_Buffer[i]);    //���յ�������ԭ������,���ڲ���

                status = 0xff;  //״̬��һ����0ֵ
                if((RX1_Cnt >= 12) && (RX1_Buffer[1] == ' ') && (RX1_Buffer[10] == ' ')) //�������Ϊ10���ֽ�
                {
                    for(i=0; i<10; i++)
                    {
                        if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';  //Сдת��д
                    }
                    addr = GetAddress();
                    if(addr < 0x00ffffff)
                    {
                        if(RX1_Buffer[0] == 'W')    //д��N���ֽ�
                        {
                            j = RX1_Cnt - 11;
                            if(j > Tmp_Length)  j = Tmp_Length; //Խ����
                            EEPROM_SectorErase(addr);           //��������
                            i = EEPROM_write_n(addr,&RX1_Buffer[11],j);      //дN���ֽ�
                            if(i == 0)
                            {
                                PrintString1("\r\n��д��");
                                if(j >= 100)    {UART1_TxByte((u8)(j/100+'0'));   j = j % 100;}
                                if(j >= 10)     {UART1_TxByte((u8)(j/10+'0'));    j = j % 10;}
                                UART1_TxByte((u8)(j%10+'0'));
                                PrintString1("�ֽڣ�\r\n");
                            }
                            else    PrintString1("\r\nд�����\r\n");
                            status = 0; //������ȷ
                        }

                        else if(RX1_Buffer[0] == 'R')   //PC���󷵻�N�ֽ�EEPROM����
                        {
                            j = GetDataLength();
                            if(j > Tmp_Length)  j = Tmp_Length; //Խ����
                            if(j > 0)
                            {
                                PrintString1("\r\n����");
                                UART1_TxByte((u8)(j/10+'0'));
                                UART1_TxByte((u8)(j%10+'0'));
                                PrintString1("���ֽ���������:\r\n");
                                EEPROM_read_n(addr,tmp,j);
                                for(i=0; i<j; i++)  UART1_TxByte(tmp[i]);
                                UART1_TxByte(0x0d);
                                UART1_TxByte(0x0a);
                                status = 0; //������ȷ
                            }
                        }
                    }
                }
                if(status != 0) PrintString1("\r\n�������\r\n");
                RX1_Cnt  = 0;   //����ֽ���
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

//========================================================================
// ����: u8 CheckData(u8 dat)
// ����: ���ַ�"0~9,A~F��a~f"ת��ʮ������.
// ����: dat: Ҫ�����ַ�.
// ����: 0x00~0x0FΪ��ȷ. 0xFFΪ����.
// �汾: V1.0, 2012-10-22
//========================================================================
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

//========================================================================
// ����: u32 GetAddress(void)
// ����: ����������뷽ʽ�ĵ�ַ.
// ����: ��.
// ����: 24λEEPROM��ַ.
// �汾: V1.0, 2013-6-6
//========================================================================
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
            if(j >= 0x10)   return 0xffffffff;   //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0xffffffff;  //error
}

/**************** ��ȡҪ�������ݵ��ֽ��� ****************************/
u8 GetDataLength(void)
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


//========================================================================
// ����: void UART1_TxByte(u8 dat)
// ����: ����һ���ֽ�.
// ����: ��.
// ����: ��.
// �汾: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    SBUF = dat;
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}


//========================================================================
// ����: void PrintString1(u8 *puts)
// ����: ����2�����ַ���������
// ����: puts:  �ַ���ָ��.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void PrintString1(u8 *puts) //����һ���ַ���
{
    for (; *puts != 0;  puts++) UART1_TxByte(*puts);    //����ֹͣ��0����
}

//========================================================================
// ����: void SetTimer2Baudraye(u16 dat)
// ����: ����Timer2�������ʷ�������
// ����: dat: Timer2����װֵ.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ��Ч.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //��ֹ�ж�
    T2R = 1;    //Timer run enable
}

//========================================================================
// ����: void UART1_config(u8 brt)
// ����: UART1��ʼ��������
// ����: brt: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void UART1_config(u8 brt)    // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
{
    /*********** ������ʹ�ö�ʱ��2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)Baudrate1);
    }

    /*********** ������ʹ�ö�ʱ��1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)(Baudrate1 / 256);
        TL1 = (u8)(Baudrate1 % 256);
        ET1 = 0;    //��ֹ�ж�
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
//  PS  = 1;    //�����ȼ��ж�
    ES  = 1;    //�����ж�
    REN = 1;    //�������
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
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
        RI = 0;     //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;     //Clear Tx flag
        B_TX1_Busy = 0;
    }
}


#define     IAP_STANDBY()   IAP_CMD = 0     //IAP���������ֹ��
#define     IAP_READ()      IAP_CMD = 1     //IAP��������
#define     IAP_WRITE()     IAP_CMD = 2     //IAPд������
#define     IAP_ERASE()     IAP_CMD = 3     //IAP��������

#define     IAP_ENABLE()    IAP_CONTR = IAP_EN; IAP_TPS = MAIN_Fosc / 1000000
#define     IAP_DISABLE()   IAP_CONTR = 0; IAP_CMD = 0; IAP_TRIG = 0; IAP_ADDRE = 0xff; IAP_ADDRH = 0xff; IAP_ADDRL = 0xff

#define IAP_EN          (1<<7)
#define IAP_SWBS        (1<<6)
#define IAP_SWRST       (1<<5)
#define IAP_CMD_FAIL    (1<<4)


//========================================================================
// ����: void DisableEEPROM(void)
// ����: ��ֹEEPROM.
// ����: none.
// ����: none.
// �汾: V1.0, 2014-6-30
//========================================================================
void DisableEEPROM(void)        //��ֹ����EEPROM
{
    IAP_CONTR = 0;          //�ر� IAP ����
    IAP_CMD = 0;            //�������Ĵ���
    IAP_TRIG = 0;           //��������Ĵ���
    IAP_ADDRE = 0xff;       //����ַ���õ��� IAP ����
    IAP_ADDRH = 0xff;       //����ַ���õ��� IAP ����
    IAP_ADDRL = 0xff;
}

//========================================================================
// ����: void EEPROM_Trig(void)
// ����: ����EEPROM����.
// ����: none.
// ����: none.
// �汾: V1.0, 2014-6-30
//========================================================================
void EEPROM_Trig(void)
{
    F0 = EA;    //����ȫ���ж�
    EA = 0;     //��ֹ�ж�, ���ⴥ��������Ч
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;    //����5AH������A5H��IAP�����Ĵ�����ÿ�ζ���Ҫ���
                        //����A5H��IAP������������������
                        //CPU�ȴ�IAP��ɺ󣬲Ż����ִ�г���
    _nop_();   //�༶��ˮ�ߵ�ָ��ϵͳ��������������4��NOP����֤IAP_DATA���������׼��
    _nop_();
    _nop_();
    _nop_();
    EA = F0;    //�ָ�ȫ���ж�
}

//========================================================================
// ����: void EEPROM_SectorErase(u32 EE_address)
// ����: ����һ������.
// ����: EE_address:  Ҫ������EEPROM�������е�һ���ֽڵ�ַ.
// ����: none.
// �汾: V1.0, 2014-6-30
//========================================================================
void EEPROM_SectorErase(u32 EE_address)
{
    IAP_ENABLE();                       //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
    IAP_ERASE();                        //�����, ������������������ı�ʱ����������������
                                        //ֻ������������û���ֽڲ�����512�ֽ�/������
                                        //����������һ���ֽڵ�ַ����������ַ��
    IAP_ADDRE = (u8)(EE_address >> 16); //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    IAP_ADDRH = (u8)(EE_address >> 8);  //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    IAP_ADDRL = (u8)EE_address;         //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    EEPROM_Trig();                      //����EEPROM����
    DisableEEPROM();                    //��ֹEEPROM����
}

//========================================================================
// ����: void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 lenth)
// ����: ��N���ֽں���.
// ����: EE_address:  Ҫ������EEPROM���׵�ַ.
//       DataAddress: Ҫ�������ݵ�ָ��.
//       length:      Ҫ�����ĳ���
// ����: 0: д����ȷ.  1: д�볤��Ϊ0����.  2: д�����ݴ���.
// �汾: V1.0, 2014-6-30
//========================================================================
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 length)
{
    IAP_ENABLE();                           //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
    IAP_READ();                             //���ֽڶ���������ı�ʱ����������������
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRH = (u8)(EE_address >> 8);  //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRL = (u8)EE_address;         //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        EEPROM_Trig();                      //����EEPROM����
        *DataAddress = IAP_DATA;            //��������������
        EE_address++;
        DataAddress++;
    }while(--length);

    DisableEEPROM();
}


//========================================================================
// ����: u8 EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length)
// ����: дN���ֽں���.
// ����: EE_address:  Ҫд���EEPROM���׵�ַ.
//       DataAddress: Ҫд�����ݵ�ָ��.
//       length:      Ҫд��ĳ���
// ����: 0: д����ȷ.  1: д�볤��Ϊ0����.  2: д�����ݴ���.
// �汾: V1.0, 2014-6-30
//========================================================================
u8 EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length)
{
    u8  i;
    u16 j;
    u8  *p;
    
    if(length == 0) return 1;   //����Ϊ0����

    IAP_ENABLE();                       //���õȴ�ʱ�䣬����IAP��������һ�ξ͹�
    i = length;
    j = EE_address;
    p = DataAddress;
    IAP_WRITE();                            //�����, ���ֽ�д����
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRH = (u8)(EE_address >> 8);  //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRL = (u8)EE_address;         //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_DATA  = *DataAddress;           //�����ݵ�IAP_DATA��ֻ�����ݸı�ʱ����������
        EEPROM_Trig();                      //����EEPROM����
        EE_address++;                       //��һ����ַ
        DataAddress++;                      //��һ������
    }while(--length);                       //ֱ������

    EE_address = j;
    length = i;
    DataAddress = p;
    i = 0;
    IAP_READ();                             //��N���ֽڲ��Ƚ�
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRH = (u8)(EE_address >> 8);  //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRL = (u8)EE_address;         //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        EEPROM_Trig();                      //����EEPROM����
        if(*DataAddress != IAP_DATA)        //������������Դ���ݱȽ�
        {
            i = 2;
            break;
        }
        EE_address++;
        DataAddress++;
    }while(--length);

    DisableEEPROM();
    return i;
}

