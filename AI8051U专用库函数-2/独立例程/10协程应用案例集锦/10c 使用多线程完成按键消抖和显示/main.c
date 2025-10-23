#include "AI8051U.H"
#include "set_io.h"
#include "set_spi.h"
#include "set_task.h"
#include "set_timer.h"
#include "ai_usb.h"
#pragma maxargs (200)
/*
ʹ��˵����
ͨ��ʵ�����ϵ�����ܿ�����ʾһ���򵥵ĺ��̵Ƶ���ʱ�����µ�led��Ϊ���̵�ָʾ���Ƶƻ���˸
ʹ���������̣߳��߳�0����ˢ���������ʾ���߳�1�Ǻ��̵�1��ִ���̣߳��߳�2�Ǻ��̵�2��ִ���߳�
*/


//74HC595��������
#define RCK P35 //ˢ�¸ղŵ�����
char show_buff[8] = {17, 17, 17, 17, 17, 17, 0, 0};//��ʾ��������ݻ�����,���������ʾ
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
unsigned char key_scanf = 0;//����ɨ�����
void main(void)
{
	EAXFR = 1; // ���������չ�Ĵ���
	WTST = 0;
	CKCON = 0;
	usb_init();
	set_timer_mode(Timer0, "1ms", Timer_End);//���ö�ʱ��0��ʱʱ��
	set_timer_isr(Timer0, set_task_mode);//���ö�ʱ���ж�Ϊ�������
	set_io_mode(pu_mode, Pin00, Pin01, Pin02, Pin03, Pin06, Pin07, Pin_End);
	set_io_mode(en_pur, Pin00, Pin01, Pin02, Pin03, Pin_End);
	//���ð�����������Ϊ׼˫��ģʽ,���Ҵ��ڲ���������
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
		
		task_start(1);//�߳�1������ɨ�谴��
		P06 = 0;P07 = 1;//��ɨ��0~3����
		task_delay(1);//�ȴ���ƽ�ȶ�
		if((P0&0x0f) != 0x0f)//�ж��а�������
		{
			task_delay(2);//��ʱ2ms,��������
			if((P0&0x0f) != 0x0f)//�ж��а�������
			{
				key_scanf = P0&0x0f;//��¼����ֵ
			}
		}
		P06 = 1;P07 = 0;//��ɨ��4~7����
		task_delay(1);//�ȴ���ƽ�ȶ�
		if((P0&0x0f) != 0x0f)//�ж��а�������
		{
			task_delay(2);//��ʱ2ms,��������
			if((P0&0x0f) != 0x0f)//�ж��а�������
			{
				key_scanf = P0&0x0f|0x10;//��¼����ֵ����ӱ��
			}
		}
		task_end(1);//�߳�1������ѭ��ִ��

		task_start(2);//�߳�2��������ʾ��ֵ
		show_buff[6] = key_scanf/16;//��ʾ��ֵ
		show_buff[7] = key_scanf%16;//��ʾ��ֵ
		task_delay(100);//��ʱ100msˢ��һ��
		task_end(1);//�߳�2������ѭ��ִ��
	}
}