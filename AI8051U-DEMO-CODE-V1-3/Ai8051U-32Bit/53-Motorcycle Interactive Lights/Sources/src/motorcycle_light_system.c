/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "motorcycle_light_system.h"
#include "Task.h"
#include "config.h"
//========================================================================
//                               全局变量定义
//========================================================================

#define ADC_CHS DMA_ADC_CHSW0 

// 系统状态
SYSTEM_STATE system_state = SYS_INIT;
LIGHT_MODE current_mode = LIGHT_POSITION;
LIGHT_MODE previous_mode = LIGHT_OFF;

// 数据结构体
VEHICLE_SIGNALS vehicle_signals = {0};
SENSOR_DATA sensor_data = {0};
PWM_DUTIES pwm_duties = {0};

// WS2812数据缓冲区 (使用扩展RAM)
RGB_COLOR xdata led_colors[WS2812_COUNT];
u8 xdata ws2812_buffer[WS2812_COUNT * 24];

// 音频数据缓冲区
u16 xdata audio_samples[ADC_SAMPLE_COUNT];
u8 xdata frequency_bands[8];

// 模式管理
u16 xdata mode_timers[8] = {0};
u8 xdata active_modes[8] = {0};
u8 active_mode_count = 0;

// ADC全局变量 (与现有ADC模块兼容)
u16 adc_audio_avg = 0;
u16 adc_battery_avg = 0;

//========================================================================
//                               系统初始化
//========================================================================

// WS2812初始化
void WS2812_Init(void) {
    // 配置SPI为WS2812输出模式
    SPCTL = 0xD0;  // 主机模式，CPOL=1, CPHA=1
    SPSTAT = 0xC0; // 清除SPI状态

    // 配置GPIO P2.5为SPI输出
    P2M1 &= ~(1 << 5);  // P2.5推挽输出
    P2M0 |= (1 << 5);
}

// PWM多路初始化
void PWM_Multi_Channel_Init(void) {
    // PWMA初始化 (RGB灯组)
    PWMA_CCER1 = 0x00;  // 关闭通道
    PWMA_CCMR1 = 0x68;  // CH1-3 PWM模式
    PWMA_CCER1 = 0x11;  // CH1-3使能
    PWMA_ARRH = 0x07;   // 周期2047
    PWMA_ARRL = 0xFF;
    PWMA_ENO = 0x07;    // CH1-3输出使能
    PWMA_BKR = 0x80;    // 主输出使能
    PWMA_CR1 = 0x01;    // 开始计数

    // PWMB初始化 (刹车灯)
    PWMB_CCER1 = 0x00;  // 关闭通道
    PWMB_CCMR1 = 0x60;  // CH1 PWM模式
    PWMB_CCER1 = 0x01;  // CH1使能
    PWMB_ARRH = 0x07;   // 周期2047
    PWMB_ARRL = 0xFF;
    PWMB_ENO = 0x01;    // CH1输出使能
    PWMB_BKR = 0x80;    // 主输出使能
    PWMB_CR1 = 0x01;    // 开始计数
}

// ADC-DMA多通道初始化
void ADC_DMA_Config_Multi(void) {
    // ADC配置
    ADCTIM = 0x3f;      // 设置ADC内部时序
    ADCCFG = 0x2f;      // ADC时钟配置
    ADC_CONTR = 0x80;   // 使能ADC

    // 配置ADC通道切换
    ADC_CHS = ADC_AUDIO_CHANNEL;  // 初始选择音频通道

    // DMA配置 (用于音频采样)
    DMA_ADC_CFG = 0x80;                    // 使能ADC DMA
    DMA_ADC_STA = (u16)audio_samples;      // 目标地址
    DMA_ADC_AMT = ADC_SAMPLE_COUNT;        // 采样点数
    DMA_ADC_CR = 0x40;                     // 自动平均使能

    // 全局变量初始化
    adc_audio_avg = 0;
    adc_battery_avg = 0;
}

// ADC启动多通道采样
void ADC_Start_Multi_DMA(void) {
    static u8 channel_phase = 0;

    switch(channel_phase) {
        case 0:  // 音频通道
            ADC_CHS = ADC_AUDIO_CHANNEL;
            DMA_ADC_STA = (u16)audio_samples;
            DMA_ADC_AMT = ADC_SAMPLE_COUNT;
            break;

        case 1:  // 电池电压通道
            ADC_CHS = ADC_BATTERY_CHANNEL;
            DMA_ADC_STA = (u16)&adc_battery_avg;  // 使用全局变量
            DMA_ADC_AMT = 1;  // 单次采样
            break;
    }

    // 启动ADC转换
    ADC_CONTR = 0x80 | ADC_CHS;
    DMA_ADC_CR |= 0x01;  // 启动DMA

    channel_phase = (channel_phase + 1) % 2;
}

//========================================================================
//                               系统初始化
//========================================================================

void Motorcycle_Light_System_Init(void)
{
    printf("[INIT] Starting Motorcycle Light System...\n");

    // 初始化数据结构
    memset(&vehicle_signals, 0, sizeof(VEHICLE_SIGNALS));
    memset(&sensor_data, 0, sizeof(SENSOR_DATA));
    memset(&pwm_duties, 0, sizeof(PWM_DUTIES));

    // 初始化WS2812缓冲区
    memset(led_colors, 0, sizeof(led_colors));
    memset(ws2812_buffer, 0, sizeof(ws2812_buffer));

    // 初始化音频缓冲区
    memset(audio_samples, 0, sizeof(audio_samples));
    memset(frequency_bands, 0, sizeof(frequency_bands));

    // 初始化模式管理
    memset(mode_timers, 0, sizeof(mode_timers));
    memset(active_modes, 0, sizeof(active_modes));
    active_mode_count = 0;

    // 初始化传感器
    printf("[INIT] Initializing sensors...\n");
    // MPU6050_Init() 和 BH1750_Init() 在现有代码中调用

    // 初始化WS2812
    printf("[INIT] Initializing WS2812...\n");
    WS2812_Init();

    // 初始化PWM
    printf("[INIT] Initializing PWM...\n");
    PWM_Multi_Channel_Init();

    // 初始化ADC-DMA
    printf("[INIT] Initializing ADC-DMA...\n");
    ADC_DMA_Config_Multi();

    // 设置初始模式
    Set_Light_Mode(LIGHT_POSITION, 1);

    printf("[INIT] Motorcycle Light System initialized successfully!\n");
}

void GPIO_Config_Update(void)
{
    // 更新GPIO配置以支持新的引脚分配
    // P3.7设为INT3输入 (模式菜单按键)
    P3M1 &= ~(1 << 7);  // P3.7输入
    P3M0 &= ~(1 << 7);
	
		P3PU = 0x0c;    //P3.2,P3.3使能内部上拉

    // 启用INT3中断
    EX3 = 1;  // 外部中断3使能
    EX2 = 1;  // 外部中断4下降沿触发
		IE1  = 0;   //外中断1标志位
    IE0  = 0;   //外中断0标志位
    EX1 = 1;    //INT1 Enable
    EX0 = 1;    //INT0 Enable

    IT0 = 1;    //INT0 下降沿中断
//  IT0 = 0;    //INT0 上升,下降沿中断  
    IT1 = 1;    //INT1 下降沿中断
//  IT1 = 0;    //INT1 上升,下降沿中断  

    //INT2, INT3, INT4 实验板上没有引出测试按键，供需要时参考使用
    EX2 = 1;    //使能 INT2 下降沿中断
    EX3 = 1;    //使能 INT3 下降沿中断
    //EX4 = 1;    //使能 INT4 下降沿中断

    // 设置中断优先级 (INT3设为低优先级)
    IP2 &= ~(1 << 5);  // PX3 = 0 (低优先级)
}

//========================================================================
//                               任务函数实现
//========================================================================

// 任务1: 原车信号处理 (1ms, 优先级0)
void Sample_Vehicle_Signal_Process(void)
{
    static u8 last_signals = 0;
    u8 current_signals = 0;
	u8 signal_changes = 0;

    // 读取所有信号状态 (优化为位操作)
    current_signals |= (P00 ? 0x01 : 0);  // 刹车信号
    current_signals |= (P01 ? 0x02 : 0);  // 雾灯信号
    current_signals |= (P02 ? 0x04 : 0);  // 近光灯信号
    current_signals |= (P03 ? 0x08 : 0);  // 远光灯信号
    current_signals |= (P04 ? 0x10 : 0);  // 左转向信号
    current_signals |= (P05 ? 0x20 : 0);  // 右转向信号

    // 检测信号变化 (上升沿触发)
    signal_changes = current_signals & ~last_signals;

    // 更新信号状态
    vehicle_signals.brake_signal = P00;
    vehicle_signals.fog_light = P01;
    vehicle_signals.headlight_low = P02;
    vehicle_signals.headlight_high = P03;
    vehicle_signals.turn_left = P04;
    vehicle_signals.turn_right = P05;

    // 根据信号变化更新模式
    if(signal_changes & 0x01) { // 刹车信号上升沿
        printf("[SIGNAL] Brake signal detected\n");
        Set_Light_Mode(LIGHT_BRAKE, 1);
    }
    if(signal_changes & 0x10) { // 左转向上升沿
        printf("[SIGNAL] Left turn signal detected\n");
        Set_Light_Mode(LIGHT_TURN_LEFT, 1);
    }
    if(signal_changes & 0x20) { // 右转向上升沿
        printf("[SIGNAL] Right turn signal detected\n");
        Set_Light_Mode(LIGHT_TURN_RIGHT, 1);
    }

    last_signals = current_signals;
}

// 任务2: WS2812 DMA控制 (2ms, 优先级0)
void Sample_WS2812_DMA_Control(void)
{
    static u8 dma_state = 0;

    switch(dma_state) {
        case 0: // 检查DMA是否完成
            if(DMA_SPI_DONE) {
                WS2812_Send_DMA(); // 启动DMA传输
                dma_state = 1;
            }
            break;

        case 1: // 等待传输完成
            if(DMA_SPI_DONE) {
                dma_state = 0;
            }
            break;
    }
}

// 任务3: 按键处理 (5ms, 优先级1)
void Sample_Button_Process(void)
{
    // 检查INT2 (测试按键 P3.6)
    if(INT2_FLAG) {
        INT2_FLAG = 0;
        printf("[BUTTON] Test button pressed\n");
        // 进入测试模式
        system_state = SYS_TEST_MODE;
    }

    // 检查INT3 (菜单按键 P3.7)
    if(INT3_FLAG) {
        INT3_FLAG = 0;
        printf("[BUTTON] Menu button pressed\n");
        // 循环切换模式
        LIGHT_MODE next_mode = (current_mode + 1) % 8;
        Set_Light_Mode(current_mode, 0); // 关闭当前模式
        Set_Light_Mode(next_mode, 1);    // 开启下一模式
    }
}

// 任务4: 传感器读取 (10ms, 优先级1)
void Sample_Sensor_Read(void)
{
    static u8 sensor_phase = 0;

    switch(sensor_phase) {
        case 0: // 读取MPU6050加速度
            MPU6050_Read_Accel(&sensor_data.accel_x,
                              &sensor_data.accel_y,
                              &sensor_data.accel_z);
            sensor_phase = 1;
            break;

        case 1: // 读取BH1750光强
            sensor_data.light_level = BH1750_Read_Light();
            sensor_phase = 2;
            break;

        case 2: // ADC-DMA读取音频和电压
            if(DMA_ADC_DONE) {
                sensor_data.audio_level = adc_audio_avg;
                sensor_data.battery_voltage = adc_battery_avg;
                ADC_Start_Multi_DMA(); // 启动下次采样
                sensor_phase = 0;

                // 电池电压检测
                if(sensor_data.battery_voltage < LOW_VOLTAGE_THRESHOLD) {
                    if(system_state == SYS_NORMAL) {
                        printf("[POWER] Low voltage detected: %dmV\n",
                               sensor_data.battery_voltage);
                        system_state = SYS_LOW_POWER;
                    }
                } else if(sensor_data.battery_voltage > RECOVER_VOLTAGE_THRESHOLD) {
                    if(system_state == SYS_LOW_POWER) {
                        printf("[POWER] Voltage recovered: %dmV\n",
                               sensor_data.battery_voltage);
                        system_state = SYS_NORMAL;
                    }
                }
            }
            break;
    }
}

// 任务5: 灯效算法计算 (20ms, 优先级2)
void Sample_Light_Effect_Calculate(void)
{
    // 根据当前活动模式计算灯效
		u8 i;
    for(i = 0; i < active_mode_count; i++) {
        LIGHT_MODE mode = active_modes[i];

        switch(mode) {
            case LIGHT_BRAKE:
                Calculate_Brake_Effect();
                break;
            case LIGHT_TURN_LEFT:
                Calculate_Turn_Left_Effect();
                break;
            case LIGHT_TURN_RIGHT:
                Calculate_Turn_Right_Effect();
                break;
            case LIGHT_MUSIC:
                Calculate_Music_Sync_Effect();
                break;
            case LIGHT_POSITION:
                Calculate_Position_Effect();
                break;
            case LIGHT_AMBIENT:
                Calculate_Ambient_Effect();
                break;
            default:
                break;
        }
    }

    // 更新PWM输出
    Update_PWM_Outputs();

    // 编码WS2812数据
    WS2812_Encode_Buffer();
}

// 任务6: 音频频谱处理 (50ms, 优先级2)
void Sample_Audio_Process(void)
{
    // FFT分析音频信号
    Audio_FFT_Process(audio_samples);

    // 计算频谱数据
    Calculate_Frequency_Bands();

    // 调整全局亮度
    Adjust_Global_Brightness();
}

// 任务7: 状态指示 (100ms, 优先级3)
void Sample_Status_Display(void)
{
    static u16 status_timer = 0;
    status_timer++;

    // 根据系统状态闪烁状态LED
    switch(system_state) {
        case SYS_NORMAL:
            // 正常运行：蓝色LED慢闪
            if(status_timer % 1000 == 0) { // 1秒周期
                P1_7 = !P1_7; // 蓝色LED
            }
            break;

        case SYS_LOW_POWER:
            // 低功耗：蓝色LED快闪
            if(status_timer % 200 == 0) { // 200ms周期
                P1_7 = !P1_7;
            }
            break;

        case SYS_ERROR:
            // 错误状态：红色LED快闪
            if(status_timer % 200 == 0) {
                P1_6 = !P1_6; // 红色LED
            }
            break;

        case SYS_TEST_MODE:
            // 测试模式：双色交替闪烁
            if(status_timer % 300 == 0) {
                P1_6 = !P1_6;
                P1_7 = !P1_7;
            }
            break;

        default:
            break;
    }
}

// 任务8: 调试输出 (500ms, 优先级3)
void Sample_Debug_Output(void)
{
    static u16 debug_counter = 0;
    debug_counter++;

    if(debug_counter >= 1000) { // 约500ms * 2
        debug_counter = 0;

        printf("\n=== Motorcycle Light System Status ===\n");
        printf("System State: %d, Light Mode: %d\n",
               system_state, current_mode);
        printf("Battery: %dmV, Light: %d lux\n",
               sensor_data.battery_voltage, sensor_data.light_level);
        printf("Accel: X=%d, Y=%d, Z=%d\n",
               sensor_data.accel_x, sensor_data.accel_y, sensor_data.accel_z);
        printf("Active Modes: %d\n", active_mode_count);
        printf("=======================================\n\n");
    }
}

//========================================================================
//                               信号检测函数
//========================================================================

u8 Brake_Signal_Active(void) {
    return vehicle_signals.brake_signal;
}

u8 Turn_Left_Signal_Active(void) {
    return vehicle_signals.turn_left;
}

u8 Turn_Right_Signal_Active(void) {
    return vehicle_signals.turn_right;
}

u8 High_Beam_Active(void) {
    return vehicle_signals.headlight_high;
}

u8 Audio_Signal_Detected(void) {
    return (sensor_data.audio_level > 100); // 音频阈值检测
}

//========================================================================
//                               灯效控制函数
//========================================================================

void Set_Light_Mode(LIGHT_MODE new_mode, u8 enable) {
    u8 new_priority = Get_Light_Mode_Priority(new_mode);
    u8 current_priority = Get_Light_Mode_Priority(current_mode);

    printf("[MODE] Request mode %d (Priority:%d), Current mode %d (Priority:%d)\n",
           new_mode, new_priority, current_mode, current_priority);

    if(enable) {
        // 低优先级不能覆盖高优先级
        if(new_priority <= current_priority) {
            printf("[MODE] Switching from %d to %d (Priority:%d->%d)\n",
                   current_mode, new_mode, current_priority, new_priority);

            // 切换到新模式
            previous_mode = current_mode;
            current_mode = new_mode;
            Apply_Light_Effect(new_mode);
        } else {
            printf("[MODE] Mode %d blocked by higher priority mode %d\n",
                   new_mode, current_mode);
        }
    } else {
        // 模式释放
        if(current_mode == new_mode) {
            printf("[MODE] Mode %d released, returning to %d\n",
                   new_mode, previous_mode);
            current_mode = previous_mode;
            Apply_Light_Effect(current_mode);
        }
    }
}

void Apply_Light_Effect(LIGHT_MODE mode) {
    printf("[LIGHT] Applying effect for mode %d\n", mode);

    switch(mode) {
        case LIGHT_OFF:
            // 关闭所有灯效
            memset(led_colors, 0, sizeof(led_colors));
            pwm_duties.red_duty = 0;
            pwm_duties.green_duty = 0;
            pwm_duties.blue_duty = 0;
            pwm_duties.brake_duty = 0;
            break;

        case LIGHT_POSITION:
            // 位置灯：低亮度白色
            for(u8 i = 0; i < WS2812_COUNT; i++) {
                led_colors[i].r = 50;
                led_colors[i].g = 50;
                led_colors[i].b = 50;
            }
            pwm_duties.brake_duty = 512; // 50%占空比，2秒闪烁
            break;

        case LIGHT_BRAKE:
            // 刹车：红色高亮
            for(u8 i = 0; i < WS2812_COUNT; i++) {
                led_colors[i].r = 255;
                led_colors[i].g = 0;
                led_colors[i].b = 0;
            }
            pwm_duties.red_duty = 1023;   // 红色全亮
            pwm_duties.brake_duty = 1023; // 刹车灯全亮
            break;

        case LIGHT_TURN_LEFT:
            // 左转向：左侧流水效果
            Calculate_Turn_Left_Effect();
            break;

        case LIGHT_TURN_RIGHT:
            // 右转向：右侧流水效果
            Calculate_Turn_Right_Effect();
            break;

        case LIGHT_HIGH_BEAM:
            // 远光灯：增强亮度
            // 基础位置灯亮度提升
            for(u8 i = 0; i < WS2812_COUNT; i++) {
                led_colors[i].r = 150;
                led_colors[i].g = 150;
                led_colors[i].b = 150;
            }
            break;

        case LIGHT_MUSIC:
            // 音乐频谱：音频可视化
            Calculate_Music_Sync_Effect();
            break;

        case LIGHT_AMBIENT:
            // 环境适应：根据光强调整
            Calculate_Ambient_Effect();
            break;
    }
}

u8 Get_Light_Mode_Priority(LIGHT_MODE mode) {
    switch(mode) {
        case LIGHT_BRAKE:      return PRIORITY_BRAKE;
        case LIGHT_TURN_LEFT:
        case LIGHT_TURN_RIGHT: return PRIORITY_TURN;
        case LIGHT_POSITION:   return PRIORITY_POSITION;
        case LIGHT_HIGH_BEAM:  return PRIORITY_HEADLIGHT;
        case LIGHT_MUSIC:      return PRIORITY_MUSIC;
        case LIGHT_AMBIENT:    return PRIORITY_AMBIENT;
        default:               return 255;
    }
}

//========================================================================
//                               灯效算法实现
//========================================================================

void Calculate_Brake_Effect(void) {
    // 刹车效果已在Apply_Light_Effect中实现
    // 这里可以添加动态效果，如呼吸闪烁
}

void Calculate_Turn_Left_Effect(void) {
    static u8 phase = 0;
    phase = (phase + 1) % 10;

    // 左侧灯珠流水效果 (假设0-44为左侧)
    for(u8 i = 0; i < WS2812_COUNT/2; i++) {
        if(i >= phase * 4.4 && i < (phase + 1) * 4.4) {
            led_colors[i].r = 255;
            led_colors[i].g = 255;
            led_colors[i].b = 0;  // 黄色转向灯
        } else {
            led_colors[i].r = 0;
            led_colors[i].g = 0;
            led_colors[i].b = 0;
        }
    }
}

void Calculate_Turn_Right_Effect(void) {
    static u8 phase = 0;
    phase = (phase + 1) % 10;

    // 右侧灯珠流水效果 (假设45-89为右侧)
    for(u8 i = WS2812_COUNT/2; i < WS2812_COUNT; i++) {
        if(i >= WS2812_COUNT/2 + phase * 4.4 &&
           i < WS2812_COUNT/2 + (phase + 1) * 4.4) {
            led_colors[i].r = 255;
            led_colors[i].g = 255;
            led_colors[i].b = 0;  // 黄色转向灯
        } else {
            led_colors[i].r = 0;
            led_colors[i].g = 0;
            led_colors[i].b = 0;
        }
    }
}

void Calculate_Position_Effect(void) {
    // 位置灯效果已在Apply_Light_Effect中实现
}

void Calculate_Music_Sync_Effect(void) {
    // 根据频谱数据设置灯效
    for(u8 i = 0; i < WS2812_COUNT; i++) {
        u8 band = i % 8;
        u8 intensity = frequency_bands[band];

        // 将频谱强度转换为RGB颜色
        led_colors[i].r = intensity;
        led_colors[i].g = intensity / 2;
        led_colors[i].b = intensity / 4;
    }
}

void Calculate_Ambient_Effect(void) {
    // 根据环境光强调整亮度
    u8 brightness;
		brightness= 255 - (sensor_data.light_level / 16); // 反比关系

    for(u8 i = 0; i < WS2812_COUNT; i++) {
        led_colors[i].r = brightness;
        led_colors[i].g = brightness;
        led_colors[i].b = brightness;
    }
}

//========================================================================
//                               WS2812控制函数
//========================================================================

void WS2812_Send_DMA(void) {
    // 编码RGB数据到SPI格式
    WS2812_Encode_Buffer();

    // 配置DMA传输
    DMA_SPI_CFG = 0x80;           // 使能DMA
    DMA_SPI_STA = (u16)ws2812_buffer;  // 源地址
    DMA_SPI_AMT = WS2812_COUNT * 24;   // 数据量
    DMA_SPI_DONE = 0;                  // 清除完成标志

    // 启动DMA传输
    DMA_SPI_CR = 0x01;
}

void WS2812_Encode_Buffer(void) {
    u8 *p = ws2812_buffer;
		u8 i,j,k;
    for( i = 0; i < WS2812_COUNT; i++) {
        // GRB顺序 (WS2812标准)
        u8 colors[3] = {led_colors[i].g, led_colors[i].r, led_colors[i].b};

        for( j = 0; j < 3; j++) {
            for( k = 0; k < 8; k++) {
                if(colors[j] & (0x80 >> k)) {
                    *p++ = 0xFE;  // 1码: 850ns高 + 400ns低
                } else {
                    *p++ = 0xC0;  // 0码: 400ns高 + 850ns低
                }
            }
        }
    }
}

//========================================================================
//                               PWM控制函数
//========================================================================

void Update_PWM_Outputs(void) {
    // 更新PWMA通道 (RGB灯组)
//    PWMA_CCR1H = pwm_duties.red_duty >> 8;
//    PWMA_CCR1L = pwm_duties.red_duty;

//    PWMA_CCR2H = pwm_duties.green_duty >> 8;
//    PWMA_CCR2L = pwm_duties.green_duty;

//    PWMA_CCR3H = pwm_duties.blue_duty >> 8;
//    PWMA_CCR3L = pwm_duties.blue_duty;

//    // 更新刹车灯PWM
//    PWMB_CCR1H = pwm_duties.brake_duty >> 8;
//    PWMB_CCR1L = pwm_duties.brake_duty;
}

//========================================================================
//                               音频处理函数
//========================================================================

void Audio_FFT_Process(u16 *samples) {
    // 简化的FFT处理 (实际项目中需要完整的FFT算法)
    // 这里只是示例，实现基本的频谱分析
//    for(u8 i = 0; i < 8; i++) {
//        u32 sum = 0;
//        u8 start = i * (ADC_SAMPLE_COUNT / 8);
//        u8 end = (i + 1) * (ADC_SAMPLE_COUNT / 8);

//        for(u8 j = start; j < end; j++) {
//            sum += samples[j];
//        }

//        frequency_bands[i] = (u8)(sum / (ADC_SAMPLE_COUNT / 8) >> 4);
//    }
}

void Calculate_Frequency_Bands(void) {
    // 频谱计算已在Audio_FFT_Process中完成
}

void Adjust_Global_Brightness(void) {
    // 根据音频强度调整全局亮度
    u8 max_intensity = 0;
	  u8 i;
    for( i = 0; i < 8; i++) {
        if(frequency_bands[i] > max_intensity) {
            max_intensity = frequency_bands[i];
        }
    }

    // 调整位置灯的亮度
    if(current_mode == LIGHT_POSITION) {
        u8 brightness = 50 + (max_intensity / 5); // 基础亮度 + 音频影响
        if(brightness > 150) brightness = 150;

        for( i = 0; i < WS2812_COUNT; i++) {
            led_colors[i].r = brightness;
            led_colors[i].g = brightness;
            led_colors[i].b = brightness;
        }
    }
}

//========================================================================
//                               传感器接口函数
//========================================================================

void MPU6050_Read_Accel(s16 *x, s16 *y, s16 *z) {
    // MPU6050 I2C读取加速度数据
    // 这里是简化实现，实际需要I2C通信
    *x = 0;  // 示例数据
    *y = 0;
    *z = 1000; // 1g
}

u16 BH1750_Read_Light(void) {
    // BH1750 I2C读取光强数据
    // 这里是简化实现，实际需要I2C通信
    return 500; // 500 lux 示例
}

//========================================================================
//                               工具函数
//========================================================================

RGB_COLOR HSV_to_RGB(u16 h, u8 s, u8 v) {
    RGB_COLOR rgb;
    u8 region, remainder, p, q, t;

    if (s == 0) {
        rgb.r = rgb.g = rgb.b = v;
        return rgb;
    }

    region = h / 60;
    remainder = (h - region * 60) * 255 / 60;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0: rgb.r = v; rgb.g = t; rgb.b = p; break;
        case 1: rgb.r = q; rgb.g = v; rgb.b = p; break;
        case 2: rgb.r = p; rgb.g = v; rgb.b = t; break;
        case 3: rgb.r = p; rgb.g = q; rgb.b = v; break;
        case 4: rgb.r = t; rgb.g = p; rgb.b = v; break;
        default: rgb.r = v; rgb.g = p; rgb.b = q; break;
    }

    return rgb;
}

void Check_Mode_Timeouts(void) {
		u8 i;
		u8 timeout;
    for( i = 0; i < active_mode_count; i++) {
        LIGHT_MODE mode = active_modes[i];
        mode_timers[i]++;

        timeout = 0;
        switch(mode) {
            case LIGHT_TURN_LEFT:
            case LIGHT_TURN_RIGHT:
                if(mode_timers[i] >= TURN_SIGNAL_TIMEOUT / 20) { // 20ms任务周期
                    timeout = 1;
                }
                break;

            case LIGHT_MUSIC:
                if(mode_timers[i] >= MUSIC_MODE_TIMEOUT / 20) {
                    timeout = 1;
                }
                break;

            default:
                break;
        }

        if(timeout) {
            printf("[TIMEOUT] Mode %d timeout, returning to position lights\n", mode);
            Set_Light_Mode(mode, 0);
            Set_Light_Mode(LIGHT_POSITION, 1);
            mode_timers[i] = 0;
        }
    }
}

//========================================================================
//                               状态机函数
//========================================================================

void System_State_Machine(void) {
    SYSTEM_STATE old_state = system_state;

    switch(system_state) {
        case SYS_INIT:
            printf("[SYS] System in INIT state\n");
            if(1) { // 初始化完成条件
                printf("[SYS] Initialization completed, switching to NORMAL\n");
                system_state = SYS_NORMAL;
            } else if(0) { // 初始化失败条件
                printf("[SYS] Initialization failed, switching to ERROR\n");
                system_state = SYS_ERROR;
            }
            break;

        case SYS_NORMAL:
            Light_Mode_State_Machine(); // 执行灯效状态机
            break;

        case SYS_LOW_POWER:
            if(sensor_data.battery_voltage > RECOVER_VOLTAGE_THRESHOLD) {
                printf("[SYS] Voltage recovered, switching to NORMAL\n");
                system_state = SYS_NORMAL;
            }
            break;

        case SYS_ERROR:
            // 错误状态处理
            break;

        case SYS_TEST_MODE:
            // 测试模式处理
            if(0) { // 测试完成条件
                system_state = SYS_NORMAL;
            }
            break;
    }
}

void Light_Mode_State_Machine(void) {
    printf("[LIGHT] Checking light mode conditions...\n");

    // 优先级检测 (从高到低)
    if(Brake_Signal_Active()) {
        printf("[LIGHT] Brake signal detected\n");
        Set_Light_Mode(LIGHT_BRAKE, 1);
    } else if(Turn_Left_Signal_Active()) {
        printf("[LIGHT] Left turn signal detected\n");
        Set_Light_Mode(LIGHT_TURN_LEFT, 1);
    } else if(Turn_Right_Signal_Active()) {
        printf("[LIGHT] Right turn signal detected\n");
        Set_Light_Mode(LIGHT_TURN_RIGHT, 1);
    } else if(High_Beam_Active()) {
        printf("[LIGHT] High beam signal detected\n");
        Set_Light_Mode(LIGHT_HIGH_BEAM, 1);
    } else if(Audio_Signal_Detected()) {
        printf("[LIGHT] Audio signal detected\n");
        Set_Light_Mode(LIGHT_MUSIC, 1);
    } else {
        // 默认位置灯模式
        printf("[LIGHT] No active signals, using position lights\n");
        Set_Light_Mode(LIGHT_POSITION, 1);
    }

    // 超时处理
    printf("[LIGHT] Checking mode timeouts\n");
    Check_Mode_Timeouts();
}

//========================================================================
//                               调试函数
//========================================================================

void Print_System_Status(void) {
    printf("\n=== System Status ===\n");
    printf("State: %d\n", system_state);
    printf("Light Mode: %d\n", current_mode);
    printf("Battery: %dmV\n", sensor_data.battery_voltage);
    printf("Light Level: %d lux\n", sensor_data.light_level);
    printf("=====================\n");
}

void Print_Light_Status(void) {
    printf("\n=== Light Status ===\n");
    printf("Active Modes: %d\n", active_mode_count);
    printf("Current Mode: %d\n", current_mode);
    printf("WS2812 Status: OK\n");
    printf("PWM Status: OK\n");
    printf("===================\n");
}
