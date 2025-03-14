;AI8051U.INC

$SAVE
$NOLIST

/////////////////////////////////////////////////

_BIT0       EQU         01H
_BIT1       EQU         02H
_BIT2       EQU         04H
_BIT3       EQU         08H
_BIT4       EQU         10H
_BIT5       EQU         20H
_BIT6       EQU         40H
_BIT7       EQU         80H

/////////////////////////////////////////////////

P0          DATA        080H
P00         BIT         P0.0
P01         BIT         P0.1
P02         BIT         P0.2
P03         BIT         P0.3
P04         BIT         P0.4
P05         BIT         P0.5
P06         BIT         P0.6
P07         BIT         P0.7

SP          DATA        081H
DPL         DATA        082H
DPH         DATA        083H
DPXL        DATA        084H
SPH         DATA        085H

PCON        DATA        087H
SMOD        BIT         PCON.7
SMOD0       BIT         PCON.6
LVDF        BIT         PCON.5
POF         BIT         PCON.4
GF1         BIT         PCON.3
GF0         BIT         PCON.2
PD          BIT         PCON.1
IDL         BIT         PCON.0

TCON        DATA        088H
TF1         BIT         TCON.7
TR1         BIT         TCON.6
TF0         BIT         TCON.5
TR0         BIT         TCON.4
IE1         BIT         TCON.3
IT1         BIT         TCON.2
IE0         BIT         TCON.1
IT0         BIT         TCON.0

TMOD        DATA        089H
T1_GATE     BIT         TMOD.7
T1_CT       BIT         TMOD.6
T1_M1       BIT         TMOD.5
T1_M0       BIT         TMOD.4
T0_GATE     BIT         TMOD.3
T0_CT       BIT         TMOD.2
T0_M1       BIT         TMOD.1
T0_M0       BIT         TMOD.0

TL0         DATA        08AH
TL1         DATA        08BH
TH0         DATA        08CH
TH1         DATA        08DH

AUXR        DATA        08EH
T012        BIT         AUXR.7
T1X12       BIT         AUXR.6
S1M06       BIT         AUXR.5
T2R         BIT         AUXR.4
T2_CT       BIT         AUXR.3
T2X12       BIT         AUXR.2
EXTRAM      BIT         AUXR.1
S1BRT       BIT         AUXR.0

INTCLKO     DATA        08FH
EX4         BIT         INTCLKO.6
EX3         BIT         INTCLKO.5
EX2         BIT         INTCLKO.4
T2CLKO      BIT         INTCLKO.2
T1CLKO      BIT         INTCLKO.1
T0CLKO      BIT         INTCLKO.0

P1          DATA        090H
P10         BIT         P1.0
P11         BIT         P1.1
P12         BIT         P1.2
P13         BIT         P1.3
P14         BIT         P1.4
P15         BIT         P1.5
P16         BIT         P1.6
P17         BIT         P1.7

P1M1        DATA        091H
P1M0        DATA        092H
P0M1        DATA        093H
P0M0        DATA        094H
P2M1        DATA        095H
P2M0        DATA        096H

AUXR2       DATA        097H
RAMTINY     BIT         AUXR2.7
CPUMODE     BIT         AUXR2.6
RAMEXE      BIT         AUXR2.5
CANFD       BIT         AUXR2.4
CANSEL      BIT         AUXR2.3
CAN2EN      BIT         AUXR2.2
CANEN       BIT         AUXR2.1
LINEN       BIT         AUXR2.0

SCON        DATA        098H
SM0         BIT         SCON.7
SM1         BIT         SCON.6
SM2         BIT         SCON.5
REN         BIT         SCON.4
TB8         BIT         SCON.3
RB8         BIT         SCON.2
TI          BIT         SCON.1
RI          BIT         SCON.0

SBUF        DATA        099H

S2CON       DATA        09AH
S2SM0       BIT         S2CON.7
S2SM1       BIT         S2CON.6
S2SM2       BIT         S2CON.5
S2REN       BIT         S2CON.4
S2TB8       BIT         S2CON.3
S2RB8       BIT         S2CON.2
S2TI        BIT         S2CON.1
S2RI        BIT         S2CON.0

S2BUF       DATA        09BH

IRCBAND     DATA        09DH
USBCKS      BIT         IRCBAND.7
USBCKS2     BIT         IRCBAND.6
HIRCSEL1    BIT         IRCBAND.1
HIRCSEL0    BIT         IRCBAND.0

LIRTRIM     DATA        09EH
IRTRIM      DATA        09FH

P2          DATA        0A0H
P20         BIT         P2.0
P21         BIT         P2.1
P22         BIT         P2.2
P23         BIT         P2.3
P24         BIT         P2.4
P25         BIT         P2.5
P26         BIT         P2.6
P27         BIT         P2.7

BUS_SPEED   DATA        0A1H

P_SW1       DATA        0A2H
S1_S1       BIT         P_SW1.7
S1_S0       BIT         P_SW1.6
CAN_S1      BIT         P_SW1.5
CAN_S0      BIT         P_SW1.4
SPI_S1      BIT         P_SW1.3
SPI_S0      BIT         P_SW1.2
LIN_S1      BIT         P_SW1.1
LIN_S0      BIT         P_SW1.0

VRTRIM      DATA        0A6H

IE          DATA        0A8H
EA          BIT         IE.7
ELVD        BIT         IE.6
EADC        BIT         IE.5
ES          BIT         IE.4
ET1         BIT         IE.3
EX1         BIT         IE.2
ET0         BIT         IE.1
EX0         BIT         IE.0

SADDR       DATA        0A9H
WKTCL       DATA        0AAH
WKTCH       DATA        0ABH
WKTEN       BIT         WKTCH.7

S3CON       DATA        0ACH
S3SM0       BIT         S3CON.7
S3ST3       BIT         S3CON.6
S3SM2       BIT         S3CON.5
S3REN       BIT         S3CON.4
S3TB8       BIT         S3CON.3
S3RB8       BIT         S3CON.2
S3TI        BIT         S3CON.1
S3RI        BIT         S3CON.0

S3BUF       DATA        0ADH
TA          DATA        0AEH

IE2         DATA        0AFH
EUSB        BIT         IE2.7
ET4         BIT         IE2.6
ET3         BIT         IE2.5
ES4         BIT         IE2.4
ES3         BIT         IE2.3
ET2         BIT         IE2.2
ESPI        BIT         IE2.1
ES2         BIT         IE2.0

P3          DATA        0B0H
P30         BIT         P3.0
P31         BIT         P3.1
P32         BIT         P3.2
P33         BIT         P3.3
P34         BIT         P3.4
P35         BIT         P3.5
P36         BIT         P3.6
P37         BIT         P3.7

RD          BIT         P3.7
WR          BIT         P3.6
T1          BIT         P3.5
T0          BIT         P3.4
INT1        BIT         P3.3
INT0        BIT         P3.2
TXD         BIT         P3.1
RXD         BIT         P3.0

P3M1        DATA        0B1H
P3M0        DATA        0B2H
P4M1        DATA        0B3H
P4M0        DATA        0B4H

IP2         DATA        0B5H
PUSB        BIT         IP2.7
PI2C        BIT         IP2.6
PCMP        BIT         IP2.5
PX4         BIT         IP2.4
PPWMB       BIT         IP2.3
PPWMA       BIT         IP2.2
PSPI        BIT         IP2.1
PS2         BIT         IP2.0

IP2H        DATA        0B6H
PUSBH       BIT         IP2H.7
PI2CH       BIT         IP2H.6
PCMPH       BIT         IP2H.5
PX4H        BIT         IP2H.4
PPWMBH      BIT         IP2H.3
PPWMAH      BIT         IP2H.2
PSPIH       BIT         IP2H.1
PS2H        BIT         IP2H.0

IPH         DATA        0B7H
PPCAH       BIT         IPH.6
PLVDH       BIT         IPH.6
PADCH       BIT         IPH.5
PSH         BIT         IPH.4
PT1H        BIT         IPH.3
PX1H        BIT         IPH.2
PT0H        BIT         IPH.1
PX0H        BIT         IPH.0

IP          DATA        0B8H
PPCA        BIT         IP.7
PLVD        BIT         IP.6
PADC        BIT         IP.5
PS          BIT         IP.4
PT1         BIT         IP.3
PX1         BIT         IP.2
PT0         BIT         IP.1
PX0         BIT         IP.0

SADEN       DATA        0B9H

P_SW2       DATA        0BAH
EAXFR       BIT         P_SW2.7
I2C_S1      BIT         P_SW2.5
I2C_S0      BIT         P_SW2.4
CMPO_S      BIT         P_SW2.3
S4_S        BIT         P_SW2.2
S3_S        BIT         P_SW2.1
S2_S        BIT         P_SW2.0

P_SW3       DATA        0BBH
I2S_S1      BIT         P_SW3.7
I2S_S0      BIT         P_SW3.6
S2SPI_S1    BIT         P_SW3.5
S2SPI_S0    BIT         P_SW3.4
S1SPI_S1    BIT         P_SW3.3
S1SPI_S0    BIT         P_SW3.2
CAN2_S1     BIT         P_SW3.1
CAN2_S0     BIT         P_SW3.0

ADC_CONTR   DATA        0BCH
ADC_POWER   BIT         ADC_CONTR.7
ADC_START   BIT         ADC_CONTR.6
ADC_FLAG    BIT         ADC_CONTR.5
ADC_EPWMT   BIT         ADC_CONTR.4

ADC_RES     DATA        0BDH
ADC_RESL    DATA        0BEH

P_SW4       DATA        0BFH
QSPI_S1     BIT         P_SW4.1
QSPI_S0     BIT         P_SW4.0

P4          DATA        0C0H
P40         BIT         P4.0
P41         BIT         P4.1
P42         BIT         P4.2
P43         BIT         P4.3
P44         BIT         P4.4
P45         BIT         P4.5
P46         BIT         P4.6
P47         BIT         P4.7

WDT_CONTR   DATA        0C1H
WDT_FLAG    BIT         WDT_CONTR.7
EN_WDT      BIT         WDT_CONTR.5
CLR_WDT     BIT         WDT_CONTR.4
IDL_WDT     BIT         WDT_CONTR.3

IAP_DATA    DATA        0C2H
IAP_ADDRH   DATA        0C3H
IAP_ADDRL   DATA        0C4H
IAP_CMD     DATA        0C5H
IAP_TRIG    DATA        0C6H

IAP_CONTR   DATA        0C7H
IAPEN       BIT         IAP_CONTR.7
SWBS        BIT         IAP_CONTR.6
SWRST       BIT         IAP_CONTR.5
CMD_FAIL    BIT         IAP_CONTR.4
SWBS2       BIT         IAP_CONTR.3

P5          DATA        0C8H
P50         BIT         P5.0
P51         BIT         P5.1
P52         BIT         P5.2
P53         BIT         P5.3
P54         BIT         P5.4
P55         BIT         P5.5
P56         BIT         P5.6
P57         BIT         P5.7

P5M1        DATA        0C9H
P5M0        DATA        0CAH
P6M1        DATA        0CBH
P6M0        DATA        0CCH

SPSTAT      DATA        0CDH
SPIF        BIT         SPSTAT.7
WCOL        BIT         SPSTAT.6

SPCTL       DATA        0CEH
SSIG        BIT         SPCTL.7
SPEN        BIT         SPCTL.6
DORD        BIT         SPCTL.5
MSTR        BIT         SPCTL.4
CPOL        BIT         SPCTL.3
CPHA        BIT         SPCTL.2
SPR1        BIT         SPCTL.1
SPR0        BIT         SPCTL.0

SPDAT       DATA        0CFH

PSW         DATA        0D0H
CY          BIT         PSW.7
AC          BIT         PSW.6
F0          BIT         PSW.5
RS1         BIT         PSW.4
RS0         BIT         PSW.3
OV          BIT         PSW.2
F1          BIT         PSW.1
P           BIT         PSW.0

PSW1        DATA        0D1H
N           BIT         PSW1.5
Z           BIT         PSW1.1

T4H         DATA        0D2H
T4L         DATA        0D3H
T3H         DATA        0D4H
T3L         DATA        0D5H
T2H         DATA        0D6H
T2L         DATA        0D7H

USBCLK      DATA        0DCH

T4T3M       DATA        0DDH
T4R         BIT         T4T3M.7
T4_CT       BIT         T4T3M.6
T4X12       BIT         T4T3M.5
T4CLKO      BIT         T4T3M.4
T3R         BIT         T4T3M.3
T3_CT       BIT         T4T3M.2
T3X12       BIT         T4T3M.1
T3CLKO      BIT         T4T3M.0

ADCCFG      DATA        0DEH
RESFMT      BIT         ADCCFG.5

IP3         DATA        0DFH
PI2S        BIT         IP3.3
PRTC        BIT         IP3.2
PS4         BIT         IP3.1
PS3         BIT         IP3.0

ACC         DATA        0E0H
P7M1        DATA        0E1H
P7M0        DATA        0E2H
DPS         DATA        0E3H

CMPCR1      DATA        0E6H
CMPEN       BIT         CMPCR1.7
CMPIF       BIT         CMPCR1.6
PIE         BIT         CMPCR1.5
NIE         BIT         CMPCR1.4
CMPOE       BIT         CMPCR1.1
CMPRES      BIT         CMPCR1.0

CMPCR2      DATA        0E7H
INVCMPO     BIT         CMPCR2.7
DISFLT      BIT         CMPCR2.6

P6          DATA        0E8H
P60         BIT         P6.0
P61         BIT         P6.1
P62         BIT         P6.2
P63         BIT         P6.3
P64         BIT         P6.4
P65         BIT         P6.5
P66         BIT         P6.6
P67         BIT         P6.7

WTST        DATA        0E9H
CKCON       DATA        0EAH
MXAX        DATA        0EBH
USBDAT      DATA        0ECH
DMAIR       DATA        0EDH

IP3H        DATA        0EEH
PI2SH       BIT         IP3H.3
PRTCH       BIT         IP3H.2
PS4H        BIT         IP3H.1
PS3H        BIT         IP3H.0

AUXINTIF    DATA        0EFH
INT4IF      BIT         AUXINTIF.6
INT3IF      BIT         AUXINTIF.5
INT2IF      BIT         AUXINTIF.4
T4IF        BIT         AUXINTIF.2
T3IF        BIT         AUXINTIF.1
T2IF        BIT         AUXINTIF.0

B           DATA        0F0H

USBCON      DATA        0F4H
ENUSB       BIT         USBCON.7
ENUSBRST    BIT         USBCON.6
PS2M        BIT         USBCON.5
PUEN        BIT         USBCON.4
PDEN        BIT         USBCON.3
DFREC       BIT         USBCON.2
DP          BIT         USBCON.1
DM          BIT         USBCON.0

IAP_TPS     DATA        0F5H
IAP_ADDRE   DATA        0F6H

P7          DATA        0F8H
P70         BIT         P7.0
P71         BIT         P7.1
P72         BIT         P7.2
P73         BIT         P7.3
P74         BIT         P7.4
P75         BIT         P7.5
P76         BIT         P7.6
P77         BIT         P7.7

USBADR      DATA        0FCH

S4CON       DATA        0FDH
S4SM0       BIT         S4CON.7
S4ST4       BIT         S4CON.6
S4SM2       BIT         S4CON.5
S4REN       BIT         S4CON.4
S4TB8       BIT         S4CON.3
S4RB8       BIT         S4CON.2
S4TI        BIT         S4CON.1
S4RI        BIT         S4CON.0

S4BUF       DATA        0FEH

RSTCFG      DATA        0FFH
ENLVR       BIT         RSTCFG.6
P54RST      BIT         RSTCFG.4

;
;如下特殊功能寄存器位于扩展RAM区域
;访问这些寄存器,需先将P_SW2的BIT7设置为1,才可正常读写
;

CLKSEL          EQU     07EFE00H
CLKDIV          EQU     07EFE01H
HIRCCR          EQU     07EFE02H
XOSCCR          EQU     07EFE03H
IRC32KCR        EQU     07EFE04H
MCLKOCR         EQU     07EFE05H
IRCDB           EQU     07EFE06H
IRC48MCR        EQU     07EFE07H
X32KCR          EQU     07EFE08H
HSCLKDIV        EQU     07EFE0BH

P0PU            EQU     07EFE10H
P1PU            EQU     07EFE11H
P2PU            EQU     07EFE12H
P3PU            EQU     07EFE13H
P4PU            EQU     07EFE14H
P5PU            EQU     07EFE15H
P6PU            EQU     07EFE16H
P7PU            EQU     07EFE17H
P0NCS           EQU     07EFE18H
P1NCS           EQU     07EFE19H
P2NCS           EQU     07EFE1AH
P3NCS           EQU     07EFE1BH
P4NCS           EQU     07EFE1CH
P5NCS           EQU     07EFE1DH
P6NCS           EQU     07EFE1EH
P7NCS           EQU     07EFE1FH
P0SR            EQU     07EFE20H
P1SR            EQU     07EFE21H
P2SR            EQU     07EFE22H
P3SR            EQU     07EFE23H
P4SR            EQU     07EFE24H
P5SR            EQU     07EFE25H
P6SR            EQU     07EFE26H
P7SR            EQU     07EFE27H
P0DR            EQU     07EFE28H
P1DR            EQU     07EFE29H
P2DR            EQU     07EFE2AH
P3DR            EQU     07EFE2BH
P4DR            EQU     07EFE2CH
P5DR            EQU     07EFE2DH
P6DR            EQU     07EFE2EH
P7DR            EQU     07EFE2FH
P0IE            EQU     07EFE30H
P1IE            EQU     07EFE31H
P2IE            EQU     07EFE32H
P3IE            EQU     07EFE33H
P4IE            EQU     07EFE34H
P5IE            EQU     07EFE35H
P6IE            EQU     07EFE36H
P7IE            EQU     07EFE37H
P0PD            EQU     07EFE40H
P1PD            EQU     07EFE41H
P2PD            EQU     07EFE42H
P3PD            EQU     07EFE43H
P4PD            EQU     07EFE44H
P5PD            EQU     07EFE45H
P6PD            EQU     07EFE46H
P7PD            EQU     07EFE47H
P0BP            EQU		07EFE48H
P1BP            EQU		07EFE49H
P2BP            EQU		07EFE4AH
P3BP            EQU		07EFE4BH
P4BP            EQU		07EFE4CH
P5BP            EQU		07EFE4DH
P6BP            EQU		07EFE4EH
P7BP            EQU		07EFE4FH

LCMIFCFG        EQU     07EFE50H
LCMIFCFG2       EQU     07EFE51H
LCMIFCR         EQU     07EFE52H
LCMIFSTA        EQU     07EFE53H
LCMIFDATL       EQU     07EFE54H
LCMIFDATH       EQU     07EFE55H
LCMIFPSCR       EQU     07EFE56H

RTCCR           EQU     07EFE60H
RTCCFG          EQU     07EFE61H
RTCIEN          EQU     07EFE62H
RTCIF           EQU     07EFE63H
ALAHOUR         EQU     07EFE64H
ALAMIN          EQU     07EFE65H
ALASEC          EQU     07EFE66H
ALASSEC         EQU     07EFE67H
INIYEAR         EQU     07EFE68H
INIMONTH        EQU     07EFE69H
INIDAY          EQU     07EFE6AH
INIHOUR         EQU     07EFE6BH
INIMIN          EQU     07EFE6CH
INISEC          EQU     07EFE6DH
INISSEC         EQU     07EFE6EH
INIWEEK         EQU     07EFE6FH
WEEK            EQU     07EFE6FH
YEAR            EQU     07EFE70H
MONTH           EQU     07EFE71H
DAY             EQU     07EFE72H
HOUR            EQU     07EFE73H
MIN             EQU     07EFE74H
SEC             EQU     07EFE75H
SSEC            EQU     07EFE76H

T11CR           EQU     07EFE78H
T11PS           EQU     07EFE79H
T11H            EQU     07EFE7AH
T11L            EQU     07EFE7BH

I2CCFG          EQU     07EFE80H
I2CMSCR         EQU     07EFE81H
I2CMSST         EQU     07EFE82H
I2CSLCR         EQU     07EFE83H
I2CSLST         EQU     07EFE84H
I2CSLADR        EQU     07EFE85H
I2CTXD          EQU     07EFE86H
I2CRXD          EQU     07EFE87H
I2CMSAUX        EQU     07EFE88H
I2CPSCR         EQU     07EFE89H

SPI_CLKDIV      EQU     07EFE90H
PWMA_CLKDIV     EQU     07EFE91H
PWMB_CLKDIV     EQU     07EFE92H
TFPU_CLKDIV     EQU     07EFE93H
I2S_CLKDIV      EQU     07EFE94H

RSTFLAG         EQU     07EFE99H
RSTCR0          EQU     07EFE9AH
RSTCR1          EQU     07EFE9BH
RSTCR2          EQU     07EFE9CH
RSTCR3          EQU     07EFE9DH
RSTCR4          EQU     07EFE9EH
RSTCR5          EQU     07EFE9FH

TM0PS           EQU     07EFEA0H
TM1PS           EQU     07EFEA1H
TM2PS           EQU     07EFEA2H
TM3PS           EQU     07EFEA3H
TM4PS           EQU     07EFEA4H
ADCTIM          EQU     07EFEA8H
ADCEXCFG        EQU     07EFEADH
CMPEXCFG        EQU     07EFEAEH

PWMA_ETRPS      EQU     07EFEB0H
PWMA_ENO        EQU     07EFEB1H
PWMA_PS         EQU     07EFEB2H
PWMA_IOAUX      EQU     07EFEB3H
PWMB_ETRPS      EQU     07EFEB4H
PWMB_ENO        EQU     07EFEB5H
PWMB_PS         EQU     07EFEB6H
PWMB_IOAUX      EQU     07EFEB7H
PWMA_PS2        EQU     07EFEB8H
PWMA_RCRH       EQU     07EFEB9H
PWMB_RCRH       EQU     07EFEBAH

PWMA_CR1        EQU     07EFEC0H
PWMA_CR2        EQU     07EFEC1H
PWMA_SMCR       EQU     07EFEC2H
PWMA_ETR        EQU     07EFEC3H
PWMA_IER        EQU     07EFEC4H
PWMA_SR1        EQU     07EFEC5H
PWMA_SR2        EQU     07EFEC6H
PWMA_EGR        EQU     07EFEC7H
PWMA_CCMR1      EQU     07EFEC8H
PWMA_CCMR2      EQU     07EFEC9H
PWMA_CCMR3      EQU     07EFECAH
PWMA_CCMR4      EQU     07EFECBH
PWMA_CCER1      EQU     07EFECCH
PWMA_CCER2      EQU     07EFECDH
PWMA_CNTRH      EQU     07EFECEH
PWMA_CNTRL      EQU     07EFECFH
PWMA_PSCRH      EQU     07EFED0H
PWMA_PSCRL      EQU     07EFED1H
PWMA_ARRH       EQU     07EFED2H
PWMA_ARRL       EQU     07EFED3H
PWMA_RCR        EQU     07EFED4H
PWMA_CCR1H      EQU     07EFED5H
PWMA_CCR1L      EQU     07EFED6H
PWMA_CCR2H      EQU     07EFED7H
PWMA_CCR2L      EQU     07EFED8H
PWMA_CCR3H      EQU     07EFED9H
PWMA_CCR3L      EQU     07EFEDAH
PWMA_CCR4H      EQU     07EFEDBH
PWMA_CCR4L      EQU     07EFEDCH
PWMA_BKR        EQU     07EFEDDH
PWMA_DTR        EQU     07EFEDEH
PWMA_OISR       EQU     07EFEDFH

PWMB_CR1        EQU     07EFEE0H
PWMB_CR2        EQU     07EFEE1H
PWMB_SMCR       EQU     07EFEE2H
PWMB_ETR        EQU     07EFEE3H
PWMB_IER        EQU     07EFEE4H
PWMB_SR1        EQU     07EFEE5H
PWMB_SR2        EQU     07EFEE6H
PWMB_EGR        EQU     07EFEE7H
PWMB_CCMR1      EQU     07EFEE8H
PWMB_CCMR2      EQU     07EFEE9H
PWMB_CCMR3      EQU     07EFEEAH
PWMB_CCMR4      EQU     07EFEEBH
PWMB_CCER1      EQU     07EFEECH
PWMB_CCER2      EQU     07EFEEDH
PWMB_CNTRH      EQU     07EFEEEH
PWMB_CNTRL      EQU     07EFEEFH
PWMB_PSCRH      EQU     07EFEF0H
PWMB_PSCRL      EQU     07EFEF1H
PWMB_ARRH       EQU     07EFEF2H
PWMB_ARRL       EQU     07EFEF3H
PWMB_RCR        EQU     07EFEF4H
PWMB_CCR5H      EQU     07EFEF5H
PWMB_CCR5L      EQU     07EFEF6H
PWMB_CCR6H      EQU     07EFEF7H
PWMB_CCR6L      EQU     07EFEF8H
PWMB_CCR7H      EQU     07EFEF9H
PWMB_CCR7L      EQU     07EFEFAH
PWMB_CCR8H      EQU     07EFEFBH
PWMB_CCR8L      EQU     07EFEFCH
PWMB_BKR        EQU     07EFEFDH
PWMB_DTR        EQU     07EFEFEH
PWMB_OISR       EQU     07EFEFFH

P0INTE          EQU     07EFD00H
P1INTE          EQU     07EFD01H
P2INTE          EQU     07EFD02H
P3INTE          EQU     07EFD03H
P4INTE          EQU     07EFD04H
P5INTE          EQU     07EFD05H
P6INTE          EQU     07EFD06H
P7INTE          EQU     07EFD07H
P0INTF          EQU     07EFD10H
P1INTF          EQU     07EFD11H
P2INTF          EQU     07EFD12H
P3INTF          EQU     07EFD13H
P4INTF          EQU     07EFD14H
P5INTF          EQU     07EFD15H
P6INTF          EQU     07EFD16H
P7INTF          EQU     07EFD17H
P0IM0           EQU     07EFD20H
P1IM0           EQU     07EFD21H
P2IM0           EQU     07EFD22H
P3IM0           EQU     07EFD23H
P4IM0           EQU     07EFD24H
P5IM0           EQU     07EFD25H
P6IM0           EQU     07EFD26H
P7IM0           EQU     07EFD27H
P0IM1           EQU     07EFD30H
P1IM1           EQU     07EFD31H
P2IM1           EQU     07EFD32H
P3IM1           EQU     07EFD33H
P4IM1           EQU     07EFD34H
P5IM1           EQU     07EFD35H
P6IM1           EQU     07EFD36H
P7IM1           EQU     07EFD37H
P0WKUE          EQU     07EFD40H
P1WKUE          EQU     07EFD41H
P2WKUE          EQU     07EFD42H
P3WKUE          EQU     07EFD43H
P4WKUE          EQU     07EFD44H
P5WKUE          EQU     07EFD45H
P6WKUE          EQU     07EFD46H
P7WKUE          EQU     07EFD47H

CCAPM2          EQU     07EFD50H
CCAP2L          EQU     07EFD51H
CCAP2H          EQU     07EFD52H
PCA_PWM2        EQU     07EFD53H
CCAPM3          EQU     07EFD54H
CCAP3L          EQU     07EFD55H
CCAP3H          EQU     07EFD56H
PCA_PWM3        EQU     07EFD57H
CCAPM0          EQU     07EFD58H
CCAP0L          EQU     07EFD59H
CCAP0H          EQU     07EFD5AH
PCA_PWM0        EQU     07EFD5BH
CCAPM1          EQU     07EFD5CH
CCAP1L          EQU     07EFD5DH
CCAP1H          EQU     07EFD5EH
PCA_PWM1        EQU     07EFD5FH

PINIPL          EQU     07EFD60H
PINIPH          EQU     07EFD61H

CCON            EQU     07EFD64H
CL              EQU     07EFD65H
CH              EQU     07EFD66H
CMOD            EQU     07EFD67H

UR1TOCR         EQU     07EFD70H
UR1TOSR         EQU     07EFD71H
UR1TOTH         EQU     07EFD72H
UR1TOTL         EQU     07EFD73H
UR2TOCR         EQU     07EFD74H
UR2TOSR         EQU     07EFD75H
UR2TOTH         EQU     07EFD76H
UR2TOTL         EQU     07EFD77H
UR3TOCR         EQU     07EFD78H
UR3TOSR         EQU     07EFD79H
UR3TOTH         EQU     07EFD7AH
UR3TOTL         EQU     07EFD7BH
UR4TOCR         EQU     07EFD7CH
UR4TOSR         EQU     07EFD7DH
UR4TOTH         EQU     07EFD7EH
UR4TOTL         EQU     07EFD7FH
SPITOCR         EQU     07EFD80H
SPITOSR         EQU     07EFD81H
SPITOTH         EQU     07EFD82H
SPITOTL         EQU     07EFD83H
I2CTOCR         EQU     07EFD84H
I2CTOSR         EQU     07EFD85H
I2CTOTH         EQU     07EFD86H
I2CTOTL         EQU     07EFD87H

UR1TOTE         EQU     07EFD88H
UR2TOTE         EQU     07EFD89H
UR3TOTE         EQU     07EFD8AH
UR4TOTE         EQU     07EFD8BH
SPITOTE         EQU     07EFD8CH
I2CTOTE         EQU     07EFD8DH

I2SCR           EQU     07EFD98H
I2SSR           EQU     07EFD99H
I2SDRH          EQU     07EFD9AH
I2SDRL          EQU     07EFD9BH
I2SPRH          EQU     07EFD9CH
I2SPRL          EQU     07EFD9DH
I2SCFGH         EQU     07EFD9EH
I2SCFGL         EQU     07EFD9FH
I2SMD           EQU     07EFDA0H
I2SMCKDIV       EQU     07EFDA1H

CRECR           EQU     07EFDA8H
CRECNTH         EQU     07EFDA9H
CRECNTL         EQU     07EFDAAH
CRERES          EQU     07EFDABH

S2CFG           EQU     07EFDB4H
S2ADDR          EQU     07EFDB5H
S2ADEN          EQU     07EFDB6H
USARTCR1        EQU     07EFDC0H
USARTCR2        EQU     07EFDC1H
USARTCR3        EQU     07EFDC2H
USARTCR4        EQU     07EFDC3H
USARTCR5        EQU     07EFDC4H
USARTGTR        EQU     07EFDC5H
USARTBRH        EQU     07EFDC6H
USARTBRL        EQU     07EFDC7H
USART2CR1       EQU     07EFDC8H
USART2CR2       EQU     07EFDC9H
USART2CR3       EQU     07EFDCAH
USART2CR4       EQU     07EFDCBH
USART2CR5       EQU     07EFDCCH
USART2GTR       EQU     07EFDCDH
USART2BRH       EQU     07EFDCEH
USART2BRL       EQU     07EFDCFH

CHIPID          EQU     07EFDE0H

CHIPID0         EQU     07EFDE0H
CHIPID1         EQU     07EFDE1H
CHIPID2         EQU     07EFDE2H
CHIPID3         EQU     07EFDE3H
CHIPID4         EQU     07EFDE4H
CHIPID5         EQU     07EFDE5H
CHIPID6         EQU     07EFDE6H
CHIPID7         EQU     07EFDE7H
CHIPID8         EQU     07EFDE8H
CHIPID9         EQU     07EFDE9H
CHIPID10        EQU     07EFDEAH
CHIPID11        EQU     07EFDEBH
CHIPID12        EQU     07EFDECH
CHIPID13        EQU     07EFDEDH
CHIPID14        EQU     07EFDEEH
CHIPID15        EQU     07EFDEFH
CHIPID16        EQU     07EFDF0H
CHIPID17        EQU     07EFDF1H
CHIPID18        EQU     07EFDF2H
CHIPID19        EQU     07EFDF3H
CHIPID20        EQU     07EFDF4H
CHIPID21        EQU     07EFDF5H
CHIPID22        EQU     07EFDF6H
CHIPID23        EQU     07EFDF7H
CHIPID24        EQU     07EFDF8H
CHIPID25        EQU     07EFDF9H
CHIPID26        EQU     07EFDFAH
CHIPID27        EQU     07EFDFBH
CHIPID28        EQU     07EFDFCH
CHIPID29        EQU     07EFDFDH
CHIPID30        EQU     07EFDFEH
CHIPID31        EQU     07EFDFFH

CHIPIDX         EQU     07EFBD0H

CHIPIDX0        EQU     07EFBD0H
CHIPIDX1        EQU     07EFBD1H
CHIPIDX2        EQU     07EFBD2H
CHIPIDX3        EQU     07EFBD3H
CHIPIDX4        EQU     07EFBD4H
CHIPIDX5        EQU     07EFBD5H
CHIPIDX6        EQU     07EFBD6H
CHIPIDX7        EQU     07EFBD7H
CHIPIDX8        EQU     07EFBD8H
CHIPIDX9        EQU     07EFBD9H
CHIPIDX10       EQU     07EFBDAH
CHIPIDX11       EQU     07EFBDBH
CHIPIDX12       EQU     07EFBDCH
CHIPIDX13       EQU     07EFBDDH
CHIPIDX14       EQU     07EFBDEH
CHIPIDX15       EQU     07EFBDFH
CHIPIDX16       EQU     07EFBE0H
CHIPIDX17       EQU     07EFBE1H
CHIPIDX18       EQU     07EFBE2H
CHIPIDX19       EQU     07EFBE3H
CHIPIDX20       EQU     07EFBE4H
CHIPIDX21       EQU     07EFBE5H
CHIPIDX22       EQU     07EFBE6H
CHIPIDX23       EQU     07EFBE7H
CHIPIDX24       EQU     07EFBE8H
CHIPIDX25       EQU     07EFBE9H
CHIPIDX26       EQU     07EFBEAH
CHIPIDX27       EQU     07EFBEBH
CHIPIDX28       EQU     07EFBECH
CHIPIDX29       EQU     07EFBEDH
CHIPIDX30       EQU     07EFBEEH
CHIPIDX31       EQU     07EFBEFH

HSPWMA_CFG      EQU     07EFBF0H
HSPWMA_ADR      EQU     07EFBF1H
HSPWMA_DAT      EQU     07EFBF2H
HSPWMA_ADRH     EQU     07EFBF3H
HSPWMB_CFG      EQU     07EFBF4H
HSPWMB_ADR      EQU     07EFBF5H
HSPWMB_DAT      EQU     07EFBF6H
HSPWMB_ADRH     EQU     07EFBF7H
HSSPI_CFG       EQU     07EFBF8H
HSSPI_CFG2      EQU     07EFBF9H
HSSPI_STA       EQU     07EFBFAH
HSSPI_PSCR      EQU     07EFBFBH

DMA_M2M_CFG     EQU     07EFA00H
DMA_M2M_CR      EQU     07EFA01H
DMA_M2M_STA     EQU     07EFA02H
DMA_M2M_AMT     EQU     07EFA03H
DMA_M2M_DONE    EQU     07EFA04H
DMA_M2M_TXAH    EQU     07EFA05H
DMA_M2M_TXAL    EQU     07EFA06H
DMA_M2M_RXAH    EQU     07EFA07H
DMA_M2M_RXAL    EQU     07EFA08H

DMA_ADC_CFG     EQU     07EFA10H
DMA_ADC_CR      EQU     07EFA11H
DMA_ADC_STA     EQU     07EFA12H
DMA_ADC_AMT     EQU     07EFA13H
DMA_ADC_DONE    EQU     07EFA14H
DMA_ADC_RXAH    EQU     07EFA17H
DMA_ADC_RXAL    EQU     07EFA18H
DMA_ADC_CFG2    EQU     07EFA19H
DMA_ADC_CHSW0   EQU     07EFA1AH
DMA_ADC_CHSW1   EQU     07EFA1BH
DMA_ADC_ITVH    EQU     07EFA1EH
DMA_ADC_ITVL    EQU     07EFA1FH

DMA_SPI_CFG     EQU     07EFA20H
DMA_SPI_CR      EQU     07EFA21H
DMA_SPI_STA     EQU     07EFA22H
DMA_SPI_AMT     EQU     07EFA23H
DMA_SPI_DONE    EQU     07EFA24H
DMA_SPI_TXAH    EQU     07EFA25H
DMA_SPI_TXAL    EQU     07EFA26H
DMA_SPI_RXAH    EQU     07EFA27H
DMA_SPI_RXAL    EQU     07EFA28H
DMA_SPI_CFG2    EQU     07EFA29H
DMA_SPI_ITVH    EQU     07EFA2EH
DMA_SPI_ITVL    EQU     07EFA2FH

DMA_UR1T_CFG    EQU     07EFA30H
DMA_UR1T_CR     EQU     07EFA31H
DMA_UR1T_STA    EQU     07EFA32H
DMA_UR1T_AMT    EQU     07EFA33H
DMA_UR1T_DONE   EQU     07EFA34H
DMA_UR1T_TXAH   EQU     07EFA35H
DMA_UR1T_TXAL   EQU     07EFA36H
DMA_UR1R_CFG    EQU     07EFA38H
DMA_UR1R_CR     EQU     07EFA39H
DMA_UR1R_STA    EQU     07EFA3AH
DMA_UR1R_AMT    EQU     07EFA3BH
DMA_UR1R_DONE   EQU     07EFA3CH
DMA_UR1R_RXAH   EQU     07EFA3DH
DMA_UR1R_RXAL   EQU     07EFA3EH

DMA_UR2T_CFG    EQU     07EFA40H
DMA_UR2T_CR     EQU     07EFA41H
DMA_UR2T_STA    EQU     07EFA42H
DMA_UR2T_AMT    EQU     07EFA43H
DMA_UR2T_DONE   EQU     07EFA44H
DMA_UR2T_TXAH   EQU     07EFA45H
DMA_UR2T_TXAL   EQU     07EFA46H
DMA_UR2R_CFG    EQU     07EFA48H
DMA_UR2R_CR     EQU     07EFA49H
DMA_UR2R_STA    EQU     07EFA4AH
DMA_UR2R_AMT    EQU     07EFA4BH
DMA_UR2R_DONE   EQU     07EFA4CH
DMA_UR2R_RXAH   EQU     07EFA4DH
DMA_UR2R_RXAL   EQU     07EFA4EH

DMA_UR3T_CFG    EQU     07EFA50H
DMA_UR3T_CR     EQU     07EFA51H
DMA_UR3T_STA    EQU     07EFA52H
DMA_UR3T_AMT    EQU     07EFA53H
DMA_UR3T_DONE   EQU     07EFA54H
DMA_UR3T_TXAH   EQU     07EFA55H
DMA_UR3T_TXAL   EQU     07EFA56H
DMA_UR3R_CFG    EQU     07EFA58H
DMA_UR3R_CR     EQU     07EFA59H
DMA_UR3R_STA    EQU     07EFA5AH
DMA_UR3R_AMT    EQU     07EFA5BH
DMA_UR3R_DONE   EQU     07EFA5CH
DMA_UR3R_RXAH   EQU     07EFA5DH
DMA_UR3R_RXAL   EQU     07EFA5EH

DMA_UR4T_CFG    EQU     07EFA60H
DMA_UR4T_CR     EQU     07EFA61H
DMA_UR4T_STA    EQU     07EFA62H
DMA_UR4T_AMT    EQU     07EFA63H
DMA_UR4T_DONE   EQU     07EFA64H
DMA_UR4T_TXAH   EQU     07EFA65H
DMA_UR4T_TXAL   EQU     07EFA66H
DMA_UR4R_CFG    EQU     07EFA68H
DMA_UR4R_CR     EQU     07EFA69H
DMA_UR4R_STA    EQU     07EFA6AH
DMA_UR4R_AMT    EQU     07EFA6BH
DMA_UR4R_DONE   EQU     07EFA6CH
DMA_UR4R_RXAH   EQU     07EFA6DH
DMA_UR4R_RXAL   EQU     07EFA6EH

DMA_LCM_CFG     EQU     07EFA70H
DMA_LCM_CR      EQU     07EFA71H
DMA_LCM_STA     EQU     07EFA72H
DMA_LCM_AMT     EQU     07EFA73H
DMA_LCM_DONE    EQU     07EFA74H
DMA_LCM_TXAH    EQU     07EFA75H
DMA_LCM_TXAL    EQU     07EFA76H
DMA_LCM_RXAH    EQU     07EFA77H
DMA_LCM_RXAL    EQU     07EFA78H
DMA_LCM_ITVH    EQU     07EFA7EH
DMA_LCM_ITVL    EQU     07EFA7FH

DMA_M2M_AMTH    EQU     07EFA80H
DMA_M2M_DONEH   EQU     07EFA81H
DMA_ADC_AMTH    EQU     07EFA82H
DMA_ADC_DONEH   EQU     07EFA83H
DMA_SPI_AMTH    EQU     07EFA84H
DMA_SPI_DONEH   EQU     07EFA85H
DMA_LCM_AMTH    EQU     07EFA86H
DMA_LCM_DONEH   EQU     07EFA87H
DMA_UR1T_AMTH   EQU     07EFA88H
DMA_UR1T_DONEH  EQU     07EFA89H
DMA_UR1R_AMTH   EQU     07EFA8AH
DMA_UR1R_DONEH  EQU     07EFA8BH
DMA_UR2T_AMTH   EQU     07EFA8CH
DMA_UR2T_DONEH  EQU     07EFA8DH
DMA_UR2R_AMTH   EQU     07EFA8EH
DMA_UR2R_DONEH  EQU     07EFA8FH
DMA_UR3T_AMTH   EQU     07EFA90H
DMA_UR3T_DONEH  EQU     07EFA91H
DMA_UR3R_AMTH   EQU     07EFA92H
DMA_UR3R_DONEH  EQU     07EFA93H
DMA_UR4T_AMTH   EQU     07EFA94H
DMA_UR4T_DONEH  EQU     07EFA95H
DMA_UR4R_AMTH   EQU     07EFA96H
DMA_UR4R_DONEH  EQU     07EFA97H

DMA_I2CT_CFG    EQU     07EFA98H
DMA_I2CT_CR     EQU     07EFA99H
DMA_I2CT_STA    EQU     07EFA9AH
DMA_I2CT_AMT    EQU     07EFA9BH
DMA_I2CT_DONE   EQU     07EFA9CH
DMA_I2CT_TXAH   EQU     07EFA9DH
DMA_I2CT_TXAL   EQU     07EFA9EH
DMA_I2CR_CFG    EQU     07EFAA0H
DMA_I2CR_CR     EQU     07EFAA1H
DMA_I2CR_STA    EQU     07EFAA2H
DMA_I2CR_AMT    EQU     07EFAA3H
DMA_I2CR_DONE   EQU     07EFAA4H
DMA_I2CR_RXAH   EQU     07EFAA5H
DMA_I2CR_RXAL   EQU     07EFAA6H

DMA_I2CT_AMTH   EQU     07EFAA8H
DMA_I2CT_DONEH  EQU     07EFAA9H
DMA_I2CR_AMTH   EQU     07EFAAAH
DMA_I2CR_DONEH  EQU     07EFAABH
DMA_I2C_CR      EQU     07EFAADH
DMA_I2C_ST1     EQU     07EFAAEH
DMA_I2C_ST2     EQU     07EFAAFH

DMA_I2ST_CFG    EQU     07EFAB0H
DMA_I2ST_CR     EQU     07EFAB1H
DMA_I2ST_STA    EQU     07EFAB2H
DMA_I2ST_AMT    EQU     07EFAB3H
DMA_I2ST_DONE   EQU     07EFAB4H
DMA_I2ST_TXAH   EQU     07EFAB5H
DMA_I2ST_TXAL   EQU     07EFAB6H
DMA_I2SR_CFG    EQU     07EFAB8H
DMA_I2SR_CR     EQU     07EFAB9H
DMA_I2SR_STA    EQU     07EFABAH
DMA_I2SR_AMT    EQU     07EFABBH
DMA_I2SR_DONE   EQU     07EFABCH
DMA_I2SR_RXAH   EQU     07EFABDH
DMA_I2SR_RXAL   EQU     07EFABEH

DMA_I2ST_AMTH   EQU     07EFAC0H
DMA_I2ST_DONEH  EQU     07EFAC1H
DMA_I2SR_AMTH   EQU     07EFAC2H
DMA_I2SR_DONEH  EQU     07EFAC3H
DMA_I2C_ITVH    EQU     07EFAC4H
DMA_I2C_ITVL    EQU     07EFAC5H
DMA_I2S_ITVH    EQU     07EFAC6H
DMA_I2S_ITVL    EQU     07EFAC7H
DMA_UR1_ITVH    EQU     07EFAC8H
DMA_UR1_ITVL    EQU     07EFAC9H
DMA_UR2_ITVH    EQU     07EFACAH
DMA_UR2_ITVL    EQU     07EFACBH
DMA_UR3_ITVH    EQU     07EFACCH
DMA_UR3_ITVL    EQU     07EFACDH
DMA_UR4_ITVH    EQU     07EFACEH
DMA_UR4_ITVL    EQU     07EFACFH

DMA_QSPI_CFG    EQU     07EFAD0H
DMA_QSPI_CR     EQU     07EFAD1H
DMA_QSPI_STA    EQU     07EFAD2H
DMA_QSPI_AMT    EQU     07EFAD3H
DMA_QSPI_DONE   EQU     07EFAD4H
DMA_QSPI_TXAH   EQU     07EFAD5H
DMA_QSPI_TXAL   EQU     07EFAD6H
DMA_QSPI_RXAH   EQU     07EFAD7H
DMA_QSPI_RXAL   EQU     07EFAD8H
DMA_QSPI_AMTH   EQU     07EFADBH
DMA_QSPI_DONEH  EQU     07EFADCH
DMA_QSPI_ITVH   EQU     07EFADEH
DMA_QSPI_ITVL   EQU     07EFADFH

DMA_P2P_CR1     EQU     07EFAF0H
DMA_P2P_CR2     EQU     07EFAF1H
DMA_ARB_CFG     EQU     07EFAF8H
DMA_ARB_STA     EQU     07EFAF9H

QSPI_CR1        EQU     07EF900H
QSPI_CR2        EQU     07EF901H
QSPI_CR3        EQU     07EF902H
QSPI_CR4        EQU     07EF903H
QSPI_DCR1       EQU     07EF904H
QSPI_DCR2       EQU     07EF905H
QSPI_SR1        EQU     07EF906H
QSPI_SR2        EQU     07EF907H
QSPI_FCR        EQU     07EF908H
QSPI_HCR1       EQU     07EF909H
QSPI_HCR2       EQU     07EF90AH
QSPI_DLR1       EQU     07EF910H
QSPI_DLR2       EQU     07EF911H
QSPI_CCR1       EQU     07EF914H
QSPI_CCR2       EQU     07EF915H
QSPI_CCR3       EQU     07EF916H
QSPI_CCR4       EQU     07EF917H
QSPI_AR1        EQU     07EF918H
QSPI_AR2        EQU     07EF919H
QSPI_AR3        EQU     07EF91AH
QSPI_AR4        EQU     07EF91BH
QSPI_ABR        EQU     07EF91CH
QSPI_DR         EQU     07EF920H
QSPI_PSMKR1     EQU     07EF924H
QSPI_PSMAR1     EQU     07EF928H
QSPI_PIR1       EQU     07EF92CH
QSPI_PIR2       EQU     07EF92DH

PWMA_ENO2       EQU     07EF930H
PWMA_IOAUX2     EQU     07EF931H
PWMA_CR3        EQU     07EF932H
PWMA_SR3        EQU     07EF933H
PWMA_CCER3      EQU     07EF934H
PWMA_CCMR1X     EQU     07EF938H
PWMA_CCMR2X     EQU     07EF939H
PWMA_CCMR3X     EQU     07EF93AH
PWMA_CCMR4X     EQU     07EF93BH
PWMA_CCMR5      EQU     07EF93CH
PWMA_CCMR5X     EQU     07EF93DH
PWMA_CCMR6      EQU     07EF93EH
PWMA_CCMR6X     EQU     07EF93FH
PWMA_CCR5H      EQU     07EF940H
PWMA_CCR5L      EQU     07EF941H
PWMA_CCR5X      EQU     07EF942H
PWMA_CCR6H      EQU     07EF943H
PWMA_CCR6L      EQU     07EF944H
PWMA_DER        EQU     07EF948H
PWMA_DBA        EQU     07EF949H
PWMA_DBL        EQU     07EF94AH
PWMA_DMACR      EQU     07EF94BH

DMA_PWMAT_CFG   EQU     07EF980H
DMA_PWMAT_CR    EQU     07EF981H
DMA_PWMAT_STA   EQU     07EF982H
DMA_PWMAT_AMTH  EQU     07EF984H
DMA_PWMAT_AMT   EQU     07EF985H
DMA_PWMAT_DONEH EQU     07EF986H
DMA_PWMAT_DONE  EQU     07EF987H
DMA_PWMAT_TXAH  EQU     07EF988H
DMA_PWMAT_TXAL  EQU     07EF989H
DMA_PWMA_ITVH   EQU     07EF98EH
DMA_PWMA_ITVL   EQU     07EF98FH

DMA_PWMAR_CFG   EQU     07EF990H
DMA_PWMAR_CR    EQU     07EF991H
DMA_PWMAR_STA   EQU     07EF992H
DMA_PWMAR_AMTH  EQU     07EF994H
DMA_PWMAR_AMT   EQU     07EF995H
DMA_PWMAR_DONEH EQU     07EF996H
DMA_PWMAR_DONE  EQU     07EF997H
DMA_PWMAR_RXAH  EQU     07EF998H
DMA_PWMAR_RXAL  EQU     07EF999H

/////////////////////////////////////////////////

;USB Control Regiter

FADDR           EQU     0
POWER           EQU     1
INTRIN1         EQU     2
INTROUT1        EQU     4
INTRUSB         EQU     6
INTRIN1E        EQU     7
INTROUT1E       EQU     9
INTRUSBE        EQU     11
FRAME1          EQU     12
FRAME2          EQU     13
INDEX           EQU     14
INMAXP          EQU     16
CSR0            EQU     17
INCSR1          EQU     17
INCSR2          EQU     18
OUTMAXP         EQU     19
OUTCSR1         EQU     20
OUTCSR2         EQU     21
COUNT0          EQU     22
OUTCOUNT1       EQU     22
OUTCOUNT2       EQU     23
FIFO0           EQU     32
FIFO1           EQU     33
FIFO2           EQU     34
FIFO3           EQU     35
FIFO4           EQU     36
FIFO5           EQU     37
UTRKCTL         EQU     48
UTRKSTS         EQU     49

/////////////////////////////////////////////////

$RESTORE

