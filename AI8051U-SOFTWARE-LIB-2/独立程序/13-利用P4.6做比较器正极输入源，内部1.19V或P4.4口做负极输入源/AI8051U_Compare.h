/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_COMPARE_H
#define __AI8051U_COMPARE_H

#include "config.h"

//========================================================================
//                              ��������
//========================================================================

#define    CMP_P_P46    0x00    //����������ѡ��, 0: ѡ���ڲ�P4.6��������
#define    CMP_P_P50    0x01    //����������ѡ��, 1: ѡ���ڲ�P5.0��������
#define    CMP_P_P51    0x02    //����������ѡ��, 2: ѡ���ڲ�P5.1��������
#define    CMP_P_ADC    0x03    //����������ѡ��, 3: ��ADC_CHS[3:0]��ѡ���ADC�������������.
#define    CMP_N_P44    0x00    //���븺����ѡ��, 0: ѡ���ⲿP4.4������.
#define    CMP_N_GAP    0x01    //���븺����ѡ��, 1: ѡ���ڲ�BandGap��ѹBGv��������.

#define    CMPO_P45()    CMPO_S = 0    //��������P4.5.
#define    CMPO_P41()    CMPO_S = 1    //��������P4.1.

typedef struct
{ 
    u8    CMP_EN;           //�Ƚ���������ֹ,   ENABLE,DISABLE
    u8    CMP_P_Select;     //�Ƚ�����������ѡ��, CMP_P_P46/CMP_P_P50/CMP_P_P51/CMP_P_ADC.
    u8    CMP_N_Select;     //�Ƚ������븺��ѡ��, CMP_N_GAP: ѡ���ڲ�BandGap����OP��ĵ�ѹ��������, CMP_N_P44: ѡ��P4.4��������.
    u8    CMP_Outpt_En;     //����ȽϽ�����,   ENABLE,DISABLE
    u8    CMP_InvCMPO;      //�Ƚ������ȡ��, ENABLE,DISABLE
    u8    CMP_100nsFilter;  //�ڲ�0.1us�˲�,  ENABLE,DISABLE
    u8    CMP_OutDelayDuty; //0~63, �ȽϽ���仯��ʱ������
} CMP_InitDefine; 

void CMP_Inilize(CMP_InitDefine *CMPx);

#endif
