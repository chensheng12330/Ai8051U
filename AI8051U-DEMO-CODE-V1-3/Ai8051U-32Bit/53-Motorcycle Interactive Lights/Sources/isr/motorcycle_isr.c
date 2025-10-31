/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "motorcycle_light_system.h"
#include "config.h"

//========================================================================
// 外部中断服务函数 (INT2/INT3 - 摩托车灯组系统按键)
//========================================================================

// INT2 中断服务函数 (测试按键 P3.6)
void INT2_ISR(void) interrupt 10
{
    INT2_FLAG = 0;  // 清除中断标志

    printf("[ISR] INT2 triggered - Test button\n");

    // 切换到测试模式
    system_state = SYS_TEST_MODE;

    // 可以在这里添加按键消抖或其他处理
}

// INT3 中断服务函数 (菜单按键 P3.7)
void INT3_ISR(void) interrupt 11
{
    INT3_FLAG = 0;  // 清除中断标志

    printf("[ISR] INT3 triggered - Menu button\n");

    // 循环切换灯效模式
    static LIGHT_MODE current_test_mode = LIGHT_OFF;
    current_test_mode = (LIGHT_MODE)((current_test_mode + 1) % 8);

    printf("[ISR] Switching to mode: %d\n", current_test_mode);

    // 关闭所有模式，然后开启新模式
    for(LIGHT_MODE mode = LIGHT_OFF; mode <= LIGHT_AMBIENT; mode = (LIGHT_MODE)(mode + 1)) {
        if(mode != current_test_mode) {
            Set_Light_Mode(mode, 0);
        }
    }
    Set_Light_Mode(current_test_mode, 1);

    // 可以在这里添加按键消抖或其他处理
}

//========================================================================
// DMA中断服务函数 (可选，用于高级DMA控制)
//========================================================================

// SPI DMA完成中断 (可选)
void SPI_DMA_ISR(void) interrupt DMA_SPI_VECTOR
{
    DMA_SPI_DONE = 0;  // 清除完成标志

    // WS2812 DMA传输完成，可以在这里添加处理逻辑
    // printf("[DMA] SPI transfer completed\n");
}

// ADC DMA完成中断 (可选)
void ADC_DMA_ISR(void) interrupt DMA_ADC_VECTOR
{
    DMA_ADC_DONE = 0;  // 清除完成标志

    // ADC采样完成，可以在这里处理采样数据
    // printf("[DMA] ADC sampling completed\n");
}
