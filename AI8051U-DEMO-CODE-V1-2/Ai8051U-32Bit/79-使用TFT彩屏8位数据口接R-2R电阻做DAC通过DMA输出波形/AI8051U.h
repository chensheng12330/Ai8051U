

#ifndef	__AI8051U_H
#define	__AI8051U_H

#include <intrins.h>

/*  BYTE Registers  */
sfr P0    = 0x80;
sfr SP    = 0x81;
sfr DPL   = 0x82;
sfr DPH   = 0x83;
sfr DPXL  = 0x84;
sfr	SPH   = 0x85;
sfr PCON  = 0x87;

sfr TCON = 0x88;
sfr TMOD = 0x89;
sfr TL0  = 0x8A;
sfr TL1  = 0x8B;
sfr TH0  = 0x8C;
sfr TH1  = 0x8D;
sfr	AUXR = 0x8E;
sfr INTCLKO  = 0x8F;

sfr P1    = 0x90;
sfr P1M1  = 0x91;	//P1M1.n,P1M0.n 	=00--->Standard,	01--->push-pull		实际上1T的都一样
sfr P1M0  = 0x92;	//					=10--->pure input,	11--->open drain
sfr P0M1  = 0x93;	//P0M1.n,P0M0.n 	=00--->Standard,	01--->push-pull
sfr P0M0  = 0x94;	//					=10--->pure input,	11--->open drain
sfr P2M1  = 0x95;	//P2M1.n,P2M0.n 	=00--->Standard,	01--->push-pull
sfr P2M0  = 0x96;	//					=10--->pure input,	11--->open drain
sfr	AUXR2 = 0x97;

sfr SCON  = 0x98;  //串口1控制寄存器
sfr SBUF  = 0x99;  //串口1数据寄存器
sfr S2CON = 0x9A;  //串口2控制寄存器
sfr S2BUF = 0x9B;  //串口2数据寄存器
sfr IRCBAND = 0x9D;	//IRC频段选择检测
sfr LIRTRIM = 0x9E;	//IRC频率微调寄存器
sfr IRTRIM  = 0x9F;	//IRC频率调整寄存器

sfr P2    = 0xA0;
sfr BUS_SPEED = 0xA1;
sfr P_SW1  = 0xA2;	//外设端口切换寄存器1
sfr VRTRIM = 0xa6;

sfr IE    = 0xA8;
sfr SADDR = 0xA9;
sfr WKTCL = 0xAA;  //唤醒定时器低字节
sfr WKTCH = 0xAB;  //唤醒定时器高字节
sfr	S3CON = 0xAC;  //串口4控制寄存器
sfr S3BUF = 0xAD;  //串口4数据寄存器
sfr TA    = 0xAE;  //DPTR时序控制寄存器
sfr IE2   = 0xAF;  //中断允许寄存器2

sfr P3    = 0xB0;
sfr P3M1  = 0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
sfr P3M0  = 0xB2;	//					=10--->pure input,	11--->open drain
sfr P4M1  = 0xB3;	//P4M1.n,P4M0.n 	=00--->Standard,	01--->push-pull
sfr P4M0  = 0xB4;	//					=10--->pure input,	11--->open drain
sfr IP2   = 0xB5;	//
sfr IP2H  = 0xB6;	//
sfr IPH   = 0xB7;

sfr IP        = 0xB8;	//中断优先级控制寄存器
sfr SADEN     = 0xB9;	//串口1从机地址屏蔽寄存器
sfr	P_SW2     = 0xBA;	//外设端口切换寄存器2
sfr	P_SW3     = 0xBB;	//外设端口切换寄存器3
sfr ADC_CONTR = 0xBC;	//ADC控制寄存器
sfr ADC_RES   = 0xBD;	//ADC结果高字节
sfr ADC_RESL  = 0xBE;	//ADC结果低字节
sfr	P_SW4     = 0xBF;	//外设端口切换寄存器4

sfr P4        = 0xC0;
sfr WDT_CONTR = 0xC1;
sfr IAP_DATA  = 0xC2;
sfr IAP_ADDRH = 0xC3;
sfr IAP_ADDRL = 0xC4;
sfr IAP_CMD   = 0xC5;
sfr IAP_TRIG  = 0xC6;
sfr IAP_CONTR = 0xC7;

sfr P5     = 0xC8;	//
sfr P5M1   = 0xC9;	//	P5M1.n,P5M0.n 	=00--->Standard,	01--->push-pull
sfr P5M0   = 0xCA;	//					=10--->pure input,	11--->open drain
sfr P6M1   = 0xCB;	//	P6M1.n,P6M0.n 	=00--->Standard,	01--->push-pull
sfr P6M0   = 0xCC;	//					=10--->pure input,	11--->open drain
sfr SPSTAT = 0xCD;	//SPI状态寄存器
sfr SPCTL  = 0xCE;	//SPI控制寄存器
sfr SPDAT  = 0xCF;	//SPI数据寄存器

sfr PSW    = 0xD0;	//程序状态寄存器
sfr	PSW1   = 0xD1;	//程序状态寄存器1
sfr	T4H    = 0xD2;
sfr	T4L    = 0xD3;
sfr	T3H    = 0xD4;
sfr	T3L    = 0xD5;
sfr	T2H    = 0xD6;
sfr	T2L    = 0xD7;

sfr	TH4    = 0xD2;
sfr	TL4    = 0xD3;
sfr	TH3    = 0xD4;
sfr	TL3    = 0xD5;
sfr	TH2    = 0xD6;
sfr	TL2    = 0xD7;

sfr USBCLK     = 0xDC;  //USB时钟控制寄存器
sfr T4T3M      = 0xDD;
sfr	ADCCFG     = 0xDE;  //ADC配置寄存器
sfr	IP3        = 0xDF;  //中断优先级控制寄存器3

sfr ACC    = 0xE0;
sfr	P7M1   = 0xE1;	//	P7M1.n,P7M0.n 	=00--->Standard,	01--->push-pull
sfr	P7M0   = 0xE2;	//					=10--->pure input,	11--->open drain
sfr	DPS    = 0xE3;	//DPTR指针选择寄存器
sfr	CMPCR1 = 0xE6;	//比较器控制寄存器1
sfr	CMPCR2 = 0xE7;	//比较器控制寄存器2

sfr	P6     = 0xE8;
sfr	WTST   = 0xE9;	//程序读取控制寄存器
sfr	CKCON  = 0xEA;  //XRAM控制寄存器
sfr	MXAX   = 0xEB;  //MOVX扩展地址寄存器
sfr USBDAT = 0xEC;	//USB数据寄存器
sfr	DMAIR  = 0xED;  //FMU DMA指令寄存器
sfr	IP3H   = 0xEE;	//高中断优先级控制寄存器3
sfr	AUXINTIF = 0xEF;	//扩展外部中断标志寄存器 B6-INT4IF, B5-INT3IF, B4-INT2IF, B2-T4IF, B1-T3IF, B0-T2IF

sfr B         = 0xF0;
sfr USBCON    = 0xF4;  //USB控制寄存器。
sfr	IAP_TPS   = 0xF5;  //IAP等待时间控制寄存器
sfr	IAP_ADDRE = 0xF6;  //IAP扩展高地址寄存器

sfr	P7       = 0xF8;
sfr USBADR   = 0xFC;  //USB地址寄存器
sfr	S4CON    = 0xFD;  //串口4控制寄存器
sfr	S4BUF    = 0xFE;  //串口4数据寄存器
sfr	RSTCFG   = 0xFF;  //复位配置寄存器


//如下特殊功能寄存器位于扩展RAM区域
//访问这些寄存器,需先将EAXFR设置为1,才可正常读写
//    EAXFR = 1; 或者 P_SW2 |= 0x80;

//========================== XFR: 0x7EF900~0x7EF9FF =========================================================
#define     QSPI_CR1                (*(unsigned char volatile far *)0x7ef900)	/* QSPI控制寄存器1 */
#define     QSPI_CR2                (*(unsigned char volatile far *)0x7ef901)	/* QSPI控制寄存器2 */
#define     QSPI_CR3                (*(unsigned char volatile far *)0x7ef902)	/* QSPI控制寄存器3 */
#define     QSPI_CR4                (*(unsigned char volatile far *)0x7ef903)	/* QSPI控制寄存器4 */
#define     QSPI_DCR1               (*(unsigned char volatile far *)0x7ef904)	/* QSPI器件配置寄存器1 */
#define     QSPI_DCR2               (*(unsigned char volatile far *)0x7ef905)	/* QSPI器件配置寄存器2 */
#define     QSPI_SR1                (*(unsigned char volatile far *)0x7ef906)	/* QSPI状态寄存器1 */
#define     QSPI_SR2                (*(unsigned char volatile far *)0x7ef907)	/* QSPI状态寄存器2 */
#define     QSPI_FCR                (*(unsigned char volatile far *)0x7ef908)	/* QSPI标志清零寄存器 */
#define     QSPI_HCR1               (*(unsigned char volatile far *)0x7ef909)	/* QSPI控制寄存器1 */
#define     QSPI_HCR2               (*(unsigned char volatile far *)0x7ef90a)	/* QSPI控制寄存器2 */

#define     QSPI_DLR1               (*(unsigned char volatile far *)0x7ef910)	/* QSPI数据长度寄存器1 */
#define     QSPI_DLR2               (*(unsigned char volatile far *)0x7ef911)	/* QSPI数据长度寄存器2 */
#define     QSPI_CCR1               (*(unsigned char volatile far *)0x7ef914)	/* QSPI通信配置寄存器1 */
#define     QSPI_CCR2               (*(unsigned char volatile far *)0x7ef915)	/* QSPI通信配置寄存器2 */
#define     QSPI_CCR3               (*(unsigned char volatile far *)0x7ef916)	/* QSPI通信配置寄存器3 */
#define     QSPI_CCR4               (*(unsigned char volatile far *)0x7ef917)	/* QSPI通信配置寄存器4 */
#define     QSPI_AR1                (*(unsigned char volatile far *)0x7ef918)	/* QSPI地址寄存器1 */
#define     QSPI_AR2                (*(unsigned char volatile far *)0x7ef919)	/* QSPI地址寄存器2 */
#define     QSPI_AR3                (*(unsigned char volatile far *)0x7ef91a)	/* QSPI地址寄存器3 */
#define     QSPI_AR4                (*(unsigned char volatile far *)0x7ef91b)	/* QSPI地址寄存器4 */
#define     QSPI_ABR                (*(unsigned char volatile far *)0x7ef91c)	/* QSPI交替字节寄存器1 */

#define     QSPI_DR                 (*(unsigned char volatile far *)0x7ef920)	/* QSPI数据寄存器 */
#define     QSPI_PSMKR1             (*(unsigned char volatile far *)0x7ef924)	/* QSPI状态屏蔽寄存器 */
#define     QSPI_PSMAR1             (*(unsigned char volatile far *)0x7ef928)	/* QSPI状态匹配寄存器 */
#define     QSPI_PIR1               (*(unsigned char volatile far *)0x7ef92c)	/* QSPI轮询间隔寄存器1 */
#define     QSPI_PIR2               (*(unsigned char volatile far *)0x7ef92d)	/* QSPI轮询间隔寄存器2 */

#define     PWMA_ENO2               (*(unsigned char volatile far *)0x7ef930)	/* PWMA输出使能寄存器2 */
#define     PWMA_IOAUX2             (*(unsigned char volatile far *)0x7ef931)	/* PWMA输出附加寄存器2 */
#define     PWMA_CR3                (*(unsigned char volatile far *)0x7ef932)	/* PWMA控制寄存器3 */
#define     PWMA_SR3                (*(unsigned char volatile far *)0x7ef933)	/* PWMA状态寄存器3 */
#define     PWMA_CCER3              (*(unsigned char volatile far *)0x7ef934)	/* PWMA捕获比较使能寄存器3 */
#define     PWMA_CCMR1X             (*(unsigned char volatile far *)0x7ef938)	/* PWMA捕获比较模式扩展寄存器1x */
#define     PWMA_CCMR2X             (*(unsigned char volatile far *)0x7ef939)	/* PWMA捕获比较模式扩展寄存器2x */
#define     PWMA_CCMR3X             (*(unsigned char volatile far *)0x7ef93a)	/* PWMA捕获比较模式扩展寄存器3x */
#define     PWMA_CCMR4X             (*(unsigned char volatile far *)0x7ef93b)	/* PWMA捕获比较模式扩展寄存器4x */
#define     PWMA_CCMR5              (*(unsigned char volatile far *)0x7ef93c)	/* PWMA捕获比较模式寄存器5 */
#define     PWMA_CCMR5X             (*(unsigned char volatile far *)0x7ef93d)	/* PWMA捕获比较模式扩展寄存器5x */
#define     PWMA_CCMR6              (*(unsigned char volatile far *)0x7ef93e)	/* PWMA捕获比较模式寄存器6 */
#define     PWMA_CCMR6X             (*(unsigned char volatile far *)0x7ef93f)	/* PWMA捕获比较模式扩展寄存器6x */
#define     PWMA_CCR5H              (*(unsigned char volatile far *)0x7ef940)	/* PWMA捕获比较寄存器5高字节 */
#define     PWMA_CCR5L              (*(unsigned char volatile far *)0x7ef941)	/* PWMA捕获比较寄存器5低字节 */
#define     PWMA_CCR5X              (*(unsigned char volatile far *)0x7ef942)	/* PWMA捕获比较扩展寄存器5 */
#define     PWMA_CCR6H              (*(unsigned char volatile far *)0x7ef943)	/* PWMA捕获比较寄存器6高字节 */
#define     PWMA_CCR6L              (*(unsigned char volatile far *)0x7ef944)	/* PWMA捕获比较寄存器6低字节 */
#define     PWMA_DER                (*(unsigned char volatile far *)0x7ef948)	/* PWMA_DMA使能控制寄存器 */
#define     PWMA_DBA                (*(unsigned char volatile far *)0x7ef949)	/* PWMA_DMA基址寄存器 */
#define     PWMA_DBL                (*(unsigned char volatile far *)0x7ef94a)	/* PWMA_DMA基址长度寄存器 */
#define     PWMA_DMACR              (*(unsigned char volatile far *)0x7ef94b)	/* PWMA_DMA控制寄存器 */

#define     DMA_PWMAT_CFG           (*(unsigned char volatile far *)0x7ef980)	/* PWMAT_DMA配置寄存器 */
#define     DMA_PWMAT_CR            (*(unsigned char volatile far *)0x7ef981)	/* PWMAT_DMA控制寄存器 */
#define     DMA_PWMAT_STA           (*(unsigned char volatile far *)0x7ef982)	/* PWMAT_DMA状态寄存器 */
#define     DMA_PWMAT_AMTH          (*(unsigned char volatile far *)0x7ef984)	/* PWMAT_DMA传输总字节数高字节 */
#define     DMA_PWMAT_AMT           (*(unsigned char volatile far *)0x7ef985)	/* PWMAT_DMA传输总字节数低字节 */
#define     DMA_PWMAT_DONEH         (*(unsigned char volatile far *)0x7ef986)	/* PWMAT_DMA传输完成字节数高字节 */
#define     DMA_PWMAT_DONE          (*(unsigned char volatile far *)0x7ef987)	/* PWMAT_DMA传输完成字节数低字节 */
#define     DMA_PWMAT_TXAH          (*(unsigned char volatile far *)0x7ef988)	/* PWMAT_DMA发送高地址 */
#define     DMA_PWMAT_TXAL          (*(unsigned char volatile far *)0x7ef989)	/* PWMAT_DMA发送低地址 */
#define     DMA_PWMA_ITVH           (*(unsigned char volatile far *)0x7ef98e)	/* PWMAT_DMA时间间隔寄存器高字节 */
#define     DMA_PWMA_ITVL           (*(unsigned char volatile far *)0x7ef98f)	/* PWMAT_DMA时间间隔寄存器低字节 */

#define     DMA_PWMAR_CFG           (*(unsigned char volatile far *)0x7ef990)	/* PWMAR_DMA配置寄存器 */
#define     DMA_PWMAR_CR            (*(unsigned char volatile far *)0x7ef991)	/* PWMAR_DMA控制寄存器 */
#define     DMA_PWMAR_STA           (*(unsigned char volatile far *)0x7ef992)	/* PWMAR_DMA状态寄存器 */
#define     DMA_PWMAR_AMTH          (*(unsigned char volatile far *)0x7ef994)	/* PWMAR_DMA传输总字节数高字节 */
#define     DMA_PWMAR_AMT           (*(unsigned char volatile far *)0x7ef995)	/* PWMAR_DMA传输总字节数低字节 */
#define     DMA_PWMAR_DONEH         (*(unsigned char volatile far *)0x7ef996)	/* PWMAR_DMA传输完成字节数高字节 */
#define     DMA_PWMAR_DONE          (*(unsigned char volatile far *)0x7ef997)	/* PWMAR_DMA传输完成字节数低字节 */
#define     DMA_PWMAR_RXAH          (*(unsigned char volatile far *)0x7ef998)	/* PWMAR_DMA接收高地址 */
#define     DMA_PWMAR_RXAL          (*(unsigned char volatile far *)0x7ef999)	/* PWMAR_DMA接收低地址 */

//========================== XFR: 0x7EFA00~0x7EFAFF =========================================================
#define DMA_M2M_CFG     (*(unsigned char volatile far *)0x7efa00)	/*  M2M_DMA配置寄存器 */
#define DMA_M2M_CR      (*(unsigned char volatile far *)0x7efa01)	/*  M2M_DMA控制寄存器 */
#define DMA_M2M_STA     (*(unsigned char volatile far *)0x7efa02)	/*  M2M_DMA状态寄存器 */
#define DMA_M2M_AMT     (*(unsigned char volatile far *)0x7efa03)	/*  M2M_DMA传输总字节数低字节 */
#define DMA_M2M_DONE    (*(unsigned char volatile far *)0x7efa04)	/*  M2M_DMA传输完成字节数低字节 */
#define DMA_M2M_TXAH    (*(unsigned char volatile far *)0x7efa05)	/*  M2M_DMA发送高地址 */
#define DMA_M2M_TXAL    (*(unsigned char volatile far *)0x7efa06)	/*  M2M_DMA发送低地址 */
#define DMA_M2M_RXAH    (*(unsigned char volatile far *)0x7efa07)	/*  M2M_DMA接收高地址 */
#define DMA_M2M_RXAL    (*(unsigned char volatile far *)0x7efa08)	/*  M2M_DMA接收地地址 */

#define DMA_ADC_CFG     (*(unsigned char volatile far *)0x7efa10)	/*  ADC_DMA配置寄存器 */
#define DMA_ADC_CR      (*(unsigned char volatile far *)0x7efa11)	/*  ADC_DMA控制寄存器 */
#define DMA_ADC_STA     (*(unsigned char volatile far *)0x7efa12)	/*  ADC_DMA状态寄存器 */
#define DMA_ADC_AMT     (*(unsigned char volatile far *)0x7efa13)	/*  ADC_DMA传输总字节数低字节 */
#define DMA_ADC_DONE    (*(unsigned char volatile far *)0x7efa14)	/*  ADC_DMA传输完成字节数低字节 */
#define DMA_ADC_RXAH    (*(unsigned char volatile far *)0x7efa17)	/*  ADC_DMA接收高地址 */
#define DMA_ADC_RXAL    (*(unsigned char volatile far *)0x7efa18)	/*  ADC_DMA接收地地址 */
#define DMA_ADC_CFG2    (*(unsigned char volatile far *)0x7efa19)	/*  ADC_DMA配置寄存器2 */
#define DMA_ADC_CHSW0   (*(unsigned char volatile far *)0x7efa1a)	/*  ADC_DMA通道使能寄存器0 */
#define DMA_ADC_CHSW1   (*(unsigned char volatile far *)0x7efa1b)	/*  ADC_DMA通道使能寄存器1 */
#define DMA_ADC_ITVH    (*(unsigned char volatile far *)0x7efa1e)	/*  ADC_DMA时间间隔寄存器高字节 */
#define DMA_ADC_ITVL    (*(unsigned char volatile far *)0x7efa1f)	/*  ADC_DMA时间间隔寄存器低字节 */

#define DMA_SPI_CFG     (*(unsigned char volatile far *)0x7efa20)	/*  SPI_DMA配置寄存器 */
#define DMA_SPI_CR      (*(unsigned char volatile far *)0x7efa21)	/*  SPI_DMA控制寄存器 */
#define DMA_SPI_STA     (*(unsigned char volatile far *)0x7efa22)	/*  SPI_DMA状态寄存器 */
#define DMA_SPI_AMT     (*(unsigned char volatile far *)0x7efa23)	/*  SPI_DMA传输总字节数低字节 */
#define DMA_SPI_DONE    (*(unsigned char volatile far *)0x7efa24)	/*  SPI_DMA传输完成字节数低字节 */
#define DMA_SPI_TXAH    (*(unsigned char volatile far *)0x7efa25)	/*  SPI_DMA发送高地址 */
#define DMA_SPI_TXAL    (*(unsigned char volatile far *)0x7efa26)	/*  SPI_DMA发送低地址 */
#define DMA_SPI_RXAH    (*(unsigned char volatile far *)0x7efa27)	/*  SPI_DMA接收高地址 */
#define DMA_SPI_RXAL    (*(unsigned char volatile far *)0x7efa28)	/*  SPI_DMA接收地地址 */
#define DMA_SPI_CFG2    (*(unsigned char volatile far *)0x7efa29)	/*  SPI_DMA配置寄存器2 */
#define DMA_SPI_ITVH    (*(unsigned char volatile far *)0x7efa2e)	/*  SPI_DMA时间间隔寄存器高字节 */
#define DMA_SPI_ITVL    (*(unsigned char volatile far *)0x7efa2f)	/*  SPI_DMA时间间隔寄存器低字节 */

#define DMA_UR1T_CFG    (*(unsigned char volatile far *)0x7efa30)	/*  UR1T_DMA配置寄存器 */
#define DMA_UR1T_CR     (*(unsigned char volatile far *)0x7efa31)	/*  UR1T_DMA控制寄存器 */
#define DMA_UR1T_STA    (*(unsigned char volatile far *)0x7efa32)	/*  UR1T_DMA状态寄存器 */
#define DMA_UR1T_AMT    (*(unsigned char volatile far *)0x7efa33)	/*  UR1T_DMA传输总字节数低字节 */
#define DMA_UR1T_DONE   (*(unsigned char volatile far *)0x7efa34)	/*  UR1T_DMA传输完成字节数低字节 */
#define DMA_UR1T_TXAH   (*(unsigned char volatile far *)0x7efa35)	/*  UR1T_DMA发送高地址 */
#define DMA_UR1T_TXAL   (*(unsigned char volatile far *)0x7efa36)	/*  UR1T_DMA发送低地址 */
#define DMA_UR1R_CFG    (*(unsigned char volatile far *)0x7efa38)	/*  UR1R_DMA配置寄存器 */
#define DMA_UR1R_CR     (*(unsigned char volatile far *)0x7efa39)	/*  UR1R_DMA控制寄存器 */
#define DMA_UR1R_STA    (*(unsigned char volatile far *)0x7efa3a)	/*  UR1R_DMA状态寄存器 */
#define DMA_UR1R_AMT    (*(unsigned char volatile far *)0x7efa3b)	/*  UR1R_DMA传输总字节数低字节 */
#define DMA_UR1R_DONE   (*(unsigned char volatile far *)0x7efa3c)	/*  UR1R_DMA传输完成字节数低字节 */
#define DMA_UR1R_RXAH   (*(unsigned char volatile far *)0x7efa3d)	/*  UR1R_DMA接收高地址 */
#define DMA_UR1R_RXAL   (*(unsigned char volatile far *)0x7efa3e)	/*  UR1R_DMA接收低地址 */

#define DMA_UR2T_CFG    (*(unsigned char volatile far *)0x7efa40)	/*  UR2T_DMA配置寄存器 */
#define DMA_UR2T_CR     (*(unsigned char volatile far *)0x7efa41)	/*  UR2T_DMA控制寄存器 */
#define DMA_UR2T_STA    (*(unsigned char volatile far *)0x7efa42)	/*  UR2T_DMA状态寄存器 */
#define DMA_UR2T_AMT    (*(unsigned char volatile far *)0x7efa43)	/*  UR2T_DMA传输总字节数低字节 */
#define DMA_UR2T_DONE   (*(unsigned char volatile far *)0x7efa44)	/*  UR2T_DMA传输完成字节数低字节 */
#define DMA_UR2T_TXAH   (*(unsigned char volatile far *)0x7efa45)	/*  UR2T_DMA发送高地址 */
#define DMA_UR2T_TXAL   (*(unsigned char volatile far *)0x7efa46)	/*  UR2T_DMA发送低地址 */
#define DMA_UR2R_CFG    (*(unsigned char volatile far *)0x7efa48)	/*  UR2R_DMA配置寄存器 */
#define DMA_UR2R_CR     (*(unsigned char volatile far *)0x7efa49)	/*  UR2R_DMA控制寄存器 */
#define DMA_UR2R_STA    (*(unsigned char volatile far *)0x7efa4a)	/*  UR2R_DMA状态寄存器 */
#define DMA_UR2R_AMT    (*(unsigned char volatile far *)0x7efa4b)	/*  UR2R_DMA传输总字节数低字节 */
#define DMA_UR2R_DONE   (*(unsigned char volatile far *)0x7efa4c)	/*  UR2R_DMA传输完成字节数低字节 */
#define DMA_UR2R_RXAH   (*(unsigned char volatile far *)0x7efa4d)	/*  UR2R_DMA接收高地址 */
#define DMA_UR2R_RXAL   (*(unsigned char volatile far *)0x7efa4e)	/*  UR2R_DMA接收低地址 */

#define DMA_UR3T_CFG    (*(unsigned char volatile far *)0x7efa50)	/*  UR3T_DMA配置寄存器 */
#define DMA_UR3T_CR     (*(unsigned char volatile far *)0x7efa51)	/*  UR3T_DMA控制寄存器 */
#define DMA_UR3T_STA    (*(unsigned char volatile far *)0x7efa52)	/*  UR3T_DMA状态寄存器 */
#define DMA_UR3T_AMT    (*(unsigned char volatile far *)0x7efa53)	/*  UR3T_DMA传输总字节数低字节 */
#define DMA_UR3T_DONE   (*(unsigned char volatile far *)0x7efa54)	/*  UR3T_DMA传输完成字节数低字节 */
#define DMA_UR3T_TXAH   (*(unsigned char volatile far *)0x7efa55)	/*  UR3T_DMA发送高地址 */
#define DMA_UR3T_TXAL   (*(unsigned char volatile far *)0x7efa56)	/*  UR3T_DMA发送低地址 */
#define DMA_UR3R_CFG    (*(unsigned char volatile far *)0x7efa58)	/*  UR3R_DMA配置寄存器 */
#define DMA_UR3R_CR     (*(unsigned char volatile far *)0x7efa59)	/*  UR3R_DMA控制寄存器 */
#define DMA_UR3R_STA    (*(unsigned char volatile far *)0x7efa5a)	/*  UR3R_DMA状态寄存器 */
#define DMA_UR3R_AMT    (*(unsigned char volatile far *)0x7efa5b)	/*  UR3R_DMA传输总字节数低字节 */
#define DMA_UR3R_DONE   (*(unsigned char volatile far *)0x7efa5c)	/*  UR3R_DMA传输完成字节数低字节 */
#define DMA_UR3R_RXAH   (*(unsigned char volatile far *)0x7efa5d)	/*  UR3R_DMA接收高地址 */
#define DMA_UR3R_RXAL   (*(unsigned char volatile far *)0x7efa5e)	/*  UR3R_DMA接收低地址 */

#define DMA_UR4T_CFG    (*(unsigned char volatile far *)0x7efa60)	/*  UR4T_DMA配置寄存器 */
#define DMA_UR4T_CR     (*(unsigned char volatile far *)0x7efa61)	/*  UR4T_DMA控制寄存器 */
#define DMA_UR4T_STA    (*(unsigned char volatile far *)0x7efa62)	/*  UR4T_DMA状态寄存器 */
#define DMA_UR4T_AMT    (*(unsigned char volatile far *)0x7efa63)	/*  UR4T_DMA传输总字节数低字节 */
#define DMA_UR4T_DONE   (*(unsigned char volatile far *)0x7efa64)	/*  UR4T_DMA传输完成字节数低字节 */
#define DMA_UR4T_TXAH   (*(unsigned char volatile far *)0x7efa65)	/*  UR4T_DMA发送高地址 */
#define DMA_UR4T_TXAL   (*(unsigned char volatile far *)0x7efa66)	/*  UR4T_DMA发送低地址 */
#define DMA_UR4R_CFG    (*(unsigned char volatile far *)0x7efa68)	/*  UR4R_DMA配置寄存器 */
#define DMA_UR4R_CR     (*(unsigned char volatile far *)0x7efa69)	/*  UR4R_DMA控制寄存器 */
#define DMA_UR4R_STA    (*(unsigned char volatile far *)0x7efa6a)	/*  UR4R_DMA状态寄存器 */
#define DMA_UR4R_AMT    (*(unsigned char volatile far *)0x7efa6b)	/*  UR4R_DMA传输总字节数低字节 */
#define DMA_UR4R_DONE   (*(unsigned char volatile far *)0x7efa6c)	/*  UR4R_DMA传输完成字节数低字节 */
#define DMA_UR4R_RXAH   (*(unsigned char volatile far *)0x7efa6d)	/*  UR4R_DMA接收高地址 */
#define DMA_UR4R_RXAL   (*(unsigned char volatile far *)0x7efa6e)	/*  UR4R_DMA接收低地址 */

#define DMA_LCM_CFG     (*(unsigned char volatile far *)0x7efa70)	/*  LCM_DMA配置寄存器 */
#define DMA_LCM_CR      (*(unsigned char volatile far *)0x7efa71)	/*  LCM_DMA控制寄存器 */
#define DMA_LCM_STA     (*(unsigned char volatile far *)0x7efa72)	/*  LCM_DMA状态寄存器 */
#define DMA_LCM_AMT     (*(unsigned char volatile far *)0x7efa73)	/*  LCM_DMA传输总字节数低字节 */
#define DMA_LCM_DONE    (*(unsigned char volatile far *)0x7efa74)	/*  LCM_DMA传输完成字节数低字节 */
#define DMA_LCM_TXAH    (*(unsigned char volatile far *)0x7efa75)	/*  LCM_DMA发送高地址 */
#define DMA_LCM_TXAL    (*(unsigned char volatile far *)0x7efa76)	/*  LCM_DMA发送低地址 */
#define DMA_LCM_RXAH    (*(unsigned char volatile far *)0x7efa77)	/*  LCM_DMA接收高地址 */
#define DMA_LCM_RXAL    (*(unsigned char volatile far *)0x7efa78)	/*  LCM_DMA接收地地址 */
#define DMA_LCM_ITVH    (*(unsigned char volatile far *)0x7efa7e)	/*  LCM_DMA时间间隔寄存器高字节 */
#define DMA_LCM_ITVL    (*(unsigned char volatile far *)0x7efa7f)	/*  LCM_DMA时间间隔寄存器低字节 */

#define DMA_M2M_AMTH    (*(unsigned char volatile far *)0x7efa80)	/*  M2M_DMA传输总字节数高字节 */
#define DMA_M2M_DONEH   (*(unsigned char volatile far *)0x7efa81)	/*  M2M_DMA传输完成字节数高字节 */
#define DMA_ADC_AMTH    (*(unsigned char volatile far *)0x7efa82)	/*  ADC_DMA传输总字节数高字节 */
#define DMA_ADC_DONEH   (*(unsigned char volatile far *)0x7efa83)	/*  ADC_DMA传输完成字节数高字节 */
#define DMA_SPI_AMTH    (*(unsigned char volatile far *)0x7efa84)	/*  SPI_DMA传输总字节数高字节 */
#define DMA_SPI_DONEH   (*(unsigned char volatile far *)0x7efa85)	/*  SPI_DMA传输完成字节数高字节 */
#define DMA_LCM_AMTH    (*(unsigned char volatile far *)0x7efa86)	/*  LCM_DMA传输总字节数高字节 */
#define DMA_LCM_DONEH   (*(unsigned char volatile far *)0x7efa87)	/*  LCM_DMA传输完成字节数高字节 */
#define DMA_UR1T_AMTH   (*(unsigned char volatile far *)0x7efa88)	/*  UR1T_DMA传输总字节数高字节 */
#define DMA_UR1T_DONEH  (*(unsigned char volatile far *)0x7efa89)	/*  UR1T_DMA传输完成字节数高字节 */
#define DMA_UR1R_AMTH   (*(unsigned char volatile far *)0x7efa8a)	/*  UR1R_DMA传输总字节数高字节 */
#define DMA_UR1R_DONEH  (*(unsigned char volatile far *)0x7efa8b)	/*  UR1R_DMA传输完成字节数高字节 */
#define DMA_UR2T_AMTH   (*(unsigned char volatile far *)0x7efa8c)	/*  UR2T_DMA传输总字节数高字节 */
#define DMA_UR2T_DONEH  (*(unsigned char volatile far *)0x7efa8d)	/*  UR2T_DMA传输完成字节数高字节 */
#define DMA_UR2R_AMTH   (*(unsigned char volatile far *)0x7efa8e)	/*  UR2R_DMA传输总字节数高字节 */
#define DMA_UR2R_DONEH  (*(unsigned char volatile far *)0x7efa8f)	/*  UR2R_DMA传输完成字节数高字节 */
#define DMA_UR3T_AMTH   (*(unsigned char volatile far *)0x7efa90)	/*  UR3T_DMA传输总字节数高字节 */
#define DMA_UR3T_DONEH  (*(unsigned char volatile far *)0x7efa91)	/*  UR3T_DMA传输完成字节数高字节 */
#define DMA_UR3R_AMTH   (*(unsigned char volatile far *)0x7efa92)	/*  UR3R_DMA传输总字节数高字节 */
#define DMA_UR3R_DONEH  (*(unsigned char volatile far *)0x7efa93)	/*  UR3R_DMA传输完成字节数高字节 */
#define DMA_UR4T_AMTH   (*(unsigned char volatile far *)0x7efa94)	/*  UR4T_DMA传输总字节数高字节 */
#define DMA_UR4T_DONEH  (*(unsigned char volatile far *)0x7efa95)	/*  UR4T_DMA传输完成字节数高字节 */
#define DMA_UR4R_AMTH   (*(unsigned char volatile far *)0x7efa96)	/*  UR4R_DMA传输总字节数高字节 */
#define DMA_UR4R_DONEH  (*(unsigned char volatile far *)0x7efa97)	/*  UR4R_DMA传输完成字节数高字节 */

#define DMA_I2CT_CFG    (*(unsigned char volatile far *)0x7efa98)	/*  I2CT_DMA配置寄存器 */
#define DMA_I2CT_CR     (*(unsigned char volatile far *)0x7efa99)	/*  I2CT_DMA控制寄存器 */
#define DMA_I2CT_STA    (*(unsigned char volatile far *)0x7efa9a)	/*  I2CT_DMA状态寄存器 */
#define DMA_I2CT_AMT    (*(unsigned char volatile far *)0x7efa9b)	/*  I2CT_DMA传输总字节数低字节   */
#define DMA_I2CT_DONE   (*(unsigned char volatile far *)0x7efa9c)	/*  I2CT_DMA传输完成字节数低字节 */
#define DMA_I2CT_TXAH   (*(unsigned char volatile far *)0x7efa9d)	/*  I2CT_DMA发送高地址 */
#define DMA_I2CT_TXAL   (*(unsigned char volatile far *)0x7efa9e)	/*  I2CT_DMA发送低地址 */
#define DMA_I2CR_CFG    (*(unsigned char volatile far *)0x7efaa0)	/*  I2CR_DMA配置寄存器 */
#define DMA_I2CR_CR     (*(unsigned char volatile far *)0x7efaa1)	/*  I2CR_DMA控制寄存器 */
#define DMA_I2CR_STA    (*(unsigned char volatile far *)0x7efaa2)	/*  I2CR_DMA状态寄存器 */
#define DMA_I2CR_AMT    (*(unsigned char volatile far *)0x7efaa3)	/*  I2CR_DMA传输总字节数低字节   */
#define DMA_I2CR_DONE   (*(unsigned char volatile far *)0x7efaa4)	/*  I2CR_DMA传输完成字节数低字节 */
#define DMA_I2CR_RXAH   (*(unsigned char volatile far *)0x7efaa5)	/*  I2CR_DMA接收高地址 */
#define DMA_I2CR_RXAL   (*(unsigned char volatile far *)0x7efaa6)	/*  I2CR_DMA接收低地址 */

#define DMA_I2CT_AMTH   (*(unsigned char volatile far *)0x7efaa8)	/*  I2CT_DMA传输总字节数高字节   */
#define DMA_I2CT_DONEH  (*(unsigned char volatile far *)0x7efaa9)	/*  I2CT_DMA传输完成字节数高字节 */
#define DMA_I2CR_AMTH   (*(unsigned char volatile far *)0x7efaaa)	/*  I2CR_DMA传输总字节数高字节   */
#define DMA_I2CR_DONEH  (*(unsigned char volatile far *)0x7efaab)	/*  I2CR_DMA传输完成字节数高字节 */
#define DMA_I2C_CR      (*(unsigned char volatile far *)0x7efaad)	/*  I2C_DMA控制寄存器 */
#define DMA_I2C_ST1     (*(unsigned char volatile far *)0x7efaae)	/*  I2C_DMA状态寄存器 */
#define DMA_I2C_ST2     (*(unsigned char volatile far *)0x7efaaf)	/*  I2C_DMA状态寄存器 */

#define DMA_I2ST_CFG    (*(unsigned char volatile far *)0x7efab0)	/*  I2ST_DMA配置寄存器 */
#define DMA_I2ST_CR     (*(unsigned char volatile far *)0x7efab1)	/*  I2ST_DMA控制寄存器 */
#define DMA_I2ST_STA    (*(unsigned char volatile far *)0x7efab2)	/*  I2ST_DMA状态寄存器 */
#define DMA_I2ST_AMT    (*(unsigned char volatile far *)0x7efab3)	/*  I2ST_DMA传输总字节数低字节   */
#define DMA_I2ST_DONE   (*(unsigned char volatile far *)0x7efab4)	/*  I2ST_DMA传输完成字节数低字节 */
#define DMA_I2ST_TXAH   (*(unsigned char volatile far *)0x7efab5)	/*  I2ST_DMA发送高地址 */
#define DMA_I2ST_TXAL   (*(unsigned char volatile far *)0x7efab6)	/*  I2ST_DMA发送低地址 */
#define DMA_I2SR_CFG    (*(unsigned char volatile far *)0x7efab8)	/*  I2SR_DMA配置寄存器 */
#define DMA_I2SR_CR     (*(unsigned char volatile far *)0x7efab9)	/*  I2SR_DMA控制寄存器 */
#define DMA_I2SR_STA    (*(unsigned char volatile far *)0x7efaba)	/*  I2SR_DMA状态寄存器 */
#define DMA_I2SR_AMT    (*(unsigned char volatile far *)0x7efabb)	/*  I2SR_DMA传输总字节数低字节   */
#define DMA_I2SR_DONE   (*(unsigned char volatile far *)0x7efabc)	/*  I2SR_DMA传输完成字节数低字节 */
#define DMA_I2SR_RXAH   (*(unsigned char volatile far *)0x7efabd)	/*  I2SR_DMA接收高地址 */
#define DMA_I2SR_RXAL   (*(unsigned char volatile far *)0x7efabe)	/*  I2SR_DMA接收低地址 */

#define DMA_I2ST_AMTH   (*(unsigned char volatile far *)0x7efac0)	/*  I2ST_DMA传输总字节数高字节 */
#define DMA_I2ST_DONEH  (*(unsigned char volatile far *)0x7efac1)	/*  I2ST_DMA传输完成字节数高字节 */
#define DMA_I2SR_AMTH   (*(unsigned char volatile far *)0x7efac2)	/*  I2SR_DMA传输总字节数高字节 */
#define DMA_I2SR_DONEH  (*(unsigned char volatile far *)0x7efac3)	/*  I2SR_DMA传输完成字节数高字节 */
#define DMA_I2C_ITVH    (*(unsigned char volatile far *)0x7efac4)	/*  I2C_DMA时间间隔寄存器高字节 */
#define DMA_I2C_ITVL    (*(unsigned char volatile far *)0x7efac5)	/*  I2C_DMA时间间隔寄存器低字节 */
#define DMA_I2S_ITVH    (*(unsigned char volatile far *)0x7efac6)	/*  I2S_DMA时间间隔寄存器高字节 */
#define DMA_I2S_ITVL    (*(unsigned char volatile far *)0x7efac7)	/*  I2S_DMA时间间隔寄存器低字节 */
#define DMA_UR1_ITVH    (*(unsigned char volatile far *)0x7efac8)	/*  UR1_DMA时间间隔寄存器高字节 */
#define DMA_UR1_ITVL    (*(unsigned char volatile far *)0x7efac9)	/*  UR1_DMA时间间隔寄存器低字节 */
#define DMA_UR2_ITVH    (*(unsigned char volatile far *)0x7efaca)	/*  UR2_DMA时间间隔寄存器高字节 */
#define DMA_UR2_ITVL    (*(unsigned char volatile far *)0x7efacb)	/*  UR2_DMA时间间隔寄存器低字节 */
#define DMA_UR3_ITVH    (*(unsigned char volatile far *)0x7efacc)	/*  UR3_DMA时间间隔寄存器高字节 */
#define DMA_UR3_ITVL    (*(unsigned char volatile far *)0x7efacd)	/*  UR3_DMA时间间隔寄存器低字节 */
#define DMA_UR4_ITVH    (*(unsigned char volatile far *)0x7eface)	/*  UR4_DMA时间间隔寄存器高字节 */
#define DMA_UR4_ITVL    (*(unsigned char volatile far *)0x7efacf)	/*  UR4_DMA时间间隔寄存器低字节 */

#define DMA_QSPI_CFG    (*(unsigned char volatile far *)0x7efad0)	/*  QSPI_DMA配置寄存器 */
#define DMA_QSPI_CR     (*(unsigned char volatile far *)0x7efad1)	/*  QSPI_DMA控制寄存器 */
#define DMA_QSPI_STA    (*(unsigned char volatile far *)0x7efad2)	/*  QSPI_DMA状态寄存器 */
#define DMA_QSPI_AMT    (*(unsigned char volatile far *)0x7efad3)	/*  QSPI_DMA传输总字节数低字节   */
#define DMA_QSPI_DONE   (*(unsigned char volatile far *)0x7efad4)	/*  QSPI_DMA传输完成字节数低字节 */
#define DMA_QSPI_TXAH   (*(unsigned char volatile far *)0x7efad5)	/*  QSPI_DMA发送高地址 */
#define DMA_QSPI_TXAL   (*(unsigned char volatile far *)0x7efad6)	/*  QSPI_DMA发送低地址 */
#define DMA_QSPI_RXAH   (*(unsigned char volatile far *)0x7efad7)	/*  QSPI_DMA接收高地址 */
#define DMA_QSPI_RXAL   (*(unsigned char volatile far *)0x7efad8)	/*  QSPI_DMA接收低地址 */
#define DMA_QSPI_AMTH   (*(unsigned char volatile far *)0x7efadb)	/*  QSPI_DMA传输总字节数高字节   */
#define DMA_QSPI_DONEH  (*(unsigned char volatile far *)0x7efadc)	/*  QSPI_DMA传输完成字节数高字节 */
#define DMA_QSPI_ITVH   (*(unsigned char volatile far *)0x7efade)	/*  QSPI_DMA时间间隔寄存器高字节 */
#define DMA_QSPI_ITVL   (*(unsigned char volatile far *)0x7efadf)	/*  QSPI_DMA时间间隔寄存器低字节 */

#define DMA_P2P_CR1     (*(unsigned char volatile far *)0x7efaf0)	/*  P2P_DMA控制寄存器1 */
#define DMA_P2P_CR2     (*(unsigned char volatile far *)0x7efaf1)	/*  P2P_DMA控制寄存器2 */
#define DMA_ARB_CFG     (*(unsigned char volatile far *)0x7efaf8)	/*  DMA总裁配置寄存器 */
#define DMA_ARB_STA     (*(unsigned char volatile far *)0x7efaf9)	/*  DMA总裁状态寄存器 */

//========================== XFR: 0x7EFB00~0x7EFBFF =========================================================
#define HSPWMA_CFG  (*(unsigned char volatile far *)0x7efbf0)	/*  高速PWMA配置寄存器 */
#define HSPWMA_ADR  (*(unsigned char volatile far *)0x7efbf1)	/*  高速PWMA地址寄存器低字节 */
#define HSPWMA_DAT  (*(unsigned char volatile far *)0x7efbf2)	/*  高速PWMA数据寄存器 */
#define HSPWMA_ADRH (*(unsigned char volatile far *)0x7efbf3)	/*  高速PWMA地址寄存器高字节 */
#define HSPWMB_CFG  (*(unsigned char volatile far *)0x7efbf4)	/*  高速PWMB配置寄存器 */
#define HSPWMB_ADR  (*(unsigned char volatile far *)0x7efbf5)	/*  高速PWMB地址寄存器 */
#define HSPWMB_DAT  (*(unsigned char volatile far *)0x7efbf6)	/*  高速PWMB数据寄存器 */
#define HSPWMB_ADRH (*(unsigned char volatile far *)0x7efbf7)	/*  高速PWMB地址寄存器高字节 */
#define HSSPI_CFG   (*(unsigned char volatile far *)0x7efbf8)	/*  高速SPI配置寄存器 */
#define HSSPI_CFG2  (*(unsigned char volatile far *)0x7efbf9)	/*  高速SPI配置寄存器2 */
#define HSSPI_STA   (*(unsigned char volatile far *)0x7efbfa)	/*  高速SPI状态寄存器 */
#define HSSPI_PSCR  (*(unsigned char volatile far *)0x7efbfb)	/*  高速SPI时钟分频寄存器 */

//========================== XFR: 0x7EFD00~0x7EFDFF =========================================================
#define P0INTE      (*(unsigned char volatile far *)0x7efd00)	/* P0口中断使能寄存器 */
#define P1INTE      (*(unsigned char volatile far *)0x7efd01)	/* P1口中断使能寄存器 */
#define P2INTE      (*(unsigned char volatile far *)0x7efd02)	/* P2口中断使能寄存器 */
#define P3INTE      (*(unsigned char volatile far *)0x7efd03)	/* P3口中断使能寄存器 */
#define P4INTE      (*(unsigned char volatile far *)0x7efd04)	/* P4口中断使能寄存器 */
#define P5INTE      (*(unsigned char volatile far *)0x7efd05)	/* P5口中断使能寄存器 */
#define P6INTE      (*(unsigned char volatile far *)0x7efd06)	/* P6口中断使能寄存器 */
#define P7INTE      (*(unsigned char volatile far *)0x7efd07)	/* P7口中断使能寄存器 */

#define P0INTF      (*(unsigned char volatile far *)0x7efd10)	/* P0口中断标志寄存器 */
#define P1INTF      (*(unsigned char volatile far *)0x7efd11)	/* P1口中断标志寄存器 */
#define P2INTF      (*(unsigned char volatile far *)0x7efd12)	/* P2口中断标志寄存器 */
#define P3INTF      (*(unsigned char volatile far *)0x7efd13)	/* P3口中断标志寄存器 */
#define P4INTF      (*(unsigned char volatile far *)0x7efd14)	/* P4口中断标志寄存器 */
#define P5INTF      (*(unsigned char volatile far *)0x7efd15)	/* P5口中断标志寄存器 */
#define P6INTF      (*(unsigned char volatile far *)0x7efd16)	/* P6口中断标志寄存器 */
#define P7INTF      (*(unsigned char volatile far *)0x7efd17)	/* P7口中断标志寄存器 */

#define P0IM0       (*(unsigned char volatile far *)0x7efd20)	/* P0口中断模式寄存器0 */
#define P1IM0       (*(unsigned char volatile far *)0x7efd21)	/* P1口中断模式寄存器0 */
#define P2IM0       (*(unsigned char volatile far *)0x7efd22)	/* P2口中断模式寄存器0 */
#define P3IM0       (*(unsigned char volatile far *)0x7efd23)	/* P3口中断模式寄存器0 */
#define P4IM0       (*(unsigned char volatile far *)0x7efd24)	/* P4口中断模式寄存器0 */
#define P5IM0       (*(unsigned char volatile far *)0x7efd25)	/* P5口中断模式寄存器0 */
#define P6IM0       (*(unsigned char volatile far *)0x7efd26)	/* P6口中断模式寄存器0 */
#define P7IM0       (*(unsigned char volatile far *)0x7efd27)	/* P7口中断模式寄存器0 */

#define P0IM1       (*(unsigned char volatile far *)0x7efd30)	/* P0口中断模式寄存器1 */
#define P1IM1       (*(unsigned char volatile far *)0x7efd31)	/* P1口中断模式寄存器1 */
#define P2IM1       (*(unsigned char volatile far *)0x7efd32)	/* P2口中断模式寄存器1 */
#define P3IM1       (*(unsigned char volatile far *)0x7efd33)	/* P3口中断模式寄存器1 */
#define P4IM1       (*(unsigned char volatile far *)0x7efd34)	/* P4口中断模式寄存器1 */
#define P5IM1       (*(unsigned char volatile far *)0x7efd35)	/* P5口中断模式寄存器1 */
#define P6IM1       (*(unsigned char volatile far *)0x7efd36)	/* P6口中断模式寄存器1 */
#define P7IM1       (*(unsigned char volatile far *)0x7efd37)	/* P7口中断模式寄存器1 */

#define INTE_0       0x01	/* 使能Px.0口中断功能 */
#define INTE_1       0x02	/* 使能Px.1口中断功能 */
#define INTE_2       0x04	/* 使能Px.2口中断功能 */
#define INTE_3       0x08	/* 使能Px.3口中断功能 */
#define INTE_4       0x10	/* 使能Px.4口中断功能 */
#define INTE_5       0x20	/* 使能Px.5口中断功能 */
#define INTE_6       0x40	/* 使能Px.6口中断功能 */
#define INTE_7       0x80	/* 使能Px.7口中断功能 */

#define INTF_0       0x01	/* 使能Px.0口中断标志 */
#define INTF_1       0x02	/* 使能Px.1口中断标志 */
#define INTF_2       0x04	/* 使能Px.2口中断标志 */
#define INTF_3       0x08	/* 使能Px.3口中断标志 */
#define INTF_4       0x10	/* 使能Px.4口中断标志 */
#define INTF_5       0x20	/* 使能Px.5口中断标志 */
#define INTF_6       0x40	/* 使能Px.6口中断标志 */
#define INTF_7       0x80	/* 使能Px.7口中断标志 */

#define	INT_FALL	0		/* 下降沿中断   */
#define	INT_RISE	1		/* 上升沿中断   */
#define	INT_LOW		2		/* 低电平中断   */
#define	INT_HIGH	3		/* 高电平中断   */

#define P0WKUE      (*(unsigned char volatile far *)0x7efd40)	/* P0口中断唤醒使能寄存器 */
#define P1WKUE      (*(unsigned char volatile far *)0x7efd41)	/* P1口中断唤醒使能寄存器 */
#define P2WKUE      (*(unsigned char volatile far *)0x7efd42)	/* P2口中断唤醒使能寄存器 */
#define P3WKUE      (*(unsigned char volatile far *)0x7efd43)	/* P3口中断唤醒使能寄存器 */
#define P4WKUE      (*(unsigned char volatile far *)0x7efd44)	/* P4口中断唤醒使能寄存器 */
#define P5WKUE      (*(unsigned char volatile far *)0x7efd45)	/* P5口中断唤醒使能寄存器 */
#define P6WKUE      (*(unsigned char volatile far *)0x7efd46)	/* P6口中断唤醒使能寄存器 */
#define P7WKUE      (*(unsigned char volatile far *)0x7efd47)	/* P7口中断唤醒使能寄存器 */

#define CCAPM2      (*(unsigned char volatile far *)0x7efd50)	/* PCA模块2模式控制寄存器 */
#define CCAP2L      (*(unsigned char volatile far *)0x7efd51)	/* PCA模块2低字节 */
#define CCAP2H      (*(unsigned char volatile far *)0x7efd52)	/* PCA模块2高字节 */
#define PCA_PWM2    (*(unsigned char volatile far *)0x7efd53)	/* PCA模块2的PWM模式寄存器 */
#define CCAPM3      (*(unsigned char volatile far *)0x7efd54)	/* PCA模块3模式控制寄存器 */
#define CCAP3L      (*(unsigned char volatile far *)0x7efd55)	/* PCA模块3低字节 */
#define CCAP3H      (*(unsigned char volatile far *)0x7efd56)	/* PCA模块3高字节 */
#define PCA_PWM3    (*(unsigned char volatile far *)0x7efd57)	/* PCA模块3的PWM模式寄存器 */
#define CCAPM0      (*(unsigned char volatile far *)0x7efd58)	/* PCA模块0模式控制寄存器 */
#define CCAP0L      (*(unsigned char volatile far *)0x7efd59)	/* PCA模块0低字节 */
#define CCAP0H      (*(unsigned char volatile far *)0x7efd5a)	/* PCA模块0高字节 */
#define PCA_PWM0    (*(unsigned char volatile far *)0x7efd5b)	/* PCA模块0的PWM模式寄存器 */
#define CCAPM1      (*(unsigned char volatile far *)0x7efd5c)	/* PCA模块1模式控制寄存器 */
#define CCAP1L      (*(unsigned char volatile far *)0x7efd5d)	/* PCA模块1低字节 */
#define CCAP1H      (*(unsigned char volatile far *)0x7efd5e)	/* PCA模块1高字节 */
#define PCA_PWM1    (*(unsigned char volatile far *)0x7efd5f)	/* PCA模块1的PWM模式寄存器 */

#define PINIPL      (*(unsigned char volatile far *)0x7efd60)	/* IO口中断优先级低寄存器 */
#define PINIPH      (*(unsigned char volatile far *)0x7efd61)	/* IO口中断优先级高寄存器 */
#define CCON        (*(unsigned char volatile far *)0x7efd64)	/* PCA控制寄存器 */
#define CL          (*(unsigned char volatile far *)0x7efd65)	/* PCA计数器低字节 */
#define CH          (*(unsigned char volatile far *)0x7efd66)	/* PCA计数器高字节 */
#define CMOD        (*(unsigned char volatile far *)0x7efd67)	/* PCA模式寄存器 */

#define UR1TOCR     (*(unsigned char volatile far *)0x7efd70)	/* 串口1超时控制寄存器 */
#define UR1TOSR     (*(unsigned char volatile far *)0x7efd71)	/* 串口1超时状态寄存器 */
#define UR1TOTH     (*(unsigned char volatile far *)0x7efd72)	/* 串口1超时长度控制寄存器高字节 */
#define UR1TOTL     (*(unsigned char volatile far *)0x7efd73)	/* 串口1超时长度控制寄存器低字节 */
#define UR2TOCR     (*(unsigned char volatile far *)0x7efd74)	/* 串口2超时控制寄存器 */
#define UR2TOSR     (*(unsigned char volatile far *)0x7efd75)	/* 串口2超时状态寄存器 */
#define UR2TOTH     (*(unsigned char volatile far *)0x7efd76)	/* 串口2超时长度控制寄存器高字节 */
#define UR2TOTL     (*(unsigned char volatile far *)0x7efd77)	/* 串口2超时长度控制寄存器低字节 */
#define UR3TOCR     (*(unsigned char volatile far *)0x7efd78)	/* 串口3超时控制寄存器 */
#define UR3TOSR     (*(unsigned char volatile far *)0x7efd79)	/* 串口3超时状态寄存器 */
#define UR3TOTH     (*(unsigned char volatile far *)0x7efd7a)	/* 串口3超时长度控制寄存器高字节 */
#define UR3TOTL     (*(unsigned char volatile far *)0x7efd7b)	/* 串口3超时长度控制寄存器低字节 */
#define UR4TOCR     (*(unsigned char volatile far *)0x7efd7c)	/* 串口4超时控制寄存器 */
#define UR4TOSR     (*(unsigned char volatile far *)0x7efd7d)	/* 串口4超时状态寄存器 */
#define UR4TOTH     (*(unsigned char volatile far *)0x7efd7e)	/* 串口4超时长度控制寄存器高字节 */
#define UR4TOTL     (*(unsigned char volatile far *)0x7efd7f)	/* 串口4超时长度控制寄存器低字节 */

#define SPITOCR     (*(unsigned char volatile far *)0x7efd80)	/* SPI超时控制寄存器 */
#define SPITOSR     (*(unsigned char volatile far *)0x7efd81)	/* SPI超时状态寄存器 */
#define SPITOTH     (*(unsigned char volatile far *)0x7efd82)	/* SPI超时长度控制寄存器高字节 */
#define SPITOTL     (*(unsigned char volatile far *)0x7efd83)	/* SPI超时长度控制寄存器低字节 */
#define I2CTOCR     (*(unsigned char volatile far *)0x7efd84)	/* I2C超时控制寄存器 */
#define I2CTOSR     (*(unsigned char volatile far *)0x7efd85)	/* I2C超时状态寄存器 */
#define I2CTOTH     (*(unsigned char volatile far *)0x7efd86)	/* I2C超时长度控制寄存器高字节 */
#define I2CTOTL     (*(unsigned char volatile far *)0x7efd87)	/* I2C超时长度控制寄存器低字节 */

#define UR1TOTE     (*(unsigned char volatile far *)0x7efd88)	/* 串口1接收超时长度寄存器 */
#define UR2TOTE     (*(unsigned char volatile far *)0x7efd89)	/* 串口2接收超时长度寄存器 */
#define UR3TOTE     (*(unsigned char volatile far *)0x7efd8a)	/* 串口3接收超时长度寄存器 */
#define UR4TOTE     (*(unsigned char volatile far *)0x7efd8b)	/* 串口4接收超时长度寄存器 */
#define SPITOTE     (*(unsigned char volatile far *)0x7efd8c)	/* SPI接收超时长度寄存器 */
#define I2CTOTE     (*(unsigned char volatile far *)0x7efd8d)	/* I2C接收超时长度寄存器 */

#define I2SCR       (*(unsigned char volatile far *)0x7efd98)	/* I2S控制寄存器 */
#define I2SSR       (*(unsigned char volatile far *)0x7efd99)	/* I2S状态寄存器 */
#define I2SDRH      (*(unsigned char volatile far *)0x7efd9a)	/* I2S数据寄存器高字节 先读写*/
#define I2SDRL      (*(unsigned char volatile far *)0x7efd9b)	/* I2S数据寄存器低字节 后读写*/
#define I2SPRH      (*(unsigned char volatile far *)0x7efd9c)	/* I2S分频寄存器高字节 分频系数=I2SPRL*2+(I2SPRH & 1) */
#define I2SPRL      (*(unsigned char volatile far *)0x7efd9d)	/* I2S分频寄存器低字节 */
#define I2SCFGH     (*(unsigned char volatile far *)0x7efd9e)	/* I2S配置寄存器高字节 */
#define I2SCFGL     (*(unsigned char volatile far *)0x7efd9f)	/* I2S配置寄存器低字节 */
#define I2SMD       (*(unsigned char volatile far *)0x7efda0)	/* I2S从模式控制寄存器 */
#define I2SMCKDIV   (*(unsigned char volatile far *)0x7efda1)	/* I2S_MCLK分频寄存器 */

#define CRECR       (*(unsigned char volatile far *)0x7efda8)	/* CRE控制寄存器 */
#define CRECNTH     (*(unsigned char volatile far *)0x7efda9)	/* CRE校准目标寄存器高字节 */
#define CRECNTL     (*(unsigned char volatile far *)0x7efdaa)	/* CRE校准目标寄存器低字节 */
#define CRERES      (*(unsigned char volatile far *)0x7efdab)	/* CRE分辨率控制寄存器 */

#define S2CFG       (*(unsigned char volatile far *)0x7efdb4)	/* 串口2配置寄存器 */
#define S2ADDR      (*(unsigned char volatile far *)0x7efdb5)	/* 串口2从机地址寄存器 */
#define S2ADEN      (*(unsigned char volatile far *)0x7efdb6)	/* 串口2从机地址屏蔽寄存器 */

#define USARTCR1    (*(unsigned char volatile far *)0x7efdc0)	/* 串口1控制寄存器1 */
#define USARTCR2    (*(unsigned char volatile far *)0x7efdc1)	/* 串口1控制寄存器2 */
#define USARTCR3    (*(unsigned char volatile far *)0x7efdc2)	/* 串口1控制寄存器3 */
#define USARTCR4    (*(unsigned char volatile far *)0x7efdc3)	/* 串口1控制寄存器4 */
#define USARTCR5    (*(unsigned char volatile far *)0x7efdc4)	/* 串口1控制寄存器5 */
#define USARTGTR    (*(unsigned char volatile far *)0x7efdc5)	/* 串口1保护时间寄存器 */
#define USARTBRH    (*(unsigned char volatile far *)0x7efdc6)	/* 串口1波特率寄存器高字节 */
#define USARTBRL    (*(unsigned char volatile far *)0x7efdc7)	/* 串口1波特率寄存器低字节 */

#define USART2CR1   (*(unsigned char volatile far *)0x7efdc8)	/* 串口2控制寄存器1 */
#define USART2CR2   (*(unsigned char volatile far *)0x7efdc9)	/* 串口2控制寄存器2 */
#define USART2CR3   (*(unsigned char volatile far *)0x7efdca)	/* 串口2控制寄存器3 */
#define USART2CR4   (*(unsigned char volatile far *)0x7efdcb)	/* 串口2控制寄存器4 */
#define USART2CR5   (*(unsigned char volatile far *)0x7efdcc)	/* 串口2控制寄存器5 */
#define USART2GTR   (*(unsigned char volatile far *)0x7efdcd)	/* 串口2保护时间寄存器 */
#define USART2BRH   (*(unsigned char volatile far *)0x7efdce)	/* 串口2波特率寄存器高字节 */
#define USART2BRL   (*(unsigned char volatile far *)0x7efdcf)	/* 串口2波特率寄存器低字节 */

#define CHIPID0     (*(unsigned char volatile far *)0x7efde0) /* 硬件ID0 */
#define CHIPID1     (*(unsigned char volatile far *)0x7efde1) /* 硬件ID1 */
#define CHIPID2     (*(unsigned char volatile far *)0x7efde2) /* 硬件ID2 */
#define CHIPID3     (*(unsigned char volatile far *)0x7efde3) /* 硬件ID3 */
#define CHIPID4     (*(unsigned char volatile far *)0x7efde4) /* 硬件ID4 */
#define CHIPID5     (*(unsigned char volatile far *)0x7efde5) /* 硬件ID5 */
#define CHIPID6     (*(unsigned char volatile far *)0x7efde6) /* 硬件ID6 */
#define CHIPID7     (*(unsigned char volatile far *)0x7efde7) /* 硬件ID7 */
#define CHIPID8     (*(unsigned char volatile far *)0x7efde8) /* 硬件ID8 */
#define CHIPID9     (*(unsigned char volatile far *)0x7efde9) /* 硬件ID9 */
#define CHIPID10    (*(unsigned char volatile far *)0x7efdea) /* 硬件ID10 */
#define CHIPID11    (*(unsigned char volatile far *)0x7efdeb) /* 硬件ID11 */
#define CHIPID12    (*(unsigned char volatile far *)0x7efdec) /* 硬件ID12 */
#define CHIPID13    (*(unsigned char volatile far *)0x7efded) /* 硬件ID13 */
#define CHIPID14    (*(unsigned char volatile far *)0x7efdee) /* 硬件ID14 */
#define CHIPID15    (*(unsigned char volatile far *)0x7efdef) /* 硬件ID15 */
#define CHIPID16    (*(unsigned char volatile far *)0x7efdf0) /* 硬件ID16 */
#define CHIPID17    (*(unsigned char volatile far *)0x7efdf1) /* 硬件ID17 */
#define CHIPID18    (*(unsigned char volatile far *)0x7efdf2) /* 硬件ID18 */
#define CHIPID19    (*(unsigned char volatile far *)0x7efdf3) /* 硬件ID19 */
#define CHIPID20    (*(unsigned char volatile far *)0x7efdf4) /* 硬件ID20 */
#define CHIPID21    (*(unsigned char volatile far *)0x7efdf5) /* 硬件ID21 */
#define CHIPID22    (*(unsigned char volatile far *)0x7efdf6) /* 硬件ID22 */
#define CHIPID23    (*(unsigned char volatile far *)0x7efdf7) /* 硬件ID23 */
#define CHIPID24    (*(unsigned char volatile far *)0x7efdf8) /* 硬件ID24 */
#define CHIPID25    (*(unsigned char volatile far *)0x7efdf9) /* 硬件ID25 */
#define CHIPID26    (*(unsigned char volatile far *)0x7efdfa) /* 硬件ID26 */
#define CHIPID27    (*(unsigned char volatile far *)0x7efdfb) /* 硬件ID27 */
#define CHIPID28    (*(unsigned char volatile far *)0x7efdfc) /* 硬件ID28 */
#define CHIPID29    (*(unsigned char volatile far *)0x7efdfd) /* 硬件ID29 */
#define CHIPID30    (*(unsigned char volatile far *)0x7efdfe) /* 硬件ID30 */
#define CHIPID31    (*(unsigned char volatile far *)0x7efdff) /* 硬件ID31 */

#define T22M_ADDR           CHIPID11                //22.1184MHz
#define T24M_ADDR           CHIPID12                //24MHz
#define T27M_ADDR           CHIPID13                //27MHz
#define T30M_ADDR           CHIPID14                //30MHz
#define T33M_ADDR           CHIPID15                //33.1776MHz
#define T35M_ADDR           CHIPID16                //35MHz
#define T36M_ADDR           CHIPID17                //36.864MHz
#define T40M_ADDR           CHIPID18                //40.96MHz
#define T44M_ADDR           CHIPID19                //44.2368MHz
#define T45M_ADDR           CHIPID20                //45.1584MHz
#define VRT6M_ADDR          CHIPID21                //VRTRIM_20M
#define VRT10M_ADDR         CHIPID22                //VRTRIM_24M
#define VRT27M_ADDR         CHIPID23                //VRTRIM_44M
#define VRT44M_ADDR         CHIPID24                //VRTRIM_64M

//========================== XFR: 0x7EFE00~0x7EFEFF =========================================================
#define CLKSEL      (*(unsigned char volatile far *)0x7efe00)	/* 主时钟源选择寄存器    */
#define	CLKDIV		(*(unsigned char volatile far *)0x7efe01)	/* 主时钟分频寄存器      */
#define	HIRCCR		(*(unsigned char volatile far *)0x7efe02)	/* 内部高速振荡器控制寄存器 */
#define	XOSCCR		(*(unsigned char volatile far *)0x7efe03)	/* 外部高速晶振振荡器控制寄存器    */
#define	IRC32KCR	(*(unsigned char volatile far *)0x7efe04)	/* 内部32K低速IRC时钟控制寄存器   */
#define	MCLKOCR		(*(unsigned char volatile far *)0x7efe05)	/* 主时钟输出控制寄存器  */
#define	IRCDB		(*(unsigned char volatile far *)0x7efe06)	/* 内部高速振荡器去抖控制寄存器   */
#define	IRC48MCR	(*(unsigned char volatile far *)0x7efe07)	/* 内部48M振荡器控制寄存器  */
#define	X32KCR		(*(unsigned char volatile far *)0x7efe08)	/* 外部32K晶振振荡器控制寄存器   */
#define	HSCLKDIV	(*(unsigned char volatile far *)0x7efe0b)	/* 高速时钟分频寄存器  */

#define	P0PU		(*(unsigned char volatile far *)0x7efe10)	/* P0口上拉电阻控制寄存器, 0: 禁止(默认), 1: 允许  */
#define	P1PU		(*(unsigned char volatile far *)0x7efe11)	/* P1口上拉电阻控制寄存器  */
#define	P2PU		(*(unsigned char volatile far *)0x7efe12)	/* P2口上拉电阻控制寄存器  */
#define	P3PU		(*(unsigned char volatile far *)0x7efe13)	/* P3口上拉电阻控制寄存器  */
#define	P4PU		(*(unsigned char volatile far *)0x7efe14)	/* P4口上拉电阻控制寄存器  */
#define	P5PU		(*(unsigned char volatile far *)0x7efe15)	/* P5口上拉电阻控制寄存器  */
#define	P6PU		(*(unsigned char volatile far *)0x7efe16)	/* P6口上拉电阻控制寄存器  */
#define	P7PU		(*(unsigned char volatile far *)0x7efe17)	/* P7口上拉电阻控制寄存器  */

#define	P0NCS		(*(unsigned char volatile far *)0x7efe18)	/* P0 Non Schmit Trigger  0: 使能端口施密特触发功能(默认), 1: 禁止  */
#define	P1NCS		(*(unsigned char volatile far *)0x7efe19)	/* P1 Non Schmit Trigger  */
#define	P2NCS		(*(unsigned char volatile far *)0x7efe1a)	/* P2 Non Schmit Trigger  */
#define	P3NCS		(*(unsigned char volatile far *)0x7efe1b)	/* P3 Non Schmit Trigger  */
#define	P4NCS		(*(unsigned char volatile far *)0x7efe1c)	/* P4 Non Schmit Trigger  */
#define	P5NCS		(*(unsigned char volatile far *)0x7efe1d)	/* P5 Non Schmit Trigger  */
#define	P6NCS		(*(unsigned char volatile far *)0x7efe1e)	/* P6 Non Schmit Trigger  */
#define	P7NCS		(*(unsigned char volatile far *)0x7efe1f)	/* P7 Non Schmit Trigger  */

#define	P0SR		(*(unsigned char volatile far *)0x7efe20)	/* P0口电平转换速率寄存器 默认1: 普通模式, 0:高速模式 */
#define	P1SR		(*(unsigned char volatile far *)0x7efe21)	/* P1口电平转换速率寄存器  */
#define	P2SR		(*(unsigned char volatile far *)0x7efe22)	/* P2口电平转换速率寄存器  */
#define	P3SR		(*(unsigned char volatile far *)0x7efe23)	/* P3口电平转换速率寄存器  */
#define	P4SR		(*(unsigned char volatile far *)0x7efe24)	/* P4口电平转换速率寄存器  */
#define	P5SR		(*(unsigned char volatile far *)0x7efe25)	/* P5口电平转换速率寄存器  */
#define	P6SR		(*(unsigned char volatile far *)0x7efe26)	/* P6口电平转换速率寄存器  */
#define	P7SR		(*(unsigned char volatile far *)0x7efe27)	/* P7口电平转换速率寄存器  */

#define	P0DR		(*(unsigned char volatile far *)0x7efe28)	/* P0口驱动电流控制寄存器 默认1: 普通模式, 0:大电流模式*/
#define	P1DR		(*(unsigned char volatile far *)0x7efe29)	/* P1口驱动电流控制寄存器 */
#define	P2DR		(*(unsigned char volatile far *)0x7efe2a)	/* P2口驱动电流控制寄存器 */
#define	P3DR		(*(unsigned char volatile far *)0x7efe2b)	/* P3口驱动电流控制寄存器 */
#define	P4DR		(*(unsigned char volatile far *)0x7efe2c)	/* P4口驱动电流控制寄存器 */
#define	P5DR		(*(unsigned char volatile far *)0x7efe2d)	/* P5口驱动电流控制寄存器 */
#define	P6DR		(*(unsigned char volatile far *)0x7efe2e)	/* P6口驱动电流控制寄存器 */
#define	P7DR		(*(unsigned char volatile far *)0x7efe2f)	/* P7口驱动电流控制寄存器 */

#define	P0IE		(*(unsigned char volatile far *)0x7efe30)	/* P0口输入使能控制寄存器, 1允许(默认), 0禁止 */
#define	P1IE		(*(unsigned char volatile far *)0x7efe31)	/* P1口输入使能控制寄存器 */
#define	P2IE		(*(unsigned char volatile far *)0x7efe32)	/* P2口输入使能控制寄存器 */
#define	P3IE		(*(unsigned char volatile far *)0x7efe33)	/* P3口输入使能控制寄存器 */
#define	P4IE		(*(unsigned char volatile far *)0x7efe34)	/* P4口输入使能控制寄存器 */
#define	P5IE		(*(unsigned char volatile far *)0x7efe35)	/* P5口输入使能控制寄存器 */
#define	P6IE		(*(unsigned char volatile far *)0x7efe36)	/* P6口输入使能控制寄存器 */
#define	P7IE		(*(unsigned char volatile far *)0x7efe37)	/* P7口输入使能控制寄存器 */

#define	P0PD		(*(unsigned char volatile far *)0x7efe40)	/* P0口下拉电阻控制寄存器, 0: 禁止(默认), 1: 允许  */
#define	P1PD		(*(unsigned char volatile far *)0x7efe41)	/* P1口下拉电阻控制寄存器  */
#define	P2PD		(*(unsigned char volatile far *)0x7efe42)	/* P2口下拉电阻控制寄存器  */
#define	P3PD		(*(unsigned char volatile far *)0x7efe43)	/* P3口下拉电阻控制寄存器  */
#define	P4PD		(*(unsigned char volatile far *)0x7efe44)	/* P4口下拉电阻控制寄存器  */
#define	P5PD		(*(unsigned char volatile far *)0x7efe45)	/* P5口下拉电阻控制寄存器  */
#define	P6PD		(*(unsigned char volatile far *)0x7efe46)	/* P6口下拉电阻控制寄存器  */
#define	P7PD		(*(unsigned char volatile far *)0x7efe47)	/* P7口下拉电阻控制寄存器  */
#define	P0BP        (*(unsigned char volatile far *)0x7efe48)	/* P0端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P1BP        (*(unsigned char volatile far *)0x7efe49)	/* P1端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P2BP        (*(unsigned char volatile far *)0x7efe4a)	/* P2端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P3BP        (*(unsigned char volatile far *)0x7efe4b)	/* P3端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P4BP        (*(unsigned char volatile far *)0x7efe4c)	/* P4端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P5BP        (*(unsigned char volatile far *)0x7efe4d)	/* P5端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P6BP        (*(unsigned char volatile far *)0x7efe4e)	/* P6端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */
#define	P7BP        (*(unsigned char volatile far *)0x7efe4f)	/* P7端口外设自动控制控制寄存器，默认1不控制，0：外设自动控制  */

#define	LCMIFCFG	(*(unsigned char volatile far *)0x7efe50)	/* LCM接口配置寄存器 */
#define	LCMIFCFG2	(*(unsigned char volatile far *)0x7efe51)	/* LCM接口配置寄存器2 */
#define	LCMIFCR		(*(unsigned char volatile far *)0x7efe52)	/* LCM接口控制寄存器 */
#define	LCMIFSTA	(*(unsigned char volatile far *)0x7efe53)	/* LCM接口状态寄存器 */
#define	LCMIFDATL	(*(unsigned char volatile far *)0x7efe54)	/* LCM接口低字节数据 */
#define	LCMIFDATH	(*(unsigned char volatile far *)0x7efe55)	/* LCM接口高字节数据 */
#define	LCMIFPSCR	(*(unsigned char volatile far *)0x7efe56)	/* LCM接口时钟预分频寄存器 */

#define	RTCCR		(*(unsigned char volatile far *)0x7efe60)	/* RTC控制寄存器 */
#define	RTCCFG		(*(unsigned char volatile far *)0x7efe61)	/* RTC配置寄存器 */
#define	RTCIEN		(*(unsigned char volatile far *)0x7efe62)	/* RTC中断使能寄存器 */
#define	RTCIF		(*(unsigned char volatile far *)0x7efe63)	/* RTC中断请求寄存器 */
#define	ALAHOUR		(*(unsigned char volatile far *)0x7efe64)	/* RTC闹钟的小时值 */
#define	ALAMIN		(*(unsigned char volatile far *)0x7efe65)	/* RTC闹钟的分钟值 */
#define	ALASEC		(*(unsigned char volatile far *)0x7efe66)	/* RTC闹钟的秒值 */
#define	ALASSEC		(*(unsigned char volatile far *)0x7efe67)	/* RTC闹钟的1/128秒值 */
#define	INIYEAR		(*(unsigned char volatile far *)0x7efe68)	/* RTC年初始化 */
#define	INIMONTH	(*(unsigned char volatile far *)0x7efe69)	/* RTC月初始化 */
#define	INIDAY		(*(unsigned char volatile far *)0x7efe6a)	/* RTC日初始化 */
#define	INIHOUR		(*(unsigned char volatile far *)0x7efe6b)	/* RTC时初始化 */
#define	INIMIN		(*(unsigned char volatile far *)0x7efe6c)	/* RTC分初始化 */
#define	INISEC		(*(unsigned char volatile far *)0x7efe6d)	/* RTC秒初始化 */
#define	INISSEC		(*(unsigned char volatile far *)0x7efe6e)	/* RTC1/128秒初始化 */
#define	INIWEEK		(*(unsigned char volatile far *)0x7efe6f)	/* RTC星期初始化 */
#define	WEEK		(*(unsigned char volatile far *)0x7efe6f)	/* RTC星期计数 */
#define	YEAR		(*(unsigned char volatile far *)0x7efe70)	/* RTC的年计数值 */
#define	MONTH		(*(unsigned char volatile far *)0x7efe71)	/* RTC的月计数值 */
#define	DAY			(*(unsigned char volatile far *)0x7efe72)	/* RTC的日计数值 */
#define	HOUR		(*(unsigned char volatile far *)0x7efe73)	/* RTC的时计数值 */
#define	MIN			(*(unsigned char volatile far *)0x7efe74)	/* RTC的分计数值 */
#define	SEC			(*(unsigned char volatile far *)0x7efe75)	/* RTC的秒计数值 */
#define	SSEC		(*(unsigned char volatile far *)0x7efe76)	/* RTC的1/128秒计数值 */
#define	RTC_WEEK	(*(unsigned char volatile far *)0x7efe6f)	/* RTC星期计数 */
#define	RTC_YEAR	(*(unsigned char volatile far *)0x7efe70)	/* RTC的年计数值 */
#define	RTC_MONTH	(*(unsigned char volatile far *)0x7efe71)	/* RTC的月计数值 */
#define	RTC_DAY		(*(unsigned char volatile far *)0x7efe72)	/* RTC的日计数值 */
#define	RTC_HOUR	(*(unsigned char volatile far *)0x7efe73)	/* RTC的时计数值 */
#define	RTC_MIN		(*(unsigned char volatile far *)0x7efe74)	/* RTC的分计数值 */
#define	RTC_SEC		(*(unsigned char volatile far *)0x7efe75)	/* RTC的秒计数值 */
#define	RTC_SSEC	(*(unsigned char volatile far *)0x7efe76)	/* RTC的1/128秒计数值 */

#define	T11CR	    (*(unsigned char volatile xdata *)0xfe78)	/* 定时器T11控制寄存器 */
#define	T11PS	    (*(unsigned char volatile xdata *)0xfe79)	/* 定时器T11时钟预分频寄存器 */
#define	T11H	    (*(unsigned char volatile xdata *)0xfe7a)	/* 定时器T11高字节 */
#define	T11L	    (*(unsigned char volatile xdata *)0xfe7b)	/* 定时器T11低字节 */
#define	TM11PS	    (*(unsigned char volatile xdata *)0xfe79)	/* 定时器T11时钟预分频寄存器 */
#define	TH11	    (*(unsigned char volatile xdata *)0xfe7a)	/* 定时器T11高字节 */
#define	TL11	    (*(unsigned char volatile xdata *)0xfe7b)	/* 定时器T11低字节 */

#define	I2CCFG		(*(unsigned char volatile far *)0x7efe80)	/* I2C配置寄存器  */
#define	I2CMSCR		(*(unsigned char volatile far *)0x7efe81)	/* I2C主机控制寄存器  */
#define	I2CMSST		(*(unsigned char volatile far *)0x7efe82)	/* I2C主机状态寄存器  */
#define	I2CSLCR		(*(unsigned char volatile far *)0x7efe83)	/* I2C从机控制寄存器  */
#define	I2CSLST		(*(unsigned char volatile far *)0x7efe84)	/* I2C从机状态寄存器  */
#define	I2CSLADR	(*(unsigned char volatile far *)0x7efe85)	/* I2C从机地址寄存器  */
#define	I2CTXD		(*(unsigned char volatile far *)0x7efe86)	/* I2C数据发送寄存器  */
#define	I2CRXD		(*(unsigned char volatile far *)0x7efe87)	/* I2C数据接收寄存器  */
#define	I2CMSAUX	(*(unsigned char volatile far *)0x7efe88)	/* I2C主机辅助控制寄存器  */
#define I2CPSCR     (*(unsigned char volatile far *)0x7efe89)	/* I2C主机时钟分频寄存器  */

#define SPI_CLKDIV   (*(unsigned char volatile far *)0x7efe90)	/* SPI时钟分频寄存器  */
#define PWMA_CLKDIV  (*(unsigned char volatile far *)0x7efe91)	/* PWMA时钟分频寄存器  */
#define PWMB_CLKDIV  (*(unsigned char volatile far *)0x7efe92)	/* PWMB时钟分频寄存器  */
#define TFPU_CLKDIV  (*(unsigned char volatile far *)0x7efe93)	/* TFPU时钟分频寄存器  */
#define I2S_CLKDIV   (*(unsigned char volatile far *)0x7efe94)	/* I2S时钟分频寄存器  */

#define	RSTFLAG		(*(unsigned char volatile far *)0x7efe99)	/* 复位标志寄存器  */
#define	RSTCR0		(*(unsigned char volatile far *)0x7efe9a)	/* 复位控制寄存器0  */
#define	RSTCR1		(*(unsigned char volatile far *)0x7efe9b)	/* 复位控制寄存器1  */
#define	RSTCR2		(*(unsigned char volatile far *)0x7efe9c)	/* 复位控制寄存器2  */
#define	RSTCR3		(*(unsigned char volatile far *)0x7efe9d)	/* 复位控制寄存器3  */
#define	RSTCR4		(*(unsigned char volatile far *)0x7efe9e)	/* 复位控制寄存器4  */
#define	RSTCR5		(*(unsigned char volatile far *)0x7efe9f)	/* 复位控制寄存器5  */

#define	TM0PS		(*(unsigned char volatile far *)0x7efea0)	/* 定时器0时钟预分频寄存器  */
#define	TM1PS		(*(unsigned char volatile far *)0x7efea1)	/* 定时器1时钟预分频寄存器  */
#define	TM2PS		(*(unsigned char volatile far *)0x7efea2)	/* 定时器2时钟预分频寄存器  */
#define	TM3PS		(*(unsigned char volatile far *)0x7efea3)	/* 定时器3时钟预分频寄存器  */
#define	TM4PS		(*(unsigned char volatile far *)0x7efea4)	/* 定时器4时钟预分频寄存器  */
#define	ADCTIM		(*(unsigned char volatile far *)0x7efea8)	/* ADC时序控制寄存器  */
#define	ADCEXCFG	(*(unsigned char volatile far *)0x7efead)	/* ADC扩展配置寄存器  */
#define	CMPEXCFG	(*(unsigned char volatile far *)0x7efeae)	/* 比较器扩展配置寄存器  */

#define	PWMA_ETRPS	(*(unsigned char volatile far *)0x7efeb0)	/* PWMA的ETR选择寄存器  */
#define	PWMA_ENO	(*(unsigned char volatile far *)0x7efeb1)	/* PWMA输出使能寄存器   */
#define	PWMA_PS		(*(unsigned char volatile far *)0x7efeb2)	/* PWMA输出脚选择寄存器 */
#define	PWMA_IOAUX	(*(unsigned char volatile far *)0x7efeb3)	/* PWMA辅助寄存器       */
#define	PWMB_ETRPS	(*(unsigned char volatile far *)0x7efeb4)	/* PWMB的ETR选择寄存器  */
#define	PWMB_ENO	(*(unsigned char volatile far *)0x7efeb5)	/* PWMB输出使能寄存器   */
#define	PWMB_PS		(*(unsigned char volatile far *)0x7efeb6)	/* PWMB输出脚选择寄存器 */
#define	PWMB_IOAUX	(*(unsigned char volatile far *)0x7efeb7)	/* PWMB辅助寄存器       */
#define PWMA_PS2    (*(unsigned char volatile far *)0x7efeb8)	/* PWMA输出脚选择寄存器2 */
#define PWMA_RCRH   (*(unsigned char volatile far *)0x7efeb9)	/* PWMA重复计数寄存器2 高字节 */
#define PWMB_RCRH   (*(unsigned char volatile far *)0x7efeba)	/* PWMB重复计数寄存器2 高字节 */

#define	PWMA_CR1	(*(unsigned char volatile far *)0x7efec0)	/* PWMA控制寄存器1  */
#define	PWMA_CR2	(*(unsigned char volatile far *)0x7efec1)	/* PWMA控制寄存器2  */
#define	PWMA_SMCR	(*(unsigned char volatile far *)0x7efec2)	/* PWMA从模式控制寄存器  */
#define	PWMA_ETR	(*(unsigned char volatile far *)0x7efec3)	/* PWMA外部触发寄存器  */
#define	PWMA_IER	(*(unsigned char volatile far *)0x7efec4)	/* PWMA中断使能寄存器  */
#define	PWMA_SR1	(*(unsigned char volatile far *)0x7efec5)	/* PWMA状态寄存器1  */
#define	PWMA_SR2	(*(unsigned char volatile far *)0x7efec6)	/* PWMA状态寄存器2  */
#define	PWMA_EGR	(*(unsigned char volatile far *)0x7efec7)	/* PWMA事件发生寄存器  */

#define	PWMA_CCMR1	(*(unsigned char volatile far *)0x7efec8)	/* PWMA捕获/比较模式寄存器1  */
#define	PWMA_CCMR2	(*(unsigned char volatile far *)0x7efec9)	/* PWMA捕获/比较模式寄存器2  */
#define	PWMA_CCMR3	(*(unsigned char volatile far *)0x7efeca)	/* PWMA捕获/比较模式寄存器3  */
#define	PWMA_CCMR4	(*(unsigned char volatile far *)0x7efecb)	/* PWMA捕获/比较模式寄存器4  */
#define	PWMA_CCER1	(*(unsigned char volatile far *)0x7efecc)	/* PWMA捕获比较使能寄存器1  */
#define	PWMA_CCER2	(*(unsigned char volatile far *)0x7efecd)	/* PWMA捕获比较使能寄存器2  */
#define	PWMA_CNTRH	(*(unsigned char volatile far *)0x7efece)	/* PWMA计数器高字节   */
#define	PWMA_CNTRL	(*(unsigned char volatile far *)0x7efecf)	/* PWMA计数器低字节   */

#define PWMA_PSCRH  (*(unsigned char volatile far *)0x7efed0)	/* PWMA预分频器高字节 */
#define PWMA_PSCRL  (*(unsigned char volatile far *)0x7efed1)	/* PWMA预分频器低字节 */
#define PWMA_ARRH   (*(unsigned char volatile far *)0x7efed2)	/* PWMA自动重装寄存器高字节 */
#define PWMA_ARRL   (*(unsigned char volatile far *)0x7efed3)	/* PWMA自动重装寄存器低字节 */
#define PWMA_RCR    (*(unsigned char volatile far *)0x7efed4)	/* PWMA重复计数寄存器 */
#define PWMA_CCR1H  (*(unsigned char volatile far *)0x7efed5)	/* PWMA比较捕获寄存器1高字节 */
#define PWMA_CCR1L  (*(unsigned char volatile far *)0x7efed6)	/* PWMA比较捕获寄存器1低字节 */
#define PWMA_CCR2H  (*(unsigned char volatile far *)0x7efed7)	/* PWMA比较捕获寄存器2高字节 */
#define PWMA_CCR2L  (*(unsigned char volatile far *)0x7efed8)	/* PWMA比较捕获寄存器2低字节 */
#define PWMA_CCR3H  (*(unsigned char volatile far *)0x7efed9)	/* PWMA比较捕获寄存器3高字节 */
#define PWMA_CCR3L  (*(unsigned char volatile far *)0x7efeda)	/* PWMA比较捕获寄存器3低字节 */
#define PWMA_CCR4H  (*(unsigned char volatile far *)0x7efedb)	/* PWMA比较捕获寄存器4高字节 */
#define PWMA_CCR4L  (*(unsigned char volatile far *)0x7efedc)	/* PWMA比较捕获寄存器4低字节 */
#define PWMA_BKR    (*(unsigned char volatile far *)0x7efedd)	/* PWMA刹车寄存器 */
#define PWMA_DTR    (*(unsigned char volatile far *)0x7efede)	/* PWMA死区控制寄存器 */
#define PWMA_OISR   (*(unsigned char volatile far *)0x7efedf)	/* PWMA输出空闲状态寄存器 */

#define	PWMB_CR1	(*(unsigned char volatile far *)0x7efee0)	/* PWMB控制寄存器1  */
#define	PWMB_CR2	(*(unsigned char volatile far *)0x7efee1)	/* PWMB控制寄存器2  */
#define	PWMB_SMCR	(*(unsigned char volatile far *)0x7efee2)	/* PWMB从模式控制寄存器  */
#define	PWMB_ETR	(*(unsigned char volatile far *)0x7efee3)	/* PWMB外部触发寄存器  */
#define	PWMB_IER	(*(unsigned char volatile far *)0x7efee4)	/* PWMB中断使能寄存器  */
#define	PWMB_SR1	(*(unsigned char volatile far *)0x7efee5)	/* PWMB状态寄存器1  */
#define	PWMB_SR2	(*(unsigned char volatile far *)0x7efee6)	/* PWMB状态寄存器2  */
#define	PWMB_EGR	(*(unsigned char volatile far *)0x7efee7)	/* PWMB事件发生寄存器  */

#define	PWMB_CCMR1	(*(unsigned char volatile far *)0x7efee8)	/* PWMB捕获/比较模式寄存器1  */
#define	PWMB_CCMR2	(*(unsigned char volatile far *)0x7efee9)	/* PWMB捕获/比较模式寄存器2  */
#define	PWMB_CCMR3	(*(unsigned char volatile far *)0x7efeea)	/* PWMB捕获/比较模式寄存器3  */
#define	PWMB_CCMR4	(*(unsigned char volatile far *)0x7efeeb)	/* PWMB捕获/比较模式寄存器4  */
#define	PWMB_CCER1	(*(unsigned char volatile far *)0x7efeec)	/* PWMB捕获比较使能寄存器1  */
#define	PWMB_CCER2	(*(unsigned char volatile far *)0x7efeed)	/* PWMB捕获比较使能寄存器2  */
#define	PWMB_CNTRH	(*(unsigned char volatile far *)0x7efeee)	/* PWMB计数器高字节   */
#define	PWMB_CNTRL	(*(unsigned char volatile far *)0x7efeef)	/* PWMB计数器低字节   */

#define PWMB_PSCRH  (*(unsigned char volatile far *)0x7efef0)	/* PWMB预分频器高字节 */
#define PWMB_PSCRL  (*(unsigned char volatile far *)0x7efef1)	/* PWMB预分频器低字节 */
#define PWMB_ARRH   (*(unsigned char volatile far *)0x7efef2)	/* PWMB自动重装寄存器高字节 */
#define PWMB_ARRL   (*(unsigned char volatile far *)0x7efef3)	/* PWMB自动重装寄存器低字节 */
#define PWMB_RCR    (*(unsigned char volatile far *)0x7efef4)	/* PWMB重复计数寄存器 */
#define PWMB_CCR1H  (*(unsigned char volatile far *)0x7efef5)	/* PWMB比较捕获寄存器1高字节 */
#define PWMB_CCR1L  (*(unsigned char volatile far *)0x7efef6)	/* PWMB比较捕获寄存器1低字节 */
#define PWMB_CCR2H  (*(unsigned char volatile far *)0x7efef7)	/* PWMB比较捕获寄存器2高字节 */
#define PWMB_CCR2L  (*(unsigned char volatile far *)0x7efef8)	/* PWMB比较捕获寄存器2低字节 */
#define PWMB_CCR3H  (*(unsigned char volatile far *)0x7efef9)	/* PWMB比较捕获寄存器3高字节 */
#define PWMB_CCR3L  (*(unsigned char volatile far *)0x7efefa)	/* PWMB比较捕获寄存器3低字节 */
#define PWMB_CCR4H  (*(unsigned char volatile far *)0x7efefb)	/* PWMB比较捕获寄存器4高字节 */
#define PWMB_CCR4L  (*(unsigned char volatile far *)0x7efefc)	/* PWMB比较捕获寄存器4低字节 */
#define PWMB_BKR    (*(unsigned char volatile far *)0x7efefd)	/* PWMB刹车寄存器 */
#define PWMB_DTR    (*(unsigned char volatile far *)0x7efefe)	/* PWMB死区控制寄存器 */
#define PWMB_OISR   (*(unsigned char volatile far *)0x7efeff)	/* PWMB输出空闲状态寄存器 */

#define PWMB_CCR5H  (*(unsigned char volatile far *)0x7efef5)	/* PWMB比较捕获寄存器1高字节 */
#define PWMB_CCR5L  (*(unsigned char volatile far *)0x7efef6)	/* PWMB比较捕获寄存器1低字节 */
#define PWMB_CCR6H  (*(unsigned char volatile far *)0x7efef7)	/* PWMB比较捕获寄存器2高字节 */
#define PWMB_CCR6L  (*(unsigned char volatile far *)0x7efef8)	/* PWMB比较捕获寄存器2低字节 */
#define PWMB_CCR7H  (*(unsigned char volatile far *)0x7efef9)	/* PWMB比较捕获寄存器3高字节 */
#define PWMB_CCR7L  (*(unsigned char volatile far *)0x7efefa)	/* PWMB比较捕获寄存器3低字节 */
#define PWMB_CCR8H  (*(unsigned char volatile far *)0x7efefb)	/* PWMB比较捕获寄存器4高字节 */
#define PWMB_CCR8L  (*(unsigned char volatile far *)0x7efefc)	/* PWMB比较捕获寄存器4低字节 */

//=============================================================================================================
#define	PullUpEnable(port, pu)		port |=  (pu)		/* 允许端口内部上拉电阻       PxPU, 要设置的端口对应位为1 */
#define	PullUpDisable(port, pu)		port &= ~(pu)		/* 禁止端口内部上拉电阻(默认) PxPU, 要设置的端口对应位为1 */
#define	PullDownEnable(port, pu)	port |=  (pu)		/* 允许端口内部下拉电阻       PxPD, 要设置的端口对应位为1 */
#define	PullDownDisable(port, pu)	port &= ~(pu)		/* 禁止端口内部下拉电阻(默认) PxPD, 要设置的端口对应位为1 */
#define	SchmitEnable(port, smt)		port &= ~(smt)		/* 允许端口施密特触发功能(默认) PxNCS, 要设置的端口对应位为1 */
#define	SchmitDisable(port, smt)	port |=  (smt)		/* 允许端口施密特触发功能       PxNCS, 要设置的端口对应位为1 */
#define	SlewRateNormal(port, sr)	port |=  (sr)		/* 端口输出设置为普通速度(默认) PxSR, 要设置的端口对应位为1 */
#define	SlewRateHigh(port, sr)		port &= ~(sr)		/* 端口输出设置为高速模式       PxSR, 要设置的端口对应位为1 */
#define	CurrentNormal(port, cu)		port |=  (cu)		/* 端口输出电流为普通模式(默认) PxDR, 要设置的端口对应位为1 */
#define	CurrentHigt(port, cu)		port &= ~(cu)		/* 端口输出电流为大电流模式     PxDR, 要设置的端口对应位为1 */
#define	InputEnable(port, ie)		port |=  (ie)		/* 允许端口数字输入(默认) PxIE, 要设置的端口对应位为1 */
#define	InputDisable(port, ie)		port &= ~(ie)		/* 禁止端口数字输入       PxIE, 要设置的端口对应位为1 */
#define	BypassEnable(port, bp)		port &= ~(bp)		/* 允许外设自动控制端口       PxBP, 要设置的端口对应位为1 */
#define	BypassDisable(port, bp)		port |=  (bp)		/* 禁止外设自动控制端口(默认) PxBP, 要设置的端口对应位为1 */
//=============================================================================================================



#define	I2C_USE_P24P23()	P_SW2 &= ~0x30						/* 将I2C切换到P2.4(SCL) P2.3(SDA).*/
#define	I2C_USE_P15P14()	P_SW2  = (P_SW2 & ~0x30) | 0x10		/* 将I2C切换到P1.5(SCL) P1.4(SDA)(上电默认).*/
#define	I2C_USE_P32P33()	P_SW2 |= 0x30						/* 将I2C切换到P3.2(SCL) P3.3(SDA).*/

#define	MainFosc_IRC()		CLKSEL = (CLKSEL & ~0x03)			/* 选择内部噶精度IRC时钟 */
#define	MainFosc_XTAL()		CLKSEL = (CLKSEL & ~0x03) | 0x01	/* 选择外部高速晶振或时钟 */
#define	MainFosc_XTAL32K()	CLKSEL = (CLKSEL & ~0x03) | 0x02	/* 选择外部32K晶振 */
#define	MainFosc_IRC32K()	CLKSEL =  CLKSEL | 0x03				/* 选择内部32K时钟 */
#define	EXT_CLOCK()			XOSCCR = 0x80					/* 选择外部时钟 */
#define	EXT_CRYSTAL()		XOSCCR = 0xC0					/* 选择外部晶振 */
#define	MCLKO_None()		MCLKOCR = 0						/* 主时钟不输出 	    */
#define	MCLKO47_DIV(n)		MCLKOCR = n						/* 主时钟从P4.7分频输出 */
#define	MCLKO56_DIV(n)		MCLKOCR = n | 0x80				/* 主时钟从P5.6分频输出 */


/*  P0:  0x80 */
sbit  P00 = P0^0;
sbit  P01 = P0^1;
sbit  P02 = P0^2;
sbit  P03 = P0^3;
sbit  P04 = P0^4;
sbit  P05 = P0^5;
sbit  P06 = P0^6;
sbit  P07 = P0^7;

/*   PCON:   0x87  */
sbit    SMOD  =   PCON^7;
sbit    SMOD0 =   PCON^6;
sbit    LVDF  =   PCON^5;
sbit    POF   =   PCON^4;
sbit    GF1   =   PCON^3;
sbit    GF0   =   PCON^2;
sbit    PD    =   PCON^1;
sbit    IDL   =   PCON^0;

/*  TCON:  0x88 */
sbit TF1  = TCON^7;	//定时器1溢出中断标志位
sbit TR1  = TCON^6;	//定时器1运行控制位
sbit TF0  = TCON^5;	//定时器0溢出中断标志位
sbit TR0  = TCON^4;	//定时器0运行控制位
sbit IE1  = TCON^3;	//外中断1标志位
sbit IT1  = TCON^2;	//外中断1信号方式控制位，1：下降沿中断，0：上升下降均中断。
sbit IE0  = TCON^1;	//外中断0标志位
sbit IT0  = TCON^0;	//外中断0信号方式控制位，1：下降沿中断，0：上升下降均中断。

/*   TMOD:     0x89 */
sbit    T1_GATE =  TMOD^7;
sbit    T1_CT   =  TMOD^6;
sbit    T1_M1   =  TMOD^5;
sbit    T1_M0   =  TMOD^4;
sbit    T0_GATE =  TMOD^3;
sbit    T0_CT   =  TMOD^2;
sbit    T0_M1   =  TMOD^1;
sbit    T0_M0   =  TMOD^0;

/*    AUXR:     0x8e  */
sbit    T0x12   =  AUXR^7;
sbit    T1x12   =  AUXR^6;
sbit    S1M0x6  =  AUXR^5;
sbit    T2R     =  AUXR^4;
sbit    T2_CT   =  AUXR^3;
sbit    T2x12   =  AUXR^2;
sbit    EXTRAM  =  AUXR^1;
sbit    S1BRT   =  AUXR^0;

/*   INTCLKO:  0x8f  */
sbit    EX4     =  INTCLKO^6;
sbit    EX3     =  INTCLKO^5;
sbit    EX2     =  INTCLKO^4;
sbit    T2CLKO  =  INTCLKO^2;
sbit    T1CLKO  =  INTCLKO^1;
sbit    T0CLKO  =  INTCLKO^0;

//                                           7    6    5    4    3     2       1       0      Reset Value
//INTCLKO:  0x8f  中断与时钟输出控制寄存器  -   EX4  EX3  EX2   -   T2CLKO  T1CLKO  T0CLKO    0000,0000
#define	INT4_Enable()	EX4 = 1
#define	INT3_Enable()	EX3 = 1
#define	INT2_Enable()	EX2 = 1
#define	INT1_Enable()	EX1 = 1
#define	INT0_Enable()	EX0 = 1

#define	INT4_Disable()	EX4 = 0
#define	INT3_Disable()	EX3 = 0
#define	INT2_Disable()	EX2 = 0
#define	INT1_Disable()	EX1 = 0
#define	INT0_Disable()	EX0 = 0

/*  P1:  0x90 */
sbit  P10 = P1^0;
sbit  P11 = P1^1;
sbit  P12 = P1^2;
sbit  P13 = P1^3;
sbit  P14 = P1^4;
sbit  P15 = P1^5;
sbit  P16 = P1^6;
sbit  P17 = P1^7;

/*   AUXR2:   0x97  */
sbit    RAMTINY     =           AUXR2^7;
sbit    CPUMODE     =           AUXR2^6;
sbit    RAMEXE      =           AUXR2^5;
sbit    CANFD       =           AUXR2^4;
sbit    CANSEL      =           AUXR2^3;
sbit    CAN2EN      =           AUXR2^2;
sbit    CANEN       =           AUXR2^1;
sbit    LINEN       =           AUXR2^0;

/*  SCON:    0x98 */
sbit SM0  = SCON^7;	//SM0/FE		SM0 SM1 = 00 ~ 11: 方式0~3
sbit SM1  = SCON^6;	//
sbit SM2  = SCON^5;	//多机通讯
sbit REN  = SCON^4;	//接收允许
sbit TB8  = SCON^3;	//发送数据第8位
sbit RB8  = SCON^2;	//接收数据第8位
sbit TI   = SCON^1;	//发送中断标志位
sbit RI   = SCON^0;	//接收中断标志位

/*    S2CON:     0x9a  */
sbit    S2SM0 =  S2CON^7;
sbit    S2SM1 =  S2CON^6;
sbit    S2SM2 =  S2CON^5;
sbit    S2REN =  S2CON^4;
sbit    S2TB8 =  S2CON^3;
sbit    S2RB8 =  S2CON^2;
sbit    S2TI  =  S2CON^1;
sbit    S2RI  =  S2CON^0;

/*   IRCBAND:   0x9d  */
sbit    USBCKS   =  IRCBAND^7;
sbit    USBCKS2  =  IRCBAND^6;
sbit    HIRCSEL1 =  IRCBAND^1;
sbit    HIRCSEL0 =  IRCBAND^0;

/*  P2:  0xa0  */
sbit  P20 = P2^0;
sbit  P21 = P2^1;
sbit  P22 = P2^2;
sbit  P23 = P2^3;
sbit  P24 = P2^4;
sbit  P25 = P2^5;
sbit  P26 = P2^6;
sbit  P27 = P2^7;

/*    P_SW1:     0xa2 */
sbit    S1_S1  =  P_SW1^7;
sbit    S1_S0  =  P_SW1^6;
sbit    CAN_S1 =  P_SW1^5;
sbit    CAN_S0 =  P_SW1^4;
sbit    SPI_S1 =  P_SW1^3;
sbit    SPI_S0 =  P_SW1^2;
sbit    LIN_S1 =  P_SW1^1;
sbit    LIN_S0 =  P_SW1^0;

/*  IE:  0xa8  */
sbit EA   = IE^7;	//中断允许总控制位
sbit ELVD = IE^6;	//低压监测中断允许位
sbit EADC = IE^5;	//ADC 中断 允许位
sbit ES   = IE^4;	//串行中断 允许控制位
sbit ET1  = IE^3;	//定时中断1允许控制位
sbit EX1  = IE^2;	//外部中断1允许控制位
sbit ET0  = IE^1;	//定时中断0允许控制位
sbit EX0  = IE^0;	//外部中断0允许控制位

/*    S3CON:   0xac  */
sbit    S3SM0       =           S3CON^7;
sbit    S3ST3       =           S3CON^6;
sbit    S3SM2       =           S3CON^5;
sbit    S3REN       =           S3CON^4;
sbit    S3TB8       =           S3CON^3;
sbit    S3RB8       =           S3CON^2;
sbit    S3TI        =           S3CON^1;
sbit    S3RI        =           S3CON^0;

//							7     6     5    4    3    2    1     0    Reset Value
//    IE2:    0xaF  		-     -     -    -    -    -   ESPI  ES2   0000,0000B	//Auxiliary Interrupt
sbit    EUSB        =           IE2^7;
sbit    ET4         =           IE2^6;
sbit    ET3         =           IE2^5;
sbit    ES4         =           IE2^4;
sbit    ES3         =           IE2^3;
sbit    ET2         =           IE2^2;
sbit    ESPI        =           IE2^1;
sbit    ES2         =           IE2^0;

#define		SPI_INT_ENABLE()		ESPI = 1	/* 允许SPI中断		*/
#define		SPI_INT_DISABLE()		ESPI = 0	/* 允许SPI中断		*/
#define		UART2_INT_ENABLE()		ES2  = 1	/* 允许串口2中断	*/
#define		UART2_INT_DISABLE()		ES2  = 0	/* 允许串口2中断	*/

/*  P3:   0xb0  */
sbit  P30 = P3^0;
sbit  P31 = P3^1;
sbit  P32 = P3^2;
sbit  P33 = P3^3;
sbit  P34 = P3^4;
sbit  P35 = P3^5;
sbit  P36 = P3^6;
sbit  P37 = P3^7;

sbit RXD  = P3^0;
sbit TXD  = P3^1;
sbit INT0 = P3^2;
sbit INT1 = P3^3;
sbit T0   = P3^4;
sbit T1   = P3^5;
sbit WR   = P3^6;
sbit RD   = P3^7;

sbit INT2 = P3^6;
sbit INT3 = P3^7;
sbit INT4 = P3^0;

sbit CLKOUT0   = P3^5;
sbit CLKOUT1   = P3^4;

/*    IP2:     0xb5  */
sbit    PUSB  =  IP2^7;
sbit    PI2C  =  IP2^6;
sbit    PCMP  =  IP2^5;
sbit    PX4   =  IP2^4;
sbit    PPWMB =  IP2^3;
sbit    PPWMA =  IP2^2;
sbit    PSPI  =  IP2^1;
sbit    PS2   =  IP2^0;

/*   IP2H:       0xb6  */
sbit    PUSBH  =  IP2H^7;
sbit    PI2CH  =  IP2H^6;
sbit    PCMPH  =  IP2H^5;
sbit    PX4H   =  IP2H^4;
sbit    PPWMBH =  IP2H^3;
sbit    PPWMAH =  IP2H^2;
sbit    PSPIH  =  IP2H^1;
sbit    PS2H   =  IP2H^0;

/*  IPH:      0xb7 */
//                                           7      6      5     4     3     2    1     0        Reset Value
//  IPH: 0xB7       //中断优先级高位       PPCAH  PLVDH  PADCH  PSH  PT1H  PX1H  PT0H  PX0H   0000,0000
sbit    PPCAH =  IPH^7;
sbit    PLVDH =  IPH^6;
sbit    PADCH =  IPH^5;
sbit    PSH   =  IPH^4;
sbit    PT1H  =  IPH^3;
sbit    PX1H  =  IPH^2;
sbit    PT0H  =  IPH^1;
sbit    PX0H  =  IPH^0;

//                                          7     6     5    4    3    2    1    0    Reset Value
//sfr IP      = 0xB8; //中断优先级低位      -   PLVD   PADC  PS  PT1  PX1  PT0  PX0   0000,0000
//--------
sbit PPCA   = IP^7;	//PCA 中断优先级
sbit PLVD	= IP^6;	//低压监测中断优先级
sbit PADC	= IP^5;	//ADC 中断优先级
sbit PS   	= IP^4;	//串行中断0优先级设定位
sbit PT1	= IP^3;	//定时中断1优先级设定位
sbit PX1	= IP^2;	//外部中断1优先级设定位
sbit PT0	= IP^1;	//定时中断0优先级设定位
sbit PX0	= IP^0;	//外部中断0优先级设定位

/*    P_SW2:      0xba  */
sbit    EAXFR       =  P_SW2^7;	//1: MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展SFR(XSFR),  0: MOVX A,@DPTR/MOVX @DPTR,A指令的操作对象为扩展RAM(XRAM).
sbit    I2C_S1      =  P_SW2^5;
sbit    I2C_S0      =  P_SW2^4;
sbit    CMPO_S      =  P_SW2^3;
sbit    S4_S        =  P_SW2^2;
sbit    S3_S        =  P_SW2^1;
sbit    S2_S        =  P_SW2^0;
#define	CMPO_P45()	P_SW2 &= ~0x08	//比较结果输出到P4.5.
#define	CMPO_P41()	P_SW2 |=  0x08	//比较结果输出到P4.1.

/*    P_SW3:        0xbb  */
sbit    I2S_S1      =  P_SW3^7;
sbit    I2S_S0      =  P_SW3^6;
sbit    S2SPI_S1    =  P_SW3^5;
sbit    S2SPI_S0    =  P_SW3^4;
sbit    S1SPI_S1    =  P_SW3^3;
sbit    S1SPI_S0    =  P_SW3^2;
sbit    CAN2_S1     =  P_SW3^1;
sbit    CAN2_S0     =  P_SW3^0;

/*     ADC_CONTR:      0xbc  */
sbit    ADC_POWER   =  ADC_CONTR^7;
sbit    ADC_START   =  ADC_CONTR^6;
sbit    ADC_FLAG    =  ADC_CONTR^5;
sbit    ADC_EPWMT   =  ADC_CONTR^4;

/*    P_SW4:      0xbf */
sbit    QSPI_S1 = P_SW4^1;
sbit    QSPI_S0 = P_SW4^0;

/*     P4:      0xc0  */
sbit  P40 = P4^0;
sbit  P41 = P4^1;
sbit  P42 = P4^2;
sbit  P43 = P4^3;
sbit  P44 = P4^4;
sbit  P45 = P4^5;
sbit  P46 = P4^6;
sbit  P47 = P4^7;

/*     WDT_CONTR:    0xc1  */
sbit    WDT_FLAG    =  WDT_CONTR^7;
sbit    EN_WDT      =  WDT_CONTR^5;
sbit    CLR_WDT     =  WDT_CONTR^4;
sbit    IDL_WDT     =  WDT_CONTR^3;

/*    IAP_CONTR:    0xc7  */
sbit    IAPEN       =  IAP_CONTR^7;
sbit    SWBS        =  IAP_CONTR^6;
sbit    SWRST       =  IAP_CONTR^5;
sbit    CMD_FAIL    =  IAP_CONTR^4;
sbit    SWBS2       =  IAP_CONTR^3;

/*  P5:    0xc8  */
sbit  P50 = P5^0;
sbit  P51 = P5^1;
sbit  P52 = P5^2;
sbit  P53 = P5^3;
sbit  P54 = P5^4;
sbit  P55 = P5^5;
sbit  P56 = P5^6;
sbit  P57 = P5^7;

//   SPSTAT:    0xcd  SPI状态寄存器
//   7       6      5   4   3   2   1   0    	Reset Value
//	SPIF	WCOL	-	-	-	-	-	-
sbit    SPIF  =  SPSTAT^7;	/* SPI传输完成标志。写入1清0。*/
sbit    WCOL  =  SPSTAT^6;  /* SPI写冲突标志。写入1清0。  */

// SPCTL:     0xCE	SPI控制寄存器
//   7       6       5       4       3       2       1       0    	Reset Value
//	SSIG	SPEN	DORD	MSTR	CPOL	CPHA	SPR1	SPR0		0x00
sbit    SSIG        =           SPCTL^7;
sbit    SPEN        =           SPCTL^6;
sbit    DORD        =           SPCTL^5;
sbit    MSTR        =           SPCTL^4;
sbit    CPOL        =           SPCTL^3;
sbit    CPHA        =           SPCTL^2;
sbit    SPR1        =           SPCTL^1;
sbit    SPR0        =           SPCTL^0;

#define	SPI_SSIG_None()		SPCTL |=  (1<<7)		/* 1: 忽略SS脚	*/
#define	SPI_SSIG_Enable()	SPCTL &= ~(1<<7)		/* 0: SS脚用于决定主从机	*/
#define	SPI_Enable()		SPCTL |=  (1<<6)		/* 1: 允许SPI	*/
#define	SPI_Disable()		SPCTL &= ~(1<<6)		/* 0: 禁止SPI	*/
#define	SPI_LSB_First()		SPCTL |=  (1<<5)		/* 1: LSB先发	*/
#define	SPI_MSB_First()		SPCTL &= ~(1<<5)		/* 0: MSB先发	*/
#define	SPI_Master()		SPCTL |=  (1<<4)		/* 1: 设为主机	*/
#define	SPI_Slave()			SPCTL &= ~(1<<4)		/* 0: 设为从机	*/
#define	SPI_SCLK_NormalH()	SPCTL |=  (1<<3)		/* 1: 空闲时SCLK为高电平	*/
#define	SPI_SCLK_NormalL()	SPCTL &= ~(1<<3)		/* 0: 空闲时SCLK为低电平	*/
#define	SPI_PhaseH()		SPCTL |=  (1<<2)		/* 1: 	*/
#define	SPI_PhaseL()		SPCTL &= ~(1<<2)		/* 0: 	*/
#define	SPI_Speed(n)		SPCTL = (SPCTL & ~3) | (n)	/*设置速度, 0 -- fosc/4, 1 -- fosc/8, 2 -- fosc/16, 3 -- fosc/32	*/

/*  PSW:   0xd0   */
sbit CY   = PSW^7;
sbit AC   = PSW^6;
sbit F0   = PSW^5;
sbit RS1  = PSW^4;
sbit RS0  = PSW^3;
sbit OV   = PSW^2;
sbit F1   = PSW^1;
sbit P    = PSW^0;

/*    T4T3M:         0xdd  */
sbit    T4R     = T4T3M^7;
sbit    T4_CT   = T4T3M^6;
sbit    T4x12   = T4T3M^5;
sbit    T4CLKO  = T4T3M^4;
sbit    T3R     = T4T3M^3;
sbit    T3_CT   = T4T3M^2;
sbit    T3x12   = T4T3M^1;
sbit    T3CLKO  = T4T3M^0;

/*   ADCCFG:      0xde */
//sbit    RESFMT  = ADCCFG^5;

/*   IP3:      0xdf */
sbit    PI2S  =  IP3^3;
sbit    PRTC  =  IP3^2;
sbit    PS4   =  IP3^1;
sbit    PS3   =  IP3^0;

/*    ACC:  0xe0  */
sbit ACC0 = ACC^0;
sbit ACC1 = ACC^1;
sbit ACC2 = ACC^2;
sbit ACC3 = ACC^3;
sbit ACC4 = ACC^4;
sbit ACC5 = ACC^5;
sbit ACC6 = ACC^6;
sbit ACC7 = ACC^7;

//                       7      6     5    4    3    2     1      0        Reset Value
// CMPCR1: 0xE6        CMPEN  CMPIF  PIE  NIE   -    -   CMPOE  CMPRES      0000xx00B
sbit    CMPEN   =  CMPCR1^7;	//1: 允许比较器, 0: 禁止,关闭比较器电源
sbit    CMPIF   =  CMPCR1^6;	//比较器中断标志, 包括上升沿或下降沿中断, 软件清0
sbit    PIE     =  CMPCR1^5;	//1: 比较结果由0变1, 产生上升沿中断
sbit    NIE     =  CMPCR1^4;	//1: 比较结果由1变0, 产生下降沿中断
sbit    CMPOE   =  CMPCR1^1;	//1: 允许比较结果输出, 0: 禁止.
sbit    CMPRES  =  CMPCR1^0;	//比较结果, 1: CMP+电平高于CMP-,  0: CMP+电平低于CMP-,  只读

//                       7        6       5  4  3  2  1  0    Reset Value
// CMPCR2: 0xE7       INVCMPO  DISFLT       LCDTY[5:0]       00000000B			LCDTY=0~63, 比较结果变化延时周期数
sbit    INVCMPO  =  CMPCR2^7;	//1: 比较器输出IO取反,  0: 不取反
sbit    DISFLT   =  CMPCR2^6;	//1: 关闭0.1uF滤波,   	0: 允许

/*  P6:   0xe8  */
sbit  P60 = P6^0;
sbit  P61 = P6^1;
sbit  P62 = P6^2;
sbit  P63 = P6^3;
sbit  P64 = P6^4;
sbit  P65 = P6^5;
sbit  P66 = P6^6;
sbit  P67 = P6^7;

/*    IP3H:        0xee  */
sbit    PI2SH  =  IP3H^3;
sbit    PRTCH  =  IP3H^2;
sbit    PS4H   =  IP3H^1;
sbit    PS3H   =  IP3H^0;

//                                     7    6       5       4      3    2     1     0      Reset Value
//  AUXINTIF: 0xef 辅助中断标志寄存器  -  INT4IF  INT3IF  INT2IF   -   T4IF  T3IF  T2IF    0000,0000
sbit    INT4IF  =  AUXINTIF^6;
sbit    INT3IF  =  AUXINTIF^5;
sbit    INT2IF  =  AUXINTIF^4;
sbit    T4IF    =  AUXINTIF^2;
sbit    T3IF    =  AUXINTIF^1;
sbit    T2IF    =  AUXINTIF^0;

#define	INT4_Clear()	INT4IF = 0		/* 清除外中断4标志位 */
#define	INT3_Clear()	INT3IF = 0		/* 清除外中断3标志位 */
#define	INT2_Clear()	INT2IF = 0		/* 清除外中断2标志位 */
#define	INT1_Clear()	IE1 = 0			/* 清除外中断1标志位 */
#define	INT0_Clear()	IE0 = 0			/* 清除外中断0标志位 */

#define	INT0_Fall()		IT0 = 1		/* INT0 下降沿中断           */
#define	INT0_RiseFall()	IT0 = 0		/* INT0 下降沿上升沿均中断   */
#define	INT1_Fall()		IT1 = 1		/* INT1 下降沿中断           */
#define	INT1_RiseFall()	IT0 = 0		/* INT1 下降沿上升沿均中断   */

/*   B:  0xf0  */
sbit B0 = B^0;
sbit B1 = B^1;
sbit B2 = B^2;
sbit B3 = B^3;
sbit B4 = B^4;
sbit B5 = B^5;
sbit B6 = B^6;
sbit B7 = B^7;

/*      USBCON:        0xf4 */
sbit    ENUSB   =  USBCON^7;
sbit    USBRST  =  USBCON^6;
sbit    PS2M    =  USBCON^5;
sbit    PUEN    =  USBCON^4;
sbit    PDEN    =  USBCON^3;
sbit    DFREC   =  USBCON^2;
sbit    DP      =  USBCON^1;
sbit    DM      =  USBCON^0;


/*  P7:   0xf8  */
sbit  P70 = P7^0;
sbit  P71 = P7^1;
sbit  P72 = P7^2;
sbit  P73 = P7^3;
sbit  P74 = P7^4;
sbit  P75 = P7^5;
sbit  P76 = P7^6;
sbit  P77 = P7^7;

/*    S4CON :          0xfd */
sbit    S4SM0  =  S4CON^7;
sbit    S4ST4  =  S4CON^6;
sbit    S4SM2  =  S4CON^5;
sbit    S4REN  =  S4CON^4;
sbit    S4TB8  =  S4CON^3;
sbit    S4RB8  =  S4CON^2;
sbit    S4TI   =  S4CON^1;
sbit    S4RI   =  S4CON^0;

/*    RSTCFG :          0xff */
sbit    ENLVR   =  RSTCFG^6;
sbit    P54RST  =  RSTCFG^4;


/*************************************************************************************************/

/////////////////////////////////////////////////
//USB Control Regiter
/////////////////////////////////////////////////

#define     USBBASE                 0
#define     FADDR                   (USBBASE + 0)
#define     UPDATE                  0x80
#define     POWER                   (USBBASE + 1)
#define     ISOUD                   0x80
#define     USBRST                  0x08
#define     USBRSU                  0x04
#define     USBSUS                  0x02
#define     ENSUS                   0x01
#define     INTRIN1                 (USBBASE + 2)
#define     EP5INIF                 0x20
#define     EP4INIF                 0x10
#define     EP3INIF                 0x08
#define     EP2INIF                 0x04
#define     EP1INIF                 0x02
#define     EP0IF                   0x01
#define     INTROUT1                (USBBASE + 4)
#define     EP5OUTIF                0x20
#define     EP4OUTIF                0x10
#define     EP3OUTIF                0x08
#define     EP2OUTIF                0x04
#define     EP1OUTIF                0x02
#define     INTRUSB                 (USBBASE + 6)
#define     SOFIF                   0x08
#define     RSTIF                   0x04
#define     RSUIF                   0x02
#define     SUSIF                   0x01
#define     INTRIN1E                (USBBASE + 7)
#define     EP5INIE                 0x20
#define     EP4INIE                 0x10
#define     EP3INIE                 0x08
#define     EP2INIE                 0x04
#define     EP1INIE                 0x02
#define     EP0IE                   0x01
#define     INTROUT1E               (USBBASE + 9)
#define     EP5OUTIE                0x20
#define     EP4OUTIE                0x10
#define     EP3OUTIE                0x08
#define     EP2OUTIE                0x04
#define     EP1OUTIE                0x02
#define     INTRUSBE                (USBBASE + 11)
#define     SOFIE                   0x08
#define     RSTIE                   0x04
#define     RSUIE                   0x02
#define     SUSIE                   0x01
#define     FRAME1                  (USBBASE + 12)
#define     FRAME2                  (USBBASE + 13)
#define     INDEX                   (USBBASE + 14)
#define     INMAXP                  (USBBASE + 16)
#define     CSR0                    (USBBASE + 17)
#define     SSUEND                  0x80
#define     SOPRDY                  0x40
#define     SDSTL                   0x20
#define     SUEND                   0x10
#define     DATEND                  0x08
#define     STSTL                   0x04
#define     IPRDY                   0x02
#define     OPRDY                   0x01
#define     INCSR1                  (USBBASE + 17)
#define     INCLRDT                 0x40
#define     INSTSTL                 0x20
#define     INSDSTL                 0x10
#define     INFLUSH                 0x08
#define     INUNDRUN                0x04
#define     INFIFONE                0x02
#define     INIPRDY                 0x01
#define     INCSR2                  (USBBASE + 18)
#define     INAUTOSET               0x80
#define     INISO                   0x40
#define     INMODEIN                0x20
#define     INMODEOUT               0x00
#define     INENDMA                 0x10
#define     INFCDT                  0x08
#define     OUTMAXP                 (USBBASE + 19)
#define     OUTCSR1                 (USBBASE + 20)
#define     OUTCLRDT                0x80
#define     OUTSTSTL                0x40
#define     OUTSDSTL                0x20
#define     OUTFLUSH                0x10
#define     OUTDATERR               0x08
#define     OUTOVRRUN               0x04
#define     OUTFIFOFUL              0x02
#define     OUTOPRDY                0x01
#define     OUTCSR2                 (USBBASE + 21)
#define     OUTAUTOCLR              0x80
#define     OUTISO                  0x40
#define     OUTENDMA                0x20
#define     OUTDMAMD                0x10
#define     COUNT0                  (USBBASE + 22)
#define     OUTCOUNT1               (USBBASE + 22)
#define     OUTCOUNT2               (USBBASE + 23)
#define     FIFO0                   (USBBASE + 32)
#define     FIFO1                   (USBBASE + 33)
#define     FIFO2                   (USBBASE + 34)
#define     FIFO3                   (USBBASE + 35)
#define     FIFO4                   (USBBASE + 36)
#define     FIFO5                   (USBBASE + 37)
#define     UTRKCTL                 (USBBASE + 48)
#define     UTRKSTS                 (USBBASE + 49)


/*************************************************************************************************/
//                     7     6     5    4    3    2   1   0       Reset Value
//sfr SCON  = 0x98;   SM0   SM1   SM2  REN  TB8  RB8  TI  RI      00000000B		 //S1 Control

#define		S1_DoubleRate()		PCON  |=  0x80
#define		S1_SHIFT()			SCON  &=  0x3f

#define		S1_8bit()			SCON   =  (SCON & 0x3f) | 0x40
#define		S1_9bit()			SCON   =  (SCON & 0x3f) | 0xc0
#define		S1_RX_Enable()		SCON  |=  0x10
#define		S1_RX_Disable()		SCON  &= ~0x10
#define		TI1					TI					/* 判断TI1是否发送完成								 */
#define		RI1					RI					/* 判断RI1是否接收完成								 */
#define		SET_TI1()			TI = 1				/* 设置TI1(引起中断)								 */
#define		CLR_TI1()			TI = 0				/* 清除TI1											 */
#define		CLR_RI1()			RI = 0				/* 清除RI1											 */
#define		S1TB8_SET()			TB8 = 1				/* 设置TB8											 */
#define		S1TB8_CLR()			TB8 = 0				/* 清除TB8											 */
#define		S1_Int_Enable()		ES     =  1			/* 串口1允许中断									 */
#define		S1_Int_Disable()	ES     =  0			/* 串口1禁止中断									 */
#define 	S1_BRT_UseTimer1()	AUXR  &= ~1
#define 	S1_BRT_UseTimer2()	AUXR  |=  1
#define		S1_USE_P30P31()		P_SW1 &= ~0xc0						//UART1 使用P30 P31口	默认
#define		S1_USE_P36P37()		P_SW1  =  (P_SW1 & ~0xc0) | 0x40	//UART1 使用P36 P37口
#define		S1_USE_P16P17()		P_SW1  =  (P_SW1 & ~0xc0) | 0x80	//UART1 使用P16 P17口
#define		S1_USE_P43P44()		P_SW1  =  (P_SW1 & ~0xc0) | 0xc0	//UART1 使用P4.3 P4.4口

//						  7      6      5      4      3      2     1     0        Reset Value
//sfr S2CON = 0x9A;		S2SM0    -    S2SM2  S2REN  S2TB8  S2RB8  S2TI  S2RI      00000000B		 //S2 Control

#define		S2_MODE0()			S2CON &= ~(1<<7)	/* 串口2模式0，8位UART，波特率 = 定时器2的溢出率 / 4 */
#define		S2_MODE1()			S2CON |=  (1<<7)	/* 串口2模式1，9位UART，波特率 = 定时器2的溢出率 / 4 */
#define		S2_8bit()			S2CON &= ~(1<<7)	/* 串口2模式0，8位UART，波特率 = 定时器2的溢出率 / 4 */
#define		S2_9bit()			S2CON |=  (1<<7)	/* 串口2模式1，9位UART，波特率 = 定时器2的溢出率 / 4 */
#define		S2_RX_Enable()		S2CON |=  (1<<4)	/* 允许串2接收										 */
#define		S2_RX_Disable()		S2CON &= ~(1<<4)	/* 禁止串2接收										 */
#define		TI2					(S2CON & 2) 		/* 判断TI2是否发送完成								 */
#define		RI2					(S2CON & 1) 		/* 判断RI2是否接收完成								 */
#define		SET_TI2()			S2CON |=  2         /* 设置TI2(引起中断)								 */
#define		CLR_TI2()			S2CON &= ~2         /* 清除TI2											 */
#define		CLR_RI2()			S2CON &= ~1			/* 清除RI2											 */
#define		S2TB8_SET()			S2CON |=  (1<<3)	/* 设置TB8											 */
#define		S2TB8_CLR()			S2CON &= ~(1<<3)	/* 清除TB8											 */
#define		S2_Int_Enable()		IE2   |=  1			/* 串口2允许中断									 */
#define		S2_Int_Disable()	IE2   &= ~1			/* 串口2禁止中断									 */
#define		S2_USE_P10P11()		P_SW2 &= ~1			/* UART2 使用P1口	默认							 */
#define		S2_USE_P46P47()		P_SW2 |=  1			/* UART2 使用P4.6 P4.7口							 */

//						  7      6      5      4      3      2     1     0        Reset Value
//sfr S3CON = 0xAC;		S3SM0  S3ST3  S3SM2  S3REN  S3TB8  S3RB8  S3TI  S3RI      00000000B		 //S3 Control

#define		S3_MODE0()			S3CON &= ~(1<<7)	/* 串口3模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S3_MODE1()			S3CON |=  (1<<7)	/* 串口3模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define		S3_8bit()			S3CON &= ~(1<<7)	/* 串口3模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S3_9bit()			S3CON |=  (1<<7)	/* 串口3模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define		S3_RX_Enable()		S3CON |=  (1<<4)	/* 允许串3接收									     */
#define		S3_RX_Disable()		S3CON &= ~(1<<4)	/* 禁止串3接收									     */
#define		TI3					(S3CON & 2)	        /* 判断TI3是否发送完成								 */
#define		RI3					(S3CON & 1)         /* 判断RI3是否接收完成								 */
#define		SET_TI3()			S3CON |=  2         /* 设置TI3(引起中断)								 */
#define		CLR_TI3()			S3CON &= ~2         /* 清除TI3											 */
#define		CLR_RI3()			S3CON &= ~1			/* 清除RI3											 */
#define		S3TB8_SET()			S3CON |=  (1<<3)	/* 设置TB8											 */
#define		S3TB8_CLR()			S3CON &= ~(1<<3)	/* 清除TB8											 */
#define		S3_Int_Enable()		IE2   |=  (1<<3)	/* 串口3允许中断								     */
#define		S3_Int_Disable()	IE2   &= ~(1<<3)	/* 串口3禁止中断								     */
#define 	S3_BRT_UseTimer3()	S3CON |=  (1<<6)	/* BRT select Timer3								 */
#define 	S3_BRT_UseTimer2()	S3CON &= ~(1<<6)	/* BRT select Timer2								 */
#define		S3_USE_P00P01()		P_SW2 &= ~2			/* UART3 使用P0.0 P0.1口	默认				     */
#define		S3_USE_P50P51()		P_SW2 |=  2			/* UART3 使用P5.0 P5.1口						     */

//						  7      6      5      4      3      2     1     0        Reset Value
//sfr S4CON = 0x84;		S4SM0  S4ST4  S4SM2  S4REN  S4TB8  S4RB8  S4TI  S4RI      00000000B		 //S4 Control

#define		S4_MODE0()			S4CON &= ~(1<<7)	/* 串口4模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S4_MODE1()			S4CON |=  (1<<7)	/* 串口4模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define		S4_8bit()			S4CON &= ~(1<<7)	/* 串口4模式0，8位UART，波特率 = 定时器的溢出率 / 4  */
#define		S4_9bit()			S4CON |=  (1<<7)	/* 串口4模式1，9位UART，波特率 = 定时器的溢出率 / 4  */
#define		S4_RX_Enable()		S4CON |=  (1<<4)	/* 允许串4接收									     */
#define		S4_RX_Disable()		S4CON &= ~(1<<4)	/* 禁止串4接收									     */
#define		TI4					(S4CON & 2)         /* 判断TI3是否发送完成							     */
#define		RI4					(S4CON & 1)         /* 判断RI3是否接收完成							     */
#define		SET_TI4()			S4CON |=  2			/* 设置TI3(引起中断)							     */
#define		CLR_TI4()			S4CON &= ~2			/* 清除TI3										     */
#define		CLR_RI4()			S4CON &= ~1			/* 清除RI3										     */
#define		S4TB8_SET()			S4CON |=  8			/* 设置TB8										     */
#define		S4TB8_CLR()			S4CON &= ~8			/* 清除TB8										     */
#define		S4_Int_Enable()		IE2   |=  (1<<4)	/* 串口4允许中断								     */
#define		S4_Int_Disable()	IE2   &= ~(1<<4)	/* 串口4禁止中断								     */
#define 	S4_BRT_UseTimer4()	S4CON |=  (1<<6)	/* BRT select Timer4								 */
#define 	S4_BRT_UseTimer2()	S4CON &= ~(1<<6)	/* BRT select Timer2								 */
#define		S4_USE_P02P03()		P_SW2 &= ~4			/* UART4 使用P0.2 P0.3口	默认				     */
#define		S4_USE_P52P53()		P_SW2 |=  4			/* UART4 使用P5.2 P5.3口						     */


/**********************************************************/
//						   7     6       5      4     3      2      1      0    Reset Value
//sfr AUXR  = 0x8E;		T0x12 T1x12 UART_M0x6  T2R  T2_C/T T2x12 EXTRAM  S1ST2  0000,0000	//Auxiliary Register

#define 	InternalXdata_Disable()		AUXR |=  2		/* 禁止使用内部xdata, 所有访问xdata都是访问外部xdata  */
#define 	InternalXdata_Enable()		AUXR &= ~2		/* 允许使用内部xdata, 当访问的地址在内部xdata范围时, 访问内部的xadta, 当地址超过内部xdata时, 访问外部xdata  */
#define		S1_M0x6()					AUXR |=  (1<<5)	/* UART Mode0 Speed is 6x Standard       */
#define		S1_M0x1()					AUXR &= ~(1<<5)	/* default,	UART Mode0 Speed is Standard */

//====================================
#define		Timer0_16bitAutoReload()	TMOD &= ~0x03					/* 16位自动重装	*/
#define		Timer0_16bit()				TMOD  = (TMOD & ~0x03) | 0x01	/* 16位         */
#define		Timer0_8bitAutoReload()		TMOD  = (TMOD & ~0x03) | 0x02	/* 8位自动重装	*/
#define		Timer0_16bitAutoRL_NoMask()	TMOD |=  0x03		/* 16位自动重装不可屏蔽中断	*/
#define 	Timer0_Run()	 			TR0 = 1				/* 允许定时器0计数			*/
#define 	Timer0_Stop()	 			TR0 = 0				/* 禁止定时器0计数			*/
#define		Timer0_Gate_INT0_P32()		TMOD |=  (1<<3)		/* 时器0由外部INT0高电平允许定时计数 */
#define		Timer0_AsTimer()			TMOD &= ~(1<<2)		/* 时器0用做定时器	*/
#define		Timer0_AsCounter()			TMOD |=  (1<<2)		/* 时器0用做计数器	*/
#define		Timer0_AsCounterP34()		TMOD |=  (1<<2)		/* 时器0用做计数器	*/
#define 	Timer0_1T()					AUXR |=  (1<<7)		/* Timer0 clodk = fo	*/
#define 	Timer0_12T()				AUXR &= ~(1<<7)		/* Timer0 clodk = fo/12	12分频,	default	*/
#define		Timer0_CLKO_Enable()		INT_CLKO |=  1			/* 允许 T0 溢出脉冲在T0(P3.5)脚输出，Fck0 = 1/2 T0 溢出率，T0可以1T或12T。	*/
#define		Timer0_CLKO_Disable()		INT_CLKO &= ~1
#define		Timer0_CLKO_Enable_P35()	INT_CLKO |=  1			/* 允许 T0 溢出脉冲在T0(P3.5)脚输出，Fck0 = 1/2 T0 溢出率，T0可以1T或12T。	*/
#define		Timer0_CLKO_Disable_P35()	INT_CLKO &= ~1
#define 	Timer0_InterruptEnable()	ET0 = 1				/* 允许Timer1中断.*/
#define 	Timer0_InterruptDisable()	ET0 = 0				/* 禁止Timer1中断.*/

#define		T0_Load(n)					TH0 = (n) / 256,	TL0 = (n) % 256
#define		T0_Load_us_1T(n)			Timer0_AsTimer(),Timer0_1T(), Timer0_16bitAutoReload(),TH0=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)/256, TL0=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)%256
#define		T0_Load_us_12T(n)			Timer0_AsTimer(),Timer0_12T(),Timer0_16bitAutoReload(),TH0=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)/256,TL0=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)%256
#define		T0_Frequency_1T_P35(n)		ET0=0,Timer0_AsTimer(),Timer0_1T(),Timer0_16bitAutoReload(),TH0=(65536-(n/2+MAIN_Fosc/2)/(n))/256,TL0=(65536-(n/2+MAIN_Fosc/2)/(n))%256,INT_CLKO |= 1,TR0=1		/* fx=fosc/(2*M)/n,  M=1 or M=12 */
#define		T0_Frequency_12T_P35(n)		ET0=0,Timer0_AsTimer(),Timer0_12T(),Timer0_16bitAutoReload(),TH0=(65536-(n/2+MAIN_Fosc/24)/(n))/256,TL0=(65536-(n/2+MAIN_Fosc/24)/(n))%256,INT_CLKO |= 1,TR0=1	/* fx=fosc/(2*M)/n,  M=1 or M=12 */

//====================================
#define		Timer1_16bitAutoReload()	TMOD &= ~0x30					/* 16位自动重装	*/
#define		Timer1_16bit()				TMOD  = (TMOD & ~0x30) | 0x10	/* 16位			*/
#define		Timer1_8bitAutoReload()		TMOD  = (TMOD & ~0x30) | 0x20	/* 8位自动重装	*/
#define 	Timer1_Run()	 			TR1 = 1				/* 允许定时器1计数			*/
#define 	Timer1_Stop()	 			TR1 = 0				/* 禁止定时器1计数			*/
#define		Timer1_Gate_INT1_P33()		TMOD |=  (1<<7)		/* 时器1由外部INT1高电平允许定时计数	*/
#define		Timer1_AsTimer()			TMOD &= ~(1<<6)		/* 时器1用做定时器			*/
#define		Timer1_AsCounter()			TMOD |=  (1<<6)		/* 时器1用做计数器			*/
#define		Timer1_AsCounterP35()		TMOD |=  (1<<6)		/* 时器1用做计数器			*/
#define 	Timer1_1T()					AUXR |=  (1<<6)		/* Timer1 clodk = fo		*/
#define 	Timer1_12T()				AUXR &= ~(1<<6)		/* Timer1 clodk = fo/12	12分频,	default	*/
#define		Timer1_CLKO_Enable()		INT_CLKO |=  2			/* 允许 T1 溢出脉冲在T1(P3.4)脚输出，Fck1 = 1/2 T1 溢出率，T1可以1T或12T。	*/
#define		Timer1_CLKO_Disable()		INT_CLKO &= ~2
#define		Timer1_CLKO_Enable_P34()	INT_CLKO |=  2			/* 允许 T1 溢出脉冲在T1(P3.4)脚输出，Fck1 = 1/2 T1 溢出率，T1可以1T或12T。	*/
#define		Timer1_CLKO_Disable_P34()	INT_CLKO &= ~2
#define 	Timer1_InterruptEnable()	ET1 = 1				/* 允许Timer1中断.	*/
#define 	Timer1_InterruptDisable()	ET1 = 0				/* 禁止Timer1中断.	*/

#define		T1_Load(n)					TH1 = (n) / 256,	TL1 = (n) % 256
#define		T1_Load_us_1T(n)			Timer1_AsTimer(),Timer1_1T(), Timer1_16bitAutoReload(),TH1=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)/256, TL1=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)%256
#define		T1_Load_us_12T(n)			Timer1_AsTimer(),Timer1_12T(),Timer1_16bitAutoReload(),TH1=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)/256,TL1=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)%256
#define		T1_Frequency_1T_P34(n)		ET1=0,Timer1_AsTimer(),Timer1_1T(),Timer1_16bitAutoReload(),TH1=(65536-(n/2+MAIN_Fosc/2)/(n))/256,TL1=(65536-(n/2+MAIN_Fosc/2)/(n))%256,INT_CLKO |= 2,TR1=1		/* fx=fosc/(2*M)/n,  M=1 or M=12 */
#define		T1_Frequency_12T_P34(n)		ET1=0,Timer1_AsTimer(),Timer1_12T(),Timer1_16bitAutoReload(),TH1=(65536-(n/2+MAIN_Fosc/24)/(n))/256,TL1=(65536-(n/2+MAIN_Fosc/24)/(n))%256,INT_CLKO |= 2,TR1=1	/* fx=fosc/(2*M)/n,  M=1 or M=12 */

//====================================
#define 	Timer2_Run()	 			AUXR |=  (1<<4)	/* 允许定时器2计数	*/
#define 	Timer2_Stop()	 			AUXR &= ~(1<<4)	/* 禁止定时器2计数	*/
#define		Timer2_AsTimer()			AUXR &= ~(1<<3)	/* 时器2用做定时器	*/
#define		Timer2_AsCounter()			AUXR |=  (1<<3)	/* 时器2用做计数器	*/
#define		Timer2_AsCounterP31()		AUXR |=  (1<<3)	/* 时器2用做计数器	*/
#define 	Timer2_1T()					AUXR |=  (1<<2)	/* Timer0 clock = fo	*/
#define 	Timer2_12T()				AUXR &= ~(1<<2)	/* Timer0 clock = fo/12	12分频,	default	*/
#define		Timer2_CLKO_Enable()		INT_CLKO |=  4		/* 允许 T2 溢出脉冲在P1.3脚输出，Fck2 = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer2_CLKO_Disable()		INT_CLKO &= ~4
#define		Timer2_CLKO_Enable_P13()	INT_CLKO |=  4		/* 允许 T2 溢出脉冲在P1.3脚输出，Fck2 = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer2_CLKO_Disable_P13()	INT_CLKO &= ~4
#define 	Timer2_InterruptEnable()	IE2  |=  (1<<2)	/* 允许Timer2中断.	*/
#define 	Timer2_InterruptDisable()	IE2  &= ~(1<<2)	/* 禁止Timer2中断.	*/

#define		T2_Load(n)					TH2 = (n) / 256,	TL2 = (n) % 256
#define		T2_Load_us_1T(n)			Timer2_AsTimer(),Timer2_1T(), TH2=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)/256, TL2=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)%256
#define		T2_Load_us_12T(n)			Timer2_AsTimer(),Timer2_12T(),TH2=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)/256,TL2=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)%256
#define		T2_Frequency_1T_P13(n)		Timer2_InterruptDisable(),Timer2_AsTimer(),Timer2_1T(), TH2=(65536-(n/2+MAIN_Fosc/2)/(n))/256, TL2=(65536-(n/2+MAIN_Fosc/2)/(n))%256, Timer2_CLKO_Enable_P13(),Timer2_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */
#define		T2_Frequency_12T_P13(n)		Timer2_InterruptDisable(),Timer2_AsTimer(),Timer2_12T(),TH2=(65536-(n/2+MAIN_Fosc/24)/(n))/256,TL2=(65536-(n/2+MAIN_Fosc/24)/(n))%256,Timer2_CLKO_Enable_P13(),Timer2_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */

//====================================
#define 	Timer3_Run()	 			T4T3M |=  (1<<3)	/* 允许定时器3计数	*/
#define 	Timer3_Stop()	 			T4T3M &= ~(1<<3)	/* 禁止定时器3计数	*/
#define		Timer3_AsTimer()			T4T3M &= ~(1<<2)	/* 时器3用做定时器	*/
#define		Timer3_AsCounter()			T4T3M |=  (1<<2)	/* 时器3用做计数器, P0.5为外部脉冲	*/
#define		Timer3_AsCounterP05()		T4T3M |=  (1<<2)	/* 时器3用做计数器, P0.5为外部脉冲	*/
#define 	Timer3_1T()					T4T3M |=  (1<<1)	/* 1T模式	*/
#define 	Timer3_12T()				T4T3M &= ~(1<<1)	/* 12T模式,	default	*/
#define		Timer3_CLKO_Enable()		T4T3M |=  1			/* 允许T3溢出脉冲在T3(P0.4)脚输出，Fck = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer3_CLKO_Disable()		T4T3M &= ~1			/* 禁止T3溢出脉冲在T3(P0.4)脚输出	*/
#define		Timer3_CLKO_Enable_P04()	T4T3M |=  1			/* 允许T3溢出脉冲在T3(P0.4)脚输出，Fck = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer3_CLKO_Disable_P04()	T4T3M &= ~1			/* 禁止T3溢出脉冲在T3(P0.4)脚输出	*/
#define 	Timer3_InterruptEnable()	IE2  |=  (1<<5)		/* 允许Timer3中断.	*/
#define 	Timer3_InterruptDisable()	IE2  &= ~(1<<5)		/* 禁止Timer3中断.	*/

#define		T3_Load(n)					TH3 = (n) / 256,	TL3 = (n) % 256
#define		T3_Load_us_1T(n)			Timer3_AsTimer(),Timer3_1T(), TH3=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)/256, TL3=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)%256
#define		T3_Load_us_12T(n)			Timer3_AsTimer(),Timer3_12T(),TH3=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)/256,TL3=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)%256
#define		T3_Frequency_1T_P04(n)		Timer3_InterruptDisable(),Timer3_AsTimer(),Timer3_1T(), TH3=(65536-(n/2+MAIN_Fosc/2)/(n))/256, TL3=(65536-(n/2+MAIN_Fosc/2)/(n))%256, Timer3_CLKO_P04_Enable,Timer3_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */
#define		T3_Frequency_12T_P04(n)		Timer3_InterruptDisable(),Timer3_AsTimer(),Timer3_12T(),TH3=(65536-(n/2+MAIN_Fosc/24)/(n))/256,TL3=(65536-(n/2+MAIN_Fosc/24)/(n))%256,Timer3_CLKO_P04_Enable,Timer3_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */

//====================================
#define 	Timer4_Run()	 			T4T3M |=  (1<<7)	/* 允许定时器4计数	*/
#define 	Timer4_Stop()	 			T4T3M &= ~(1<<7)	/* 禁止定时器4计数	*/
#define		Timer4_AsTimer()			T4T3M &= ~(1<<6)	/* 时器4用做定时器  */
#define		Timer4_AsCounter()			T4T3M |=  (1<<6)	/* 时器4用做计数器, P0.7为外部脉冲	*/
#define		Timer4_AsCounterP07()		T4T3M |=  (1<<6)	/* 时器4用做计数器, P0.7为外部脉冲	*/
#define 	Timer4_1T()					T4T3M |=  (1<<5)	/* 1T模式	*/
#define 	Timer4_12T()				T4T3M &= ~(1<<5)	/* 12T模式,	default	*/
#define		Timer4_CLKO_Enable()		T4T3M |=  (1<<4)	/* 允许T4溢出脉冲在T4(P0.6)脚输出，Fck = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer4_CLKO_Disable()		T4T3M &= ~(1<<4)	/* 禁止T4溢出脉冲在T4(P0.6)脚输出	*/
#define		Timer4_CLKO_Enable_P06()	T4T3M |=  (1<<4)	/* 允许T4溢出脉冲在T4(P0.6)脚输出，Fck = 1/2 T2 溢出率，T2可以1T或12T。	*/
#define		Timer4_CLKO_Disable_P06()	T4T3M &= ~(1<<4)	/* 禁止T4溢出脉冲在T4(P0.6)脚输出	*/
#define 	Timer4_InterruptEnable()	IE2  |=  (1<<6)		/* 允许Timer4中断.	*/
#define 	Timer4_InterruptDisable()	IE2  &= ~(1<<6)		/* 禁止Timer4中断.	*/

#define		T4_Load(n)					TH4 = (n) / 256,	TL4 = (n) % 256
#define		T4_Load_us_1T(n)			Timer4_AsTimer(),Timer4_1T(), TH4=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)/256, TL4=(65536-((MAIN_Fosc/1000)*(n)+500)/1000)%256
#define		T4_Load_us_12T(n)			Timer4_AsTimer(),Timer4_12T(),TH4=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)/256,TL4=(65536-((MAIN_Fosc/12000)*(n)+500)/1000)%256
#define		T4_Frequency_1T_P06(n)		Timer4_InterruptDisable(),Timer4_AsTimer(),Timer4_1T(), TH4=(65536-(n/2+MAIN_Fosc/2)/(n))/256, TL4=(65536-(n/2+MAIN_Fosc/2)/(n))%256, Timer4_CLKO_P06_Enable(),Timer4_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */
#define		T4_Frequency_12T_P06(n)		Timer4_InterruptDisable(),Timer4_AsTimer(),Timer4_12T(),TH4=(65536-(n/2+MAIN_Fosc/24)/(n))/256,TL4=(65536-(n/2+MAIN_Fosc/24)/(n))%256,Timer4_CLKO_P06_Enable(),Timer4_Run()	/* fx=fosc/(2*M)/n,  M=1 or M=12 */
//====================================================================================================================

//sfr WDT_CONTR = 0xC1; //Watch-Dog-Timer Control register
//                                      7     6     5      4       3      2   1   0     Reset Value
//                                  WDT_FLAG  -  EN_WDT CLR_WDT IDLE_WDT PS2 PS1 PS0    xx00,0000
#define D_WDT_FLAG			(1<<7)
#define D_EN_WDT			(1<<5)
#define D_CLR_WDT			(1<<4)	/* auto clear	*/
#define D_IDLE_WDT			(1<<3)	/* WDT counter when Idle	*/
#define D_WDT_SCALE_2		0
#define D_WDT_SCALE_4		1
#define D_WDT_SCALE_8		2		/* T=393216*N/fo	*/
#define D_WDT_SCALE_16		3
#define D_WDT_SCALE_32		4
#define D_WDT_SCALE_64		5
#define D_WDT_SCALE_128		6
#define D_WDT_SCALE_256		7

#define	WDT_reset(n)	WDT_CONTR = D_EN_WDT + D_CLR_WDT + D_IDLE_WDT + (n)		/* 初始化WDT，喂狗 */


//						  7     6      5    4     3      2    1     0     Reset Value
//sfr PCON   = 0x87;	SMOD  SMOD0  LVDF  POF   GF1    GF0   PD   IDL    0001,0000	 //Power Control
//SMOD		//串口双倍速
//SMOD0
#define		LVDF		(1<<5)	/* P4.6低压检测标志 */
//POF
//GF1
//GF0
//#define 	D_PD		2		/* set 1, power down mode */
//#define 	D_IDLE		1		/* set 1, idle mode */
#define		MCU_IDLE()			PCON |= 1	/* MCU 进入 IDLE 模式 */
#define		MCU_POWER_DOWN()	PCON |= 2	/* MCU 进入 睡眠 模式 */


//sfr IAP_CMD   = 0xC5;
#define		IAP_STANDBY()	IAP_CMD = 0		/* ISP空闲命令（禁止）*/
#define		IAP_READ()		IAP_CMD = 1		/* ISP读出命令		*/
#define		IAP_WRITE()		IAP_CMD = 2		/* ISP写入命令		*/
#define		IAP_ERASE()		IAP_CMD = 3		/* ISP擦除命令		*/

//sfr IAP_TRIG  = 0xC6;
#define 	IAP_TRIG()	IAP_TRIG = 0x5A,	IAP_TRIG = 0xA5		/* ISP触发命令 */

//							  7    6    5      4    3    2    1     0    Reset Value
//sfr IAP_CONTR = 0xC7;		IAPEN SWBS SWRST CFAIL  -    -    -     -    0000,x000	//IAP Control Register
#define IAP_EN			(1<<7)
#define IAP_SWBS		(1<<6)
#define IAP_SWRST		(1<<5)
#define IAP_CMD_FAIL	(1<<4)


/* ADC Register */
//								7       6      5       4         3      2    1    0   Reset Value
//sfr ADC_CONTR = 0xBC;		ADC_POWER SPEED1 SPEED0 ADC_FLAG ADC_START CHS2 CHS1 CHS0 0000,0000	/* AD 转换控制寄存器 */
//sfr ADC_RES  = 0xBD;		ADCV.9 ADCV.8 ADCV.7 ADCV.6 ADCV.5 ADCV.4 ADCV.3 ADCV.2	  0000,0000	/* A/D 转换结果高8位 */
//sfr ADC_RESL = 0xBE;												  ADCV.1 ADCV.0	  0000,0000	/* A/D 转换结果低2位 */
//sfr ADC_CONTR  = 0xBC;	//直接用MOV操作，不要用与或



#define		SPI_USE_P12P13P14P15()	P_SW1 &= ~0x0c					/* 将SPI切换到P12(SS) P13(MOSI) P14(MISO) P15(SCLK)(上电默认)。*/
#define		SPI_USE_P22P23P24P25()	P_SW1 = (P_SW1 & ~0x0c) | 0x04	/* 将SPI切换到P22(SS) P23(MOSI) P24(MISO) P25(SCLK)。*/
#define		SPI_USE_P35P34P33P32()	P_SW1 =  P_SW1 | 0x0C			/* 将SPI切换到P35(SS) P34(MOSI) P33(MISO) P32(SCLK)。*/


/*
;PCA_PWMn:    7       6     5   4   3   2     1       0
;			EBSn_1	EBSn_0	-	-	-	-	EPCnH	EPCnL
;B5-B2:		保留
;B1(EPCnH):	在PWM模式下，与CCAPnH组成9位数。
;B0(EPCnL):	在PWM模式下，与CCAPnL组成9位数。
*/
#define		PWM0_NORMAL()	PCA_PWM0 &= ~3					/* PWM0正常输出(默认)	*/
#define		PWM0_OUT_0()	PCA_PWM0 |=  3, CCAP0H = 0xff	/* PWM0一直输出0		*/
#define		PWM0_OUT_1()	PCA_PWM0 &= 0xc0, CCAP0H = 0	/* PWM0一直输出1		*/

#define		PWMA_NORMAL()	PCA_PWMA &= ~3					/* PWMA正常输出(默认)	*/
#define		PWMA_OUT_0()	PCA_PWMA |=  3, CCAP1H = 0xff	/* PWMA一直输出0		*/
#define		PWMA_OUT_1()	PCA_PWMA &= 0xc0, CCAP1H = 0	/* PWMA一直输出1		*/

#define		PWMB_NORMAL()	PCA_PWMB &= ~3					/* PWMB正常输出(默认)	*/
#define		PWMB_OUT_0()	PCA_PWMB |=  3, CCAP2H = 0xff	/* PWMB一直输出0		*/
#define		PWMB_OUT_1()	PCA_PWMB &= 0xc0, CCAP2H = 0	/* PWMB一直输出1		*/


/**********************************************************/
typedef 	unsigned char	u8;
typedef 	unsigned int	u16;
typedef 	unsigned long	u32;

/**********************************************************/
#define NOP1()  _nop_()
#define NOP2()  NOP1(),NOP1()
#define NOP3()  NOP2(),NOP1()
#define NOP4()  NOP3(),NOP1()
#define NOP5()  NOP4(),NOP1()
#define NOP6()  NOP5(),NOP1()
#define NOP7()  NOP6(),NOP1()
#define NOP8()  NOP7(),NOP1()
#define NOP9()  NOP8(),NOP1()
#define NOP10() NOP9(),NOP1()
#define NOP11() NOP10(),NOP1()
#define NOP12() NOP11(),NOP1()
#define NOP13() NOP12(),NOP1()
#define NOP14() NOP13(),NOP1()
#define NOP15() NOP14(),NOP1()
#define NOP16() NOP15(),NOP1()
#define NOP17() NOP16(),NOP1()
#define NOP18() NOP17(),NOP1()
#define NOP19() NOP18(),NOP1()
#define NOP20() NOP19(),NOP1()
#define NOP21() NOP20(),NOP1()
#define NOP22() NOP21(),NOP1()
#define NOP23() NOP22(),NOP1()
#define NOP24() NOP23(),NOP1()
#define NOP25() NOP24(),NOP1()
#define NOP26() NOP25(),NOP1()
#define NOP27() NOP26(),NOP1()
#define NOP28() NOP27(),NOP1()
#define NOP29() NOP28(),NOP1()
#define NOP30() NOP29(),NOP1()
#define NOP31() NOP30(),NOP1()
#define NOP32() NOP31(),NOP1()
#define NOP33() NOP32(),NOP1()
#define NOP34() NOP33(),NOP1()
#define NOP35() NOP34(),NOP1()
#define NOP36() NOP35(),NOP1()
#define NOP37() NOP36(),NOP1()
#define NOP38() NOP37(),NOP1()
#define NOP39() NOP38(),NOP1()
#define NOP40() NOP39(),NOP1()
#define NOP(N)  NOP##N()


/**********************************************/
#define	Pin0		0x01	//IO引脚 Px.0
#define	Pin1		0x02	//IO引脚 Px.1
#define	Pin2		0x04	//IO引脚 Px.2
#define	Pin3		0x08	//IO引脚 Px.3
#define	Pin4		0x10	//IO引脚 Px.4
#define	Pin5		0x20	//IO引脚 Px.5
#define	Pin6		0x40	//IO引脚 Px.6
#define	Pin7		0x80	//IO引脚 Px.7
#define	PinAll		0xFF	//IO所有引脚

#define	GPIO_Pin_0		0x01	//IO引脚 Px.0
#define	GPIO_Pin_1		0x02	//IO引脚 Px.1
#define	GPIO_Pin_2		0x04	//IO引脚 Px.2
#define	GPIO_Pin_3		0x08	//IO引脚 Px.3
#define	GPIO_Pin_4		0x10	//IO引脚 Px.4
#define	GPIO_Pin_5		0x20	//IO引脚 Px.5
#define	GPIO_Pin_6		0x40	//IO引脚 Px.6
#define	GPIO_Pin_7		0x80	//IO引脚 Px.7
#define	GPIO_Pin_All	0xFF	//IO所有引脚

#define	GPIO_PullUp		0	//上拉准双向口
#define	GPIO_HighZ		1	//浮空输入
#define	GPIO_OUT_OD		2	//开漏输出
#define	GPIO_OUT_PP		3	//推挽输出

#define	GPIO_P0			0		//
#define	GPIO_P1			1
#define	GPIO_P2			2
#define	GPIO_P3			3
#define	GPIO_P4			4
#define	GPIO_P5			5
#define	GPIO_P6			6
#define	GPIO_P7			7

#define P0n_standard(bitn)			P0M1 &= ~(bitn),	P0M0 &= ~(bitn)	/* 00  */
#define P0n_push_pull(bitn)			P0M1 &= ~(bitn),	P0M0 |=  (bitn)	/* 01  */
#define P0n_pure_input(bitn)		P0M1 |=  (bitn),	P0M0 &= ~(bitn)	/* 10  */
#define P0n_open_drain(bitn)		P0M1 |=  (bitn),	P0M0 |=  (bitn)	/* 11  */

#define P1n_standard(bitn)			P1M1 &= ~(bitn),	P1M0 &= ~(bitn)
#define P1n_push_pull(bitn)			P1M1 &= ~(bitn),	P1M0 |=  (bitn)
#define P1n_pure_input(bitn)		P1M1 |=  (bitn),	P1M0 &= ~(bitn)
#define P1n_open_drain(bitn)		P1M1 |=  (bitn),	P1M0 |=  (bitn)

#define P2n_standard(bitn)			P2M1 &= ~(bitn),	P2M0 &= ~(bitn)
#define P2n_push_pull(bitn)			P2M1 &= ~(bitn),	P2M0 |=  (bitn)
#define P2n_pure_input(bitn)		P2M1 |=  (bitn),	P2M0 &= ~(bitn)
#define P2n_open_drain(bitn)		P2M1 |=  (bitn),	P2M0 |=  (bitn)

#define P3n_standard(bitn)			P3M1 &= ~(bitn),	P3M0 &= ~(bitn)
#define P3n_push_pull(bitn)			P3M1 &= ~(bitn),	P3M0 |=  (bitn)
#define P3n_pure_input(bitn)		P3M1 |=  (bitn),	P3M0 &= ~(bitn)
#define P3n_open_drain(bitn)		P3M1 |=  (bitn),	P3M0 |=  (bitn)

#define P4n_standard(bitn)			P4M1 &= ~(bitn),	P4M0 &= ~(bitn)
#define P4n_push_pull(bitn)			P4M1 &= ~(bitn),	P4M0 |=  (bitn)
#define P4n_pure_input(bitn)		P4M1 |=  (bitn),	P4M0 &= ~(bitn)
#define P4n_open_drain(bitn)		P4M1 |=  (bitn),	P4M0 |=  (bitn)

#define P5n_standard(bitn)			P5M1 &= ~(bitn),	P5M0 &= ~(bitn)
#define P5n_push_pull(bitn)			P5M1 &= ~(bitn),	P5M0 |=  (bitn)
#define P5n_pure_input(bitn)		P5M1 |=  (bitn),	P5M0 &= ~(bitn)
#define P5n_open_drain(bitn)		P5M1 |=  (bitn),	P5M0 |=  (bitn)

#define P6n_standard(bitn)			P6M1 &= ~(bitn),	P6M0 &= ~(bitn)
#define P6n_push_pull(bitn)			P6M1 &= ~(bitn),	P6M0 |=  (bitn)
#define P6n_pure_input(bitn)		P6M1 |=  (bitn),	P6M0 &= ~(bitn)
#define P6n_open_drain(bitn)		P6M1 |=  (bitn),	P6M0 |=  (bitn)

#define P7n_standard(bitn)			P7M1 &= ~(bitn),	P7M0 &= ~(bitn)
#define P7n_push_pull(bitn)			P7M1 &= ~(bitn),	P7M0 |=  (bitn)
#define P7n_pure_input(bitn)		P7M1 |=  (bitn),	P7M0 &= ~(bitn)
#define P7n_open_drain(bitn)		P7M1 |=  (bitn),	P7M0 |=  (bitn)


/****************************************************************/



#define		LVD_InterruptEnable()		ELVD = 1
#define		LVD_InterruptDisable()		ELVD = 0


//sfr WKTCL = 0xAA;	//STC11F\10和STC15系列 唤醒定时器低字节
//sfr WKTCH = 0xAB;	//STC11F\10和STC15系列 唤醒定时器高字节
//	B7		B6	B5	B4	B3	B2	B1	B0		B7	B6	B5	B4	B3	B2	B1	B0
//	WKTEN				S11	S10	S9	S8		S7	S6	S5	S4	S3	S2	S1	S0	n * 560us
#define		WakeTimerDisable()		WKTCH &= 0x7f	/* WKTEN = 0		禁止睡眠唤醒定时器 */
#define		WakeTimerSet(scale)		WKTCL = (scale) % 256,WKTCH = (scale) / 256 | 0x80	/* WKTEN = 1	允许睡眠唤醒定时器 */



/////////////////////////////////////////////////
//Interrupt Vector
/////////////////////////////////////////////////
#define INT0_VECTOR         0       //FF0003H
#define TMR0_VECTOR         1       //FF000BH
#define INT1_VECTOR         2       //FF0013H
#define TMR1_VECTOR         3       //FF001BH
#define UART1_VECTOR        4       //FF0023H
#define ADC_VECTOR          5       //FF002BH
#define LVD_VECTOR          6       //FF0033H
#define PCA_VECTOR          7       //FF003BH
#define UART2_VECTOR        8       //FF0043H
#define SPI_VECTOR          9       //FF004BH
#define INT2_VECTOR         10      //FF0053H
#define INT3_VECTOR         11      //FF005BH
#define TMR2_VECTOR         12      //FF0063H
#define USER_VECTOR         13      //FF006BH
#define INT4_VECTOR         16      //FF0083H
#define UART3_VECTOR        17      //FF008BH
#define UART4_VECTOR        18      //FF0093H
#define TMR3_VECTOR         19      //FF009BH
#define TMR4_VECTOR         20      //FF00A3H
#define CMP_VECTOR          21      //FF00ABH
#define I2C_VECTOR          24      //FF00C3H
#define USB_VECTOR          25      //FF00CBH
#define PWMA_VECTOR         26      //FF00D3H
#define PWMB_VECTOR         27      //FF00DBH

#define RTC_VECTOR          36      //FF0123H
#define P0_VECTOR           37      //FF012BH
#define P1_VECTOR           38      //FF0133H
#define P2_VECTOR           39      //FF013BH
#define P3_VECTOR           40      //FF0143H
#define P4_VECTOR           41      //FF014BH
#define P5_VECTOR           42      //FF0153H
#define P6_VECTOR           43      //FF015BH
#define P7_VECTOR           44      //FF0163H
#define DMA_M2M_VECTOR      47      //FF017BH
#define DMA_ADC_VECTOR      48      //FF0183H
#define DMA_SPI_VECTOR      49      //FF018BH
#define DMA_UR1T_VECTOR     50      //FF0193H
#define DMA_UR1R_VECTOR     51      //FF019BH
#define DMA_UR2T_VECTOR     52      //FF01A3H
#define DMA_UR2R_VECTOR     53      //FF01ABH
#define DMA_UR3T_VECTOR     54      //FF01B3H
#define DMA_UR3R_VECTOR     55      //FF01BBH
#define DMA_UR4T_VECTOR     56      //FF01C3H
#define DMA_UR4R_VECTOR     57      //FF01CBH
#define DMA_LCM_VECTOR      58      //FF01D3H
#define LCM_VECTOR          59      //FF01DBH
#define DMA_I2CT_VECTOR     60      //FF01E3H
#define DMA_I2CR_VECTOR     61      //FF01EBH
#define I2S_VECTOR          62      //FF01F3H
#define DMA_I2ST_VECTOR     63      //FF01FBH
#define DMA_I2SR_VECTOR     64      //FF0203H
#define DMA_QSPI_VECTOR     65      //020BH
#define QSPI_VECTOR         66      //0213H
#define TMR11_VECTOR        67      //021BH
#define DMA_PWMAT_VECTOR    72      //0243H
#define DMA_PWMAR_VECTOR    73      //024BH

//=============================================================

#define	TRUE	1
#define	FALSE	0

#define	PolityLow			0	//低优先级中断
#define	PolityHigh			1	//高优先级中断

#define		ENABLE		1
#define		DISABLE		0

#endif
