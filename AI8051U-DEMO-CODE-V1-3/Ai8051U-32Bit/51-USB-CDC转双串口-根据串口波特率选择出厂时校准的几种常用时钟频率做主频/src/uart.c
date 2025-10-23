/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "uart.h"
#include "usb.h"
#include "usb_req_class.h"
#include "util.h"

BOOL Uart1TxBusy;
BOOL Uart2TxBusy;

BYTE Uart1RxRptr;
BYTE Uart2RxRptr;
BYTE Uart1RxWptr;
BYTE Uart2RxWptr;

BYTE Uart1TxRptr;
BYTE Uart2TxRptr;
BYTE Uart1TxWptr;
BYTE Uart2TxWptr;

BYTE xdata Uart1RxBuffer[256];
BYTE xdata Uart2RxBuffer[256];
BYTE xdata Uart1TxBuffer[256];
BYTE xdata Uart2TxBuffer[256];

void uart_init()
{
    SCON = 0x50;
    TMOD &= ~0xf0;
    TL1 = BR(MAIN_Fosc,115200);
    TH1 = BR(MAIN_Fosc,115200) >> 8;
    T1x12 = 1;
    TR1 = 1;
    PS = 1;     //提高串口1中断优先级，防止数据丢失
    ES = 1;
    
    S2CON = 0x50;
    T2L = BR(MAIN_Fosc,115200);
    T2H = BR(MAIN_Fosc,115200) >> 8;
    T2x12 = 1;
    T2R = 1;
    PS2 = 1;    //提高串口2中断优先级，防止数据丢失
    ES2 = 1;

    LineCoding1.dwDTERate = REV4(115200);
    LineCoding1.bCharFormat = 0;
    LineCoding1.bParityType = 0;
    LineCoding1.bDataBits = 8;

    LineCoding2.dwDTERate = REV4(115200);
    LineCoding2.bCharFormat = 0;
    LineCoding2.bParityType = 0;
    LineCoding2.bDataBits = 8;
    
    Uart1TxBusy = 0;
    Uart2TxBusy = 0;
    Uart1RxRptr = 0;
    Uart2RxRptr = 0;
    Uart1RxWptr = 0;
    Uart2RxWptr = 0;
    Uart1TxRptr = 0;
    Uart2TxRptr = 0;
    Uart1TxWptr = 0;
    Uart2TxWptr = 0;
}

void Uart1_isr() interrupt UART1_VECTOR
{
    if (TI)
    {
        TI = 0;
        Uart1TxBusy = 0;
    }

    if (RI)
    {
        RI = 0;
        Uart1TxBuffer[Uart1TxWptr++] = SBUF;
    }
}

void uart2_isr() interrupt UART2_VECTOR
{
    if (S2TI)
    {
        S2TI = 0;
        Uart2TxBusy = 0;
    }

    if (S2RI)
    {
        S2RI = 0;
        Uart2TxBuffer[Uart2TxWptr++] = S2BUF;
    }
}

void uart_set_parity()
{
    if (Interface == 0)
    {
        switch (LineCoding1.bParityType)
        {
        default:
        case NONE_PARITY:
            SCON = 0x50;
            break;
        case ODD_PARITY:
        case EVEN_PARITY:
        case MARK_PARITY:
            SCON = 0xd8;
            break;
        case SPACE_PARITY:
            SCON = 0xd0;
            break;
        }
    }
    else if (Interface == 2)
    {
        switch (LineCoding2.bParityType)
        {
        default:
        case NONE_PARITY:
            S2CON = 0x50;
            break;
        case ODD_PARITY:
        case EVEN_PARITY:
        case MARK_PARITY:
            S2CON = 0xd8;
            break;
        case SPACE_PARITY:
            S2CON = 0xd0;
            break;
        }
    }
}

#ifdef Dynamic_Frequency        //开启波特率动态调整主频控制开关
//========================================================================
// 函数: void SetMCLK(u8 clk)
// 描述: 设置主频
// 参数: clk: 主频序号
// 返回: none.
// 版本: VER1.0
// 日期: 2022-10-14
// 备注: 
//========================================================================
void SetMCLK(BYTE clk) //设置主频
{
	switch(clk)
	{
	case 1:
        if(MAIN_Fosc == 24000000L) break;
		//选择24MHz
		CLKDIV = 0x04;
		IRTRIM = T24M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	24000000L;	//定义主时钟
		break;

	case 2:
        if(MAIN_Fosc == 30000000L) break;
		//选择30MHz
		CLKDIV = 0x04;
		IRTRIM = T30M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	30000000L;	//定义主时钟
		break;

	case 3:
        if(MAIN_Fosc == 40000000L) break;
		//选择40MHz
		CLKDIV = 0x04;
		IRTRIM = T40M_ADDR;
		VRTRIM = VRT44M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_44M;
		CLKDIV = 1;
		MAIN_Fosc =	40000000L;	//定义主时钟
		break;

	default:
        if(MAIN_Fosc == 22118400L) break;
		//选择22.1184MHz
		CLKDIV = 0x04;
		IRTRIM = T22M_ADDR;
		VRTRIM = VRT27M_ADDR;
		IRCBAND = (IRCBAND & ~0x03) | IRCBAND_27M;
		CLKDIV = 1;
		MAIN_Fosc =	22118400L;	//定义主时钟
		break;
	}
}
#endif

void uart_set_baud()
{
    DWORD baud,baud2;
    WORD temp,temp2;
    
    if (Interface == 0)
    {
        baud = LineCoding1.dwDTERate;
        baud2 = LineCoding2.dwDTERate;
    }
    else if (Interface == 2)
    {
        baud = LineCoding2.dwDTERate;
        baud2 = LineCoding1.dwDTERate;
    }
    
    baud = reverse4(baud);

#ifdef Dynamic_Frequency        //开启波特率动态调整主频控制开关
    baud2 = reverse4(baud2);

    switch (baud)
    {
        case 14400:
        case 19200:
        case 28800:
        case 38400:
        case 57600:
        case 115200:
        case 230400:
        case 460800:
        case 921600:    //可被 22.1184M 整除
            SetMCLK(0); //主时钟 22.1184MHz
            break;
        case 1500000:   //可被 30M 整除
            SetMCLK(2); //主时钟 30MHz
            break;
        default:
            SetMCLK(1); //主时钟 24MHz
            break;
    }
#endif
    temp = (WORD)BR(MAIN_Fosc,baud);
    temp2 = (WORD)BR(MAIN_Fosc,baud2);
    
    if (Interface == 0)
    {
        TL1 = temp;
        TH1 = temp >> 8;
        #ifdef Dynamic_Frequency        //开启波特率动态调整主频控制开关
        T2L = temp2;
        T2H = temp2 >> 8;
        #endif
    }
    else if (Interface == 2)
    {
        T2L = temp;
        T2H = temp >> 8;
        #ifdef Dynamic_Frequency        //开启波特率动态调整主频控制开关
        TL1 = temp2;
        TH1 = temp2 >> 8;
        #endif
    }
}

void uart_polling()
{
    BYTE dat;
    BYTE cnt;

    if (DeviceState != DEVSTATE_CONFIGURED)
        return;

    if (!Ep4InBusy && (Uart1TxRptr != Uart1TxWptr))
    {
        EUSB = 0;
        Ep4InBusy = 1;
        usb_write_reg(INDEX, 4);
        cnt = 0;
        while (Uart1TxRptr != Uart1TxWptr)
        {
            usb_write_reg(FIFO4, Uart1TxBuffer[Uart1TxRptr++]);
            cnt++;
            if (cnt == EP4IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!Ep5InBusy && (Uart2TxRptr != Uart2TxWptr))
    {
        EUSB = 0;
        Ep5InBusy = 1;
        usb_write_reg(INDEX, 5);
        cnt = 0;
        while (Uart2TxRptr != Uart2TxWptr)
        {
            usb_write_reg(FIFO5, Uart2TxBuffer[Uart2TxRptr++]);
            cnt++;
            if (cnt == EP5IN_SIZE) break;
        }
        usb_write_reg(INCSR1, INIPRDY);
        EUSB = 1;
    }

    if (!Uart1TxBusy && (Uart1RxRptr != Uart1RxWptr))
    {
        dat = Uart1RxBuffer[Uart1RxRptr++];
        Uart1TxBusy = 1;
        switch (LineCoding1.bParityType)
        {
        case NONE_PARITY:
        case SPACE_PARITY:
            TB8 = 0;
            break;
        case ODD_PARITY:
            ACC = dat;
            TB8 = !P;
            break;
        case EVEN_PARITY:
            ACC = dat;
            TB8 = P;
            break;
        case MARK_PARITY:
            TB8 = 1;
            break;
        }
        SBUF = dat;
//        while (Uart1TxBusy);
    }

    if (!Uart2TxBusy && (Uart2RxRptr != Uart2RxWptr))
    {
        dat = Uart2RxBuffer[Uart2RxRptr++];
        Uart2TxBusy = 1;
        switch (LineCoding2.bParityType)
        {
        case NONE_PARITY:
        case SPACE_PARITY:
            S2TB8 = 0;
            break;
        case ODD_PARITY:
            ACC = dat;
            S2TB8 = !P;
            break;
        case EVEN_PARITY:
            ACC = dat;
            S2TB8 = P;
            break;
        case MARK_PARITY:
            S2TB8 = 1;
            break;
        }
        S2BUF = dat;

//        while (Uart2Busy);
    }

    if (Ep4OutBusy)
    {
        EUSB = 0;
        if ((BYTE)(Uart1RxWptr - Uart1RxRptr) < (BYTE)(256 - EP4OUT_SIZE))
        {
            Ep4OutBusy = 0;
            usb_write_reg(INDEX, 4);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }

    if (Ep5OutBusy)
    {
        EUSB = 0;
        if ((BYTE)(Uart2RxWptr - Uart2RxRptr) < (BYTE)(256 - EP5OUT_SIZE))
        {
            Ep5OutBusy = 0;
            usb_write_reg(INDEX, 5);
            usb_write_reg(OUTCSR1, 0);
        }
        EUSB = 1;
    }
}
