#ifndef __SET_ADC_H__
#define __SET_ADC_H__

#include "AI8051U.h"
extern unsigned int adc_value[16];
#define ADC_DIV 8 // ADC时钟分频，默认为8(18分频，计算为(n+1)*2分频)，基本不用关注
//ADC_DIV的范围是0~15

// ADC通道号枚举，用于指定相应的ADC通道
typedef enum
{
    Adc0_P10 = 0,Adc1_P11,Adc2_P12,Adc3_P13,Adc4_P14,Adc5_P15,Adc6_P16,Adc7_P17,
    Adc8_P00,Adc9_P01,Adc10_P02,Adc11_P03,Adc12_P04,Adc13_P05,Adc14_P06,
    Adc15_1_19V,//内部1.19v参考源采样通道
    Adc_End = 0xff//通道号结束标记
} adc_ch;

// ADC模式选择，暂时支持较少的模式，后续可扩展更灵活的设置
typedef enum
{
    single_mode = 0,//单次采样模式，采样一次后停止采样
    cycl_mode//循环采样模式，采样一次后继续采样
} adc_mode;

void set_adc_mode(adc_mode mode, ...);//设置ADC采样模式,后面传入需要设置的ADC引脚
//循环模式下，设定采样的adc通道不会停止，会在中断中继续采样。
//此时如果想要停止采样，需要调用set_adc_mode(single_mode,ch)
//则再采样一次后即可停止采样。而单次采样一次后也是自动停止的。
//即单次采样下，if(get_adc_state(adc_ch))只会被执行一次。

char get_adc_state(adc_ch ch);//获取对应ADC通道是否采样完成，0为未完成，1为完成
//检测到采样完成后，可以使用adc_value[adc_ch]来获取对应ADC通道检测到的数值。
//其他时候虽然也可以访问adc_value[adc_ch]，但是得到的值是上一次采样的值。
//如果是没有打开的通道，直接使用adc_value[adc_ch]得到的值不保证正确。



#endif