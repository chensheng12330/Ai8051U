/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

/*************  本程序功能说明  **************

本例程基于AI8051U为主控芯片的实验箱进行编写测试。

使用Keil C251编译器，Memory Model推荐设置XSmall模式，默认定义变量在edata，单时钟存取访问速度快。

edata建议保留1K给堆栈使用，空间不够时可将大数组、不常用变量加xdata关键字定义到xdata空间。

通过串口1(P3.0 P3.1)对STC内部自带的EEPROM(FLASH)进行读写测试。

对FLASH做扇区擦除、写入、读出的操作，命令指定地址。

默认波特率:  115200,8,N,1. 
默认主时钟:  22.1184MHz.

串口命令设置: (命令字母不区分大小写)
    W 0x000040 1234567890  --> 对0x000040地址写入字符1234567890.
    R 0x000040 10          --> 对0x000040地址读出10个字节数据. 

注意：下载时，下载界面"硬件选项"中设置用户EEPROM大小，EEPROM大小包括用户系统区
例如需要用户系统区4K、EEPROM区8K，那么用户EEPROM大小需要设置12K。

并确保串口命令中的地址在EEPROM设置的大小范围之内。

******************************************/

#include "..\..\comm\AI8051U.h"
#include "intrins.h"

#define     MAIN_Fosc       22118400L   //定义主时钟（精确计算115200波特率）

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

#define Baudrate1   (65536 - MAIN_Fosc / 115200 / 4)
#define Tmp_Length  100      //读写EEPROM缓冲长度

#define UART1_BUF_LENGTH    (Tmp_Length+11)  //串口缓冲长度

u8  RX1_TimeOut;
u8  TX1_Cnt;    //发送计数
u8  RX1_Cnt;    //接收计数
bit B_TX1_Busy; //发送忙标志

u8  RX1_Buffer[UART1_BUF_LENGTH];   //接收缓冲
u8  tmp[Tmp_Length];        //EEPROM操作缓冲


void    UART1_config(u8 brt);   // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
void    PrintString1(u8 *puts);
void    UART1_TxByte(u8 dat);
void    delay_ms(u8 ms);
u8      CheckData(u8 dat);
u32     GetAddress(void);
u8      GetDataLength(void);
void    EEPROM_SectorErase(u32 EE_address);
void    EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 length);
u8      EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length);


/********************* 主函数 *************************/
void main(void)
{
    u8  i,j;
    u32 addr;
    u8  status;

    WTST = 0;  //设置程序指令延时参数，赋值为0可将CPU执行指令的速度设置为最快
    EAXFR = 1; //扩展寄存器(XFR)访问使能
    CKCON = 0; //提高访问XRAM速度

    P0M1 = 0x00;   P0M0 = 0x00;   //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;   //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;   //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x00;   //设置为准双向口
    P4M1 = 0x00;   P4M0 = 0x00;   //设置为准双向口
    P5M1 = 0x00;   P5M0 = 0x00;   //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;   //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;   //设置为准双向口

    UART1_config(1);    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
    EA = 1; //允许总中断

    PrintString1("AI8051U系列单片机EEPROM测试程序，串口命令设置如下:\r\n");    //UART1发送一个字符串
    PrintString1("W 0x000040 1234567890  --> 对0x000040地址写入字符1234567890.\r\n");   //UART1发送一个字符串
    PrintString1("R 0x000040 10          --> 对0x000040地址读出10个字节内容.\r\n"); //UART1发送一个字符串

    while(1)
    {
        delay_ms(1);
        if(RX1_TimeOut > 0)     //超时计数
        {
            if(--RX1_TimeOut == 0)
            {
              for(i=0; i<RX1_Cnt; i++)    UART1_TxByte(RX1_Buffer[i]);    //把收到的数据原样返回,用于测试

                status = 0xff;  //状态给一个非0值
                if((RX1_Cnt >= 12) && (RX1_Buffer[1] == ' ') && (RX1_Buffer[10] == ' ')) //最短命令为10个字节
                {
                    for(i=0; i<10; i++)
                    {
                        if((RX1_Buffer[i] >= 'a') && (RX1_Buffer[i] <= 'z'))    RX1_Buffer[i] = RX1_Buffer[i] - 'a' + 'A';  //小写转大写
                    }
                    addr = GetAddress();
                    if(addr < 0x00ffffff)
                    {
                        if(RX1_Buffer[0] == 'W')    //写入N个字节
                        {
                            j = RX1_Cnt - 11;
                            if(j > Tmp_Length)  j = Tmp_Length; //越界检测
                            EEPROM_SectorErase(addr);           //擦除扇区
                            i = EEPROM_write_n(addr,&RX1_Buffer[11],j);      //写N个字节
                            if(i == 0)
                            {
                                PrintString1("\r\n已写入");
                                if(j >= 100)    {UART1_TxByte((u8)(j/100+'0'));   j = j % 100;}
                                if(j >= 10)     {UART1_TxByte((u8)(j/10+'0'));    j = j % 10;}
                                UART1_TxByte((u8)(j%10+'0'));
                                PrintString1("字节！\r\n");
                            }
                            else    PrintString1("\r\n写入错误！\r\n");
                            status = 0; //命令正确
                        }

                        else if(RX1_Buffer[0] == 'R')   //PC请求返回N字节EEPROM数据
                        {
                            j = GetDataLength();
                            if(j > Tmp_Length)  j = Tmp_Length; //越界检测
                            if(j > 0)
                            {
                                PrintString1("\r\n读出");
                                UART1_TxByte((u8)(j/10+'0'));
                                UART1_TxByte((u8)(j%10+'0'));
                                PrintString1("个字节内容如下:\r\n");
                                EEPROM_read_n(addr,tmp,j);
                                for(i=0; i<j; i++)  UART1_TxByte(tmp[i]);
                                UART1_TxByte(0x0d);
                                UART1_TxByte(0x0a);
                                status = 0; //命令正确
                            }
                        }
                    }
                }
                if(status != 0) PrintString1("\r\n命令错误！\r\n");
                RX1_Cnt  = 0;   //清除字节数
            }
        }
    }
}

//========================================================================
// 函数: void delay_ms(u8 ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2021-3-9
// 备注: 
//========================================================================
void delay_ms(u8 ms)
{
     u16 i;
     do{
          i = MAIN_Fosc / 6000;
          while(--i);
     }while(--ms);
}

//========================================================================
// 函数: u8 CheckData(u8 dat)
// 描述: 将字符"0~9,A~F或a~f"转成十六进制.
// 参数: dat: 要检测的字符.
// 返回: 0x00~0x0F为正确. 0xFF为错误.
// 版本: V1.0, 2012-10-22
//========================================================================
u8 CheckData(u8 dat)
{
    if((dat >= '0') && (dat <= '9'))        return (dat-'0');
    if((dat >= 'A') && (dat <= 'F'))        return (dat-'A'+10);
    return 0xff;
}

//========================================================================
// 函数: u32 GetAddress(void)
// 描述: 计算各种输入方式的地址.
// 参数: 无.
// 返回: 24位EEPROM地址.
// 版本: V1.0, 2013-6-6
//========================================================================
u32 GetAddress(void)
{
    u32 address;
    u8  i,j;
    
    address = 0;
    if((RX1_Buffer[2] == '0') && (RX1_Buffer[3] == 'X'))
    {
        for(i=4; i<10; i++)
        {
            j = CheckData(RX1_Buffer[i]);
            if(j >= 0x10)   return 0xffffffff;   //error
            address = (address << 4) + j;
        }
        return (address);
    }
    return  0xffffffff;  //error
}

/**************** 获取要读出数据的字节数 ****************************/
u8 GetDataLength(void)
{
    u8  i;
    u8  length;
    
    length = 0;
    for(i=11; i<RX1_Cnt; i++)
    {
        if(CheckData(RX1_Buffer[i]) >= 10)  break;
        length = length * 10 + CheckData(RX1_Buffer[i]);
    }
    return (length);
}


//========================================================================
// 函数: void UART1_TxByte(u8 dat)
// 描述: 发送一个字节.
// 参数: 无.
// 返回: 无.
// 版本: V1.0, 2014-6-30
//========================================================================
void UART1_TxByte(u8 dat)
{
    SBUF = dat;
    B_TX1_Busy = 1;
    while(B_TX1_Busy);
}


//========================================================================
// 函数: void PrintString1(u8 *puts)
// 描述: 串口2发送字符串函数。
// 参数: puts:  字符串指针.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void PrintString1(u8 *puts) //发送一个字符串
{
    for (; *puts != 0;  puts++) UART1_TxByte(*puts);    //遇到停止符0结束
}

//========================================================================
// 函数: void SetTimer2Baudraye(u16 dat)
// 描述: 设置Timer2做波特率发生器。
// 参数: dat: Timer2的重装值.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void SetTimer2Baudraye(u16 dat)  // 选择波特率, 2: 使用Timer2做波特率, 其它值: 无效.
{
    T2R = 0;    //Timer stop
    T2_CT = 0;  //Timer2 set As Timer
    T2x12 = 1;  //Timer2 set as 1T mode
    T2H = (u8)(dat / 256);
    T2L = (u8)(dat % 256);
    ET2 = 0;    //禁止中断
    T2R = 1;    //Timer run enable
}

//========================================================================
// 函数: void UART1_config(u8 brt)
// 描述: UART1初始化函数。
// 参数: brt: 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_config(u8 brt)    // 选择波特率, 2: 使用Timer2做波特率, 其它值: 使用Timer1做波特率.
{
    /*********** 波特率使用定时器2 *****************/
    if(brt == 2)
    {
        S1BRT = 1;	//S1 BRT Use Timer2;
        SetTimer2Baudraye((u16)Baudrate1);
    }

    /*********** 波特率使用定时器1 *****************/
    else
    {
        TR1 = 0;
        S1BRT = 0;		//S1 BRT Use Timer1;
        T1_CT = 0;		//Timer1 set As Timer
        T1x12 = 1;		//Timer1 set as 1T mode
        TMOD &= ~0x30;//Timer1_16bitAutoReload;
        TH1 = (u8)(Baudrate1 / 256);
        TL1 = (u8)(Baudrate1 % 256);
        ET1 = 0;    //禁止中断
        TR1 = 1;
    }
    /*************************************************/

    SCON = (SCON & 0x3f) | 0x40;    //UART1模式, 0x00: 同步移位输出, 0x40: 8位数据,可变波特率, 0x80: 9位数据,固定波特率, 0xc0: 9位数据,可变波特率
//  PS  = 1;    //高优先级中断
    ES  = 1;    //允许中断
    REN = 1;    //允许接收
    P_SW1 &= 0x3f;
    P_SW1 |= 0x00;      //UART1 switch to, 0x00: P3.0 P3.1, 0x40: P3.6 P3.7, 0x80: P1.6 P1.7, 0xC0: P4.3 P4.4

    B_TX1_Busy = 0;
    TX1_Cnt = 0;
    RX1_Cnt = 0;
}


//========================================================================
// 函数: void UART1_int (void) interrupt UART1_VECTOR
// 描述: UART1中断函数。
// 参数: nine.
// 返回: none.
// 版本: VER1.0
// 日期: 2014-11-28
// 备注: 
//========================================================================
void UART1_int (void) interrupt 4
{
    if(RI)
    {
        RI = 0;     //Clear Rx flag
        RX1_Buffer[RX1_Cnt] = SBUF;
        if(++RX1_Cnt >= UART1_BUF_LENGTH)   RX1_Cnt = 0;
        RX1_TimeOut = 5;
    }

    if(TI)
    {
        TI = 0;     //Clear Tx flag
        B_TX1_Busy = 0;
    }
}


#define     IAP_STANDBY()   IAP_CMD = 0     //IAP空闲命令（禁止）
#define     IAP_READ()      IAP_CMD = 1     //IAP读出命令
#define     IAP_WRITE()     IAP_CMD = 2     //IAP写入命令
#define     IAP_ERASE()     IAP_CMD = 3     //IAP擦除命令

#define     IAP_ENABLE()    IAP_CONTR = IAP_EN; IAP_TPS = MAIN_Fosc / 1000000
#define     IAP_DISABLE()   IAP_CONTR = 0; IAP_CMD = 0; IAP_TRIG = 0; IAP_ADDRE = 0xff; IAP_ADDRH = 0xff; IAP_ADDRL = 0xff

#define IAP_EN          (1<<7)
#define IAP_SWBS        (1<<6)
#define IAP_SWRST       (1<<5)
#define IAP_CMD_FAIL    (1<<4)


//========================================================================
// 函数: void DisableEEPROM(void)
// 描述: 禁止EEPROM.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void DisableEEPROM(void)        //禁止访问EEPROM
{
    IAP_CONTR = 0;          //关闭 IAP 功能
    IAP_CMD = 0;            //清除命令寄存器
    IAP_TRIG = 0;           //清除触发寄存器
    IAP_ADDRE = 0xff;       //将地址设置到非 IAP 区域
    IAP_ADDRH = 0xff;       //将地址设置到非 IAP 区域
    IAP_ADDRL = 0xff;
}

//========================================================================
// 函数: void EEPROM_Trig(void)
// 描述: 触发EEPROM操作.
// 参数: none.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_Trig(void)
{
    F0 = EA;    //保存全局中断
    EA = 0;     //禁止中断, 避免触发命令无效
    IAP_TRIG = 0x5A;
    IAP_TRIG = 0xA5;    //先送5AH，再送A5H到IAP触发寄存器，每次都需要如此
                        //送完A5H后，IAP命令立即被触发启动
                        //CPU等待IAP完成后，才会继续执行程序。
    _nop_();   //多级流水线的指令系统，触发命令后建议加4个NOP，保证IAP_DATA的数据完成准备
    _nop_();
    _nop_();
    _nop_();
    EA = F0;    //恢复全局中断
}

//========================================================================
// 函数: void EEPROM_SectorErase(u32 EE_address)
// 描述: 擦除一个扇区.
// 参数: EE_address:  要擦除的EEPROM的扇区中的一个字节地址.
// 返回: none.
// 版本: V1.0, 2014-6-30
//========================================================================
void EEPROM_SectorErase(u32 EE_address)
{
    IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
    IAP_ERASE();                        //宏调用, 送扇区擦除命令，命令不需改变时，不需重新送命令
                                        //只有扇区擦除，没有字节擦除，512字节/扇区。
                                        //扇区中任意一个字节地址都是扇区地址。
    IAP_ADDRE = (u8)(EE_address >> 16); //送扇区地址高字节（地址需要改变时才需重新送地址）
    IAP_ADDRH = (u8)(EE_address >> 8);  //送扇区地址中字节（地址需要改变时才需重新送地址）
    IAP_ADDRL = (u8)EE_address;         //送扇区地址低字节（地址需要改变时才需重新送地址）
    EEPROM_Trig();                      //触发EEPROM操作
    DisableEEPROM();                    //禁止EEPROM操作
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
void EEPROM_read_n(u32 EE_address,u8 *DataAddress,u8 length)
{
    IAP_ENABLE();                           //设置等待时间，允许IAP操作，送一次就够
    IAP_READ();                             //送字节读命令，命令不需改变时，不需重新送命令
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
        IAP_ADDRH = (u8)(EE_address >> 8);  //送地址中字节（地址需要改变时才需重新送地址）
        IAP_ADDRL = (u8)EE_address;         //送地址低字节（地址需要改变时才需重新送地址）
        EEPROM_Trig();                      //触发EEPROM操作
        *DataAddress = IAP_DATA;            //读出的数据送往
        EE_address++;
        DataAddress++;
    }while(--length);

    DisableEEPROM();
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
u8 EEPROM_write_n(u32 EE_address,u8 *DataAddress,u8 length)
{
    u8  i;
    u16 j;
    u8  *p;
    
    if(length == 0) return 1;   //长度为0错误

    IAP_ENABLE();                       //设置等待时间，允许IAP操作，送一次就够
    i = length;
    j = EE_address;
    p = DataAddress;
    IAP_WRITE();                            //宏调用, 送字节写命令
    do
    {
        IAP_ADDRE = (u8)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
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
        IAP_ADDRE = (u8)(EE_address >> 16); //送地址高字节（地址需要改变时才需重新送地址）
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

    DisableEEPROM();
    return i;
}

