
/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/



//	本程序是STC8051U系列的内置EEPROM读写程序。

#include "eeprom.h"


//========================================================================
// 函数: void	ISP_Disable(void)
// 描述: 禁止访问ISP/IAP.
// 参数: non.
// 返回: non.
// 版本: V1.0, 2012-10-22
//========================================================================
void	DisableEEPROM(void)	//禁止访问EEPROM
{
	IAP_TPS   = 0;
	IAP_CONTR = 0;			//禁止ISP/IAP操作
	IAP_CMD   = 0;			//去除ISP/IAP命令
	IAP_TRIG  = 0;			//防止ISP/IAP命令误触发
	IAP_ADDRE = 0xff;       //将地址设置到非 IAP 区域
	IAP_ADDRH = 0xff;		//清0地址高字节
	IAP_ADDRL = 0xff;		//清0地址低字节，指向非EEPROM区，防止误操作
}

//========================================================================
// 函数: void EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2024-6-30
//========================================================================
void EEPROM_Trig(void)
{
//	EA = 0;				//禁止中断. 如果只有主循环或中断中操作操作EEPROM，则不需要关中断，如果两者都操作EEPROM，则主循环中操作EEPROM必须关中断以免竞争。
	IAP_TRIG = 0x5A;	//先送5AH，再送A5H到IAP触发寄存器，每次都需要如此
	IAP_TRIG = 0xA5;	//送完A5H后，IAP命令立即被触发启动, CPU等待IAP完成后，才会继续执行程序。
	_nop_();			//由于STC8051是多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
	_nop_();
	_nop_();
	_nop_();
//	EA = 1;		//重新允许中断
}

//========================================================================
// 函数: void EEPROM_SectorErase(u32 EE_address)
// 描述: 擦除一个扇区.
// 参数: EE_address:  要擦除的EEPROM的扇区中的一个字节地址.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_SectorErase(u16 EE_address)
{											//只有扇区擦除，没有字节擦除，512字节/扇区。 扇区中任意一个字节地址都是扇区地址。
	IAP_CONTR = IAP_EN;						//允许ISP/IAP操作
	IAP_TPS = (u8)(MAIN_Fosc / 1000000UL);	//工作频率设置
    IAP_ERASE();                        	//宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
    IAP_ADDRE = 0; 							//送扇区地址高字节（地址需要改变时才需重新送地址）
    IAP_ADDRH = (u8)(EE_address >> 8);		//送扇区地址中字节（地址需要改变时才需重新送地址）
    IAP_ADDRL = (u8)EE_address;				//送扇区地址低字节（地址需要改变时才需重新送地址）
    EEPROM_Trig();							//触发EEPROM操作
    DisableEEPROM();						//禁止EEPROM操作
}

//========================================================================
// 函数: void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 lenth)
// 描述: 读N个字节函数.
// 参数: EE_address:  要读出的EEPROM的首地址.
//       DataAddress: 要读出数据的指针.
//       length:      要读出的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_read_n(u16 EE_address, u8 *DataAddress, u8 length)
{
	IAP_CONTR = IAP_EN;						//允许ISP/IAP操作
	IAP_TPS = (u8)(MAIN_Fosc / 1000000UL);	//根据工作频率设置设置等待时间
    IAP_READ();                             //送字节读命令，命令不需改变时，不需重新送命令
    do
    {
        IAP_ADDRE = 0; 						//送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        EEPROM_Trig();                      //触发EEPROM操作
        *DataAddress = IAP_DATA;            //读出的数据送往
        EE_address++;
        DataAddress++;
    }while(--length);
	DisableEEPROM();		//禁止EEPWOM操作
}


//========================================================================
// 函数: u8 EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length)
// 描述: 写N个字节函数.
// 参数: EE_address:  要写入的EEPROM的首地址.
//       DataAddress: 要写入数据的指针.
//       length:      要写入的长度
// 返回: 0: 写入正确.  1: 写入长度为0错误.  2: 写入数据错误.
// 版本: V1.0, 2014-6-30
//========================================================================
u8 EEPROM_write_n(u16 EE_address, u8 *DataAddress, u8 length)
{
    u8  i;
    u16 j;
    u8  *p;

    if(length == 0) return 1;   //长度为0错误

	IAP_CONTR = IAP_EN;						//允许ISP/IAP操作
	IAP_TPS = (u8)(MAIN_Fosc / 1000000UL);	//根据工作频率设置设置等待时间
    i = length;
    j = EE_address;
    p = DataAddress;
    IAP_WRITE();                            //宏调用, 送字节写命令
    do
    {
        IAP_ADDRE = 0; 						//送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        IAP_DATA  = *DataAddress;           //送数据到IAP_DATA，只有数据改变时才需重新送
        EEPROM_Trig();                      //触发EEPROM操作
        EE_address++;                       //下一个地址
        DataAddress++;                      //下一个数据
    }while(--length);                       //直到结束

    EE_address = j;
    length = i;
    DataAddress = p;
    i = 0;
    IAP_READ();                             //读N个字节并比较
    do
    {
        IAP_ADDRE = 0; 						//送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        EEPROM_Trig();                      //触发EEPROM操作
        if(*DataAddress != IAP_DATA)        //读出的数据与源数据比较
        {
            i = 2;
            break;
        }
        EE_address++;
        DataAddress++;
    }while(--length);
	DisableEEPROM();		//禁止EEPWOM操作
    return i;
}

