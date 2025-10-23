
#ifndef	__EEPROM_H
#define	__EEPROM_H

#include	"inc\stc.h"


void	DisableEEPROM(void);
void	EEPROM_SectorErase(u16 EE_address);
void	EEPROM_read_n( u16 EE_address, u8 *DataAddress, u8 length);
u8		EEPROM_write_n(u16 EE_address, u8 *DataAddress, u8 length);


#endif