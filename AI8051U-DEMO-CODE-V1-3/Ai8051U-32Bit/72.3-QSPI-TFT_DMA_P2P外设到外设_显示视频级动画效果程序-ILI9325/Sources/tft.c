#include "config.h"
#include "tft.h"
#include "lcm.h"


void TFT_WriteReg(BYTE reg, WORD dat)     
{
    LCM_WriteCmd_CS(reg);
    LCM_WriteData_CS((BYTE)(dat >> 8));
    LCM_WriteData_CS((BYTE)dat);
}

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

//*************2.4inch ILI9325初始化**********//    
    TFT_WriteReg(0xE5, 0x78F0); 
    TFT_WriteReg(0x01, 0x0100); 
    TFT_WriteReg(0x02, 0x0700); 
    TFT_WriteReg(0x03, 0x1030); 
    TFT_WriteReg(0x04, 0x0000); 
    TFT_WriteReg(0x08, 0x0202);  
    TFT_WriteReg(0x09, 0x0000);
    TFT_WriteReg(0x0A, 0x0000); 
    TFT_WriteReg(0x0C, 0x0000); 
    TFT_WriteReg(0x0D, 0x0000);
    TFT_WriteReg(0x0F, 0x0000);
    //power on sequence VGHVGL
    TFT_WriteReg(0x10, 0x0000);   
    TFT_WriteReg(0x11, 0x0007);  
    TFT_WriteReg(0x12, 0x0000);  
    TFT_WriteReg(0x13, 0x0000); 
    TFT_WriteReg(0x07, 0x0000); 
    //vgh  
    TFT_WriteReg(0x10, 0x1690);   
    TFT_WriteReg(0x11, 0x0227);
    delay_ms(10); 
    //vregiout  
    TFT_WriteReg(0x12, 0x009D); //0x001b
    delay_ms(10);  
    //vom amplitude 
    TFT_WriteReg(0x13, 0x1900);
    delay_ms(10);  
    //vom H 
    TFT_WriteReg(0x29, 0x0025); 
    TFT_WriteReg(0x2B, 0x000D); 
    //gamma 
    TFT_WriteReg(0x30, 0x0007);
    TFT_WriteReg(0x31, 0x0303);
    TFT_WriteReg(0x32, 0x0003); //0006
    TFT_WriteReg(0x35, 0x0206);
    TFT_WriteReg(0x36, 0x0008);
    TFT_WriteReg(0x37, 0x0406); 
    TFT_WriteReg(0x38, 0x0304); //0200
    TFT_WriteReg(0x39, 0x0007); 
    TFT_WriteReg(0x3C, 0x0602); //0504
    TFT_WriteReg(0x3D, 0x0008); 
    //ram 
    TFT_WriteReg(0x50, 0x0000); 
    TFT_WriteReg(0x51, 0x00EF);
    TFT_WriteReg(0x52, 0x0000); 
    TFT_WriteReg(0x53, 0x013F);  
    TFT_WriteReg(0x60, 0xA700); 
    TFT_WriteReg(0x61, 0x0001); 
    TFT_WriteReg(0x6A, 0x0000); 
    // 
    TFT_WriteReg(0x80, 0x0000); 
    TFT_WriteReg(0x81, 0x0000); 
    TFT_WriteReg(0x82, 0x0000); 
    TFT_WriteReg(0x83, 0x0000); 
    TFT_WriteReg(0x84, 0x0000); 
    TFT_WriteReg(0x85, 0x0000); 
    // 
    TFT_WriteReg(0x90, 0x0010); 
    TFT_WriteReg(0x92, 0x0600); 
     
    TFT_WriteReg(0x07, 0x0133);
    TFT_WriteReg(0x00, 0x0022);

    //设置LCD显示方向
#ifdef HORIZONTAL
    TFT_WriteReg(0x03, 0x1038);
    TFT_WriteReg(0x01, 0x0000);
#else
    TFT_WriteReg(0x03, 0x1030);
    TFT_WriteReg(0x01, 0x0100);
#endif
}

void TFT_ShowStart()
{    
    TFT_WriteReg(0x50, 0);
    TFT_WriteReg(0x51, 240);
    TFT_WriteReg(0x52, 0);
    TFT_WriteReg(0x53, 320);
    TFT_WriteReg(0x20, 0);
    TFT_WriteReg(0x21, 0);
    
    LCM_WriteCmd_CS(0x22);
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

