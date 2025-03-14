/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "stc.h"
#include "usb.h"
#include "usb_req_class.h"
#include "util.h"
#include "uart.h"

LINECODING LineCoding1;
LINECODING LineCoding2;

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case SET_LINE_CODING:
        usb_set_line_coding();
        break;
    case GET_LINE_CODING:
        usb_get_line_coding();
        break;
    case SET_CONTROL_LINE_STATE:
        usb_set_ctrl_line_state();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_set_line_coding()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Interface = Setup.wIndexL;
    if (Interface == 0)
    {
        Ep0State.pData = (BYTE *)&LineCoding1;
    }
    else if (Interface == 2)
    {
        Ep0State.pData = (BYTE *)&LineCoding2;
    }
    else
    {
        usb_setup_stall();
        return;
    }
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}

void usb_get_line_coding()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Interface = Setup.wIndexL;
    if (Interface == 0)
    {
        Ep0State.pData = (BYTE *)&LineCoding1;
    }
    else if (Interface == 2)
    {
        Ep0State.pData = (BYTE *)&LineCoding2;
    }
    else
    {
        usb_setup_stall();
        return;
    }
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_ctrl_line_state()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    usb_setup_status();
}

void usb_uart_settings()
{
    if (Interface == 0)
    {
        LineCoding1.bCharFormat = 0;
        LineCoding1.bDataBits = 8;
    }
    else if (Interface == 2)
    {
        LineCoding2.bCharFormat = 0;
        LineCoding2.bDataBits = 8;
    }

    uart_set_parity();
    uart_set_baud();
}
