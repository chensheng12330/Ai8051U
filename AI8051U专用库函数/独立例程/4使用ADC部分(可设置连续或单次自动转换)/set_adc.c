#include "set_adc.h"
#include "stdarg.h"//����ɱ����ͷ�ļ�

unsigned int adc_value[16] = {0};//ADCֵ�������飬����ֱ�Ӵ���ö��ֵ��ѯ
unsigned char adc_ch_loop[16] = {0};//ADCͨ��ѭ�����У�����������ADCʣ���ת������
char adc_this_ch = Adc0_P10; //����ָʾ��ǰ����ת����ADCͨ��
unsigned int adc_state = 0;
#define SET_ADC_CH(ch) {ADC_CONTR &= 0xf0;ADC_CONTR |= (ch);}
void set_adc_mode(adc_mode mode, ...)
{
    va_list args;         // �ɱ�����б�
    va_start(args, mode); // ��ʼ���ɱ�����б�
    while (1)
    {
        adc_ch ch = va_arg(args, adc_ch);
        if(ch == Adc_End)break;// ����ѭ��
        if(mode == cycl_mode)
        {
            adc_ch_loop[ch] = Adc_End;
        }else{
            adc_ch_loop[ch] = (adc_ch_loop[ch]==Adc_End)?1:adc_ch_loop[ch]+1;
            //����ģʽ�´����������Ե���
        }
        SET_ADC_CH(ch);
				adc_this_ch = ch;
    }
    ADC_POWER = 1;//��ADC�ĵ�Դ
    ADCCFG = 0x20+ADC_DIV;//����ADCʱ�ӷ�Ƶ��Ĭ��Ϊ8(18��Ƶ������Ϊ(n+1)*2��Ƶ)�������Ҷ���ת�����
    ADCTIM = 0x3f;//������Ĳ���ʱ�䣬��֤ADC���������㹻�󣬱�֤ADC�������ֵ��ݵĳ��
    EADC = 1;//��ADC�ж�
    ADC_START = 1;//��ʼת��
    va_end(args); // ����ɱ�����б�   
}

char get_adc_state(adc_ch ch)
{
    char state = 0;
    if(adc_state == 0)return 0;// û��ת�����
    state = (adc_state>>ch)&1;
    if(state == 0)return 0;
    adc_state &= ~(1<<ch);// �����־λ
    return state;
}   

void adc_isr(void) interrupt 5
{
    unsigned char cnt;
    ADC_FLAG = 0;// ����жϱ�־λ
    adc_value[adc_this_ch] = ((unsigned int)ADC_RES<<8)|ADC_RESL;//����ADCֵ
    adc_state |= (1<<(adc_this_ch));// ���ñ�־λ
    for (cnt = 0; cnt < 16; cnt++)//ʮ����û�о�����
    {
				adc_this_ch++;
				if(adc_this_ch>15)adc_this_ch = 0;
        if(adc_ch_loop[adc_this_ch] != 0)
        {
            if(adc_ch_loop[adc_this_ch] != Adc_End)
						adc_ch_loop[adc_this_ch]--;
            SET_ADC_CH(adc_this_ch);
            break;
        }
    }
    if(cnt == 16)ADC_START = 0;// ���û�о͹ر�ADC
    else ADC_START = 1;// ������оͼ���ת��
}