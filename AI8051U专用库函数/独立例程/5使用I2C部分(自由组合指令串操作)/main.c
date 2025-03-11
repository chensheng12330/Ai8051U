#include "AI8051U.h"
#include "set_i2c.h"  //����I2C�Ŀ⺯����֧�ֿ��ָ��ϲ���
#include "set_io.h"	  //����I/O���ֵĿ⺯��
#include "set_uart.h" //���ô��ڵĿ⺯�����ḻ��ʹ�÷������H�ļ��е���ϸ˵��
#include "stdio.h"	  //ʹ��sscanf�������õĿ�
#include "string.h"	  //ʹ��strncmp�������õĿ�
/*
ʹ��˵����
����1��������115200��P30��P31�ڣ����򴮿ڷ���"get"�����ɷ���һ�����֡�
ʹ��"set:1"(1�����滻Ϊ0~255�ڵ���������)���������������
�������ʹ���������ϵ�AT24C02���д洢���ϵ�Ҳ���ᶪʧ��
*/

int num = 0;
char c = 0;

void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	// set_uart.h ����ʹ������
	set_uart_mode(Uart1, Uart_End);
	// ���ô���1ΪĬ��ģʽ��115200bps��64byteֹͣ�жϣ�Ĭ��timer2��p30p31

	// set_i2c.h ����ʹ������
	set_i2c_mode(I2c0, "50khz", I2c_P32_3, I2c_End);
	// ʹ��50khz��ͨѶ�ٶ�,Ĭ������ģʽ�������л�ΪI2c_P32_3

	// set_io.h ����ʹ������
	set_io_mode(pu_mode, Pin32, Pin33, Pin_End); // ����P24,P23Ϊ׼˫��
	set_io_mode(en_pur, Pin32, Pin33, Pin_End);	 // ����P24,P23���ڲ����������
	EA = 1;										 // �����ж�
	while (1)
	{
		if (get_uart_state(Uart1)) // ��⴮��1�Ƿ�������
		{
			if (strncmp(_uart1_rx_buff, "get", 3) == 0)					  // ��⴮��1�Ƿ�����get�ַ�,strncmpƥ��ɹ�����0���������������������
				set_i2c_cmd(I2c0, 1, S_Tx_Rack, 0xa0, Tx_Rack, 0x00,	  // ������ַ0xa0��д��ַ00
							S_Tx_Rack, 0xa1, Rx_Tnak, &c, Stop, Cmd_End); // ���¿�ʼ��������ַ0xa1��������
			if (sscanf(_uart1_rx_buff, "set:%d", &num) == 1)			  // ��⴮��1�Ƿ�����set:����
				set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xa0, Tx_Rack, 0x00,	  // ����i2c��������������Ϊ0,������ַ0xa0
							Tx_Rack, num, Stop, Cmd_End);				  // ���д����ַ0д��num
		}
		if (get_i2c_state(I2c0, 0))
			uart_printf(Uart1, "set_yes\r\n"); // ���i2c������0�Ƿ����,��ɻش�yes
		if (get_i2c_state(I2c0, 1))
			uart_printf(Uart1, "num:%d\r\n", (int)c); // ���i2c������1�Ƿ����,��ɺ�ش�num
	}
}