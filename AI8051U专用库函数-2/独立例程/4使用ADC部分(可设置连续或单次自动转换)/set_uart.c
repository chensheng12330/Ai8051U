#include "set_uart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // 默认为40Mhz,主频率
char _set_uart_fosc = 0;                //默认是没有更改的，设置主频后变为1，以屏蔽自动获取部分
char uart_flag = 0;              // 用于存储串口数据超时中断标志位
char xdata _uart1_tx_buff[Uart1_Tx] = {0};
char xdata _uart2_tx_buff[Uart2_Tx] = {0};
char xdata _uart3_tx_buff[Uart3_Tx] = {0};
char xdata _uart4_tx_buff[Uart4_Tx] = {0};
char xdata _uart1_rx_buff[Uart1_Rx] = {0};
char xdata _uart2_rx_buff[Uart2_Rx] = {0};
char xdata _uart3_rx_buff[Uart3_Rx] = {0};
char xdata _uart4_rx_buff[Uart4_Rx] = {0};
char tx_state[4] = {0,0,0,0};
int rx_cnt[4] = {0,0,0,0};

void set_uart_fosc(long fosc)
{
    _main_fosc = fosc;// 设置串口部分使用主频
    _set_uart_fosc = 1;//标明串口部分的频率设置过了
}

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // 查询一次当前的时钟频率，只能查询使用内部IRC的值，未查询到就不改变_main_fosc的值
{   //此函数会临时占用T4和T11定时器进行计算，使用完成后会进行释放
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //定时器部分缓存
    long _xM_Value = 0;//标准时钟和待测时钟下的值 
    if(_set_uart_fosc == 1)return;//如果已经设置过，则退出
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
    _set_uart_fosc = 1;//获取一次后不再重复获取
}

//通用宏
#define SET_REGISTER_BIT(reg, dat, num){          \
        reg &= ~((dat) << (num)); /* 清除目标位 */ \
        reg |= ((dat) << (num));  /* 设置目标位 */ }
#define SET_UART_CONFIG(ren, uart_num){\
    UR##uart_num##TOCR = 0xc0;UR##uart_num##TOSR = 0x80; \
    UR##uart_num##TOTL = (unsigned char)(idle_byte); \
    UR##uart_num##TOTH = (unsigned char)(idle_byte >> 8); \
    UR##uart_num##TOTE = (unsigned char)(idle_byte >> 16);ren##REN = 1;}
#define SET_UART_VALUE(th,tl){\
    th = (unsigned char)((65536 - (unsigned int)(_main_fosc / (float)(baud * 4))) >> 8); \
    tl = (unsigned char)(65536 - (unsigned int)(_main_fosc / (float)(baud * 4))); }
#define UART_DMA_CONFIG(n){ \
    DMA_UR##n##T_CFG = 0x80;        /* 打开DMA发送中断 */ \
    DMA_UR##n##T_STA = 0x00;        /* 清空状态标志位 */ \
    DMA_UR##n##T_TXAH = (unsigned char)((unsigned int)&_uart##n##_tx_buff >> 8); \
    DMA_UR##n##T_TXAL = (unsigned char)((unsigned int)&_uart##n##_tx_buff); \
    DMA_UR##n##T_CR = 0xc0;         /* bit7 1:使能 UARTn_DMA, bit6 1:开始 UARTn_DMA 自动发送 */ \
    DMA_UR##n##R_CFG = 0x00;        /* 关闭DMA接收中断 */ \
    DMA_UR##n##R_STA = 0x00;        /* 清空状态标志位 */ \
    DMA_UR##n##R_AMT = (unsigned char)(Uart##n##_Rx-2);        /*设置传输总字节数(低8位)：n+1*/\
    DMA_UR##n##R_AMTH = (unsigned char)((Uart##n##_Rx-2)>>8);       /*设置传输总字节数(高8位)：n+1*/\
    DMA_UR##n##R_RXAH = (unsigned char)((unsigned int)&_uart##n##_rx_buff >> 8); \
    DMA_UR##n##R_RXAL = (unsigned char)((unsigned int)&_uart##n##_rx_buff); \
    DMA_UR##n##R_CR = 0xa1;}         /* bit7 1:使能 UARTn_DMA, bit5 1:开始 UARTn_DMA 自动接收, bit0 1:清除 FIFO */
// 私有变量定义
long baud = 115200L, idle_byte = 64;              // 默认波特率和超时中断字节数
long _baud = 0, _idle_byte = 0;                   // 临时变量,用于存放识别数据
char _char = 0, _user_timer2 = 0, use_timer2 = 1; // 用于存放识别缓存
int _sw_dat = 0, sw_dat = 0;                      // 临时变量
int _dat_len = 0; dat_len = 0, even_odd = 0;                  // 用于存放串口数据长度,8位/9位模式

// 串口设置，波特率设置,超时中断字节数设置,串口IO组别设置,波特率使用哪个定时器设置（1为使用timer2）
void uart_setting(uart_name uart)
{
    char sw_num = sw_dat%10;//取出实际需要控制的数字
    get_main_fosc();
    switch (uart)             // 串口选择
    {
        case Uart1:
            SM1 = 1,S1_S0 = sw_num&1,S1_S1 = (sw_num>>1)&1;// 串口IO组别选择
            S1BRT = use_timer2, SM0 = dat_len&1;
            SET_REGISTER_BIT(USARTCR2,dat_len&1, 2);
            SET_REGISTER_BIT(USARTCR2,even_odd&1, 1);
            if(use_timer2){SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {SET_UART_VALUE(TH1,TL1);T1x12 = 1;TR1 = 1;};// 选择定时器2或定时器1
            UART_DMA_CONFIG(1);SET_UART_CONFIG( ,1);
            break;
        case Uart2:
            SET_REGISTER_BIT(S2CFG,1,0);//设置w1位
            S2SM1 = 1,S2_S = sw_num&1;// 串口IO组别选择
            S2SM0 = dat_len&1;
            SET_REGISTER_BIT(USART2CR2,dat_len&1, 2);
            SET_REGISTER_BIT(USART2CR2,even_odd&1, 1);
            SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;
            UART_DMA_CONFIG(2);SET_UART_CONFIG(S2,2);
            break;
        case Uart3:
            S3SM0 = dat_len&1,S3_S = sw_num&1;// 串口IO组别选择
            S3ST3 = use_timer2;
            if(use_timer2){S3ST3 = 0;SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {S3ST3 = 1;SET_UART_VALUE(T3H,T3L);T3x12 = 1;T3R = 1;};// 选择定时器2或定时器1
            UART_DMA_CONFIG(3);SET_UART_CONFIG(S3,3);
            break;
        case Uart4:
            S4SM0 = dat_len&1,S4_S = sw_num&1;// 串口IO组别选择
            S4ST4 = use_timer2;
            if(use_timer2){S4ST4 = 0;SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {S4ST4 = 1;SET_UART_VALUE(T4H,T4L);T4x12 = 1;T4R = 1;};// 选择定时器2或定时器1
            UART_DMA_CONFIG(4);SET_UART_CONFIG(S4,4);
            break;
        default:break;
    }
}

void set_uart_mode(uart_name uart, ...)
{
    char *arg;
    va_list args;         // 可变参数列表
    va_start(args, uart); // 初始化可变参数列表
    baud = 115200L;idle_byte = 64;use_timer2 = 1;// 默认波特率，默认超时中断字节数，默认使用定时器2
    sw_dat = 0;dat_len = 0;even_odd = 0;// 默认第一个切换引脚参数，默认8位数据长度，默认偶校验位        
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;                                             // 遇到哨兵，结束
        sw_dat = sscanf(arg, "uart%d", &_sw_dat) == 1 ? _sw_dat : sw_dat;                       // 解析串口IO组别
        baud = sscanf(arg, "%ldbp%c", &_baud, &_char) == 2 ? _baud : baud;                      // 解析串口波特率
        idle_byte = sscanf(arg, "%ldbyt%c", &_idle_byte, &_char) == 2 ? _idle_byte : idle_byte; // 解析串口超时中断字节数
        use_timer2 = sscanf(arg, "\x04time%c", &_char) == 1 ? 0 : use_timer2;                   // 波特率使用哪个定时器
        dat_len = sscanf(arg, "\x05%dlen%d", &_dat_len, &even_odd) == 2 ? _dat_len : dat_len;
    }
    uart_setting(uart);
    va_end(args); // 清理可变参数列表
}

//串口处理不同模式的函数
#define UART_TX_SWITCH(n){ \
    switch (arg[0]) { \
        case '\x01': \
            hex_dat = va_arg(args, char); \
            _uart##n##_tx_buff[0] = hex_dat; \
            DMA_UR##n##T_AMTH = 0, DMA_UR##n##T_AMT = 0; \
            DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* 挂起发送忙标志位 */ \
            break; /* hex */ \
        case '\x02': \
            arg = va_arg(args, char *); \
            dat_len = va_arg(args, int) - 1; \
            if (dat_len > Uart##n##_Tx) break; /* 数据长度超出缓冲区 */ \
            DMA_UR##n##T_AMT = (unsigned char)dat_len; \
            DMA_UR##n##T_AMTH = (unsigned char)(dat_len >> 8); \
            for (; dat_len >= 0; dat_len--) _uart##n##_tx_buff[dat_len] = arg[dat_len]; \
            DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* 挂起发送忙标志位 */ \
            break; /* buff */ \
        default: \
            dat_len = vsprintf(_uart##n##_tx_buff, arg, args) - 1; \
            if (dat_len > Uart##n##_Tx) break; /* 数据长度超出缓冲区 */ \
            else { \
                DMA_UR##n##T_AMT = (unsigned char)dat_len; \
                DMA_UR##n##T_AMTH = (unsigned char)(dat_len >> 8); \
                DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* 挂起发送忙标志位 */ \
            } \
            break; /* 正常 printf */ }}

void uart_printf(uart_name uart, ...)
{
    char *arg;
    char hex_dat = 0;
    int dat_len = 0;
    va_list args; // 可变参数列表
    va_start(args, uart);  // 初始化可变参数列表
    arg = va_arg(args, char *);
    if(tx_state[uart]==0)
    switch (uart)
    {
    case Uart1:UART_TX_SWITCH(1);break;
    case Uart2:UART_TX_SWITCH(2);break;
    case Uart3:UART_TX_SWITCH(3);break;
    case Uart4:UART_TX_SWITCH(4);break;
    default:break;
    }
    va_end(args); // 清理可变参数列表
}

char get_uart_state(uart_name num)
{
    char state = 0;
    if (uart_flag == 0)return 0;      // 没有中断,提前返回
    state = ((uart_flag >> num) & 1); // 获取中断状态
    if(state == 0)return 0;
    uart_flag &= ~(1 << num);         // 清除中断标志缓存
    return state;                     // 返回中断状态
}

//中断函数部分
#define uart_isr(n) void uart##n##_isr(void) interrupt UART##n##_VECTOR { \
    if(UR##n##TOSR & 0x01) \
    { \
        DMA_UR##n##R_STA = 0x00; \
        uart_flag |= (1 << Uart##n); \
        UR##n##TOSR = 0x80; \
        rx_cnt[Uart##n] = (((unsigned int)DMA_UR##n##R_DONEH << 8) + DMA_UR##n##R_DONE); \
        if(rx_cnt[Uart##n] == 0)rx_cnt[Uart##n] = Uart##n##_Rx-2;\
        _uart##n##_rx_buff[rx_cnt[Uart##n]] = '\0'; \
        DMA_UR##n##R_CR = 0; \
        DMA_UR##n##R_CR = 0xa1;}}\
void uart##n##_dma_isr(void) interrupt DMA_UR##n##T_VECTOR { \
            DMA_UR##n##T_STA = 0x00; \
            tx_state[Uart##n] = 0;}
uart_isr(1);uart_isr(2);uart_isr(3);uart_isr(4);