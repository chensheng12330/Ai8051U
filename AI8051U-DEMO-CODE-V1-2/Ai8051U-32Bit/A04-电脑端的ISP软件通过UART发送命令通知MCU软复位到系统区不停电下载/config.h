#ifndef __STC_H__
#define __STC_H__

#include "../comm/AI8051U.h"
#include <intrins.h>
#include <stdio.h>

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned int ushort;
typedef unsigned long ulong;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

#define MAIN_Fosc       24000000UL
#define BAUD            (65536 - MAIN_Fosc/4/115200)

#endif
