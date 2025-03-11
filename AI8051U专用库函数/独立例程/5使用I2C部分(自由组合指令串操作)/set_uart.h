#ifndef __SET_UART_H__
#define __SET_UART_H__

#include "AI8051U.H"

// UARTö�٣�����ָ����Ӧ�Ĵ���
typedef enum
{
    Uart1 = 0,
    Uart2,
    Uart3,
    Uart4
} uart_name;

// ���ڻ�����������󳤶�ö��ֵ
typedef enum
{
    Uart1_Tx = 120, // ����1���ͻ�����������󳤶�
    Uart2_Tx = 60,  // ����2���ͻ�����������󳤶�
    Uart3_Tx = 60,  // ����3���ͻ�����������󳤶�
    Uart4_Tx = 60,  // ����4���ͻ�����������󳤶�
    Uart1_Rx = 120, // ����1���ջ�����������󳤶�
    Uart2_Rx = 60,  // ����2���ջ�����������󳤶�
    Uart3_Rx = 60,  // ����3���ջ�����������󳤶�
    Uart4_Rx = 60   // ����4���ջ�����������󳤶�
} uart_len;

// �궨�岿��
#define Uart_End "end"       // ������־
#define Uart1_P30_1 "uart10" // �����л��궨��,ǰ�����RXD���������TXD
#define Uart1_P36_7 "uart11"
#define Uart1_P16_7 "uart12"
#define Uart1_P43_4 "uart13"
#define Uart2_P12_3 "uart20"
#define Uart2_P42_3 "uart21"
#define Uart3_P00_1 "uart30"
#define Uart3_P50_1 "uart31"
#define Uart4_P02_3 "uart40"
#define Uart4_P52_3 "uart41"

#define Hex_Mode "\x01 hex"   // ���ֽ�ģʽ��ʹ���˷���ʾ��ascii�ַ���������ײ
#define Buff_Mode "\x02 buff" // ������ģʽ

#define Use_Timer2 "\x03timer" // ʹ�ö�ʱ��2��Ϊ���ڲ����ʷ�������Ĭ��ѡ��ʱ��2
#define Use_Timerx "\x04timer" // ʹ�ô��ڶ�Ӧ�Ķ�ʱ����Ϊ�����ʣ����紮��1ʹ�ö�ʱ��1
//�����x�ǹ̶���������������дΪUse_Timer1�����ӡ��ڴ���ʹ�ò�ͬ�����ʵ�ʱ�򣬾���Ҫÿ�����ڶ�ʹ���Լ���Ӧ�Ķ�ʱ����
//���û�����ã�Ĭ�϶�Ϊ��ʱ��2�����������ʻᱻ�������õĸ����ǵ���

#define Base_8b "\x050len0" // 8λ����λ,��У��λ��Ĭ��ģʽ
#define Odd_9b "\x051len1"  // 9λ����λ����У��λ����Ҫע����ǣ��Զ���żУ��ֻ�д���1/2ӵ��
#define Even_9b "\x051len0" // 9λ����λ��żУ��λ������3/4����żУ����Ҫ��������S3TB8��S4TB8

// ���Կ��ļ����õĻ�������
extern char xdata _uart1_rx_buff[Uart1_Rx];
extern char xdata _uart2_rx_buff[Uart2_Rx];
extern char xdata _uart3_rx_buff[Uart3_Rx];
extern char xdata _uart4_rx_buff[Uart4_Rx];
// ���Կ��ļ����õĽ������ݳ��ȣ�����get_uart_state()��ȡ����״̬ʱ�����ͬʱʹ��
extern int rx_cnt[4];
// ��������ڲ�ѯ���ڷ��͵�æ��־��tx_state[Uart1]Ϊ1���ʶ����æ��Ϊ0���ʶ���Ϳ���
extern char tx_state[4];

// ���ô���ģʽ��Ĭ������Ϊ115200�����ʣ�8λ����λ��1λֹͣλ.
// �䳤�������ɱ����Ϊ�����ʡ���ʱ�ж�����λ�������л����ţ������Ҫʹ��Uart_End������
// �ٸ����ӣ�set_uart_mode(Uart1, "9600bps", Uart1_P36_7, Uart_End);
// �������˼�����ô���1Ϊ9600�����ʣ�8λ����λ��1λֹͣλ�����л�����ΪP36��P37�ϣ���ʱ�ж�Ϊ64byte
// ��ʱ�жϵ������Ƕ������Զ��ְ���64byte����û����64���ֽڣ����ݲ����ʣ����ͻ��Զ��жϣ�Ȼ��������ݷְ���
// �پٸ����ӣ�set_uart_mode(Uart1, "32byte", "115200bps", Uart_End);
// �������˼�����ô���1Ϊ115200�����ʣ����л�����ΪP30��P31�ϣ�Ĭ�����ţ�����ʱ�ж�Ϊ32byte��
// �䳤��������֧���������룬�����ʺͳ�ʱ�ж���Ҫ���ϵ�λbps��byte���м䲻Ҫ�пո�
// ���������䳤���������磺set_uart_mode(Uart1, Uart_End);//�����115200�����ʣ�64byte��P30P31��UART1�£�
// ���������ѡ��ӵ��Ĭ��ֵ��������Ҳ���Եġ�
void set_uart_mode(uart_name uart, ...);
// Ĭ��ֵΪ115200�����ʣ���ʱ�ж�64byte�������л�ÿ�����ڵĵ�һ�����μ�����ĺ궨�壩
// uart1��P30��P31��uart2��P12��P13��uart3��P00��P01��uart4��P02��P03

char get_uart_state(uart_name uart);
//  ��ȡ����״̬������ֵΪ0������û�н��յ����ݣ�1�����ڽ��յ������ݣ��������ݴ����˳�ʱ�жϡ�

//  ���Զ�ָ�����ڷ������ݣ���Ϊ����ģʽ����ͨ�ַ�����ӡģʽ��hex���ݵ��ֽ����ģʽ��buff���������ģʽ��
//  ��ͨ�ַ�����ӡģʽ��uart_printf(Uart1, "num:%d\r\n",cnt);//����������printfһ��ʹ��
//  hex���ݵ��ֽ����ģʽ��uart_printf(Uart1, Hex_Mode, 0x12);//���0x12һ���ֽڵ�����
//  buff���������ģʽ��uart_printf(Uart1, Buff_Mode, dat, 20);//���20���ֽڵ����ݣ���dat����0��ַ��ʼ��
void uart_printf(uart_name uart, ...);

// �������ô��ڵ�ʱ��Ƶ�ʣ��������ϵͳʱ��Ƶ�ʣ���λ��Hz
// һ����˵���ùܣ����õ�ʱ����Զ�����
void set_uart_fosc(long fosc);

#endif