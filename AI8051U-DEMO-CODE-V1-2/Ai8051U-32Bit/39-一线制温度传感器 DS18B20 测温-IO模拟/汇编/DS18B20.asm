;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;/************* 功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;通过一个IO口获取一线制温度传感器 DS18B20 温度值.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;通过数码管显示测量的温度值，同时通过串口1(P3.0,P3.1)打印温度值.

;下载时, 选择时钟 24MHz (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHz

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 中断频率, 1000次/秒

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************
UART1_Baudrate EQU     (-52)   ;115200bps @ 24MHz      UART1_Baudrate = 65536UL - ((Fosc_KHZ / 4) / Baudrate)

;*******************************************************************

;*************  IO口定义    **************/
P_HC595_SER     BIT     P3.4  ;   //pin 14    SER     data input
P_HC595_RCLK    BIT     P3.5  ;   //pin 12    RCLk    store (latch) clock
P_HC595_SRCLK   BIT     P3.2  ;   //pin 11    SRCLK   Shift data clock

DQ  BIT P3.3                ;DS18B20的数据口位P3.3

;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ;   1ms标志

LED8            DATA    30H     ;   显示缓冲 30H ~ 37H
display_index   DATA    38H     ;   显示位索引

msecond         DATA    39H     ;

;*******************************************************************
;*******************************************************************

    ORG     0000H           ;程序复位入口，编译器自动定义到 0FF0000H 地址
    LJMP    F_Main

    ORG     000BH           ;1 Timer0 interrupt
    LJMP    F_Timer0_Interrupt

    ORG     0100H           ;编译器自动定义到 0FF0100H 地址
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
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================
    MOV     display_index, #0
    MOV     R0, #LED8
    MOV     R2, #8
L_ClearLoop:
    MOV     @R0, #DIS_BLACK     ;上电消隐
    INC     R0
    DJNZ    R2, L_ClearLoop
    
    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();

    MOV     A, #1
    LCALL   F_UART1_config
    SETB    EA          ; 打开总中断

;=====================================================

L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
;=================== 检测300ms是否到 ==================================
    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #300
    JC      L_Main_Loop     ;if(msecond < 300), jmp
    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    CALL    DS18B20_Reset       ;设备复位
    MOV     A,#0CCH             ;跳过ROM命令
    CALL    DS18B20_WriteByte   ;送出命令
    MOV     A,#044H             ;开始转换
    CALL    DS18B20_WriteByte   ;送出命令
    JNB     DQ,$                ;等待转换完成

    CALL    DS18B20_Reset       ;设备复位
    MOV     A,#0CCH             ;跳过ROM命令
    CALL    DS18B20_WriteByte   ;送出命令
    MOV     A,#0BEH             ;读暂存存储器
    CALL    DS18B20_WriteByte   ;送出命令
    CALL    DS18B20_ReadByte    ;读温度低字节
    MOV     R7,A                ;存储数据
    CALL    DS18B20_ReadByte    ;读温度高字节
    MOV     R6,A                ;存储数据

    MOV     WR2, #5         ;0.0625 * 10，保留1位小数点
    MUL     WR6, WR2        ;(R6,R7)* R3 -->(R4,R5,R6,R7)
    MOV     WR0, #00000H    ;除数(R0,R1,R2,R3)
    MOV     WR2, #8         ;温度 * 0.625 = 温度 * 5/8
    MOV     DMAIR, #04H     ;32位无符号除法 (R4,R5,R6,R7)/(R0,R1,R2,R3)=(R4,R5,R6,R7),余数在(R0,R1,R2,R3)

    MOV     A,#054H         ;"T"
    CALL    F_SendData
    MOV     A,#03DH         ;"="
    CALL    F_SendData

    MOV     WR0, #00000H
    MOV     WR2, #1000
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示温度值
    ANL     A, #0x0F
    MOV     LED8+4, A

    JNZ     L_LED8_3_Not_0
    MOV     LED8+4, #DIS_BLACK      ;千位为0则消隐
    MOV     A,#0F0H
    JMP     L_QuitRead_Temp
L_LED8_3_Not_0:
    JNB     F0, L_QuitRead_Temp
    MOV     LED8+4, #DIS_   ;负温度, 显示-
    MOV     A,#0FDH
L_QuitRead_Temp:

    ADD     A,#030H
    CALL    F_SendData      ;串口打印温度值

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示温度值
    ANL     A, #0x0F
    MOV     LED8+5, A

    ADD     A,#030H
    CALL    F_SendData      ;串口打印温度值

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #00000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示温度值
    ANL     A, #0x0F
    ADD     A, #DIS_DOT
    MOV     LED8+6, A

    ADD     A,#010H
    CALL    F_SendData      ;串口打印温度值
    MOV     A,#02EH         ;"."
    CALL    F_SendData

    MOV     A, R3           ;显示温度值
    ANL     A, #0x0F
    MOV     LED8+7, A

    ADD     A,#030H
    CALL    F_SendData      ;串口打印温度值

    MOV     A,#0DH
    CALL    F_SendData      ;串口打印回车换行符
    MOV     A,#0AH
    CALL    F_SendData

    LJMP    L_Main_Loop


;**************************************
;延时X微秒(12M)
;不同的工作环境,需要调整此函数
;入口参数:R7
;出口参数:无
;**************************************
DelayXus:                   ;6 此延时函数是使用1T的指令周期进行计算,与传统的12T的MCU不同
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    NOP                     ;1
    DJNZ R1,DelayXus        ;4
    RET                     ;4

;**************************************
;复位DS18B20,并检测设备是否存在
;入口参数:无
;出口参数:无
;**************************************
DS18B20_Reset:
    PUSH    1
    CLR     DQ                 ;送出低电平复位信号
    MOV     R1,#240            ;延时至少480us
    CALL    DelayXus
    MOV     R1,#240
    CALL    DelayXus
    SETB    DQ                 ;释放数据线
    MOV     R1,#60             ;等待60us
    CALL    DelayXus
    MOV     C,DQ               ;检测存在脉冲
    MOV     R1,#240            ;等待设备释放数据线
    CALL    DelayXus
    MOV     R1,#180
    CALL    DelayXus
    JC      DS18B20_Reset      ;如果设备不存在,则继续等待
    POP     1
    RET

;**************************************
;从DS18B20读1字节数据
;入口参数:无
;出口参数:ACC
;**************************************
DS18B20_ReadByte:
    CLR     A
    PUSH    0
    PUSH    1
    MOV     0,#8               ;8位计数器
ReadNext:
    CLR     DQ                 ;开始时间片
    MOV     R1,#1              ;延时等待
    CALL    DelayXus
    SETB    DQ                 ;准备接收
    MOV     R1,#1
    CALL    DelayXus
    MOV     C,DQ               ;读取数据
    RRC     A
    MOV     R1,#60             ;等待时间片结束
    CALL    DelayXus
    DJNZ    0,ReadNext
    POP     1
    POP     0
    RET

;**************************************
;向DS18B20写1字节数据
;入口参数:ACC
;出口参数:无
;**************************************
DS18B20_WriteByte:
    PUSH    0
    PUSH    1
    MOV     0,#8               ;8位计数器
WriteNext:
    CLR     DQ                 ;开始时间片
    MOV     R1,#1              ;延时等待
    CALL    DelayXus
    RRC     A                  ;输出数据
    MOV     DQ,C
    MOV     R1,#60             ;等待时间片结束
    CALL    DelayXus
    SETB    DQ                 ;准备送出下一位数据
    MOV     R1,#1
    CALL    DelayXus
    DJNZ    0,WriteNext
    POP     1
    POP     0
    RET

; *********************** 显示相关程序 ****************************************
T_Display:                      ;标准字库
;    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
DB  03FH,006H,05BH,04FH,066H,06DH,07DH,007H,07FH,06FH,077H,07CH,039H,05EH,079H,071H
;  black  -    H    J    K    L    N    o    P    U    t    G    Q    r    M    y
DB  000H,040H,076H,01EH,070H,038H,037H,05CH,073H,03EH,078H,03dH,067H,050H,037H,06EH
;    0.   1.   2.   3.   4.   5.   6.   7.   8.   9.   -1
DB  0BFH,086H,0DBH,0CFH,0E6H,0EDH,0FDH,087H,0FFH,0EFH,046H

T_COM:
DB  001H,002H,004H,008H,010H,020H,040H,080H     ;   位码

;========================================================================
; 函数: F_Send_595
; 描述: 向HC595发送一个字节子程序。
; 参数: ACC: 要发送的字节数据.
; 返回: none.
; 版本: VER1.0
; 日期: 2024-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_Send_595:
    PUSH    02H     ;R2入栈
    MOV     R2, #8
L_Send_595_Loop:
    RLC     A
    MOV     P_HC595_SER,C
    SETB    P_HC595_SRCLK
    CLR     P_HC595_SRCLK
    DJNZ    R2, L_Send_595_Loop
    POP     02H     ;R2出栈
    RET

;//========================================================================
;// 函数: F_DisplayScan
;// 描述: 显示扫描子程序。
;// 参数: none.
;// 返回: none.
;// 版本: VER1.0
;// 日期: 2013-4-1
;// 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;//========================================================================
F_DisplayScan:
    PUSH    DPH     ;DPH入栈
    PUSH    DPL     ;DPL入栈
    PUSH    00H     ;R0 入栈
    
    MOV     DPTR, #T_Display
    MOV     A, display_index
    ADD     A, #LED8
    MOV     R0, A
    MOV     A, @R0
    MOVC    A, @A+DPTR
    LCALL   F_Send_595      ;输出段码

    MOV     DPTR, #T_COM
    MOV     A, display_index
    MOVC    A, @A+DPTR
    CPL     A
    LCALL   F_Send_595      ;输出位码
    
    SETB    P_HC595_RCLK
    CLR     P_HC595_RCLK    ;   锁存输出数据
    INC     display_index
    MOV     A, display_index
    ANL     A, #0F8H            ; if(display_index >= 8)
    JZ      L_QuitDisplayScan
    MOV     display_index, #0;  ;8位结束回0
L_QuitDisplayScan:
    POP     00H     ;R0 出栈
    POP     DPL     ;DPL出栈
    POP     DPH     ;DPH出栈
    RET

;**************** 中断函数 ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈

    LCALL   F_DisplayScan   ; 1ms扫描显示一位
    SETB    B_1ms           ; 1ms标志

    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

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


    END

