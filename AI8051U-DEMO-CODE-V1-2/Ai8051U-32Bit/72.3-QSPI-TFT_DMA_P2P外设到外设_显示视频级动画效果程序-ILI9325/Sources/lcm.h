#ifndef __LCM_H__
#define __LCM_H__

#define DAT             P2

sbit    LCM_CS      =   P0^5;
sbit    LCM_RS      =   P4^5;
sbit    LCM_WR      =   P3^6;
sbit    LCM_RD      =   P3^7;
sbit    LCM_RST     =   P4^7;

void LCM_Init();
void LCM_WriteCmd_CS(BYTE cmd);
void LCM_WriteData_CS(BYTE dat);    
void LCM_WriteData(BYTE dat);    

#endif
