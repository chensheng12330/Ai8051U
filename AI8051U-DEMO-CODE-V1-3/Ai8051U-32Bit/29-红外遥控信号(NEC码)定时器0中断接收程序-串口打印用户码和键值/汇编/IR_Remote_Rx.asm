;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  ����˵��    **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;������ճ����������г�����������NEC���롣

;Ӧ�ò��ѯ B_IR_Press ��־Ϊ1,���ѽ��յ�һ���������IR_code��, �����������û�������� B_IR_Press ��־.

;���ڴ�ӡ������յ��û����밴����.

;����1(P3.0,P3.1)���ã�115200,N,8,1��ʹ��HEXģʽ��ӡ��

;������ս�(P35)������ܿ��ƽ�(RCK)���ã����Բ������������ʾ��

;����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ        EQU     24000   ;24000KHZ, �û�ֻ��Ҫ�Ķ����ֵ����Ӧ�Լ�ʵ�ʵ�Ƶ��
STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ
Timer0_Reload   EQU     (65536 - (Fosc_KHZ/10))   ; Timer 0 �ж�Ƶ��, 10000��/��

;*******************************************************************
;*******************************************************************


;*************  IO�ڶ���    **************/

P_IR_RX         BIT P3.5    ;��������������IO��

;*************  ���ر�������    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms��־
P_IR_RX_temp    BIT     Flag0.1 ; �û����ɲ���, Last sample
B_IR_Sync       BIT     Flag0.2 ; �û����ɲ���, ���յ�ͬ����־
B_IR_Press      BIT     Flag0.3 ; �û�ʹ��, ������������

cnt_1ms         DATA    39H     ;

;*************  ������ճ����������    **************

IR_SampleCnt    DATA    3AH ;�û����ɲ���, ��������
IR_BitCnt       DATA    3BH ;�û����ɲ���, ����λ��
IR_UserH        DATA    3CH ;�û����ɲ���, �û���(��ַ)���ֽ�
IR_UserL        DATA    3DH ;�û����ɲ���, �û���(��ַ)���ֽ�
IR_data         DATA    3EH ;�û����ɲ���, ����ԭ��
IR_DataShit     DATA    3FH ;�û����ɲ���, ������λ

IR_code         DATA    40H ;�û�ʹ��, �������
UserCodeH       DATA    41H ;�û�ʹ��, �û�����ֽ�
UserCodeL       DATA    42H ;�û�ʹ��, �û�����ֽ�

;========================================================================
; ����: ����1����1�ֽ����ݡ�
; ����: P_DATA: ���͵�����.
;========================================================================
PRINT MACRO P_DATA
    MOV     SBUF, P_DATA    ;����һ���ֽ�
    JNB     TI, $           ;�ȴ��������
    CLR     TI
ENDM

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

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
    MOV     PSW, #0     ;ѡ���0��R0~R7
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================

    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();

    LCALL   UART1_INIT
    SETB    EA          ; �����ж�
    
    MOV     cnt_1ms, #10

;=====================================================

;=====================================================
L_Main_Loop:

    JNB     B_1ms,  L_Main_Loop     ;1msδ��
    CLR     B_1ms
    
    JNB     B_IR_Press, L_Main_Loop ;δ��⵽�յ��������

    CLR     B_IR_Press      ;��⵽�յ��������
    PRINT   #052H           ;"R"
    PRINT   UserCodeH
    PRINT   UserCodeL
    PRINT   IR_code
    LJMP    L_Main_Loop

;**************** �жϺ��� ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms�жϺ���
    PUSH    PSW     ;PSW��ջ
    PUSH    ACC     ;ACC��ջ
    PUSH    AR7     ;SampleTime

    LCALL   F_IR_RX_NEC

    DJNZ    cnt_1ms, L_Quit_1ms
    MOV     cnt_1ms, #10
    SETB    B_1ms   ;1ms��־
L_Quit_1ms:

    POP     AR7
    POP     ACC     ;ACC��ջ
    POP     PSW     ;PSW��ջ
    RETI

;*******************************************************************
;*********************** IR Remote Module **************************
;*********************** By  (Coody) 2002-8-24 *********************
;*********************** IR Remote Module **************************
;this programme is used for Receive IR Remote (NEC Code).

;data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

;send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

;data rate: 108ms/Frame


;Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
;Bit0: high=0.5626ms, low=0.5626ms
;Bit1: high=1.6879ms, low=0.5626ms
;frame rate = 108ms ( pause 23 ms or 96 ms)

;******************** �������ʱ��궨��, �û���Ҫ�����޸�  *******************

D_IR_sample         EQU 100                 ;��ѯʱ����, 100us, �������Ҫ����60us~250us֮��
D_IR_SYNC_MAX       EQU (15000/D_IR_sample) ;SYNC max time
D_IR_SYNC_MIN       EQU (9700 /D_IR_sample) ;SYNC min time
D_IR_SYNC_DIVIDE    EQU (12375/D_IR_sample) ;decide data 0 or 1
D_IR_DATA_MAX       EQU (3000 /D_IR_sample) ;data max time
D_IR_DATA_MIN       EQU (600  /D_IR_sample) ;data min time
D_IR_DATA_DIVIDE    EQU (1687 /D_IR_sample) ;decide data 0 or 1
D_IR_BIT_NUMBER     EQU 32                  ;bit number

;*******************************************************************************************
;**************************** IR RECEIVE MODULE ********************************************

F_IR_RX_NEC:
    INC     IR_SampleCnt        ;Sample + 1

    MOV     C, P_IR_RX_temp     ;Save Last sample status
    MOV     F0, C
    MOV     C, P_IR_RX          ;Read current status
    MOV     P_IR_RX_temp, C

    JNB     F0, L_QuitIrRx              ;Pre-sample is high
    JB      P_IR_RX_temp, L_QuitIrRx    ;and current sample is low, so is fall edge

    MOV     R7, IR_SampleCnt            ;get the sample time
    MOV     IR_SampleCnt, #0            ;Clear the sample counter

    MOV     A, R7       ; if(SampleTime > D_IR_SYNC_MAX)    B_IR_Sync = 0
    SETB    C
    SUBB    A, #D_IR_SYNC_MAX
    JC      L_IR_RX_1
    CLR     B_IR_Sync       ;large than the Maxim SYNC time, then error
    RET
    
L_IR_RX_1:
    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    CLR     C
    SUBB    A, #D_IR_SYNC_MIN
    JC      L_IR_RX_2

    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    SUBB    A, #D_IR_SYNC_DIVIDE
    JC      L_QuitIrRx
    SETB    B_IR_Sync           ;has received SYNC
    MOV     IR_BitCnt, #D_IR_BIT_NUMBER     ;Load bit number
    RET

L_IR_RX_2:
    JNB     B_IR_Sync, L_QuitIrRx   ;else if(B_IR_Sync), has received SYNC
    MOV     A, R7       ; if(SampleTime > D_IR_DATA_MAX)    B_IR_Sync = 0, data samlpe time too large
    SETB    C
    SUBB    A, #D_IR_DATA_MAX
    JC      L_IR_RX_3
    CLR     B_IR_Sync       ;data samlpe time too large
    RET

L_IR_RX_3:
    MOV     A, IR_DataShit      ;data shift right 1 bit
    CLR     C
    RRC     A
    MOV     IR_DataShit, A
    
    MOV     A, R7
    CLR     C
    SUBB    A, #D_IR_DATA_DIVIDE
    JC      L_IR_RX_4
    ORL     IR_DataShit, #080H  ;if(SampleTime >= D_IR_DATA_DIVIDE) IR_DataShit |= 0x80;    //devide data 0 or 1
L_IR_RX_4:
    DEC     IR_BitCnt
    MOV     A, IR_BitCnt
    JNZ     L_IR_RX_5           ;bit number is over?
    CLR     B_IR_Sync           ;Clear SYNC
    MOV     A, IR_DataShit      ;if(~IR_DataShit == IR_data)        //�ж�����������
    CPL     A
    XRL     A, IR_data
    JNZ     L_QuitIrRx
    
    MOV     UserCodeH, IR_UserH
    MOV     UserCodeL, IR_UserL
    MOV     IR_code, IR_data
    SETB    B_IR_Press          ;������Ч
    RET

L_IR_RX_5:
    MOV     A, IR_BitCnt        ;else if((IR_BitCnt & 7)== 0)   one byte receive
    ANL     A, #07H
    JNZ     L_QuitIrRx
    MOV     IR_UserL, IR_UserH      ;Save the User code high byte
    MOV     IR_UserH, IR_data       ;Save the User code low byte
    MOV     IR_data,  IR_DataShit   ;Save the IR data byte
L_QuitIrRx:
    RET

;========================================================================
; ����: UART1_INIT
; ����: UART1��ʼ������.
; ����: None
; ����: none.
; �汾: V1.0, 2024-07-22
;========================================================================
UART1_INIT:                 ;115200bps@24.000MHz
    MOV     SCON,#50H       ;8λ����,�ɱ䲨����
    ORL     AUXR,#40H       ;��ʱ��ʱ��1Tģʽ
    ANL     AUXR,#0FEH      ;����1ѡ��ʱ��1Ϊ�����ʷ�����
    ANL     TMOD,#0FH       ;���ö�ʱ��ģʽ
    MOV     TL1,#0CCH       ;���ö�ʱ��ʼֵ
    MOV     TH1,#0FFH       ;���ö�ʱ��ʼֵ
    CLR     ET1             ;��ֹ��ʱ���ж�
    SETB    TR1             ;��ʱ��1��ʼ��ʱ

    ANL     P_SW1, #0x3f
    ORL     P_SW1, #0x00    ;UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    RET


    END

