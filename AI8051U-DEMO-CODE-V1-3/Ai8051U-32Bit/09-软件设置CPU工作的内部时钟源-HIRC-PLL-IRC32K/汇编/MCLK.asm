;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;/************* ����˵��    **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;����ʹ��P0��������ƣ���ʾϵͳʱ��Դ�л�Ч����

;�����ÿ��һ���л�һ��ʱ��Դ���ֱ���Ĭ��IRC��Ƶ����Ƶ12��Ƶ��PLL 96M 16��Ƶ����2��Ƶ���ڲ�32K IRC��

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

;*******************************************************************
;*******************************************************************

;*******************************************************************

Mode        DATA    21H
Count       DATA    22H

;*******************************************************************
            ORG     0000H      ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
            LJMP    MAIN

            ORG     0100H      ;�������Զ����嵽 0FF0100H ��ַ
MAIN:
    MOV     WTST, #00H     ;���ó���ָ����ʱ��������ֵΪ0�ɽ�CPUִ��ָ����ٶ�����Ϊ���
    MOV     CKCON,#00H     ;��߷���XRAM�ٶ�
    ORL     P_SW2,#080H    ;ʹ�ܷ���XFR

    MOV     P0M1, #00H     ;����Ϊ׼˫���
    MOV     P0M0, #00H
    MOV     P1M1, #00H     ;����Ϊ׼˫���
    MOV     P1M0, #00H
    MOV     P2M1, #00H     ;����Ϊ׼˫���
    MOV     P2M0, #00H
    MOV     P3M1, #00H     ;����Ϊ׼˫���
    MOV     P3M0, #00H
    MOV     P4M1, #00H     ;����Ϊ׼˫���
    MOV     P4M0, #00H
    MOV     P5M1, #00H     ;����Ϊ׼˫���
    MOV     P5M0, #00H
    MOV     P6M1, #00H     ;����Ϊ׼˫���
    MOV     P6M0, #00H
    MOV     P7M1, #00H     ;����Ϊ׼˫���
    MOV     P7M0, #00H

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0     ;ѡ���0��R0~R7

    CLR     P4.0        ;LED Power On
    MOV     Mode, #0
    MOV     R0, #0XFE
L_MainLoop:
    MOV     P0,R0
    
    MOV     A,R0
	RL      A
    MOV     R0,A

    JB      ACC.0,L_MainDelay
    LCALL   F_MCLK_Sel

L_MainDelay:
    MOV     A, #10
    LCALL   F_delay_ms      ;��ʱ10ms
    SJMP    L_MainLoop

;========================================================================
; ����: F_delay_ms
; ����: ��ʱ�ӳ���
; ����: ACC: ��ʱms��.
; ����: none.
; �汾: VER1.0
; ����: 2021-3-16
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_delay_ms:
    PUSH    02H     ;��ջR2
    PUSH    03H     ;��ջR3
    PUSH    04H     ;��ջR4

    MOV     R4,A
L_delay_ms_1:
    MOV     WR2, #(Fosc_KHZ / 4)
L_delay_ms_2:
    DEC     WR2, #1         ;1T
    JNE     L_delay_ms_2    ;3T
    DJNZ    R4, L_delay_ms_1

    POP     04H     ;��ջR4
    POP     03H     ;��ջR3
    POP     02H     ;��ջR2
    RET

;========================================================================
; ����: F_MCLK_Sel
; ����: ��Ƶ���ó���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
F_MCLK_Sel:
    MOV     A,Mode
	JNZ     F_MCLK_1
F_MCLK_0:
    MOV     A, #080H                    ;�����ڲ� IRC
    MOV     WR6, #WORD0 HIRCCR
    MOV     WR4, #WORD2 HIRCCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                   ;�ȴ�ʱ���ȶ�

    MOV     A, #00H                     ;ʱ�Ӳ���Ƶ
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #00H                     ;ѡ���ڲ�IRC(Ĭ��)
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_1:
    MOV     A,Mode
    XRL     A,#1
	JNZ     F_MCLK_2
    MOV     A, #080H                    ;�����ڲ� IRC
    MOV     WR6, #WORD0 HIRCCR
    MOV     WR4, #WORD2 HIRCCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                   ;�ȴ�ʱ���ȶ�

    MOV     A, #48                      ;ʱ��48��Ƶ
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #00H                     ;ѡ���ڲ�IRC(Ĭ��)
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_2:
    MOV     A,Mode
    XRL     A,#2
	JNZ     F_MCLK_3
    MOV     A, #0A0H                    ;ʹ��PLL��Ƶ, ����ʱ��2��Ƶ(ѡ��PLL����ʱ�ӷ�Ƶ,��֤����ʱ��Ϊ12M)
    MOV     WR6, #WORD0 USBCLK
    MOV     WR4, #WORD2 USBCLK
    MOV     @DR4, R11

    MOV     Count, #100
    DJNZ    Count,$                     ;�ȴ�PLL��Ƶ

    MOV     A, #8                       ;ʱ��8��Ƶ
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #08H                     ;ѡ��PLL���ʱ��2��Ƶ���ʱ����Ϊ��ʱ��
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_3:
    MOV     A,Mode
    XRL     A,#3
	JNZ     F_MCLK_CLR
    MOV     A, #080H                    ;�����ڲ� 32K IRC
    MOV     WR6, #WORD0 IRC32KCR
    MOV     WR4, #WORD2 IRC32KCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                  ;�ȴ�ʱ���ȶ�

    MOV     A, #00H                     ;ʱ�Ӳ���Ƶ
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #03H                     ;ѡ���ڲ� 32K
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
F_MCLK_CLR:
    MOV     Mode, #0
    RET

;F_MCLK_4:
    ;MOV     A,Mode
    ;XRL     A,#3
	;JNZ     F_MCLK_CLR
    ;MOV     A, #0C0H                   ;�����ⲿ����
    ;MOV     WR6, #WORD0 XOSCCR
    ;MOV     WR4, #WORD2 XOSCCR
    ;MOV     @DR4, R11
    ;MOV     R11, @DR4
    ;JNB     ACC.0,$-1                  ;�ȴ�ʱ���ȶ�

    ;MOV     A, #00H                     ;ʱ�Ӳ���Ƶ
    ;MOV     WR6, #WORD0 CLKDIV
    ;MOV     WR4, #WORD2 CLKDIV
    ;MOV     @DR4, R11
    ;MOV     A, #001H                     ;ѡ���ⲿ����
    ;MOV     WR6, #WORD0 CLKSEL
    ;MOV     WR4, #WORD2 CLKSEL
    ;MOV     @DR4, R11
    ;MOV     Mode, #0
    ;RET

    END

