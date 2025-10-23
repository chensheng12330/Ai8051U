/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ��������˵��  **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

���¶�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

��������UART1(P3.0,P3.1): 115200,N,8,1.

����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

/***********************************************************/

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

/****************************** �û������ ***********************************/

#define MAIN_Fosc       24000000L   //������ʱ��
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2
#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

/*****************************************************************************/

sbit SDA = P3^3;
sbit SCL = P3^2;

/*************  ���س�������    **************/

#define SLAW        0xA0
#define SLAR        0xA1

/*************  ���ر�������    **************/


/*************  ���غ�������    **************/
void WriteNbyte(u8 addr, u8 *p, u8 number);
void ReadNbyte( u8 addr, u8 *p, u8 number);
void delay_ms(u8 ms);
void UartInit(void);

/**********************************************/
void main(void)
{
    u8  i;
    u8  tmp[8];

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
    
    UartInit();

    I2C_S1 =1;      //I2C���ܽ�ѡ��00:P2.4,P2.3; 01:P1.5,P1.4; 11:P3.2,P3.3
    I2C_S0 =1;
    I2CCFG = 0xc2;  //ʹ��I2C����ģʽ
    I2CPSCR = 0x00; //MSSPEED[13:6]
    I2CMSST = 0x00;

    EA = 1;         //�����ж�
    
    ReadNbyte(0, tmp, 8);
    printf("Read1 = ");     //��ӡ��һ�ζ�ȡ����
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
        tmp[i]++;
    }
    printf("\r\n");

    WriteNbyte(0, tmp, 8);  //д���µ�����
    delay_ms(250);
    delay_ms(250);

    ReadNbyte(0, tmp, 8);
    printf("Read2 = ");     //��ӡ�ڶ��ζ�ȡ����
    for(i=0; i<8; i++)
    {
        printf("%02x ",tmp[i]);
    }
    printf("\r\n");

    while(1);
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

/********************** I2C���� ************************/
void Wait()
{
    while (!(I2CMSST & 0x40));
    I2CMSST &= ~0x40;
}

void Start()
{
    I2CMSCR = 0x01;                         //����START����
    Wait();
}

void SendData(char dat)
{
    I2CTXD = dat;                           //д���ݵ����ݻ�����
    I2CMSCR = 0x02;                         //����SEND����
    Wait();
}

void RecvACK()
{
    I2CMSCR = 0x03;                         //���Ͷ�ACK����
    Wait();
}

char RecvData()
{
    I2CMSCR = 0x04;                         //����RECV����
    Wait();
    return I2CRXD;
}

void SendACK()
{
    I2CMSST = 0x00;                         //����ACK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void SendNAK()
{
    I2CMSST = 0x01;                         //����NAK�ź�
    I2CMSCR = 0x05;                         //����ACK����
    Wait();
}

void Stop()
{
    I2CMSCR = 0x06;                         //����STOP����
    Wait();
}

void WriteNbyte(u8 addr, u8 *p, u8 number)  /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //������ʼ����
    SendData(SLAW);                         //�����豸��ַ+д����
    RecvACK();
    SendData(addr);                         //���ʹ洢��ַ
    RecvACK();
    do
    {
        SendData(*p++);
        RecvACK();
    }
    while(--number);
    Stop();                                 //����ֹͣ����
}

void ReadNbyte(u8 addr, u8 *p, u8 number)   /*  WordAddress,First Data Address,Byte lenth   */
{
    Start();                                //������ʼ����
    SendData(SLAW);                         //�����豸��ַ+д����
    RecvACK();
    SendData(addr);                         //���ʹ洢��ַ
    RecvACK();
    Start();                                //������ʼ����
    SendData(SLAR);                         //�����豸��ַ+������
    RecvACK();
    do
    {
        *p = RecvData();
        p++;
        if(number != 1) SendACK();          //send ACK
    }
    while(--number);
    SendNAK();                              //send no ACK	
    Stop();                                 //����ֹͣ����
}

/******************** ���ڴ�ӡ���� ********************/
void UartInit(void)
{
#if(PrintUart == 1)
    S1_S1 = 0;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    S1_S0 = 0;
	SCON = (SCON & 0x3f) | 0x40; 
	T1x12 = 1;      //��ʱ��ʱ��1Tģʽ
	S1BRT = 0;      //����1ѡ��ʱ��1Ϊ�����ʷ�����
	TL1  = TM;
	TH1  = TM>>8;
	TR1 = 1;        //��ʱ��1��ʼ��ʱ

//	SCON = (SCON & 0x3f) | 0x40; 
//	T2L  = TM;
//	T2H  = TM>>8;
//	AUXR |= 0x15;   //����1ѡ��ʱ��2Ϊ�����ʷ�����
#else
	S2_S = 1;       //UART2 switch to: 0: P1.2 P1.3,  1: P4.2 P4.3
    S2CFG |= 0x01;  //ʹ�ô���2ʱ��W1λ��������Ϊ1��������ܻ��������Ԥ�ڵĴ���
	S2CON = (S2CON & 0x3f) | 0x40; 
	T2L  = TM;
	T2H  = TM>>8;
	AUXR |= 0x14;	      //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
#endif
}

void UartPutc(unsigned char dat)
{
#if(PrintUart == 1)
	SBUF = dat; 
	while(TI==0);
	TI = 0;
#else
	S2BUF  = dat; 
	while(S2TI == 0);
	S2TI = 0;    //Clear Tx flag
#endif
}

char putchar(char c)
{
	UartPutc(c);
	return c;
}
