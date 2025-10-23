/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "AI8051U_WDT.h"
#include "AI8051U_GPIO.h"
#include "AI8051U_Delay.h"

/*************   ����˵��   ***************

��ʼ����ת��ƽ, 5���ι��, �ȴ����Ź���λ.

����ʱ, ѡ��ʱ�� 40MHz (�û�����"config.h"�޸�Ƶ��).

******************************************/

/*************    ���س�������    **************/


/*************    ���ر�������    **************/
u16 ms_cnt;
u8  second;    //�����õļ�������

/*************    ���غ�������    **************/


/*************  �ⲿ�����ͱ������� *****************/



/******************** IO������ ********************/
void GPIO_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;        //�ṹ����

    GPIO_InitStructure.Pin  = GPIO_Pin_2;       //ָ��Ҫ��ʼ����IO, GPIO_Pin_0 ~ GPIO_Pin_7, �����
    GPIO_InitStructure.Mode = GPIO_PullUp;      //ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
    GPIO_Inilize(GPIO_P4,&GPIO_InitStructure);  //��ʼ��
}

/******************** WDT���� ********************/
void WDT_config(void)
{
    WDT_InitTypeDef WDT_InitStructure;                  //�ṹ����

    WDT_InitStructure.WDT_Enable     = ENABLE;          //���Ź�ʹ��   ENABLE��DISABLE
    WDT_InitStructure.WDT_IDLE_Mode  = WDT_IDLE_STOP;   //IDLEģʽ�Ƿ�ֹͣ����       WDT_IDLE_STOP,WDT_IDLE_RUN
    WDT_InitStructure.WDT_PS         = WDT_SCALE_16;    //���Ź���ʱ��ʱ�ӷ�Ƶϵ��    WDT_SCALE_2,WDT_SCALE_4,WDT_SCALE_8,WDT_SCALE_16,WDT_SCALE_32,WDT_SCALE_64,WDT_SCALE_128,WDT_SCALE_256
    WDT_Inilize(&WDT_InitStructure);                    //��ʼ��
}

/******************** ������***********************/
void main(void)
{
    WTST = 0;        //���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    EAXSFR();        //��չSFR(XFR)����ʹ�� 
    CKCON = 0;       //��߷���XRAM�ٶ�

    GPIO_config();
    
    P42 = 0;
    delay_ms(200);
    P42 = 1;
    delay_ms(200);
    P42 = 0;
    delay_ms(200);
    P42 = 1;
    delay_ms(200);
    
    WDT_config();

    RSTFLAG |= 0x04;    //���ÿ��Ź���λ��Ҫ���P3.2��״̬�����⿴�Ź���λ�����USB����ģʽ

    while(1)
    {
        delay_ms(1);        //��ʱ1ms
        if(second <= 5)     //5���ι��, ����λ,
            WDT_Clear();    //ι��

        if(++ms_cnt >= 1000)
        {
            ms_cnt = 0;
            second++;
        }
    }
}
