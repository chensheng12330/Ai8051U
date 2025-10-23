#ifndef __SET_IO_H__
#define __SET_IO_H__

#include "AI8051U.H"

// IOģʽö��
typedef enum
{
    pu_mode = 0,           // ׼˫���ģʽ��pull-up��д
    pp_mode,               // �������ģʽ��push-pull��д
    hz_mode,               // ��������ģʽ��high-z��д
    od_mode,               // ��©ģʽ��open-drain��д
    dis_pur = 10,          // �ر��������� disable pull-up-resistor��д
    en_pur = 11,           // ������������ enable pull-up-resistor��д
    en_schmitt_trig = 20,  // ����ʩ���ش���ģʽ enable schmitt trigger��д
    dis_schmitt_trig = 21, // �ر�ʩ���ش���ģʽ disable schmitt trigger��д
    low_speed = 31,        // �ϵ͵ĵ�ƽת���ٶȣ���Ӧ�����³�Ƚ�С
    high_speed = 30,       // �ϸߵĵ�ƽת���ٶȣ���Ӧ�����³��Ƚϴ�
    small_current = 41,    // С����������һ����������
    big_current = 40,      // �������������ǿ��������
    en_dinput = 51,        // ����������ʹ�� enable digital input��д���� I/O ���������ֿ�ʱ����������Ϊ 1���� MCU �޷���ȡ�ⲿ�˿ڵĵ�ƽ��
    dis_dinput = 50,       // �ر���������ʹ�� disable digital input��д��������ʱ��ͣ��/ʡ��ģʽǰ����������Ϊ 0��������ж���ĺĵ硣
    dis_pdr = 60,          // �ر��������� disable pull-down-resistor��д
    en_pdr = 61,           // ���������� enable pull-down-resistor��д
    dis_auto_config = 71,  // �ر��Զ���������ģʽ disable auto configuration��д������ģ������� I/O ��ģʽ���Զ����ã���Ҫ�û�ʹ�� PxM0/PxM1 �Ĵ����� I/O �������á�
    en_auto_config = 70    // ���Զ���������ģʽ enable auto configuration��д������ģ������� I/O ��ģʽ�����Զ����ã������û�ʹ�� PxM0/PxM1 ����Ӧ I/O ���е����á�
} io_mode;

// IO����ö��
typedef enum
{
    // Ϊ�˷�ֹ��P00����ͷ�ļ������ͻ������ʹ��Pin00��������
    Pin00 = 0,Pin01,Pin02,Pin03,Pin04,Pin05,Pin06,Pin07,
    Pin10,Pin11,Pin12,Pin13,Pin14,Pin15,Pin16,Pin17,
    Pin20,Pin21,Pin22,Pin23,Pin24,Pin25,Pin26,Pin27,
    Pin30,Pin31,Pin32,Pin33,Pin34,Pin35,Pin36,Pin37,
    Pin40,Pin41,Pin42,Pin43,Pin44,Pin45,Pin46,Pin47,
    Pin50,Pin51,Pin52,Pin53,Pin54,Pin55,Pin56,Pin57,Pin_End = 0xff
} io_name;

// ��������
void set_io_mode(io_mode mode, ...); // ��������IO��ģʽ
// ��ϸ����:modeΪIO��ģʽ������Ϊ�ɱ������io_name��������Ҫע����ǣ����������Ҫ���Pin_End��Ϊ������
// ����set_io_mode(pu_mode,Pin00,Pin21,Pin32,Pin_End);���ǽ�P00,P21,P32��3��IO����Ϊ��������ģʽ

#endif