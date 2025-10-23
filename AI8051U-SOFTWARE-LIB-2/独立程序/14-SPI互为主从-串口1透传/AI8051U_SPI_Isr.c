/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_SPI.h"

//========================================================================
// ����: SPI_ISR_Handler
// ����: SPI�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2020-09-23
//========================================================================
void SPI_ISR_Handler() interrupt SPI_VECTOR
{
    if(SPIF)
    {
        if(MSTR)	//����ģʽ
        {
            B_SPI_Busy = 0;
        }
        else        //�ӻ�ģʽ
        {
            if(SPI_RxCnt >= SPI_BUF_LENTH) SPI_RxCnt = 0;
            SPI_RxBuffer[SPI_RxCnt++] = SPDAT;
        }
    }
    SPI_ClearFlag();	    //�� SPIF �� WCOL ��־

    if(SPITOSR & 0x01)
    {
        SPI_RxTimerOut = 1;
        SPI_TOIFClear();     //���� CTOCF �����ʱ��־λ TOIF
    }
}
