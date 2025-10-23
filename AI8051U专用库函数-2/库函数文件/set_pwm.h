#ifndef __SET_PWM_H__
#define __SET_PWM_H__

#include "AI8051U.H"

// PWM �豸ö��
typedef enum
{
    Pwm1,Pwm2,Pwm3,Pwm4, //PWMA�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
    Pwm5,Pwm6,Pwm7,Pwm8  //PWMB�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
} pwm_name;

typedef enum
{
    Pwm1_Pwm2,Pwm3_Pwm4, //PWMA�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
    Pwm5_Pwm6,Pwm7_Pwm8  //PWMB�飬���õ����ڱ���һ�£����ö����ͬ�����ڰ������һ����Ч
} pwm_inx;

typedef struct
{
    char sync_pwm;//ͬ�������־λ
    unsigned int cycl;//����õ�������
    unsigned int dat;//��������������
}cap_dat;

// �궨�岿��
#define Pwm_End "end"       // ������־

#define Pwm_Out_Mode "mode0"//����pwmΪ���ģʽ��Ĭ��ֵ
#define Pwm_In_Mode "mode1"//����pwmΪ����ģʽ��ͬһ����Լ����Ҳ����
//����pwm1����Ϊ�����pwm2����Ϊ���������ǿ��Ե�
//����ͬһ��pwm���ܼ�����Ϊ���Ҳ����Ϊ���룬�ᰴ�����һ����������Ч

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

/*������PWMͨ�����������ģʽ�¿����趨������*/
#define En_Out_P "out1" //ֻ��PwmxP�����Ĭ��ֵ����x��Χ1~8��
#define En_Out_N "out2" //ֻ��PwmxN�����(Pwm5~8û��PwmxN�������Ҳ����Ч��)
#define En_Out_PN "out3" //��PwmxP��PwmxN���,�������
#define Dis_Out "out0" //�ر�PwmxP��PwmxN���

#define Out_CPWM "pol0" //�����������ΪPwmxPͨ���ߵ�ƽ��PwmxNͨ���͵�ƽ��Ч������������PWM��Ĭ��ֵ
#define Out_IPWM "pol1" //����PwmxN�������Ϊ����PwmxP���ԣ���PwmxN�����PwmxPһ������ͬ��PWM�����Ǵ�������
//��������趨ͨ�������ڻ����źŵļ����趨����PWMB�飨PWM5~8��������֧�ֻ����ź�������������������ͬ���趨��Ч

#define Self_Capture "ic1" //�������벶��Ϊ�Լ���Ӧ��Pwmͨ����Ĭ��ֵ
#define Cross_Capture "ic2" //�������벶��Ϊ�Աߵ�Pwmͨ��
//PWM1~8�У�����һ�飬����PWM1��PWM2Ϊͬһ�飬PWM1����������ͨ����Ӧ��PWM1�ܽţ����ý���ͨ�����Ӧ��PWM2�ܽ�
//��PWM2����������ͨ��ΪPWM2�ܽţ����ý���ͨ�����Ӧ��PWM1�ܽţ���������������
//�����������������õ�ͬһ���ܽţ��Ϳ���ʵ��ͬʱ����һ���źŵ������غ��½��أ��Ӷ�ʵ�ֲ���ռ�ձ��ź�
/*������PWMͨ�����������ģʽ�¿����趨������*/


/*������PWMͨ������������ģʽ�¿����趨������*/
#define En_In_P "in1" //����ֻ�����PwmxP���룬PwmxN�޷���������Ϊ����ʱ��Ĭ��ֵ
#define Dis_In "in0" //�ر�Pwmx����

#define Cap_In_Rising_Edge "cap0" //�������벶��Ϊ�����ز���Ĭ��ֵ
#define Cap_In_Falling_Edge "cap1" //�������벶��Ϊ�½��ز���

/*������PWMͨ������������ģʽ�¿����趨������*/


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

extern unsigned int pwma_max, pwmb_max;//pwma���pwmb������ֵ��ARR�Ĵ���ֵ

// PWM���񷵻�ֵ��λö��
typedef enum
{
    khz = 0,//Ƶ�ʵ�λ��ǧ����
    hz,//Ƶ�ʵ�λ������
    ms,//ʱ�䵥λ������
    us//ʱ�䵥λ��΢��
} dat_unit;//�������ݵ�λ

//���㲶�����ڣ���һ�����������Pwm1~Pwm8����Ӧͨ��Ӧ����ǰ���ʼ��Ϊ����ģʽ����������쳣����᷵��-1
//�ڶ�����������ͨ������Ĳ�����ָ����λ��khz��hz������ģʽ�����ص�float���Ƕ�Ӧ��λ��ֵ
//��Ҫע����ǣ������ѯ��ʱ���ֲ�����δ��ɻ��߲����������᷵��-1��ע�⴦���쳣���
//�ڶ�������������ָ��Ϊʱ�䵥λ������ָ��Ϊms��us���ֱ��ʾ�����΢�룬���ص�float���Ƕ�Ӧ��λ��ֵ
float get_pwm_period(pwm_name pwm, dat_unit unit);


// ���㲶��ռ�ձȣ����������Pwm1_Pwm2~Pwm7_Pwm8����Ӧͨ��Ӧ����ǰ���ʼ��Ϊ����ģʽ����������쳣����᷵��-1
// ����ռ�ձ���Ҫռ���������ڵ�Pwm���裬���Զ������ռ�ձȣ����ط�Χ0~100%
// ��Ҫע����ǣ������ѯ��ʱ���ֲ�����δ��ɻ��߲����������᷵��-1��ע�⴦���쳣���
// ʾ����duty = get_pwm_duty(Pwm1_Pwm2);//����pwm1�����pwm2����ʱ��ռ�ձȣ��Զ����ݲ������ݴ�Сȷ��ռ�ձ�
float get_pwm_duty(pwm_inx pwm);


//��������PWM���ֵ�ʱ��ԴƵ�ʣ�foscΪʱ��Ƶ�ʣ���λΪHz��
//��ʹ���������������£���Ƶ���Զ���ȡ���û����Բ��ù���������⡣
void set_pwm_fosc(long fosc);
#endif