#ifndef __SET_TIMER_H__
#define __SET_TIMER_H__

#include "AI8051U.H"

// ��ʱ��ö��
typedef enum
{
    Timer0 = 0, // ��ʱ��0����ʹ��ģʽ0~3
    Timer1,     // ��ʱ��1����ʹ��ģʽ0~2
    Timer2,     // ��ʱ��2~4ֻ��ʹ��ģʽ0
    Timer3,
    Timer4,
    Timer11
} timer_num;

// ��ʱ����������
#define Timer_End "end"              // ������־
#define En_Int "\x01int1"         // �򿪶�Ӧ��ʱ�����жϹ��ܣ�Ĭ���Ǵ򿪵�
#define Dis_Int "\x01int0"       // �رն�Ӧ��ʱ�����жϹ���
#define En_OutClk "\x01outclk1"   // �򿪶�Ӧ��ʱ����ʱ��������ܣ�Ĭ���ǹرյ�
#define Dis_OutClk "\x01outclk0" // �رն�Ӧ��ʱ����ʱ���������
// T0CLKO=P35, T1CLKO=P34, T2CLKO=P11, T3CLKO=P05, T4CLKO=P07, T11CLKO=P21
// ��ʱ������Ϊ��ʱʱ��Ͷ�ʱ�����������뷽ʽ����������룬Ĭ��ֵΪ��ʱʱ��1s
// ��ʱ����֧�ֵ�λhz��Сд�ĺ��ȣ�����ʱʱ��֧�ֵ�λms��Сд�ĺ��룩��s ����д���룩
// ֧�ָ������룬������"1.5s"��"120ms"��"333hz"���ǺϷ�������

// �������ܣ����ö�ʱ��ģʽ��֧�ֲ������������Ĭ��ֵ���ù���
// Ĭ��ֵΪ1s��ʱ�����жϣ��ر�ʱ��������ܡ���set_timer_mode(Timer0, Timer_End);
// ��Ч�ڣ�set_timer_mode(Timer0, "1s", Dis_OutClk, En_Int, Timer_End);
// ����������ӵĹ���������Timer0Ϊ1000hz�Ķ�ʱ����
// set_timer_mode(Timer0, "1000hz", Timer_End);
// ����������ӵĹ���������Timer0Ϊ10ms�Ķ�ʱ����
// set_timer_mode(Timer0, "10ms", Timer_End);
void set_timer_mode(timer_num num, ...);

//�����ǻ�ȡ��ʱ�����ж�״̬�ģ�ʹ�ø߼����ù��ܺ󣬻��Զ�������ʱ���Ͷ�Ӧ���жϡ�
//�жϴ��������Ѿ���������ˣ��û�ֻ��Ҫ���Ķ�ʱ��״̬����ֵ���ɡ�
//��������Ĳ����Ƕ�ʱ���ı�ţ�����ֵ�Ƕ�ʱ����״̬��0��ʾ��ʱ��û���жϣ�1��ʾ��ʱ���жϡ�
char get_timer_state(timer_num num);

//�������ö�ʱ�����ֵ�ʱ��ԴƵ�ʣ�foscΪʱ��Ƶ�ʣ���λΪHz��
//��ʹ���������������£���Ƶ���Զ���ȡ���û����Բ��ù���������⡣
void set_timer_fosc(long fosc);

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P0 = ~P0};
//set_timer_isr(Timer0, isr);//����isr����ΪTimer0���жϺ���
void set_timer_isr(timer_num num, void (*isr)(void)); // �����жϷ������

#endif