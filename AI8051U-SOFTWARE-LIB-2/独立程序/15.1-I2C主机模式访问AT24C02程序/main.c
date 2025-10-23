/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_I2C.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_Timer.h"
#include "AI8051U_Switch.h"

/*************    ����˵��    **************

ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

���¶�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

��������UART1(P3.0,P3.1): 115200,N,8,1.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/

#define SLAW        0xA0
#define SLAR        0xA1

/*************    ���ر�������    **************/


/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/

extern bit T0_Flag;

/******************** IO������ ********************/
void GPIO_config(void)
{
	P3_MODE_IO_PU(GPIO_Pin_LOW);	    //P3.0~P3.3 ����Ϊ׼˫���
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;          //�ṹ����
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_12T;        //ָ��ʱ��ģʽ,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������ʱ��ʱ��, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / (12*100UL)));    //�ж�Ƶ��, 100��/��
    TIM_InitStructure.TIM_PS        = 100;                  //8λԤ��Ƶ��(n+1), 0~255
    TIM_InitStructure.TIM_Run       = ENABLE;               //�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
    Timer_Inilize(Timer0,&TIM_InitStructure);               //��ʼ��Timer0, Timer0,Timer1,Timer2,Timer3,Timer4
    NVIC_Timer0_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/****************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure; //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;     //ѡ�����ʷ�����, BRT_Timer1, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������, һ�� 110 ~ 115200
//    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE �� DISABLE
//    COMx_InitStructure.ParityMode  = PARITY_NONE;       //У��ģʽ,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (ʹ��У��λ��Ҫ����9λģʽ)
//    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
//    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
//    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART1_SW(UART1_SW_P30_P31);         //UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/****************  I2C��ʼ������ *****************/
void I2C_config(void)
{
    I2C_InitTypeDef I2C_InitStructure;

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//����ѡ��   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C����ʹ��,   ENABLE, DISABLE
    I2C_InitStructure.I2C_MS_WDTA   = DISABLE;          //����ʹ���Զ�����,  ENABLE, DISABLE
    I2C_InitStructure.I2C_Speed     = 16;               //�����ٶ�=Fosc/2/(Speed*2+4),      0~63
	I2C_Init(&I2C_InitStructure);
    NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);  //����ģʽ, I2C_Mode_Master, I2C_Mode_Slave; �ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    I2C_SW(I2C_P32_P33);    //I2C_P24_P23,I2C_P15_P14,I2C_P32_P33
}

/******************** task A **************************/
void main(void)
{
    u8  i;
    u8  tmp[8];
    
    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

    GPIO_config();
    Timer_config();
    UART_config();
    I2C_config();
    EA = 1;
    
    printf("AI8051U I2C����ģʽ����AT24C02����\r\n");

    while (1)
    {
        if(T0_Flag)     //��ʱ��ÿ1��������һ���жϱ�־
        {
            T0_Flag = 0;
            
            I2C_ReadNbyte(SLAW, 0, tmp, 8);
            printf("Read = ");      //��ӡ��ȡ��ֵ
            for(i=0; i<8; i++)
            {
                printf("%02x ",tmp[i]);
                tmp[i]++;           //��ȡ��ֵ+1
            }
            printf("\r\n");

            I2C_WriteNbyte(SLAW, 0, tmp, 8);  //д���µ���ֵ
        }
    }
}
