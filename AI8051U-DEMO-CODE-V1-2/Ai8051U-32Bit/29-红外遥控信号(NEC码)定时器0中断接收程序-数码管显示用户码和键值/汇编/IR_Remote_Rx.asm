;/*---------------------------------------------------------------------*/
;/* --- Web: www.STCAI.com ---------------------------------------------*/
;/*---------------------------------------------------------------------*/

;*************  功能说明    **************

;本例程基于AI8051U为主控芯片的实验箱进行编写测试。

;使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

;edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

;红外接收程序。适用于市场上用量最大的NEC编码。

;应用层查询 B_IR_Press 标志为1,则已接收到一个键码放在IR_code中, 处理完键码后，用户程序清除 B_IR_Press 标志.

;串口打印红外接收的用户码与按键码.

;串口1(P3.0,P3.1)配置：115200,N,8,1，使用HEX模式打印。

;红外接收脚(P35)与数码管控制脚(RCK)复用，所以不能用数码管显示。

;下载时, 选择时钟 24MHz (用户可自行修改频率).

;******************************************/

$include (../../comm/AI8051U.INC)

;/****************************** 用户定义宏 ***********************************/

Fosc_KHZ        EQU     24000   ;24000KHZ, 用户只需要改动这个值以适应自己实际的频率
STACK_POIRTER   EQU     0D0H    ;堆栈开始地址
Timer0_Reload   EQU     (65536 - (Fosc_KHZ/10))   ; Timer 0 中断频率, 10000次/秒

;*******************************************************************
;*******************************************************************


;*************  IO口定义    **************/

P_IR_RX         BIT P3.5    ;定义红外接收输入IO口

;*************  本地变量声明    **************/
Flag0           DATA    20H
B_1ms           BIT     Flag0.0 ; 1ms标志
P_IR_RX_temp    BIT     Flag0.1 ; 用户不可操作, Last sample
B_IR_Sync       BIT     Flag0.2 ; 用户不可操作, 已收到同步标志
B_IR_Press      BIT     Flag0.3 ; 用户使用, 按键动作发生

cnt_1ms         DATA    39H     ;

;*************  红外接收程序变量声明    **************

IR_SampleCnt    DATA    3AH ;用户不可操作, 采样计数
IR_BitCnt       DATA    3BH ;用户不可操作, 编码位数
IR_UserH        DATA    3CH ;用户不可操作, 用户码(地址)高字节
IR_UserL        DATA    3DH ;用户不可操作, 用户码(地址)低字节
IR_data         DATA    3EH ;用户不可操作, 数据原码
IR_DataShit     DATA    3FH ;用户不可操作, 数据移位

IR_code         DATA    40H ;用户使用, 红外键码
UserCodeH       DATA    41H ;用户使用, 用户码高字节
UserCodeL       DATA    42H ;用户使用, 用户码低字节

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
    
    MOV     cnt_1ms, #10

;=====================================================

;=====================================================
L_Main_Loop:

    JNB     B_1ms,  L_Main_Loop     ;1ms未到
    CLR     B_1ms
    
    JNB     B_IR_Press, L_Main_Loop ;未检测到收到红外键码

    CLR     B_IR_Press      ;检测到收到红外键码
    PRINT   #052H           ;"R"
    PRINT   UserCodeH
    PRINT   UserCodeL
    PRINT   IR_code
    LJMP    L_Main_Loop

;**************** 中断函数 ***************************************************

F_Timer0_Interrupt: ;Timer0 1ms中断函数
    PUSH    PSW     ;PSW入栈
    PUSH    ACC     ;ACC入栈
    PUSH    AR7     ;SampleTime

    LCALL   F_IR_RX_NEC

    DJNZ    cnt_1ms, L_Quit_1ms
    MOV     cnt_1ms, #10
    SETB    B_1ms   ;1ms标志
L_Quit_1ms:

    POP     AR7
    POP     ACC     ;ACC出栈
    POP     PSW     ;PSW出栈
    RETI

;*******************************************************************
;*********************** IR Remote Module **************************
;*********************** By  (Coody) 2002-8-24 *********************
;*********************** IR Remote Module **************************
;this programme is used for Receive IR Remote (NEC Code).

;data format: Synchro, AddressH, AddressL, data, /data, (total 32 bit).

;send a frame(85ms), pause 23ms, send synchro of continue frame, pause 94ms

;data rate: 108ms/Frame


;Synchro: low=9ms, high=4.5 / 2.25ms, low=0.5626ms
;Bit0: high=0.5626ms, low=0.5626ms
;Bit1: high=1.6879ms, low=0.5626ms
;frame rate = 108ms ( pause 23 ms or 96 ms)

;******************** 红外采样时间宏定义, 用户不要随意修改  *******************

D_IR_sample         EQU 100                 ;查询时间间隔, 100us, 红外接收要求在60us~250us之间
D_IR_SYNC_MAX       EQU (15000/D_IR_sample) ;SYNC max time
D_IR_SYNC_MIN       EQU (9700 /D_IR_sample) ;SYNC min time
D_IR_SYNC_DIVIDE    EQU (12375/D_IR_sample) ;decide data 0 or 1
D_IR_DATA_MAX       EQU (3000 /D_IR_sample) ;data max time
D_IR_DATA_MIN       EQU (600  /D_IR_sample) ;data min time
D_IR_DATA_DIVIDE    EQU (1687 /D_IR_sample) ;decide data 0 or 1
D_IR_BIT_NUMBER     EQU 32                  ;bit number

;*******************************************************************************************
;**************************** IR RECEIVE MODULE ********************************************

F_IR_RX_NEC:
    INC     IR_SampleCnt        ;Sample + 1

    MOV     C, P_IR_RX_temp     ;Save Last sample status
    MOV     F0, C
    MOV     C, P_IR_RX          ;Read current status
    MOV     P_IR_RX_temp, C

    JNB     F0, L_QuitIrRx              ;Pre-sample is high
    JB      P_IR_RX_temp, L_QuitIrRx    ;and current sample is low, so is fall edge

    MOV     R7, IR_SampleCnt            ;get the sample time
    MOV     IR_SampleCnt, #0            ;Clear the sample counter

    MOV     A, R7       ; if(SampleTime > D_IR_SYNC_MAX)    B_IR_Sync = 0
    SETB    C
    SUBB    A, #D_IR_SYNC_MAX
    JC      L_IR_RX_1
    CLR     B_IR_Sync       ;large than the Maxim SYNC time, then error
    RET
    
L_IR_RX_1:
    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    CLR     C
    SUBB    A, #D_IR_SYNC_MIN
    JC      L_IR_RX_2

    MOV     A, R7       ; else if(SampleTime >= D_IR_SYNC_MIN)
    SUBB    A, #D_IR_SYNC_DIVIDE
    JC      L_QuitIrRx
    SETB    B_IR_Sync           ;has received SYNC
    MOV     IR_BitCnt, #D_IR_BIT_NUMBER     ;Load bit number
    RET

L_IR_RX_2:
    JNB     B_IR_Sync, L_QuitIrRx   ;else if(B_IR_Sync), has received SYNC
    MOV     A, R7       ; if(SampleTime > D_IR_DATA_MAX)    B_IR_Sync = 0, data samlpe time too large
    SETB    C
    SUBB    A, #D_IR_DATA_MAX
    JC      L_IR_RX_3
    CLR     B_IR_Sync       ;data samlpe time too large
    RET

L_IR_RX_3:
    MOV     A, IR_DataShit      ;data shift right 1 bit
    CLR     C
    RRC     A
    MOV     IR_DataShit, A
    
    MOV     A, R7
    CLR     C
    SUBB    A, #D_IR_DATA_DIVIDE
    JC      L_IR_RX_4
    ORL     IR_DataShit, #080H  ;if(SampleTime >= D_IR_DATA_DIVIDE) IR_DataShit |= 0x80;    //devide data 0 or 1
L_IR_RX_4:
    DEC     IR_BitCnt
    MOV     A, IR_BitCnt
    JNZ     L_IR_RX_5           ;bit number is over?
    CLR     B_IR_Sync           ;Clear SYNC
    MOV     A, IR_DataShit      ;if(~IR_DataShit == IR_data)        //判断数据正反码
    CPL     A
    XRL     A, IR_data
    JNZ     L_QuitIrRx
    
    MOV     UserCodeH, IR_UserH
    MOV     UserCodeL, IR_UserL
    MOV     IR_code, IR_data
    SETB    B_IR_Press          ;数据有效
    RET

L_IR_RX_5:
    MOV     A, IR_BitCnt        ;else if((IR_BitCnt & 7)== 0)   one byte receive
    ANL     A, #07H
    JNZ     L_QuitIrRx
    MOV     IR_UserL, IR_UserH      ;Save the User code high byte
    MOV     IR_UserH, IR_data       ;Save the User code low byte
    MOV     IR_data,  IR_DataShit   ;Save the IR data byte
L_QuitIrRx:
    RET

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


    END

