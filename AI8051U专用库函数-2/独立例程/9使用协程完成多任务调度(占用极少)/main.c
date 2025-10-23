#include "ai8051u.h"
#include "set_io.h"
#include "set_timer.h"
#include "set_task.h"

/*
ʹ��˵����
ʹ��Э��ǰ��Ҫ�趨Э�̵�ʱ���׼base���Ƽ�ʹ��1ms��ʱ���׼������ʹ�ö�ʱ���жϷ�ʽ����
������ͨ��һ���򵥵Ķ��������չ��Э�̵�ʹ��Ч��
����1��LED00ÿ��100ms��˸һ�Σ�����P32����ʱ����ͣLED00����˸��ѭ��ִ��
����2��LED01��200ms����500ms��ѭ��ִ��
����3��LED02����1000ms��Ȼ��{������˸(50ms)3�Σ�����300ms}<-���ѭ��5�Σ�ѭ��ִ��
*/

int cnt1 = 0, cnt2 = 0;//����ѭ���ñ���

void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	set_timer_mode(Timer0, "1ms", Timer_End);//�趨��ʱ��0��ʱʱ��Ϊ1ms
	set_timer_isr(Timer0, set_task_mode);//����Э��ʱ���׼
	
	set_io_mode(pu_mode, Pin40, Pin00, Pin01, Pin02, Pin_End);//�趨P40��P00��P01��P02��ģʽΪ׼˫���ģʽ
	P40 = 0;//��LED�Ĺ���
	//set_io_mode(hz_mode, Pin32, Pin_End);//�ϵ�˿�Ĭ��Ϊ���裬���Կ��Բ����ظ����ö˿�ģʽ
	set_io_mode(en_pur, Pin32, Pin_End);//��P32�˿ڵ��������裬�����ȡ����״̬
	EA = 1;//�����ж�
	while(1)
	{
		task_start(0);//Э��0��ʼ
		P00 = ~P00;//ÿ��ִ��ȡ��P00�˿ڵ�ƽ
		task_delay(100);//�趨��ʱ100ms
		task_wait(!P32);//P32��ƽ��Ϊ�ж�������Ϊ1��ȴ���Ϊ0���������ִ��
		//��Ϊ��������Ϊ0������������һֱ��˸������P32��LED00ֹͣ��˸
		task_end(1);//Ϊ1��ѭ��ִ��
		
		task_start(1);//Э��1��ʼ
		P01 = 0;//P01�˿���0��LED��
		task_delay(200);//�趨��ʱ200ms
		P01 = 1;//P01�˿���1��LED��
		task_delay(500);//�趨��ʱ500ms
		task_end(1);//Ϊ1��ѭ��ִ��
		
		task_start(2);//Э��2��ʼ
		task_delay(1000);//�ȳ���ʱһ��
		task_for(cnt1=0, cnt1++)//��ʼ�������б��ʽ(�м�Ҫ�ö�������!)
		{
			task_for(cnt2=0, cnt2++)//�ڶ���forѭ��
			{
				P02 = ~P02;//ȡ��P02�˿�
				task_delay(50);//С��ʱ��������˸
			}
			task_break(cnt2<3);//�ڶ���for���ж�������Ϊ1�򷵻�for��ͷ��Ϊ0�����ִ��
			P02 = 0;//ǿ�Ƹ�P02�˿��õ�
			task_delay(300);//�趨��ʱ300ms
		}
		task_break(cnt1<5);//��һ��for���ж�������Ϊ1�򷵻�for��ͷ��Ϊ0�����ִ��
		task_end(1);//Ϊ1��ѭ������
	}
}

