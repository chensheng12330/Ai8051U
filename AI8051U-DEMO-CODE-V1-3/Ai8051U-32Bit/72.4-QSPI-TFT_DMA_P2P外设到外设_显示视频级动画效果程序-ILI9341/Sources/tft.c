#include "config.h"
#include "tft.h"
#include "lcm.h"


void TFT_Reset()
{
    LCM_CS = 1;
    delay_ms(50);    
    LCM_RST = 0;
    delay_ms(150);
    LCM_RST = 1;
    delay_ms(50);
}

void TFT_Init()
{
    TFT_Reset();            //初始化之前复位

//*************2.4inch ILI9341初始化**********//
    LCM_WriteCmd_CS(0xCF);  
    LCM_WriteData_CS(0x00); 
    LCM_WriteData_CS(0xD9); //0xC1 
    LCM_WriteData_CS(0X30); 
    LCM_WriteCmd_CS(0xED);  
    LCM_WriteData_CS(0x64); 
    LCM_WriteData_CS(0x03); 
    LCM_WriteData_CS(0X12); 
    LCM_WriteData_CS(0X81); 
    LCM_WriteCmd_CS(0xE8);  
    LCM_WriteData_CS(0x85); 
    LCM_WriteData_CS(0x10); 
    LCM_WriteData_CS(0x7A); 
    LCM_WriteCmd_CS(0xCB);  
    LCM_WriteData_CS(0x39); 
    LCM_WriteData_CS(0x2C); 
    LCM_WriteData_CS(0x00); 
    LCM_WriteData_CS(0x34); 
    LCM_WriteData_CS(0x02); 
    LCM_WriteCmd_CS(0xF7);  
    LCM_WriteData_CS(0x20); 
    LCM_WriteCmd_CS(0xEA);  
    LCM_WriteData_CS(0x00); 
    LCM_WriteData_CS(0x00); 
    LCM_WriteCmd_CS(0xC0);    //Power control 
    LCM_WriteData_CS(0x1B);   //VRH[5:0] 
    LCM_WriteCmd_CS(0xC1);    //Power control 
    LCM_WriteData_CS(0x12);   //SAP[2:0];BT[3:0] 0x01
    LCM_WriteCmd_CS(0xC5);    //VCM control 
    LCM_WriteData_CS(0x08);      //30
    LCM_WriteData_CS(0x26);      //30
    LCM_WriteCmd_CS(0xC7);    //VCM control2 
    LCM_WriteData_CS(0XB7); 
    LCM_WriteCmd_CS(0x36);    // Memory Access Control 
    LCM_WriteData_CS(0x08);
    LCM_WriteCmd_CS(0x3A);   
    LCM_WriteData_CS(0x55); 
    LCM_WriteCmd_CS(0xB1);   
    LCM_WriteData_CS(0x00);   
    LCM_WriteData_CS(0x1A); 
    LCM_WriteCmd_CS(0xB6);    // Display Function Control 
    LCM_WriteData_CS(0x0A); 
    LCM_WriteData_CS(0xA2); 
    LCM_WriteCmd_CS(0xF2);    // 3Gamma Function Disable 
    LCM_WriteData_CS(0x00); 
    LCM_WriteCmd_CS(0x26);    //Gamma curve selected 
    LCM_WriteData_CS(0x01); 
    LCM_WriteCmd_CS(0xE0);    //Set Gamma 
    LCM_WriteData_CS(0x0F); 
    LCM_WriteData_CS(0x1D); 
    LCM_WriteData_CS(0x1A); 
    LCM_WriteData_CS(0x0A); 
    LCM_WriteData_CS(0x0D); 
    LCM_WriteData_CS(0x07); 
    LCM_WriteData_CS(0x49); 
    LCM_WriteData_CS(0X66); 
    LCM_WriteData_CS(0x3B); 
    LCM_WriteData_CS(0x07); 
    LCM_WriteData_CS(0x11); 
    LCM_WriteData_CS(0x01); 
    LCM_WriteData_CS(0x09); 
    LCM_WriteData_CS(0x05); 
    LCM_WriteData_CS(0x04);          
    LCM_WriteCmd_CS(0XE1);    //Set Gamma 
    LCM_WriteData_CS(0x00); 
    LCM_WriteData_CS(0x18); 
    LCM_WriteData_CS(0x1D); 
    LCM_WriteData_CS(0x02); 
    LCM_WriteData_CS(0x0F); 
    LCM_WriteData_CS(0x04); 
    LCM_WriteData_CS(0x36); 
    LCM_WriteData_CS(0x13); 
    LCM_WriteData_CS(0x4C); 
    LCM_WriteData_CS(0x07); 
    LCM_WriteData_CS(0x13); 
    LCM_WriteData_CS(0x0F); 
    LCM_WriteData_CS(0x2E); 
    LCM_WriteData_CS(0x2F); 
    LCM_WriteData_CS(0x05); 
    LCM_WriteCmd_CS(0x2B); 
    LCM_WriteData_CS(0x00);
    LCM_WriteData_CS(0x00);
    LCM_WriteData_CS(0x01);
    LCM_WriteData_CS(0x3f);
    LCM_WriteCmd_CS(0x2A); 
    LCM_WriteData_CS(0x00);
    LCM_WriteData_CS(0x00);
    LCM_WriteData_CS(0x00);
    LCM_WriteData_CS(0xef);     
    LCM_WriteCmd_CS(0x11); //Exit Sleep
    delay_ms(120);
    LCM_WriteCmd_CS(0x29); //display on    

    //设置LCD显示方向
    LCM_WriteCmd_CS(0x36);
    LCM_WriteData_CS((1<<3)|(1<<5)|(1<<6));
}

void TFT_ShowStart()
{
    LCM_WriteCmd_CS(0x2A);
    LCM_WriteData_CS(0);
    LCM_WriteData_CS(0);
    LCM_WriteData_CS((BYTE)(TFT_W>>8));
    LCM_WriteData_CS(0x00FF&TFT_W);

    LCM_WriteCmd_CS(0x2B);
    LCM_WriteData_CS(0);
    LCM_WriteData_CS(0);
    LCM_WriteData_CS((BYTE)(TFT_H>>8));
    LCM_WriteData_CS(0x00FF&TFT_H);    

    LCM_WriteCmd_CS(0x2C);

    LCM_CS = 0;
}

void TFT_ShowData(BYTE dat)
{
    LCM_WriteData(dat);
}

void TFT_ShowDataW(WORD dat)
{
    LCM_WriteData((BYTE)(dat >> 8));
    LCM_WriteData((BYTE)dat);
}

void TFT_ShowEnd()
{
    LCM_CS = 1;
}

