#include "AI8051U.H"
#include "set_io.h"
#include "set_spi.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"

/*
ʹ��˵����
ͨ��ʵ�����ϵ�����ܿ�����ʾһ���򵥵ĺ��̵Ƶ���ʱ�����µ�led��Ϊ���̵�ָʾ���Ƶƻ���˸
ʹ���������̣߳��߳�0����ˢ���������ʾ���߳�1�Ǻ��̵�1��ִ���̣߳��߳�2�Ǻ��̵�2��ִ���߳�
*/

//�궨����̵ƶ˿�����
//��һ����̵�
#define RED1 P00
#define YELLOW1 P01
#define GREEN1 P02
//�ڶ�����̵�
#define RED2 P05
#define YELLOW2 P06
#define GREEN2 P07
//���Ұ���10s��ƣ�7s�̵ƣ�3s�Ƶ���ѭ��ִ��

//74HC595��������
#define RCK P35 //ˢ�¸ղŵ�����
char show_buff[8] = {0, 0, 17, 17, 17, 17, 0, 0};//��ʾ��������ݻ�����,�м�������ʾ
char _show_buff[2] = {0};//ʵ��ˢ�����û�����
char code seg_data[19] = {
	0x3F,       /*'0', 0*/
    0x06,       /*'1', 1*/
    0x5B,       /*'2', 2*/
    0x4F,       /*'3', 3*/
    0x66,       /*'4', 4*/
    0x6D,       /*'5', 5*/
    0x7D,       /*'6', 6*/
    0x07,       /*'7', 7*/
    0x7F,       /*'8', 8*/
    0x6F,       /*'9', 9*/
    0x77,       /*'A', 10*/
    0x7C,       /*'B', 11*/
    0x39,       /*'C', 12*/
    0x5E,       /*'D', 13*/
    0x79,       /*'E', 14*/
    0x71,       /*'F', 15*/
    0x40,       /*'-', 16*/
    0x00,       /*' ', 17*/
    0x80,       /*'.', 18*/};

char seg_num = 0;//��̬ˢ���õ�����ʱ����
char traffic_lights1 = 0, traffic_lights2 = 0;//��ͨ�ƿ��Ʊ���
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "1ms", Timer_End);//���ö�ʱ��0��ʱʱ��
	set_timer_isr(Timer0, set_task_mode);//���ö�ʱ���ж�Ϊ�������
	set_io_mode(pu_mode, Pin40, Pin00, Pin01, Pin02, Pin05, Pin06, Pin07, Pin_End);
	//����LED��������Ϊ׼˫��ģʽ
	set_io_mode(pu_mode, Pin32, Pin34, Pin35, Pin_End);//����74HC595��������Ϊ׼˫��ģʽ
	P40 = 0;//��LED���ֵ�Դ
	set_spi_mode(SPI0, Spi_P35_4_3_2, Spi_End);//ʹ��SPI������74HC595
	EA = 1;//�����ж�
	while(1)
	{
		task_start(0);//�߳�0������ˢ����ʾ
		task_for(seg_num = 0, seg_num++)
		{
			_show_buff[0] = seg_data[show_buff[seg_num]];//����ˢ�£�1��Ч
			_show_buff[1] = ~(1<<seg_num);//λѡ,0��Ч
			task_delay(1);//��ʱһ�£���ֹˢ�¹���̫����
			spi_printf(SPI0, Buff_Mode, _show_buff, 2);
			task_wait(!get_spi_state(SPI0));//�ȴ�SPI�������
			RCK = 1;RCK = 0;//����ˢ��
		}
		task_break(seg_num < 8);
		task_end(1);//�߳�0������ѭ��ִ��

		task_start(1);//���ڿ��ƺ�·��1
		//���̵�1��10s��ƣ�7s�̵ƣ�3s�Ƶ�
		YELLOW1 = 1;RED1 = 0;//�رջƵƣ��򿪺��
		task_for(traffic_lights1 = 10, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//��0����
			show_buff[1] = traffic_lights1%10;
			task_delay(1000);
		}
		task_break(traffic_lights1>0);
		RED1 = 1;GREEN1 = 0;//�رպ�ƣ����̵�
		task_for(traffic_lights1 = 7, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//��0����
			show_buff[1] = traffic_lights1%10;
			task_delay(1000);
		}
		task_break(traffic_lights1>0);
		GREEN1 = 1;YELLOW1 = 0;//�ر��̵ƣ��򿪻Ƶ�
		task_for(traffic_lights1 = 3, traffic_lights1--)
		{
			show_buff[0] = (traffic_lights1/10)==0?17:traffic_lights1/10;//��0����
			show_buff[1] = traffic_lights1%10;
			task_delay(500);
			YELLOW1 = 1;
			task_delay(500);
			YELLOW1 = 0;//��˸�Ƶ�
		}
		task_break(traffic_lights1>0);
		task_end(1);//�߳�1������ѭ��ִ��

		task_start(2);//���ڿ��ƺ�·��2
		//���̵�2��7s�̵ƣ�3s�Ƶƣ�10s���
		RED2 = 1;GREEN2 = 0;// �رպ�ƣ����̵�
		task_for(traffic_lights2 = 7, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//��0����
			show_buff[7] = traffic_lights2%10;
			task_delay(1000);
		}
		task_break(traffic_lights2>0);
		GREEN2 = 1;YELLOW2 = 0;// �ر��̵ƣ��򿪻Ƶ�
		task_for(traffic_lights2 = 3, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//��0����
			show_buff[7] = traffic_lights2%10;
			task_delay(500);
			YELLOW2 = 1;
			task_delay(500);
			YELLOW2 = 0;//��˸�Ƶ�
		}
		task_break(traffic_lights2>0);
		YELLOW2 = 1;RED2 = 0;// �رջƵƣ��򿪺��
		task_for(traffic_lights2 = 10, traffic_lights2--)
		{
			show_buff[6] = (traffic_lights2/10)==0?17:traffic_lights2/10;//��0����
			show_buff[7] = traffic_lights2%10;
			task_delay(1000);
		}
		task_break(traffic_lights2>0);
		task_end(1);//�߳�1������ѭ��ִ��
	}
}