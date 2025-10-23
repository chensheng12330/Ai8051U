#ifndef __SET_IO_H__
#define __SET_IO_H__

#include "AI8051U.H"

// IO模式枚举
typedef enum
{
    pu_mode = 0,           // 准双向口模式，pull-up缩写
    pp_mode,               // 推挽输出模式，push-pull缩写
    hz_mode,               // 高阻输入模式，high-z缩写
    od_mode,               // 开漏模式，open-drain缩写
    dis_pur = 10,          // 关闭上拉电阻 disable pull-up-resistor缩写
    en_pur = 11,           // 开启上拉电阻 enable pull-up-resistor缩写
    en_schmitt_trig = 20,  // 开启施密特触发模式 enable schmitt trigger缩写
    dis_schmitt_trig = 21, // 关闭施密特触发模式 disable schmitt trigger缩写
    low_speed = 31,        // 较低的电平转换速度，相应的上下冲比较小
    high_speed = 30,       // 较高的电平转换速度，相应的上下冲会比较大
    small_current = 41,    // 小电流驱动，一般驱动能力
    big_current = 40,      // 大电流驱动，增强驱动能力
    en_dinput = 51,        // 打开数字输入使能 enable digital input缩写，若 I/O 被当作数字口时，必须设置为 1，否 MCU 无法读取外部端口的电平。
    dis_dinput = 50,       // 关闭数字输入使能 disable digital input缩写，进入主时钟停振/省电模式前，必须设置为 0，否则会有额外的耗电。
    dis_pdr = 60,          // 关闭下拉电阻 disable pull-down-resistor缩写
    en_pdr = 61,           // 打开下拉电阻 enable pull-down-resistor缩写
    dis_auto_config = 71,  // 关闭自动配置引脚模式 disable auto configuration缩写，外设模块对所用 I/O 的模式不自动配置，需要用户使用 PxM0/PxM1 寄存器对 I/O 进行配置。
    en_auto_config = 70    // 打开自动配置引脚模式 enable auto configuration缩写，外设模块对所用 I/O 的模式进行自动配置，忽略用户使用 PxM0/PxM1 对相应 I/O 进行的配置。
} io_mode;

// IO引脚枚举
typedef enum
{
    // 为了防止跟P00这种头文件定义冲突，这里使用Pin00这种命名
    Pin00 = 0,Pin01,Pin02,Pin03,Pin04,Pin05,Pin06,Pin07,
    Pin10,Pin11,Pin12,Pin13,Pin14,Pin15,Pin16,Pin17,
    Pin20,Pin21,Pin22,Pin23,Pin24,Pin25,Pin26,Pin27,
    Pin30,Pin31,Pin32,Pin33,Pin34,Pin35,Pin36,Pin37,
    Pin40,Pin41,Pin42,Pin43,Pin44,Pin45,Pin46,Pin47,
    Pin50,Pin51,Pin52,Pin53,Pin54,Pin55,Pin56,Pin57,Pin_End = 0xff
} io_name;

// 声明函数
void set_io_mode(io_mode mode, ...); // 批量设置IO的模式
// 详细解释:mode为IO的模式，后面为可变参数的io_name参数，需要注意的是，输入完后需要添加Pin_End作为结束符
// 例如set_io_mode(pu_mode,Pin00,Pin21,Pin32,Pin_End);就是将P00,P21,P32这3个IO设置为上拉输入模式

#endif