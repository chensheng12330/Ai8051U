#include "set_int.h"
#include "stdarg.h"

//����ģʽ��
#define SET_MODE(mode, x) \
switch(mode) { \
case rising_falling_edge_mode: IT##x = 0; EX##x = 1; break; \
case falling_edge_mode: IT##x = 1; EX##x = 1; break; \
case dis_int: IT##x = 0; EX##x = 0; break; }\

char int_flag = 0;//�жϱ�־,���ڻ����ȡ�����жϱ�־
void (*_int_isr[5])(void) = {0};//���庯��ָ������

//�����ж�ģʽ
void set_int_mode(int mode, ...)
{
    va_list args;// �ɱ�����б�
    va_start(args, mode);  // ��ʼ���ɱ�����б�
    while (1) {
        int_num num = va_arg(args, int_num);  // ��ȡ��һ��INT���
        if (num == Int_End||num>Int4) break;  // �����ڱ�ֵ������ѭ��
        switch(num)
        {
            case Int0:SET_MODE(mode,0);break;//����INT0��ص�ģʽ
            case Int1:SET_MODE(mode,1);break;//����INT1��ص�ģʽ
            default:
            if(mode == dis_int) INTCLKO &= ~(1<<(num+2));//��ֹ�ж�
            else INTCLKO |= (1<<(num+2));break;//����INT2��INT3��INT4��ص�ģʽ
        }
    }
    va_end(args);  // ����ɱ�����б�
}

//��ȡ�ж�״̬�������жϱ�ţ�����0Ϊû���жϣ�����1Ϊ���ж�
char get_int_state(int_num num)
{
    char state = 0;// �ж�״̬
    if(int_flag == 0)return 0;// û���ж�,��ǰ����
    state = ((int_flag>>num)&1);// ��ȡ�ж�״̬
    if(state == 0)return 0;
    int_flag &= ~(1<<num);// ����жϱ�־����
    return state;// �����ж�״̬
}

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P0 = ~P0};
//set_int_isr(Int0, isr);//����isr����ΪInt0���жϺ���
void set_int_isr(int_num num, void (*isr)(void)) // �����жϷ������
{
    _int_isr[num] = isr;
}

#define int_isr(n) \
void int##n##_isr(void) interrupt INT##n##_VECTOR \
{ int_flag |= (1 << n); if(_int_isr[n] != 0)_int_isr[n]();}
int_isr(0)int_isr(1)int_isr(2)int_isr(3)int_isr(4)// �жϷ�����