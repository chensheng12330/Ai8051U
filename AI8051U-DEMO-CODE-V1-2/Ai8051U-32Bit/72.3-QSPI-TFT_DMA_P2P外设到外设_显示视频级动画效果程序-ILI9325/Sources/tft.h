#ifndef __TFT_H__
#define __TFT_H__

#define HORIZONTAL

#define TFT_W               240
#define TFT_H               320

void TFT_WriteReg(BYTE reg, WORD dat);
void TFT_Reset();
void TFT_Init();
void TFT_ShowStart();
void TFT_ShowData(BYTE dat);
void TFT_ShowDataW(WORD dat);
void TFT_ShowEnd();

#endif
