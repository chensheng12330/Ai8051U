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
#include "AI8051U_Soft_I2C.h"

/*************    ����˵��    **************

�ڲ�����I2C�������߿��������ӻ�ģʽ��SCL->P3.2, SDA->P3.3;
IO��ģ��I2C������ģʽ��SCL->P0.0, SDA->P0.1;
ͨ���ⲿ�������� P0.0->P3.2, P0.1->P3.3��ʵ��I2C�Է����չ��ܡ�

ʹ��Timer0��16λ�Զ���װ������1ms����,�������������������,�û��޸�MCU��ʱ��Ƶ��ʱ,�Զ���ʱ��1ms.
������ÿ���Ӽ�1, ������ΧΪ0~9999.

�ϵ������ÿ���ӷ���һ�μ������ݣ�ͨ�����ڴ�ӡ�շ����ݡ�

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/

u16 msecond;
u16 second;   //�����õ����������
u8  tmp[4];     //ͨ������

/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/

extern bit T0_1ms;

/******************** IO������ ********************/
void GPIO_config(void)
{
    P0_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);        //P0.0,P0.1 ����Ϊ׼˫���
    P3_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);        //P3.2,P3.3 ����Ϊ׼˫���
}

/************************ ��ʱ������ ****************************/
void Timer_config(void)
{
    TIM_InitTypeDef TIM_InitStructure;          //�ṹ����
    TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;  //ָ������ģʽ,  TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
    TIM_InitStructure.TIM_ClkMode   = TIM_CLOCK_1T;         //ָ��ʱ��ģʽ,  TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
    TIM_InitStructure.TIM_ClkOut    = DISABLE;              //�Ƿ������ʱ��ʱ��, ENABLE��DISABLE
    TIM_InitStructure.TIM_Value     = (u16)(65536UL - (MAIN_Fosc / 1000UL));    //�ж�Ƶ��, 1000��/��
    TIM_InitStructure.TIM_PS        = 0;                    //8λԤ��Ƶ��(n+1), 0~255
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

    I2C_InitStructure.I2C_Mode      = I2C_Mode_Slave;   //����ѡ��   I2C_Mode_Master, I2C_Mode_Slave
    I2C_InitStructure.I2C_Enable    = ENABLE;           //I2C����ʹ��,   ENABLE, DISABLE
    I2C_InitStructure.I2C_SL_MA     = ENABLE;           //ʹ�ܴӻ���ַ�ȽϹ���,   ENABLE, DISABLE
    I2C_InitStructure.I2C_SL_ADR    = 0x2d;             //�ӻ��豸��ַ,  0~127  (0x2d<<1 = 0x5a)
    I2C_Init(&I2C_InitStructure);
    NVIC_I2C_Init(I2C_Mode_Slave,I2C_ESTAI|I2C_ERXI|I2C_ETXI|I2C_ESTOI,Priority_0);    //����ģʽ, I2C_Mode_Master, I2C_Mode_Slave; �ж�ʹ��, I2C_ESTAI/I2C_ERXI/I2C_ETXI/I2C_ESTOI/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    I2C_SW(I2C_P32_P33);    //I2C_P24_P23,I2C_P15_P14,I2C_P32_P33
}

/******************** task A **************************/
void main(void)
{
    u8  i;
    
    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�

    GPIO_config();
    Timer_config();
    UART_config();
    I2C_config();
    EA = 1;
    
    printf("AI8051U I2C�����շ����Գ���\r\n");

    while (1)
    {
        if(DisplayFlag)
        {
            DisplayFlag = 0;

            printf("I2C Read: ");
            for(i=0; i<4; i++)  printf("%d",I2C_Buffer[i]);
            printf("\r\n");
        }
        
        if(T0_1ms)
        {
            T0_1ms = 0;
            
            if(++msecond >= 1000)   //1�뵽
            {
                msecond = 0;        //��1000ms����
                second++;         //�����+1
                if(second >= 10000)    second = 0;   //�������ΧΪ0~9999
                
                printf("I2C Send: %04u\r\n",second);

                tmp[0] = second / 1000;
                tmp[1] = (second % 1000) / 100;
                tmp[2] = (second % 100) / 10;
                tmp[3] = second % 10;

                SI2C_WriteNbyte(SLAW, 0, tmp, 4);
            }
        }
    }
}
