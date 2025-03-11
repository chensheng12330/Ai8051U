#include "stc.h"
#include "usb.h"
#include "usb_desc.h"
#include "usb_req_class.h"
#include "util.h"
#include "i2s.h"

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case SET_CUR:
        usb_set_cur();
        break;
    case GET_CUR:
        usb_get_cur();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_get_cur()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (IN_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = &WaveMute;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}

void usb_set_cur()
{
    if ((DeviceState != DEVSTATE_CONFIGURED) ||
        (Setup.bmRequestType != (OUT_DIRECT | CLASS_REQUEST | INTERFACE_RECIPIENT)))
    {
        usb_setup_stall();
        return;
    }

    Ep0State.pData = &WaveMute;
    Ep0State.wSize = Setup.wLength;

    usb_setup_out();
}
