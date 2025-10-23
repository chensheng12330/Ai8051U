/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "APP_DMA_UART.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_DMA.h"

/*************    ����˵��    **************

ͨ��PC��MCU��������, MCU���յ��������Զ�����DMA�ռ�.

��DMA�ռ�������ô�С�����ݺ�ͨ�����ڵ�DMA�Զ����͹��ܰѴ洢�ռ������ԭ������.

�ö�ʱ���������ʷ�����������ʹ��1Tģʽ(���ǵͲ�������12T)����ѡ��ɱ�������������ʱ��Ƶ�ʣ�����߾��ȡ�

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/


//========================================================================
//                               ���س�������    
//========================================================================


//========================================================================
//                               ���ر�������
//========================================================================

u8 xdata DmaBuffer[512];    //�շ����û��棬ͬʱʹ�ö�·����ʱÿ��������ֱ��建�棬�����໥����

//========================================================================
//                               ���غ�������
//========================================================================


//========================================================================
//                            �ⲿ�����ͱ�������
//========================================================================


//========================================================================
// ����: DMA_UART_init
// ����: �û���ʼ������.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void DMA_UART_init(void)
{
    u16 i;
    COMx_InitDefine COMx_InitStructure;         //�ṹ����
    DMA_UART_InitTypeDef DMA_UART_InitStructure;//�ṹ����
    
    //----------------------------------------------
    P3_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);    //P3.0,P3.1 ����Ϊ׼˫��� - UART1
    P4_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);    //P4.2,P4.3 ����Ϊ׼˫��� - UART2
    P0_MODE_IO_PU(GPIO_Pin_0 | GPIO_Pin_1);    //P0.0,P0.1 ����Ϊ׼˫��� - UART3
    P0_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3);    //P0.2,P0.3 ����Ϊ׼˫��� - UART4
    
    //----------------------------------------------
    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;     //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;         //ѡ�����ʷ�����, BRT_Timer1~BRT_Timer4 (ע��: ����2�̶�ʹ��BRT_Timer2)
    COMx_InitStructure.UART_BaudRate  = 115200ul;           //������,     һ��110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;             //��������,   ENABLE��DISABLE
    COMx_InitStructure.ParityMode  = PARITY_NONE;           //У��ģʽ,   PARITY_NONE,PARITY_EVEN,PARITY_ODD (ʹ��У��λ��Ҫ����9λģʽ)
    COMx_InitStructure.TimeOutEnable  = ENABLE;             //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;          //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;        //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;                //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART1, &COMx_InitStructure);         //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART1_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    UART_Configuration(UART2, &COMx_InitStructure);         //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART2_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    UART_Configuration(UART3, &COMx_InitStructure);         //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART3_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3
    UART_Configuration(UART4, &COMx_InitStructure);         //��ʼ������ UART1,UART2,UART3,UART4
    NVIC_UART4_Init(ENABLE,Priority_0);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    printf("AI8051U UART DMA Test Programme!\r\n");  //UART����һ���ַ���

    //----------------------------------------------
    DMA_UART_InitStructure.DMA_TX_Length = 511;             //DMA�������ֽ��� (0~65535) + 1
    DMA_UART_InitStructure.DMA_TX_Buffer = (u16)DmaBuffer;  //�������ݴ洢��ַ
    DMA_UART_InitStructure.DMA_RX_Length = 511;             //DMA�������ֽ��� (0~65535) + 1
    DMA_UART_InitStructure.DMA_RX_Buffer = (u16)DmaBuffer;  //�������ݴ洢��ַ
    DMA_UART_InitStructure.DMA_TX_Enable = ENABLE;          //DMAʹ�� ENABLE,DISABLE
    DMA_UART_InitStructure.DMA_RX_Enable = ENABLE;          //DMAʹ�� ENABLE,DISABLE
    DMA_UART_Inilize(UART1, &DMA_UART_InitStructure);       //��ʼ��
    DMA_UART_Inilize(UART2, &DMA_UART_InitStructure);       //��ʼ��
    DMA_UART_Inilize(UART3, &DMA_UART_InitStructure);       //��ʼ��
    DMA_UART_Inilize(UART4, &DMA_UART_InitStructure);       //��ʼ��

    NVIC_DMA_UART1_Tx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART1_Rx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART2_Tx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART2_Rx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART3_Tx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART3_Rx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART4_Tx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3
    NVIC_DMA_UART4_Rx_Init(ENABLE,Priority_0,Priority_0);   //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0~Priority_3; �������ȼ�(�͵���) Priority_0~Priority_3

    DMA_UR1R_CLRFIFO();        //��� DMA FIFO
    DMA_UR2R_CLRFIFO();        //��� DMA FIFO
    DMA_UR3R_CLRFIFO();        //��� DMA FIFO
    DMA_UR4R_CLRFIFO();        //��� DMA FIFO
    //----------------------------------------------
    DmaTx1Flag = 0;
    DmaRx1Flag = 0;
    DmaTx2Flag = 0;
    DmaRx2Flag = 0;
    DmaTx3Flag = 0;
    DmaRx3Flag = 0;
    DmaTx4Flag = 0;
    DmaRx4Flag = 0;
    for(i=0; i<256; i++)
    {
        DmaBuffer[i] = i;
    }
    DMA_UR1T_TRIG();    //����UART1���͹���
    DMA_UR1R_TRIG();    //����UART1���չ���
    DMA_UR2T_TRIG();    //����UART2���͹���
    DMA_UR2R_TRIG();    //����UART2���չ���
    DMA_UR3T_TRIG();    //����UART3���͹���
    DMA_UR3R_TRIG();    //����UART3���չ���
    DMA_UR4T_TRIG();    //����UART4���͹���
    DMA_UR4R_TRIG();    //����UART4���չ���
}

//========================================================================
// ����: Sample_DMA_UART
// ����: �û�Ӧ�ó���.
// ����: None.
// ����: None.
// �汾: V1.0, 2021-05-27
//========================================================================
void Sample_DMA_UART(void)
{
    u16 number;
    if(COM1.RX_TimeOut)       //����һ�����ݽ�����������ʱ�ж�
    {
        COM1.RX_TimeOut = 0;

        number = ((u16)DMA_UR1R_DONEH << 8) + DMA_UR1R_DONE; //��ȡ�ѽ����ֽڸ���
        CLR_TI(); //������ͱ�־��DMA�����겻���Զ������־λ��
        printf("number=%u\r\n",number);

        SET_DMA_UR1R_CR(DMA_DISABLE);
        DMA_UR1T_AMT = (u8)(number-1);       //���ô������ֽ���(��8λ)��n+1
        DMA_UR1T_AMTH = (u8)((number-1)>>8); //���ô������ֽ���(��8λ)��n+1

        SET_DMA_UR1T_CR(DMA_ENABLE | UR_T_TRIG);    //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����
        SET_DMA_UR1R_CR(DMA_ENABLE | UR_R_TRIG | CLR_FIFO); //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����, ��� FIFO
    }

    if(COM2.RX_TimeOut)       //����һ�����ݽ�����������ʱ�ж�
    {
        COM2.RX_TimeOut = 0;

        number = ((u16)DMA_UR2R_DONEH << 8) + DMA_UR2R_DONE; //��ȡ�ѽ����ֽڸ���
        CLR_TI2(); //������ͱ�־��DMA�����겻���Զ������־λ��

        SET_DMA_UR2R_CR(DMA_DISABLE);
        DMA_UR2T_AMT = (u8)(number-1);       //���ô������ֽ���(��8λ)��n+1
        DMA_UR2T_AMTH = (u8)((number-1)>>8); //���ô������ֽ���(��8λ)��n+1

        SET_DMA_UR2T_CR(DMA_ENABLE | UR_T_TRIG);    //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����
        SET_DMA_UR2R_CR(DMA_ENABLE | UR_R_TRIG | CLR_FIFO); //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����, ��� FIFO
    }

    if(COM3.RX_TimeOut)       //����һ�����ݽ�����������ʱ�ж�
    {
        COM3.RX_TimeOut = 0;

        number = ((u16)DMA_UR3R_DONEH << 8) + DMA_UR3R_DONE; //��ȡ�ѽ����ֽڸ���
        CLR_TI3(); //������ͱ�־��DMA�����겻���Զ������־λ��

        SET_DMA_UR3R_CR(DMA_DISABLE);
        DMA_UR3T_AMT = (u8)(number-1);       //���ô������ֽ���(��8λ)��n+1
        DMA_UR3T_AMTH = (u8)((number-1)>>8); //���ô������ֽ���(��8λ)��n+1

        SET_DMA_UR3T_CR(DMA_ENABLE | UR_T_TRIG);    //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����
        SET_DMA_UR3R_CR(DMA_ENABLE | UR_R_TRIG | CLR_FIFO); //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����, ��� FIFO
    }

    if(COM4.RX_TimeOut)       //����һ�����ݽ�����������ʱ�ж�
    {
        COM4.RX_TimeOut = 0;

        number = ((u16)DMA_UR4R_DONEH << 8) + DMA_UR4R_DONE; //��ȡ�ѽ����ֽڸ���
        CLR_TI4(); //������ͱ�־��DMA�����겻���Զ������־λ��

        SET_DMA_UR4R_CR(DMA_DISABLE);
        DMA_UR4T_AMT = (u8)(number-1);       //���ô������ֽ���(��8λ)��n+1
        DMA_UR4T_AMTH = (u8)((number-1)>>8); //���ô������ֽ���(��8λ)��n+1

        SET_DMA_UR4T_CR(DMA_ENABLE | UR_T_TRIG);    //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����
        SET_DMA_UR4R_CR(DMA_ENABLE | UR_R_TRIG | CLR_FIFO); //ʹ�� UART_DMA, ��ʼ UART_DMA �Զ�����, ��� FIFO
    }

//    if((DmaTx1Flag) && (DmaRx1Flag))    //������Ҫ��ʱ����ʹ����ʱ���߶�ʱ�������Դ����������´�������ʱ�����ⲿ�����շ��Ĳ������ݶ�ʧ
//    {
//        DmaTx1Flag = 0;
//        DmaRx1Flag = 0;
//        DMA_UR1T_TRIG();    //���´���UART1���͹��ܣ�����DMA�ж��Ｐʱ����
//        DMA_UR1R_TRIG();    //���´���UART1���չ��ܣ�����DMA�ж��Ｐʱ����
//    }

//    if((DmaTx2Flag) && (DmaRx2Flag))    //������Ҫ��ʱ����ʹ����ʱ���߶�ʱ�������Դ����������´�������ʱ�����ⲿ�����շ��Ĳ������ݶ�ʧ
//    {
//        DmaTx2Flag = 0;
//        DmaRx2Flag = 0;
//        DMA_UR2T_TRIG();    //���´���UART2���͹��ܣ�����DMA�ж��Ｐʱ����
//        DMA_UR2R_TRIG();    //���´���UART2���չ��ܣ�����DMA�ж��Ｐʱ����
//    }

//    if((DmaTx3Flag) && (DmaRx3Flag))    //������Ҫ��ʱ����ʹ����ʱ���߶�ʱ�������Դ����������´�������ʱ�����ⲿ�����շ��Ĳ������ݶ�ʧ
//    {
//        DmaTx3Flag = 0;
//        DmaRx3Flag = 0;
//        DMA_UR3T_TRIG();    //���´���UART3���͹��ܣ�����DMA�ж��Ｐʱ����
//        DMA_UR3R_TRIG();    //���´���UART3���չ��ܣ�����DMA�ж��Ｐʱ����
//    }

//    if((DmaTx4Flag) && (DmaRx4Flag))    //������Ҫ��ʱ����ʹ����ʱ���߶�ʱ�������Դ����������´�������ʱ�����ⲿ�����շ��Ĳ������ݶ�ʧ
//    {
//        DmaTx4Flag = 0;
//        DmaRx4Flag = 0;
//        DMA_UR4T_TRIG();    //���´���UART4���͹��ܣ�����DMA�ж��Ｐʱ����
//        DMA_UR4R_TRIG();    //���´���UART4���չ��ܣ�����DMA�ж��Ｐʱ����
//    }
}
