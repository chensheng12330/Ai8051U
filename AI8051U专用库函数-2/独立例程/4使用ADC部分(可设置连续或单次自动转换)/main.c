#include "AI8051U.h"
#include "set_uart.h" //���ô��ڵĿ⺯�����ḻ��ʹ�÷������H�ļ��е���ϸ˵��
#include "set_timer.h"//���ö�ʱ���Ŀ⺯�������ڹ̶�ʱ�份�ѷ��͸���λ��ADC�������
#include "set_adc.h"//����ADC�Ŀ⺯��������ADC����

/*
ʹ��˵����
����1��������115200����ÿ��0.5S����һ�ε�ǰ��ȡ����ADC���ݡ�
�����������Ҳ�ADC�������Ϳ��Կ������صĶ����仯��
��Ҫע����ǣ���Ҫ�������ܿ��������ı仯������㰴�Ļ����´�ɨ���ʱ���Ѿ����ɿ���״̬�ˡ�
*/

unsigned int show_value;
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	// set_uart.h ����ʹ������
	set_uart_mode(Uart1, Uart_End);
	// ���ô���1ΪĬ��ģʽ��115200bps��64byteֹͣ�жϣ�Ĭ��timer2��p30p31

	// set_timer.h ����ʹ������
	set_timer_mode(Timer0, "0.5s", Timer_End);
	// ���ö�ʱ��0�Ķ�ʱʱ��Ϊ0.5s

	// set_adc.h ����ʹ������
	set_adc_mode(cycl_mode, Adc0_P10, Adc_End);
	// ����Ϊѭ������ģʽ��ÿ�����adcת�����Զ���ʼ��һ��
	EA = 1; // �����ж�
	while (1)
	{
		if(get_adc_state(Adc0_P10))//��ȡADC0ͨ���Ƿ�ת�����
		{
			show_value = adc_value[Adc0_P10];//��ͨ�����ݽ��ж�ȡ
		}
		if(get_timer_state(Timer0))//�趨��0.5sʱ�䵽
		{
			uart_printf(Uart1,"ADC_KEY_Value:%d\r\n",show_value);//���͵�ǰ��ADC����
			//���������ϵ������ǣ�����ĳ��ADC�������ɵõ���������ʾ�ȶ���ADC��ֵ
			//���糤��ADC����0���ͷǳ���������״̬����������
		}
	}
}