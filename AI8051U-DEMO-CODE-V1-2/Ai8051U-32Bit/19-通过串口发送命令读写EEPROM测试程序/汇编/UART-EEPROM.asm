;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  ��������˵��  **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;ͨ������1(P3.0 P3.1)��STC�ڲ��Դ���EEPROM(FLASH)���ж�д���ԡ�

;��FLASH������������д�롢�����Ĳ���������ָ����ַ��

;Ĭ�ϲ�����:  115200,8,N,1. 
;Ĭ����ʱ��:  22.1184MHz.

;������������: (������ĸ�����ִ�Сд)
;   W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.
;   R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����. 

;ע�⣺����ʱ�����ؽ���"Ӳ��ѡ��"�������û�EEPROM��С��EEPROM��С�����û�ϵͳ��
;������Ҫ�û�ϵͳ��4K��EEPROM��8K����ô�û�EEPROM��С��Ҫ����12K��

;��ȷ�����������еĵ�ַ��EEPROM���õĴ�С��Χ֮�ڡ�

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/
Fosc_KHZ    EQU 22118   ;22118KHZ

;UART1_Baudrate EQU     (-4608) ;   600bps @ 11.0592MHz
;UART1_Baudrate EQU     (-2304) ;  1200bps @ 11.0592MHz UART1_Baudrate = 65536UL - ((MAIN_Fosc / 4) / Baudrate)
;UART1_Baudrate EQU     (-1152) ;  2400bps @ 11.0592MHz
;UART1_Baudrate EQU     (-576)  ;  4800bps @ 11.0592MHz
;UART1_Baudrate EQU     (-288)  ;  9600bps @ 11.0592MHz
;UART1_Baudrate EQU     (-144)  ; 19200bps @ 11.0592MHz
;UART1_Baudrate EQU     (-72)   ; 38400bps @ 11.0592MHz
;UART1_Baudrate EQU     (-48)   ; 57600bps @ 11.0592MHz
;UART1_Baudrate EQU      (-24)   ;115200bps @ 11.0592MHz

;UART1_Baudrate EQU     (-7680) ;   600bps @ 18.432MHz
;UART1_Baudrate EQU     (-3840) ;  1200bps @ 18.432MHz
;UART1_Baudrate EQU     (-1920) ;  2400bps @ 18.432MHz
;UART1_Baudrate EQU     (-960)  ;  4800bps @ 18.432MHz
;UART1_Baudrate EQU     (-480)  ;  9600bps @ 18.432MHz
;UART1_Baudrate EQU     (-240)  ; 19200bps @ 18.432MHz
;UART1_Baudrate EQU     (-120)  ; 38400bps @ 18.432MHz
;UART1_Baudrate EQU     (-80)   ; 57600bps @ 18.432MHz
;UART1_Baudrate EQU     (-40)   ;115200bps @ 18.432MHz

;UART1_Baudrate EQU     (-9216) ;   600bps @ 22.1184MHz
;UART1_Baudrate EQU     (-4608) ;  1200bps @ 22.1184MHz
;UART1_Baudrate EQU     (-2304) ;  2400bps @ 22.1184MHz
;UART1_Baudrate EQU     (-1152) ;  4800bps @ 22.1184MHz
;UART1_Baudrate EQU     (-576)  ;  9600bps @ 22.1184MHz
;UART1_Baudrate EQU     (-288)  ; 19200bps @ 22.1184MHz
;UART1_Baudrate EQU     (-144)  ; 38400bps @ 22.1184MHz
;UART1_Baudrate EQU     (-96)   ; 57600bps @ 22.1184MHz
UART1_Baudrate  EQU    (-48)   ;115200bps @ 22.1184MHz

;UART1_Baudrate EQU     (-6912) ; 1200bps @ 33.1776MHz
;UART1_Baudrate EQU     (-3456) ; 2400bps @ 33.1776MHz
;UART1_Baudrate EQU     (-1728) ; 4800bps @ 33.1776MHz
;UART1_Baudrate EQU     (-864)  ; 9600bps @ 33.1776MHz
;UART1_Baudrate EQU     (-432)  ;19200bps @ 33.1776MHz
;UART1_Baudrate EQU     (-216)  ;38400bps @ 33.1776MHz
;UART1_Baudrate EQU     (-144)  ;57600bps @ 33.1776MHz
;UART1_Baudrate EQU     (-72)   ;115200bps @ 33.1776MHz


IAP_EN          EQU     (1 SHL 7)
IAP_SWBS        EQU     (1 SHL 6)
IAP_SWRST       EQU     (1 SHL 5)
IAP_CMD_FAIL    EQU     (1 SHL 4)

TPS_WAIT        EQU     22   ;22.1184MHZ / 1000000


RX1_Lenth   EQU     32      ; ���ڽ��ջ��峤��

B_TX1_Busy  BIT     20H.0   ; ����æ��־

TX1_Cnt     DATA    30H     ; ���ͼ���
RX1_Cnt     DATA    31H     ; ���ռ���
address_E   DATA    32H
address_H   DATA    33H
address_L   DATA    34H
length      DATA    35H
RX1_TimeOut DATA    36H

RX1_Buffer  DATA    40H     ; 40 ~ 5FH ���ջ���
tmp         DATA    60H     ; 60~7F

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ



;*******************************************************************
;*******************************************************************
        ORG     0000H       ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
        LJMP    F_Main

        ORG     0023H       ;4  UART1 interrupt
        LJMP    F_UART1_Interrupt


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
    MOV     A, #1
    LCALL   F_UART1_config  ; ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    SETB    EA      ; ����ȫ���ж�
    
    MOV     DPTR, #TestString1  ;Load string address to DPTR
    LCALL   F_SendString1       ;Send string
    MOV     DPTR, #TestString1A ;Load string address to DPTR
    LCALL   F_SendString1       ;Send string
    MOV     DPTR, #TestString1B ;Load string address to DPTR
    LCALL   F_SendString1       ;Send string

;=================== ��ѭ�� ==================================
L_MainLoop:
    MOV     A, #1
    LCALL   F_delay_ms
    MOV     A, RX1_TimeOut      ;��ʱ����
    JZ      L_MainLoop          ;Ϊ0, ѭ��
    DJNZ    RX1_TimeOut, L_MainLoop ;-1,��Ϊ0ѭ��

    MOV     A, RX1_Cnt
    CLR     C
    SUBB    A, #12
    JC      L_Jump_ProcessErr   ; RX1_Cnt < 12 bytes, jump

    MOV     R0, #RX1_Buffer
L_CheckCharLoop:                ; Сдת��д
    MOV     A, @R0
    CLR     C
    SUBB    A, #'a'
    JC      L_CheckLessThan_a   ;if(RX1_Buffer[i] < 'a'), jump
    MOV     A, @R0
    CLR     C
    SUBB    A, #('z'+1)
    JNC     L_CheckLargeThan_z  ;if(RX1_Buffer[i] > 'z'), jump
    MOV     A, @R0
    ADD     A, #('A' - 'a')     ; Сдת��д
    MOV     @R0, A
L_CheckLessThan_a:
L_CheckLargeThan_z:
    INC     R0
    CJNE    R0, #(RX1_Buffer+10), L_CheckCharLoop
    SJMP    L_CalculateAddr
    
L_Jump_ProcessErr:
    LJMP    L_Rx2ProcessErr

L_CalculateAddr:    
    LCALL   F_GetAddress            ; �����ַ
    JB      F0, L_Jump_ProcessErr   ; ��ַ����

    MOV     R0, #RX1_Buffer+10
    CJNE    @R0, #' ', L_Jump_ProcessErr    ;��������Ƿ���ȷ   RX1_Buffer[10] = ' '

    MOV     R0, #RX1_Buffer
    CJNE    @R0, #'R', L_CMD_Not_R  ;��������Ƕ�����   RX1_Buffer[0] = 'R'
    LCALL   F_GetReadDataLength
    JB      F0, L_Jump_ProcessErr   ;���ȴ���
    MOV     A, length
    JZ      L_Jump_ProcessErr   ; ���ȴ���
    CLR     C
    SUBB    A, #RX1_Lenth+1
    JNC     L_Jump_ProcessErr   ; ���ȴ���
    
    MOV     R0, #RX1_Buffer
    LCALL   F_EEPROM_read_n     ;address_H,address_L,R0,length
    MOV     DPTR, #TestString2  ;"����"
    LCALL   F_SendString1
    MOV     A, length
    MOV     B, #100
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;���ͳ��� ��λ
    MOV     A, #10
    XCH     A, B
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;���ͳ��� ʮλ
    XCH     A,B
    ADD     A, #'0'
    LCALL   F_SendByte      ;���ͳ��� ��λ
    MOV     DPTR, #TestString3  ;"���ֽ��������£�\r\n"
    LCALL   F_SendString1
    MOV     R2, length
    MOV     R0, #RX1_Buffer
L_TxDataLoop:
    MOV     A, @R0
    LCALL   F_SendByte      ;��������
    INC     R0
    DJNZ    R2, L_TxDataLoop
    MOV     A, #0DH
    LCALL   F_SendByte  ;�س�
    MOV     A, #0AH
    LCALL   F_SendByte  ;����
    LJMP    L_QuitRx2Process

L_CMD_Not_R:
    MOV     R0, #RX1_Buffer
    CJNE    @R0, #'W', L_CMD_Not_W  ;���������д����   RX1_Buffer[0] = 'W'
    MOV     A, RX1_Cnt
    CLR     C
    SUBB    A, #11
    MOV     length, A
    MOV     R0, #RX1_Buffer+11
    LCALL   F_EEPROM_SectorErase    ; ����һ������
    LCALL   F_EEPROM_write_n        ; дN���ֽڲ�У��   ;address_H,address_L,R0,length
    JB      F0, L_WriteError        ; д�����, ת
    MOV     DPTR, #TestString4      ; "��д��"
    LCALL   F_SendString1
    MOV     A, length
    MOV     B, #100
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;д�볤�� ��λ
    MOV     A, #10
    XCH     A, B
    DIV     AB
    ADD     A, #'0'
    LCALL   F_SendByte      ;д�볤�� ʮλ
    XCH     A,B
    ADD     A, #'0'
    LCALL   F_SendByte      ;д�볤�� ��λ
    MOV     DPTR, #TestString5      ;"�ֽ�����!\r\n"
    LCALL   F_SendString1
    SJMP    L_QuitRx2Process

L_WriteError:
    MOV     DPTR, #TestString6      ;"д�����!\r\n"
    LCALL   F_SendString1
    SJMP    L_QuitRx2Process

L_AddrNotEmpty:
    MOV     DPTR, #TestString7      ;"Ҫд��ĵ�ַΪ�ǿ�,����д��,���Ȳ���!\r\n"
    LCALL   F_SendString1
    SJMP    L_QuitRx2Process

L_CMD_Not_W:
L_Rx2ProcessErr:
    MOV     DPTR, #TestString7      ;"�������!\r\n"
    LCALL   F_SendString1

L_QuitRx2Process:
    MOV     RX1_Cnt, #0     ;   //����ֽ���

    LJMP    L_MainLoop
;===================================================================


;**********************************************

;========================================================================
; ����: F_CheckData
; ����: ASCII������'0'~'9','A'~'F'ת��ʮ�����Ƶ����� 0~9, A~F.
; ����: ACC: Ҫת�������ݡ�
; ����: F0 = 0 ��ȷ,  F0 = 1 ����.
; �汾: V1.0, 2014-1-22
;========================================================================
F_CheckData:
    CLR     F0  ;0--��ȷ, 1--����
    MOV     R7, A
    CLR     C
    SUBB    A, #'0'
    JC      L_CheckDataErr  ; < '0'
    MOV     A, R7
    CLR     C
    SUBB    A, #'9'+1
    JNC     L_CheckDataChar ; > '9'
    MOV     A, R7
    CLR     C
    SUBB    A, #'0'
    RET
L_CheckDataChar:
    MOV     A,R7
    CLR     C
    SUBB    A, #'A'
    JC      L_CheckDataErr  ; < 'A'
    MOV     A, R7
    CLR     C
    SUBB    A, #'F'+1
    JNC     L_CheckDataChar ; > 'F'
    MOV     A, R7
    CLR     C
    SUBB    A, #'A'-10
    RET

L_CheckDataErr:
    SETB    F0  ;0--��ȷ, 1--����
    RET

;========================================================================
; ����: F_GetAddress
; ����: ��ȡҪ������EEPROM���׵�ַ.
; ����: non.
; ����: F0 = 0 ��ȷ,  F0 = 1 ����.
; �汾: V1.0, 2014-1-22
;========================================================================
F_GetAddress:
    MOV     address_E, #0
    MOV     address_H, #0
    MOV     address_L, #0

    MOV     R0, #RX1_Buffer+2
    CJNE    @R0, #'0', L_AddrError  ;����ַ�Ƿ�0X��ͷ
    INC     R0
    CJNE    @R0, #'X', L_AddrError

    MOV     R0, #RX1_Buffer+4
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_E, A
    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_E
    MOV     address_E, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_H, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_H
    MOV     address_H, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    SWAP    A
    MOV     address_L, A

    INC     R0
    MOV     A, @R0
    LCALL   F_CheckData
    JB      F0, L_AddrError
    ORL     A, address_L
    MOV     address_L, A
    CLR     F0
    RET
L_AddrError:
    SETB    F0
    RET

;========================================================================
; ����: F_CheckNumber
; ����: ��ȡҪ�������ݵ��ֽ���.
; ����: non.
; ����: F0 = 0 ��ȷ,  F0 = 1 ����.
; �汾: V1.0, 2014-1-22
;========================================================================
F_CheckNumber:
    CLR     F0  ;0--��ȷ, 1--����
    MOV     R7, A
    CLR     C
    SUBB    A, #'0'
    JC      L_CheckNumberErr    ; < '0'
    MOV     A, R7
    CLR     C
    SUBB    A, #'9'+1
    JNC     L_CheckNumberErr    ; > '9'
    MOV     A, R7
    CLR     C
    SUBB    A, #'0'
    RET
L_CheckNumberErr:
    SETB    F0  ;0--��ȷ, 1--����
    RET

F_GetReadDataLength:
    CLR     F0  ;0--��ȷ, 1--����
    
    MOV     length, #0
    MOV     R2, #11
    MOV     R0, #RX1_Buffer+11
L_GetReadDataLengthLoop:    
    MOV     A, length
    MOV     B, #10
    MUL     AB
    MOV     length, A
    MOV     A, @R0
    LCALL   F_CheckNumber
    JB      F0, L_GetReadDataLengthErr
    ADD     A, length
    MOV     length, A
    INC     R0
    INC     R2
    MOV     A, R2
    CJNE    A, RX1_Cnt, L_GetReadDataLengthLoop
    RET
L_GetReadDataLengthErr:
    RET


TestString1:
    DB  "AI8051Uϵ�е�Ƭ��EEPROM���Գ���!",0DH,0AH,0
TestString1A:
    DB  "W 0x000040 1234567890  --> ��0x000040��ַд���ַ�1234567890.",0DH,0AH,0
TestString1B:
    DB  "R 0x000040 10          --> ��0x000040��ַ����10���ֽ�����.",0DH,0AH,0
TestString2:
    DB  "����",0
TestString3:
    DB  "���ֽ���������:",0DH,0AH,0
TestString4:
    DB  "��д��",0
TestString5:
    DB  "�ֽ�����!",0DH,0AH,0
TestString6:
    DB  "д�����!",0DH,0AH,0
TestString7:
    DB  "�������!",0DH,0AH,0

F_SendByte:
    SETB    B_TX1_Busy      ;��־����æ
    MOV     SBUF, A        ;����һ���ֽ�
    JB      B_TX1_Busy, $   ;�ȴ��������
    RET

;========================================================================
; ����: F_SendString1
; ����: ����2�����ַ���������
; ����: DPTR: �ַ����׵�ַ.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_SendString1:
    CLR     A
    MOVC    A, @A+DPTR      ;Get current char
    JZ      L_SendEnd       ;Check the end of the string
    LCALL   F_SendByte      ;����һ���ֽ�
    INC     DPTR            ;increment string ptr
    SJMP    F_SendString1       ;Check next
L_SendEnd:
    RET

;========================================================================
; ����: F_SetTimer2Baudraye
; ����: ����Timer2�������ʷ�������
; ����: DPTR: Timer2����װֵ.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_SetTimer2Baudraye:    ; ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
    ANL     AUXR, #NOT (1 SHL 4)    ; Timer stop    ������ʹ��Timer2����
    ANL     AUXR, #NOT (1 SHL 3)    ; Timer2 set As Timer
    ORL     AUXR, #(1 SHL 2)        ; Timer2 set as 1T mode
    MOV     T2H, DPH
    MOV     T2L, DPL
    ANL     IE2, #NOT (1 SHL 2)     ; ��ֹ�ж�
    ORL     AUXR, #(1 SHL 4)        ; Timer run enable
    RET

;========================================================================
; ����: F_UART1_config
; ����: UART1��ʼ��������
; ����: ACC: ѡ������, 2: ʹ��Timer2��������, ����ֵ: ʹ��Timer1��������.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_UART1_config:
    CJNE    A, #2, L_Uart1NotUseTimer2
    ORL     AUXR, #0x01     ; S1 BRT Use Timer2;
    MOV     DPTR, #UART1_Baudrate
    LCALL   F_SetTimer2Baudraye
    SJMP    L_SetupUart1

L_Uart1NotUseTimer2:
    CLR     TR1                 ; Timer Stop    ������ʹ��Timer1����
    ANL     AUXR, #NOT 0x01     ; S1 BRT Use Timer1;
    ORL     AUXR, #(1 SHL 6)    ; Timer1 set as 1T mode
    ANL     TMOD, #NOT (1 SHL 6); Timer1 set As Timer
    ANL     TMOD, #NOT 0x30     ; Timer1_16bitAutoReload;
    MOV     TH1, #HIGH UART1_Baudrate
    MOV     TL1, #LOW  UART1_Baudrate
    CLR     ET1                 ; ��ֹ�ж�
    ANL     INTCLKO, #NOT 0x02  ; �����ʱ��
    SETB    TR1

L_SetupUart1:
    ANL     SCON, #0x3f
    ORL     SCON, #0x40     ; UART1ģʽ, 0x00: ͬ����λ���, 0x40: 8λ����,�ɱ䲨����, 0x80: 9λ����,�̶�������, 0xc0: 9λ����,�ɱ䲨����
;   SETB    PS      ; �����ȼ��ж�
    SETB    REN     ; �������
    SETB    ES      ; �����ж�

    ANL     P_SW1, #0x3f
    ORL     P_SW1, #0x00        ; UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
;   ORL     PCON2, #(1 SHL 4)   ; �ڲ���·RXD��TXD, ���м�, ENABLE,DISABLE

    CLR     B_TX1_Busy
    MOV     RX1_Cnt, #0;
    MOV     TX1_Cnt, #0;
    MOV     RX1_TimeOut, #0
    RET

;========================================================================
; ����: F_UART1_Interrupt
; ����: UART1�жϺ�����
; ����: nine.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_UART1_Interrupt:
    PUSH    PSW
    PUSH    ACC
    PUSH    AR0
    
    JNB     RI, L_QuitUartRx
    CLR     RI
    MOV     RX1_TimeOut, #5
    MOV     A, #RX1_Buffer
    ADD     A, RX1_Cnt
    MOV     R0, A
    MOV     @R0, SBUF  ;����һ���ֽ�
    INC     RX1_Cnt
    MOV     A, RX1_Cnt
    CJNE    A, #RX1_Lenth, L_QuitUartRx
    MOV     RX1_Cnt, #0     ; �����������
L_QuitUartRx:

    JNB     TI, L_QuitUartTx
    CLR     TI
    CLR     B_TX1_Busy      ; �������æ��־
L_QuitUartTx:

    POP     AR0
    POP     ACC
    POP     PSW
    RETI


;========================================================================
; ����: F_delay_ms
; ����: ��ʱ�ӳ���
; ����: ACC: ��ʱms��.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
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


;========================================================================
; ����: F_DisableEEPROM
; ����: ��ֹ����ISP/IAP.
; ����: non.
; ����: non.
; �汾: V1.0, 2012-10-22
;========================================================================
F_DisableEEPROM:
    MOV     IAP_CONTR, #0       ; ��ֹISP/IAP����
    MOV     IAP_CMD,  #0        ; ȥ��ISP/IAP����
    MOV     IAP_TRIG, #0        ; ��ֹISP/IAP�����󴥷�
    MOV     IAP_ADDRE, #0FFH    ; ��0��ַ���ֽ�
    MOV     IAP_ADDRH, #0FFH    ; ��0��ַ���ֽ�
    MOV     IAP_ADDRL, #0FFH    ; ��0��ַ���ֽڣ�ָ���EEPROM������ֹ�����
    RET

;========================================================================
; ����: F_EEPROM_Trig
; ����: ����EEPROM����.
; ����: none.
; ����: none.
; �汾: V1.0, 2014-6-30
;========================================================================
F_EEPROM_Trig:
    MOV     C, EA
    MOV     F0, C           ;����ȫ���ж�
    CLR     EA              ;��ֹ�ж�, ���ⴥ��������Ч
    MOV     IAP_TRIG, #0x5A ;����5AH������A5H��ISP/IAP�����Ĵ�����ÿ�ζ���Ҫ���
                            ;����A5H��ISP/IAP������������������.
                            ;CPU�ȴ�IAP��ɺ󣬲Ż����ִ�г���.
    MOV     IAP_TRIG, #0xA5
    NOP     ;�༶��ˮ�ߵ�ָ��ϵͳ��������������4��NOP����֤IAP_DATA���������׼��
    NOP
    NOP
    NOP
    MOV     C, F0
    MOV     EA, C       ;�ָ�ȫ���ж�
    RET

;========================================================================
; ����: F_EEPROM_read_n
; ����: ��ָ��EEPROM�׵�ַ����n���ֽڷ�ָ���Ļ���.
; ����: address_H,address_L:  ����EEPROM���׵�ַ.
;       R0:                   �������ݷŻ�����׵�ַ.
;       length:               �������ֽڳ���.
; ����: non.
; �汾: V1.0, 2012-10-22
;========================================================================

F_EEPROM_read_n:
    PUSH    AR2

    MOV     R2, length
    MOV     IAP_ADDRE, address_E    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRH, address_H    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRL, address_L    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_CONTR, #IAP_EN      ; ����ISP/IAP��������һ�ξ͹�
	MOV     IAP_TPS, #TPS_WAIT      ; ���õȴ�ʱ�䣬
    MOV     IAP_CMD, #1             ; ISP�����������ֽڶ���������ı�ʱ����������������
L_EE_Read_Loop:
    LCALL   F_EEPROM_Trig           ; ����EEPROM����
    MOV     @R0, IAP_DATA

    MOV     A, IAP_ADDRL            ; ��ַ +1
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_Read_Loop

    LCALL   F_DisableEEPROM
    POP     AR2
    RET


;========================================================================
; ����: F_EEPROM_SectorErase
; ����: ��ָ����ַ��EEPROM��������.
; ����: address_H,address_L:  Ҫ����������EEPROM�ĵ�ַ.
; ����: non.
; �汾: V1.0, 2013-5-10
;========================================================================
F_EEPROM_SectorErase:
                                    ; ֻ������������û���ֽڲ�����512�ֽ�/������
                                    ; ����������һ���ֽڵ�ַ����������ַ��
    MOV     IAP_ADDRE, address_E    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRH, address_H    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRL, address_L    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_CONTR, #IAP_EN      ; ����ISP/IAP��������һ�ξ͹�
	MOV     IAP_TPS, #TPS_WAIT      ; ���õȴ�ʱ�䣬
    MOV     IAP_CMD, #3             ; ��������������
    LCALL   F_EEPROM_Trig           ; ����EEPROM����
    LCALL   F_DisableEEPROM         ; ��ֹEEPROM����
    RET

;========================================================================
; ����: F_EEPROM_write_n
; ����: �ѻ����n���ֽ�д��ָ���׵�ַ��EEPROM, ����У��.
; ����: address_H,address_L:    д��EEPROM���׵�ַ.
;       R0:                     д��Դ���ݵĻ�����׵�ַ.
;       length:                 д����ֽڳ���.
; ����: F0 == 0, д����ȷ,  F0 == 1, д�����.
; �汾: V1.0, 2014-1-22
;========================================================================
F_EEPROM_write_n:
    PUSH    AR2
    PUSH    AR0
    MOV     R2, length
    MOV     IAP_ADDRE, address_E    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRH, address_H    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRL, address_L    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_CONTR, #IAP_EN      ; ����ISP/IAP��������һ�ξ͹�
	MOV     IAP_TPS, #TPS_WAIT      ; ���õȴ�ʱ�䣬
    MOV     IAP_CMD, #2             ; ���ֽ�д��������ı�ʱ����������������
L_EE_W_Loop:
    MOV     IAP_DATA, @R0           ; �����ݵ�IAP_DATA��ֻ�����ݸı�ʱ����������
    LCALL   F_EEPROM_Trig           ; ����EEPROM����
    MOV     A, IAP_ADDRL            ; ��ַ +1
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_W_Loop

    POP     AR0
    MOV     R2, length              ; д���Ƚ�
    MOV     IAP_ADDRE, address_E    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRH, address_H    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_ADDRL, address_L    ; �͵�ַ���ֽڣ���ַ��Ҫ�ı�ʱ���������͵�ַ��
    MOV     IAP_CONTR, #IAP_EN      ; ����ISP/IAP��������һ�ξ͹�
	MOV     IAP_TPS, #TPS_WAIT      ; ���õȴ�ʱ�䣬
    MOV     IAP_CMD, #1             ; ISP�����������ֽڶ���������ı�ʱ����������������
L_EE_Compare_Loop:
    LCALL   F_EEPROM_Trig           ; ����EEPROM����
    MOV     A, IAP_DATA
    XRL     A, @R0
    JNZ     L_EE_CompareErr

    MOV     A, IAP_ADDRL    ;
    ADD     A, #1
    MOV     IAP_ADDRL, A
    MOV     A, IAP_ADDRH
    ADDC    A, #0
    MOV     IAP_ADDRH, A
    MOV     A, IAP_ADDRE
    ADDC    A, #0
    MOV     IAP_ADDRE, A
    INC     R0
    DJNZ    R2, L_EE_Compare_Loop

    LCALL   F_DisableEEPROM
    CLR     F0
    POP     AR2
    RET

L_EE_CompareErr:
    LCALL   F_DisableEEPROM
    SETB    F0
    POP     AR2
    RET


    END

