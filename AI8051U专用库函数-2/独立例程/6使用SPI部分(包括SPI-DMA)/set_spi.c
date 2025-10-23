#include "set_spi.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

char spi_flag = 0;
char static _char; // ˽�б���
int _ma_sl, ma_sl, _en_dis, en_dis, _iosw, iosw;
int _spi_sw, spi_sw, _div, div, _msblsb, msblsb;
int _cpol, cpol, _cpha, cpha;

//DMA������
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
        SPI_S0 = spi_sw&1;SPI_S1 = (spi_sw>>1)&1;ESPI = 0;//�ر�SPI�ж�
        HSSPI_CFG2 &= ~(1<<6);HSSPI_CFG2 |= (iosw&1)<<6;
        SPCTL = (1<<7)|((en_dis&1)<<6)|((msblsb&1)<<5)|((ma_sl&1)<<4)|((cpol&1)<<3)|((cpha&1)<<2)|(div&3);
        DMA_SPI_CFG = 0xe0;DMA_SPI_CR = 0x80;DMA_SPI_STA = 0x00;
        DMA_SPI_TXAH = (unsigned char)((unsigned int)&_spi0_tx_buff >> 8);DMA_SPI_TXAL = (unsigned char)((unsigned int)&_spi0_tx_buff);
        DMA_SPI_RXAH = (unsigned char)((unsigned int)&_spi0_rx_buff >> 8);DMA_SPI_RXAL = (unsigned char)((unsigned int)&_spi0_rx_buff);
        break;
    case SPI1:
        S1SPI_S0 = spi_sw&1;S1SPI_S1 = (spi_sw>>1)&1;SCON = 0x10;ES = 0;//�ر��ж�,�������, ģʽ0
        USARTCR1 &= ~(0x1f);USARTCR1 |= ((msblsb&1)<<6)|((en_dis&1)<<4)|(((~ma_sl)&1)<<2)|((cpol&1)<<1)|(cpha&1);USARTCR4 &= ~(0x03);
        USARTCR4 |= (div&3);USARTCR1 |= ((en_dis&1)<<3);
        DMA_UR1R_CFG = 0x00;DMA_UR1R_STA = 0x00;
        DMA_UR1R_AMT = (unsigned char)(_Spi1_Rx-2);DMA_UR1R_AMTH = (unsigned char)((_Spi1_Rx-2)>>8);/*���ô������ֽ���(��8λ)��n+1*/
        DMA_UR1R_RXAH = (unsigned char)((unsigned int)&_spi1_rx_buff >> 8);DMA_UR1R_RXAL = (unsigned char)((unsigned int)&_spi1_rx_buff);
        DMA_UR1T_CFG = 0x00;DMA_UR1T_STA = 0x00;DMA_UR1T_AMT = 0;DMA_UR1T_AMTH = 0;
        DMA_UR1T_TXAH = (unsigned char)((unsigned int)&_spi1_tx_buff >> 8);DMA_UR1T_TXAL = (unsigned char)((unsigned int)&_spi1_tx_buff);
        break;
    case SPI2:
        S2SPI_S0 = spi_sw&1;S2SPI_S1 = (spi_sw>>1)&1;S2CON = 0x10;ES2 = 0;//�ر��ж�
        USART2CR1 &= ~(0x1f);USART2CR1 |= ((msblsb&1)<<6)|((en_dis&1)<<4)|(((~ma_sl)&1)<<2)|((cpol&1)<<1)|(cpha&1);USART2CR4 &= ~(0x03);
        USART2CR4 |= (div&3);USART2CR1 |= ((en_dis&1)<<3);
        DMA_UR2R_CFG = 0x00;DMA_UR2R_STA = 0x00;
        DMA_UR2R_AMT = (unsigned char)(_Spi2_Rx-2);DMA_UR2R_AMTH = (unsigned char)((_Spi2_Rx-2)>>8);/*���ô������ֽ���(��8λ)��n+1*/
        DMA_UR2R_RXAH = (unsigned char)((unsigned int)&_spi2_rx_buff >> 8);DMA_UR2R_RXAL = (unsigned char)((unsigned int)&_spi2_rx_buff);
        DMA_UR2T_CFG = 0x00;DMA_UR2T_STA = 0x00;DMA_UR2T_AMT = 0;DMA_UR2T_AMTH = 0;
        DMA_UR2T_TXAH = (unsigned char)((unsigned int)&_spi2_tx_buff >> 8);DMA_UR2T_TXAL = (unsigned char)((unsigned int)&_spi2_tx_buff);
        break;
    default:break;//û�оͲ�����
    }
}

// ��������SPI�ĸ��ֳ�ʼ�����������в�����֧��Ĭ��ֵ���������룬�����Ҫ����Spi_End������־
// �������Դ�����ֱ�Ӹ��ƣ�ͬһ��Ĳ���ֻ��ʹ��һ����ʹ�ö�������Ĳ�����Ч��˳��û��Ҫ��
// �ٸ����ӣ�����SPI1������ΪSpi_P14_5_6_7��ʱ�ӷ�ƵΪ16��Ƶ��SCLK����Ϊ��
// �������½��������������ز���������Ϊ��λ���ȣ�������MOSI��MISO���ţ�����ģʽ��ʹ��SPI
// set_spi_mode(SPI1, Spi_P14_5_6_7, Spi_ClkDiv_16, High_Falling, MSB, Out_In, Spi_Enable, Spi_Master, NoSw_MOSI_MISO, Spi_End);
// �������ʾ��������ǵ�Ч�ģ�set_spi_mode(SPI1, Spi_End);//ȫ��ʹ��Ĭ��ֵ
// Ҳ���԰������ set_spi_mode(SPI1, Spi_P24_5_6_7, Low_Rising, Spi_End);//ֻ���������ź���������
void set_spi_mode(spi_name spi, ...)
{
    char *arg;
    va_list args;        // �ɱ�����б�
    va_start(args, spi); // ��ʼ���ɱ�����б�
    ma_sl = 1;en_dis = 1; iosw = 0; spi_sw = 0; div = 2; msblsb = 0; cpol = 1; cpha = 1; //���ò���Ĭ��ֵ
    while (1)
    {
        arg = va_arg(args, char *);
        if (sscanf(arg, "en%c", &_char) == 1)break;                               // �����ڱ�������
        ma_sl = (sscanf(arg, "\x01masl%d", &_ma_sl) == 1) ? _ma_sl : ma_sl;       // ��������ģʽ���Ǵӻ�ģʽ
        en_dis = (sscanf(arg, "\x01en_dis%d", &_en_dis) == 1) ? _en_dis : en_dis; // ����ʹ�ܻ��ǽ���
        iosw = (sscanf(arg, "\x01sw%d", &_iosw) == 1) ? _iosw : iosw;             // ����MISO��MOSI�Ƿ񽻻�
        spi_sw = (sscanf(arg, "spi%d", &_spi_sw) == 1) ? _spi_sw : spi_sw;        // ����SPI�����л������
        div = (sscanf(arg, "div%d", &_div) == 1) ? _div : div;                    // ����SPIʱ�ӷ�Ƶ
        msblsb = (sscanf(arg, "sign%d", &_msblsb) == 1) ? _msblsb : msblsb;       // ������λ���ȣ�����/���գ����ǵ�λ����
        cpol = (sscanf(arg, "level%d", &_cpol) == 1) ? _cpol : cpol;              // ����ʱ���źż���
        cpha = (sscanf(arg, "clock%d", &_cpha) == 1) ? _cpha : cpha;              // ����ʱ���ź���λ
    }
    set_spi_setting(spi);
    va_end(args); // ����ɱ�����б�
}

// ���ڻ�ȡSPI�ĵ�ǰ״̬����ΪSPI��ȫ˫���ģ����Ե�������ɣ�ͬʱҲ�ǽ�����ɣ���ʱ��ͻ᷵��1�����򷵻�0
char get_spi_state(spi_name spi)
{
    char state;
    if(((DMA_UR1T_STA|DMA_UR2T_STA|spi_flag)&1)==0)
        return 0; // ��ǰ����
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
    return state;            // �����ж�״̬
}

// ���ڷ���SPI�����ݣ���ΪHex_Mode��Buff_Mode����ģʽ
// Hex_ModeΪ���ֽڷ��ͣ�������ֱ�Ӳ���SPI�����ݼĴ�������һ��byte������
// Buff_ModeΪ���������ͣ���Ҫ����һ��char���͵�����ָ�룬Ȼ���ٴ������跢�͵����ݳ���
void spi_printf(spi_name spi, ...)
{
    char *arg;
    int dat_len = 0,hex_dat;
    va_list args;        // �ɱ�����б�
    va_start(args, spi); // ��ʼ���ɱ�����б�
    arg = va_arg(args, char *);
    if(arg[0] == '\x01')
    {//Ϊhex_mode
        dat_len = 0;hex_dat = va_arg(args, int);
        switch (spi){
        case SPI0:_spi0_tx_buff[0] = hex_dat;break;
        case SPI1:_spi1_tx_buff[0] = hex_dat;break;
        case SPI2:_spi2_tx_buff[0] = hex_dat;break;
        default:break;}
    }else
    {//Ϊbuff_mode
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
    va_end(args); // ����ɱ�����б�
}

void spi_isr(void) interrupt DMA_SPI_VECTOR
{
    DMA_SPI_STA = 0x00;spi_flag = 1;
}