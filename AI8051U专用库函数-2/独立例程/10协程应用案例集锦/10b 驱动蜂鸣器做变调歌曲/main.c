#include "AI8051U.H"
#include "set_io.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
ʹ��˵����
ͨ��ʵ�����ϵ�����ñ3���������ӿڲ�ú󣬼����������ķ���������
ʹ���������̣߳��߳�0��������������������߳�1�����޸ķ�������Ƶ��
��ʼ��ʱ�����߳�ʱ���׼�޸ĵ���Ϊ100us������Ƶ�ʵ���
*/

#define Beep P50 //����������
char x_delay = 0;//��������ʱ����,��ʱ����
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "100us", Timer_End);//���ö�ʱ��0��ʱʱ��,����ʱ��Ϊ100us
	set_timer_isr(Timer0, set_task_mode);//���ö�ʱ���ж�Ϊ�������
	set_io_mode(pu_mode, Pin50, Pin_End);//���÷���������Ϊ׼˫���
	EA = 1;//�����ж�
	while(1)
	{
		task_start(0);//�߳�0��ʼ,����������
		Beep = ~Beep;
		task_delay(x_delay);
		task_end(1);//�߳�0����,ѭ��ִ��

		task_start(1);//�߳�1��ʼ,���Ʒ��������
		task_for(x_delay = 0, x_delay++)
		{
			task_delay(1000);//��������100us*1000=100ms
		}
		task_break(x_delay < 20);
		task_for(x_delay = 20, x_delay--)
		{
			task_delay(300);//Ѹ���½�100us*300=30ms
		}
		task_break(x_delay > 0);
		task_end(1);//�߳�1����,ѭ��ִ��
	}
}
