;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;比较器的正极可以是 P4.6、P5.0、P5.1 端口或者 ADC 的模拟输入通道，

;而负极可以是 P4.4 端口或者是内部 BandGap 经过 OP 后的 REFV 电压（1.19V内部固定比较电压）。

;通过中断或者查询方式读取比较器比较结果，CMP+的电平低于CMP-的电平P43口输出低电平(LED10亮)，反之输出高电平(LED10灭)。

;下载时, 选择时钟 24MHZ (用户可自行修改频率).

;******************************************

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ    EQU 24000   ;24MHZ

STACK_POIRTER   EQU     0D0H    ; 堆栈开始地址

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/


;*************  本地变量声明    **************/


;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     00ABH               ;21  CMP interrupt
        LJMP    F_CMP_Interrupt


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
    MOV     PSW, #0
    USING   0       ;选择第0组R0~R7

;================= 用户初始化程序 ====================================

    CLR     A
    ANL     A,#NOT 03H                  ; P4.6为CMP+输入脚
;    ORL     A,#01H                      ; P5.0为CMP+输入脚
;    ORL     A,#02H                      ; P5.1为CMP+输入脚
;    ORL     A,#03H                      ; ADC输入脚为CMP+输入脚
;    ANL     A,#NOT 04H                  ; P4.4为CMP-输入脚
    ORL     A,# 04H                     ; 内部1.19V参考信号源为CMP-输入脚
    MOV     WR6, #WORD0 CMPEXCFG
    MOV     WR4, #WORD2 CMPEXCFG
    MOV     @DR4, R11

    MOV     CMPCR2,#00H
    ANL     CMPCR2,#NOT 80H             ;比较器正向输出
;    ORL     CMPCR2,#80H                 ;比较器反向输出
    ANL     CMPCR2,#NOT 40H             ;使能0.1us滤波
;    ORL     CMPCR2,#40H                 ;禁止0.1us滤波
;    ANL     CMPCR2,#NOT 3FH             ;比较器结果直接输出
    ORL     CMPCR2,#10H                 ;比较器结果经过16个去抖时钟后输出
    MOV     CMPCR1,#00H
    ORL     CMPCR1,#30H                 ;使能比较器边沿中断
;    ANL     CMPCR1,#NOT 20H             ;禁止比较器上升沿中断
;    ORL     CMPCR1,#20H                 ;使能比较器上升沿中断
;    ANL     CMPCR1,#NOT 10H             ;禁止比较器下降沿中断
;    ORL     CMPCR1,#10H                 ;使能比较器下降沿中断
;    ANL     CMPCR1,#NOT 02H             ;禁止比较器输出
    ORL     CMPCR1,#02H                 ;使能比较器输出
    ORL     CMPCR1,#80H                 ;使能比较器模块
    SETB    EA          ; 打开总中断
    
;=================== 主循环 ==================================
L_MainLoop:

    ;MOV     A, CMPCR1
	;RRC     A
	;MOV     P4.3,C

    LJMP    L_MainLoop

;**********************************************/


;========================================================================
; 函数: F_CMP_Interrupt
; 描述: 比较器中断函数.
; 参数: non.
; 返回: non.
; 版本: V1.0, 2021-3-3
;========================================================================
F_CMP_Interrupt:
    PUSH    PSW
    PUSH    ACC
    PUSH    AR2

    ANL     CMPCR1, #NOT 040H      ; 清中断标志
    MOV     A, CMPCR1
	RRC     A
	MOV     P4.3,C

L_QuitCMP_Init:
    POP     AR2
    POP     ACC
    POP     PSW
    RETI

;*******************************************************************

    END

