# StateMachine - 状态机辅助工具

## 📋 工具简介

**StateMachine**是一个轻量级状态机辅助工具，通过宏定义简化状态机编写。

### 核心特性

- ✅ **纯宏定义** - 无函数调用，0开销
- ✅ **极轻量** - Flash占用0字节
- ✅ **易使用** - 简化状态机编写
- ✅ **可选用** - 不强制使用
- ✅ **跨平台** - 适用于任何C51项目

---

## 🚀 快速使用

### 基础示例

```c
#include "StateMachine/StateMachine.h"

void Sample_MySensor(void)  // 假设10ms周期
{
    static u8 state = 0;
    static u32 timestamp = 0;
    
    switch(state)
    {
        case 0:  // 初始化传感器
            sensor_init();
            SM_DELAY_START(timestamp);  // 开始计时
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:  // 等待100ms稳定
            if(SM_DELAY_CHECK(timestamp, 100))
            {
                SM_GOTO_STATE(state, 2);
            }
            break;
            
        case 2:  // 读取数据
            data = sensor_read();
            SM_GOTO_STATE(state, 0);
            break;
    }
}
```

**就这么简单！** ✅

---

## 📚 API参考

### 时间延迟API

| API | 说明 | 示例 |
|-----|------|------|
| `SM_GET_TICK()` | 获取当前时间戳 | `ts = SM_GET_TICK();` |
| `SM_DELAY_START(ts)` | 开始延时 | `SM_DELAY_START(timestamp);` |
| `SM_DELAY_CHECK(ts, ms)` | 检查延时是否到期 | `if(SM_DELAY_CHECK(ts, 100))` |
| `SM_ELAPSED(ts)` | 计算已过时间 | `elapsed = SM_ELAPSED(ts);` |

### 状态转换API

| API | 说明 | 示例 |
|-----|------|------|
| `SM_GOTO_STATE(s, new)` | 跳转到新状态 | `SM_GOTO_STATE(state, 2);` |
| `SM_GOTO_STATE_WITH_DELAY(s, new, ts)` | 跳转并开始计时 | `SM_GOTO_STATE_WITH_DELAY(state, 1, ts);` |

### 超时检测API

| API | 说明 | 示例 |
|-----|------|------|
| `SM_TIMEOUT(ts, ms)` | 检测是否超时 | `if(SM_TIMEOUT(ts, 500))` |

### 计数器延时API（适合粗延时）

| API | 说明 | 示例 |
|-----|------|------|
| `SM_COUNT_DELAY_START(cnt)` | 重置计数器 | `SM_COUNT_DELAY_START(count);` |
| `SM_COUNT_DELAY_CHECK(cnt, max)` | 检查计数 | `if(SM_COUNT_DELAY_CHECK(count, 10))` |

---

## 💡 使用场景

### 场景1：I2C传感器读取

```c
void Sample_I2C_Sensor(void)  // 10ms周期
{
    static u8 state = 0;
    static u32 ts = 0;
    
    switch(state)
    {
        case 0:  // 发送启动命令
            I2C_Start();
            I2C_SendByte(SENSOR_ADDR);
            I2C_SendByte(CMD_START);
            I2C_Stop();
            
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:  // 等待20ms转换完成
            if(SM_DELAY_CHECK(ts, 20))
            {
                SM_GOTO_STATE(state, 2);
            }
            break;
            
        case 2:  // 读取数据
            I2C_Start();
            I2C_SendByte(SENSOR_ADDR | 0x01);
            data = I2C_ReceiveByte();
            I2C_Stop();
            
            SM_GOTO_STATE(state, 0);
            break;
    }
}
```

---

### 场景2：带超时的通信

```c
void Sample_UART_Command(void)  // 10ms周期
{
    static u8 state = 0;
    static u32 ts = 0;
    static bit timeout = 0;
    
    switch(state)
    {
        case 0:  // 发送命令
            UART_SendCommand(cmd);
            SM_DELAY_START(ts);
            timeout = 0;
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:  // 等待响应（最长500ms）
            if(UART_DataReady())
            {
                SM_GOTO_STATE(state, 2);  // 收到响应
            }
            else if(SM_TIMEOUT(ts, 500))
            {
                timeout = 1;
                SM_GOTO_STATE(state, 3);  // 超时处理
            }
            break;
            
        case 2:  // 正常处理
            data = UART_ReadData();
            printf("Data: %u\r\n", data);
            SM_GOTO_STATE(state, 0);
            break;
            
        case 3:  // 超时处理
            printf("Timeout!\r\n");
            SM_GOTO_STATE(state, 0);
            break;
    }
}
```

---

### 场景3：多步骤初始化

```c
void Sample_Complex_Init(void)  // 10ms周期
{
    static u8 state = 0;
    static u32 ts = 0;
    
    switch(state)
    {
        case 0:  // 步骤1：上电
            power_on();
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:  // 等待50ms稳定
            if(SM_DELAY_CHECK(ts, 50))
            {
                SM_GOTO_STATE_WITH_DELAY(state, 2, ts);
            }
            break;
            
        case 2:  // 步骤2：配置寄存器
            configure_registers();
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 3);
            break;
            
        case 3:  // 等待10ms
            if(SM_DELAY_CHECK(ts, 10))
            {
                SM_GOTO_STATE_WITH_DELAY(state, 4, ts);
            }
            break;
            
        case 4:  // 步骤3：启动
            start_device();
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 5);
            break;
            
        case 5:  // 等待100ms
            if(SM_DELAY_CHECK(ts, 100))
            {
                SM_GOTO_STATE(state, 6);
            }
            break;
            
        case 6:  // 完成
            printf("Init completed\r\n");
            SM_GOTO_STATE(state, 0);
            break;
    }
}
```

---

## 📊 性能分析

### Flash占用

```
StateMachine.h: 0字节（仅宏定义）

使用状态机的任务:
  - 与手写状态机相同
  - 宏展开后编译为内联代码
  - 无函数调用开销

结论: Flash增加0字节 ✅✅✅
```

### RAM占用

```
每个状态机任务需要:
  - state变量: 1字节
  - timestamp变量: 4字节（如果使用时间延时）
  - 其他局部变量: 按需

示例任务:
  static u8 state = 0;       // 1字节
  static u32 timestamp = 0;  // 4字节
  
总计: 5字节/任务

10个状态机任务: 50字节 (2.4%)
```

### CPU开销

```
宏展开后的代码:
  SM_DELAY_START(ts):
    → timestamp = system_tick_ms;
    开销: 3条指令 ≈ 0.06μs @ 48MHz
    
  SM_DELAY_CHECK(ts, 100):
    → (system_tick_ms - timestamp) >= 100
    开销: 5条指令 ≈ 0.10μs @ 48MHz
    
  SM_GOTO_STATE(state, 2):
    → state = 2;
    开销: 1条指令 ≈ 0.02μs @ 48MHz

总开销: 与手写代码完全相同 ✅
无额外函数调用开销 ✅
```

---

## 🎯 对比分析

### 手写状态机 vs 使用辅助工具

| 特性 | 手写 | 使用StateMachine | 优势 |
|------|------|-----------------|------|
| Flash | 基准 | 相同 | - |
| RAM | 基准 | 相同 | - |
| CPU | 基准 | 相同 | - |
| 代码可读性 | ⭐⭐ | ⭐⭐⭐⭐⭐ | 宏名称清晰 |
| 编写速度 | ⭐⭐ | ⭐⭐⭐⭐⭐ | 快2倍 |
| 维护性 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 易于理解 |
| 调试性 | ⭐⭐ | ⭐⭐⭐⭐ | 调试宏支持 |

---

## 🔧 集成方法

### 方式1：直接包含（推荐）

```c
// 在需要状态机的任务文件中
#include "StateMachine/StateMachine.h"

void My_Complex_Task(void)
{
    static u8 state = 0;
    static u32 ts = 0;
    
    switch(state)
    {
        case 0:
            // 使用StateMachine宏
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:
            if(SM_DELAY_CHECK(ts, 100))
            {
                SM_GOTO_STATE(state, 2);
            }
            break;
    }
}
```

**无需修改项目配置，直接使用！**

---

### 方式2：全局包含（可选）

```c
// 在app.h中添加
#include "StateMachine/StateMachine.h"

// 所有任务文件自动可用StateMachine宏
```

---

## 💡 高级特性

### 状态机调试

```c
// 启用调试
#define STATE_MACHINE_DEBUG

#include "StateMachine/StateMachine.h"

void MyTask(void)
{
    switch(state)
    {
        case 0:
            SM_DEBUG_STATE(STATE_INIT);  // 打印: [SM] State: STATE_INIT
            init();
            SM_DEBUG_DELAY(100);         // 打印: [SM] Delay: 100ms
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:
            if(SM_TIMEOUT(ts, 500))
            {
                SM_DEBUG_TIMEOUT();      // 打印: [SM] Timeout!
                SM_GOTO_STATE(state, 2);
            }
            break;
    }
}
```

---

### 状态枚举（可选）

```c
// 使用枚举代替数字，代码更清晰
SM_STATE_ENUM(MySensor) {
    STATE_IDLE = 0,
    STATE_INIT,
    STATE_WAIT_STABLE,
    STATE_READ_DATA,
    STATE_PROCESS,
    STATE_ERROR
};

void Sample_Sensor(void)
{
    static u8 state = STATE_IDLE;  // 使用枚举名
    static u32 ts = 0;
    
    switch(state)
    {
        case STATE_IDLE:
            // ...
            SM_GOTO_STATE(state, STATE_INIT);
            break;
            
        case STATE_INIT:
            // ...
            SM_GOTO_STATE(state, STATE_WAIT_STABLE);
            break;
            
        // ... 代码更易读
    }
}
```

---

## 📊 实际应用案例

### 案例1：WS2812驱动（需要精确延时）

```c
void Sample_WS2812(void)  // 20ms周期
{
    static u8 state = 0;
    static u32 ts_us = 0;  // 需要微秒级（如果有）
    
    switch(state)
    {
        case 0:  // 发送RGB数据
            WS2812_SendData(rgb_buffer, led_count);
            SM_DELAY_START(ts);  // 使用ms级即可
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:  // 等待复位时序（>50μs）
            if(SM_DELAY_CHECK(ts, 1))  // 1ms足够了
            {
                SM_GOTO_STATE(state, 0);  // 可发送下一帧
            }
            break;
    }
}
```

---

### 案例2：协议处理

```c
void Sample_Protocol_Handler(void)  // 10ms周期
{
    static u8 state = 0;
    static u32 ts = 0;
    static u8 retry_count = 0;
    
    switch(state)
    {
        case 0:  // 空闲，等待命令
            if(UART_DataAvailable())
            {
                cmd = UART_ReadByte();
                retry_count = 0;
                SM_GOTO_STATE_WITH_DELAY(state, 1, ts);
            }
            break;
            
        case 1:  // 处理命令
            ProcessCommand(cmd);
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 2);
            break;
            
        case 2:  // 等待100ms
            if(SM_DELAY_CHECK(ts, 100))
            {
                SM_GOTO_STATE_WITH_DELAY(state, 3, ts);
            }
            break;
            
        case 3:  // 发送响应
            UART_SendResponse(response);
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 4);
            break;
            
        case 4:  // 等待确认（超时500ms）
            if(UART_AckReceived())
            {
                printf("Success!\r\n");
                SM_GOTO_STATE(state, 0);
            }
            else if(SM_TIMEOUT(ts, 500))
            {
                if(++retry_count < 3)
                {
                    printf("Retry %u\r\n", retry_count);
                    SM_GOTO_STATE(state, 3);  // 重试
                }
                else
                {
                    printf("Failed!\r\n");
                    SM_GOTO_STATE(state, 0);  // 放弃
                }
            }
            break;
    }
}
```

---

## 🔍 对比优化前后

### 传统阻塞式（❌ 不推荐）

```c
void Bad_Task(void)
{
    sensor_init();
    delay_ms(100);  // ❌ 阻塞100ms，其他任务无法执行
    data = sensor_read();
    delay_ms(50);   // ❌ 又阻塞50ms
    process(data);
    
    // 总阻塞时间：150ms
    // 影响：所有任务延迟150ms
}
```

### 手写状态机（✅ 可用但繁琐）

```c
void Manual_StateMachine(void)
{
    static u8 state = 0;
    static u32 timestamp = 0;
    
    switch(state)
    {
        case 0:
            sensor_init();
            timestamp = system_tick_ms;  // 手写
            state = 1;  // 手写
            break;
            
        case 1:
            if((system_tick_ms - timestamp) >= 100)  // 手写判断
            {
                state = 2;  // 手写
            }
            break;
            
        case 2:
            data = sensor_read();
            timestamp = system_tick_ms;  // 手写
            state = 3;  // 手写
            break;
            
        case 3:
            if((system_tick_ms - timestamp) >= 50)  // 手写判断
            {
                state = 4;  // 手写
            }
            break;
            
        case 4:
            process(data);
            state = 0;  // 手写
            break;
    }
}
```

### 使用StateMachine工具（✅ 推荐）

```c
void Good_StateMachine(void)
{
    static u8 state = 0;
    static u32 ts = 0;
    
    switch(state)
    {
        case 0:
            sensor_init();
            SM_DELAY_START(ts);  // 清晰
            SM_GOTO_STATE(state, 1);
            break;
            
        case 1:
            if(SM_DELAY_CHECK(ts, 100))  // 清晰
            {
                SM_GOTO_STATE(state, 2);
            }
            break;
            
        case 2:
            data = sensor_read();
            SM_DELAY_START(ts);
            SM_GOTO_STATE(state, 3);
            break;
            
        case 3:
            if(SM_DELAY_CHECK(ts, 50))
            {
                SM_GOTO_STATE(state, 4);
            }
            break;
            
        case 4:
            process(data);
            SM_GOTO_STATE(state, 0);
            break;
    }
}
```

**代码对比：**
```
可读性: ⭐⭐⭐⭐⭐ (宏名称一目了然)
维护性: ⭐⭐⭐⭐⭐ (易于修改延时时间)
性能: 完全相同 ✅
Flash: 完全相同 ✅
```

---

## ✅ 总结

### StateMachine辅助工具优势

**1. 零开销**
```
Flash: 0字节（纯宏定义）
RAM: 与手写相同
CPU: 与手写相同

宏展开后与手写代码完全一样 ✅
```

**2. 极易用**
```
学习成本: 5分钟
使用方法: 包含头文件即可
集成难度: 0（无需集成，直接用）
```

**3. 可选性**
```
不需要: 不包含头文件
需要: #include即可使用
无任何强制 ✅
```

**4. 依赖少**
```
仅依赖:
  - system_tick_ms (已在intKey模块中定义)
  - 或者用计数器方式（无依赖）
```

---

**版本：** V1.0  
**类型：** 辅助工具（非强制框架）  
**推荐度：** ⭐⭐⭐⭐⭐  
**Flash占用：** 0字节！

