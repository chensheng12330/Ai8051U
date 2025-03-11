;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/


;/************* 功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;程序演示芯片PCA输出呼吸灯效果PWM信号.

;PCA0从P4.2口输出8位PWM信号，可通过实验箱LED11查看效果.

;PCA1从P4.3口输出10位PWM信号，可通过实验箱LED10查看效果.

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24000KHZ

STACK_POIRTER   EQU     0D0H    ;堆栈开始地址

Timer0_Reload   EQU     (65536 - (Fosc_KHZ / 6))  ; Timer 0 (12T)中断频率, 500次/秒

;*******************************************************************

PWM0_Flag       BIT     20H.0
PWM1_Flag       BIT     20H.1

PWM0_Duty       DATA    30H
PWM1_Duty       DATA    32H

;*******************************************************************

;*******************************************************************

Mode        DATA    21H
Count       DATA    22H

;*******************************************************************
    ORG     0000H           ;程序复位入口，编译器自动定义到 0FF0000H 地址
    LJMP    MAIN

    ORG     000BH           ;1  Timer0 interrupt
    LJMP    F_Timer0_Interrupt

;******************** 主程序 **************************/
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

;================= 用户初始化程序 ====================================
    CLR     PWM0_Flag
    CLR     PWM1_Flag
    MOV     WR6, #128
    MOV     PWM0_Duty, WR6
    MOV     WR6, #256
    MOV     PWM1_Duty, WR6

    CLR     TR0
    ANL     AUXR, #NOT(1 SHL 7) ; Timer0_12T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    SETB    EA          ; 打开总中断
    
    LCALL   F_PWM_Init          ; PWM初始化

L_MainLoop:
    SJMP    L_MainLoop

;========================================================================
; 函数: F_PWM_Init
; 描述: PWM初始化程序.
; 参数: none
; 返回: none.
; 版本: V1.0, 2021-3-3
;========================================================================
F_PWM_Init:
    //bit[6:5]=0: ECI/P1.2, CCP0/P1.3, CCP1/P1.4, CCP2/P1.1
    //bit[6:5]=1: ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    //bit[6:5]=2: ECI/P2.3, CCP0/P2.0, CCP1/P2.1, CCP2/P2.2
    MOV     WR6, #WORD0 CMOD
    MOV     WR4, #WORD2 CMOD
    MOV     R11, @DR4
    ANL     A,#09FH
    ORL     A,#(1 SHL 5)        ;选择PCA通道 1:ECI/P4.1, CCP0/P4.2, CCP1/P4.3, CCP2/-
    ORL     A,#0EH              ;PCA 时钟为系统时钟/8
    MOV     @DR4, R11

    MOV     A, #00H
    MOV     WR6, #WORD0 CL
    MOV     WR4, #WORD2 CL
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CH
    MOV     WR4, #WORD2 CH
    MOV     @DR4, R11

    ;配置 PCA0 模块
    MOV     A, #042H            ;PCA 模块 0 为 PWM 工作模式
    MOV     WR6, #WORD0 CCAPM0
    MOV     WR4, #WORD2 CCAPM0
    MOV     @DR4, R11

    MOV     A, #00H             ;PCA 模块 0 输出 8 位 PWM
    MOV     WR6, #WORD0 PCA_PWM0
    MOV     WR4, #WORD2 PCA_PWM0
    MOV     @DR4, R11

    MOV     A, #080H            ;设置 PWM 初始占空比
    MOV     WR6, #WORD0 CCAP0L
    MOV     WR4, #WORD2 CCAP0L
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CCAP0H
    MOV     WR4, #WORD2 CCAP0H
    MOV     @DR4, R11

    ;配置 PCA1 模块
    MOV     A, #042H            ;PCA 模块 1 为 PWM 工作模式
    MOV     WR6, #WORD0 CCAPM1
    MOV     WR4, #WORD2 CCAPM1
    MOV     @DR4, R11

    MOV     A, #0C0H            ;PCA 模块 1 输出 10 位 PWM
    MOV     WR6, #WORD0 PCA_PWM1
    MOV     WR4, #WORD2 PCA_PWM1
    MOV     @DR4, R11

    MOV     A, #040H            ;设置 PWM 初始占空比
    MOV     WR6, #WORD0 CCAP1L
    MOV     WR4, #WORD2 CCAP1L
    MOV     @DR4, R11
    MOV     WR6, #WORD0 CCAP1H
    MOV     WR4, #WORD2 CCAP1H
    MOV     @DR4, R11

    ;配置 PCA2 模块
;    MOV     A, #042H            ;PCA 模块 2 为 PWM 工作模式
;    MOV     WR6, #WORD0 CCAPM2
;    MOV     WR4, #WORD2 CCAPM2
;    MOV     @DR4, R11

;    MOV     A, #080H            ;PCA 模块 2 输出 6 位 PWM
;    MOV     WR6, #WORD0 PCA_PWM2
;    MOV     WR4, #WORD2 PCA_PWM2
;    MOV     @DR4, R11

;    MOV     A, #020H            ;设置 PWM 初始占空比
;    MOV     WR6, #WORD0 CCAP2L
;    MOV     WR4, #WORD2 CCAP2L
;    MOV     @DR4, R11
;    MOV     WR6, #WORD0 CCAP2H
;    MOV     WR4, #WORD2 CCAP2H
;    MOV     @DR4, R11

    MOV     WR6, #WORD0 CCON
    MOV     WR4, #WORD2 CCON
    MOV     R11, @DR4
    ORL     A,#040H              ;启动 PCA 计时器
    MOV     @DR4, R11

    RET

;========================================================================
; 函数: F_UpdatePwm
; 描述: 更新PWM占空比值. 
; 参数: [PWMn_Duty_H PWMn_Duty_L]: pwm占空比值.
; 返回: none.
; 版本: V1.0, 2021-3-3
;========================================================================
F_UpdatePwm:

;    PCA_PWM0 = (PCA_PWM0 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
;    CCAP0H = (u8)pwm_value;

    MOV     WR0, PWM0_Duty      ;设置占空比时间
    MOV     WR6, WR0
    ANL     WR6, #0300H
    SRL     WR6
    SRL     WR6
    SRL     WR6
    SRL     WR6
    MOV     WR30, #WORD0 PCA_PWM0
    MOV     WR28, #WORD2 PCA_PWM0
    MOV     R6, @DR28
    ANL     R6, #0CDH
    ORL     R6,R7

    MOV     WR2, WR0
    ANL     WR2, #0400H
    MOVZ    WR2,R2
    SRL     WR2
    MOV     R7,R3
    ORL     R7,R6
    MOV     @DR28,R7

    MOV     R3,R1
    MOV     WR6, #WORD0 CCAP0H
    MOV     WR4, #WORD2 CCAP0H
    MOV     @DR4,R3

;    PCA_PWM1 = (PCA_PWM1 & ~0x32) | (u8)((pwm_value & 0x0300) >> 4) | (u8)((pwm_value & 0x0400) >> 9);
;    CCAP1H = (u8)pwm_value;

    MOV     WR0, PWM1_Duty      ;设置占空比时间
    MOV     WR6, WR0
    ANL     WR6, #0300H
    SRL     WR6
    SRL     WR6
    SRL     WR6
    SRL     WR6
    MOV     WR30, #WORD0 PCA_PWM1
    MOV     WR28, #WORD2 PCA_PWM1
    MOV     R6, @DR28
    ANL     R6, #0CDH
    ORL     R6,R7

    MOV     WR2, WR0
    ANL     WR2, #0400H
    MOVZ    WR2,R2
    SRL     WR2
    MOV     R7,R3
    ORL     R7,R6
    MOV     @DR28,R7

    MOV     R3,R1
    MOV     WR6, #WORD0 CCAP1H
    MOV     WR4, #WORD2 CCAP1H
    MOV     @DR4,R3

    RET

;**************** 中断函数 ***************************************************
F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈
    PUSH    R1      ;R1入栈
    PUSH    R0      ;R0入栈

    JB      PWM1_Flag, T0_PWM1_SUB
    MOV     WR0, PWM1_Duty
    INC     WR0, #1
    MOV     PWM1_Duty, WR0   ;PWM1_Duty + 1
    CMP     WR0, #1023
    JC      T0_PWM0_Start
    SETB    PWM1_Flag
    SJMP    T0_PWM0_Start
T0_PWM1_SUB:
    MOV     WR0, PWM1_Duty
    DEC     WR0, #1
    MOV     PWM1_Duty, WR0   ;PWM1_Duty - 1
    CMP     WR0, #0
    JG      T0_PWM0_Start
    CLR     PWM1_Flag

T0_PWM0_Start:
    JB      PWM0_Flag, T0_PWM0_SUB
    MOV     WR0, PWM0_Duty
    INC     WR0, #1
    MOV     PWM0_Duty, WR0   ;PWM0_Duty + 1
    CMP     WR0, #255
    JC      F_QuitTimer0
    SETB    PWM0_Flag
    SJMP    F_QuitTimer0
T0_PWM0_SUB:
    MOV     WR0, PWM0_Duty
    DEC     WR0, #1
    MOV     PWM0_Duty, WR0   ;PWM0_Duty - 1
    CMP     WR0, #0
    JG      F_QuitTimer0
    CLR     PWM0_Flag

F_QuitTimer0:
    LCALL   F_UpdatePwm

    POP     R0      ;R0出栈
    POP     R1      ;R1出栈
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;========================================================================

    END

