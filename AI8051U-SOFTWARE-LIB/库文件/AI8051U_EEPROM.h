/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __AI8051U_EEPROM_H
#define __AI8051U_EEPROM_H

#include "config.h"

//========================================================================
//                              ��������
//========================================================================


//========================================================================
//                               IAP����
//========================================================================

#define IAP_STANDBY()   IAP_CMD = 0        //IAP���������ֹ��
#define IAP_READ()      IAP_CMD = 1        //IAP��������
#define IAP_WRITE()     IAP_CMD = 2        //IAPд������
#define IAP_ERASE()     IAP_CMD = 3        //IAP��������

#define IAP_ENABLE()    IAPEN = 1
#define IAP_DISABLE()   IAP_CONTR = 0; IAP_CMD = 0; IAP_TRIG = 0; IAP_ADDRH = 0xff; IAP_ADDRL = 0xff


void DisableEEPROM(void);
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_SectorErase(u32 EE_address);


#endif