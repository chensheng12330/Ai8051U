/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "AI8051U_GPIO.h"

u8 ioIndex;

//========================================================================
// 函数: P0_ISR_Handler
// 描述: P0中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P0_ISR_Handler (void) interrupt P0INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P0INTF;     //获取中断标志
    if (B) 
    { 
        P0INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x00;
        if (B1) ioIndex = 0x01;
        if (B2) ioIndex = 0x02;
        if (B3) ioIndex = 0x03;
        if (B4) ioIndex = 0x04;
        if (B5) ioIndex = 0x05;
        if (B6) ioIndex = 0x06;
        if (B7) ioIndex = 0x07;
    }
}

//========================================================================
// 函数: P1_ISR_Handler
// 描述: P1中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P1_ISR_Handler (void) interrupt P1INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P1INTF;     //获取中断标志
    if(B)
    {
        P1INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x10;
        if (B1) ioIndex = 0x11;
        if (B2) ioIndex = 0x12;
        if (B3) ioIndex = 0x13;
        if (B4) ioIndex = 0x14;
        if (B5) ioIndex = 0x15;
        if (B6) ioIndex = 0x16;
        if (B7) ioIndex = 0x17;
    }
}

//========================================================================
// 函数: P2_ISR_Handler
// 描述: P2中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P2_ISR_Handler (void) interrupt P2INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P2INTF;     //获取中断标志
    if(B)
    {
        P2INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x20;
        if (B1) ioIndex = 0x21;
        if (B2) ioIndex = 0x22;
        if (B3) ioIndex = 0x23;
        if (B4) ioIndex = 0x24;
        if (B5) ioIndex = 0x25;
        if (B6) ioIndex = 0x26;
        if (B7) ioIndex = 0x27;
    }
}

//========================================================================
// 函数: P3_ISR_Handler
// 描述: P3中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P3_ISR_Handler (void) interrupt P3INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P3INTF;     //获取中断标志
    if(B)
    {
        P3INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x30;
        if (B1) ioIndex = 0x31;
        if (B2) ioIndex = 0x32;
        if (B3) ioIndex = 0x33;
        if (B4) ioIndex = 0x34;
        if (B5) ioIndex = 0x35;
        if (B6) ioIndex = 0x36;
        if (B7) ioIndex = 0x37;
    }
}

//========================================================================
// 函数: P4_ISR_Handler
// 描述: P4中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P4_ISR_Handler (void) interrupt P4INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P4INTF;     //获取中断标志
    if(B)
    {
        P4INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x40;
        if (B1) ioIndex = 0x41;
        if (B2) ioIndex = 0x42;
        if (B3) ioIndex = 0x43;
        if (B4) ioIndex = 0x44;
        if (B5) ioIndex = 0x45;
        if (B6) ioIndex = 0x46;
        if (B7) ioIndex = 0x47;
    }
}

//========================================================================
// 函数: P5_ISR_Handler
// 描述: P5中断函数.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2025-02-10
//========================================================================
void P5_ISR_Handler (void) interrupt P5INT_VECTOR
{
	// TODO: 在此处添加用户代码
    B = P5INTF;     //获取中断标志
    if(B)
    {
        P5INTF = 0x00;      //清中断标志
        if (B0) ioIndex = 0x50;
        if (B1) ioIndex = 0x51;
        if (B2) ioIndex = 0x52;
        if (B3) ioIndex = 0x53;
        if (B4) ioIndex = 0x54;
        if (B5) ioIndex = 0x55;
        if (B6) ioIndex = 0x56;
        if (B7) ioIndex = 0x57;
    }
}
