;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  ����˵��    **************

; �����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

; ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

; edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

; ��STC��MCU��IO��ʽ����74HC595����8λ����ܡ�

; ��ʾЧ��Ϊ: ��ʾ�����, 5���ι��, �ȸ�λ.

; ����ʱ, ѡ��ʱ�� 24MHZ (�û��������޸�Ƶ��).

;******************************************

$include (../../comm/AI8051U.INC)

;****************************** �û������ ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ; ��ջ��ʼ��ַ

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
;*******************************************************************

;WDT_CONTR DATA 0xC1     ;Watch-Dog-Timer Control register
;                                      7     6     5      4       3      2   1   0     Reset Value
;                                  WDT_FLAG  -  EN_WDT CLR_WDT IDLE_WDT PS2 PS1 PS0    xx00,0000
D_WDT_FLAG      EQU (1 SHL 7)
D_EN_WDT        EQU (1 SHL 5)
D_CLR_WDT       EQU (1 SHL 4)   ;auto clear
D_IDLE_WDT      EQU (1 SHL 3)   ;WDT counter when Idle
D_WDT_SCALE_2   EQU 0
D_WDT_SCALE_4   EQU 1
D_WDT_SCALE_8   EQU 2       ;T=393216*N/fo
D_WDT_SCALE_16  EQU 3
D_WDT_SCALE_32  EQU 4
D_WDT_SCALE_64  EQU 5
D_WDT_SCALE_128 EQU 6
D_WDT_SCALE_256 EQU 7
        
;*************  IO�ڶ���    **************/

P_HC595_SER     BIT     P3.4  ;   //pin 14    SER     data input
P_HC595_RCLK    BIT     P3.5  ;   //pin 12    RCLk    store (latch) clock
P_HC595_SRCLK   BIT     P3.2  ;   //pin 11    SRCLK   Shift data clock

;*************  ���ر�������    **************/
LED8            DATA    30H     ; ��ʾ���� 30H ~ 37H
display_index   DATA    38H     ; ��ʾλ����

msecond         DATA    39H     ;
tes_cnt         DATA    3BH     ; �����õļ�������

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

;*******************************************************************
;*******************************************************************


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
    MOV     A, #00000100B        ;���ÿ��Ź���λ��Ҫ���P3.2��״̬�������Ź���λ�����USB����ģʽ
    MOV     WR6, #WORD0 RSTFLAG
    MOV     WR4, #WORD2 RSTFLAG
    MOV     @DR4, R11

    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;�ϵ�����
    INC     R0
    DJNZ    R2, L_ClearLoop

    MOV     tes_cnt, #0
    MOV     LED8+7, tes_cnt

;=================== ��ѭ�� ==================================
L_MainLoop:
    MOV     A, #1           ;��ʱ1ms
    LCALL   F_delay_ms  
    LCALL   F_DisplayScan

    MOV     A, tes_cnt
    CLR     C
    SUBB    A, #6   ; if(tes_cnt <= 5)  //5���ι��, ����λ,
    JNC     L_QuitWatchDog
    MOV     WDT_CONTR, #(D_EN_WDT + D_CLR_WDT + D_WDT_SCALE_16) ; ι��
L_QuitWatchDog:
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_MainLoop      ;if(msecond < 1000), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    INC     tes_cnt     ; ���Լ���+1
    MOV     LED8+7, tes_cnt ;

L_cnt1000_NotZero:
    LJMP    L_MainLoop

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

    POP     04H     ;��ջR2
    POP     03H     ;��ջR3
    POP     02H     ;��ջR4
    RET

; *********************** ��ʾ��س��� ****************************************
T_Display:                      ;��׼�ֿ�
;    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB  03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K    L    N    o    P    U    t    G    Q    r    M    y
DB  000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB  0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB  001H,002H,004H,008H,010H,020H,040H,080H     ;   λ��

;========================================================================
; ����: F_Send_595
; ����: ��HC595����һ���ֽ��ӳ���
; ����: ACC: Ҫ���͵��ֽ�����.
; ����: none.
; �汾: VER1.0
; ����: 2024-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_Send_595:
    PUSH    02H     ;R2��ջ
    MOV     R2, #8
L_Send_595_Loop:
    RLC     A
    MOV     P_HC595_SER,C
    SETB    P_HC595_SRCLK
    CLR     P_HC595_SRCLK
    DJNZ    R2, L_Send_595_Loop
    POP     02H     ;R2��ջ
    RET

;========================================================================
; ����: F_DisplayScan
; ����: ��ʾɨ���ӳ���
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_DisplayScan:
    PUSH    DPH     ;DPH��ջ
    PUSH    DPL     ;DPL��ջ
    PUSH    00H     ;R0 ��ջ

    MOV     DPTR, #T_Display
    MOV     A, display_index
    ADD     A, #LED8
    MOV     R0, A
    MOV     A, @R0
    MOVC    A, @A+DPTR
    LCALL   F_Send_595      ;�������

    MOV     DPTR, #T_COM
    MOV     A, display_index
    MOVC    A, @A+DPTR
    CPL     A
    LCALL   F_Send_595      ;���λ��

    SETB    P_HC595_RCLK
    CLR     P_HC595_RCLK    ;   �����������
    INC     display_index
    MOV     A, display_index
    ANL     A, #0F8H            ; if(display_index >= 8)
    JZ      L_QuitDisplayScan
    MOV     display_index, #0;  ;8λ������0

L_QuitDisplayScan:
    POP     00H     ;R0 ��ջ
    POP     DPL     ;DPL��ջ
    POP     DPH     ;DPH��ջ
    RET



    END

