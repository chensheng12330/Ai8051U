/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�û������ں궨���иı�MCU��ʱ��Ƶ��. ��Χ 8MHZ ~ 33MHZ.

������ճ���ģ���г�����������NEC�ı��롣

�û������ں궨����ָ���û���.

ʹ��PWM4����38KHZ�ز�, 1/3ռ�ձ�, ÿ��38KHZ���ڷ���ܷ���9us,�ر�16.3us.

ʹ�ÿ������ϵ�16��IOɨ�谴��, MCU��˯��, ����ɨ�谴��.

��������, ��һ֡Ϊ����, �����֡Ϊ�ظ�֡,��������, ���嶨�������вο�NEC�ı�������.

���ͷź�, ֹͣ����.

����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

******************************************/

#include "..\..\comm\AI8051U.h"
#include "stdio.h"
#include "intrins.h"

typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/****************************** �û������ ***********************************/

#define MAIN_Fosc       24000000UL
#define Baudrate        115200L
#define TM              (65536 -(MAIN_Fosc/Baudrate/4))
#define PrintUart       1        //1:printf ʹ�� UART1; 2:printf ʹ�� UART2

/*****************************************************************************/


/*************  ���س�������    **************/


/*************  IO���̱�������  **************/

u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;   //���м��̱���
u8  KeyHoldCnt; //�����¼�ʱ
u8  KeyCode;    //���û�ʹ�õļ���, 1~16��Ч

/*************  ���ⷢ����ر���    **************/

#define User_code   0xFF00      //��������û���

sbit    P_IR_TX   = P2^7;   //������ⷢ�Ͷ˿�
#define IR_TX_ON    0
#define IR_TX_OFF   1

static u16 tx_cnt;     //���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us
u8 TxTime;     //����ʱ��

/************* ���غ������� **************/

void delay_ms(u8 ms);
void IO_KeyScan(void);
void PWM_config(void);
void IR_TxPulse(u16 pulse);
void IR_TxSpace(u16 pulse);
void IR_TxByte(u8 dat);
void UartInit(void);

/********************* ������ *************************/
void main(void)
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

    UartInit();
    PWM_config();
    P_IR_TX = IR_TX_OFF;
    EA = 1;                     //�����ж�
    
    while(1)
    {
        delay_ms(30);       //30ms
        IO_KeyScan();

        if(KeyCode != 0)        //��⵽����
        {
            TxTime = 0;
                                //һ֡������С���� = 9 + 4.5 + 0.5625 + 24 * 1.125 + 8 * 2.25 = 59.0625 ms
                                //һ֡������󳤶� = 9 + 4.5 + 0.5625 + 8 * 1.125 + 24 * 2.25 = 77.0625 ms
            IR_TxPulse(342);    //��Ӧ9ms��ͬ��ͷ       9ms
            IR_TxSpace(171);    //��Ӧ4.5ms��ͬ��ͷ��� 4.5ms
            IR_TxPulse(21);     //��ʼ��������          0.5625ms

            IR_TxByte(User_code%256);   //���û�����ֽ�
            IR_TxByte(User_code/256);   //���û�����ֽ�
            IR_TxByte(KeyCode);         //������
            IR_TxByte(~KeyCode);        //�����ݷ���
            
            if(TxTime < 56)     //һ֡�����77ms����, �����Ļ�,����ʱ��     108ms
            {
                TxTime = 56 - TxTime;
                TxTime = TxTime + TxTime / 8;
                delay_ms(TxTime);
            }
            delay_ms(31);

            while(IO_KeyState != 0) //��δ�ͷ�
            {
                IR_TxPulse(342);    //��Ӧ9ms��   ͬ��ͷ        9ms
                IR_TxSpace(86);     //��Ӧ2.25ms��ͬ��ͷ���    2.25ms
                IR_TxPulse(21);     //��ʼ��������              0.5625ms
                delay_ms(96);
                IO_KeyScan();
            }

            printf("KeyCode = %u\r\n",KeyCode);
            KeyCode = 0;
        }
    }
}

//========================================================================
// ����: void delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
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

/*****************************************************
    ���м�ɨ�����
    ʹ��XY����4x4���ķ�����ֻ�ܵ������ٶȿ�

   Y     P04      P05      P06      P07
          |        |        |        |
X         |        |        |        |
P00 ---- K00 ---- K01 ---- K02 ---- K03 ----
          |        |        |        |
P01 ---- K04 ---- K05 ---- K06 ---- K07 ----
          |        |        |        |
P02 ---- K08 ---- K09 ---- K10 ---- K11 ----
          |        |        |        |
P03 ---- K12 ---- K13 ---- K14 ---- K15 ----
          |        |        |        |
******************************************************/


u8 code T_KeyTable[16] = {0,1,2,0,3,0,0,0,4,0,0,0,0,0,0,0};

void IO_KeyDelay(void)
{
    u8 i;
    i = 60;
    while(--i)  ;
}

void IO_KeyScan(void)    //50ms call
{
    u8  j;

    j = IO_KeyState1;   //������һ��״̬

    P0 = 0xf0;  //X�ͣ���Y
    IO_KeyDelay();
    IO_KeyState1 = P0 & 0xf0;

    P0 = 0x0f;  //Y�ͣ���X
    IO_KeyDelay();
    IO_KeyState1 |= (P0 & 0x0f);
    IO_KeyState1 ^= 0xff;   //ȡ��
    
    if(j == IO_KeyState1)   //�������ζ����
    {
        j = IO_KeyState;
        IO_KeyState = IO_KeyState1;
        if(IO_KeyState != 0)    //�м�����
        {
            F0 = 0;
            if(j == 0)  F0 = 1; //��һ�ΰ���
            else if(j == IO_KeyState)
            {
                if(++IO_KeyHoldCnt >= 20)   //1����ؼ�
                {
                    IO_KeyHoldCnt = 18;
                    F0 = 1;
                }
            }
            if(F0)
            {
                j = T_KeyTable[IO_KeyState >> 4];
                if((j != 0) && (T_KeyTable[IO_KeyState& 0x0f] != 0)) 
                    KeyCode = (j - 1) * 4 + T_KeyTable[IO_KeyState & 0x0f] + 16;    //������룬17~32
            }
        }
        else    IO_KeyHoldCnt = 0;
    }
    P0 = 0xff;
}


/************* �������庯�� **************/
void IR_TxPulse(u16 pulse)
{
    tx_cnt = pulse;
    PWMA_CCER2 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCMR4 = 0x60; //���� PWM4 ģʽ1 ���
    PWMA_CCER2 = 0x70; //ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
    PWMA_IER = 0x10;   //ʹ�ܲ���/�Ƚ� 4 �ж�
    while(tx_cnt);
}

/************* ���Ϳ��к��� **************/
void IR_TxSpace(u16 pulse)
{
    tx_cnt = pulse;
    PWMA_CCER2 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCMR4 = 0x40; //���� PWM4 ǿ��Ϊ��Ч��ƽ
    PWMA_CCER2 = 0x70; //ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
    PWMA_IER = 0x10;   //ʹ�ܲ���/�Ƚ� 4 �ж�
    while(tx_cnt);
}


/************* ����һ���ֽں��� **************/
void IR_TxByte(u8 dat)
{
    u8 i;
    for(i=0; i<8; i++)
    {
        if(dat & 1)     IR_TxSpace(63), TxTime += 2;    //����1��Ӧ 1.6875 + 0.5625 ms 
        else            IR_TxSpace(21), TxTime++;       //����0��Ӧ 0.5625 + 0.5625 ms
        IR_TxPulse(21);         //���嶼��0.5625ms
        dat >>= 1;              //��һ��λ
    }
}

//========================================================================
// ����: void   PWM_config(void)
// ����: PCA���ú���.
// ����: None
// ����: none.
// �汾: V1.0, 2012-11-22
//========================================================================
void PWM_config(void)
{
    PWMA_CCER2 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
    PWMA_CCMR4 = 0x60; //���� PWM4 ģʽ1 ���
    //PWMA_CCER2 = 0xB0; //ʹ�� CC4E ͨ��, �͵�ƽ��Ч

    PWMA_ARRH = 0x02; //��������ʱ��
    PWMA_ARRL = 0x77;
    PWMA_CCR4H = 0;
    PWMA_CCR4L = 210; //����ռ�ձ�ʱ��

    PWMA_PS = 0x80;  //�߼� PWM ͨ�� 4N �����ѡ��λ, 0x00:P1.7, 0x40:P0.7, 0x80:P2.7
//  PWMA_PS = 0x80;  //�߼� PWM ͨ�� 4P �����ѡ��λ, 0x00:P1.6, 0x40:P0.6, 0x80:P2.6
    PWMA_ENO = 0x80; //ʹ�� PWM4N ���
//  PWMA_ENO = 0x40; //ʹ�� PWM4P ���
    PWMA_BKR = 0x80; //ʹ�������
//    PWMA_IER = 0x10; //ʹ���ж�
    PWMA_CR1 |= 0x81;  //ʹ��ARRԤװ�أ���ʼ��ʱ
}

/******************* PWM�жϺ��� ********************/
void PWMA_ISR() interrupt PWMA_VECTOR
{
    if(PWMA_SR1 & 0X10)
    {
        PWMA_SR1 &=~0X10;
        //PWMA_SR1 = 0;
        if(--tx_cnt == 0)
        {
            PWMA_CCER2 = 0x00; //д CCMRx ǰ���������� CCxE �ر�ͨ��
            PWMA_CCMR4 = 0x40; //���� PWM4 ǿ��Ϊ��Ч��ƽ
            PWMA_CCER2 = 0x70; //ʹ�� CC4NE ͨ��, �͵�ƽ��Ч
            PWMA_IER = 0x00;   // �ر��ж�
        }
    }
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
	AUXR |= 0x14;   //��ʱ��2ʱ��1Tģʽ,��ʼ��ʱ
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
