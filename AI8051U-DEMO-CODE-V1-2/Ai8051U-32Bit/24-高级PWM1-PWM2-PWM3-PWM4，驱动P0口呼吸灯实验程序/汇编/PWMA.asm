;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;/************* ����˵��    **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;�߼�PWM��ʱ�� PWM1P/PWM1N,PWM2P/PWM2N,PWM3P/PWM3N,PWM4P/PWM4N ÿ��ͨ�����ɶ���ʵ��PWM������������������Գ����.

;8��ͨ��PWM���ö�ӦP0��8���˿�.

;ͨ��P0�������ӵ�8��LED�ƣ�����PWMʵ�ֺ�����Ч��.

;PWM���ں�ռ�ձȿ��Ը�����Ҫ�������ã���߿ɴ�65535.

;����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 �ж�Ƶ��, 1000��/��

;*******************************************************************
;*******************************************************************


;*************  ���ر�������    **************/
PWM1_Flag       BIT     20H.0
PWM2_Flag       BIT     20H.1
PWM3_Flag       BIT     20H.2
PWM4_Flag       BIT     20H.3

PWM1_Duty       DATA    30H
PWM2_Duty       DATA    32H
PWM3_Duty       DATA    34H
PWM4_Duty       DATA    36H

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

;******************** ������ **************************/
        ORG     0100H       ;�������Զ����嵽 0FF0100H ��ַ
F_Main:
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
    MOV     PSW, #0
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================
    CLR     PWM1_Flag
    CLR     PWM2_Flag
    CLR     PWM3_Flag
    CLR     PWM4_Flag
    MOV     WR6, #0
    MOV     PWM1_Duty, WR6
    MOV     WR6, #256
    MOV     PWM2_Duty, WR6
    MOV     WR6, #512
    MOV     PWM3_Duty, WR6
    MOV     WR6, #1024
    MOV     PWM4_Duty, WR6

    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    SETB    EA          ; �����ж�
    
    LCALL   F_PWM_Init          ; PWM��ʼ��
    CLR     P4.0

;=================== ��ѭ�� ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;========================================================================
; ����: F_PWM_Init
; ����: PWM��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-3
;========================================================================
F_PWM_Init:
    MOV     A, #00H              ;д CCMRx ǰ���������� CCxE �ر�ͨ��
    MOV     WR6, #WORD0 PWMA_CCER1
    MOV     WR4, #WORD2 PWMA_CCER1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11
    MOV     A, #060H             ;�� PWMx ģʽ1 ���
    MOV     WR6, #WORD0 PWMA_CCMR1
    MOV     WR4, #WORD2 PWMA_CCMR1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR2
    MOV     WR4, #WORD2 PWMA_CCMR2
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR3
    MOV     WR4, #WORD2 PWMA_CCMR3
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11
    MOV     A, #055H             ;����ͨ�����ʹ�ܺͼ���
    MOV     WR6, #WORD0 PWMA_CCER1
    MOV     WR4, #WORD2 PWMA_CCER1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCER2
    MOV     WR4, #WORD2 PWMA_CCER2
    MOV     @DR4, R11

    MOV     A, #068H             ;����PWMA_CCRxԤת�ع���(��ҪCCxE=1�ſ�д)
    MOV     WR6, #WORD0 PWMA_CCMR1
    MOV     WR4, #WORD2 PWMA_CCMR1
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR2
    MOV     WR4, #WORD2 PWMA_CCMR2
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR3
    MOV     WR4, #WORD2 PWMA_CCMR3
    MOV     @DR4, R11
    MOV     WR6, #WORD0 PWMA_CCMR4
    MOV     WR4, #WORD2 PWMA_CCMR4
    MOV     @DR4, R11

    MOV     A, #3                ;��������ʱ��
    MOV     WR6, #WORD0 PWMA_ARRH
    MOV     WR4, #WORD2 PWMA_ARRH
    MOV     @DR4, R11
    MOV     A, #0FFH
    MOV     WR6, #WORD0 PWMA_ARRL
    MOV     WR4, #WORD2 PWMA_ARRL
    MOV     @DR4, R11

    MOV     A, #0FFH             ;ʹ�� PWM1~4 ���
    MOV     WR6, #WORD0 PWMA_ENO
    MOV     WR4, #WORD2 PWMA_ENO
    MOV     @DR4, R11
    MOV     A, #055H             ;�߼� PWM ͨ�������ѡ��λ, P0
    MOV     WR6, #WORD0 PWMA_PS
    MOV     WR4, #WORD2 PWMA_PS
    MOV     @DR4, R11
    MOV     A, #080H             ;ʹ�������
    MOV     WR6, #WORD0 PWMA_BKR
    MOV     WR4, #WORD2 PWMA_BKR
    MOV     @DR4, R11

    MOV     WR6, #WORD0 PWMA_CR1
    MOV     WR4, #WORD2 PWMA_CR1
    MOV     R11, @DR4
    ORL     A,#081H              ;ʹ��ARRԤװ��,��ʼ��ʱ
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
    MOV     WR2, PWM1_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR1H
    MOV     WR4, #WORD2 PWMA_CCR1H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMA_CCR1L
    MOV     @DR4, R3

    MOV     WR2, PWM2_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR2H
    MOV     WR4, #WORD2 PWMA_CCR2H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMA_CCR2L
    MOV     @DR4, R3

    MOV     WR2, PWM3_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR3H
    MOV     WR4, #WORD2 PWMA_CCR3H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMA_CCR3L
    MOV     @DR4, R3

    MOV     WR2, PWM4_Duty      ;����ռ�ձ�ʱ��
    MOV     WR6, #WORD0 PWMA_CCR4H
    MOV     WR4, #WORD2 PWMA_CCR4H
    MOV     @DR4, R2
    MOV     WR6, #WORD0 PWMA_CCR4L
    MOV     @DR4, R3
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
    JC      T0_PWM2_Start
    SETB    PWM1_Flag
    SJMP    T0_PWM2_Start
T0_PWM1_SUB:
    MOV     WR0, PWM1_Duty
    DEC     WR0, #1
    MOV     PWM1_Duty, WR0   ;PWM1_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM2_Start
    CLR     PWM1_Flag

T0_PWM2_Start:
    JB      PWM2_Flag, T0_PWM2_SUB
    MOV     WR0, PWM2_Duty
    INC     WR0, #1
    MOV     PWM2_Duty, WR0   ;PWM2_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM3_Start
    SETB    PWM2_Flag
    SJMP    T0_PWM3_Start
T0_PWM2_SUB:
    MOV     WR0, PWM2_Duty
    DEC     WR0, #1
    MOV     PWM2_Duty, WR0   ;PWM2_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM3_Start
    CLR     PWM2_Flag

T0_PWM3_Start:
    JB      PWM3_Flag, T0_PWM3_SUB
    MOV     WR0, PWM3_Duty
    INC     WR0, #1
    MOV     PWM3_Duty, WR0   ;PWM3_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM4_Start
    SETB    PWM3_Flag
    SJMP    T0_PWM4_Start
T0_PWM3_SUB:
    MOV     WR0, PWM3_Duty
    DEC     WR0, #1
    MOV     PWM3_Duty, WR0   ;PWM3_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM4_Start
    CLR     PWM3_Flag

T0_PWM4_Start:
    JB      PWM4_Flag, T0_PWM4_SUB
    MOV     WR0, PWM4_Duty
    INC     WR0, #1
    MOV     PWM4_Duty, WR0   ;PWM4_Duty + 1
    CMP     WR0, #1023
    JC      F_QuitTimer0
    SETB    PWM4_Flag
    SJMP    F_QuitTimer0
T0_PWM4_SUB:
    MOV     WR0, PWM4_Duty
    DEC     WR0, #1
    MOV     PWM4_Duty, WR0   ;PWM4_Duty - 1
    CMP     WR0, #0
    JG      F_QuitTimer0
    CLR     PWM4_Flag

F_QuitTimer0:
    LCALL   F_UpdatePwm

    POP     R0      ;R0��ջ
    POP     R1      ;R1��ջ
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;======================================================================

    END

