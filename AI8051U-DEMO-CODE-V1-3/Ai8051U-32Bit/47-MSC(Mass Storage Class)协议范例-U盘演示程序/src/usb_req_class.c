/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "usb.h"
#include "usb_req_class.h"
#include "util.h"
#include "usb_desc.h"

void usb_req_class()
{
    switch (Setup.bRequest)
    {
    case DEVICERESET:
        usb_device_reset();
        break;
    case GETMAXLUN:
        usb_get_maxlun();
        break;
    default:
        usb_setup_stall();
        return;
    }
}

void usb_device_reset()
{
    usb_setup_status();
}

void usb_get_maxlun()
{
    Ep0State.pData = PACKET0;
    Ep0State.wSize = Setup.wLength;

    usb_setup_in();
}
