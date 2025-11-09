;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

; 本例程基于AI8051U为主控芯片的实验箱进行编写测试。

; 使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

; edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

; 程序使用P0口来演示跑马灯，输出低驱动。

; 下载时, 选择时钟 24MHZ (用户可自行修改频率)。

;******************************************
$include (../../comm/AI8051U.INC)

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

;*******************************************************************
;*******************************************************************
    ORG     0FF:0000H             ;程序复位入口，编译器自动定义到 0FF0000H 地址
//    ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
    LJMP    F_Main

;******************** 主程序 **************************/
    ORG     0FF:0100H        ;编译器自动定义到 0FF0100H 地址
//    ORG     0100H          ;编译器自动定义到 0FF0100H 地址
F_Main:
    MOV     WTST, #00H     ;设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    MOV     CKCON,#00H     ;提高访问XRAM速度
    ORL     P_SW2,#080H    ;使能访问XFR

    MOV     P0M1, #00H     ;设置为推挽输出
    MOV     P0M0, #0FFH
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

    CLR     P4.0        ;LED Power On
L_MainLoop:
    CLR     P0.0
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.0

    CLR     P0.1
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.1

    CLR     P0.2
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.2

    CLR     P0.3
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.3

    CLR     P0.4
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.4

    CLR     P0.5
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.5

    CLR     P0.6
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.6

    CLR     P0.7
    LCALL   F_delay_ms      ;延时250ms
    SETB    P0.7

    SJMP    L_MainLoop

;========================================================================
; 函数: F_delay_ms
; 描述: 延时子程序。
; 参数: ACC: 延时ms数.
; 返回: none.
; 版本: VER1.0
; 日期: 2021-3-16
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_delay_ms:
    PUSH    02H     ;入栈R2
    PUSH    03H     ;入栈R3
    PUSH    04H     ;入栈R4

    MOV     R4,#250

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

