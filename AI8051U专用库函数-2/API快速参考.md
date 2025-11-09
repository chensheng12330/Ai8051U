# AI8051U 库函数 API 快速参考

> 快速查询手册 - 常用API和参数一览

---

## 📌 IO控制 (set_io)

### 函数原型
```c
void set_io_mode(io_mode mode, ...);
```

### 常用模式
| 宏定义 | 说明 | 应用场景 |
|--------|------|----------|
| `pu_mode` | 准双向 | 通用IO，默认模式 |
| `pp_mode` | 推挽输出 | LED、继电器 |
| `hz_mode` | 高阻输入 | 按键、传感器 |
| `od_mode` | 开漏输出 | I2C通信 |
| `en_pur` | 使能上拉 | 按键输入 |
| `en_pdr` | 使能下拉 | 特殊应用 |
| `big_current` | 大电流 | 驱动负载 |
| `high_speed` | 高速 | 高频信号 |

### 快速示例
```c
// LED输出
set_io_mode(pp_mode, Pin00, Pin01, Pin_End);

// 按键输入
set_io_mode(hz_mode, Pin32, Pin_End);
set_io_mode(en_pur, Pin32, Pin_End);

// I2C引脚
set_io_mode(od_mode, Pin23, Pin_End);
set_io_mode(en_pur, Pin23, Pin_End);
```

---

## 📌 IO中断 (io_int)

### 函数原型
```c
void set_ioint_mode(int mode, ...);
char get_ioint_state(io_name pin);
void set_ioint_isr(io_name pin, void (*isr)(void));
```

### 中断模式
| 宏定义 | 触发方式 |
|--------|----------|
| `falling_edge_mode` | 下降沿 |
| `rising_edge_mode` | 上升沿 |
| `low_level_mode` | 低电平 |
| `high_level_mode` | 高电平 |
| `en_int` | 使能中断 |
| `dis_int` | 禁用中断 |

### 快速示例
```c
// 按键中断
set_ioint_mode(falling_edge_mode, Pin32, Pin_End);
set_ioint_mode(en_int, Pin32, Pin_End);
EA = 1;

// 查询方式
if(get_ioint_state(Pin32)) {
    // 处理中断
}

// 回调方式
void key_handler(void) { P00 = ~P00; }
set_ioint_isr(Pin32, key_handler);
```

---

## 📌 定时器 (set_timer)

### 函数原型
```c
void set_timer_mode(timer_num num, ...);
char get_timer_state(timer_num num);
unsigned int get_timer_cnt(timer_num num);
void set_timer_isr(timer_num num, void (*isr)(void));
```

### 定时器列表
`Timer0`, `Timer1`, `Timer2`, `Timer3`, `Timer4`, `Timer11`

### 时间格式
| 格式 | 示例 |
|------|------|
| 秒 | `"1s"`, `"0.5s"` |
| 毫秒 | `"100ms"`, `"1.5ms"` |
| 微秒 | `"500us"` |
| 频率 | `"50hz"`, `"1000hz"` |

### 快速示例
```c
// 100ms定时
set_timer_mode(Timer0, "100ms", Timer_End);
if(get_timer_state(Timer0)) {
    // 定时到
}

// 使能时钟输出
set_timer_mode(Timer1, "1000hz", En_OutClk, Timer_End);

// 外部计数
set_timer_mode(Timer0, Cnt_Mode, Timer_End);
unsigned int cnt = get_timer_cnt(Timer0);

// 回调函数
void timer_callback(void) { /* ... */ }
set_timer_isr(Timer0, timer_callback);
```

---

## 📌 串口 (set_uart)

### 函数原型
```c
void set_uart_mode(uart_name uart, ...);
char get_uart_state(uart_name uart);
void uart_printf(uart_name uart, ...);
```

### 串口列表
`Uart1`, `Uart2`, `Uart3`, `Uart4`

### 引脚切换
| 串口 | 可选引脚 |
|------|----------|
| Uart1 | `Uart1_P30_1`(默认), `Uart1_P36_7`, `Uart1_P16_7`, `Uart1_P43_4` |
| Uart2 | `Uart2_P12_3`(默认), `Uart2_P42_3` |
| Uart3 | `Uart3_P00_1`(默认), `Uart3_P50_1` |
| Uart4 | `Uart4_P02_3`(默认), `Uart4_P52_3` |

### 波特率格式
`"9600bps"`, `"115200bps"` 等

### 快速示例
```c
// 基本配置
set_uart_mode(Uart1, Uart_End);  // 默认115200

// 自定义波特率
set_uart_mode(Uart1, "9600bps", Uart1_P36_7, Uart_End);

// 多串口
set_uart_mode(Uart1, "115200bps", Use_Timerx, Uart_End);
set_uart_mode(Uart2, "9600bps", Use_Timerx, Uart_End);

// 发送数据
uart_printf(Uart1, "Temp:%d\r\n", temp);
uart_printf(Uart1, Hex_Mode, 0x55);
uart_printf(Uart1, Buff_Mode, buf, 10);

// 接收数据
if(get_uart_state(Uart1)) {
    sscanf(_uart1_rx_buff, "cmd:%d", &num);
}
```

---

## 📌 ADC采样 (set_adc)

### 函数原型
```c
void set_adc_mode(adc_mode mode, ...);
char get_adc_state(adc_ch ch);
extern unsigned int adc_value[16];
```

### ADC通道
| 通道 | 引脚 |
|------|------|
| `Adc0_P10` ~ `Adc7_P17` | P1口 |
| `Adc8_P00` ~ `Adc14_P06` | P0口 |
| `Adc15_1_19V` | 内部基准 |

### 采样模式
- `single_mode` - 单次
- `cycl_mode` - 循环

### 快速示例
```c
// 循环采样
set_adc_mode(cycl_mode, Adc0_P10, Adc_End);
if(get_adc_state(Adc0_P10)) {
    unsigned int val = adc_value[Adc0_P10];
}

// 多通道
set_adc_mode(cycl_mode, Adc0_P10, Adc1_P11, Adc_End);
```

---

## 📌 PWM控制 (set_pwm)

### 函数原型
```c
void set_pwm_mode(pwm_name pwm, ...);
void set_pwm_duty(pwm_name pwm, float duty);
float get_pwm_period(pwm_name pwm, dat_unit unit);
float get_pwm_duty(pwm_inx pwm);
```

### PWM通道
`Pwm1` ~ `Pwm4` (PWMA组), `Pwm5` ~ `Pwm8` (PWMB组)

### 输出模式
| 宏定义 | 说明 |
|--------|------|
| `En_Out_P` | 仅P通道(默认) |
| `En_Out_N` | 仅N通道 |
| `En_Out_PN` | 互补输出 |

### 频率格式
`"1khz"`, `"10khz"`, `"50hz"` 等

### 占空比格式
`"50%"`, `"75.5%"` 等

### 快速示例
```c
// 基本输出
set_pwm_mode(Pwm1, Pwm1_P00_01, "10khz", "50%", Pwm_End);

// 互补输出
set_pwm_mode(Pwm1, "20khz", "80%", En_Out_PN, "50clk", Pwm_End);

// 动态调占空比
set_pwm_duty(Pwm1, 75.5);

// 输入捕获
set_pwm_mode(Pwm5, Pwm_In_Mode, Self_Capture, Pwm_End);
set_pwm_mode(Pwm6, Pwm_In_Mode, Cross_Capture, Pwm_End);
float freq = get_pwm_period(Pwm5, khz);
float duty = get_pwm_duty(Pwm5_Pwm6);
```

---

## 📌 I2C通信 (set_i2c)

### 函数原型
```c
void set_i2c_mode(i2c_name i2c, ...);
void set_i2c_cmd(i2c_name i2c, int task_num, ...);
char get_i2c_state(i2c_name i2c, int task_num);
```

### I2C引脚
| 宏定义 | SCL | SDA |
|--------|-----|-----|
| `I2c_P24_3` | P24 | P23 |
| `I2c_P15_4` | P15 | P14 |
| `I2c_P32_3` | P32 | P33 |

### I2C指令
| 指令 | 说明 |
|------|------|
| `Start` | 起始信号 |
| `Stop` | 停止信号 |
| `Tx_Dat` | 发送数据 |
| `Rx_Dat` | 接收数据 |
| `Rack` | 接收ACK |
| `Tack` | 发送ACK |
| `Tnak` | 发送NACK |
| `S_Tx_Rack` | Start+Tx+Rack |
| `Tx_Rack` | Tx+Rack |
| `Rx_Tack` | Rx+Tack |
| `Rx_Tnak` | Rx+Tnak |

### 快速示例
```c
// 配置I2C
set_i2c_mode(I2c0, "400khz", I2c_End);

// 写EEPROM
set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xa0, 
            Tx_Rack, 0x00, Tx_Rack, 0x55, 
            Stop, Cmd_End);

// 读EEPROM
char data;
set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xa0, Tx_Rack, 0x00,
            S_Tx_Rack, 0xa1, Rx_Tnak, &data, 
            Stop, Cmd_End);

// 等待完成
while(!get_i2c_state(I2c0, 0));
```

---

## 📌 SPI通信 (set_spi)

### 函数原型
```c
void set_spi_mode(spi_name spi, ...);
char get_spi_state(spi_name spi);
void spi_printf(spi_name spi, ...);
```

### SPI列表
`SPI0`, `SPI1`(占用Uart1), `SPI2`(占用Uart2)

### 引脚切换
| 宏定义 | SS | MOSI | MISO | SCLK |
|--------|-----|------|------|------|
| `Spi_P14_5_6_7` | P14 | P15 | P16 | P17 |
| `Spi_P24_5_6_7` | P24 | P25 | P26 | P27 |
| `Spi_P40_1_2_3` | P40 | P41 | P42 | P43 |
| `Spi_P35_4_3_2` | P35 | P34 | P33 | P32 |

### 时钟分频
`Spi_ClkDiv_2`, `Spi_ClkDiv_4`, `Spi_ClkDiv_8`, `Spi_ClkDiv_16`

### 快速示例
```c
// 基本配置
set_spi_mode(SPI0, Spi_End);

// 自定义配置
set_spi_mode(SPI0, Spi_ClkDiv_4, High_Falling, MSB, Spi_End);

// 发送单字节
spi_printf(SPI0, Hex_Mode, 0x55);
while(!get_spi_state(SPI0));

// 发送缓冲区
char buf[10] = {0x01, 0x02, 0x03};
spi_printf(SPI0, Buff_Mode, buf, 3);
while(!get_spi_state(SPI0));

// 读取接收数据
char rx = _spi0_rx_buff[0];
```

---

## 📌 EEPROM存储 (set_eeprom)

### 函数原型
```c
unsigned char set_eeprom_base(eeprom_mode mode, ...);
void set_eeprom_mode(const char *mode, void *value_addr, unsigned int len);
void set_eeprom_sync(const char *sync);
```

### 基础操作
| 模式 | 用法 |
|------|------|
| `Read_Byte` | `char val = set_eeprom_base(Read_Byte, addr);` |
| `Write_Byte` | `set_eeprom_base(Write_Byte, addr, val);` |
| `Read_Buff` | `set_eeprom_base(Read_Buff, addr, buf, len);` |
| `Write_Buff` | `set_eeprom_base(Write_Buff, addr, buf, len);` |
| `Erase_Sector` | `set_eeprom_base(Erase_Sector, addr);` |

### 变量绑定（推荐）
```c
int settings = 1234;
set_eeprom_mode(Hex_Mode, &settings, sizeof(settings));
set_eeprom_sync(Push);  // 保存
set_eeprom_sync(Pull);  // 读取
```

---

## 📌 协程调度 (set_task)

### 协程宏
```c
task_start(num)         // 开始协程
task_delay(ms)          // 延时
task_wait(condition)    // 条件等待
task_for(init, cnt)     // for循环
task_while(condition)   // while循环
task_break(condition)   // 跳出循环
task_end(reload)        // 结束(1=循环,0=单次)
```

### 快速示例
```c
// 配置时间基准（必须）
set_timer_mode(Timer0, "1ms", Timer_End);
set_timer_isr(Timer0, set_task_mode);
EA = 1;

// 协程1：100ms闪烁
while(1) {
    task_start(0);
        P00 = ~P00;
        task_delay(100);
    task_end(1);
}

// 协程2：条件等待
task_start(1);
    task_delay(1000);
    task_wait(!P32);  // 等待按键
    P01 = ~P01;
task_end(1);

// 协程3：循环
int cnt;
task_start(2);
    task_for(cnt=0, cnt++) {
        P02 = ~P02;
        task_delay(50);
    }
    task_break(cnt < 10);
task_end(1);
```

---

## 📌 常用代码片段

### 初始化模板
```c
#include "AI8051U.h"
#include "set_io.h"

void main(void)
{
    EAXFR = 1;   // 使能扩展寄存器
    WTST = 0;    // 等待时间
    CKCON = 0;   // 时钟控制
    
    // 配置外设...
    
    EA = 1;      // 开启总中断
    
    while(1)
    {
        // 主循环
    }
}
```

### 延时函数（阻塞）
```c
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = ms; i > 0; i--)
        for(j = 120; j > 0; j--);  // @40MHz约1ms
}
```

### 按键消抖
```c
// 方法1：硬件（电容）
// 方法2：软件延时
if(P32 == 0) {
    delay_ms(10);
    if(P32 == 0) {
        // 确认按下
        while(P32 == 0);  // 等待释放
    }
}

// 方法3：协程（推荐）
task_start(0);
    task_wait(P32 == 0);
    task_delay(20);  // 消抖
    if(P32 == 0) {
        // 处理按键
    }
task_end(1);
```

---

## 📌 参数速查表

### 默认值汇总
| 功能 | 默认值 |
|------|--------|
| 串口波特率 | 115200bps |
| 串口超时 | 64byte |
| 串口定时器 | Timer2 |
| 定时器周期 | 1s |
| PWM频率 | 1kHz |
| PWM占空比 | 50% |
| PWM死区 | 10clk |
| PWM输出 | En_Out_P |
| I2C速率 | 400kHz |
| I2C引脚 | I2c_P24_3 |
| SPI分频 | 16分频 |
| SPI模式 | High_Falling |
| SPI顺序 | MSB |
| ADC分频 | 8分频 |

### 资源限制
| 项目 | 数量/大小 |
|------|----------|
| 定时器 | 6个 |
| 串口 | 4个 |
| ADC通道 | 16个 |
| PWM通道 | 8个 |
| I2C | 1个 |
| SPI | 3个 |
| 协程 | 10个(可扩展) |
| EEPROM变量绑定 | 10个(可扩展) |

---

## 📌 错误排查清单

### 编译错误
- [ ] 是否包含了正确的头文件？
- [ ] 是否添加了对应的.c文件到项目？
- [ ] 是否使用了 `Auto_Keil.exe` 配置工程？
- [ ] 参数最后是否有结束标志（`xxx_End`）？

### 功能异常
- [ ] 是否调用了 `EAXFR = 1; WTST = 0; CKCON = 0;`？
- [ ] 是否开启了总中断 `EA = 1;`？
- [ ] IO模式是否配置正确？
- [ ] 主频设置是否正确？
- [ ] 是否等待操作完成（非阻塞函数）？

### 串口问题
- [ ] 波特率是否匹配？
- [ ] 引脚配置是否正确？
- [ ] 多串口是否使用了 `Use_Timerx`？
- [ ] 是否检查了发送忙标志 `tx_state[]`？

### 协程问题
- [ ] 是否配置了1ms时间基准？
- [ ] 是否调用了 `set_timer_isr(Timer0, set_task_mode);`？
- [ ] 协程编号是否超过 `Task_Max_Num`？
- [ ] 是否在协程内使用了阻塞延时？

---

**快速参考手册结束**

*查看完整文档: AI8051U库函数使用手册.md*

