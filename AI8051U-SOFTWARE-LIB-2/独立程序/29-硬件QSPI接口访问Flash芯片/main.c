/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "w25qxx.h"
#include "AI8051U_QSPI.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_UART.h"
#include "AI8051U_Switch.h"

/*************   ����˵��   ***************

ͨ��Ӳ��QSPI�ӿ�1�ߡ�2�ߡ�4��ģʽ��֧��QSPIЭ���Flash���ж�д���ԡ�

����(P3.0,P3.1)��ӡ���ʽ����Ĭ������: 115200,8,N,1.

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/


/*************    ���س�������    **************/


/******************* ���Ŷ��� ************************/

sbit    QSPI_CS     =   P4^0;
sbit    QSPI_IO0    =   P4^1;
sbit    QSPI_IO1    =   P4^2;
sbit    QSPI_SCK    =   P4^3;
sbit    QSPI_IO2    =   P5^2;
sbit    QSPI_IO3    =   P5^3;

/*************    ���ر�������    **************/

int i;
BYTE xdata buf[1024];

/*************    ���غ�������    **************/

#define SIZE            30

#define INIT_BUF()      for (i=0; i<SIZE; i++) buf[i] = 0;
#define SET_BUF()       for (i=0; i<SIZE; i++) buf[i] = (BYTE)(i + 0x00);
#define PRINT_BUF()     for (i=0; i<SIZE; i++)                              \
                        {                                                   \
                            printf("%02bx ", buf[i]);                       \
                            if ((i % 32) == 31)                             \
                                printf("\n                            ");   \
                        }                                                   \
                        printf("\n");

/*************  �ⲿ�����ͱ������� *****************/


/******************** IO������ ********************/
void GPIO_config(void)
{
    P4_MODE_OUT_PP(GPIO_Pin_0 | GPIO_Pin_3);    //P4.0, P4.3 ����Ϊ�������
    P4_SPEED_HIGH(GPIO_Pin_LOW);                //P4.0~P4.3 ���ÿ��ٵ�ƽת��
    P5_SPEED_HIGH(GPIO_Pin_2 | GPIO_Pin_3);     //P5.2, P5.3 ���ÿ��ٵ�ƽת��
    P4_PULL_UP_ENABLE(GPIO_Pin_LOW);            //P4.0~P4.3 ��������ʹ��
    P5_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P5.2, P5.3 ��������ʹ��
    P4_BP_ENABLE(GPIO_Pin_1 | GPIO_Pin_2);      //P4.1, P4.2 Ӳ���Զ����ö˿�ģʽ
    P5_BP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3);      //P5.2, P5.3 Ӳ���Զ����ö˿�ģʽ
    QSPI_SW(QSPI_P40_P41_P42_P52_P53_P43);      //QSPI_P14_P15_P16_P13_P12_P17,QSPI_P40_P41_P42_P52_P53_P43,QSPI_P47_P25_P26_P46_P45_P27

    P3_PULL_UP_ENABLE(GPIO_Pin_2 | GPIO_Pin_3); //P3.2 ��������ʹ��
    
    QSPI_CS = 1;
    QSPI_SCK = 1;
    QSPI_IO0 = 1;
    QSPI_IO1 = 1;
    QSPI_IO2 = 1;
    QSPI_IO3 = 1;
}

/******************** SPI ���� ********************/
void QSPI_config(void)
{
    QSPI_InitTypeDef QSPI_InitStructure;    //�ṹ����

    NVIC_QSPI_Init(DISABLE,Priority_0);     //�ж�ʹ������, QSPI_SMIE/QSPI_FTIE/QSPI_TCIE/QSPI_TEIE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3
    QSPI_InitStructure.FIFOLevel  = 31;     //����FIFO��ֵ, 0~31
    QSPI_InitStructure.ClockDiv   = 3;      //����QSPIʱ�� = ϵͳʱ��/(n+1), 0~255
    QSPI_InitStructure.CSHold     = 1;      //����CS����ʱ��Ϊ(n+1)��QSPIʱ��, 0~7
    QSPI_InitStructure.CKMode     = 1;      //���ÿ���ʱCLK��ƽ, 0/1
    QSPI_InitStructure.FlashSize  = 25;     //����Flash��СΪ2^(25+1)=64M�ֽ�, 0~31
    QSPI_InitStructure.SIOO       = DISABLE;//����һ��ָ��ģʽ, ENABLE(����һ��������ָ��)/DISABLE(ÿ�����������ָ��)
    QSPI_InitStructure.QSPI_EN    = ENABLE; //QSPIʹ��, ENABLE/DISABLE
    QSPI_Inilize(&QSPI_InitStructure);      //��ʼ��
}

/******************** UART ���� ********************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;     //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;     //ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE �� DISABLE
    UART_Configuration(UART1, &COMx_InitStructure);     //��ʼ������1 UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
}

/**********************************************/
void main(void)
{
    WTST = 0;        //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();        //��չSFR(XFR)����ʹ�� 
    CKCON = 0;      //��߷���XRAM�ٶ�

    GPIO_config();
    UART_config();
    QSPI_config();
    EA = 1;

    printf("QSPI Test !\n");
    printf("W25Q_ReadJEDECID_9F         %08lx\n", W25Q_ReadJEDECID_9F());
    printf("SR1: %02bx\n", W25Q_ReadSR1_05());
    printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    printf("SR3: %02bx\n", W25Q_ReadSR3_15());
    
    if ((W25Q_ReadSR2_35() & 0x02) == 0)
    {
        W25Q_WriteEnableVSR_50();
        if ((W25Q_ReadJEDECID_9F() & 0xffff) == 0x4014) //W25Q80дSR2������ͬ
        {
            W25Q_WriteSR12_01(0x0002);
        }
        else
        {
            W25Q_WriteSR2_31(0x02);
        }
        printf("SR2: %02bx\n", W25Q_ReadSR2_35());
    }
    
    printf("W25Q_Erase4K_20\n");
    W25Q_Erase4K_20(0, TRUE);
    
    printf("W25Q_ReadData_03            ");
    INIT_BUF();
    W25Q_ReadData_03(0, buf, SIZE);
    PRINT_BUF();

    printf("W25Q_PageProgram_02\n");
    SET_BUF();
    W25Q_PageProgram_02(0, buf, SIZE);
    
    printf("W25Q_ReadData_03            ");
    INIT_BUF();
    W25Q_ReadData_03(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_0B            ");
    INIT_BUF();
    W25Q_FastRead_0B(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_3B            ");
    INIT_BUF();
    W25Q_FastRead_3B(0, buf, SIZE);
    PRINT_BUF();
    
    printf("W25Q_FastRead_6B            ");
    INIT_BUF();
    W25Q_FastRead_6B(0, buf, SIZE);
    PRINT_BUF();

    while (1)
    {
    }
}
