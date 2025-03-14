;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;每触发一次T0中断, LED10产生一次翻转；每触发一次T1中断, LED11产生一次翻转.

;由于按键是机械按键, 按下有抖动, 而本例程没有去抖动处理, 所以按一次产生多次中断也是正常的.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************

$include (../../comm/AI8051U.INC)

;****************************** 用户定义宏 ***********************************/

Fosc_KHZ        EQU     24000   ;24000KHZ
STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/


;*************  本地变量声明    **************/


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

        ORG     001BH               ;3  Timer1 interrupt
        LJMP    F_Timer1_Interrupt


;*******************************************************************
;*******************************************************************


;******************** 主程序 **************************/
    ORG     0100H          ;编译器自动定义到 0FF0100H 地址
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
    MOV     P3M1, #030H    ;设置为准双向口, P3.4,P3.5高阻输入
    MOV     P3M0, #00H
    MOV     P4M1, #00H     ;设置为准双向口
    MOV     P4M0, #00H
    MOV     P5M1, #00H     ;设置为准双向口
    MOV     P5M0, #00H
    MOV     P6M1, #00H     ;设置为准双向口
    MOV     P6M0, #00H
    MOV     P7M1, #00H     ;设置为准双向口
    MOV     P7M0, #00H

    MOV     A, #030H       ; P3.4,P3.5使能内部4.1K上拉电阻
    MOV     WR6, #WORD0 P3PU
    MOV     WR4, #WORD2 P3PU
    MOV     @DR4, R11

    MOV     SP, #STACK_POIRTER
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    CLR     A
    MOV     TMOD,A
    ORL     TMOD, #(1 SHL 2)    ; 使能T0外部计数模式
    ORL     TMOD, #(1 SHL 6)    ; 使能T1外部计数模式
    MOV     TH0, #0FFH
    MOV     TL0, #0FFH
    MOV     TH1, #0FFH
    MOV     TL1, #0FFH
    SETB    TR0 ; 启动定时器T0
    SETB    TR1 ; 启动定时器T1
    SETB    ET0 ; 使能定时器中断T0
    SETB    ET1 ; 使能定时器中断T1

    ANL     INTCLKO, #NOT 03H ; T0,T1不输出时钟

    SETB    EA      ;允许总中断

;=================== 主循环 ==================================
L_MainLoop:
    LJMP    L_MainLoop

;========================================================================
; 函数: F_Timer0_Interrupt
; 描述: Timer0中断函数.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2020-11-4
; 备注: 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_Timer0_Interrupt:
    CPL     P4.3
    RETI
    
;========================================================================
; 函数: F_Timer1_Interrupt
; 描述: Timer1中断函数.
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2020-11-4
; 备注: 所用到的通用寄存器都入栈保护, 退出时恢复原来数据不改变.
;========================================================================
F_Timer1_Interrupt:
    CPL     P4.2
    RETI


    END

