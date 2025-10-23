/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef	__AI8051U_SOFT_I2C_H
#define	__AI8051U_SOFT_I2C_H

#include "config.h"

#define SLAW    0x5A
#define SLAR    0x5B

void SI2C_WriteNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number);
void SI2C_ReadNbyte(u8 dev_addr, u8 mem_addr, u8 *p, u8 number);

#endif

