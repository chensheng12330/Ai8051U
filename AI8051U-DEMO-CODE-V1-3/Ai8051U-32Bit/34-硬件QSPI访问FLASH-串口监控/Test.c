/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

ͨ��Ӳ��QSPI�ӿ�1�ߡ�2�ߡ�4��ģʽ��֧��QSPIЭ���Flash���ж�д���ԡ�

����(P3.0,P3.1)��ӡ���ʽ����Ĭ������: 115200,8,N,1.

����ʱ, ѡ��ʱ�� 40MHz (�û��������޸�Ƶ��).

******************************************/

#include <stdio.h>
#include <intrins.h>
#include "qspi.h"
#include "w25qxx.h"

/*****************************************************************************/
//#define FOSC                11059200UL
//#define FOSC                24000000UL
#define FOSC                40000000UL

#define BAUD                (65536 - FOSC/4/115200)
#define SIZE                30

/*****************************************************************************/
#define INIT_BUF()      for (i=0; i<SIZE; i++) buf[i] = 0;
#define SET_BUF()       for (i=0; i<SIZE; i++) buf[i] = (BYTE)(i + 0x00);
#define PRINT_BUF()     for (i=0; i<SIZE; i++)                              \
                        {                                                   \
                            printf("%02bx ", buf[i]);                       \
                            if ((i % 32) == 31)                             \
                                printf("\n                            ");   \
                        }                                                   \
                        printf("\n");

/*****************************************************************************/
BYTE xdata buf[1024];

/*****************************************************************************/
void delay(int n)
{
    int i;

    while (n--)
    for (i = 0; i < 1000; i++)
    {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    }
}

/*****************************************************************************/
void main()
{
    int i;
    int j;
    BYTE d1, d2;

    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    i = j = d1 = d2 = 0;

    P0M0 = 0x00; P0M1 = 0x00;
    P1M0 = 0x00; P1M1 = 0x00;
    P2M0 = 0x00; P2M1 = 0x00;
    P3M0 = 0x00; P3M1 = 0x00;
    P4M0 = 0x00; P4M1 = 0x00;
    P5M0 = 0x00; P5M1 = 0x00;

    P4M0 |= 0x09;               //����CS/P4.0,SCK/P4.3Ϊǿ����ģʽ
    P4M1 &= ~0x09;
    P4SR &= ~0x0f;              //�������е�QSPI��Ϊ����ģʽ
    P5SR &= ~0x0c;
    P4PU |= 0x0f;               //ʹ�����е�QSPI�ڵ��ڲ�10K��������
    P5PU |= 0x0c;
    P4BP &= ~0x06;              //ʹ��QSPI��IO0~IO3����Ӳ���Զ����ö˿�ģʽ
    P5BP &= ~0x0c;

    SCON = 0x52;
    T2L = BAUD;
    T2H = BAUD >> 8;
    AUXR = 0x15;

    delay(1000);

    printf("AI8051U Test !\n");

    QSPI_Init();
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
    W25Q_Erase4K_20(0);
    
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

    while (1);
}
