#include "set_timer.h"
#include "math.h"
#include "stdarg.h"
#include "stdio.h"

unsigned long static _main_fosc = 40e6; // Ĭ��Ϊ40Mhz,��Ƶ��
char _set_timer_fosc = 0;                //Ĭ����û�и��ĵģ�������Ƶ���Ϊ1���������Զ���ȡ����
char timer_flag = 0;             // ���ڻ��涨ʱ���ж�״̬
void (*timer_isr[6])(void) = {0};//���庯��ָ������
// ͨ�ú�
#define SET_REGISTER_BIT(reg, dat, num){          \
        reg &= ~((1) << (num)); /* ���Ŀ��λ */ \
        reg |= ((dat) << (num));  /* ����Ŀ��λ */ }
#define SET_OUTCLK_MODE(reg, num, val) SET_REGISTER_BIT(reg, val, num) // �������ʱ��ģʽ
#define SET_INT_MODE(reg, num, val) SET_REGISTER_BIT(reg, val, num)       // �����ж�ģʽ

void set_timer_fosc(long fosc)
{
    _main_fosc = fosc;// ���ô��ڲ���ʹ����Ƶ
    _set_timer_fosc = 1;//�������ڲ��ֵ�Ƶ�����ù���
}

float static code fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // ��ѯһ�ε�ǰ��ʱ��Ƶ�ʣ�ֻ�ܲ�ѯʹ���ڲ�IRC��ֵ��δ��ѯ���Ͳ��ı�_main_fosc��ֵ
{   //�˺�������ʱռ��T4��T11��ʱ�����м��㣬ʹ����ɺ������ͷ�
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //��ʱ�����ֻ���
    long _xM_Value = 0;//��׼ʱ�Ӻʹ���ʱ���µ�ֵ 
    if(_set_timer_fosc == 1)return;//����Ѿ����ù������˳�
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; 
    _T4T3M = T4T3M; _IE2 = IE2; _IE = IE; _EA = EA; EA = 0;
    T4IF = 1; _T4H = T4H; _T4L = T4L;
    T11CR |= 0x01; _T11H = T11H; _T11L = T11L;//�������в���
    T4IF = 0; T11CR &= ~0x01;//����жϱ�־λ
    if((_CLKSEL&0x0f)!=0)return;//��ǰѡ��Ĳ����ڲ�IRCʱ�ӣ����˳�
    IRC48MCR = 0x80; while(!(IRC48MCR&1));//�������ȴ��ڲ�48Mʱ���ȶ�
    CLKDIV = 2;//�л�Ϊ2��Ƶ, ˳�򲻿��Դ��ң�����ֱ�Ӹ�48Mhzʱ�ӻ�����쳣
    CLKSEL |= 0x0c;//ѡ���ڲ�48Mʱ����Ϊϵͳʱ��Դ����ʱsysclk=24Mhz
    T4T3M &= ~0xf0; ET4 = 0;//���T4���ֵļĴ�����Ĭ��״̬���ر��жϣ���ֹ��ת���жϲ��ֺ��ܷ�
    T11CR = 0x14;//�л�Ϊ�ڲ�����IRC��������ʱ�ӣ�1Tģʽ
    T4x12 = 1;//ʹ��1Tģʽ
    T11CR &= ~0x01;//����жϱ�־λ
    T11H = T11L = 0;//���㿪ʼ����
    T4H = 0x80; T4L = 0x00;//32768��clk�����
    T4R = 1;//����T4��ʱ��
    T11CR |= 0x80;//����T11��ʱ��
    while(!(T4IF));//�ȴ�T4�ж�����
    T11CR &= ~0x81;//����жϱ�־λ��ͬʱ�ر�T11
    T4R = 0;//�ر�T4��ʱ��
    _xM_Value = (unsigned long)(((unsigned int)T11H<<8)|(unsigned int)T11L);//��¼ֵ
    T4IF = 0;//����жϱ�־λ
    if(_CLKDIV == 0)_CLKDIV = 1;//�������Ϊ0
    _main_fosc = (unsigned long)((732.421875f)*(float)((float)_xM_Value/(float)_CLKDIV));
    for(_xM_Value = 0; _xM_Value < 15; _xM_Value++){
		if(fabs((float)_main_fosc - fosc_base[_xM_Value])<3e5f){//��ֵ��Χ��0.3Mhz��
            _main_fosc = fosc_base[_xM_Value];}}//�滻��Ԥ��Ƶ��
    T4H = _T4H; T4L = _T4L; T11H = _T11H; T11L = _T11L;
    T11CR = _T11CR; T4T3M = _T4T3M;//�ָ���ʱ������
    CLKSEL = _CLKSEL; CLKDIV = _CLKDIV;//�ָ�ʱ������
    IE2 = _IE2; IE = _IE; EA = _EA;//�ָ��жϲ�������
    _set_timer_fosc = 1;//��ȡһ�κ����ظ���ȡ
}

// ˽�б�������
static unsigned char clock_div = 0;
static unsigned int timer_value = 0;
// ���� clk_div �� time_value
int calculate_clkdiv_time_value(int mode, float time)
{
    float time_scaled = (mode == 12)?(time / 12.0f):time;//����ģʽ���� time,12T ģʽ�£�time ��С 12 ��
    float clk_div_temp = (time_scaled * _main_fosc) / 65536.0f - 1.0f;// ���� clk_div �ĳ�ʼֵ
    float time_value_temp,calculated_time;
    clock_div = (unsigned char)(clk_div_temp>255.0f?255:clk_div_temp<0?0:clk_div_temp);// ȷ�� clk_div �� 0~255 ��Χ��
    time_value_temp = 65536.0f - (time_scaled * _main_fosc) / (clock_div + 1);// ���� time_value
    if (time_value_temp < 0.0f || time_value_temp > 65535.0f)// ��� time_value ������Χ������ clk_div
    {
        clock_div = (unsigned char)((time_scaled * _main_fosc) / 65536.0f);// ���¼��� clk_div
        clock_div = clock_div>255?255:clock_div;// ȷ�� clk_div �� 0~255 ��Χ��
        time_value_temp = 65536.0f - (time_scaled * _main_fosc) / (clock_div + 1);// ���¼��� time_value
    }// ȷ�� time_value �� 0~65535 ��Χ��
    timer_value = (unsigned int)(time_value_temp>65535.0f?65535:time_value_temp<0.0f?0:time_value_temp);
    calculated_time = (65536.0f - timer_value) * (clock_div + 1) / _main_fosc;// ���������Ƿ���Ч
    calculated_time *= (mode == 12)?12.0f:1; // 12T ģʽ�£�ʱ������ 12 ��
    if (fabs(calculated_time - time) < 1e-3f)// �����������Ŀ��ʱ��ƥ�䣬�򷵻ص�ǰģʽ
        return mode == 1 ? 1 : 0; // 1Tģʽ����1��12Tģʽ����0���޷����㷵��2
    return 2;// ��������Ч
}

// ��ʼ���꣬����Ԥ��Ƶ��װ��ֵ
#define INIT_TIMER(psnum,hnum,lnum){                                       \
        psnum##PS = clock_div;                      /* ʱ�ӷ�Ƶ */       \
        hnum = (unsigned char)(timer_value>>8); /* װ�ض�ʱ����λ */   \
        lnum = (unsigned char)(timer_value); /* װ�ض�ʱ����λ */}
// 12T/1Tģʽѡ�����������
#define TIMER_TxTR(timer_num,run){                                    \
        T##timer_num##x12 = _1tx12t == 0 ? 0 : 1; /* 1T 12T ģʽ */   \
        run = 1;                        /* ������ʱ�� */ }
int auto_1t_12t_mode(float time)
{
    int result;
    // �ȳ��� 1T ģʽ
    result = calculate_clkdiv_time_value(1, time);
    // ��� 1T ģʽ�޷����㣬���� 12T ģʽ
    if (result == 2)
        result = calculate_clkdiv_time_value(12, time);
    return result;
}
void timer_value_setting(timer_num num, float time, int ct) // ��λ�루s��
{
    char _1tx12t = 0; // Ĭ��1T
		if(ct==0)//��ʱģʽ�ż���1T/12T
		{
			if (time <= 0)return; // 0�����쳣����
			_1tx12t = auto_1t_12t_mode(time);
			if (_1tx12t == 2)return; // 1T 12T ģʽ����ʧ��
		}else{ clock_div = 0; timer_value = 0;}//����ģʽ�������Ƶ�������س�ֵ
		switch (num)
		{
		case Timer0:INIT_TIMER(TM0,TH0,TL0);TIMER_TxTR(0,TR0);T0_CT = ct;break; // �����ж�
		case Timer1:INIT_TIMER(TM1,TH1,TL1);TIMER_TxTR(1,TR1);T1_CT = ct;break; // �����ж�
		case Timer2:INIT_TIMER(TM2,T2H,T2L);TIMER_TxTR(2,T2R);T2_CT = ct;break; // �����ж�
		case Timer3:INIT_TIMER(TM3,T3H,T3L);TIMER_TxTR(3,T3R);T3_CT = ct;break; // �����ж�
		case Timer4:INIT_TIMER(TM4,T4H,T4L);TIMER_TxTR(4,T4R);T4_CT = ct;break; // �����ж�
		case Timer11:INIT_TIMER(T11,T11H,T11L);SET_REGISTER_BIT(T11CR, _1tx12t == 0 ? 0 : 1, 4); // ��1T/12T
				SET_REGISTER_BIT(T11CR, 1, 7);SET_REGISTER_BIT(T11CR, ct, 6);break; //������ʱ��11.
		default:break;
		}
}

void timer_setting(timer_num num, int outclk, int int_en)
{
    switch (num)
    {
        case Timer0:case Timer1: // ��ʱ��0��1
        SET_OUTCLK_MODE(INTCLKO, num, outclk);
        SET_INT_MODE(IE, num * 2 + 1, int_en);break;
        case Timer2: // ��ʱ��2
        SET_OUTCLK_MODE(INTCLKO, num, outclk);
        SET_INT_MODE(IE2, 2, int_en);break;
        case Timer3:case Timer4: // ��ʱ��3��4
        SET_OUTCLK_MODE(T4T3M, (num - Timer3) * 4, outclk);
        SET_INT_MODE(IE2, num + 2, int_en);break;
        case Timer11: // ��ʱ��11
        SET_OUTCLK_MODE(T11CR, 5, outclk);
        SET_INT_MODE(T11CR, 1, int_en);break;
        default:break;
    }
}

// �������ܣ����ö�ʱ��ģʽ��֧�ֲ������������Ĭ��ֵ���ù���
// Ĭ��ֵΪ1s��ʱ�����жϣ��ر�ʱ��������ܡ���set_timer_mode(Timer0, Timer_End);
// ��Ч�ڣ�set_timer_mode(Timer0, "1s", Dis_OutClk, En_Int, Timer_End);
// ����������ӵĹ���������Timer0Ϊ1000hz�Ķ�ʱ����
// set_timer_mode(Timer0, "1000hz", Timer_End);
// ����������ӵĹ���������Timer0Ϊ10ms�Ķ�ʱ����
// set_timer_mode(Timer0, "10ms", Timer_End);
void set_timer_mode(timer_num num, ...)
{
    char *arg;
    char enable_interrupt = 0, enable_outclk = 0, _char = 0; // �ж�ʹ�ܺ����ʱ��ʹ��
		int ct_mode = 0;              							             // ��ʱ��/������ģʽ�л�
    int _sw_dat = 0;                                         // ����ֵ����
    float _set_timer_value = 0, set_timer_value = 0;         // ����Ͷ�ʱ������ֵ
    va_list args;            // �ɱ�����б�
    va_start(args, num);     // ��ʼ���ɱ�����б�
    get_main_fosc();         // ��ȡ��ǰʱ����Ƶ
    enable_interrupt = 1;enable_outclk = 0;set_timer_value = 1.0f;// Ĭ��Ϊ���жϣ��ر�ʱ�����,��ʱʱ��Ĭ��Ϊ1�� 
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;//�����ڱ�ֵ������
        enable_interrupt = sscanf(arg, "\x01int%d", &_sw_dat) == 1 ? _sw_dat : enable_interrupt;//�����Ƿ��ж�ʹ��
        enable_outclk = sscanf(arg, "\x01outclk%d", &_sw_dat) == 1 ? _sw_dat : enable_outclk;//�����Ƿ�ʱ�����ʹ��
				ct_mode = sscanf(arg, "ct%d", &_sw_dat) == 1 ? _sw_dat : ct_mode;//�����Ƕ�ʱ���Ǽ���ģʽ
        if(sscanf(arg, "%f%c", &_set_timer_value, &_char) == 2){// ��λ����
						if(_char == 'u')set_timer_value = _set_timer_value * 1e-6f;// ΢��
            if(_char == 'm')set_timer_value = _set_timer_value * 1e-3f;// ����
            if(_char == 's')set_timer_value = _set_timer_value;// ��
            if(_char == 'h')set_timer_value = 1.0f/_set_timer_value;}// ����ת��ʱ�䵥λ
    }
    timer_setting(num, enable_outclk, enable_interrupt);// ���ö�ʱ��ģʽ
    timer_value_setting(num, set_timer_value, ct_mode); // ���ö�ʱ��ֵ
    va_end(args); // ����ɱ�����б�
}

//���ڻ�ȡ��ʱ�����ж�״̬��״̬���л�����ƣ���ȡ����ա�
char get_timer_state(timer_num num)
{
    char state = 0; // �ж�״̬
    if (timer_flag == 0)return 0;      // û���ж�,��ǰ����
    state = ((timer_flag >> num) & 1); // ��ȡ�ж�״̬
    if(state == 0)return 0;
    timer_flag &= ~(1 << num);         // ����жϱ�־����
    return state;                      // �����ж�״̬
}


unsigned int get_timer_cnt(timer_num num)
{
	unsigned int time_tmp = 0;//��ʱֵ����
	char TRx = 0;
	switch(num)
	{
		case Timer0:TRx = TR0;TR0 = 0;time_tmp = ((unsigned int)TH0<<8|(unsigned int)TL0);
		TH0 = TL0 = 0x00;TR0 = TRx;break;
		case Timer1:TRx = TR1;TR1 = 0;time_tmp = ((unsigned int)TH1<<8|(unsigned int)TL1);
		TH1 = TL1 = 0x00;TR1 = TRx;break;
		case Timer2:TRx = T2R;T2R = 0;time_tmp = ((unsigned int)T2H<<8|(unsigned int)T2L);
		T2H = T2L = 0x00;T2R = TRx;break;
		case Timer3:TRx = T3R;T3R = 0;time_tmp = ((unsigned int)T3H<<8|(unsigned int)T3L);
		T3H = T3L = 0x00;T3R = TRx;break;
		case Timer4:TRx = T4R;T4R = 0;time_tmp = ((unsigned int)T4H<<8|(unsigned int)T4L);
		T4H = T4L = 0x00;T4R = TRx;break;
		case Timer11:TRx = (T11CR>>7)&1;T11CR &= ~(1<<7);time_tmp = ((unsigned int)T11H<<8|(unsigned int)T11L);
		T11H = T11L = 0x00;T11CR |= (TRx<<7);break;
	}
	return time_tmp;
}

//�������ú���ָ�뵽��Ӧ�ĺ����У�ֱ�Ӵ����Ӧ�ĺ������ּ���
//����ʾ����ǰ�涨����һ��void isr(void){P0 = ~P0};
//set_timer_isr(Timer0, isr);//����isr����ΪTimer0���жϺ���
void set_timer_isr(timer_num num, void (*isr)(void)) // �����жϷ������
{
    timer_isr[num] = isr;
}

#define tmr_isr(n)                                      \
    void timer##n##_isr(void) interrupt TMR##n##_VECTOR \
    {                                                   \
        timer_flag |= (1 << Timer##n);                  \
				if(timer_isr[Timer##n] != 0)timer_isr[Timer##n]();            \
    }
tmr_isr(0) tmr_isr(1) tmr_isr(2) tmr_isr(3) tmr_isr(4) tmr_isr(11) // �����жϷ������