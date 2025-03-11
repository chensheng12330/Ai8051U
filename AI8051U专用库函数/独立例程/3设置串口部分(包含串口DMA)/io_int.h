#ifndef __IO_INT_H__
#define __IO_INT_H__

#include "AI8051U.H"
#include "set_io.h"
// IOINTģʽö��
typedef enum
{
    dis_int = 0,              // ��ֹIO�ж�
    en_int,                   // ʹ��IO�ж�
    falling_edge_mode = 10,   // �½����ж�ģʽ
    rising_edge_mode,         // �������ж�ģʽ
    low_level_mode,           // �͵�ƽ�ж�ģʽ
    high_level_mode,          // �ߵ�ƽ�ж�ģʽ
    priority_base = 20,       // �ж����ȼ����(Ĭ��)
    priority_low,             // �ж����ȼ���
    priority_medium,          // �ж����ȼ���
    priority_high,            // �ж����ȼ���
    en_wakeup = 30,           // ʹ��IO����
    dis_wakeup                // ��ֹIO����
} ioint_mode;

// �ⲿʹ�ú�������һ������Ϊioint_modeö�����ͣ��ڶ��������������Ϊio_nameö������
// ����һ���䳤��������һ�����ӣ�
// set_ioint_mode(en_int,Pin00,Pin01,Pin02,Pin20,Pin_End);
// �����ǽ�Pin00,Pin01,Pin02,Pin20��IO�ж�ģʽ��ʹ�ܣ�������ΪPin_End
void set_ioint_mode(ioint_mode mode, ...);

// ����һ����ȡ�жϱ�־λ�ĺ���������Ϊio_nameö�����ͣ�����ֵΪchar����
// ����ֵֻ�����0��1��0��ʾû���жϣ�1��ʾ���жϣ���ѯһ�κ��Զ����
char get_ioint_state (io_name pin);

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P1 = ~P1};
//set_ioint_isr(Pin01, isr);//����isr����ΪP0�ڵ��жϺ���(Pin00~Pin07����һ��Ч��)
void set_ioint_isr(io_name pin, void (*isr)(void)); // �����жϷ������

#endif