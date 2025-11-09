#include "set_i2c.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // 默认为40Mhz,主频率
char _set_i2c_fosc = 0;                //默认是没有更改的，设置主频后变为1，以屏蔽自动获取部分
unsigned long xdata i2c_cmd_list[Max_I2c_Task][Max_I2c_Cmd] = {0};
float i2c_speed = 0, _i2c_speed = 0;//i2c通讯速率设置
char static _char = 0;//字符缓存
char i2c_enable = 0;//i2c外设使能标志
int _i2c_sw, i2c_sw;//i2c引脚切换标志
bit i2c_read_flag = 0;//读标志，用于等待下一次的读取

#define State_Idle 0//空闲
#define State_Finish 1//完成
#define State_Running 2//正在运行
int i2c_cmd_p = 0, i2c_task_p = 0;//中断内当前执行到的命令和任务
char _i2c_state = State_Idle;//当前中断内执行任务的状态

void set_i2c_fosc(long fosc)
{
    _main_fosc = fosc;// 设置串口部分使用主频
    _set_i2c_fosc = 1;//标明串口部分的频率设置过了
}

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // 查询一次当前的时钟频率，只能查询使用内部IRC的值，未查询到就不改变_main_fosc的值
{   //此函数会临时占用T4和T11定时器进行计算，使用完成后会进行释放
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //定时器部分缓存
    long _xM_Value = 0;//标准时钟和待测时钟下的值 
    if(_set_i2c_fosc == 1)return;//如果已经设置过，则退出
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
    _set_i2c_fosc = 1;//获取一次后不再重复获取
}

void set_i2c_mode(i2c_name i2c, ...)
{
    char *arg;
    int MSSPEED = 0;//速度设置
		va_list args;         // 可变参数列表
    va_start(args, i2c); // 初始化可变参数列表
		i2c_speed = 400e3;i2c_enable = 1;i2c_sw = 0;//默认使能i2c外设
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;        // 遇到哨兵，结束
        i2c_enable = sscanf(arg, "\x01enabl%c", &_char) == 1 ? 1 : i2c_enable;//使能i2c功能
        i2c_enable = sscanf(arg, "\x01disabl%c", &_char) == 1 ? 0 : i2c_enable;//关闭i2c功能
        i2c_sw = sscanf(arg, "i2c%d", &_i2c_sw) == 1 ? _i2c_sw : i2c_sw;//获取i2c引脚切换方式
        i2c_speed = sscanf(arg, "%fkh%c", &_i2c_speed, &_char) == 2 ? (_i2c_speed*1e3) : i2c_speed;//获取i2c通讯速率
        i2c_speed = sscanf(arg, "%fmh%c", &_i2c_speed, &_char) == 2 ? (_i2c_speed*1e6) : i2c_speed;//获取i2c通讯速率
    }
    get_main_fosc();
		MSSPEED = (_main_fosc - 8 * i2c_speed) / (4 * i2c_speed);
    MSSPEED = MSSPEED >= 0 ? MSSPEED : 0; // 超越限制则以最快速度运行
    I2CCFG = 0;I2CCFG |= (MSSPEED&0x3f)|(0x40);//清除低六位,赋值低六位,设置为主机模式
    I2CCFG |= i2c_enable ? 0x80 : 0;//使能i2c功能
    I2CPSCR = (MSSPEED >> 6)&0xff;//保留高八位
    I2CMSCR |= 0x80;//允许主机模式的中断
    I2C_S0 = (i2c_sw&1);I2C_S1 = ((i2c_sw>>1)&1);//设置引脚切换方式
    memset(i2c_cmd_list,0,Max_I2c_Cmd*Max_I2c_Task);//清空所有参数
    va_end(args); // 清理可变参数列表
}

void run_i2c_cmd(void)
{
    switch(i2c_cmd_list[i2c_task_p][i2c_cmd_p])
    {
        case Start:case Rack:case Stop://后面不带数据的
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p++]/10);
        break;
        case Tack:case Tnak:
        if(i2c_cmd_list[i2c_task_p][i2c_cmd_p]==Tack)I2CMSST &= ~(1<<1);//发ACK(0)
        else I2CMSST |= (1<<1);//发NAK(1)
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p++]/10);
        break;
        case Tx_Dat:case S_Tx_Rack:case Tx_Rack://后面带一个char数据的
        I2CTXD = (unsigned char)i2c_cmd_list[i2c_task_p][i2c_cmd_p+1];
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p]/10);
        i2c_cmd_p+=2;//增加到下一个指令
        break;
        case Rx_Dat:case Rx_Tack:case Rx_Tnak://后面带一个char类型地址的
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p]/10);
        i2c_read_flag = 1;
        break;
        default:break;
    }
}

void set_i2c_cmd(i2c_name i2c, int task_num, ...)
{
    unsigned char str;
		int in_dat;
		char* arg;
    int cnt = 1;          //命令计数器
    va_list args;         // 可变参数列表
    va_start(args, i2c); // 初始化可变参数列表
    if (task_num < Max_I2c_Task||i2c_cmd_list[task_num][0] != State_Idle)//任务数量超过或者当前任务状态非空闲则跳过
    {
        i2c_cmd_list[task_num][0] = State_Running;//标明
				while (1)
				{
						str = va_arg(args, int);
						if (str == Cmd_End){i2c_cmd_list[task_num][cnt++] = Cmd_End;break;}//遇到哨兵值，结束循环
						if (cnt>(Max_I2c_Cmd-3)){i2c_cmd_list[task_num][0] = State_Idle;break;}// 超出最大长度，则取消本次装载
						switch (str)
						{
						case Start:case Rack:case Tack:case Tnak:case Stop://后面不带数据的
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;break;
						case Tx_Dat:case S_Tx_Rack:case Tx_Rack://后面带一个char数据的
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;
						in_dat = va_arg(args, int);
						i2c_cmd_list[task_num][cnt++] = (unsigned long)in_dat;break;
						case Rx_Dat:case Rx_Tack:case Rx_Tnak://后面带一个char类型地址的
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;
						arg = va_arg(args, char *);
						i2c_cmd_list[task_num][cnt++] = (unsigned long)arg;break;
						default:i2c_cmd_list[task_num][0] = State_Idle;break;
						//遇到非法数据，则取消本次装载
						}
				}
				//装载完成，根据当前运行状态来选择是否拉起一次运行
				if(_i2c_state == State_Idle)
				{
						i2c_task_p = task_num;
						i2c_cmd_p = 1;
						_i2c_state = State_Running;
						run_i2c_cmd();
				}
    }
    va_end(args); // 清理可变参数列表
}

char get_i2c_state(i2c_name i2c, int task_num)
{
    char state = 0;
    if(task_num>Max_I2c_Task)return 0;//超出最大任务数，则返回
    if(i2c == I2c0)
    {
        state = i2c_cmd_list[task_num][0]; // 获取中断状态,运行中为2，运行完成为1，空闲为0
        if(state != State_Finish)return 0;
        i2c_cmd_list[task_num][0] = State_Idle;         // 清除中断标志缓存
    }
    return state;                     // 返回中断状态
}

void i2c_isr(void) interrupt I2C_VECTOR
{
    int cnt = 0;
    I2CMSST &= ~(1<<6);//清空中断标志位
    if(i2c_read_flag)
    {
        i2c_read_flag = 0;
        *((char *)i2c_cmd_list[i2c_task_p][i2c_cmd_p+1]) = I2CRXD;//返回i2c的地址
        i2c_cmd_p+=2;//增加到下一个指令
    }
    if(i2c_cmd_list[i2c_task_p][i2c_cmd_p]==Cmd_End)
		{
				_i2c_state = State_Finish;//完成
				i2c_cmd_list[i2c_task_p][0] = State_Finish;
		}
    else run_i2c_cmd();
    if(_i2c_state == State_Finish)
    {
        for(cnt = 0;cnt<Max_I2c_Task;cnt++)
        {//从头开始搜索，遇到未执行的则进行执行
            if(i2c_cmd_list[cnt][0]==State_Running)
            {
                i2c_task_p = cnt;
                i2c_cmd_p = 1;
                _i2c_state = State_Running;
                run_i2c_cmd();
                break;
            }
        }
        if(cnt==Max_I2c_Task)_i2c_state = State_Idle;//搜索完成，则置为空闲
    }
}