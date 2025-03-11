/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  ����˵��    **************

�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

UART DMA + SPI DMA + LCM DMA����Һ��������

����ͼƬʱͨ������(P3.0,P3.1)����ͼƬ���ݣ�ͨ��DMA P2Pֱ�ӷ��͵�SPI Flash������Ҫͨ��RAM��������

��ʾʱSPI��ȡFlashͼƬ���ݣ�ͨ��DMA P2P���͸�LCM�ӿ���ʾͼƬ��

��ʾͼƬ�ֱ���:320x240����ͼƬ��320����240�����޸�"lcm.h"��"USE_HORIZONTAL"����Ϊ����(0)��
��ͼƬ��240����320�����޸�"lcm.h"��"USE_HORIZONTAL"����Ϊ����(1)��

����ʹ�õ�SPI Flashһ�����ֻ��д��256�ֽ����ݣ�UART����SPIд��Flash��DMA���ݳ���ֻ����256�ֽڡ�

SPI��ȡFlash��LCM��ʾDMA�����ݳ�������51200�ֽڡ�

DMA P2Pֻ��ҪԴ������Ҫת�������ݳ���(AMT)�����DMA���ݳ���(AMT)����0xffff������͸����

8bit I8080ģʽ, P2�ڽ�������

LCD_RS = P4^5;         //����/�����л�
LCD_WR = P3^6;         //д����
LCD_RD = P3^7;         //������
LCD_CS = P0^5;//P5^3;  //Ƭѡ
LCD_RESET = P4^7;      //��λ

SPI Flash �ӿڣ�
SPI_SS = P4^0;
SPI_MOSI = P4^1;
SPI_MISO = P4^2;
SPI_SCLK = P4^3;

UART �ӿڣ�
RX = P3^0
TX = P3^1

����ʱ, ѡ��ʱ�� 35MHz (Ƶ�ʶ��������system.h�޸�).

******************************************/

#include "system.h"
#include "spi_flash.h"
#include "uart.h"
#include "lcm.h"
#include "stdio.h"
#include "iap_eeprom.h"

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 �ж�Ƶ��, 1000��/��

#define KEY_TIMER 30        //�����������ʱ��(ms)

sbit KEY1 = P3^2;
sbit KEY2 = P3^3;

u16 Key1_cnt;
u16 Key2_cnt;
bit Key1_Flag;
bit Key2_Flag;
bit Key1_Short_Flag;
bit Key2_Short_Flag;

bit B_1ms;          //1ms��־
bit Mode_Flag;
bit AutoDisplayFlag;
u32 Max_addr;

u16 MSecond;

u16 count;

void GPIO_Init(void);
void Timer0_Init(void);
void KeyScan(void);

void main(void)
{
    u8 temp[4];
    
    WTST = 0;  //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXFR = 1; //��չ�Ĵ���(XFR)����ʹ��
    CKCON = 0; //��߷���XRAM�ٶ�

    GPIO_Init();
    Timer0_Init();
    
    LCM_Config();
    LCM_DMA_Config();
    UART1_config(1);  // ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    EA = 1;

    SPI_init();
    LCD_Init(); //LCM��ʼ��
    
    EEPROM_read_n(EE_ADDRESS,temp,4);    //����4�ֽ�
    Max_addr = ((u32)temp[0] << 24) | ((u32)temp[1] << 16) | ((u32)temp[2] << 8) | (u32)temp[3];
    if(Max_addr == 0xffffffff)
    {
        Max_addr = 0;
    }
    
    while(1)
    {
        if(Mode_Flag == 0)
        {
            if(!SpiDmaFlag && (lcdIndex > 0))
            {
                Flash_addr += DMA_AMT_LEN;
                lcdIndex--;
                if(lcdIndex == 0)
                {
                    DMA_LCM_CR = 0;
                    LCD_CS=1;
                    DMA_P2P_CR2 = 0;
                }
                else
                {
                    SPI_Read_P2P(Flash_addr);
                }
            }
        }
        else if(Mode_Flag == 1)
        {
            if((UartDmaRxFlag) && (spiIndex > 0))    //UART�������
            {
                UartDmaRxFlag = 0;
                spiIndex--;

                Flash_addr += DMA_WR_LEN;
                if(spiIndex == 0)
                {
                    Max_addr = Flash_addr;

                    temp[0] = (u8)(Max_addr >> 24);
                    temp[1] = (u8)(Max_addr >> 16);
                    temp[2] = (u8)(Max_addr >> 8);
                    temp[3] = (u8)Max_addr;
                    EEPROM_SectorErase(EE_ADDRESS); //��������
                    EEPROM_write_n(EE_ADDRESS,temp,4);
                    printf("\r\nUART1 receive end!\r\n");  //UART1����һ���ַ���
                    DMA_P2P_CR1 = 0;
                }
                else
                {
                    SPI_P2P_Write(Flash_addr);
                    UR1R_DMA_Start();
                }
            }
        }
        
        if(B_1ms)   //1ms��
        {
            B_1ms = 0;
            KeyScan();
            
            if(AutoDisplayFlag)
            {
                MSecond++;
                if(MSecond >= 1000)
                {
                    MSecond = 0;

                    SPI_DMA_Reset();
                    DMA_UR1R_CR = 0x00;            //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                    DMA_UR1R_STA = 0x00;
                
                    Mode_Flag = 0;
                    if(Flash_addr >= Max_addr)
                    {
                        Flash_addr = 0;
                    }
                    
                    lcdIndex = 3;  //3 * 51200 = 320 * 240 * 2
                    LCD_Display();
                    SPI_DMA_Config();
                    LCM_DMA_Config();

                    SPI_Read_P2P(Flash_addr);
                }
            }
        }
    }
}

//========================================================================
// ����: void delay_ms(u16 ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void delay_ms(u16 ms)
{
    u16 i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    }while(--ms);
}

//========================================================================
// ����: void GPIO_Init(void)
// ����: IO�����ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-24
// ��ע: 
//========================================================================
void GPIO_Init(void)
{
    P0M1 = 0x00;   P0M0 = 0x00;   //����Ϊ׼˫���
    P1M1 = 0x00;   P1M0 = 0x00;   //����Ϊ׼˫���
    P2M1 = 0x00;   P2M0 = 0x00;   //����Ϊ׼˫���
    P3M1 = 0x00;   P3M0 = 0x00;   //����Ϊ׼˫���
    P4M1 = 0x00;   P4M0 = 0x00;   //����Ϊ׼˫���
    P5M1 = 0x00;   P5M0 = 0x00;   //����Ϊ׼˫���
    P6M1 = 0x00;   P6M0 = 0x00;   //����Ϊ׼˫���
    P7M1 = 0x00;   P7M0 = 0x00;   //����Ϊ׼˫���
    
    //P0.5�����ó��������
    P0M0=0x20;
    P0M1=0x00;

    //P2�����ó��������
    P2M0=0xff;
    P2M1=0x00;

    //P3.3,P3.2�����óɸ�������
    //P3.7,P3.6�����ó��������
    P3M0=0xc0;
    P3M1=0x0c;

    //P4.7,P4.5�����ó��������
    P4M0=0xa0;
    P4M1=0x00;

    //P5.3,P5.2�����ó��������
    P5M0=0x0c;
    P5M1=0x00;
    
    P4SR &= 0xf0;   //P4.0~P4.3��ƽת���ٶȼӿ�
    
    P3PU |= 0x0c;   //P3.3,P3.2���ڲ�����ʹ��
}

//========================================================================
// ����: void Timer0_Init(void)
// ����: ��ʱ��0���ú�����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2022-8-24
// ��ע: 
//========================================================================
void Timer0_Init(void)
{
    AUXR = 0x80;    //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;    //Timer0 interrupt enable
    TR0 = 1;    //Tiner0 run
}

//========================================================================
// ����: void timer0_Interrupt(void) interrupt 1
// ����: ��ʱ��0�жϺ�����
// ����: nine.
// ����: none.
// �汾: VER1.0
// ����: 2014-11-28
// ��ע: 
//========================================================================
void timer0_Interrupt(void) interrupt 1
{
    B_1ms = 1;      //1ms��־
}

//========================================================================
// ����: void KeyScan(void)
// ����: ����ɨ�躯����
// ����: none.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================
void KeyScan(void)
{
    if(!KEY1)
    {
        if(!Key1_Flag)
        {
            Key1_cnt++;
            if(Key1_cnt >= 1000)        //����1s
            {
                Key1_Short_Flag = 0;    //����̰���־
                Key1_Flag = 1;            //���ð���״̬����ֹ�ظ�����

                printf("LCM auto display...\r\n");

                SPI_DMA_Reset();
                DMA_UR1R_CR = 0x00;            //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_UR1R_STA = 0x00;
                
                Mode_Flag = 0;
                AutoDisplayFlag = 1;
                Flash_addr = 0;

                lcdIndex = 3;  //3 * 51200 = 320 * 240 * 2
                LCD_Display();
//                printf("Start LCD display...\r\n");
                SPI_DMA_Config();
                LCM_DMA_Config();

                SPI_Read_P2P(Flash_addr);
            }
            else if(Key1_cnt >= KEY_TIMER)    //30ms����
            {
                Key1_Short_Flag = 1;        //���ö̰���־
            }
        }
    }
    else
    {
        if(Key1_Short_Flag)            //�ж��Ƿ�̰�
        {
            Key1_Short_Flag = 0;    //����̰���־

            SPI_DMA_Reset();
            DMA_UR1R_CR = 0x00;            //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
            DMA_UR1R_STA = 0x00;
            
            Mode_Flag = 0;
            if(AutoDisplayFlag)
            {
                AutoDisplayFlag = 0;
                Flash_addr = 0;
            }
            
            if(Flash_addr >= Max_addr)
            {
                Flash_addr = 0;
            }
            printf("Max_addr = %lu, Flash_addr = %lu\r\n",Max_addr,Flash_addr);
            
            lcdIndex = 3;  //3 * 51200 = 320 * 240 * 2
            LCD_Display();
//          printf("Start LCD display...\r\n");
            SPI_DMA_Config();
            LCM_DMA_Config();

            SPI_Read_P2P(Flash_addr);
        }
        Key1_cnt = 0;
        Key1_Flag = 0;
    }

    if(!KEY2)
    {
        if(!Key2_Flag)
        {
            Key2_cnt++;

            if(Key2_cnt >= 1000)        //����1s
            {
                Key2_Short_Flag = 0;    //����̰���־
                Key2_Flag = 1;            //���ð���״̬����ֹ�ظ�����

                SPI_DMA_Reset();
                DMA_LCM_CR = 0x00;
                DMA_LCM_STA = 0x00;
                DMA_UR1R_CR = 0x00;        //bit7 1:ʹ�� UART1_DMA, bit5 1:��ʼ UART1_DMA �Զ�����, bit0 1:��� FIFO
                DMA_UR1R_STA = 0x00;
                
                Mode_Flag = 1;
                Flash_addr = 0;
                Max_addr = 0;
                spiIndex = 0;
                printf("Start Flash Chip Erase...\r\n");
                FlashChipErase();
                printf("Flash Chip Erase OK!\r\n");
            }
            else if(Key2_cnt >= KEY_TIMER)    //30ms����
            {
                Key2_Short_Flag = 1;        //���ö̰���־
            }
        }
    }
    else
    {
        if(Key2_Short_Flag)            //�ж��Ƿ�̰�
        {
            Key2_Short_Flag = 0;    //����̰���־

            Mode_Flag = 1;
            Flash_addr = Max_addr;  //�������ͼƬ

            spiIndex = 600;  //600 * 256 = 320 * 240 * 2
            UartDmaRxFlag = 0;
            SPI_P2P_Write(Flash_addr);
            UART1_DMA_Config();
            printf("Ready to receive data...\r\n");
        }
        Key2_cnt = 0;
        Key2_Flag = 0;    //�����ͷ�
    }
}