#ifndef __CONFIG_H__
#define __CONFIG_H__

#define FOSC              40000000UL

#include <ai8051u.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <intrins.h>

typedef bit BOOL;
typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;

#define min(a, b)       (a) < (b) ? (a) : (b)
#define max(a, b)       (a) > (b) ? (a) : (b)

#define FALSE           0
#define TRUE            1

void delay_ms(WORD ms);


#endif
