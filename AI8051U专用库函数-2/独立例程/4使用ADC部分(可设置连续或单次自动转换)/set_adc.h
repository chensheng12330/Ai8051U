#ifndef __SET_ADC_H__
#define __SET_ADC_H__

#include "AI8051U.h"
extern unsigned int adc_value[16];
#define ADC_DIV 8 // ADCʱ�ӷ�Ƶ��Ĭ��Ϊ8(18��Ƶ������Ϊ(n+1)*2��Ƶ)���������ù�ע
//ADC_DIV�ķ�Χ��0~15

// ADCͨ����ö�٣�����ָ����Ӧ��ADCͨ��
typedef enum
{
    Adc0_P10 = 0,Adc1_P11,Adc2_P12,Adc3_P13,Adc4_P14,Adc5_P15,Adc6_P16,Adc7_P17,
    Adc8_P00,Adc9_P01,Adc10_P02,Adc11_P03,Adc12_P04,Adc13_P05,Adc14_P06,
    Adc15_1_19V,//�ڲ�1.19v�ο�Դ����ͨ��
    Adc_End = 0xff//ͨ���Ž������
} adc_ch;

// ADCģʽѡ����ʱ֧�ֽ��ٵ�ģʽ����������չ����������
typedef enum
{
    single_mode = 0,//���β���ģʽ������һ�κ�ֹͣ����
    cycl_mode//ѭ������ģʽ������һ�κ��������
} adc_mode;

void set_adc_mode(adc_mode mode, ...);//����ADC����ģʽ,���洫����Ҫ���õ�ADC����
//ѭ��ģʽ�£��趨������adcͨ������ֹͣ�������ж��м���������
//��ʱ�����Ҫֹͣ��������Ҫ����set_adc_mode(single_mode,ch)
//���ٲ���һ�κ󼴿�ֹͣ�����������β���һ�κ�Ҳ���Զ�ֹͣ�ġ�
//�����β����£�if(get_adc_state(adc_ch))ֻ�ᱻִ��һ�Ρ�

char get_adc_state(adc_ch ch);//��ȡ��ӦADCͨ���Ƿ������ɣ�0Ϊδ��ɣ�1Ϊ���
//��⵽������ɺ󣬿���ʹ��adc_value[adc_ch]����ȡ��ӦADCͨ����⵽����ֵ��
//����ʱ����ȻҲ���Է���adc_value[adc_ch]�����ǵõ���ֵ����һ�β�����ֵ��
//�����û�д򿪵�ͨ����ֱ��ʹ��adc_value[adc_ch]�õ���ֵ����֤��ȷ��



#endif