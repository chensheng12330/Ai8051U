#include "AI8051U.h"
#include "ai_usb.h"    //����USB�⺯��ͷ�ļ�
#include "set_int.h"   //�����ⲿ�жϵĿ⺯��
#include "set_io.h"    //����IOģʽ�Ŀ⺯���������Ķ�H�ļ��е���ϸ˵��������ֱ�ӿ��������̵�ʹ��
#include "set_timer.h" //���ö�ʱ���Ŀ⺯��

/*
ʹ��˵����
Ч��Ӧ���ǰ���P32���İ�����P00��LED00�����ĵƸı�һ����/��״̬��ͬ��P33���¸ı�P01��LED01��
ͬʱ��P02��LED02����1��ļ����������
*/
int cnt = 0;//���Լ�������
void main(void)
{
    EAXFR = 1; // ���������չ�Ĵ���
    WTST = 0;
    CKCON = 0;
		usb_init();//��ʼ��USB�⺯��
    // AI8051U��������������
    P40 = 0; // ��������LED��Դ����
    // set_io.h ����ʹ������
    set_io_mode(pp_mode, Pin40, Pin_End);
    // ����P40Ϊ�������ģʽ
		set_io_mode(pp_mode, Pin35, Pin_End);
    // ����T1=P35Ϊ׼˫��ģʽ���ֶ�ʹ�ñ任�������ʹ��T1�ڼ���ģʽ�¼���
    set_io_mode(pu_mode, Pin00, Pin01, Pin02, Pin03, Pin32, Pin33, Pin_End);
    // ����P00,P01,P02,P32(INT0),P33(INT1)Ϊ׼˫��ģʽ
    // ���һ������������Pin_End���������޷�ֹͣ������Pin_End�ǽ�����־
    // �䳤�����������趨��������������

    // set_int.h ����ʹ������
    set_int_mode(falling_edge_mode, Int0, Int1, Int_End); // ����INT0��INT1�ж�ģʽΪ�½��ش���

    // set_timer.h ����ʹ������
    set_timer_mode(Timer0, Timer_End); // ���ö�ʱ��0ΪĬ�����ã���ʱʱ��ΪΪ1s�������жϣ��ر�ʱ�����
    // ��ʱ������Ϊ��ʱʱ��Ͷ�ʱ�����������뷽ʽ����������룬Ĭ��ֵΪ��ʱʱ��1s
    // ��ʱ����֧�ֵ�λhz��Сд�ĺ��ȣ�����ʱʱ��֧�ֵ�λms��Сд�ĺ��룩��s ����д���룩
    // ֧�ָ������룬������"1.5s"��"120ms"��"333hz"���ǺϷ�������
    // ���ú���Զ�������ʱ���Ͷ�Ӧ�жϣ�ֻ��Ҫʹ��get_timer_state��ѯ����
		set_timer_mode(Timer1, Cnt_Mode, Timer_End);
		//����Timer1Ϊ����ģʽ����������˿ڹ̶�ΪP35�������������ͼ�ͼ����ֲ�˵����
		set_timer_mode(Timer2, "10ms", Timer_End);
		// ���ö�ʱ10ms������������P35�˿�ȡ��������T1�������������
    EA = 1; // �����ж�
    while (1)
    {
        // �����ǰ���P32���İ�����P00���ĵƸı�һ����/��״̬��ͬ��P33���¸ı�P01
        // ͬʱ��P02��1��ļ����������
        if (get_int_state(Int0))
            P00 = ~P00; // ���INT0�жϣ���תP00��ƽ
        if (get_int_state(Int1))
            P01 = ~P01; // ���INT1�жϣ���תP01��ƽ
        if (get_timer_state(Timer0))
            P02 = ~P02; // ��ӦIO���ϵ�LED��1S�仯һ��
				if (get_timer_state(Timer2))
				{
					P35 = ~P35;
					cnt++;
					if(cnt == 50)
					{
						printf_usb("cnt:%d, timer_cnt:%u", cnt, get_timer_cnt(Timer1));
						//����Ӧ��Ϊcnt:50, timer_cnt:25����Ϊȡ���У� ���β���������һ�����ڱ仯
						cnt = 0;
					}
				}
    }
}