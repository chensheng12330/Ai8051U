#include "set_io.h"	 //����I/O���ֵĿ⺯��
#include "set_pwm.h" //����PWM���ֵĿ⺯����֧�ֻ�����ռ�ձȺ���������
#include "set_uart.h" //���ô��ڲ��ֵĿ⺯��
#include "AI8051U.H"
/*
ʹ��˵����
PWMͨ��P00��P02���������ͬռ�ձȵ�PWM������LED���Ϲ۲쵽�����Ƶ����Ȼ᲻һ��
���У�P01��ͨ��PWM1N���PWM����PWM5P���в����ʹ�ô�����ʾ�����ڷ��������ַ���������ռ�ձȺ�����
��Ϊ���񵽵���PWM1N�����źţ�����ռ�ձ���Ӧ��Ӧ��Ϊ20%��100%��ȥ80%�õ���
*/
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	
	//set_uart.h ����ʹ������
	set_uart_mode(Uart1, Uart_End);//ȫĬ��ֵ��������PWM

	// set_io.h ����ʹ������
	set_io_mode(pp_mode, Pin40, Pin00, Pin01, Pin02, Pin_End);
	// ����ʵ�����ϵ�LED��ԴP40���Լ�LED�����ڵ�IO��P00��P02Ϊ�������ģʽ
	P40 = 0; // ��LED�Ƶĵ�Դ
	
	// set_pwm.h ����ʹ������
	set_pwm_mode(Pwm1, Pwm1_P00_01, "2khz", "80%", En_Out_PN, Pwm_End);//�򿪻������
	set_pwm_mode(Pwm2, Pwm2_P02_03, "20%", Pwm_End);
	// �л�PWMΪ��ͬ��ռ�ձ�
	set_pwm_mode(Pwm5, Pwm5_P01, Pwm_In_Mode, Self_Capture, Cap_In_Rising_Edge, Pwm_End);
	//��PWM5P�л���P01�ڣ��л���PWM����ģʽ��ӳ�䵽��ǰͨ��������������
	set_pwm_mode(Pwm6, Pwm_In_Mode, Cross_Capture, Cap_In_Falling_Edge, Pwm_End);
	//�л�Ϊ����ģʽ��ӳ�䵽�Ա�ͨ��(PWM5P),�����½���
	EA = 1;//ʹ�����ж�
	while (1)
	{
		if(get_uart_state(Uart1))//��⵽Uart1����������
		{
			uart_printf(Uart1, "duty:%.2f, cycl:%.2fkhz\r\n",get_pwm_duty(Pwm5_Pwm6), get_pwm_period(Pwm5, khz));
			//���ص�ǰ���񵽵�ռ�ձȺ����ڣ����ڵ�λΪkhz��
		}
	}
}