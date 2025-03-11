#include <AI8051U.H>
#include "set_io.h"//����I/O���ֵĿ⺯��
#include "set_pwm.h"//����PWM���ֵĿ⺯����֧�ֻ�����ռ�ձȺ���������
/*
ʹ��˵����
PWMͨ��P00��P02���������ͬռ�ձȵ�PWM������LED���Ϲ۲쵽�����Ƶ����Ȼ᲻һ��
*/
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;

	// set_io.h ����ʹ������
	set_io_mode(pp_mode, Pin40, Pin00, Pin02, Pin_End); 
	//����ʵ�����ϵ�LED��ԴP40���Լ�LED�����ڵ�IO��P00��P02Ϊ�������ģʽ
	P40 = 0;//��LED�Ƶĵ�Դ

	// set_pwm.h ����ʹ������
	set_pwm_mode(Pwm1, Pwm1_P00_01, "10khz", "80%", Pwm_End);
	set_pwm_mode(Pwm2, Pwm2_P02_03, "20%", Pwm_End);
	//�л�PWMΪ��ͬ��ռ�ձ�
	while(1)
	{
		//��ѭ��
	}
}