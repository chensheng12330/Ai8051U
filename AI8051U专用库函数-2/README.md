# AI8051U 专用库函数

[![芯片](https://img.shields.io/badge/芯片-STC_AI8051U-blue.svg)](https://www.stcai.com/)
[![编译器](https://img.shields.io/badge/编译器-Keil_C51-green.svg)](https://www.keil.com/)
[![版本](https://img.shields.io/badge/版本-V2.0-orange.svg)](库函数更新记录.txt)
[![许可](https://img.shields.io/badge/许可-开源-brightgreen.svg)]()

> 现代化、易用、功能强大的 STC AI8051U 单片机库函数集合

---

## ✨ 特性

### 🎯 核心优势

- **🚀 极简API** - 一行代码完成复杂配置
- **⚡ 非阻塞设计** - DMA自动收发，协程多任务
- **🔧 智能识别** - 自动单位转换（"100ms", "9600bps", "75.5%"）
- **📦 即插即用** - 12个完整示例，开箱即用
- **💡 协程调度** - 轻量级多任务，占用极少
- **🎨 变长参数** - 支持乱序输入和默认值

### 🛠️ 功能模块

| 模块 | 功能 | 特色 |
|------|------|------|
| 🔌 **IO控制** | GPIO配置 | 4种模式，可调驱动能力 |
| ⚡ **IO中断** | 独立中断 | 所有IO可独立中断 |
| ⏰ **定时器** | 6路定时器 | 智能单位识别 |
| 📡 **串口** | 4路UART | DMA收发，非阻塞 |
| 📊 **ADC** | 16通道采样 | 单次/循环模式 |
| 🌊 **PWM** | 8路PWM | 输入捕获，浮点占空比 |
| 🔗 **I2C** | I2C主机 | 指令串设计 |
| 📨 **SPI** | 3路SPI | DMA高速传输 |
| 💾 **EEPROM** | 数据存储 | 均衡磨损算法 |
| 🧵 **协程** | 多任务调度 | 轻量级，非阻塞 |

---

## 📁 项目结构

```
AI8051U专用库函数-2/
│
├── 📚 库函数文件/                # 核心库函数源码
│   ├── set_io.c/h               # IO控制
│   ├── io_int.c/h               # IO中断
│   ├── set_timer.c/h            # 定时器
│   ├── set_uart.c/h             # 串口通信
│   ├── set_adc.c/h              # ADC采样
│   ├── set_pwm.c/h              # PWM控制
│   ├── set_i2c.c/h              # I2C通信
│   ├── set_spi.c/h              # SPI通信
│   ├── set_eeprom.c/h           # EEPROM存储
│   └── set_task.c/h             # 协程调度器
│
├── 🎓 独立例程/                  # 12个完整应用示例
│   ├── 1设置IO部分/
│   ├── 2设置定时器及外部中断部分/
│   ├── 3设置串口部分/
│   ├── 4使用ADC部分/
│   ├── 5使用I2C部分/
│   ├── 6使用SPI部分/
│   ├── 7使用PWM部分/
│   ├── 8使用EEPROM部分/
│   ├── 9使用协程完成多任务调度/
│   ├── 10协程应用案例集锦/
│   ├── 11MPU6050陀螺仪示例/
│   └── 12AD9833DDS模块示例/
│
├── 📖 库函数使用说明/            # 官方PDF文档
│   ├── AI8051U专属库函数使用文档.pdf
│   ├── 教程-如何建立一个项目.pdf
│   └── Keil中断向量号拓展插件/
│
├── 📝 文档/                     # 新增文档
│   ├── AI8051U库函数使用手册.md  # 详细使用手册
│   ├── API快速参考.md           # API速查手册
│   └── README.md               # 本文件
│
└── 库函数更新记录.txt            # 版本历史

```

---

## 🚀 快速开始

### 第一步：创建项目

1. 在Keil中创建新项目
2. 选择芯片型号（STC AI8051U系列）
3. 添加源文件：
   ```
   main.c
   AI8051U.H
   set_io.c / set_io.h
   ```

### 第二步：自动配置

运行项目文件夹中的 `Auto_Keil.exe`，自动优化工程配置

### 第三步：编写代码

```c
#include "AI8051U.h"
#include "set_io.h"
#include "set_timer.h"

void main(void)
{
    EAXFR = 1;  WTST = 0;  CKCON = 0;
    
    // 配置LED引脚
    set_io_mode(pp_mode, Pin00, Pin_End);
    
    // 配置100ms定时器
    set_timer_mode(Timer0, "100ms", Timer_End);
    
    EA = 1;  // 开启中断
    
    while(1)
    {
        if(get_timer_state(Timer0))
        {
            P00 = ~P00;  // LED闪烁
        }
    }
}
```

### 第四步：编译下载

使用STC-ISP工具下载到单片机

---

## 📖 文档导航

### 📚 新手入门
1. 📄 [AI8051U库函数使用手册.md](AI8051U库函数使用手册.md) - **完整教程**
   - 快速开始指南
   - 10个模块详细说明
   - 100+代码示例
   - 常见问题解答

2. 📋 [API快速参考.md](API快速参考.md) - **速查手册**
   - 所有API函数原型
   - 参数速查表
   - 常用代码片段
   - 错误排查清单

3. 📘 [AI8051U专属库函数使用文档.pdf](库函数使用说明/AI8051U专属库函数使用文档.pdf) - **官方PDF**

### 🎓 示例代码
- `独立例程/` 目录包含12个完整项目
- 每个例程都有详细注释和说明

### 📝 其他文档
- [库函数更新记录.txt](库函数更新记录.txt) - 版本历史
- [教程-如何建立一个项目.pdf](库函数使用说明/教程-如何建立一个项目.pdf) - 工程创建

---

## 💡 代码示例

### 示例1：串口通信

```c
#include "AI8051U.h"
#include "set_uart.h"

void main(void)
{
    EAXFR = 1; WTST = 0; CKCON = 0;
    
    set_uart_mode(Uart1, "115200bps", Uart_End);
    EA = 1;
    
    uart_printf(Uart1, "Hello, World!\r\n");
    
    while(1)
    {
        if(get_uart_state(Uart1))
        {
            uart_printf(Uart1, "Received: %s\r\n", _uart1_rx_buff);
        }
    }
}
```

### 示例2：PWM呼吸灯

```c
#include "AI8051U.h"
#include "set_io.h"
#include "set_pwm.h"

void main(void)
{
    EAXFR = 1; WTST = 0; CKCON = 0;
    
    set_io_mode(pp_mode, Pin00, Pin_End);
    set_pwm_mode(Pwm1, Pwm1_P00_01, "1khz", "0%", Pwm_End);
    EA = 1;
    
    float duty = 0;
    char dir = 1;
    
    while(1)
    {
        set_pwm_duty(Pwm1, duty);
        delay_ms(20);
        
        if(dir) duty += 1;
        else duty -= 1;
        
        if(duty >= 100) dir = 0;
        if(duty <= 0) dir = 1;
    }
}
```

### 示例3：协程多任务

```c
#include "AI8051U.h"
#include "set_io.h"
#include "set_timer.h"
#include "set_task.h"

void main(void)
{
    EAXFR = 1; WTST = 0; CKCON = 0;
    
    set_io_mode(pp_mode, Pin00, Pin01, Pin02, Pin_End);
    set_timer_mode(Timer0, "1ms", Timer_End);
    set_timer_isr(Timer0, set_task_mode);
    EA = 1;
    
    while(1)
    {
        // 任务1：100ms闪烁
        task_start(0);
            P00 = ~P00;
            task_delay(100);
        task_end(1);
        
        // 任务2：200ms亮，500ms灭
        task_start(1);
            P01 = 0;
            task_delay(200);
            P01 = 1;
            task_delay(500);
        task_end(1);
        
        // 任务3：1秒延时后快闪
        task_start(2);
            task_delay(1000);
            P02 = ~P02;
            task_delay(50);
        task_end(1);
    }
}
```

### 示例4：I2C读取MPU6050

```c
#include "AI8051U.h"
#include "set_i2c.h"
#include "set_uart.h"

int AccX, AccY, AccZ;

void main(void)
{
    EAXFR = 1; WTST = 0; CKCON = 0;
    
    set_uart_mode(Uart1, Uart_End);
    set_i2c_mode(I2c0, "400khz", I2c_End);
    
    // 初始化MPU6050
    set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, 
                Tx_Rack, 0x6b, Tx_Rack, 0x00, 
                Stop, Cmd_End);
    EA = 1;
    
    while(1)
    {
        // 读取加速度
        set_i2c_cmd(I2c0, 0, S_Tx_Rack, 0xd0, Tx_Rack, 0x3b,
                    S_Tx_Rack, 0xd1,
                    Rx_Tack, (char*)&AccX, Rx_Tack, ((char*)&AccX)+1,
                    Rx_Tack, (char*)&AccY, Rx_Tack, ((char*)&AccY)+1,
                    Rx_Tack, (char*)&AccZ, Rx_Tnak, ((char*)&AccZ)+1,
                    Stop, Cmd_End);
        
        if(get_i2c_state(I2c0, 0))
        {
            uart_printf(Uart1, "Acc: %d, %d, %d\r\n", AccX, AccY, AccZ);
            delay_ms(100);
        }
    }
}
```

---

## 🎯 应用场景

### 🏭 工业控制
- 电机驱动（PWM）
- 传感器采集（ADC、I2C、SPI）
- 通信协议（Modbus、RS485）

### 🏠 智能家居
- 环境监测（温湿度、光照）
- 家电控制（红外、继电器）
- 无线通信（WiFi、蓝牙模块）

### 🚗 汽车电子
- CAN总线通信
- 车载传感器
- 仪表显示

### 🎓 教学实验
- 单片机原理
- 嵌入式开发
- 毕业设计

---

## 🔧 工具支持

### Auto_Keil.exe
- 自动配置工程选项
- 优化代码体积（减少269%）
- 移除未使用的IO组

### Keil中断向量号拓展插件
- 支持扩展中断号
- 简化中断函数定义

### USB-CDC调试
- 无需外置串口
- 即插即用
- 支持printf重定向

---

## 📊 性能指标

### 代码体积
| 功能 | CODE占用 | 优化后 |
|------|---------|--------|
| 基础IO | 6246字节 | 2320字节 |
| 串口通信 | ~4200字节 | - |
| 协程调度 | ~1800字节 | - |
| 完整项目 | ~15KB | - |

### 资源占用
| 资源 | 数量 | 说明 |
|------|------|------|
| 协程 | 6字节/个 | 最多10个 |
| 串口缓冲区 | 120字节 | Uart1 |
| ADC结果 | 32字节 | 16通道×2字节 |

---

## 🆚 对比传统开发

| 特性 | 传统寄存器 | 本库函数 |
|------|-----------|---------|
| **学习曲线** | 陡峭 | 平缓 |
| **代码行数** | 50+ | 1行 |
| **可读性** | 差 | 优秀 |
| **可维护性** | 低 | 高 |
| **开发效率** | 慢 | 快10倍+ |
| **错误率** | 高 | 低 |

### 示例对比

**传统方式配置串口（50+行）**：
```c
// 大量寄存器配置...
SCON = 0x50;
AUXR |= 0x01;
AUXR &= ~0x02;
T2L = 0xE0;
T2H = 0xFE;
AUXR |= 0x10;
// ... 更多配置
```

**使用本库（1行）**：
```c
set_uart_mode(Uart1, "9600bps", Uart_End);
```

---

## 🌟 核心特性详解

### 1. 变长参数设计

```c
// 支持任意数量参数
set_io_mode(pp_mode, Pin00, Pin01, Pin02, Pin03, Pin_End);

// 支持乱序输入
set_uart_mode(Uart1, Uart1_P36_7, "9600bps", Uart_End);
set_uart_mode(Uart1, "9600bps", Uart1_P36_7, Uart_End);  // 等效
```

### 2. 智能单位识别

```c
set_timer_mode(Timer0, "100ms", Timer_End);   // 毫秒
set_timer_mode(Timer0, "1.5s", Timer_End);    // 秒
set_timer_mode(Timer0, "50hz", Timer_End);    // 频率

set_uart_mode(Uart1, "9600bps", Uart_End);    // 波特率

set_pwm_mode(Pwm1, "10khz", "75.5%", Pwm_End); // 频率+占空比
```

### 3. 默认值机制

```c
// 全默认
set_uart_mode(Uart1, Uart_End);  
// = 115200bps, 64byte, Timer2, P30/P31

// 部分自定义
set_uart_mode(Uart1, "9600bps", Uart_End);
// 其他参数使用默认值
```

### 4. 协程调度器

**特点**：
- ✅ 轻量级（仅6字节/协程）
- ✅ 非阻塞（不影响其他任务）
- ✅ 易扩展（可增加协程数量）
- ✅ 类C语法（学习成本低）

**支持功能**：
- 延时 (`task_delay`)
- 条件等待 (`task_wait`)
- for循环 (`task_for` + `task_break`)
- while循环 (`task_while` + `task_break`)
- 嵌套循环

---

## 📚 学习路径

### 初级（1-3天）
1. 阅读 [快速开始](#快速开始)
2. 运行 `独立例程1` - IO控制
3. 运行 `独立例程2` - 定时器
4. 运行 `独立例程3` - 串口通信

### 中级（1周）
5. 学习 [API快速参考.md](API快速参考.md)
6. 运行 `独立例程4-8` - 外设控制
7. 理解非阻塞设计思想
8. 尝试修改示例参数

### 高级（2周）
9. 深入 [AI8051U库函数使用手册.md](AI8051U库函数使用手册.md)
10. 运行 `独立例程9-12` - 高级应用
11. 研究协程源码实现
12. 移植库函数到自己项目

### 专家（1个月+）
13. 优化自己的应用
14. 扩展库函数功能
15. 参与开源贡献

---

## ❓ 常见问题

### Q: 支持哪些芯片？
**A**: STC AI8051U 系列全系列，包括：AI8051U-P40S、AI8051U-P48S等

### Q: 可以用于商业项目吗？
**A**: 可以，本库为开源项目

### Q: 如何获取技术支持？
**A**: 
1. 查看 [AI8051U库函数使用手册.md](AI8051U库函数使用手册.md)
2. 参考 12个独立例程
3. 查看官方PDF文档

### Q: 库函数会持续更新吗？
**A**: 是的，查看 [库函数更新记录.txt](库函数更新记录.txt) 了解更新历史

### Q: 如何贡献代码？
**A**: 欢迎提交Pull Request或Issue

---

## 📦 相关资源

### 官方资源
- [STC官网](https://www.stcai.com/)
- [Keil官网](https://www.keil.com/)
- [STC-ISP下载工具](https://www.stcai.com/rjxz.html)

### 学习资源
- 📖 单片机C语言教程
- 🎥 嵌入式开发视频
- 💻 在线编程练习

---

## 📈 版本历史

### V2.0 (2025-05)
- ✨ 新增AD9833 DDS模块示例
- ✨ 新增MPU6050陀螺仪示例
- 🐛 修复多项bug

### V1.9 (2025-04)
- ✨ 协程增强，支持while循环
- 🐛 修复时钟自动获取bug

### V1.8 (2025-03)
- ✨ 首次发布协程库
- 🎉 发布Auto_Keil工具

### V1.5 (2025-02)
- ✨ PWM支持输入捕获
- ✨ I2C指令串功能

### V1.0 (2025-01)
- 🎉 项目创建
- ✨ 基础模块完成

[查看完整更新记录](库函数更新记录.txt)

---

## 🙏 致谢

感谢所有为本项目做出贡献的开发者！

---

## 📧 联系方式

- 📁 项目地址：本仓库
- 📖 文档：见本目录
- 🐛 反馈问题：提交Issue

---

## ⭐ Star History

如果这个项目对你有帮助，请给个Star ⭐ 支持一下！

---

<div align="center">

**Made with ❤️ for AI8051U Community**

[⬆ 回到顶部](#ai8051u-专用库函数)

</div>

