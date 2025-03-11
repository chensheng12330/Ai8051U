#include "set_pwm.h"
#include "stdarg.h"
#include "stdio.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // 默认为40Mhz,主频率
char _set_pwm_fosc = 0;                //默认是没有更改的，设置主频后变为1，以屏蔽自动获取部分
unsigned int pwma_max = 0, pwmb_max = 0;

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // 查询一次当前的时钟频率，只能查询使用内部IRC的值，未查询到就不改变_main_fosc的值
{   //此函数会临时占用T4和T11定时器进行计算，使用完成后会进行释放
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L; //定时器部分缓存
    long _xM_Value = 0;//标准时钟和待测时钟下的值 
    if(_set_pwm_fosc == 1)return;//如果已经设置过，则退出
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; _T4T3M = T4T3M;
    _IE2 = IE2; _IE = IE; EA = 0; _T4H = T4H; _T4L = T4L;
    _T11H = T11H; _T11L = T11L;//缓存所有参数
    if((_CLKSEL&0x0f)!=0)return;//当前选择的不是内部IRC时钟，则退出
    IRC48MCR = 0x80; while(!(IRC48MCR&1));//启动并等待内部48M时钟稳定
    CLKDIV = 2;//切换为2分频, 顺序不可以打乱，否则直接给48Mhz时钟会出现异常
    CLKSEL |= 0x0c;//选择内部48M时钟作为系统时钟源，此时sysclk=24Mhz
    T4T3M &= ~0xf0; ET4 = 0;//清除T4部分的寄存器到默认状态。关闭中断，防止跳转到中断部分后跑飞
    T11CR = 0x14;//切换为内部高速IRC，即待测时钟，1T模式
    T4x12 = 1;//使用1T模式
    T11CR &= ~0x01;//清除中断标志位
    T11H = T11L = 0;//从零开始计数
    T4H = 0x80; T4L = 0x00;//32768个clk后溢出
    T4R = 1;//启动T4计时器
    T11CR |= 0x80;//启动T11计时器
    while(!(T4IF));//等待T4中断来临
    T11CR &= ~0x81;//清除中断标志位，同时关闭T11
    T4R = 0;//关闭T4计时器
    _xM_Value = (unsigned long)(((unsigned int)T11H<<8)|(unsigned int)T11L);//记录值
    T4IF = 0;//清除中断标志位
    if(_CLKDIV == 0)_CLKDIV = 1;//避免除数为0
    _main_fosc = (unsigned long)((732.421875f)*(float)((float)_xM_Value/(float)_CLKDIV));
    for(_xM_Value = 0; _xM_Value < 15; _xM_Value++){
        if(fabs((float)_main_fosc - fosc_base[_xM_Value])<3e5f){//差值范围在0.3Mhz内
            _main_fosc = fosc_base[_xM_Value];}}//替换成预制频率
    T4H = _T4H; T4L = _T4L; T11H = _T11H; T11L = _T11L;
    T11CR = _T11CR; T4T3M = _T4T3M;//恢复定时器设置
    CLKSEL = _CLKSEL; CLKDIV = _CLKDIV;//恢复时钟配置
    IE2 = _IE2; IE = _IE;//恢复中断部分配置
    _set_pwm_fosc = 1;//获取一次后不再重复获取
}

char static _char, period_flag = 0;//默认没有更改过
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
    if (target_pwm_frequency == 0)return;// 计算最大可能的arr值，使得(arr + 1) * target_pwm_frequency <= fosc
    max_arr_value = ((float)_main_fosc / target_pwm_frequency) - 1;// 如果max_arr_value在16位范围内，则直接使用该值作为arr，并计算pscr
    if (max_arr_value <= 65535) {
        arr = (unsigned int)max_arr_value;// 计算对应的pscr值
        pscr = (unsigned int)(((float)_main_fosc / ((float)(arr + 1) * target_pwm_frequency)) - 1);}
    else {// 如果max_arr_value超过16位范围，则将arr设置为最大16位值（65535）
        arr = 65535;// 计算对应的pscr值
        pscr = (unsigned int)(((float)_main_fosc / ((float)(arr + 1) * target_pwm_frequency)) - 1);// 确保pscr也在16位范围内
        if (pscr > 65535)return;
    }
}
void pwm_setting(pwm_name pwm)
{
    unsigned int offset;
    calculate_pwm_pscr_arr(period);
    if (pwm < Pwm1 || pwm > Pwm8) return;// 检查pwm参数是否有效
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
        PWMA_BKR = 0x80;  //使能主输出
	    PWMA_CR1 = 0x81; //使能ARR预装载,开始计时
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
        PWMB_BKR = 0x80;  //使能主输出
	    PWMB_CR1 = 0x81; //使能ARR预装载,开始计时
    }
}

void set_pwm_mode(pwm_name pwm, ...)
{
    char *arg;
    va_list args;         // 可变参数列表
    va_start(args, pwm); // 初始化可变参数列表
    get_main_fosc();//获取当前的时钟频率
    set_duty = 50; sw = 0; outpn = 1;dtr = 10;
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;// 遇到哨兵，结束
        sw = sscanf(arg, "pwm%d", &_sw) == 1 ? _sw : sw;// 解析PWM通道切换引脚
        if(sscanf(arg, "%f%c", &_set_duty, &_char) == 2)
        {
            if(_char == '%')set_duty = _set_duty;//解析占空比
            if(_char == 'h'){period = _set_duty;period_flag = 1;}//解析周期，hz单位
            if(_char == 'k'){period = _set_duty*1e3;period_flag = 1;}//解析周期，khz单位
        }
        dtr = sscanf(arg, "%dcl%c", &_dtr, &_char) == 2 ? _dtr : dtr;//解析死区时间
        outpn = sscanf(arg, "out%d", &_outpn) == 1 ? _outpn : outpn;//解析输出配置
    }
    if(period_flag == 0)period = 1000;
    pwm_setting(pwm);//设置pwm部分的参数
    va_end(args); // 清理可变参数列表
}

void set_pwm_duty(pwm_name pwm, float duty)
{
    unsigned int _duty = 0;
    if(duty > 100 || duty < 0)return;//异常值返回
    if (pwm >= Pwm1 && pwm <= Pwm4)
    {
        _duty = (unsigned int)((duty / 100.0) * pwma_max);
        (*(unsigned char volatile far *)(0x7efed5+(pwm*2))) = (_duty>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efed6+(pwm*2))) = _duty&0xff;
    } // 计算对应的实际占空比
    if (pwm >= Pwm5 && pwm <= Pwm8)
    {
        _duty = (unsigned int)((duty / 100.0) * pwmb_max);
        (*(unsigned char volatile far *)(0x7efef5+((pwm-4)*2))) = (_duty>>8)&0xff;
        (*(unsigned char volatile far *)(0x7efef6+((pwm-4)*2))) = _duty&0xff;
    }
}