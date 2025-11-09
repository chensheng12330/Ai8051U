;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;/************* 功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;程序使用P0口做跑马灯，演示系统时钟源切换效果。

;跑马灯每跑一轮切换一次时钟源，分别是默认IRC主频，主频12分频，PLL 96M 16分频后再2分频，内部32K IRC。

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

;*******************************************************************
;*******************************************************************

;*******************************************************************

Mode        DATA    21H
Count       DATA    22H

;*******************************************************************
            ORG     0000H      ;程序复位入口，编译器自动定义到 0FF0000H 地址
            LJMP    MAIN

            ORG     0100H      ;编译器自动定义到 0FF0100H 地址
MAIN:
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

    CLR     P4.0        ;LED Power On
    MOV     Mode, #0
    MOV     R0, #0XFE
L_MainLoop:
    MOV     P0,R0
    
    MOV     A,R0
	RL      A
    MOV     R0,A

    JB      ACC.0,L_MainDelay
    LCALL   F_MCLK_Sel

L_MainDelay:
    MOV     A, #10
    LCALL   F_delay_ms      ;延时10ms
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

;========================================================================
; 函数: F_MCLK_Sel
; 描述: 主频设置程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
;========================================================================
F_MCLK_Sel:
    MOV     A,Mode
	JNZ     F_MCLK_1
F_MCLK_0:
    MOV     A, #080H                    ;启动内部 IRC
    MOV     WR6, #WORD0 HIRCCR
    MOV     WR4, #WORD2 HIRCCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                   ;等待时钟稳定

    MOV     A, #00H                     ;时钟不分频
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #00H                     ;选择内部IRC(默认)
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_1:
    MOV     A,Mode
    XRL     A,#1
	JNZ     F_MCLK_2
    MOV     A, #080H                    ;启动内部 IRC
    MOV     WR6, #WORD0 HIRCCR
    MOV     WR4, #WORD2 HIRCCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                   ;等待时钟稳定

    MOV     A, #48                      ;时钟48分频
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #00H                     ;选择内部IRC(默认)
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_2:
    MOV     A,Mode
    XRL     A,#2
	JNZ     F_MCLK_3
    MOV     A, #0A0H                    ;使能PLL倍频, 输入时钟2分频(选择PLL输入时钟分频,保证输入时钟为12M)
    MOV     WR6, #WORD0 USBCLK
    MOV     WR4, #WORD2 USBCLK
    MOV     @DR4, R11

    MOV     Count, #100
    DJNZ    Count,$                     ;等待PLL锁频

    MOV     A, #8                       ;时钟8分频
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #08H                     ;选择PLL输出时钟2分频后的时钟作为主时钟
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
	INC     Mode
    RET

F_MCLK_3:
    MOV     A,Mode
    XRL     A,#3
	JNZ     F_MCLK_CLR
    MOV     A, #080H                    ;启动内部 32K IRC
    MOV     WR6, #WORD0 IRC32KCR
    MOV     WR4, #WORD2 IRC32KCR
    MOV     @DR4, R11
    MOV     R11, @DR4
    JNB     ACC.0,$-1                  ;等待时钟稳定

    MOV     A, #00H                     ;时钟不分频
    MOV     WR6, #WORD0 CLKDIV
    MOV     WR4, #WORD2 CLKDIV
    MOV     @DR4, R11
    MOV     A, #03H                     ;选择内部 32K
    MOV     WR6, #WORD0 CLKSEL
    MOV     WR4, #WORD2 CLKSEL
    MOV     @DR4, R11
F_MCLK_CLR:
    MOV     Mode, #0
    RET

;F_MCLK_4:
    ;MOV     A,Mode
    ;XRL     A,#3
	;JNZ     F_MCLK_CLR
    ;MOV     A, #0C0H                   ;启动外部晶振
    ;MOV     WR6, #WORD0 XOSCCR
    ;MOV     WR4, #WORD2 XOSCCR
    ;MOV     @DR4, R11
    ;MOV     R11, @DR4
    ;JNB     ACC.0,$-1                  ;等待时钟稳定

    ;MOV     A, #00H                     ;时钟不分频
    ;MOV     WR6, #WORD0 CLKDIV
    ;MOV     WR4, #WORD2 CLKDIV
    ;MOV     @DR4, R11
    ;MOV     A, #001H                     ;选择外部晶振
    ;MOV     WR6, #WORD0 CLKSEL
    ;MOV     WR4, #WORD2 CLKSEL
    ;MOV     @DR4, R11
    ;MOV     Mode, #0
    ;RET

    END

