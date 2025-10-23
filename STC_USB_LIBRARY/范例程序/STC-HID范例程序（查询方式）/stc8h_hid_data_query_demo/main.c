#include "stc8h.h"
#include "string.h"

#include "stc32_stc8_usb.h"

void sys_init();
void DelayXms(int n);

void main()
{
    sys_init();
    usb_init();
    EA = 1;

    while (1)
    {
        if (bUsbOutReady)
        {
						USB_SendData(UsbOutBuffer, 64);      //ԭ·����, ���ڲ���HID
            usb_OUT_done();
        }
    }
}

void sys_init()
{
    P_SW2 = 0x80;
}
