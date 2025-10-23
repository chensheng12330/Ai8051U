#include "AI8051U.h"
#include "io_int.h"	  //����IO�����жϵĿ⺯����������set_io.h
#include "set_io.h"	  //����IOģʽ�Ŀ⺯���������Ķ�H�ļ��е���ϸ˵��������ֱ�ӿ��������̵�ʹ��
#include "set_uart.h" //���ô��ڵĿ⺯�����ḻ��ʹ�÷������H�ļ��е���ϸ˵��
#include "stdio.h"

/*
ʹ��˵����
����P32������1��ӡhello world����Ҫʹ��USBת���ڽ���TyepC���ؿ��ϣ���������115200bps
ʹ�ô���1���͡�cnt:123��(Ӣ�ķ��ţ����Ų���������)��Ӧ�÷���send num:123��
������ʲô���֣������󷵻�ʲô���֡�
*/

char tmp_str[5] = {'a', 'b', 'c', 'd', 'e'};
int cnt_dat = 0;
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	// AI8051U��������������
	// ���һ������������Pin_End���������޷�ֹͣ������Pin_End�ǽ�����־
	// �䳤�����������趨��������������
	set_io_mode(en_pur, Pin32, Pin_End); // ����P32���������迪��

	// io_int.h ����ʹ������
	set_ioint_mode(falling_edge_mode, Pin32, Pin_End); // ����P32���½����ж�
	set_ioint_mode(en_int, Pin32, Pin_End);			// ����P32���ж�ʹ��

	// set_uart.h ����ʹ������
	set_uart_mode(Uart1, Uart_End);
	// ���ô���1ΪĬ��ģʽ��115200bps��64byteֹͣ�жϣ�Ĭ��timer2��Uart1_P30_1
	set_uart_mode(Uart4, "9600bps", Use_Timerx, Uart4_P02_3, Uart_End);
	// ���ô���4Ϊ9600bps��32byteֹͣ�жϣ�ʹ�ö�ʱ3����Ӧ���ںŵĶ�ʱ�������л�ΪUart4_P02_3
	// ע�⣺���ʹ���˲�ͬ�Ĳ����ʣ������ʹ��Use_Timerx�������ͬ�����ʣ�����Թ���Ĭ�ϵ�Timer2
	set_io_mode(pu_mode, Pin02, Pin03, Pin_End);
	// ���ö�Ӧ����ǰ�ǵ�ʹ��set_io_mode������������,�����Ӧ����Ϊ����ģʽ�޷��������
	EA = 1; // �����ж�
	while (1)
	{
		if (get_uart_state(Uart1))
		{
			// ע�⣺ʹ��sscanf��Ҫ����stdio.h
			sscanf(_uart1_rx_buff, "cnt:%d", &cnt_dat); // ���������Բ鿴set_uart.h�л������Ķ���
			// sscanf�÷�����һ�������ǻ��������ڶ��������Ǹ�ʽ���ַ����������������Ǳ�����ַ
			uart_printf(Uart1, "send num:%d\r\n", (int)cnt_dat); // ����1��ӡ�����������ݲ���ʾ
		}
		if (get_ioint_state(Pin32)) // ����P32������1��ӡhello world
		{
			uart_printf(Uart1, "hello world!\r\n");
			// ��ͨprintf�÷�����Ƕprintf����������ͨ����һ������ʵ�ִ�ӡ���ڵ�ѡ��
			// ��printf�Դ�����У��ʹ���æ��־���������Ȼ᲻��ӡ���뵽set_uart.h�иı�Uart_Tmp_Max
			// �����������printf���ڵ�һ��printfû����ɷ��͵�����£�������pritnf�ᱻ����
			// �����Ҫ֪����Ӧ�Ĵ��ڷ����Ƿ�æ������ʹ��tx_state[Uart1]����������ѯ������Ǵ���1�ģ�

			// uart_printf(Uart1,Hex_Mode,0x0f);//���0x0f���ֽڣ�����ֱ�Ӹ�SBUFֵ
			// uart_printf(Uart1,Buff_Mode,tmp_str,5);//����ַ���tmp_str��5���ֽ�
		}
	}
}