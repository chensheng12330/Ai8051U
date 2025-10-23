#include "set_i2c.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // Ĭ��Ϊ40Mhz,��Ƶ��
char _set_i2c_fosc = 0;                //Ĭ����û�и��ĵģ�������Ƶ���Ϊ1���������Զ���ȡ����
unsigned long xdata i2c_cmd_list[Max_I2c_Task][Max_I2c_Cmd] = {0};
float i2c_speed = 0, _i2c_speed = 0;//i2cͨѶ��������
char static _char = 0;//�ַ�����
char i2c_enable = 0;//i2c����ʹ�ܱ�־
int _i2c_sw, i2c_sw;//i2c�����л���־
bit i2c_read_flag = 0;//����־�����ڵȴ���һ�εĶ�ȡ

#define State_Idle 0//����
#define State_Finish 1//���
#define State_Running 2//��������
int i2c_cmd_p = 0, i2c_task_p = 0;//�ж��ڵ�ǰִ�е������������
char _i2c_state = State_Idle;//��ǰ�ж���ִ�������״̬

void set_i2c_fosc(long fosc)
{
    _main_fosc = fosc;// ���ô��ڲ���ʹ����Ƶ
    _set_i2c_fosc = 1;//�������ڲ��ֵ�Ƶ�����ù���
}

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // ��ѯһ�ε�ǰ��ʱ��Ƶ�ʣ�ֻ�ܲ�ѯʹ���ڲ�IRC��ֵ��δ��ѯ���Ͳ��ı�_main_fosc��ֵ
{   //�˺�������ʱռ��T4��T11��ʱ�����м��㣬ʹ����ɺ������ͷ�
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //��ʱ�����ֻ���
    long _xM_Value = 0;//��׼ʱ�Ӻʹ���ʱ���µ�ֵ 
    if(_set_i2c_fosc == 1)return;//����Ѿ����ù������˳�
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; 
    _T4T3M = T4T3M; _IE2 = IE2; _IE = IE; _EA = EA; EA = 0;
    T4IF = 1; _T4H = T4H; _T4L = T4L;
    T11CR |= 0x01; _T11H = T11H; _T11L = T11L;//�������в���
    T4IF = 0; T11CR &= ~0x01;//����жϱ�־λ
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
    IE2 = _IE2; IE = _IE; EA = _EA;//�ָ��жϲ�������
    _set_i2c_fosc = 1;//��ȡһ�κ����ظ���ȡ
}

void set_i2c_mode(i2c_name i2c, ...)
{
    char *arg;
    int MSSPEED = 0;//�ٶ�����
		va_list args;         // �ɱ�����б�
    va_start(args, i2c); // ��ʼ���ɱ�����б�
		i2c_speed = 400e3;i2c_enable = 1;i2c_sw = 0;//Ĭ��ʹ��i2c����
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;        // �����ڱ�������
        i2c_enable = sscanf(arg, "\x01enabl%c", &_char) == 1 ? 1 : i2c_enable;//ʹ��i2c����
        i2c_enable = sscanf(arg, "\x01disabl%c", &_char) == 1 ? 0 : i2c_enable;//�ر�i2c����
        i2c_sw = sscanf(arg, "i2c%d", &_i2c_sw) == 1 ? _i2c_sw : i2c_sw;//��ȡi2c�����л���ʽ
        i2c_speed = sscanf(arg, "%fkh%c", &_i2c_speed, &_char) == 2 ? (_i2c_speed*1e3) : i2c_speed;//��ȡi2cͨѶ����
        i2c_speed = sscanf(arg, "%fmh%c", &_i2c_speed, &_char) == 2 ? (_i2c_speed*1e6) : i2c_speed;//��ȡi2cͨѶ����
    }
    get_main_fosc();
		MSSPEED = (_main_fosc - 8 * i2c_speed) / (4 * i2c_speed);
    MSSPEED = MSSPEED >= 0 ? MSSPEED : 0; // ��Խ������������ٶ�����
    I2CCFG = 0;I2CCFG |= (MSSPEED&0x3f)|(0x40);//�������λ,��ֵ����λ,����Ϊ����ģʽ
    I2CCFG |= i2c_enable ? 0x80 : 0;//ʹ��i2c����
    I2CPSCR = (MSSPEED >> 6)&0xff;//�����߰�λ
    I2CMSCR |= 0x80;//��������ģʽ���ж�
    I2C_S0 = (i2c_sw&1);I2C_S1 = ((i2c_sw>>1)&1);//���������л���ʽ
    memset(i2c_cmd_list,0,Max_I2c_Cmd*Max_I2c_Task);//������в���
    va_end(args); // ����ɱ�����б�
}

void run_i2c_cmd(void)
{
    switch(i2c_cmd_list[i2c_task_p][i2c_cmd_p])
    {
        case Start:case Rack:case Stop://���治�����ݵ�
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p++]/10);
        break;
        case Tack:case Tnak:
        if(i2c_cmd_list[i2c_task_p][i2c_cmd_p]==Tack)I2CMSST &= ~(1<<1);//��ACK(0)
        else I2CMSST |= (1<<1);//��NAK(1)
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p++]/10);
        break;
        case Tx_Dat:case S_Tx_Rack:case Tx_Rack://�����һ��char���ݵ�
        I2CTXD = (unsigned char)i2c_cmd_list[i2c_task_p][i2c_cmd_p+1];
				I2CMSCR &= ~0x7f;
        I2CMSCR |= (unsigned char)(i2c_cmd_list[i2c_task_p][i2c_cmd_p]/10);
        i2c_cmd_p+=2;//���ӵ���һ��ָ��
        break;
        case Rx_Dat:case Rx_Tack:case Rx_Tnak://�����һ��char���͵�ַ��
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
    int cnt = 1;          //���������
    va_list args;         // �ɱ�����б�
    va_start(args, i2c); // ��ʼ���ɱ�����б�
    if (task_num < Max_I2c_Task||i2c_cmd_list[task_num][0] != State_Idle)//���������������ߵ�ǰ����״̬�ǿ���������
    {
        i2c_cmd_list[task_num][0] = State_Running;//����
				while (1)
				{
						str = va_arg(args, int);
						if (str == Cmd_End){i2c_cmd_list[task_num][cnt++] = Cmd_End;break;}//�����ڱ�ֵ������ѭ��
						if (cnt>(Max_I2c_Cmd-3)){i2c_cmd_list[task_num][0] = State_Idle;break;}// ������󳤶ȣ���ȡ������װ��
						switch (str)
						{
						case Start:case Rack:case Tack:case Tnak:case Stop://���治�����ݵ�
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;break;
						case Tx_Dat:case S_Tx_Rack:case Tx_Rack://�����һ��char���ݵ�
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;
						in_dat = va_arg(args, int);
						i2c_cmd_list[task_num][cnt++] = (unsigned long)in_dat;break;
						case Rx_Dat:case Rx_Tack:case Rx_Tnak://�����һ��char���͵�ַ��
						i2c_cmd_list[task_num][cnt++] = (unsigned long)str;
						arg = va_arg(args, char *);
						i2c_cmd_list[task_num][cnt++] = (unsigned long)arg;break;
						default:i2c_cmd_list[task_num][0] = State_Idle;break;
						//�����Ƿ����ݣ���ȡ������װ��
						}
				}
				//װ����ɣ����ݵ�ǰ����״̬��ѡ���Ƿ�����һ������
				if(_i2c_state == State_Idle)
				{
						i2c_task_p = task_num;
						i2c_cmd_p = 1;
						_i2c_state = State_Running;
						run_i2c_cmd();
				}
    }
    va_end(args); // ����ɱ�����б�
}

char get_i2c_state(i2c_name i2c, int task_num)
{
    char state = 0;
    if(task_num>Max_I2c_Task)return 0;//����������������򷵻�
    if(i2c == I2c0)
    {
        state = i2c_cmd_list[task_num][0]; // ��ȡ�ж�״̬,������Ϊ2���������Ϊ1������Ϊ0
        if(state != State_Finish)return 0;
        i2c_cmd_list[task_num][0] = State_Idle;         // ����жϱ�־����
    }
    return state;                     // �����ж�״̬
}

void i2c_isr(void) interrupt I2C_VECTOR
{
    int cnt = 0;
    I2CMSST &= ~(1<<6);//����жϱ�־λ
    if(i2c_read_flag)
    {
        i2c_read_flag = 0;
        *((char *)i2c_cmd_list[i2c_task_p][i2c_cmd_p+1]) = I2CRXD;//����i2c�ĵ�ַ
        i2c_cmd_p+=2;//���ӵ���һ��ָ��
    }
    if(i2c_cmd_list[i2c_task_p][i2c_cmd_p]==Cmd_End)
		{
				_i2c_state = State_Finish;//���
				i2c_cmd_list[i2c_task_p][0] = State_Finish;
		}
    else run_i2c_cmd();
    if(_i2c_state == State_Finish)
    {
        for(cnt = 0;cnt<Max_I2c_Task;cnt++)
        {//��ͷ��ʼ����������δִ�е������ִ��
            if(i2c_cmd_list[cnt][0]==State_Running)
            {
                i2c_task_p = cnt;
                i2c_cmd_p = 1;
                _i2c_state = State_Running;
                run_i2c_cmd();
                break;
            }
        }
        if(cnt==Max_I2c_Task)_i2c_state = State_Idle;//������ɣ�����Ϊ����
    }
}