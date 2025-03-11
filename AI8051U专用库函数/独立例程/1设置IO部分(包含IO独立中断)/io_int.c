#include "io_int.h"
#include "stdarg.h"

char xdata ioint_isr_state[6] = {0,0,0,0,0,0}; // �����ж�״̬����
char ioint_flag = 0;                  // �����жϱ�־�����ڼ��ٲ�ѯ����
void (*_ioint_isr[6])(void) = {0};    //���庯��ָ������

#define _XDATA_REG(offset) (*(unsigned char volatile far *)(0x7efd00+offset))
#define _SET_XDATA_REG(offset, num, value) { _XDATA_REG(offset) &= ~(1<<(num));_XDATA_REG(offset) |= ((value) & 1) << (num);}
char ioint_offset[4] = {0x0,0x20,0x40,0x42};//�ж�ʹ��ƫ�Ƶ�ַ���ж�ģʽƫ�Ƶ�ַ���ж����ȼ�ƫ�Ƶ�ַ���жϻ���ƫ�Ƶ�ַ
// ����꣺ͨ������ IO ����
#define SET_IOINT(port, num, value, type){if((type/10)==1){_SET_XDATA_REG(ioint_offset[type/10]+port, num, (type%10)&1);\
    _SET_XDATA_REG(ioint_offset[type/10]+0x10+port, num, ((type%10)>>1)&1);/*�����ж�ģʽ*/\
    }else if((type/10)==2){_SET_XDATA_REG(ioint_offset[type/10], port, (type%10)&1);\
    _SET_XDATA_REG(ioint_offset[type/10]+1, port, ((type%10)>>1)&1);/*�����ж����ȼ�*/\
    }else{_SET_XDATA_REG(ioint_offset[type/10]+port, num, (type%10)&1);}/*�����жϻ��ѡ��ж�ʹ��*/}

void set_bit_ioint(char port, char num, ioint_mode mode)
{
    if (port < 0 || port > 5)return; // ���˿ں��Ƿ���Ч
    SET_IOINT(port, num, mode, mode);
}

void set_ioint_mode(ioint_mode mode, ...)
{
    va_list args;         // �ɱ�����б�
    char port, num;       // �˿ں�
    va_start(args, mode); // ��ʼ���ɱ�����б�
    while (1)
    {
        io_name pin = va_arg(args, io_name); // ��ȡ��һ�����ű��
        if (pin == Pin_End)
            break;                      // �����ڱ�ֵ������ѭ��
        port = pin / 8;                 // ����˿ںţ�P0~P5��
        num = pin % 8;                  // �������źţ�0~7��
        set_bit_ioint(port, num, mode); // ��������ģʽ
    }
    va_end(args); // ����ɱ�����б�
}

char get_ioint_state(io_name pin)
{
    char state, port, num;
    if (ioint_flag == 0)
        return 0;
    port = pin / 8; // ����˿ںţ�P0~P5��
    num = pin % 8;  // �������źţ�0~7��
    state = ((ioint_isr_state[port] >> num) & 1);
    ioint_isr_state[port] &= ~(1 << num); // ����ж�״̬λ
		ioint_flag = ioint_isr_state[0]|ioint_isr_state[1]|ioint_isr_state[2]|
		ioint_isr_state[3]|ioint_isr_state[4]|ioint_isr_state[5];
    return (state); // �����ж�״̬
}

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P1 = ~P1};
//set_ioint_isr(Pin01, isr);//����isr����ΪP0�ڵ��жϺ���(Pin00~Pin07����һ��Ч��)
void set_ioint_isr(io_name pin, void (*isr)(void)) // �����жϷ������
{
		char port = pin / 8;//����˿ں�
		if(port >= 6)return;//��Χ����
    _ioint_isr[port] = isr;
}

// �жϲ��ֺ궨��
#define pin_int_isr(port_num)                                          \
    void p##port_num##_int_isr(void) interrupt P##port_num##INT_VECTOR \
    {                                                                  \
				ioint_flag = 1;/* �����жϼ��ٱ�־λ */												\
        ioint_isr_state[port_num] |= (P##port_num##INTF&P##port_num##INTE); \
        P##port_num##INTF = 0; /* ����жϱ�־λ */             \
				if(_ioint_isr[port_num] != 0)_ioint_isr[port_num]();                       \
    }
pin_int_isr(0) pin_int_isr(1) pin_int_isr(2) pin_int_isr(3) pin_int_isr(4) pin_int_isr(5)