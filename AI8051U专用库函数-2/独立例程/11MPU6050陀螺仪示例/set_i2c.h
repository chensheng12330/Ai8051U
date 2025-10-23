#ifndef __SET_I2C_H__
#define __SET_I2C_H__

#include "AI8051U.H"

typedef enum
{
    I2c0 = 0 //目前只有一个i2c外设，但是为了兼容性，仍然需要声明
}i2c_name;

#define I2c_End "end"
#define I2c_Master "\x01master"//设置I2C为主机模式，默认为主机状态
//#define I2c_Slave "\x01slave"//设置I2C为从机模式,目前尚未适配
#define I2c_Enable "\x01enable"//设置I2C使能，默认为使能状态
#define I2c_Disable "\x01disable"//设置I2C外设关闭
#define I2c_P24_3 "i2c0" // 引脚切换宏定义,前面的是SCL，后面的是SDA
#define I2c_P15_4 "i2c1"
#define I2c_P32_3 "i2c3"
//引脚切换的默认值为I2c_P24_3

typedef enum
{
    Start = 10,     // 发送起始信号，标明所有操作的开始
    Tx_Dat = 20,    // 发送数据，SCL产生8个时钟，同时SDA按先发送高位数据的顺序发送8位数据
    Rack = 30,      // 接收ACK，SCL产生1个时钟，SDA输入状态，接收I2C从机返回ACK(0)或NACK(1)
    Rx_Dat = 40,    // 接收数据，SCL产生8个时钟，同时SDA按先接收高位数据的顺序接收8位数据
    Tack = 50,      // 发送ACK(0)，SCL产生1个时钟，SDA输出状态，发送一个ACK(0)
    Tnak = 51,      // 发送NACK(1)，SCL产生1个时钟，SDA输出状态，发送一个NACK(1)
    Stop = 60,      // 发送停止信号，标明所有操作的结束
    S_Tx_Rack = 90, // 发送起始信号和数据后接收ACK，联合操作
    Tx_Rack = 100,  // 发送数据后接收ACK，联合操作
    Rx_Tack = 110,  // 接收数据后发送ACK，联合操作
    Rx_Tnak = 120,  // 接收数据后发送NACK，联合操作
    Cmd_End = 0xff  // 命令流结束标志
} i2c_cmd;
//I2C 控制命令,适用于后面的set_i2c_cmd函数

#define Max_I2c_Cmd 50 // 最大指令缓存长度，如果超出，后面的就不会生效了
//最大缓存指令限制的是一次调用set_i2c_mode内的参数数量，除了前两个参数，后面参数每一个都算一个参数
#define Max_I2c_Task 5 //最大任务缓存长度，如果超出，后面的就不会生效了
//最大缓存任务限制的是调用set_i2c_cmd函数中，task_num的最大值。

//函数说明：设置I2C的速度和模式
//例如设置i2c为主机模式，总线通讯速率为400khz, 引脚切换（P24和P23）为如下代码
//set_i2c_mode(I2c0, "400khz", I2c_P24_3, I2c_Master, I2c_End);
//同时，设置模式支持默认值和乱序输入（除了第一个和最后一个参数，剩下的没有输入顺序要求）
//通讯速率支持单位有khz，mhz，都是小写。
//使用set_i2c_mode(I2c0, I2c_End);//默认值生效，默认值为400khz和主机模式,引脚默认为I2c_P24_3
void set_i2c_mode(i2c_name i2c, ...);

//设置i2c的指令组合，后面参数为自由组合i2c_cmd内的指令，同时最后一个参数必须为Cmd_End
//例如发送i2c地址并且发送数据，可以使用如下代码,分别是写入的0x20地址上的0x01数据为0xff
//set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0x20, Tx_Rack, 0x01, Tx_Rack, 0x01, Stop, Cmd_End);
//再给出一个读取i2c的例子,先写入了0x01的地址，然后读出一个数据
//set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0x20, Tx_Rack, 0x01,
//S_Tx_Rack, 0x21, Rx_Tnak, &dat, Stop, Cmd_End);//参数较长时可以换行输入
//需要注意的是，函数采用指令缓存操作，设置多条set_i2c_cmd()后, 使用不同的task_num,
//任务会按照task_num的大小在后台依次执行，并不会等待执行完成后再执行下一条。
//在某个已经使用的task_num执行任务时，再次设置set_i2c_cmd()，会拒绝这次的设置，直到上次任务执行完成后才可以设置
//所以尽量不要使用while直接循环调用set_i2c_cmd()，会导致缓存溢出，进而导致执行错误
void set_i2c_cmd(i2c_name i2c, int task_num, ...);

//获取i2c的状态，输入参数为i2c_name，和task_num，检测到当前任务完成后返回1，否则返回0
char get_i2c_state(i2c_name i2c, int task_num);

// 用于设置I2C的时钟频率，传入的是系统时钟频率，单位是Hz
// 一般来说不用管，设置的时候会自动设置
void set_i2c_fosc(long fosc);

#endif