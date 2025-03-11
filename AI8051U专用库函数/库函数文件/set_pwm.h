#ifndef __SET_PWM_H__
#define __SET_PWM_H__

#include "AI8051U.H"

// PWM �豸ö��
typedef enum
{
    Pwm1,Pwm2,Pwm3,Pwm4, //PWMA�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
    Pwm5,Pwm6,Pwm7,Pwm8  //PWMB�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
} pwm_name;

// �궨�岿��
#define Pwm_End "end"       // ������־

#define Pwm_Out_Mode "mode0"//����pwmΪ���ģʽ��Ĭ��ֵ
// #define Pwm_In_Mode "mode1"//��ʱ����֧��PWM���벶��ģʽ���ȴ�����֧��

extern unsigned long pscr, arr;

//ǰ�����PWMxP���������PWMxN��ֻ��һ���ľ���PWMxP��x��Χ1~8��
#define Pwm1_P10_11 "pwm10"//PWM1������Ĭ��ֵ
#define Pwm1_P00_01 "pwm11"
#define Pwm1_P20_21 "pwm12"
#define Pwm2_P12_13 "pwm20"//PWM2������Ĭ��ֵ
#define Pwm2_P02_03 "pwm21"
#define Pwm2_P22_23 "pwm22"
#define Pwm3_P14_15 "pwm30"//PWM3������Ĭ��ֵ
#define Pwm3_P04_05 "pwm31"
#define Pwm3_P24_25 "pwm32"
#define Pwm4_P16_17 "pwm40"//PWM4������Ĭ��ֵ
#define Pwm4_P06_07 "pwm41"
#define Pwm4_P26_27 "pwm42"
#define Pwm5_P01 "pwm50"//PWM5������Ĭ��ֵ
#define Pwm5_P11 "pwm51"
#define Pwm5_P21 "pwm52"
#define Pwm5_P50 "pwm53"
#define Pwm6_P03 "pwm60"//PWM6������Ĭ��ֵ
#define Pwm6_P13 "pwm61"
#define Pwm6_P23 "pwm62"
#define Pwm6_P51 "pwm63"
#define Pwm7_P05 "pwm70"//PWM7������Ĭ��ֵ
#define Pwm7_P15 "pwm71"
#define Pwm7_P25 "pwm72"
#define Pwm7_P52 "pwm73"
#define Pwm8_P07 "pwm80"//PWM8������Ĭ��ֵ
#define Pwm8_P17 "pwm81"
#define Pwm8_P27 "pwm82"
#define Pwm8_P53 "pwm83"

#define En_Out_P "out1" //ֻ��PwmxP�����Ĭ��ֵ����x��Χ1~8��
#define En_Out_N "out2" //ֻ��PwmxN�����(Pwm5~8û��PwmxN�������Ҳ����Ч��)
#define En_Out_PN "out3" //��PwmxP��PwmxN���,�������
#define Dis_Out "out0" //�ر�PwmxP��PwmxN���

//��������Pwm1~Pwm8�����ں�ռ�ձȣ�����֧��khz��hz��λ��ռ�ձ�֧��0%~100%��ռ�ձ�֧��С����
//����ռ�ձ�Ϊ25.7%����Ҳ������ġ������ռ�ձ��ǳ�ʼ����ռ�ձȣ������������Ҫ�ı�ռ�ձ�
//��ʹ��set_pwm_duty���������򱾺������¼��㲢���������������Ϊ�ķ�ʱ�䡣
//ע�⣬Pwm1~4ΪPWMA�飬Pwm5~8ΪPWMB�飬��������һ·Pwmͨ�������ں󣬸��������ͨ���趨ʱ���Բ������ظ��������ڡ�
//��������˶�����ڣ������һ�ε����õ�����Ϊ׼������Ч��
//֧������������Ϊ0~1000clk��clkΪPwm������ʱ�ӣ����統ǰPwm����ʱ��40Mhz����ô����100clk������
//������ʱ�����1/40Mhz*100=2.5us����Ҫע����ǣ����������ڳ���127clk��㲻�Ƿǳ���׼��ʱ�����ˡ�
//��Ϊԭ�����Ĵ�����8λ�ģ��ڳ���127�Ĳ������˽׶������ţ����Ժ���ֻ�ܽ��ƵĽ�����������ʱ�䡣
//�������һ��pwmͨ�������þ�����(���ڵ�Ĭ��ֵΪ1khz��ռ�ձ�Ĭ��ֵΪ50%�� ����Ĭ��ֵΪ10clk)
//set_pwm_mode(Pwm1, Pwm_Out_Mode, Pwm1_P10_11, En_Out_P, "1khz", "50%", "10clk", Pwm_End);
//����ĳ���������Pwm1���1khz��50%ռ�ձȣ�����Ϊ10clk������ֻ��Pͨ�������Nͨ����������л�����P10_P11���
//��Ȼ�������������ó���Ҳ����ʹ��ȫĬ��ֵ�����ã�set_pwm_mode(Pwm1, Pwm_End);
//���Ը����Լ���Ҫ���õĲ������������ã��������ֿ��Ա���Ĭ��ֵ
void set_pwm_mode(pwm_name pwm, ...);

// ����ռ�ձȣ���һ��������Pwm1~Pwm8���ڶ���������ռ�ձȣ�֧��С���㣬��Χ0%~100%
// ��������24.5%��ռ�ձȣ�������ôд��set_pwm_duty(Pwm1, 24.5);
void set_pwm_duty(pwm_name pwm, float duty);

#endif