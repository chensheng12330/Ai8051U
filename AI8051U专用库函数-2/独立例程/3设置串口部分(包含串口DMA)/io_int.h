#ifndef __IO_INT_H__
#define __IO_INT_H__

#include "AI8051U.H"
#include "set_io.h"
// IOINTģʽö��
#define dis_int          0      // ��ֹIO�ж�
#define en_int           1      // ʹ��IO�ж�

#define falling_edge_mode 10    // �½����ж�ģʽ
#define rising_edge_mode  11    // �������ж�ģʽ
#define low_level_mode    12    // �͵�ƽ�ж�ģʽ
#define high_level_mode   13    // �ߵ�ƽ�ж�ģʽ

#define priority_base     20    // �ж����ȼ����(Ĭ��)
#define priority_low      21    // �ж����ȼ���
#define priority_medium   22    // �ж����ȼ���
#define priority_high     23    // �ж����ȼ���

#define en_wakeup        30     // ʹ��IO����
#define dis_wakeup       31     // ��ֹIO����

// �ⲿʹ�ú�������һ������Ϊioint_modeö�����ͣ��ڶ��������������Ϊio_nameö������
// ����һ���䳤��������һ�����ӣ�
// set_ioint_mode(en_int,Pin00,Pin01,Pin02,Pin20,Pin_End);
// �����ǽ�Pin00,Pin01,Pin02,Pin20��IO�ж�ģʽ��ʹ�ܣ�������ΪPin_End
void set_ioint_mode(int mode, ...);

// ����һ����ȡ�жϱ�־λ�ĺ���������Ϊio_nameö�����ͣ�����ֵΪchar����
// ����ֵֻ�����0��1��0��ʾû���жϣ�1��ʾ���жϣ���ѯһ�κ��Զ����
char get_ioint_state (io_name pin);

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P1 = ~P1};
//set_ioint_isr(Pin01, isr);//����isr����ΪP0�ڵ��жϺ���(Pin00~Pin07����һ��Ч��)
void set_ioint_isr(io_name pin, void (*isr)(void)); // �����жϷ������

#endif