#include "set_spi.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

char spi_flag = 0;
char static _char; // 私有变量
int _ma_sl, ma_sl, _en_dis, en_dis, _iosw, iosw;
int _spi_sw, spi_sw, _div, div, _msblsb, msblsb;
int _cpol, cpol, _cpha, cpha;

//DMA缓冲区
char xdata _spi0_tx_buff[_Spi0_Tx] = {0};
char xdata _spi1_tx_buff[_Spi1_Tx] = {0};
char xdata _spi2_tx_buff[_Spi2_Tx] = {0};
char xdata _spi0_rx_buff[_Spi0_Rx] = {0};
char xdata _spi1_rx_buff[_Spi1_Rx] = {0};
char xdata _spi2_rx_buff[_Spi2_Rx] = {0};

void set_spi_setting(spi_name spi)
{
    switch (spi)
    {
    case SPI0:
        SPI_S0 = spi_sw&1;SPI_S1 = (spi_sw>>1)&1;ESPI = 0;//关闭SPI中断
        HSSPI_CFG2 &= ~(1<<6);HSSPI_CFG2 |= (iosw&1)<<6;
        SPCTL = (1<<7)|((en_dis&1)<<6)|((msblsb&1)<<5)|((ma_sl&1)<<4)|((cpol&1)<<3)|((cpha&1)<<2)|(div&3);
        DMA_SPI_CFG = 0xe0;DMA_SPI_CR = 0x80;DMA_SPI_STA = 0x00;
        DMA_SPI_TXAH = (unsigned char)((unsigned int)&_spi0_tx_buff >> 8);DMA_SPI_TXAL = (unsigned char)((unsigned int)&_spi0_tx_buff);
        DMA_SPI_RXAH = (unsigned char)((unsigned int)&_spi0_rx_buff >> 8);DMA_SPI_RXAL = (unsigned char)((unsigned int)&_spi0_rx_buff);
        break;
    case SPI1:
        S1SPI_S0 = spi_sw&1;S1SPI_S1 = (spi_sw>>1)&1;SCON = 0x10;ES = 0;//关闭中断,允许接收, 模式0
        USARTCR1 &= ~(0x1f);USARTCR1 |= ((msblsb&1)<<6)|((en_dis&1)<<4)|(((~ma_sl)&1)<<2)|((cpol&1)<<1)|(cpha&1);USARTCR4 &= ~(0x03);
        USARTCR4 |= (div&3);USARTCR1 |= ((en_dis&1)<<3);
        DMA_UR1R_CFG = 0x00;DMA_UR1R_STA = 0x00;
        DMA_UR1R_AMT = (unsigned char)(_Spi1_Rx-2);DMA_UR1R_AMTH = (unsigned char)((_Spi1_Rx-2)>>8);/*设置传输总字节数(高8位)：n+1*/
        DMA_UR1R_RXAH = (unsigned char)((unsigned int)&_spi1_rx_buff >> 8);DMA_UR1R_RXAL = (unsigned char)((unsigned int)&_spi1_rx_buff);
        DMA_UR1T_CFG = 0x00;DMA_UR1T_STA = 0x00;DMA_UR1T_AMT = 0;DMA_UR1T_AMTH = 0;
        DMA_UR1T_TXAH = (unsigned char)((unsigned int)&_spi1_tx_buff >> 8);DMA_UR1T_TXAL = (unsigned char)((unsigned int)&_spi1_tx_buff);
        break;
    case SPI2:
        S2SPI_S0 = spi_sw&1;S2SPI_S1 = (spi_sw>>1)&1;S2CON = 0x10;ES2 = 0;//关闭中断
        USART2CR1 &= ~(0x1f);USART2CR1 |= ((msblsb&1)<<6)|((en_dis&1)<<4)|(((~ma_sl)&1)<<2)|((cpol&1)<<1)|(cpha&1);USART2CR4 &= ~(0x03);
        USART2CR4 |= (div&3);USART2CR1 |= ((en_dis&1)<<3);
        DMA_UR2R_CFG = 0x00;DMA_UR2R_STA = 0x00;
        DMA_UR2R_AMT = (unsigned char)(_Spi2_Rx-2);DMA_UR2R_AMTH = (unsigned char)((_Spi2_Rx-2)>>8);/*设置传输总字节数(高8位)：n+1*/
        DMA_UR2R_RXAH = (unsigned char)((unsigned int)&_spi2_rx_buff >> 8);DMA_UR2R_RXAL = (unsigned char)((unsigned int)&_spi2_rx_buff);
        DMA_UR2T_CFG = 0x00;DMA_UR2T_STA = 0x00;DMA_UR2T_AMT = 0;DMA_UR2T_AMTH = 0;
        DMA_UR2T_TXAH = (unsigned char)((unsigned int)&_spi2_tx_buff >> 8);DMA_UR2T_TXAL = (unsigned char)((unsigned int)&_spi2_tx_buff);
        break;
    default:break;//没有就不运行
    }
}

// 用于设置SPI的各种初始化参数，所有参数都支持默认值和乱序输入，最后需要带上Spi_End结束标志
// 参数可以从上面直接复制，同一组的参数只能使用一个，使用多个是最后的参数生效，顺序没有要求
// 举个例子：设置SPI1的引脚为Spi_P14_5_6_7，时钟分频为16分频，SCLK空闲为高
// 数据在下降沿驱动，上升沿采样，数据为高位优先，不交换MOSI和MISO引脚，主机模式并使能SPI
// set_spi_mode(SPI1, Spi_P14_5_6_7, Spi_ClkDiv_16, High_Falling, MSB, Out_In, Spi_Enable, Spi_Master, NoSw_MOSI_MISO, Spi_End);
// 上面这个示例跟这个是等效的：set_spi_mode(SPI1, Spi_End);//全部使用默认值
// 也可以按需更改 set_spi_mode(SPI1, Spi_P24_5_6_7, Low_Rising, Spi_End);//只更改了引脚和驱动极性
void set_spi_mode(spi_name spi, ...)
{
    char *arg;
    va_list args;        // 可变参数列表
    va_start(args, spi); // 初始化可变参数列表
    ma_sl = 1;en_dis = 1; iosw = 0; spi_sw = 0; div = 2; msblsb = 0; cpol = 1; cpha = 1; //设置参数默认值
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;                               // 遇到哨兵，结束
        ma_sl = (sscanf(arg, "\x01masl%d", &_ma_sl) == 1) ? _ma_sl : ma_sl;       // 解析主机模式还是从机模式
        en_dis = (sscanf(arg, "\x01en_dis%d", &_en_dis) == 1) ? _en_dis : en_dis; // 解析使能还是禁用
        iosw = (sscanf(arg, "\x01sw%d", &_iosw) == 1) ? _iosw : iosw;             // 解析MISO和MOSI是否交换
        spi_sw = (sscanf(arg, "spi%d", &_spi_sw) == 1) ? _spi_sw : spi_sw;        // 解析SPI引脚切换的组别
        div = (sscanf(arg, "div%d", &_div) == 1) ? _div : div;                    // 解析SPI时钟分频
        msblsb = (sscanf(arg, "sign%d", &_msblsb) == 1) ? _msblsb : msblsb;       // 解析高位优先（发送/接收）还是低位优先
        cpol = (sscanf(arg, "level%d", &_cpol) == 1) ? _cpol : cpol;              // 解析时钟信号极性
        cpha = (sscanf(arg, "clock%d", &_cpha) == 1) ? _cpha : cpha;              // 解析时钟信号相位
    }
    set_spi_setting(spi);
    va_end(args); // 清理可变参数列表
}

// 用于获取SPI的当前状态，因为SPI是全双工的，所以当发送完成（同时也是接收完成）的时候就会返回1，否则返回0
char get_spi_state(spi_name spi)
{
    char state;
    if(((DMA_UR1T_STA|DMA_UR2T_STA|spi_flag)&1)==0)
        return 0; // 提前返回
    switch (spi){
    case SPI0:if(spi_flag){spi_flag = 0; state = 1;}else return 0;break;
    case SPI1:if(DMA_UR1T_STA&1){
        DMA_UR1T_STA = 0;DMA_UR1T_CR = 0;
        if(DMA_UR1R_STA&1){DMA_UR1R_STA = 0; DMA_UR1R_CR = 0;state = 1;}
    }else return 0;break;
    case SPI2:if(DMA_UR2T_STA&1){
        DMA_UR2T_STA = 0;DMA_UR2T_CR = 0;
        if(DMA_UR2R_STA&1){DMA_UR2R_STA = 0; DMA_UR2R_CR = 0;state = 1;}
    }else return 0;break;
    default:break;}
    return state;            // 返回中断状态
}

// 用于发送SPI的数据，分为Hex_Mode和Buff_Mode两种模式
// Hex_Mode为单字节发送，类似于直接操作SPI的数据寄存器发送一个byte的数据
// Buff_Mode为缓冲区发送，需要传入一个char类型的数组指针，然后再传入所需发送的数据长度
void spi_printf(spi_name spi, ...)
{
    char *arg;
    int dat_len = 0,hex_dat;
    va_list args;        // 可变参数列表
    va_start(args, spi); // 初始化可变参数列表
    arg = va_arg(args, char *);
    if(arg[0] == '\x01')
    {//为hex_mode
        dat_len = 0;hex_dat = va_arg(args, int);
        switch (spi){
        case SPI0:_spi0_tx_buff[0] = hex_dat;break;
        case SPI1:_spi1_tx_buff[0] = hex_dat;break;
        case SPI2:_spi2_tx_buff[0] = hex_dat;break;
        default:break;}
    }else
    {//为buff_mode
        arg = va_arg(args, char *);dat_len = va_arg(args, int) - 1;
        switch (spi){
        case SPI0:memcpy(_spi0_tx_buff, arg, dat_len+1);break;
        case SPI1:memcpy(_spi1_tx_buff, arg, dat_len+1);break;
        case SPI2:memcpy(_spi2_tx_buff, arg, dat_len+1);break;
        default:break;}
    }
    switch (spi){
    case SPI0:if (dat_len > _Spi0_Tx) break;DMA_SPI_AMTH = (unsigned char)(dat_len>>8); DMA_SPI_AMT = (unsigned char)dat_len;DMA_SPI_CR |= 0x41; break;
    case SPI1:if (dat_len > _Spi1_Tx) break;DMA_UR1T_AMTH = (unsigned char)(dat_len>>8);DMA_UR1T_AMT = (unsigned char)dat_len;DMA_UR1T_CR = 0;DMA_UR1R_CR = 0;DMA_UR1R_CR = 0xa1;DMA_UR1T_CR = 0xc0; break;
    case SPI2:if (dat_len > _Spi2_Tx) break;DMA_UR2T_AMTH = (unsigned char)(dat_len>>8);DMA_UR2T_AMT = (unsigned char)dat_len;DMA_UR2T_CR = 0;DMA_UR2R_CR = 0;DMA_UR2R_CR = 0xa1;DMA_UR2T_CR = 0xc0; break;
    default:break;}
    va_end(args); // 清理可变参数列表
}

void spi_isr(void) interrupt DMA_SPI_VECTOR
{
    DMA_SPI_STA = 0x00;spi_flag = 1;
}