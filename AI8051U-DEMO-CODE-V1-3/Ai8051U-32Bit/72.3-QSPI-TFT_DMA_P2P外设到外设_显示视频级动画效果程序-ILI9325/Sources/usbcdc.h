#ifndef __USBCDC_H__
#define __USBCDC_H__

#define DEVSTATE_ATTACHED       0
#define DEVSTATE_POWERED        1
#define DEVSTATE_DEFAULT        2
#define DEVSTATE_ADDRESS        3
#define DEVSTATE_CONFIGURED     4
#define DEVSTATE_SUSPENDED      5

int printf(const char *fmt, ...);

void usb_init();
void usb_IN(WORD size); 
void usb_OUT_done();
void USB_SendData(BYTE *dat, int size);

void CDC_Init();
void CDC_Flush();
void CDC_WaitStable();
void CDC_Process();

extern BYTE xdata UsbInBuffer[64];
extern BYTE xdata UsbOutBuffer[64];
extern BOOL bUsbOutReady;
extern BYTE DeviceState;
extern BYTE OutNumber;

#endif
