/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_LCM.h"

//========================================================================
// ����: void LCM_Inilize(LCM_InitTypeDef *LCM)
// ����: LCM ��ʼ������.
// ����: LCM: �ṹ����,��ο�LCM.h��Ķ���.
// ����: none.
// �汾: V1.0, 2021-06-02
//========================================================================
void LCM_Inilize(LCM_InitTypeDef *LCM)
{
    LCMIFSTA = 0x00;
    if(LCM->LCM_Mode == MODE_M6800) LCMIFCFG |= MODE_M6800;    //LCM�ӿ�ģʽ��M6800
    else LCMIFCFG &= ~MODE_M6800;   //LCM�ӿ�ģʽ��I8080
    
    if(LCM->LCM_Bit_Wide == BIT_WIDE_16) LCMIFCFG |= BIT_WIDE_16;   //LCM���ݿ�ȣ�16λ
    else LCMIFCFG &= ~BIT_WIDE_16;  //LCM���ݿ�ȣ�8λ
    
    if(LCM->LCM_Setup_Time <= 7) LCMIFCFG2 = (LCMIFCFG2 & ~0x1c) | (LCM->LCM_Setup_Time << 2);  //LCMͨ�����ݽ���ʱ�䣺0~7
    if(LCM->LCM_Hold_Time <= 3) LCMIFCFG2 = (LCMIFCFG2 & ~0x03) | LCM->LCM_Hold_Time;   //LCMͨ�����ݽ���ʱ�䣺0~7
    
    if(LCM->LCM_Enable == ENABLE) LCMIFCR |= 0x80;  //ʹ��LCM�ӿڹ���
    else LCMIFCR &= ~0x80;    //��ֹLCM�ӿڹ���
}
