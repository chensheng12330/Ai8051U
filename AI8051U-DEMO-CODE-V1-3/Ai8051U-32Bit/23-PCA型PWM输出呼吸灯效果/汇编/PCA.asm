;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/


;/************* ����˵��    **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;������ʾоƬPCA���������Ч��PWM�ź�.

;PCA0��P4.2�����8λPWM�źţ���ͨ��ʵ����LED11�鿴Ч��.

;PCA1��P4.3�����10λPWM�źţ���ͨ��ʵ����LED10�鿴Ч��.

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - (Fosc_KHZ / 6))  ; Timer 0 (12T)�ж�Ƶ��, 500��/��

;*******************************************************************

PWM0_Flag       BIT     20H.0
PWM1_Flag       BIT     20H.1

PWM0_Duty       DATA    30H
PWM1_Duty       DATA    32H

;*******************************************************************

;*******************************************************************

Mode        DATA    21H
Count       DATA    22H

;*******************************************************************
    ORG     0000H           ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
    LJMP    MAIN

    ORG     000BH           ;1  Timer0 interrupt
    LJMP    F_Timer0_Interrupt

;******************** ������ **************************/
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

;================= �û���ʼ������ ====================================
    CLR     PWM0_Flag
    CLR     PWM1_Flag
    MOV     WR6, #128
    MOV     PWM0_Duty, WR6
    MOV     WR6, #256
    MOV     PWM1_Duty, WR6

    CLR     TR0
    ANL     AUXR, #NOT(1 SHL 7) ; Timer0_12T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    SETB    EA          ; �����ж�
    
    LCALL   F_PWM_Init          ; PWM��ʼ��

L_MainLoop:
    SJMP    L_MainLoop

;========================================================================
; ����: F_PWM_Init
; ����: PWM��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_PWM_Init:
    //bit[6:5]=0: ECI/P1.2, CCP0/P1.3, CCP1/P1.4, CCP2/P1.1
    //bit[6:5]=1: ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    //bit[6:5]=2: ECI/P2.3, CCP0/P2.0, CCP1/P2.1, CCP2/P2.2
    MOV     WR6, #WORD0 CMOD
    MOV     WR4, #WORD2 CMOD
    MOV     R11, @DR4
    ANL     A,#09FH
    ORL     A,#(1 SHL 5)        ;ѡ��PCAͨ�� 1:ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    ORL     A,#0EH              ;PCA ʱ��Ϊϵͳʱ��/8
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 CL
    MOV     WR4, #WORD2 CL
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CH
    MOV     WR4, #WORD2 CH
    MOV     @DR4, R11

    ;���� PCA0 ģ��
    MOV     A, #042H            ;PCA ģ�� 0 Ϊ PWM ����ģʽ
    MOV     WR6, #WORD0 CCAPM0
    MOV     WR4, #WORD2 CCAPM0
    MOV     @DR4, R11

    MOV     A, #00H             ;PCA ģ�� 0 ��� 8 λ PWM
    MOV     WR6, #WORD0 PCA_PWM0
    MOV     WR4, #WORD2 PCA_PWM0
    MOV     @DR4, R11

    MOV     A, #080H            ;���� PWM ��ʼռ�ձ�
    MOV     WR6, #WORD0 CCAP0L
    MOV     WR4, #WORD2 CCAP0L
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CCAP0H
    MOV     WR4, #WORD2 CCAP0H
    MOV     @DR4, R11

    ;���� PCA1 ģ��
    MOV     A, #042H            ;PCA ģ�� 1 Ϊ PWM ����ģʽ
    MOV     WR6, #WORD0 CCAPM1
    MOV     WR4, #WORD2 CCAPM1
    MOV     @DR4, R11

    MOV     A, #0C0H            ;PCA ģ�� 1 ��� 10 λ PWM
    MOV     WR6, #WORD0 PCA_PWM1
    MOV     WR4, #WORD2 PCA_PWM1
    MOV     @DR4, R11

    MOV     A, #040H            ;���� PWM ��ʼռ�ձ�
    MOV     WR6, #WORD0 CCAP1L
    MOV     WR4, #WORD2 CCAP1L
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CCAP1H
    MOV     WR4, #WORD2 CCAP1H
    MOV     @DR4, R11

    ;���� PCA2 ģ��
;    MOV     A, #042H            ;PCA ģ�� 2 Ϊ PWM ����ģʽ
;    MOV     WR6, #WORD0 CCAPM2
;    MOV     WR4, #WORD2 CCAPM2
;    MOV     @DR4, R11

;    MOV     A, #080H            ;PCA ģ�� 2 ��� 6 λ PWM
;    MOV     WR6, #WORD0 PCA_PWM2
;    MOV     WR4, #WORD2 PCA_PWM2
;    MOV     @DR4, R11

;    MOV     A, #020H            ;���� PWM ��ʼռ�ձ�
;    MOV     WR6, #WORD0 CCAP2L
;    MOV     WR4, #WORD2 CCAP2L
;    MOV     @DR4, R11
;    MOV     WR6, #WORD0 CCAP2H
;    MOV     WR4, #WORD2 CCAP2H
;    MOV     @DR4, R11

    MOV     WR6, #WORD0 CCON
    MOV     WR4, #WORD2 CCON
    MOV     R11, @DR4
    ORL     A,#040H              ;���� PCA ��ʱ��
    MOV     @DR4, R11

    RET

;========================================================================
; ����: F_UpdatePwm
; ����: ����PWMռ�ձ�ֵ. 
; ����: [PWMn_Duty_H PWMn_Duty_L]: pwmռ�ձ�ֵ.
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_UpdatePwm:

;    PCA_PWM0 = (PCA_PWM0 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
;    CCAP0H = (u8)pwm_value;

    MOV     WR0, PWM0_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, WR0
    ANL     WR6, #0300H
    SRL     WR6
    SRL     WR6
    SRL     WR6
    SRL     WR6
    MOV     WR30, #WORD0 PCA_PWM0
    MOV     WR28, #WORD2 PCA_PWM0
    MOV     R6, @DR28
    ANL     R6, #0CDH
    ORL     R6,R7

    MOV     WR2, WR0
    ANL     WR2, #0400H
    MOVZ    WR2,R2
    SRL     WR2
    MOV     R7,R3
    ORL     R7,R6
    MOV     @DR28,R7

    MOV     R3,R1
    MOV     WR6, #WORD0 CCAP0H
    MOV     WR4, #WORD2 CCAP0H
    MOV     @DR4,R3

;    PCA_PWM1 = (PCA_PWM1 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
;    CCAP1H = (u8)pwm_value;

    MOV     WR0, PWM1_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, WR0
    ANL     WR6, #0300H
    SRL     WR6
    SRL     WR6
    SRL     WR6
    SRL     WR6
    MOV     WR30, #WORD0 PCA_PWM1
    MOV     WR28, #WORD2 PCA_PWM1
    MOV     R6, @DR28
    ANL     R6, #0CDH
    ORL     R6,R7

    MOV     WR2, WR0
    ANL     WR2, #0400H
    MOVZ    WR2,R2
    SRL     WR2
    MOV     R7,R3
    ORL     R7,R6
    MOV     @DR28,R7

    MOV     R3,R1
    MOV     WR6, #WORD0 CCAP1H
    MOV     WR4, #WORD2 CCAP1H
    MOV     @DR4,R3

    RET

;**************** �жϺ��� ***************************************************
F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    R1      ;R1��ջ
    PUSH    R0      ;R0��ջ

    JB      PWM1_Flag, T0_PWM1_SUB
    MOV     WR0, PWM1_Duty
    INC     WR0, #1
    MOV     PWM1_Duty, WR0   ;PWM1_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM0_Start
    SETB    PWM1_Flag
    SJMP    T0_PWM0_Start
T0_PWM1_SUB:
    MOV     WR0, PWM1_Duty
    DEC     WR0, #1
    MOV     PWM1_Duty, WR0   ;PWM1_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM0_Start
    CLR     PWM1_Flag

T0_PWM0_Start:
    JB      PWM0_Flag, T0_PWM0_SUB
    MOV     WR0, PWM0_Duty
    INC     WR0, #1
    MOV     PWM0_Duty, WR0   ;PWM0_Duty + 1
    CMP     WR0, #255
    JC      F_QuitTimer0
    SETB    PWM0_Flag
    SJMP    F_QuitTimer0
T0_PWM0_SUB:
    MOV     WR0, PWM0_Duty
    DEC     WR0, #1
    MOV     PWM0_Duty, WR0   ;PWM0_Duty - 1
    CMP     WR0, #0
    JG      F_QuitTimer0
    CLR     PWM0_Flag

F_QuitTimer0:
    LCALL   F_UpdatePwm

    POP     R0      ;R0��ջ
    POP     R1      ;R1��ջ
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;========================================================================

    END

