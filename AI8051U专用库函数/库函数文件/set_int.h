#ifndef __SET_INT_H__
#define __SET_INT_H__

#include "AI8051U.H"

// INTģʽö��
typedef enum
{
    rising_falling_edge_mode = 0, // �����ж�ģʽ
    falling_edge_mode,            // �½����ж�ģʽ
    dis_int                       // �ر��ж�
} int_mode;

// INTģʽö��
typedef enum
{
    Int0 = 0,      // �ⲿ�ж�0��֧�ֱ����жϺ��½����ж�
    Int1,          // �ⲿ�ж�1��֧�ֱ����жϺ��½����ж�
    Int2,          // �ⲿ�ж�2����֧���½����ж�
    Int3,          // �ⲿ�ж�3����֧���½����ж�
    Int4,          // �ⲿ�ж�4����֧���½����ж�
    Int_End = 0xff // �ж����ֽ���
} int_num;

//�������ܣ������ⲿ�ж�INT0~INT4�Ĵ�����ʽ.����һ��������ͬһ��ģʽ�Ķ���жϡ�
//��Ϊ�Ǳ䳤���������������Ҫ����INT_End������
//������set_int_mode(all_edge_mode,Int0,Int1,Int_End);//����INT0��INT1Ϊ�����ж�
void set_int_mode(int_mode mode, ...);

// �������ܣ���ȡ�ж�״̬
// ����ֵ��0��ʾû���жϣ�1��ʾ���ж�
// ������if(get_int_state(Int0))//�ж�INT0�Ƿ����ж�
char get_int_state(int_num num);

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P0 = ~P0};
//set_int_isr(Int0, isr);//����isr����ΪInt0���жϺ���
void set_int_isr(int_num num, void (*isr)(void)); // �����жϷ������

#endif