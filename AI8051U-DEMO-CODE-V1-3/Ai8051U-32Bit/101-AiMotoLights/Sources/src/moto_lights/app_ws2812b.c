/*---------------------------------------------------------------------*/
/* --- 摩托车智能联动灯组系统 - WS2812B控制模块 (SPI-DMA方式) --------*/
/*---------------------------------------------------------------------*/

#include "config.h"
#include "moto_lights/app_ws2812b.h"
#include "moto_lights/moto_lights_pinmap.h"

/*************** 功能说明 ****************

WS2812B RGB LED控制模块 - SPI-DMA硬件方式

功能:
1. 控制90颗WS2812B灯珠
2. 使用SPI硬件模块自动输出波形
3. DMA自动传输数据，CPU零占用
4. 支持分区控制（前左、前右、车身左、车身右、尾部）
5. 多种灯光效果（静态、呼吸、流水、闪烁、彩虹、频谱）

WS2812B时序要求（800kHz）:
- 0码：T0H=0.25us, T0L=1.00us
- 1码：T1H=1.00us, T1L=0.25us
- Reset: >50us低电平

SPI-DMA编码方案：
- 主频48MHz，SPI分频8 = 6MHz
- 每个WS2812B数据位用4个SPI位表示
- 数据0: 0x80 (1000)
- 数据1: 0xE0 (1110)
- 每个LED 3字节(GRB) → 12字节SPI数据
- 90颗LED：270字节RGB → 1080字节SPI数据

优点：
- DMA自动传输，不占CPU
- 不需要禁用中断
- 时序稳定可靠
- 传输速度快（约3ms）

******************************************/

//========================================================================
//                          全局变量定义
//========================================================================

// LED颜色缓冲区（GRB格式，符合WS2812B）
WS2812B_COLOR ws2812b_buffer[WS2812B_LED_COUNT];

// SPI发送缓冲区（每个LED需要12字节SPI数据）
u8 xdata ws2812b_spi_buffer[WS2812B_SPI_BYTES];

// 全局亮度（0-255）
u8 ws2812b_brightness = 255;

// DMA传输完成标志
volatile u8 ws2812b_dma_done = 1;

//========================================================================
//                          内部变量
//========================================================================

// 效果相位计数器（用于动画）
static u16 effect_phase = 0;

//========================================================================
//                          SPI-DMA配置和控制
//========================================================================

// SPI配置
static void WS2812B_SPI_Config(void)
{
    EAXFR = 1;  // 允许访问扩展寄存器
    
    // 配置SPI为主机模式
    // 速度：fosc/8 = 48MHz/8 = 6MHz (接近3.2MHz最佳值)
    SPCTL = 1;      // SPI速度：fosc/8, bit1-bit0=01
    SSIG = 1;       // 忽略SS脚，由MSTR位决定主机模式
    SPEN = 1;       // 允许SPI
    DORD = 0;       // MSB先发
    MSTR = 1;       // 主机模式
    CPOL = 0;       // 空闲时SCLK为低电平
    CPHA = 0;       // 数据在SCLK前沿采样，后沿驱动
    
    // 切换SPI到P2.4/P2.5/P2.6/P2.7（SPI组1）
    P_SW1 = (P_SW1 & ~0x0c) | (1<<2);  // SPI_S1=0, SPI_S0=1 → 切换到组1
    
    // 配置高速SPI
    HSCLKDIV = 1;   // 主时钟分频
    SPI_CLKDIV = 1; // SPI时钟分频
    
    // 配置GPIO - P2口
    // P2.5(MOSI) P2.7(SCLK) 设置为推挽输出
    P2M1 &= ~0xA0;  // P2.5,P2.7清除M1
    P2M0 |= 0xA0;   // P2.5,P2.7设置M0 → 推挽输出
    
    // P2.6(MISO) 设置为高阻输入+下拉，确保空闲时为低
    P2M1 |= 0x40;   // P2.6设置M1
    P2M0 &= ~0x40;  // P2.6清除M0 → 高阻输入
    P2PU &= ~0x40;  // P2.6禁止上拉
    P2PD |= 0x40;   // P2.6允许下拉
    
    // 设置端口输出为高速模式（用于高速SPI）
    P2SR |= 0xA0;   // P2.5, P2.7高速模式
    
    // 配置高速SPI寄存器
    HSSPI_CFG = (0<<4) | 3;         // SS_HOLD=0, SS_SETUP=3
    HSSPI_CFG2 = (1<<5) | (1<<4) | 3; // HSSPIEN=1, FIFOEN=1, SS_DACT=3
    
    printf("SPI1 configured: P2.4/P2.5/P2.6/P2.7, 48MHz/8 = 6MHz\r\n");
}

// 配置DMA
static void WS2812B_DMA_Config(void)
{
    // DMA_SPI配置
    DMA_SPI_CFG = (1<<7) |  // DMA_SPIIE: 允许中断
                  (1<<6) |  // SPI_ACT_TX: 允许发送
                  (0<<5) |  // SPI_ACT_RX: 禁止接收
                  (0<<2) |  // DMA_SPIIP: 中断优先级0
                  0;        // DMA_SPIPTY: 总线优先级0
    
    DMA_SPI_CFG2 = 0;  // 不自动控制SS脚
    
    printf("DMA configured\r\n");
}

// 启动DMA传输
static void WS2812B_DMA_Start(void)
{
    u16 addr;
    
    ws2812b_dma_done = 0;  // 清除完成标志
    
    // 设置DMA源地址
    addr = (u16)ws2812b_spi_buffer;
    DMA_SPI_TXAH = (u8)(addr >> 8);
    DMA_SPI_TXAL = (u8)(addr);
    
    // 设置传输字节数 (n+1个字节)
    DMA_SPI_AMTH = (u8)((WS2812B_SPI_BYTES - 1) >> 8);
    DMA_SPI_AMT  = (u8)((WS2812B_SPI_BYTES - 1) & 0xFF);
    
    // 设置间隔时间（0=无间隔）
    DMA_SPI_ITVH = 0;
    DMA_SPI_ITVL = 0;
    
    // 清除状态
    DMA_SPI_STA = 0x00;
    
    // 启动DMA传输
    DMA_SPI_CR = (1<<7) |  // DMA_ENSPI: 使能SPI DMA
                 (1<<6) |  // SPI_TRIG_M: 触发主机模式
                 1;        // SPI_CLRFIFO: 清除FIFO
}

//========================================================================
// 函数: WS2812B_Init
// 描述: 初始化WS2812B模块
// 参数: None.
// 返回: None.
//========================================================================
void WS2812B_Init(void)
{
    // 清空颜色缓冲区
    WS2812B_Clear();
    
    // 设置默认亮度
    ws2812b_brightness = WS2812B_BRIGHTNESS_NIGHT;
    
    // 配置SPI硬件
    WS2812B_SPI_Config();
    
    // 配置DMA
    WS2812B_DMA_Config();
    
    // 使能SPI DMA中断
    IE2 |= (1<<1);  // ESPI = 1
    
    // 初始化显示（发送复位信号）
    WS2812B_Update();
    
    printf("WS2812B initialized (SPI1-DMA mode, P2.5-MOSI, %d LEDs)\r\n", WS2812B_LED_COUNT);
}

//========================================================================
// 函数: WS2812B_RGB_to_SPI
// 描述: 将RGB数据转换为SPI数据
// 参数: None.
// 返回: None.
//========================================================================
static void WS2812B_RGB_to_SPI(void)
{
    u16 i, j;
    u8 k;
    u8 dat;
    u8 xdata *px;
    
    // 遍历所有LED
    px = (u8 xdata *)ws2812b_buffer;
    j = 0;
    
    for(i = 0; i < (WS2812B_LED_COUNT * 3); i++)
    {
        dat = *px++;
        
        // 每个字节拆分为4个SPI字节（每2位对应1个SPI字节）
        for(k = 0; k < 4; k++)
        {
            // 高半字节（bit7）
            if(dat & 0x80)
                ws2812b_spi_buffer[j] = 0xE0;  // 数据1
            else
                ws2812b_spi_buffer[j] = 0x80;  // 数据0
            
            // 低半字节（bit6）
            if(dat & 0x40)
                ws2812b_spi_buffer[j] |= 0x0E;  // 数据1
            else
                ws2812b_spi_buffer[j] |= 0x08;  // 数据0
            
            dat <<= 2;
            j++;
        }
    }
}

//========================================================================
// 函数: WS2812B_Update
// 描述: 更新WS2812B显示（发送缓冲区数据）
// 参数: None.
// 返回: None.
//========================================================================
void WS2812B_Update(void)
{
    // 等待上次DMA完成
    while(!ws2812b_dma_done);
    
    // 将RGB数据转换为SPI数据
    WS2812B_RGB_to_SPI();
    
    // 启动DMA传输
    WS2812B_DMA_Start();
}

//========================================================================
// 函数: SPI_DMA_ISR
// 描述: SPI DMA中断服务程序
// 参数: None.
// 返回: None.
//========================================================================
void SPI_DMA_ISR(void) interrupt DMA_SPI_VECTOR
{
    // 清除中断标志
    DMA_SPI_STA = 0;
    
    // 设置完成标志
    ws2812b_dma_done = 1;
}

//========================================================================
//                          颜色设置函数
//========================================================================

void WS2812B_SetPixel(u16 index, u8 r, u8 g, u8 b)
{
    if(index >= WS2812B_LED_COUNT) return;
    
    // WS2812B顺序是GRB
    ws2812b_buffer[index].g = g;
    ws2812b_buffer[index].r = r;
    ws2812b_buffer[index].b = b;
}

void WS2812B_SetZone(WS2812B_ZONE zone, u8 r, u8 g, u8 b)
{
    u16 start = WS2812B_GetZoneStart(zone);
    u16 len = WS2812B_GetZoneLength(zone);
    u16 i;
    
    if(zone == ZONE_ALL)
    {
        WS2812B_SetAll(r, g, b);
        return;
    }
    
    for(i = 0; i < len; i++)
    {
        WS2812B_SetPixel(start + i, r, g, b);
    }
}

void WS2812B_SetAll(u8 r, u8 g, u8 b)
{
    u16 i;
    for(i = 0; i < WS2812B_LED_COUNT; i++)
    {
        WS2812B_SetPixel(i, r, g, b);
    }
}

void WS2812B_Clear(void)
{
    WS2812B_SetAll(0, 0, 0);
}

void WS2812B_SetBrightness(u8 brightness)
{
    ws2812b_brightness = brightness;
}

WS2812B_COLOR WS2812B_ApplyBrightness(WS2812B_COLOR color, u8 brightness)
{
    WS2812B_COLOR result;
    result.r = ((u16)color.r * brightness) >> 8;
    result.g = ((u16)color.g * brightness) >> 8;
    result.b = ((u16)color.b * brightness) >> 8;
    return result;
}

//========================================================================
//                          灯光效果实现（与原来相同）
//========================================================================

void WS2812B_Effect_Static(WS2812B_ZONE zone, EFFECT_PARAM *param)
{
    WS2812B_COLOR color = WS2812B_ApplyBrightness(param->color1, param->brightness);
    WS2812B_SetZone(zone, color.r, color.g, color.b);
}

void WS2812B_Effect_Breathing(WS2812B_ZONE zone, EFFECT_PARAM *param)
{
    u8 brightness;
    
    param->phase += param->speed;
    if(param->phase >= 1000) param->phase = 0;
    
    if(param->phase < 500)
        brightness = (param->phase * param->brightness) / 500;
    else
        brightness = ((1000 - param->phase) * param->brightness) / 500;
    
    WS2812B_COLOR color = WS2812B_ApplyBrightness(param->color1, brightness);
    WS2812B_SetZone(zone, color.r, color.g, color.b);
}

void WS2812B_Effect_Flowing(WS2812B_ZONE zone, EFFECT_PARAM *param)
{
    u16 start = WS2812B_GetZoneStart(zone);
    u16 len = WS2812B_GetZoneLength(zone);
    u16 i;
    u16 tail_len = 5;
    
    param->phase += param->speed;
    if(param->phase >= len * 10) param->phase = 0;
    
    u16 head_pos = param->phase / 10;
    
    WS2812B_SetZone(zone, 0, 0, 0);
    
    for(i = 0; i < tail_len && (head_pos >= i); i++)
    {
        u16 pos = head_pos - i;
        if(pos < len)
        {
            u8 brightness = ((tail_len - i) * param->brightness) / tail_len;
            WS2812B_COLOR color = WS2812B_ApplyBrightness(param->color1, brightness);
            WS2812B_SetPixel(start + pos, color.r, color.g, color.b);
        }
    }
}

void WS2812B_Effect_Flashing(WS2812B_ZONE zone, EFFECT_PARAM *param)
{
    param->phase += param->speed;
    if(param->phase >= 1000) param->phase = 0;
    
    if(param->phase < 500)
    {
        WS2812B_COLOR color = WS2812B_ApplyBrightness(param->color1, param->brightness);
        WS2812B_SetZone(zone, color.r, color.g, color.b);
    }
    else
    {
        WS2812B_SetZone(zone, 0, 0, 0);
    }
}

void WS2812B_Effect_Rainbow(WS2812B_ZONE zone, EFFECT_PARAM *param)
{
    u16 start = WS2812B_GetZoneStart(zone);
    u16 len = WS2812B_GetZoneLength(zone);
    u16 i;
    
    param->phase += param->speed;
    if(param->phase >= 360) param->phase = 0;
    
    for(i = 0; i < len; i++)
    {
        u16 hue = (param->phase + (i * 360 / len)) % 360;
        WS2812B_COLOR color = WS2812B_HSV2RGB(hue, 255, param->brightness);
        WS2812B_SetPixel(start + i, color.r, color.g, color.b);
    }
}

void WS2812B_Effect_Spectrum(u8 *spectrum_data, u8 band_count)
{
    u16 i;
    u16 leds_per_band = WS2812B_LED_COUNT / band_count;
    
    for(i = 0; i < WS2812B_LED_COUNT; i++)
    {
        u8 band = i / leds_per_band;
        if(band >= band_count) band = band_count - 1;
        
        u8 level = spectrum_data[band];
        u16 hue = (band * 360) / band_count;
        
        WS2812B_COLOR color = WS2812B_HSV2RGB(hue, 255, level);
        WS2812B_SetPixel(i, color.r, color.g, color.b);
    }
}

//========================================================================
//                          辅助函数实现
//========================================================================

u16 WS2812B_GetZoneStart(WS2812B_ZONE zone)
{
    switch(zone)
    {
        case ZONE_FRONT_LEFT:  return WS2812B_ZONE_FRONT_LEFT;
        case ZONE_FRONT_RIGHT: return WS2812B_ZONE_FRONT_RIGHT;
        case ZONE_BODY_LEFT:   return WS2812B_ZONE_BODY_LEFT;
        case ZONE_BODY_RIGHT:  return WS2812B_ZONE_BODY_RIGHT;
        case ZONE_REAR:        return WS2812B_ZONE_REAR;
        default: return 0;
    }
}

u16 WS2812B_GetZoneLength(WS2812B_ZONE zone)
{
    switch(zone)
    {
        case ZONE_FRONT_LEFT:  return WS2812B_ZONE_FRONT_LEFT_LEN;
        case ZONE_FRONT_RIGHT: return WS2812B_ZONE_FRONT_RIGHT_LEN;
        case ZONE_BODY_LEFT:   return WS2812B_ZONE_BODY_LEFT_LEN;
        case ZONE_BODY_RIGHT:  return WS2812B_ZONE_BODY_RIGHT_LEN;
        case ZONE_REAR:        return WS2812B_ZONE_REAR_LEN;
        case ZONE_ALL:         return WS2812B_LED_COUNT;
        default: return 0;
    }
}

WS2812B_COLOR WS2812B_HSV2RGB(u16 hue, u8 sat, u8 val)
{
    WS2812B_COLOR color;
    u8 region, remainder, p, q, t;
    
    if(sat == 0)
    {
        color.r = val;
        color.g = val;
        color.b = val;
        return color;
    }
    
    region = hue / 60;
    remainder = (hue % 60) * 4;
    
    p = (val * (255 - sat)) >> 8;
    q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
    t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;
    
    switch(region)
    {
        case 0:  color.r = val; color.g = t;   color.b = p;   break;
        case 1:  color.r = q;   color.g = val; color.b = p;   break;
        case 2:  color.r = p;   color.g = val; color.b = t;   break;
        case 3:  color.r = p;   color.g = q;   color.b = val; break;
        case 4:  color.r = t;   color.g = p;   color.b = val; break;
        default: color.r = val; color.g = p;   color.b = q;   break;
    }
    
    return color;
}
