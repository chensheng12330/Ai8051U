;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  ����˵��    **************

;�����̻���AI8051UΪ����оƬ��ʵ������б�д���ԡ�

;ʹ��Keil C251��������Memory Model�Ƽ�����XSmallģʽ��Ĭ�϶��������edata����ʱ�Ӵ�ȡ�����ٶȿ졣

;edata���鱣��1K����ջʹ�ã��ռ䲻��ʱ�ɽ������顢�����ñ�����xdata�ؼ��ֶ��嵽xdata�ռ䡣

;ͨ��Ӳ��I2C�ӿڶ�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

;����ȡ�����ݼ�1��д��AT24C02ǰ8���ֽ�.

;���¶�ȡAT24C02ǰ8���ֽ����ݣ�ͨ�����ڴ�ӡ��ȡ���.

;MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

;��������UART1(P3.0,P3.1): 115200,N,8,1��ʹ��HEXģʽ��ӡ����.

;MCU�ϵ��ִ��1�����϶��������ظ��ϵ�/�ϵ����AT24C02ǰ8���ֽڵ���������.

;����ʱ, ѡ��ʱ�� 24MHz (�û��������޸�Ƶ��).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** �û������ ***********************************/

Fosc_KHZ        EQU     24000   ;24000KHz

STACK_POIRTER   EQU     0D0H    ;��ջ��ʼ��ַ

;*******************************************************************

SLAW    EQU     0xA0
SLAR    EQU     0xA1

;*******************************************************************

;/****************************** �û������ ***********************************/

UART1_Baudrate EQU     (-52)   ;115200bps @ 24MHz      UART1_Baudrate = 65536UL - ((Fosc_KHZ / 4) / Baudrate)

;*************  ���ر�������    **************/

EEPROM      DATA    30H     ; �洢���� 30H ~ 37H

;*******************************************************************
;*******************************************************************
        ORG     0000H       ;����λ��ڣ��������Զ����嵽 0FF0000H ��ַ
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
    MOV     PSW, #0     ;ѡ���0��R0~R7
    USING   0       ;ѡ���0��R0~R7

;================= �û���ʼ������ ====================================

    MOV     A, #1
    LCALL   F_UART1_config
    LCALL   F_I2C_Init
    SETB    EA          ; �����ж�

    LCALL   F_ReadEEP   ;��24Cxx
    LCALL   F_WriteEEP  ;д24Cxx
    MOV     A, #250
    LCALL   F_delay_ms
    MOV     A, #250
    LCALL   F_delay_ms
    LCALL   F_ReadEEP   ;��24Cxx

;=================== ��ѭ�� ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;========================================================================
; ����: F_I2C_Init
; ����: I2C��ʼ������.
; ����: none
; ����: none.
; �汾: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #030H        ;I2C���ܽ�ѡ��00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #0C2H            ;ʹ��I2C����ģʽ
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     @DR4, R11
    RET

;========================================================================
; ����: F_SendData
; ����: ����1����һ���ֽ����ݺ�����
; ����: ��Ҫ���͵�����.
; ����: none.
; �汾: VER1.0
; ����: 2014-11-28
; ��ע: 
;========================================================================
F_SendData:
    MOV     SBUF, A     ;����һ���ֽ�
    JNB     TI, $       ;�ȴ��������
    CLR     TI
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
;   SETB    REN     ; �������
;   SETB    ES      ; �����ж�

    ANL     P_SW1, #0x3f
    ORL     P_SW1, #0x00        ; UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    RET

;************ I2C��غ��� ****************
;========================================================================
; ����: Wait
; ����: I2C������ʱ.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Wait:
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     R11, @DR4
    JNB     ACC.6,Wait
    ANL     A,#NOT 40H
    MOV     @DR4,R11
    RET

;========================================================================
; ����: Start
; ����: ����I2C. 
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Start:
    MOV     A, #01H
    MOV     WR6, #WORD0 I2CMSCR  ;����START����
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; ����: Stop
; ����: ֹͣI2C. 
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
Stop:
    MOV     A, #06H
    MOV     WR6, #WORD0 I2CMSCR  ;����STOP����
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; ����: SendData
; ����: ��������.
; ����: I2CTXD -> A.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendData:
    MOV     WR6,#WORD0 I2CTXD   ;д���ݵ����ݻ�����
    MOV     WR4,#WORD2 I2CTXD
    MOV     @DR4,R11
    MOV     A,#00000010B        ;����SEND����
    MOV     WR6,#WORD0 I2CMSCR
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: RecvACK
; ����: ���Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
RecvACK:
    MOV     A,#03H
    MOV     WR6,#WORD0 I2CMSCR  ;���Ͷ�ACK����
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    CLR     C
    MOV     WR6,#WORD0 I2CMSST  ;���Ͷ�ACK����
    MOV     WR4,#WORD2 I2CMSST
    MOV     R11,@DR4
    JNB     ACC.1,$+4
    SETB    C
    RET
        
;========================================================================
; ����: RecvData
; ����: ��������.
; ����: none.
; ����: I2CRXD -> A.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
RecvData:
    MOV     A,#04H
    MOV     WR6,#WORD0 I2CMSCR  ;����RECV����
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    MOV     WR6,#WORD0 I2CRXD
    MOV     WR4,#WORD2 I2CRXD
    MOV     R11,@DR4    ;�����ݻ�������ȡ����
    RET

;========================================================================
; ����: SendACK
; ����: ����Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendACK:
    MOV     A,#00H
    MOV     WR6,#WORD0 I2CMSST  ;����ACK�ź�
    MOV     WR4,#WORD2 I2CMSST
    MOV     @DR4,R11
    MOV     A,#05H
    MOV     WR6,#WORD0 I2CMSCR  ;����ACK����
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: SendNAK
; ����: ���ͷ�Ӧ��.
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
;========================================================================
SendNAK:
    MOV     A,#01H
    MOV     WR6,#WORD0 I2CMSST  ;����NAK�ź�
    MOV     WR4,#WORD2 I2CMSST
    MOV     @DR4,R11
    MOV     A,#05H
    MOV     WR6,#WORD0 I2CMSCR  ;����ACK����
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; ����: F_WriteNbyte
; ����: дN���ֽ��ӳ���
; ����: R2: дI2C�����׵�ַ,  R0: д�����ݴ���׵�ַ,  R3: д���ֽ���
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_WriteNbyte:
    LCALL   Start
    MOV     A, #SLAW
    LCALL   SendData
    LCALL   RecvACK
    JC      L_WriteN_StopI2C

    MOV     A, R2
    LCALL   SendData
    LCALL   RecvACK
    JC      L_WriteN_StopI2C

L_WriteNbyteLoop:
    MOV     A, @R0
    LCALL   SendData
    INC     R0
    LCALL   RecvACK
    JC      L_WriteN_StopI2C
    DJNZ    R3, L_WriteNbyteLoop 
L_WriteN_StopI2C:
    LCALL   Stop
    RET

;========================================================================
; ����: F_ReadNbyte
; ����: ��N���ֽ��ӳ���
; ����: R2: ��I2C�����׵�ַ,  R0: �������ݴ���׵�ַ,  R3: �����ֽ���
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: ����ACCC��PSW��, ���õ���ͨ�üĴ�������ջ
;========================================================================
F_ReadNbyte:
    LCALL   Start
    MOV     A, #SLAW
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    MOV     A, R2
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    LCALL   Start
    MOV     A, #SLAR
    LCALL   SendData
    LCALL   RecvACK
    JC      L_ReadN_StopI2C

    MOV     A, R3
    ANL     A, #0xfe    ;�ж��Ƿ����1
    JZ      L_ReadLastByte
    DEC     R3          ;����1�ֽ�, ��-1
L_ReadNbyteLoop:
    LCALL   RecvData    ;*p = I2C_ReadAbyte();  p++;
    MOV     @R0, A
    INC     R0
    LCALL   SendACK     ;send ACK
    DJNZ    R3, L_ReadNbyteLoop 
L_ReadLastByte:
    LCALL   RecvData    ;*p = I2C_ReadAbyte()
    MOV     @R0, A
    LCALL   SendNAK     ;send no ACK
L_ReadN_StopI2C:
    LCALL   Stop
    RET

;========================================================================
; ����: F_ReadEEP
; ����: ��24Cxx������
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: 
;========================================================================
F_ReadEEP:
    MOV     R2, #0      ; ��I2C�����׵�ַ
    MOV     R0, #EEPROM ; �������ݴ���׵�ַ
    MOV     R3, #8      ; �����ֽ���
    LCALL   F_ReadNbyte ; R2: ��I2C�����׵�ַ,  R0: �������ݴ���׵�ַ,  R3: �����ֽ���

    MOV     A,EEPROM+0
    LCALL   F_SendData
    MOV     A,EEPROM+1
    LCALL   F_SendData
    MOV     A,EEPROM+2
    LCALL   F_SendData
    MOV     A,EEPROM+3
    LCALL   F_SendData
    MOV     A,EEPROM+4
    LCALL   F_SendData
    MOV     A,EEPROM+5
    LCALL   F_SendData
    MOV     A,EEPROM+6
    LCALL   F_SendData
    MOV     A,EEPROM+7
    LCALL   F_SendData
;    MOV     A,#0DH
;    LCALL   F_SendData
;    MOV     A,#0AH
;    LCALL   F_SendData
    RET

;========================================================================
; ����: F_WriteEEP
; ����: д24Cxx������
; ����: none.
; ����: none.
; �汾: VER1.0
; ����: 2013-4-1
; ��ע: 
;========================================================================
F_WriteEEP:
    INC     EEPROM+0
    INC     EEPROM+1
    INC     EEPROM+2
    INC     EEPROM+3
    INC     EEPROM+4
    INC     EEPROM+5
    INC     EEPROM+6
    INC     EEPROM+7

    ANL     EEPROM+0, #0FH
    ANL     EEPROM+1, #0FH
    ANL     EEPROM+2, #0FH
    ANL     EEPROM+3, #0FH
    ANL     EEPROM+4, #0FH
    ANL     EEPROM+5, #0FH
    ANL     EEPROM+6, #0FH
    ANL     EEPROM+7, #0FH

    MOV     R2, #0      ;дI2C�����׵�ַ
    MOV     R0, #EEPROM ;д�����ݴ���׵�ַ
    MOV     R3, #8      ;д���ֽ���
    LCALL   F_WriteNbyte    ;
    RET

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

    POP     04H     ;��ջR4
    POP     03H     ;��ջR3
    POP     02H     ;��ջR2
    RET



    END

