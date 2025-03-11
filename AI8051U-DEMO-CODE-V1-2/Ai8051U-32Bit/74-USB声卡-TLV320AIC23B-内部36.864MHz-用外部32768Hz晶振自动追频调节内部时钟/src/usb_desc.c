#include "stc.h"
#include "usb_desc.h"

char code DEVICEDESC[18] =
{
    0x12,                   //bLength(18);
    0x01,                   //bDescriptorType(Device);
    0x00,0x02,              //bcdUSB(2.00);
    0x00,                   //bDeviceClass(0);
    0x00,                   //bDeviceSubClass0);
    0x00,                   //bDeviceProtocol(0);
    0x40,                   //bMaxPacketSize0(64);
    0xbf,0x34,              //idVendor(34bf);
    0x08,0xff,              //idProduct(ff08);
    0x01,0x01,              //bcdDevice(1.01);
    0x01,                   //iManufacturer(1);
    0x02,                   //iProduct(2);
    0x00,                   //iSerialNumber(0);
    0x01,                   //bNumConfigurations(1);
};

char code CONFIGDESC[110] =
{
    0x09,                   //bLength(9);
    0x02,                   //bDescriptorType(Configuration);
    0x6e,0x00,              //wTotalLength(110);
    0x02,                   //bNumInterfaces(2);
    0x01,                   //bConfigurationValue(1);
    0x00,                   //iConfiguration(0);
    0x80,                   //bmAttributes(BUSPower);
    0x32,                   //MaxPower(100mA);

    0x09,                   //bLength(9);
    0x04,                   //bDescriptorType(Interface);
    0x00,                   //bInterfaceNumber(0);
    0x00,                   //bAlternateSetting(0);
    0x00,                   //bNumEndpoints(0);
    0x01,                   //bInterfaceClass(AUDIO);
    0x01,                   //bInterfaceSubClass(AUDIOCONTROL);
    0x00,                   //bInterfaceProtocol(PR_PROTOCOL_UNDEFINED);
    0x00,                   //iInterface(0);

    0x09,                   //bLength(9)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x01,                   //bDescriptorSubtype(HEADER)
    0x00,0x01,              //bcdADC(1.0)
    0x28,0x00,              //wTotalLength(40)
    0x01,                   //bInCollection(1 AudioStreaming interface)
    0x01,                   //baInterfaceNr(AS #1, Interface 1 is stream)

    0x0c,                   //bLength(12)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x02,                   //bDescriptorSubtype(INPUT_TERMINAL)
    0x01,                   //bTerminalID(1)
    0x01,0x01,              //wTerminalType(USB Streaming)
    0x00,                   //bAssocTerminal(0)
    0x02,                   //bNrChannels(2)
    0x03,0x00,              //wChannelConfig(Left Front,Right Front)
    0x00,                   //iChannelNames(0)
    0x00,                   //iTerminal(0)

    0x0a,                   //bLength(10)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x06,                   //bDescriptorSubtype(FEATURE_UNIT)
    0x02,                   //bUnitID(2)
    0x01,                   //bSourceID(#1, USB Streaming IT)
    0x01,                   //bControlSize(1)
    0x01,                   //bmaMasterControls(Mute)
    0x00,                   //bmaChannelControls(Volume)
    0x00,                   //bmaChannelControls(Volume)
    0x00,                   //iFeature(0)

    0x09,                   //bLength(9)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x03,                   //bDescriptorSubtype(OUTPUT_TERMINAL)
    0x03,                   //bTerminalID(3)
    0x01,0x03,              //wTerminalType(USB Speaker)
    0x00,                   //bAssocTerminal(0)
    0x02,                   //bSourceID(#2, Feature UNIT)
    0x00,                   //iTerminal(0)

    0x09,                   //bLength(9)
    0x04,                   //bDescriptorType(Interface)
    0x01,                   //bInterfaceNumber(1)
    0x00,                   //bAlternateSetting 0)
    0x00,                   //bNumEndpoints(0)
    0x01,                   //bInterfaceClass(AUDIO)
    0x02,                   //bInterfaceSubClass(AUDIOSTREAMING)
    0x00,                   //bInterfaceProtocol(PR_PROTOCOL_UNDEFINED)
    0x00,                   //iInterface(0)

    0x09,                   //bLength(9)
    0x04,                   //bDescriptorType(Interface)
    0x01,                   //bInterfaceNumber(1)
    0x01,                   //bAlternateSetting(1)
    0x01,                   //bNumEndpoints(1)
    0x01,                   //bInterfaceClass(AUDIO)
    0x02,                   //bInterfaceSubClass(AUDIOSTREAMING)
    0x00,                   //bInterfaceProtocol(PR_PROTOCOL_UNDEFINED)
    0x00,                   //iInterface(0)

    0x07,                   //bLength(7)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x01,                   //bDescriptorSubtype(AS_GENERAL)
    0x01,                   //bTerminalLink(USB Streaming IT)
    0x01,                   //bDelay(1)
    0x01,0x00,              //wFormatTag(PCM format)

    0x0b,                   //bLength(11)
    0x24,                   //bDescriptorType(CS_INTERFACE)
    0x02,                   //bDescriptorSubtype(FORMAT_TYPE)
    0x01,                   //bFormatType(TYPE_I)
    0x02,                   //bNrChannels(2)
    0x02,                   //bSubFrameSize(2)
    0x10,                   //bBitResolution(16)
    0x01,                   //bSamFreqType(One sampling frequency)
    (SAMPFREQ),             //tSamFreq(48K)
    (SAMPFREQ >> 8),
    (SAMPFREQ >> 16),

    0x09,                   //bLength(9)
    0x05,                   //bDescriptorType(Endpoint)
    0x05,                   //bEndpointAddress(EndPoint5 as OUT)
    0x05,                   //bmAttributes(Asynchronous)
    BLOCKSIZE,0x00,         //wMaxPacketSize(192)
    0x01,                   //bInterval(1 millisecond)
    0x00,                   //bRefresh(0)
    0x00,                   //bSynchAddress(no synchronization)

    0x07,                   //bLength(7)
    0x25,                   //bDescriptorType(CS_ENDPOINT)
    0x01,                   //bDescriptorSubtype(EP_GENERAL)
    0x00,                   //bmAttributes(0)
    0x00,                   //bLockDelayUnits(0)
    0x00,0x00,              //wLockDelay(0)
};

char code LANGIDDESC[4] =
{
    0x04,0x03,
    0x09,0x04,
};

char code MANUFACTDESC[8] =
{
    0x08,0x03,
    'S',0,
    'T',0,
    'C',0,
};

char code PRODUCTDESC[28] =
{
    0x1c,0x03,
    'S',0,
    'T',0,
    'C',0,
    ' ',0,
    'U',0,
    'S',0,
    'B',0,
    ' ',0,
    'A',0,
    'u',0,
    'd',0,
    'i',0,
    'o',0,
};

char code PACKET0[2] = 
{
    0, 0,
};

char code PACKET1[2] = 
{
    1, 0,
};
