#ifndef	__IAP_EEPROM_H
#define	__IAP_EEPROM_H

//#define EE_ADDRESS  0x00FC00  //�����EEPROM��ʼ��ַ, 8051U��һ���EEPROM��С�̶�Ϊ64K��С�������汾���޸�
#define EE_ADDRESS  0x000000  //�����EEPROM��ʼ��ַ, 8051U��һ���EEPROM��С�̶�Ϊ64K��С�������汾���޸�

void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_write_n(u32 EE_address,u8 *DataAddress,u16 number);
void EEPROM_SectorErase(u32 EE_address);

#endif
