#include "AI8051U.h"
#include "io_int.h" //����IO�жϵĿ⺯��,������set_io.h
#include "set_io.h" //����IOģʽ�Ŀ⺯���������Ķ�H�ļ��е���ϸ˵��������ֱ�ӿ��������̵�ʹ��

/*
ʹ��˵����
Ч��Ӧ����P02��P04��P07������P00��001΢΢����
��һ���������ϵ�P34�����ͻ��ԭ��LED��״̬ȡ����
*/

void main(void)
{
    EAXFR = 1; // ���������չ�Ĵ���
    WTST = 0;
    CKCON = 0;
    // AI8051U��������������
    P40 = 0; // ��������LED��Դ����
    // set_io.h ����ʹ������
    set_io_mode(pp_mode, Pin02, Pin04, Pin07, Pin40, Pin_End); // ����P02,P04,P07,P40Ϊ�������ģʽ
    set_io_mode(en_pdr, Pin00, Pin01, Pin_End);              // ����P00,P01Ϊ�������迪��
    set_io_mode(en_pur, Pin34, Pin_End);                     // ����P34�������������迪��
    // ���һ������������Pin_End���������޷�ֹͣ������Pin_End�ǽ�����־
    // �䳤�����������趨��������������
	
    // io_int.h ����ʹ������
    set_ioint_mode(falling_edge_mode, Pin34, Pin_End); // ����P34Ϊ�½����ж�ģʽ
    set_ioint_mode(en_int, Pin34, Pin_End);         // ��P34��IO���ж�
    EA = 1;                                         // �����ж�
	
    P0 = 0x00;
    // ��P0�ڵĲ���LED������������ʹ��AI8051U���������
    // ʵ��Ч��Ӧ����P02��P04��P07������P00��001΢΢��
    while (1)
    {
        if (get_ioint_state (Pin34))
        {
            P0 = ~P0; // ��תP0��ֵ,��һ���������ϵ�P34�����ͻ����һ����/���л�
        }
    }
}