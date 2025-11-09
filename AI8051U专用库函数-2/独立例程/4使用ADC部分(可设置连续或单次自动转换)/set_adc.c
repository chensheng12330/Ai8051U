#include "set_adc.h"
#include "stdarg.h"//引入可变参数头文件

unsigned int adc_value[16] = {0};//ADC值保存数组，可以直接传入枚举值查询
unsigned char adc_ch_loop[16] = {0};//ADC通道循环队列，里面设置着ADC剩余的转换次数
char adc_this_ch = Adc0_P10; //用于指示当前正在转换的ADC通道
unsigned int adc_state = 0;
#define SET_ADC_CH(ch) {ADC_CONTR &= 0xf0;ADC_CONTR |= (ch);}
void set_adc_mode(adc_mode mode, ...)
{
    va_list args;         // 可变参数列表
    va_start(args, mode); // 初始化可变参数列表
    while (1)
    {
        adc_ch ch = va_arg(args, adc_ch);
        if(ch == Adc_End)break;// 结束循环
        if(mode == cycl_mode)
        {
            adc_ch_loop[ch] = Adc_End;
        }else{
            adc_ch_loop[ch] = (adc_ch_loop[ch]==Adc_End)?1:adc_ch_loop[ch]+1;
            //单次模式下触发次数可以叠加
        }
        SET_ADC_CH(ch);
				adc_this_ch = ch;
    }
    ADC_POWER = 1;//打开ADC的电源
    ADCCFG = 0x20+ADC_DIV;//设置ADC时钟分频，默认为8(18分频，计算为(n+1)*2分频)，设置右对齐转换结果
    ADCTIM = 0x3f;//设置最长的采样时间，保证ADC采样窗口足够大，保证ADC采样保持电容的充电
    EADC = 1;//打开ADC中断
    ADC_START = 1;//开始转换
    va_end(args); // 清理可变参数列表   
}

char get_adc_state(adc_ch ch)
{
    char state = 0;
    if(adc_state == 0)return 0;// 没有转换完成
    state = (adc_state>>ch)&1;
    if(state == 0)return 0;
    adc_state &= ~(1<<ch);// 清除标志位
    return state;
}   

void adc_isr(void) interrupt 5
{
    unsigned char cnt;
    ADC_FLAG = 0;// 清除中断标志位
    adc_value[adc_this_ch] = ((unsigned int)ADC_RES<<8)|ADC_RESL;//保存ADC值
    adc_state |= (1<<(adc_this_ch));// 设置标志位
    for (cnt = 0; cnt < 16; cnt++)//十六次没有就跳出
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
    if(cnt == 16)ADC_START = 0;// 如果没有就关闭ADC
    else ADC_START = 1;// 如果还有就继续转换
}