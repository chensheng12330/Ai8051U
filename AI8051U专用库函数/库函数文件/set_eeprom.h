#ifndef __SET_EEPROM_H__
#define __SET_EEPROM_H__

#include "AI8051U.H"

//均衡磨损自动占用EEPROM的第一个扇区(512Byte)，可以自定义设置起始值和长度值
#define EEPROM_Offset 0
#define EEPROM_Pack_Len 512
#define EEPROM_Pack_Max 10 //设置最多可以绑定多少个变量，如果不够可以调大。没有绑定使用的部分不会参与EEPROM存储
//EEPROM基础操作函数的模式
typedef enum
{
    Read_Byte = 10,//读取EEPROM,1个字节(函数等效为:char set_eeprom_base(Read_Byte, unsigned long addr);)
    Read_Buff = 11,//读取EEPROM,多个字节(函数等效为:void set_eeprom_base(Read_Buff, unsigned long addr, char* buf, int len);)
    Write_Byte = 20,//写入EEPROM,1个字节(函数等效为:void set_eeprom_base(Write_Byte, unsigned long addr, char value);)
    Write_Buff = 21,//写入EEPROM,多个字节(函数等效为:void set_eeprom_base(Write_Buff, unsigned long addr, char* buf, int len);)
    Erase_Sector = 30,//擦除EEPROM,1个扇区(函数等效为:void set_eeprom_base(Erase_Sector, unsigned long addr);)
    Erase_Sectors = 31//擦除EEPROM,多个扇区(函数等效为:void set_eeprom_base(Erase_Sectors, unsigned long addr, int len);)
} eeprom_mode;


struct EEPROM_Pack 
{
    void *addr;//数据地址
    unsigned char len;//数据长度
};

#define Hex_Mode "\x01 hex"   // 单字节模式, 用于绑定单个变量
// #define Buff_Mode "\x02 buff" // 数组模式，用于同时绑定多个变量，目前暂未支持，等待后续更新

#define Push "\x03 push" //用于将已经绑定的变量值推送到EEPROM中，方向是从RAM->EEPROM
#define Pull "\x04 pull" //用于将EEPROM中的值推送到RAM中，方向是从EEPROM->RAM

//EEPROM基础使用函数，用于完成底层基础操作，建议使用后续的变量绑定方式
//后续的函数带有均衡磨损功能，可以更方便的使用
//本函数具体使用方式请查看eeprom_mode的枚举定义注释，有对应的等效函数
unsigned char set_eeprom_base(eeprom_mode mode, unsigned long addr, ...);

//用于绑定变量到EEPROM，以实现类似掉电不变化的变量的效果。
//绑定之后，调用后面的set_eeprom_sync()函数，即可实现从EEPROM->RAM方向或者从RAM->EEPROM方向的同步
//使用示例，定义一个int变量：int num; 然后将这个变量绑定进EEPROM中
//set_eeprom_mode(Hex_Mode,&num,sizeof(num));
void set_eeprom_mode(const char *mode, void *value_addr, unsigned int len);

//用于同步EEPROM中的值到RAM中，或者从RAM中同步到EEPROM中，通过传入Push或者Pull来选择方向
void set_eeprom_sync(const char *sync);

//均衡磨损的数据存储格式：0xaa|len|数据部分|add8
//存储会自动搜索空部分来存储数据，如果存储位置不足，则自动擦除扇区，重新存储数据
//每次pull的时候，发现与存储格式不匹配，则会退出匹配，之前匹配的不受影响
//每次push的时候，会自动将上一次的数据包中的所有数据改写成0x00
//以防其他数据干扰搜索

#endif