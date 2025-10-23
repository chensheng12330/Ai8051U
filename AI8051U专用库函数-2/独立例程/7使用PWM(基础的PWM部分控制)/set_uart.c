#include "set_uart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "math.h"

unsigned long static _main_fosc = 40e6; // Ĭ��Ϊ40Mhz,��Ƶ��
char _set_uart_fosc = 0;                //Ĭ����û�и��ĵģ�������Ƶ���Ϊ1���������Զ���ȡ����
char uart_flag = 0;              // ���ڴ洢�������ݳ�ʱ�жϱ�־λ
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
    _main_fosc = fosc;// ���ô��ڲ���ʹ����Ƶ
    _set_uart_fosc = 1;//�������ڲ��ֵ�Ƶ�����ù���
}

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // ��ѯһ�ε�ǰ��ʱ��Ƶ�ʣ�ֻ�ܲ�ѯʹ���ڲ�IRC��ֵ��δ��ѯ���Ͳ��ı�_main_fosc��ֵ
{   //�˺�������ʱռ��T4��T11��ʱ�����м��㣬ʹ����ɺ������ͷ�
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L, _EA; //��ʱ�����ֻ���
    long _xM_Value = 0;//��׼ʱ�Ӻʹ���ʱ���µ�ֵ 
    if(_set_uart_fosc == 1)return;//����Ѿ����ù������˳�
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
    _set_uart_fosc = 1;//��ȡһ�κ����ظ���ȡ
}

//ͨ�ú�
#define SET_REGISTER_BIT(reg, dat, num){          \
        reg &= ~((dat) << (num)); /* ���Ŀ��λ */ \
        reg |= ((dat) << (num));  /* ����Ŀ��λ */ }
#define SET_UART_CONFIG(ren, uart_num){\
    UR##uart_num##TOCR = 0xc0;UR##uart_num##TOSR = 0x80; \
    UR##uart_num##TOTL = (unsigned char)(idle_byte); \
    UR##uart_num##TOTH = (unsigned char)(idle_byte >> 8); \
    UR##uart_num##TOTE = (unsigned char)(idle_byte >> 16);ren##REN = 1;}
#define SET_UART_VALUE(th,tl){\
    th = (unsigned char)((65536 - (unsigned int)(_main_fosc / (float)(baud * 4))) >> 8); \
    tl = (unsigned char)(65536 - (unsigned int)(_main_fosc / (float)(baud * 4))); }
#define UART_DMA_CONFIG(n){ \
    DMA_UR##n##T_CFG = 0x80;        /* ��DMA�����ж� */ \
    DMA_UR##n##T_STA = 0x00;        /* ���״̬��־λ */ \
    DMA_UR##n##T_TXAH = (unsigned char)((unsigned int)&_uart##n##_tx_buff >> 8); \
    DMA_UR##n##T_TXAL = (unsigned char)((unsigned int)&_uart##n##_tx_buff); \
    DMA_UR##n##T_CR = 0xc0;         /* bit7 1:ʹ�� UARTn_DMA, bit6 1:��ʼ UARTn_DMA �Զ����� */ \
    DMA_UR##n##R_CFG = 0x00;        /* �ر�DMA�����ж� */ \
    DMA_UR##n##R_STA = 0x00;        /* ���״̬��־λ */ \
    DMA_UR##n##R_AMT = (unsigned char)(Uart##n##_Rx-2);        /*���ô������ֽ���(��8λ)��n+1*/\
    DMA_UR##n##R_AMTH = (unsigned char)((Uart##n##_Rx-2)>>8);       /*���ô������ֽ���(��8λ)��n+1*/\
    DMA_UR##n##R_RXAH = (unsigned char)((unsigned int)&_uart##n##_rx_buff >> 8); \
    DMA_UR##n##R_RXAL = (unsigned char)((unsigned int)&_uart##n##_rx_buff); \
    DMA_UR##n##R_CR = 0xa1;}         /* bit7 1:ʹ�� UARTn_DMA, bit5 1:��ʼ UARTn_DMA �Զ�����, bit0 1:��� FIFO */
// ˽�б�������
long baud = 115200L, idle_byte = 64;              // Ĭ�ϲ����ʺͳ�ʱ�ж��ֽ���
long _baud = 0, _idle_byte = 0;                   // ��ʱ����,���ڴ��ʶ������
char _char = 0, _user_timer2 = 0, use_timer2 = 1; // ���ڴ��ʶ�𻺴�
int _sw_dat = 0, sw_dat = 0;                      // ��ʱ����
int _dat_len = 0; dat_len = 0, even_odd = 0;                  // ���ڴ�Ŵ������ݳ���,8λ/9λģʽ

// �������ã�����������,��ʱ�ж��ֽ�������,����IO�������,������ʹ���ĸ���ʱ�����ã�1Ϊʹ��timer2��
void uart_setting(uart_name uart)
{
    char sw_num = sw_dat%10;//ȡ��ʵ����Ҫ���Ƶ�����
    get_main_fosc();
    switch (uart)             // ����ѡ��
    {
        case Uart1:
            SM1 = 1,S1_S0 = sw_num&1,S1_S1 = (sw_num>>1)&1;// ����IO���ѡ��
            S1BRT = use_timer2, SM0 = dat_len&1;
            SET_REGISTER_BIT(USARTCR2,dat_len&1, 2);
            SET_REGISTER_BIT(USARTCR2,even_odd&1, 1);
            if(use_timer2){SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {SET_UART_VALUE(TH1,TL1);T1x12 = 1;TR1 = 1;};// ѡ��ʱ��2��ʱ��1
            UART_DMA_CONFIG(1);SET_UART_CONFIG( ,1);
            break;
        case Uart2:
            SET_REGISTER_BIT(S2CFG,1,0);//����w1λ
            S2SM1 = 1,S2_S = sw_num&1;// ����IO���ѡ��
            S2SM0 = dat_len&1;
            SET_REGISTER_BIT(USART2CR2,dat_len&1, 2);
            SET_REGISTER_BIT(USART2CR2,even_odd&1, 1);
            SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;
            UART_DMA_CONFIG(2);SET_UART_CONFIG(S2,2);
            break;
        case Uart3:
            S3SM0 = dat_len&1,S3_S = sw_num&1;// ����IO���ѡ��
            S3ST3 = use_timer2;
            if(use_timer2){S3ST3 = 0;SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {S3ST3 = 1;SET_UART_VALUE(T3H,T3L);T3x12 = 1;T3R = 1;};// ѡ��ʱ��2��ʱ��1
            UART_DMA_CONFIG(3);SET_UART_CONFIG(S3,3);
            break;
        case Uart4:
            S4SM0 = dat_len&1,S4_S = sw_num&1;// ����IO���ѡ��
            S4ST4 = use_timer2;
            if(use_timer2){S4ST4 = 0;SET_UART_VALUE(T2H,T2L);T2x12 = 1;T2R = 1;}
            else {S4ST4 = 1;SET_UART_VALUE(T4H,T4L);T4x12 = 1;T4R = 1;};// ѡ��ʱ��2��ʱ��1
            UART_DMA_CONFIG(4);SET_UART_CONFIG(S4,4);
            break;
        default:break;
    }
}

void set_uart_mode(uart_name uart, ...)
{
    char *arg;
    va_list args;         // �ɱ�����б�
    va_start(args, uart); // ��ʼ���ɱ�����б�
    baud = 115200L;idle_byte = 64;use_timer2 = 1;// Ĭ�ϲ����ʣ�Ĭ�ϳ�ʱ�ж��ֽ�����Ĭ��ʹ�ö�ʱ��2
    sw_dat = 0;dat_len = 0;even_odd = 0;// Ĭ�ϵ�һ���л����Ų�����Ĭ��8λ���ݳ��ȣ�Ĭ��żУ��λ        
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;                                             // �����ڱ�������
        sw_dat = sscanf(arg, "uart%d", &_sw_dat) == 1 ? _sw_dat : sw_dat;                       // ��������IO���
        baud = sscanf(arg, "%ldbp%c", &_baud, &_char) == 2 ? _baud : baud;                      // �������ڲ�����
        idle_byte = sscanf(arg, "%ldbyt%c", &_idle_byte, &_char) == 2 ? _idle_byte : idle_byte; // �������ڳ�ʱ�ж��ֽ���
        use_timer2 = sscanf(arg, "\x04time%c", &_char) == 1 ? 0 : use_timer2;                   // ������ʹ���ĸ���ʱ��
        dat_len = sscanf(arg, "\x05%dlen%d", &_dat_len, &even_odd) == 2 ? _dat_len : dat_len;
    }
    uart_setting(uart);
    va_end(args); // ����ɱ�����б�
}

//���ڴ���ͬģʽ�ĺ���
#define UART_TX_SWITCH(n){ \
    switch (arg[0]) { \
        case '\x01': \
            hex_dat = va_arg(args, char); \
            _uart##n##_tx_buff[0] = hex_dat; \
            DMA_UR##n##T_AMTH = 0, DMA_UR##n##T_AMT = 0; \
            DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* ������æ��־λ */ \
            break; /* hex */ \
        case '\x02': \
            arg = va_arg(args, char *); \
            dat_len = va_arg(args, int) - 1; \
            if (dat_len > Uart##n##_Tx) break; /* ���ݳ��ȳ��������� */ \
            DMA_UR##n##T_AMT = (unsigned char)dat_len; \
            DMA_UR##n##T_AMTH = (unsigned char)(dat_len >> 8); \
            for (; dat_len >= 0; dat_len--) _uart##n##_tx_buff[dat_len] = arg[dat_len]; \
            DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* ������æ��־λ */ \
            break; /* buff */ \
        default: \
            dat_len = vsprintf(_uart##n##_tx_buff, arg, args) - 1; \
            if (dat_len > Uart##n##_Tx) break; /* ���ݳ��ȳ��������� */ \
            else { \
                DMA_UR##n##T_AMT = (unsigned char)dat_len; \
                DMA_UR##n##T_AMTH = (unsigned char)(dat_len >> 8); \
                DMA_UR##n##T_CR = 0xc0, tx_state[uart] = 1; /* ������æ��־λ */ \
            } \
            break; /* ���� printf */ }}

void uart_printf(uart_name uart, ...)
{
    char *arg;
    char hex_dat = 0;
    int dat_len = 0;
    va_list args; // �ɱ�����б�
    va_start(args, uart);  // ��ʼ���ɱ�����б�
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
    va_end(args); // ����ɱ�����б�
}

char get_uart_state(uart_name num)
{
    char state = 0;
    if (uart_flag == 0)return 0;      // û���ж�,��ǰ����
    state = ((uart_flag >> num) & 1); // ��ȡ�ж�״̬
    if(state == 0)return 0;
    uart_flag &= ~(1 << num);         // ����жϱ�־����
    return state;                     // �����ж�״̬
}

//�жϺ�������
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