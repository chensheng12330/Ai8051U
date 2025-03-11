#ifndef __USB_REQ_CLASS_H__
#define __USB_REQ_CLASS_H__

#define GET_CUR                 0x81
#define SET_CUR                 0x01

void usb_req_class();

void usb_get_cur();
void usb_set_cur();

extern BYTE Mute;

#endif
