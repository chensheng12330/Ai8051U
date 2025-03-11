#include "config.h"
#include "usbcdc.h"
#include "w25qxx.h"

char *USER_DEVICEDESC = NULL;
char *USER_PRODUCTDESC = NULL;
char *USER_STCISPCMD = "@STCISP#";

BYTE xdata pCdcBuffer[256];         //CDC串口数据接收缓冲区
BYTE xdata pFlashBuffer[256];       //Flash数据缓冲区
BYTE bReadPtr;                      //缓冲区读数据指针
BYTE bWritePtr;                     //缓冲区写数据指针

void CDC_Init()
{
    usb_init();
    CDC_Flush();
}

void CDC_Flush()
{
    bReadPtr = 0;
    bWritePtr = 0;
}

void CDC_WaitStable()
{
    while (DeviceState != DEVSTATE_CONFIGURED);     //等待USB完成配置
}

int printf(const char *fmt, ...)
{
    va_list va;
    int ret;

    va_start(va, fmt);
    ret = vsprintf(&UsbInBuffer[0], fmt, va);
    va_end(va);

    usb_IN(ret);

    return ret;
}

char _getkey()
{
    while (bReadPtr == bWritePtr);
    
    return pCdcBuffer[bReadPtr++];
}

void CDC_Process()
{
#define CMD_READ_ID             0xf0
#define CMD_READ_STATUS         0xf1
#define CMD_READ_DATA           0xf2
#define CMD_PROGRAM_DATA        0xf3
#define CMD_ERASE_CHIP          0xf4
#define CMD_ERASE_4K            0xf5
#define CMD_ERASE_32K           0xf6
#define CMD_ERASE_64K           0xf7

#define STG_IDLE                0x00
#define STG_CMD_HEAD1           0x01
#define STG_CMD_HEAD2           0x02
#define STG_CMD_HEAD3           0x03
#define STG_ERASE_ADDR0         0x10
#define STG_ERASE_ADDR1         0x11
#define STG_ERASE_ADDR2         0x12
#define STG_ERASE_ADDR3         0x13
#define STG_ERASE_DONE          0x14
#define STG_READ_ADDR0          0x20
#define STG_READ_ADDR1          0x21
#define STG_READ_ADDR2          0x22
#define STG_READ_ADDR3          0x23
#define STG_READ_LEN0           0x24
#define STG_READ_LEN1           0x25
#define STG_READ_LEN2           0x26
#define STG_READ_LEN3           0x27
#define STG_READ_DONE           0x28
#define STG_PROGRAM_ADDR0       0x30
#define STG_PROGRAM_ADDR1       0x31
#define STG_PROGRAM_ADDR2       0x32
#define STG_PROGRAM_ADDR3       0x33
#define STG_PROGRAM_LEN0        0x34
#define STG_PROGRAM_LEN1        0x35
#define STG_PROGRAM_LEN2        0x36
#define STG_PROGRAM_LEN3        0x37
#define STG_PROGRAM_DATA        0x38


    static BYTE stage;
    static BYTE cmd;
    static DWORD addr;
    static DWORD len;
    static WORD index;
    BYTE dat;
    BYTE cnt;
    BYTE i;
    
    if (bUsbOutReady)
    {
        for (i = 0; i < OutNumber; i++)
            pCdcBuffer[bWritePtr++] = UsbOutBuffer[i];
        
        usb_OUT_done();
    }

    while (bReadPtr != bWritePtr)
    {
        dat = _getkey();
        switch (stage)
        {
        default:
        case STG_IDLE:
L_CheckHead:
            if (dat == 'A')
                stage = STG_CMD_HEAD1;
            else
                stage = STG_IDLE;
            break;
        case STG_CMD_HEAD1:
            if (dat == 'I')
                stage = STG_CMD_HEAD2;
            else
                goto L_CheckHead;
            break;
        case STG_CMD_HEAD2:
            if (dat == 'C')
                stage = STG_CMD_HEAD3;
            else
                goto L_CheckHead;
            break;
        case STG_CMD_HEAD3:
            addr = 0;
            len = 0;
            index = 0;
            switch (cmd = dat)
            {
            case CMD_READ_ID:
                USB_SendData("AIS", 3);
                *(DWORD *)pFlashBuffer = W25Q_ReadJEDECID_9F();
                USB_SendData(&pFlashBuffer[1], 3);
                stage = STG_IDLE;
                break;
            case CMD_READ_STATUS:
                USB_SendData("AIS", 3);
                pFlashBuffer[0] = W25Q_ReadSR1_05();
                pFlashBuffer[1] = W25Q_ReadSR2_35();
                pFlashBuffer[2] = W25Q_ReadSR3_15();
                USB_SendData(pFlashBuffer, 3);
                stage = STG_IDLE;
                break;
            case CMD_READ_DATA:
                stage = STG_READ_ADDR0;
                break;
            case CMD_PROGRAM_DATA:
                stage = STG_PROGRAM_ADDR0;
                break;
            case CMD_ERASE_CHIP:
                W25Q_EraseChip_C7(FALSE);
                USB_SendData("AIS", 3);
                stage = STG_IDLE;
                break;
            case CMD_ERASE_4K:
            case CMD_ERASE_32K:
            case CMD_ERASE_64K:
                stage = STG_ERASE_ADDR0;
                break;
            default:
                goto L_CheckHead;
            }
            break;
        case STG_READ_ADDR0:
        case STG_READ_ADDR1:
        case STG_READ_ADDR2:
        case STG_READ_ADDR3:
        case STG_PROGRAM_ADDR0:
        case STG_PROGRAM_ADDR1:
        case STG_PROGRAM_ADDR2:
        case STG_PROGRAM_ADDR3:
        case STG_ERASE_ADDR0:
        case STG_ERASE_ADDR1:
        case STG_ERASE_ADDR2:
        case STG_ERASE_ADDR3:
            addr <<= 8;
            addr |= dat;
            stage++;
            if (stage == STG_ERASE_DONE)
            {
                switch (cmd)
                {
                case CMD_ERASE_4K:
                    W25Q_Erase4K_20(addr, FALSE);
                    USB_SendData("AIS", 3);
                    stage = STG_IDLE;
                    break;
                case CMD_ERASE_32K:
                    W25Q_Erase32K_52(addr, FALSE);
                    USB_SendData("AIS", 3);
                    stage = STG_IDLE;
                    break;
                case CMD_ERASE_64K:
                    W25Q_Erase64K_D8(addr, FALSE);
                    USB_SendData("AIS", 3);
                    stage = STG_IDLE;
                    break;
                default:
                    goto L_CheckHead;
                }
            }
            break;
        case STG_READ_LEN0:
        case STG_READ_LEN1:
        case STG_READ_LEN2:
        case STG_READ_LEN3:
        case STG_PROGRAM_LEN0:
        case STG_PROGRAM_LEN1:
        case STG_PROGRAM_LEN2:
        case STG_PROGRAM_LEN3:
            len <<= 8;
            len |= dat;
            stage++;
            if (stage == STG_READ_DONE)
            {
                USB_SendData("AIS", 3);
                while (len)
                {
                    cnt = min(len, 64);
                    W25Q_FastRead_6B(addr, pFlashBuffer, cnt);
                    USB_SendData(pFlashBuffer, cnt);
                    
                    addr += cnt;
                    len -= cnt;
                }
                stage = STG_IDLE;
            }
            break;
        case STG_PROGRAM_DATA:
            pFlashBuffer[index++] = dat;
            if (index >= len)
            {
                W25Q_PageProgram_02(addr, pFlashBuffer, (WORD)len);
                
                USB_SendData("AIS", 3);
                stage = STG_IDLE;
            }
            break;
        }
    }
}
