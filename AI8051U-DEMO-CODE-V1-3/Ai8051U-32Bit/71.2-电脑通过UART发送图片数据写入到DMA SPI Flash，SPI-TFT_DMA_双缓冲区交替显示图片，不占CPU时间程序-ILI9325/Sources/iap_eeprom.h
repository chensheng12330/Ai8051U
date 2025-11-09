#ifndef	__IAP_EEPROM_H
#define	__IAP_EEPROM_H

//#define EE_ADDRESS  0x00FC00  //保存的EEPROM起始地址, 8051U第一版的EEPROM大小固定为64K大小，后续版本已修复
#define EE_ADDRESS  0x000000  //保存的EEPROM起始地址, 8051U第一版的EEPROM大小固定为64K大小，后续版本已修复

void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_SectorErase(u32 EE_address);

#endif
