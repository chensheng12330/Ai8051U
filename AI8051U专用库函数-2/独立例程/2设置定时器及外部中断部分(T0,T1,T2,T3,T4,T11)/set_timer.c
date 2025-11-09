#include "set_timer.h"
#include "math.h"
#include "stdarg.h"
#include "stdio.h"

unsigned long static _main_fosc = 40e6; // 默认为40Mhz,主频率
char _set_timer_fosc = 0;                //默认是没有更改的，设置主频后变为1，以屏蔽自动获取部分
char timer_flag = 0;             // 用于缓存定时器中断状态
void (*timer_isr[6])(void) = {0};//定义函数指针数组
// 通用宏
#define SET_REGISTER_BIT(reg, dat, num){          \
        reg &= ~((1) << (num)); /* 清除目标位 */ \
        reg |= ((dat) << (num));  /* 设置目标位 */ }
#define SET_OUTCLK_MODE(reg, num, val) SET_REGISTER_BIT(reg, val, num) // 设置输出时钟模式
#define SET_INT_MODE(reg, num, val) SET_REGISTER_BIT(reg, val, num)       // 设置中断模式

void set_timer_fosc(long fosc)
{
    _main_fosc = fosc;// 设置串口部分使用主频
    _set_timer_fosc = 1;//标明串口部分的频率设置过了
}

float static code fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // 查询一次当前的时钟频率，只能查询使用内部IRC的值，未查询到就不改变_main_fosc的值
{   //此函数会临时占用T4和T11定时器进行计算，使用完成后会进行释放
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //定时器部分缓存
    long _xM_Value = 0;//标准时钟和待测时钟下的值 
    if(_set_timer_fosc == 1)return;//如果已经设置过，则退出
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; 
    _T4T3M = T4T3M; _IE2 = IE2; _IE = IE; _EA = EA; EA = 0;
    T4IF = 1; _T4H = T4H; _T4L = T4L;
    T11CR |= 0x01; _T11H = T11H; _T11L = T11L;//缓存所有参数
    T4IF = 0; T11CR &= ~0x01;//清除中断标志位
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
    IE2 = _IE2; IE = _IE; EA = _EA;//恢复中断部分配置
    _set_timer_fosc = 1;//获取一次后不再重复获取
}

// 私有变量定义
static unsigned char clock_div = 0;
static unsigned int timer_value = 0;
// 计算 clk_div 和 time_value
int calculate_clkdiv_time_value(int mode, float time)
{
    float time_scaled = (mode == 12)?(time / 12.0f):time;//根据模式调整 time,12T 模式下，time 缩小 12 倍
    float clk_div_temp = (time_scaled * _main_fosc) / 65536.0f - 1.0f;// 计算 clk_div 的初始值
    float time_value_temp,calculated_time;
    clock_div = (unsigned char)(clk_div_temp>255.0f?255:clk_div_temp<0?0:clk_div_temp);// 确保 clk_div 在 0~255 范围内
    time_value_temp = 65536.0f - (time_scaled * _main_fosc) / (clock_div + 1);// 计算 time_value
    if (time_value_temp < 0.0f || time_value_temp > 65535.0f)// 如果 time_value 超出范围，调整 clk_div
    {
        clock_div = (unsigned char)((time_scaled * _main_fosc) / 65536.0f);// 重新计算 clk_div
        clock_div = clock_div>255?255:clock_div;// 确保 clk_div 在 0~255 范围内
        time_value_temp = 65536.0f - (time_scaled * _main_fosc) / (clock_div + 1);// 重新计算 time_value
    }// 确保 time_value 在 0~65535 范围内
    timer_value = (unsigned int)(time_value_temp>65535.0f?65535:time_value_temp<0.0f?0:time_value_temp);
    calculated_time = (65536.0f - timer_value) * (clock_div + 1) / _main_fosc;// 检查计算结果是否有效
    calculated_time *= (mode == 12)?12.0f:1; // 12T 模式下，时间扩大 12 倍
    if (fabs(calculated_time - time) < 1e-3f)// 如果计算结果与目标时间匹配，则返回当前模式
        return mode == 1 ? 1 : 0; // 1T模式返回1，12T模式返回0，无法计算返回2
    return 2;// 计算结果无效
}

// 初始化宏，包含预分频和装载值
#define INIT_TIMER(psnum,hnum,lnum){                                       \
        psnum##PS = clock_div;                      /* 时钟分频 */       \
        hnum = (unsigned char)(timer_value>>8); /* 装载定时器高位 */   \
        lnum = (unsigned char)(timer_value); /* 装载定时器低位 */}
// 12T/1T模式选择和运行启动
#define TIMER_TxTR(timer_num,run){                                    \
        T##timer_num##x12 = _1tx12t == 0 ? 0 : 1; /* 1T 12T 模式 */   \
        run = 1;                        /* 开启定时器 */ }
int auto_1t_12t_mode(float time)
{
    int result;
    // 先尝试 1T 模式
    result = calculate_clkdiv_time_value(1, time);
    // 如果 1T 模式无法计算，尝试 12T 模式
    if (result == 2)
        result = calculate_clkdiv_time_value(12, time);
    return result;
}
void timer_value_setting(timer_num num, float time, int ct) // 单位秒（s）
{
    char _1tx12t = 0; // 默认1T
		if(ct==0)//定时模式才计算1T/12T
		{
			if (time <= 0)return; // 0或负数异常返回
			_1tx12t = auto_1t_12t_mode(time);
			if (_1tx12t == 2)return; // 1T 12T 模式计算失败
		}else{ clock_div = 0; timer_value = 0;}//计数模式下清零分频器和重载初值
		switch (num)
		{
		case Timer0:INIT_TIMER(TM0,TH0,TL0);TIMER_TxTR(0,TR0);T0_CT = ct;break; // 开启中断
		case Timer1:INIT_TIMER(TM1,TH1,TL1);TIMER_TxTR(1,TR1);T1_CT = ct;break; // 开启中断
		case Timer2:INIT_TIMER(TM2,T2H,T2L);TIMER_TxTR(2,T2R);T2_CT = ct;break; // 开启中断
		case Timer3:INIT_TIMER(TM3,T3H,T3L);TIMER_TxTR(3,T3R);T3_CT = ct;break; // 开启中断
		case Timer4:INIT_TIMER(TM4,T4H,T4L);TIMER_TxTR(4,T4R);T4_CT = ct;break; // 开启中断
		case Timer11:INIT_TIMER(T11,T11H,T11L);SET_REGISTER_BIT(T11CR, _1tx12t == 0 ? 0 : 1, 4); // 打开1T/12T
				SET_REGISTER_BIT(T11CR, 1, 7);SET_REGISTER_BIT(T11CR, ct, 6);break; //启动定时器11.
		default:break;
		}
}

void timer_setting(timer_num num, int outclk, int int_en)
{
    switch (num)
    {
        case Timer0:case Timer1: // 定时器0和1
        SET_OUTCLK_MODE(INTCLKO, num, outclk);
        SET_INT_MODE(IE, num * 2 + 1, int_en);break;
        case Timer2: // 定时器2
        SET_OUTCLK_MODE(INTCLKO, num, outclk);
        SET_INT_MODE(IE2, 2, int_en);break;
        case Timer3:case Timer4: // 定时器3、4
        SET_OUTCLK_MODE(T4T3M, (num - Timer3) * 4, outclk);
        SET_INT_MODE(IE2, num + 2, int_en);break;
        case Timer11: // 定时器11
        SET_OUTCLK_MODE(T11CR, 5, outclk);
        SET_INT_MODE(T11CR, 1, int_en);break;
        default:break;
    }
}

// 函数介绍：设置定时器模式，支持参数乱序输入和默认值配置功能
// 默认值为1s定时，打开中断，关闭时钟输出功能。即set_timer_mode(Timer0, Timer_End);
// 等效于：set_timer_mode(Timer0, "1s", Dis_OutClk, En_Int, Timer_End);
// 下面这个例子的功能是设置Timer0为1000hz的定时器。
// set_timer_mode(Timer0, "1000hz", Timer_End);
// 下面这个例子的功能是设置Timer0为10ms的定时器。
// set_timer_mode(Timer0, "10ms", Timer_End);
void set_timer_mode(timer_num num, ...)
{
    char *arg;
    char enable_interrupt = 0, enable_outclk = 0, _char = 0; // 中断使能和输出时钟使能
		int ct_mode = 0;              							             // 定时器/计数器模式切换
    int _sw_dat = 0;                                         // 设置值缓存
    float _set_timer_value = 0, set_timer_value = 0;         // 缓存和定时器设置值
    va_list args;            // 可变参数列表
    va_start(args, num);     // 初始化可变参数列表
    get_main_fosc();         // 获取当前时钟主频
    enable_interrupt = 1;enable_outclk = 0;set_timer_value = 1.0f;// 默认为打开中断，关闭时钟输出,定时时间默认为1秒 
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;//遇到哨兵值，结束
        enable_interrupt = sscanf(arg, "\x01int%d", &_sw_dat) == 1 ? _sw_dat : enable_interrupt;//设置是否中断使能
        enable_outclk = sscanf(arg, "\x01outclk%d", &_sw_dat) == 1 ? _sw_dat : enable_outclk;//设置是否时钟输出使能
				ct_mode = sscanf(arg, "ct%d", &_sw_dat) == 1 ? _sw_dat : ct_mode;//设置是定时还是计数模式
        if(sscanf(arg, "%f%c", &_set_timer_value, &_char) == 2){// 单位处理
						if(_char == 'u')set_timer_value = _set_timer_value * 1e-6f;// 微秒
            if(_char == 'm')set_timer_value = _set_timer_value * 1e-3f;// 毫秒
            if(_char == 's')set_timer_value = _set_timer_value;// 秒
            if(_char == 'h')set_timer_value = 1.0f/_set_timer_value;}// 赫兹转换时间单位
    }
    timer_setting(num, enable_outclk, enable_interrupt);// 设置定时器模式
    timer_value_setting(num, set_timer_value, ct_mode); // 设置定时器值
    va_end(args); // 清理可变参数列表
}

//用于获取定时器的中断状态，状态具有缓存机制，读取后清空。
char get_timer_state(timer_num num)
{
    char state = 0; // 中断状态
    if (timer_flag == 0)return 0;      // 没有中断,提前返回
    state = ((timer_flag >> num) & 1); // 获取中断状态
    if(state == 0)return 0;
    timer_flag &= ~(1 << num);         // 清除中断标志缓存
    return state;                      // 返回中断状态
}


unsigned int get_timer_cnt(timer_num num)
{
	unsigned int time_tmp = 0;//定时值缓存
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

//用于设置函数指针到对应的函数中，直接传入对应的函数名字即可
//调用示例：前面定义了一个void isr(void){P0 = ~P0};
//set_timer_isr(Timer0, isr);//设置isr函数为Timer0的中断函数
void set_timer_isr(timer_num num, void (*isr)(void)) // 设置中断服务程序
{
    timer_isr[num] = isr;
}

#define tmr_isr(n)                                      \
    void timer##n##_isr(void) interrupt TMR##n##_VECTOR \
    {                                                   \
        timer_flag |= (1 << Timer##n);                  \
				if(timer_isr[Timer##n] != 0)timer_isr[Timer##n]();            \
    }
tmr_isr(0) tmr_isr(1) tmr_isr(2) tmr_isr(3) tmr_isr(4) tmr_isr(11) // 定义中断服务程序