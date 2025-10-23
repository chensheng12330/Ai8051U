/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

�����շ������������г�����������NEC���롣

Ӧ�ò��ѯ B_IR_Press ��־Ϊ1�����ѽ��յ�һ���������IR_code��, �����������û�������� B_IR_Press ��־.

�û������ں궨����ָ���û���.

���ڴ�ӡ���ⷢ��İ����룬�Լ�������յ��û����밴����.

����1(P3.0,P3.1)���ã�115200,N,8,1��ʹ���ı�ģʽ��ӡ��

�û��ײ���򰴹̶���ʱ����(60~125us)���� "IR_RX_NEC()"����.

����IO���м�����֧��ADC���̣�����ʾ���͡����յ��ļ�ֵ��

������ս�(P35)������ܿ��ƽ�(RCK)���ã����Բ������������ʾ��

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

#define SysTick 14225   // ��/��, ϵͳ�δ�Ƶ��, ��4000~16000֮��
#define Timer0_Reload   (65536UL - ((MAIN_Fosc + SysTick/2) / SysTick))     //Timer 0 �ж�Ƶ��
#define User_code   0xFF00      //��������û���

/*************  ���س�������    **************/


/*************  IO�ڶ���    **************/

sbit P_IR_TX = P2^7;    //������ⷢ�ͽ�
sbit P_IR_RX = P3^5;    //��������������IO��

/*************  ���ر�������    **************/

bit B_1ms;          //1ms��־
u8  cnt_1ms;        //1ms������ʱ

u8  IO_KeyState, IO_KeyState1, IO_KeyHoldCnt;   //���м��̱���
u8  KeyHoldCnt; //�����¼�ʱ
u8  KeyCode;    //���û�ʹ�õļ���, 1~16��Ч
u8  cnt_27ms;

/*************  ���ⷢ�ͳ����������    **************/

u16 tx_cnt;    //���ͻ���е��������(����38KHZ������������Ӧʱ��), ����Ƶ��Ϊ38KHZ, ����26.3us

/*************  ������ճ����������    **************/

u8  IR_SampleCnt;       //��������
u8  IR_BitCnt;          //����λ��
u8  IR_UserH;           //�û���(��ַ)���ֽ�
u8  IR_UserL;           //�û���(��ַ)���ֽ�
u8  IR_data;            //����ԭ��
u8  IR_DataShit;        //������λ

bit P_IR_RX_temp;       //Last sample
bit B_IR_Sync;          //���յ�ͬ����־
bit B_IR_Press;         //������������
u8  IR_code;            //�������
u16 UserCode;           //�û���

/*************  ���غ�������    **************/

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

    AUXR |= 0x80;   //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
    EA = 1;     //�����ж�
    
    UartInit();
    PWM_config();

    while(1)
    {
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            if(++cnt_27ms >= 27)
            {
                cnt_27ms = 0;
                IO_KeyScan();

                if(KeyCode > 0)     //��⵽�յ��������
                {
                    printf("KeyCode = %u\r\n",KeyCode);

                    IR_TxPulse(342);    //��Ӧ9ms��ͬ��ͷ       9ms
                    IR_TxSpace(171);    //��Ӧ4.5ms��ͬ��ͷ��� 4.5ms
                    IR_TxPulse(21);     //��ʼ��������          0.5625ms

                    IR_TxByte(User_code%256);   //���û�����ֽ�
                    IR_TxByte(User_code/256);   //���û�����ֽ�
                    IR_TxByte(KeyCode);         //������
                    IR_TxByte(~KeyCode);        //�����ݷ���
									
                    KeyCode = 0;
                }
            }

            if(B_IR_Press)      //��⵽�յ��������
            {
                B_IR_Press = 0;
                
                printf("Read: UserCode=0x%04x,IRCode=%u\r\n",UserCode,IR_code);
            }
        }
    }
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
                if(++IO_KeyHoldCnt >= 37)   //1����ؼ�
                {
                    IO_KeyHoldCnt = 33;     //108ms repeat
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

//*******************************************************************
//*********************** IR Remote Module **************************
//*********************** By ���� (Coody) 2002-8-24 *****************
//*********************** IR Remote Module **************************
//this programme is used for Receive IR Remote (NEC Code).

//data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

//send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

//data rate: 108ms/Frame


//Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
//Bit0: high=0.5626ms, low=0.5626ms
//Bit1: high=1.6879ms, low=0.5626ms
//frame rate = 108ms ( pause 23 ms or 96 ms)

/******************** �������ʱ��궨��, �û���Ҫ�����޸�  *******************/

#define IR_SAMPLE_TIME      (1000000UL/SysTick)         //��ѯʱ����, us, �������Ҫ����60us~250us֮��
#if ((IR_SAMPLE_TIME <= 250) && (IR_SAMPLE_TIME >= 60))
    #define D_IR_sample         IR_SAMPLE_TIME      //�������ʱ�䣬��60us~250us֮��
#endif

#define D_IR_SYNC_MAX       (15000/D_IR_sample) //SYNC max time
#define D_IR_SYNC_MIN       (9700 /D_IR_sample) //SYNC min time
#define D_IR_SYNC_DIVIDE    (12375/D_IR_sample) //decide data 0 or 1
#define D_IR_DATA_MAX       (3000 /D_IR_sample) //data max time
#define D_IR_DATA_MIN       (600  /D_IR_sample) //data min time
#define D_IR_DATA_DIVIDE    (1687 /D_IR_sample) //decide data 0 or 1
#define D_IR_BIT_NUMBER     32                  //bit number

//*******************************************************************************************
//**************************** IR RECEIVE MODULE ********************************************

void IR_RX_NEC(void)
{
    u8  SampleTime;

    IR_SampleCnt++;                         //Sample + 1

    F0 = P_IR_RX_temp;                      //Save Last sample status
    P_IR_RX_temp = P_IR_RX;                 //Read current status
    if(F0 && !P_IR_RX_temp)                 //Pre-sample is high��and current sample is low, so is fall edge
    {
        SampleTime = IR_SampleCnt;          //get the sample time
        IR_SampleCnt = 0;                   //Clear the sample counter

             if(SampleTime > D_IR_SYNC_MAX)     B_IR_Sync = 0;  //large the Maxim SYNC time, then error
        else if(SampleTime >= D_IR_SYNC_MIN)                    //SYNC
        {
            if(SampleTime >= D_IR_SYNC_DIVIDE)
            {
                B_IR_Sync = 1;                  //has received SYNC
                IR_BitCnt = D_IR_BIT_NUMBER;    //Load bit number
            }
        }
        else if(B_IR_Sync)                      //has received SYNC
        {
            if(SampleTime > D_IR_DATA_MAX)      B_IR_Sync=0;    //data samlpe time too large
            else
            {
                IR_DataShit >>= 1;                  //data shift right 1 bit
                if(SampleTime >= D_IR_DATA_DIVIDE)  IR_DataShit |= 0x80;    //devide data 0 or 1
                if(--IR_BitCnt == 0)                //bit number is over?
                {
                    B_IR_Sync = 0;                  //Clear SYNC
                    if(~IR_DataShit == IR_data)     //�ж�����������
                    {
                        UserCode = ((u16)IR_UserH << 8) + IR_UserL;
                        IR_code      = IR_data;
                        B_IR_Press   = 1;           //������Ч
                    }
                }
                else if((IR_BitCnt & 7)== 0)        //one byte receive
                {
                    IR_UserL = IR_UserH;            //Save the User code high byte
                    IR_UserH = IR_data;             //Save the User code low byte
                    IR_data  = IR_DataShit;         //Save the IR data byte
                }
            }
        }
    }
}

/********************** Timer0 1ms�жϺ��� ************************/
void timer0 (void) interrupt 1
{
    IR_RX_NEC();
    if(--cnt_1ms == 0)
    {
        cnt_1ms = SysTick / 1000;
        B_1ms = 1;      //1ms��־
    }
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
        if(dat & 1)     IR_TxSpace(63);    //����1��Ӧ 1.6875 + 0.5625 ms 
        else            IR_TxSpace(21);    //����0��Ӧ 0.5625 + 0.5625 ms
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
//  PWMA_IER = 0x10; //ʹ���ж�
    PWMA_CR1 |= 0x81;//ʹ��ARRԤװ�أ���ʼ��ʱ
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
