#ifndef __SYS_H
#define __SYS_H	
#include <intrins.h>
#include "..\..\comm\AI8051U.h"

#define	u8 unsigned char
#define	u16 unsigned int
#define	u32 unsigned long

#define     MAIN_Fosc       24000000L   //定义主时钟

void delay_ms(int count);
void delay_us(int count);

	  		 
#endif  
	 
	 



