#include "set_pwm.h"
#include "stdarg.h"
#include "stdio.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // Ĭ��Ϊ40Mhz,��Ƶ��
char _set_pwm_fosc = 0;                //Ĭ����û�и��ĵģ�������Ƶ���Ϊ1���������Զ���ȡ����
unsigned int pwma_max = 0, pwmb_max = 0;

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // ��ѯһ�ε�ǰ��ʱ��Ƶ�ʣ�ֻ�ܲ�ѯʹ���ڲ�IRC��ֵ��δ��ѯ���Ͳ��ı�_main_fosc��ֵ
{   //�˺�������ʱռ��T4��T11��ʱ�����м��㣬ʹ����ɺ������ͷ�
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L; //��ʱ�����ֻ���
    long _xM_Value = 0;//��׼ʱ�Ӻʹ���ʱ���µ�ֵ 
    if(_set_pwm_fosc == 1)return;//����Ѿ����ù������˳�
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; _T4T3M = T4T3M;
    _IE2 = IE2; _IE = IE; EA = 0; _T4H = T4H; _T4L = T4L;
    _T11H = T11H; _T11L = T11L;//�������в���
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
    IE2 = _IE2; IE = _IE;//�ָ��жϲ�������
    _set_pwm_fosc = 1;//��ȡһ�κ����ظ���ȡ
}

char static _char, period_flag = 0;//Ĭ��û�и��Ĺ�
float static period, _set_duty, set_duty;
int static _sw, sw, _outpn, outpn, _dtr, dtr;

unsigned char calculate_dead_time_register(int tclk) {
    if (tclk == 0) return 0;
    if (tclk <= 31) {return tclk;} // DTGn[7:5] = 000, 001, 010, 011
    else if (tclk <= 127) {return 32 + ((tclk - 64) / 2);} // DTGn[7:5] = 100, 101
    else if (tclk <= 255) {return 96 + ((tclk - 32) / 8);}// DTGn[7:5] = 110
    else if (tclk <= 1000) {return 160 + ((tclk - 32) / 16);} // DTGn[7:5] = 111
    else {return 255;} // Maximum possible value
}

unsigned long pscr = 0, arr = 0;
void calculate_pwm_pscr_arr(float target_pwm_frequency) {
    float max_arr_value;
    if (target_pwm_frequency == 0)return;// ���������ܵ�arrֵ��ʹ��(arr + 1) * target_pwm_frequency <= fosc
    max_arr_value = ((float)_main_fosc / target_pwm_frequency) - 1;// ���max_arr_value��16λ��Χ�ڣ���ֱ��ʹ�ø�ֵ��Ϊarr��������pscr
    if (max_arr_value <= 65535) {
        arr = (unsigned int)max_arr_value;// �����Ӧ��pscrֵ
        pscr = (unsigned int)(((float)_main_fosc / ((float)(arr + 1) * target_pwm_frequency)) - 1);}
    else {// ���max_arr_value����16λ��Χ����arr����Ϊ���16λֵ��65535��
        arr = 65535;// �����Ӧ��pscrֵ
        pscr = (unsigned int)(((float)_main_fosc / ((float)(arr + 1) * target_pwm_frequency)) - 1);// ȷ��pscrҲ��16λ��Χ��
        if (pscr > 65535)return;
    }
}
void pwm_setting(pwm_name pwm)
{
    unsigned int offset;
    calculate_pwm_pscr_arr(period);
    if (pwm < Pwm1 || pwm > Pwm8) return;// ���pwm�����Ƿ���Ч
    if(pwm<4)
    {
        (*(unsigned char volatile far *)(0x7efecc+(pwm/2))) &= ~(pwm%2?0xf0:0x0f);
        (*(unsigned char volatile far *)(0x7efec8+pwm)) = 0x68;
        (*(unsigned char volatile far *)(0x7efecc+(pwm/2))) |= (pwm%2?0x50:0x05);
        PWMA_PS &= ~(3<<(pwm*2));PWMA_PS |= (sw%10)<<(pwm*2);
        PWMA_DTR = calculate_dead_time_register(dtr);
        PWMA_PSCRH = (pscr >> 8)&0xff;
        PWMA_PSCRL = (pscr)&0xff;
        PWMA_ARRH = (arr >> 8)&0xff;
        PWMA_ARRL = (arr)&0xff;
        pwma_max = arr;
        offset = (set_duty/100.0)*(float)pwma_max;
        (*(unsigned char volatile far *)(0x7efed5+(pwm*2))) = (offset>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efed6+(pwm*2))) = offset&0xff;
        PWMA_ENO &= ~(3<<(pwm*2));PWMA_ENO |= ((outpn)<<(pwm*2));
        PWMA_BKR = 0x80;  //ʹ�������
	    PWMA_CR1 = 0x81; //ʹ��ARRԤװ��,��ʼ��ʱ
    }
    else 
    {
        (*(unsigned char volatile far *)(0x7efeec+((pwm-4)/2))) &= ~((pwm-4)%2?0xf0:0x0f);
        (*(unsigned char volatile far *)(0x7efee8+(pwm-4))) = 0x68;
        (*(unsigned char volatile far *)(0x7efeec+((pwm-4)/2))) |= ((pwm-4)%2?0x50:0x05);
        PWMB_PS &= ~(3<<((pwm-4)*2));PWMB_PS |= (sw%10)<<((pwm-4)*2);
        PWMB_DTR = calculate_dead_time_register(dtr);
        PWMB_PSCRH = (pscr >> 8)&0xff;
        PWMB_PSCRL = (pscr)&0xff;
        PWMB_ARRH = (arr >> 8)&0xff;
        PWMB_ARRL = (arr)&0xff;
        pwmb_max = arr;
        offset = (set_duty/100.0)*(float)pwmb_max;
        (*(unsigned char volatile far *)(0x7efef5+((pwm-4)*2))) = (offset>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efef6+((pwm-4)*2))) = offset&0xff;
        PWMB_ENO &= ~(3<<((pwm-4)*2));PWMB_ENO |= ((outpn)<<((pwm-4)*2));
        PWMB_BKR = 0x80;  //ʹ�������
	    PWMB_CR1 = 0x81; //ʹ��ARRԤװ��,��ʼ��ʱ
    }
}

void set_pwm_mode(pwm_name pwm, ...)
{
    char *arg;
    va_list args;         // �ɱ�����б�
    va_start(args, pwm); // ��ʼ���ɱ�����б�
    get_main_fosc();//��ȡ��ǰ��ʱ��Ƶ��
    set_duty = 50; sw = 0; outpn = 1;dtr = 10;
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;// �����ڱ�������
        sw = sscanf(arg, "pwm%d", &_sw) == 1 ? _sw : sw;// ����PWMͨ���л�����
        if(sscanf(arg, "%f%c", &_set_duty, &_char) == 2)
        {
            if(_char == '%')set_duty = _set_duty;//����ռ�ձ�
            if(_char == 'h'){period = _set_duty;period_flag = 1;}//�������ڣ�hz��λ
            if(_char == 'k'){period = _set_duty*1e3;period_flag = 1;}//�������ڣ�khz��λ
        }
        dtr = sscanf(arg, "%dcl%c", &_dtr, &_char) == 2 ? _dtr : dtr;//��������ʱ��
        outpn = sscanf(arg, "out%d", &_outpn) == 1 ? _outpn : outpn;//�����������
    }
    if(period_flag == 0)period = 1000;
    pwm_setting(pwm);//����pwm���ֵĲ���
    va_end(args); // ����ɱ�����б�
}

void set_pwm_duty(pwm_name pwm, float duty)
{
    unsigned int _duty = 0;
    if(duty > 100 || duty < 0)return;//�쳣ֵ����
    if (pwm >= Pwm1 && pwm <= Pwm4)
    {
        _duty = (unsigned int)((duty / 100.0) * pwma_max);
        (*(unsigned char volatile far *)(0x7efed5+(pwm*2))) = (_duty>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efed6+(pwm*2))) = _duty&0xff;
    } // �����Ӧ��ʵ��ռ�ձ�
    if (pwm >= Pwm5 && pwm <= Pwm8)
    {
        _duty = (unsigned int)((duty / 100.0) * pwmb_max);
        (*(unsigned char volatile far *)(0x7efef5+((pwm-4)*2))) = (_duty>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efef6+((pwm-4)*2))) = _duty&0xff;
    }
}