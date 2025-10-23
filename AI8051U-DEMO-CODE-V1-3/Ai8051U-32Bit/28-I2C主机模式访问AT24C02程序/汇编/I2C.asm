;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;通过硬件I2C接口读取AT24C02前8个字节数据，通过串口打印读取结果.

;将读取的数据加1后写回AT24C02前8个字节.

;重新读取AT24C02前8个字节数据，通过串口打印读取结果.

;MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

;串口配置UART1(P3.0,P3.1): 115200,N,8,1，使用HEX模式打印数据.

;MCU上电后执行1次以上动作，可重复断电/上电测试AT24C02前8个字节的数据内容.

;下载时, 选择时钟 24MHz (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ        EQU     24000   ;24000KHz

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

;*******************************************************************

SLAW    EQU     0xA0
SLAR    EQU     0xA1

;*******************************************************************

;/****************************** 用户定义宏 ***********************************/

UART1_Baudrate EQU     (-52)   ;115200bps @ 24MHz      UART1_Baudrate = 65536UL - ((Fosc_KHZ / 4) / Baudrate)

;*************  本地变量声明    **************/

EEPROM      DATA    30H     ; 存储缓冲 30H ~ 37H

;*******************************************************************
;*******************************************************************
        ORG     0000H       ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
        ORG     0100H       ;编译器自动定义到 0FF0100H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #00H     ;设置为准双向口
    MOV     P0M0, #00H
    MOV     P1M1, #00H     ;设置为准双向口
    MOV     P1M0, #00H
    MOV     P2M1, #00H     ;设置为准双向口
    MOV     P2M0, #00H
    MOV     P3M1, #00H     ;设置为准双向口
    MOV     P3M0, #00H
    MOV     P4M1, #00H     ;设置为准双向口
    MOV     P4M0, #00H
    MOV     P5M1, #00H     ;设置为准双向口
    MOV     P5M0, #00H
    MOV     P6M1, #00H     ;设置为准双向口
    MOV     P6M0, #00H
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H
    
    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0     ;选择第0组R0~R7
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    MOV     A, #1
    LCALL   F_UART1_config
    LCALL   F_I2C_Init
    SETB    EA          ; 打开总中断

    LCALL   F_ReadEEP   ;读24Cxx
    LCALL   F_WriteEEP  ;写24Cxx
    MOV     A, #250
    LCALL   F_delay_ms
    MOV     A, #250
    LCALL   F_delay_ms
    LCALL   F_ReadEEP   ;读24Cxx

;=================== 主循环 ==================================
L_Main_Loop:

    LJMP    L_Main_Loop

;========================================================================
; 函数: F_I2C_Init
; 描述: I2C初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2021-3-4
;========================================================================
F_I2C_Init:
    ORL     P_SW2, #030H        ;I2C功能脚选择，00H:P1.5,P1.4; 10H:P2.5,P2.4; 30H:P3.2,P3.3

    MOV     A, #0C2H            ;使能I2C主机模式
    MOV     WR6, #WORD0 I2CCFG
    MOV     WR4, #WORD2 I2CCFG
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 I2CMSST
    MOV     WR4, #WORD2 I2CMSST
    MOV     @DR4, R11
    RET

;========================================================================
; 函数: F_SendData
; 描述: 串口1发送一个字节数据函数。
; 参数: 需要发送的数据.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SendData:
    MOV     SBUF, A     ;发送一个字节
    JNB     TI, $       ;等待发送完成
    CLR     TI
    RET

;========================================================================
; 函数: F_SetTimer2Baudraye
; 描述: 设置Timer2做波特率发生器。
; 参数: DPTR: Timer2的重装值.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_SetTimer2Baudraye:    ; 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    ANL     AUXR, #NOT (1 SHL 4)    ; Timer stop    波特率使用Timer2产生
    ANL     AUXR, #NOT (1 SHL 3)    ; Timer2 set As Timer
    ORL     AUXR, #(1 SHL 2)        ; Timer2 set as 1T mode
    MOV     T2H, DPH
    MOV     T2L, DPL
    ANL     IE2, #NOT (1 SHL 2)     ; 禁止中断
    ORL     AUXR, #(1 SHL 4)        ; Timer run enable
    RET

;========================================================================
; 函数: F_UART1_config
; 描述: UART1初始化函数。
; 参数: ACC: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
; 返回: none.
; 版本: VER1.0
; 日期: 2014-11-28
; 备注: 
;========================================================================
F_UART1_config:
    CJNE    A, #2, L_Uart1NotUseTimer2
    ORL     AUXR, #0x01     ; S1 BRT Use Timer2;
    MOV     DPTR, #UART1_Baudrate
    LCALL   F_SetTimer2Baudraye
    SJMP    L_SetupUart1

L_Uart1NotUseTimer2:
    CLR     TR1                 ; Timer Stop    波特率使用Timer1产生
    ANL     AUXR, #NOT 0x01     ; S1 BRT Use Timer1;
    ORL     AUXR, #(1 SHL 6)    ; Timer1 set as 1T mode
    ANL     TMOD, #NOT (1 SHL 6); Timer1 set As Timer
    ANL     TMOD, #NOT 0x30     ; Timer1_16bitAutoReload;
    MOV     TH1, #HIGH UART1_Baudrate
    MOV     TL1, #LOW  UART1_Baudrate
    CLR     ET1                 ; 禁止中断
    ANL     INTCLKO, #NOT 0x02  ; 不输出时钟
    SETB    TR1

L_SetupUart1:
    ANL     SCON, #0x3f
    ORL     SCON, #0x40     ; UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
;   SETB    PS      ; 高优先级中断
;   SETB    REN     ; 允许接收
;   SETB    ES      ; 允许中断

    ANL     P_SW1, #0x3f
    ORL     P_SW1, #0x00        ; UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    RET

;************ I2C相关函数 ****************
;========================================================================
; 函数: Wait
; 描述: I2C访问延时.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
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
; 函数: Start
; 描述: 启动I2C. 
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
Start:
    MOV     A, #01H
    MOV     WR6, #WORD0 I2CMSCR  ;发送START命令
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; 函数: Stop
; 描述: 停止I2C. 
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
Stop:
    MOV     A, #06H
    MOV     WR6, #WORD0 I2CMSCR  ;发送STOP命令
    MOV     WR4, #WORD2 I2CMSCR
    MOV     @DR4, R11
    LCALL   Wait
    RET

;========================================================================
; 函数: SendData
; 描述: 发送数据.
; 参数: I2CTXD -> A.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendData:
    MOV     WR6,#WORD0 I2CTXD   ;写数据到数据缓冲区
    MOV     WR4,#WORD2 I2CTXD
    MOV     @DR4,R11
    MOV     A,#00000010B        ;发送SEND命令
    MOV     WR6,#WORD0 I2CMSCR
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: RecvACK
; 描述: 检测应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
RecvACK:
    MOV     A,#03H
    MOV     WR6,#WORD0 I2CMSCR  ;发送读ACK命令
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    CLR     C
    MOV     WR6,#WORD0 I2CMSST  ;发送读ACK命令
    MOV     WR4,#WORD2 I2CMSST
    MOV     R11,@DR4
    JNB     ACC.1,$+4
    SETB    C
    RET
        
;========================================================================
; 函数: RecvData
; 描述: 接收数据.
; 参数: none.
; 返回: I2CRXD -> A.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
RecvData:
    MOV     A,#04H
    MOV     WR6,#WORD0 I2CMSCR  ;发送RECV命令
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    MOV     WR6,#WORD0 I2CRXD
    MOV     WR4,#WORD2 I2CRXD
    MOV     R11,@DR4    ;从数据缓冲区读取数据
    RET

;========================================================================
; 函数: SendACK
; 描述: 发送应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendACK:
    MOV     A,#00H
    MOV     WR6,#WORD0 I2CMSST  ;设置ACK信号
    MOV     WR4,#WORD2 I2CMSST
    MOV     @DR4,R11
    MOV     A,#05H
    MOV     WR6,#WORD0 I2CMSCR  ;发送ACK命令
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: SendNAK
; 描述: 发送非应答.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
SendNAK:
    MOV     A,#01H
    MOV     WR6,#WORD0 I2CMSST  ;设置NAK信号
    MOV     WR4,#WORD2 I2CMSST
    MOV     @DR4,R11
    MOV     A,#05H
    MOV     WR6,#WORD0 I2CMSCR  ;发送ACK命令
    MOV     WR4,#WORD2 I2CMSCR
    MOV     @DR4,R11
    LCALL   Wait
    RET

;========================================================================
; 函数: F_WriteNbyte
; 描述: 写N个字节子程序。
; 参数: R2: 写I2C数据首地址,  R0: 写入数据存放首地址,  R3: 写入字节数
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
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
; 函数: F_ReadNbyte
; 描述: 读N个字节子程序。
; 参数: R2: 读I2C数据首地址,  R0: 读出数据存放首地址,  R3: 读出字节数
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
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
    ANL     A, #0xfe    ;判断是否大于1
    JZ      L_ReadLastByte
    DEC     R3          ;大于1字节, 则-1
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
; 函数: F_ReadEEP
; 描述: 读24Cxx函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 
;========================================================================
F_ReadEEP:
    MOV     R2, #0      ; 读I2C数据首地址
    MOV     R0, #EEPROM ; 读出数据存放首地址
    MOV     R3, #8      ; 读出字节数
    LCALL   F_ReadNbyte ; R2: 读I2C数据首地址,  R0: 读出数据存放首地址,  R3: 读出字节数

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
; 函数: F_WriteEEP
; 描述: 写24Cxx函数。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 
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

    MOV     R2, #0      ;写I2C数据首地址
    MOV     R0, #EEPROM ;写入数据存放首地址
    MOV     R3, #8      ;写入字节数
    LCALL   F_WriteNbyte    ;
    RET

;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_delay_ms:
    PUSH    02H     ;入栈R2
    PUSH    03H     ;入栈R3
    PUSH    04H     ;入栈R4

    MOV     R4,A

L_delay_ms_1:
    MOV     WR2, #(Fosc_KHZ / 4)
    
L_delay_ms_2:
    DEC     WR2, #1         ;1T
    JNE     L_delay_ms_2    ;3T

    DJNZ    R4, L_delay_ms_1

    POP     04H     ;出栈R4
    POP     03H     ;出栈R3
    POP     02H     ;出栈R2
    RET



    END

