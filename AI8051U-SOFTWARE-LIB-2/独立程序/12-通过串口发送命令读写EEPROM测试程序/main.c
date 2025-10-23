/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_UART.h"
#include "AI8051U_NVIC.h"
#include "AI8051U_EEPROM.h"
#include "AI8051U_Switch.h"

/*************    ��������˵��    **************

ͨ������2(P4.2 P4.3)��оƬ�ڲ��Դ���EEPROM(FLASH)�ռ���ж�д���ԡ�

��FLASH������������д�롢�����Ĳ���������ָ����ַ��

Ĭ�ϲ�����:  115200,N,8,1. 

������������: (������ĸ�����ִ�Сд)
    E 0x000040             --> ��0x000040��ַ�������ݽ��в���.
    W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.
    R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����. 

ע�⣺����ʱ�����ؽ���"Ӳ��ѡ��"�������û�EEPROM��С��

��ȷ�����������еĵ�ַ��EEPROM���õĴ�С��Χ֮�ڡ�

����ʱ, ѡ��ʱ�� 40MHz (�����������ļ�"config.h"���޸�).

******************************************/

#define Max_Length  100     //��дEEPROM���峤��

/*************    ���س�������    **************/


/*************    ���ر�������    **************/

u8 tmp[Max_Length];        //EEPROM��������

/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/


/******************* IO���ú��� *******************/
void GPIO_config(void)
{
    P4_MODE_IO_PU(GPIO_Pin_2 | GPIO_Pin_3); //P4.2,P4.3 ����Ϊ׼˫���
}

/***************  ���ڳ�ʼ������ *****************/
void UART_config(void)
{
    COMx_InitDefine COMx_InitStructure;     //�ṹ����

    COMx_InitStructure.UART_Mode      = UART_8bit_BRTx; //ģʽ,   UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
//    COMx_InitStructure.UART_BRT_Use   = BRT_Timer2;   //ѡ�����ʷ�����, BRT_Timer2 (ע��: ����2�̶�ʹ��BRT_Timer2, ���Բ���ѡ��)
    COMx_InitStructure.UART_BaudRate  = 115200ul;       //������,     110 ~ 115200
    COMx_InitStructure.UART_RxEnable  = ENABLE;         //��������,   ENABLE �� DISABLE
    COMx_InitStructure.TimeOutEnable  = ENABLE;         //���ճ�ʱʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutINTEnable  = ENABLE;      //��ʱ�ж�ʹ��, ENABLE,DISABLE
    COMx_InitStructure.TimeOutScale  = TO_SCALE_BRT;    //��ʱʱ��Դѡ��, TO_SCALE_BRT,TO_SCALE_SYSCLK
    COMx_InitStructure.TimeOutTimer  = 32ul;            //��ʱʱ��, 1 ~ 0xffffff
    UART_Configuration(UART2, &COMx_InitStructure);     //��ʼ������2 UART1,UART2,UART3,UART4
    NVIC_UART2_Init(ENABLE,Priority_1);        //�ж�ʹ��, ENABLE/DISABLE; ���ȼ�(�͵���) Priority_0,Priority_1,Priority_2,Priority_3

    UART2_SW(UART2_SW_P42_P43);         //UART2_SW_P12_P13,UART2_SW_P42_P43
}

/**********************************************/

u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    if((dat >= 'a') && (dat <= 'f'))        return (dat-'a'+10);
    return 0xff;
}

//========================================================================
// ����: u32 GetAddress(void)
// ����: ����������뷽ʽ�ĵ�ַ.
// ����: ��.
// ����: 32λEEPROM��ַ.
// �汾: V1.0, 2013-6-6
//========================================================================
u32 GetAddress(void)
{
    u32 address;
    u8  i,j;
    
    address = 0;
    if((RX2_Buffer[2] == '0') && (RX2_Buffer[3] == 'X'))
    {
        for(i=4; i<10; i++)
        {
            j = CheckData(RX2_Buffer[i]);
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
    for(i=11; i<COM2.RX_Cnt; i++)
    {
        if(CheckData(RX2_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX2_Buffer[i]);
    }
    return (length);
}

/********************* ������ *************************/
void main(void)
{
    u8  i,j;
    u32 addr;
    u8  status;

    WTST = 0;   //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();   //��չSFR(XFR)����ʹ�� 
    CKCON = 0;  //��߷���XRAM�ٶ�
    SET_TPS();  //����ϵͳ�ȴ�ʱ�䵥Ԫ�����ڿ���EEPROM������SPI/I2C��ʱʱ���Լ����߻��ѵȴ�ʱ��

    GPIO_config();
    UART_config();
    EA = 1;

    PrintString2("AI8051Uϵ�е�Ƭ��EEPROM���Գ��򣬴���������������:\r\n");    //UART2����һ���ַ���
    PrintString2("E 0x000040             --> ��0x000040��ַ�������в���\xfd.\r\n");     //UART2����һ���ַ���
    PrintString2("W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.\r\n");  //UART2����һ���ַ���
    PrintString2("R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����.\r\n");    //UART2����һ���ַ���

    while(1)
    {
        if(COM2.RX_TimeOut) //��ⳬʱ��־
        {
            COM2.RX_TimeOut = 0;

//            printf("�յ��������£� ");
//            for(i=0; i<COM2.RX_Cnt; i++)    printf("%c", RX2_Buffer[i]);    //���յ�������ԭ������,���ڲ���
//            printf("\r\n");

            status = 0xff;  //״̬��һ����0ֵ
            if((COM2.RX_Cnt >= 10) && (RX2_Buffer[1] == ' ')) //�������Ϊ10���ֽ�
            {
                for(i=0; i<10; i++)
                {
                    if((RX2_Buffer[i] >= 'a') && (RX2_Buffer[i] <= 'z'))    RX2_Buffer[i] = RX2_Buffer[i] - 'a' + 'A';  //Сдת��д
                }
                addr = GetAddress();
                if(addr < 0x00ffffff)    //���Ƶ�ַ��Χ
                {
                    if(RX2_Buffer[0] == 'E')    //�ж�ָ�������Ƿ�Ϊ��E������ָ��
                    {
                        EEPROM_SectorErase(addr);           //��������
                        PrintString2("�Ѳ���\xfd��������!\r\n");
                        status = 0; //������ȷ
                    }

                    else if((RX2_Buffer[0] == 'W') && (RX2_Buffer[10] == ' '))    //�ж�ָ�������Ƿ�Ϊ��W��д��ָ��
                    {
                        j = COM2.RX_Cnt - 11;
                        if(j > Max_Length)  j = Max_Length; //Խ����
                        //EEPROM_SectorErase(addr);           //��������
                        EEPROM_write_n(addr,&RX2_Buffer[11],j);      //дN���ֽ�
                        PrintString2("��д��");
                        if(j >= 100)    {TX2_write2buff((u8)(j/100+'0'));   j = j % 100;}
                        if(j >= 10)     {TX2_write2buff((u8)(j/10+'0'));    j = j % 10;}
                        TX2_write2buff((u8)(j%10+'0'));
                        PrintString2("�ֽڣ�\r\n");
                        status = 0; //������ȷ
                    }

                    else if((RX2_Buffer[0] == 'R') && (RX2_Buffer[10] == ' '))   //PC���󷵻�N�ֽ�EEPROM����
                    {
                        j = GetDataLength();
                        if(j > Max_Length)  j = Max_Length; //Խ����
                        if(j > 0)
                        {
                            PrintString2("����");
                            TX2_write2buff((u8)(j/10+'0'));
                            TX2_write2buff((u8)(j%10+'0'));
                            PrintString2("���ֽ��������£�\r\n");
                            EEPROM_read_n(addr,tmp,j);
                            for(i=0; i<j; i++)  TX2_write2buff(tmp[i]);
                            TX2_write2buff(0x0d);
                            TX2_write2buff(0x0a);
                            status = 0; //������ȷ
                        }
                    }
                }
            }
            if(status != 0) PrintString2("�������\r\n");
            COM2.RX_Cnt = 0;
        }
    }
} 
/**********************************************/
