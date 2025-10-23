;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;读ADC测量外部电压，使用内部基准计算电压.

;用STC的MCU的IO方式控制74HC595驱动8位数码管。

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

;右边4位数码管显示测量的电压值.

;串口1(P3.0,P3.1)配置：115200,N,8,1，使用HEX模式打印(电压*100)数据。

;外部电压从板上测温电阻两端输入, 输入电压0~Vref, 不要超过Vref或低于0V. 

;实际项目使用请串一个1K的电阻到ADC输入口, ADC输入口再并一个102~103电容到地.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - Fosc_KHZ)  ; Timer 0 中断频率, 1000次/秒

DIS_DOT         EQU     020H
DIS_BLACK       EQU     010H
DIS_            EQU     011H

;*******************************************************************

;*******************************************************************


;*************  IO口定义    **************/
P_HC595_SER     BIT     P3.4  ;   //pin 14    SER     data input
P_HC595_RCLK    BIT     P3.5  ;   //pin 12    RCLk    store (latch) clock
P_HC595_SRCLK   BIT     P3.2  ;   //pin 11    SRCLK   Shift data clock

;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ;   1ms标志

LED8            DATA    30H     ;   显示缓冲 30H ~ 37H
display_index   DATA    38H     ;   显示位索引

msecond         DATA    39H     ;
Bandgap         DATA    3BH     ;
ADC3            DATA    3DH     ;

;========================================================================
; 描述: 串口1发送1字节数据。
; 参数: P_DATA: 发送的数据.
;========================================================================
PRINT MACRO P_DATA
    MOV     SBUF, P_DATA    ;发送一个字节
    JNB     TI, $           ;等待发送完成
    CLR     TI
ENDM

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt


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
    MOV     P1M1, #08H     ;设置为准双向口,P1.3设置高阻输入
    MOV     P1M0, #00H
    MOV     P2M1, #00H     ;设置为准双向口
    MOV     P2M0, #00H
    MOV     P3M1, #00H     ;设置为准双向口
    MOV     P3M0, #00H
    MOV     P4M1, #00H     ;设置为准双向口
    MOV     P4M0, #00H
    MOV     P5M1, #00H     ;设置为准双向口,P5.1设置推挽输出
    MOV     P5M0, #02H
    MOV     P6M1, #00H     ;设置为准双向口
    MOV     P6M0, #00H
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================
    SETB    P5.1    ;给NTC供电

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

    LCALL   UART1_INIT
    SETB    EA          ; 打开总中断

    LCALL   F_ADC_config    ; ADC初始化

;=====================================================

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

    ;MOV     A, #15
    ;LCALL   F_Get_ADC12bitResult    ;ACC - 通道0~15, 先读一次并丢弃结果, 让内部的采样电容的电压等于输入值.
    MOV     A, #15
    LCALL   F_Get_ADC12bitResult    ;读内部基准ADC, 读15通道, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 4096 为错误
    MOV     Bandgap, WR6            ;保存Bandgap

    MOV     A, #3
    LCALL   F_Get_ADC12bitResult    ; 读外部电压ADC, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 4096 为错误
    MOV     ADC3, WR6               ;保存adc

    MOV     WR2, #119       ; adc * 119 / Bandgap, 计算外部电压, Bandgap为1.19V, 测电压分辨率0.01V
    MUL     WR6, WR2        ;(R6,R7)* R3 -->(R4,R5,R6,R7)

    MOV     WR0, #0000H     ;除数(R0,R1,R2,R3)
    MOV     WR2, Bandgap
    MOV     DMAIR, #04H     ;32位无符号除法 (R4,R5,R6,R7)/(R0,R1,R2,R3)=(R4,R5,R6,R7),余数在(R0,R1,R2,R3)

;    PRINT   R4
;    PRINT   R5
    PRINT   R6             ;串口打印电压HEX数据(电压*100)
    PRINT   R7

    MOV     WR0, #0000H
    MOV     WR2, #100
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示电压值
    ANL     A, #0x0F
    ADD     A, #DIS_DOT
    MOV     LED8+5, A

    MOV     WR4, WR0        ;余数做被除数
    MOV     WR6, WR2
    MOV     WR0, #0000H
    MOV     WR2, #10
    MOV     DMAIR, #04H     ;32位无符号除法
    MOV     A, R7           ;显示电压值
    ANL     A, #0x0F
    MOV     LED8+6, A

    MOV     A, R3           ;显示电压值
    ANL     A, #0x0F
    MOV     LED8+7, A

L_Quit_Check_300ms:

;=====================================================

    LJMP    L_Main_Loop

;**********************************************/

F_ADC_config:
    MOV     A, #03FH            ; 设置 ADC 内部时序，ADC采样时间建议设最大值
    MOV     WR6, #WORD0 ADCTIM
    MOV     WR4, #WORD2 ADCTIM
    MOV     @DR4, R11

    MOV     ADCCFG, #02FH       ; 设置转换结果右对齐模式， ADC 时钟为系统时钟/2/16
    MOV     ADC_CONTR, #080H    ; 使能 ADC 模块
    RET

;========================================================================
; 函数: F_Get_ADC12bitResult
; 描述: 查询法读一次ADC结果.
; 参数: ACC: 选择要转换的ADC.
; 返回: (R6 R7) = 12位ADC结果.
; 版本: V1.0, 2020-11-09
;========================================================================
F_Get_ADC12bitResult:   ;ACC - 通道0~7, 查询方式做一次ADC, 返回值(R6 R7)就是ADC结果, == 4096 为错误
    MOV     R7, A           //channel
    MOV     ADC_RES,  #0;
    MOV     ADC_RESL, #0;

    MOV     A, ADC_CONTR        ;ADC_CONTR = (ADC_CONTR & 0xd0) | ADC_START | channel; 
    ANL     A, #0D0H
    ORL     A, #40H
    ORL     A, R7
    MOV     ADC_CONTR, A
    NOP
    NOP
    NOP
    NOP

L_WaitAdcLoop:
    MOV     A, ADC_CONTR
    JNB     ACC.5, L_WaitAdcLoop

    ANL     ADC_CONTR, #NOT 020H    ;清除完成标志

    MOV     A, ADC_RES      ;12位AD结果的高4位放ADC_RES的低4位，低8位在ADC_RESL。
    ANL     A, #0FH
    MOV     R6, A
    MOV     R7, ADC_RESL

L_QuitAdc:
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
; 函数: UART1_INIT
; 描述: UART1初始化程序.
; 参数: None
; 返回: none.
; 版本: V1.0, 2024-07-22
;========================================================================
UART1_INIT:                 ;115200bps@24.000MHz
    MOV     SCON,#50H       ;8位数据,可变波特率
    ORL     AUXR,#40H       ;定时器时钟1T模式
    ANL     AUXR,#0FEH      ;串口1选择定时器1为波特率发生器
    ANL     TMOD,#0FH       ;设置定时器模式
    MOV     TL1,#0CCH       ;设置定时初始值
    MOV     TH1,#0FFH       ;设置定时初始值
    CLR     ET1             ;禁止定时器中断
    SETB    TR1             ;定时器1开始计时

    ANL     P_SW1, #0x3f
    ORL     P_SW1, #0x00    ;UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4
    RET

;***************************************************************************

    END

