#include "AI8051U.h"
#include "set_spi.h"  //����SPI���ֵĺ���������DMA����
#include "set_io.h"	  //����I/O���ֵĿ⺯��
#include "set_uart.h" //���ô��ڵĿ⺯�����ḻ��ʹ�÷������H�ļ��е���ϸ˵��
/*
ʹ��˵����
����1��������115200��P30��P31�ڣ����򴮿ڷ����������ݣ����ɷ��ض�ȡ����FLASHID
���ݲ�ͬFLASH���ܻ��ȡ����ͬ��ֵ��ֻҪ�Ƿ�0x00��ID����������Ϊ����ȷ��
*/

char tx_tmp[7] = {0xab, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//��Ҫ���͵�flash����
//��һ���Ƕ�ȡIDָ����ӵ��绽�ѣ��������Ϊ������ȡ���ã�����ֵ������
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	// set_uart.h ����ʹ������
	set_uart_mode(Uart1, Use_Timerx, Uart_End);
	// ���ô���1ΪĬ��ģʽ��115200bps��64byteֹͣ�жϣ�Ĭ��timer2��p30p31

	// set_io.h ����ʹ������
	set_io_mode(pp_mode, Pin40, Pin41, Pin43, Pin52, Pin53, Pin_End); 
	// ����P40,P41,P43,P52,P53Ϊ׼˫��,����IC��ʹ�õ��������ţ�MISO(P42)�����ã�ΪĬ�ϸ���
	set_io_mode(en_pur, Pin42, Pin_End); // ����P42�ڲ�����������򿪣���ǿ������

	// set_spi.h ����ʹ������
	set_spi_mode(SPI0, Spi_P40_1_2_3, Spi_End);//ȫ������Ĭ�ϲ������л�SPI��IO��P40_1_2_3
	EA = 1;										 // �����ж�
	while (1)
	{
		if (get_uart_state(Uart1)) // ��⴮��1�Ƿ�������
		{
			//spi_printf(SPI2, Hex_Mode, 0x0f);//ʹ��Hexģʽ��ʾ��
			P40 = 0;//CS�������ͣ���ʼ����
			spi_printf(SPI0, Buff_Mode, tx_tmp, 7);//ʹ��Buffģʽ��ʾ��
		}
		if(get_spi_state(SPI0))//�������
		{
			P40 = 1;//CS�������ߣ�ֹͣ����
			uart_printf(Uart1, "flash_id:0x%02bx,0x%02bx,0x%02bx\r\n",
				(char)_spi0_rx_buff[4],(char)_spi0_rx_buff[5],(char)_spi0_rx_buff[6]);
		}
	}
}