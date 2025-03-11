/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ��������˵��  **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�Ƚ���������ͨ�� P4.6 �˿����ӿɵ����裬��1.19V�ڲ��̶��Ƚϣ�

���ڿɵ����裬��P4.6�ڵ������ѹ����1.19Vʱ�����Ƚ����жϣ���ʱ�������ݵ�EEPROM��

��P4.6�ڵ������ѹ����1.19V�󣬻��������ϵ�ʱ����ȡEEPROM���ݽ��лָ���

��STC��MCU��IO��ʽ����74HC595����8λ����ܡ�

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������,�û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.

�û�������"�û������"���޸ı������ݵ�EEPROM��ַ��

��ʾЧ��Ϊ: �ϵ����ʾ�����, ������ΧΪ0~10000����ʾ���ұߵ�5������ܡ�

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ע�⣺����ʱ�����ؽ���"Ӳ��ѡ��"���������Ҫȷ����������:

�����û�EEPROM��С����ȷ��"�û������"���趨�ĵ�ַ��EEPROM���õĴ�С��Χ֮�ڣ�EEPROM��С�����û�ϵͳ����
������Ҫ�û�ϵͳ��4K��EEPROM��8K����ô�û�EEPROM��С��Ҫ����12K��

�޸Ĺ�Ӳ��ѡ���Ҫ����Ƭ�������ϵ��Ż���Ч��

ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��)��
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** �û������ ***********************************/

#define     MAIN_Fosc       24000000L   //������ʱ��

#define     EE_ADDRESS      0x000000  //�����EEPROM��ʼ��ַ

#define     Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define     Tip_Delay       (MAIN_Fosc / 1000000)

/*****************************************************************************/

#define DIS_DOT     0x20
#define DIS_BLACK   0x10
#define DIS_        0x11

/*************  IO�ڶ���    **************/
sbit    P_HC595_SER   = P3^4;   //pin 14    SER     data input
sbit    P_HC595_RCLK  = P3^5;   //pin 12    RCLk    store (latch) clock
sbit    P_HC595_SRCLK = P3^2;   //pin 11    SRCLK   Shift data clock

/*************  ���س�������    **************/
u8 code t_display[]={                       //��׼�ֿ�
//   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,
//black  -     H    J    K    L    N    o   P    U     t    G    Q    r   M    y
    0x00,0x40,0x76,0x1E,0x70,0x38,0x37,0x5C,0x73,0x3E,0x78,0x3d,0x67,0x50,0x37,0x6e,
    0xBF,0x86,0xDB,0xCF,0xE6,0xED,0xFD,0x87,0xFF,0xEF,0x46};    //0. 1. 2. 3. 4. 5. 6. 7. 8. 9. -1

u8 code T_COM[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};      //λ��


u8  LED8[8];        //��ʾ����
u8  display_index;  //��ʾλ����
bit B_1ms;          //1ms��־
u16 msecond;
bit LowVolFlag;     //�͵�ѹ��־

u16 Test_cnt;   //�����õ����������
u16 Temp_cnt;   //�����õ����������
u8  tmp[2];     //ͨ������

void CMP_config(void);
void Display(void);
void DisableEEPROM(void);
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_SectorErase(u32 EE_address);

/******************* �Ƚ����жϺ��� ********************/
void CMP_Isr() interrupt 21
{
    u8  i;

    CMPIF = 0;          //���жϱ�־
    P42 = CMPRES;       //�жϷ�ʽ��ȡ�Ƚ����ȽϽ��
    
    if(CMPRES)
    {
        if(LowVolFlag)
        {
            LowVolFlag = 0;    //����͵�ѹ��־
            if(Test_cnt != Temp_cnt)
            {
                EEPROM_read_n(EE_ADDRESS,tmp,2);        //����2�ֽ�
                Test_cnt = ((u16)tmp[0] << 8) + tmp[1]; //�����
                if(Test_cnt > 10000)    Test_cnt = 0;   //�������ΧΪ0~10000
                Temp_cnt = Test_cnt;
            }
        }
    }
    else
    {
        P_HC595_SER = 0;
        for(i=0; i<16; i++)     //�ȹر���ʾ��ʡ��
        {
            P_HC595_SRCLK = 1;
            P_HC595_SRCLK = 0;
        }
        P_HC595_RCLK = 1;
        P_HC595_RCLK = 0;       //�����������

        if(!LowVolFlag)
        {
            LowVolFlag = 1;    //���õ͵�ѹ��־
            if(Test_cnt != Temp_cnt)
            {
                Temp_cnt = Test_cnt;
                EEPROM_SectorErase(EE_ADDRESS); //��������
                tmp[0] = (u8)(Temp_cnt >> 8);
                tmp[1] = (u8)Temp_cnt;
                EEPROM_write_n(EE_ADDRESS,tmp,2);
            }
        }
    }
}

/********************** ������ ************************/
void main(void)
{
    u8  i;
    
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

    display_index = 0;
    for(i=0; i<8; i++)  LED8[i] = DIS_BLACK;    //ȫ������
    
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run

    CMP_config();
    EA = 1;     //�����ж�
    
    for(msecond=0; msecond < 200; ) //��ʱ200ms
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            msecond ++;
        }
    }

    EEPROM_read_n(EE_ADDRESS,tmp,2);        //����2�ֽ�
    Test_cnt = ((u16)tmp[0] << 8) + tmp[1]; //�����
    if(Test_cnt > 10000)    Test_cnt = 0;   //�������ΧΪ0~10000
    
    Display();      //��ʾ�����
    
    while(1)
    {
        if((B_1ms) && (LowVolFlag == 0))   //1ms�����͵�ѹʱ������
        {
            B_1ms = 0;
            if(++msecond >= 1000)   //1�뵽
            {
                msecond = 0;        //��1000ms����
                Test_cnt++;         //�����+1
                if(Test_cnt > 10000)    Test_cnt = 0;   //�������ΧΪ0~10000
                Display();          //��ʾ�����
            }
        }
    }
}

//========================================================================
// ����: void CMP_config(void)
// ����: �Ƚ�����ʼ��������
// ����: ��.
// ����: ��.
// �汾: V1.0, 2020-6-10
//========================================================================
void CMP_config(void)
{
    CMPEXCFG = 0x00;
//  CMPEXCFG |= 0x40;       //�Ƚ���DC��������ѡ��0:0mV; 0x40:10mV; 0x80:20mV; 0xc0:30mV

//  CMPEXCFG &= ~0x04;      //P4.4ΪCMP-�����
    CMPEXCFG |= 0x04;       //�ڲ�1.19V�ο���ѹΪCMP-�����

    CMPEXCFG &= ~0x03;      //P4.6ΪCMP+�����
//  CMPEXCFG |= 0x01;       //P5.0ΪCMP+�����
//  CMPEXCFG |= 0x02;       //P5.1ΪCMP+�����
//  CMPEXCFG |= 0x03;       //ADC�����ΪCMP+�����

    CMPCR2 = 0x00;
    INVCMPO = 0;            //�Ƚ����������
//  INVCMPO = 1;            //�Ƚ����������
    DISFLT = 0;             //ʹ��0.1us�˲�
//  DISFLT = 1;             //��ֹ0.1us�˲�
//  CMPCR2 &= ~0x3f;        //�Ƚ������ֱ�����
    CMPCR2 |= 0x10;         //�Ƚ����������16��ȥ��ʱ�Ӻ����

    CMPCR1 = 0x00;
//  PIE = 0;                //��ֹ�Ƚ����������ж�
    PIE = 1;                //ʹ�ܱȽ����������ж�
//  NIE = 0;                //��ֹ�Ƚ����½����ж�
    NIE = 1;                //ʹ�ܱȽ����½����ж�

//  CMPOE = 0;              //��ֹ�Ƚ������
    CMPOE = 1;              //ʹ�ܱȽ������

    CMPO_S = 0;             //ѡ��P4.5��Ϊ�Ƚ��������
//  CMPO_S = 1;             //ѡ��P4.1��Ϊ�Ƚ��������
    CMPEN = 1;              //ʹ�ܱȽ���ģ��
}

/********************** ��ʾ�������� ************************/
void Display(void)
{
    u8  i;
    
    LED8[3] = Test_cnt / 10000;
    LED8[4] = (Test_cnt % 10000) / 1000;
    LED8[5] = (Test_cnt % 1000) / 100;
    LED8[6] = (Test_cnt % 100) / 10;
    LED8[7] = Test_cnt % 10;
    
    for(i=3; i<7; i++)  //����Ч0
    {
        if(LED8[i] > 0) break;
        LED8[i] = DIS_BLACK;
    }
}

//========================================================================
// ����: void   ISP_Disable(void)
// ����: ��ֹ����ISP/IAP.
// ����: non.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void DisableEEPROM(void)
{
    IAP_CONTR = 0;          //�ر� IAP ����
    IAP_CMD = 0;            //�������Ĵ���
    IAP_TRIG = 0;           //��������Ĵ���
    IAP_ADDRE = 0xff;       //����ַ���õ��� IAP ����
    IAP_ADDRH = 0xff;       //����ַ���õ��� IAP ����
    IAP_ADDRL = 0xff;
}

//========================================================================
// ����: void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number)
// ����: ��ָ��EEPROM�׵�ַ����n���ֽڷ�ָ���Ļ���.
// ����: EE_address:  ����EEPROM���׵�ַ.
//       DataAddress: �������ݷŻ�����׵�ַ.
//       number:      �������ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number)
{
    EA = 0;     //��ֹ�ж�

    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = Tip_Delay; //���ò����ȴ����� 24MHz
    IAP_CMD = 1;  //���� IAP ������
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRH = (u8)(EE_address >> 8);  //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRL = (u8)EE_address;         //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_TRIG = 0x5a; //д��������(0x5a)
        IAP_TRIG = 0xa5; //д��������(0xa5)
        _nop_();   //�༶��ˮ�ߵ�ָ��ϵͳ��������������4��NOP����֤IAP_DATA���������׼��
        _nop_();
        _nop_();
        _nop_();
        *DataAddress = IAP_DATA; //�� IAP ����
        EE_address++;
        DataAddress++;
    }while(--number);
    DisableEEPROM();
    EA = 1;     //���������ж�
}

//========================================================================
// ����: void EEPROM_SectorErase(u32 EE_address)
// ����: ��ָ����ַ��EEPROM��������.
// ����: EE_address:  Ҫ����������EEPROM�ĵ�ַ.
// ����: non.
// �汾: V1.0, 2013-5-10
//========================================================================
void EEPROM_SectorErase(u32 EE_address)
{
    EA = 0;     //��ֹ�ж�

    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = Tip_Delay; //���ò����ȴ����� 24MHz
    IAP_CMD = 3;      //���� IAP ��������
    IAP_ADDRE = (u8)(EE_address >> 16); //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    IAP_ADDRH = (u8)(EE_address >> 8);  //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    IAP_ADDRL = (u8)EE_address;         //��������ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    IAP_TRIG = 0x5a; //д��������(0x5a)
    IAP_TRIG = 0xa5; //д��������(0xa5)
    _nop_();   //�༶��ˮ�ߵ�ָ��ϵͳ��������������4��NOP����֤IAP_DATA���������׼��
    _nop_();
    _nop_();
    _nop_();
    DisableEEPROM();
    EA = 1;     //���������ж�
}

//========================================================================
// ����: void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number)
// ����: �ѻ����n���ֽ�д��ָ���׵�ַ��EEPROM.
// ����: EE_address:  д��EEPROM���׵�ַ.
//       DataAddress: д��Դ���ݵĻ�����׵�ַ.
//       number:      д����ֽڳ���.
// ����: non.
// �汾: V1.0, 2012-10-22
//========================================================================
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number)
{
    EA = 0;     //��ֹ�ж�

    IAP_CONTR = 0x80; //ʹ�� IAP
    IAP_TPS = Tip_Delay; //���ò����ȴ����� 24MHz
    IAP_CMD = 2;  //���� IAP д����
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRH = (u8)(EE_address >> 8);  //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_ADDRL = (u8)EE_address;         //�͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
        IAP_DATA = *DataAddress; //д IAP ����
        IAP_TRIG = 0x5a; //д��������(0x5a)
        IAP_TRIG = 0xa5; //д��������(0xa5)
        _nop_();   //�༶��ˮ�ߵ�ָ��ϵͳ��������������4��NOP����֤IAP_DATA���������׼��
        _nop_();
        _nop_();
        _nop_();
        EE_address++;
        DataAddress++;
    }while(--number);

    DisableEEPROM();
    EA = 1;     //���������ж�
}

/**************** ��HC595����һ���ֽں��� ******************/
void Send_595(u8 dat)
{
    u8  i;
    for(i=0; i<8; i++)
    {
        dat <<= 1;
        P_HC595_SER   = CY;
        P_HC595_SRCLK = 1;
        P_HC595_SRCLK = 0;
    }
}

/********************** ��ʾɨ�躯�� ************************/
void DisplayScan(void)
{   
    Send_595(t_display[LED8[display_index]]);   //�������
    Send_595(~T_COM[display_index]);            //���λ��

    P_HC595_RCLK = 1;
    P_HC595_RCLK = 0;
    if(++display_index >= 8)    display_index = 0;  //8λ������0
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    if(LowVolFlag == 0)
    {
        DisplayScan();  //1msɨ����ʾһλ
        B_1ms = 1;      //1ms��־
    }
}
