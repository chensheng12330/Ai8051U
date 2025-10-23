;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;data建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;用STC的MCU的SPI接口控制74HC595驱动8位数码管。

;显示效果为: 数码时钟.

;使用Timer0的16位自动重装来产生1ms节拍,程序运行于这个节拍下, 用户修改MCU主时钟频率时,自动定时于1ms.

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
B_1ms           BIT     Flag0.0 ; 1ms标志

LED8            DATA    30H     ; 显示缓冲 30H ~ 37H
display_index   DATA    38H     ; 显示位索引

usrhour         DATA    39H     ;
usrminute       DATA    3AH
usrsecond       DATA    3BH     ;
msecond         DATA    3CH     ; 2 byte

;*******************************************************************
;*******************************************************************
        ORG     0000H               ;程序复位入口，编译器自动定义到 0FF0000H 地址
        LJMP    F_Main

        ORG     000BH               ;1  Timer0 interrupt
        LJMP    F_Timer0_Interrupt

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
    
    MOV     SPCTL,#0DEH
    ORL     P_SW1, #(3 SHL 2)   ; 0: P1.4 P1.5 P1.6 P1.7, 1: P2.4 P2.5 P2.6 P2.7, 2: P4.0 P4.1 P4.2 P4.3, 3: P3.5 P3.4 P3.3 P3.2
    MOV     SPSTAT,#0C0H        ; Clear SPIF,WCOL
    
    CLR     TR0
    ORL     AUXR, #(1 SHL 7)    ; Timer0_1T();
    ANL     TMOD, #NOT 04H      ; Timer0_AsTimer();
    ANL     TMOD, #NOT 03H      ; Timer0_16bitAutoReload();
    MOV     TH0, #Timer0_Reload / 256   ;Timer0_Load(Timer0_Reload);
    MOV     TL0, #Timer0_Reload MOD 256
    SETB    ET0         ; Timer0_InterruptEnable();
    SETB    TR0         ; Timer0_Run();
    SETB    EA          ; 打开总中断
    
    MOV     usrhour,   #12 ; 初始化时间值
    MOV     usrminute, #0
    MOV     usrsecond, #0
    LCALL   F_DisplayRTC
    
;=================== 主循环 ==================================
L_Main_Loop:
    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms

    MOV     WR6, msecond
    INC     WR6, #1         ;msecond + 1
    MOV     msecond, WR6
    CMP     WR6, #1000
    JC      L_Main_Loop     ;if(msecond < 1000), jmp

    MOV     WR6, #0
    MOV     msecond, WR6    ;msecond = 0

    LCALL   F_RTC
    LCALL   F_DisplayRTC

    LJMP    L_Main_Loop

;========================================================================
; 函数: F_DisplayRTC
; 描述: 显示时钟子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_DisplayRTC:
    PUSH    B       ;B入栈
    
    MOV     A, usrhour
    MOV     B, #10
    DIV     AB
    MOV     LED8, A
    MOV     LED8+1, B
    MOV     LED8+2, #DIS_;
    
    MOV     A, usrminute
    MOV     B, #10
    DIV     AB
    MOV     LED8+3, A;
    MOV     LED8+4, B;
    MOV     LED8+5, #DIS_;

    MOV     A, usrsecond
    MOV     B, #10
    DIV     AB
    MOV     LED8+6, A;
    MOV     LED8+7, B;
    
    POP     B       ;B出栈
    RET

;========================================================================
; 函数: F_RTC
; 描述: RTC演示子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
F_RTC:
    INC     usrsecond      ; usrsecond + 1
    MOV     A, usrsecond
    CLR     C
    SUBB    A,#60
    JC      L_QuitRTC   ; usrsecond >= 60?
    MOV     usrsecond, #0;

    INC     usrminute      ; usrminute + 1
    MOV     A, usrminute
    CLR     C
    SUBB    A,#60
    JC      L_QuitRTC   ; usrminute >= 60?
    MOV     usrminute, #0

    INC     usrhour        ; usrhour + 1
    MOV     A, usrhour
    CLR     C
    SUBB    A,#24
    JC      L_QuitRTC   ; usrhour >= 24?
    MOV     usrhour, #0
L_QuitRTC:
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
    CLR     P_HC595_RCLK
    MOV     SPDAT,A
L_Wait:
    JNB     SPIF, L_Wait
    CLR     SPIF
    CLR     WCOL
    SETB    P_HC595_RCLK
    RET

;========================================================================
; 函数: F_DisplayScan
; 描述: 显示扫描子程序。
; 参数: none.
; 返回: none.
; 版本: VER1.0
; 日期: 2013-4-1
; 备注: 除了ACCC和PSW外, 所用到的通用寄存器都入栈
;========================================================================
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
    
;*******************************************************************

    END

