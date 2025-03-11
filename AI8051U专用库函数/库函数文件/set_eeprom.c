#include "set_eeprom.h"
#include "math.h"
#include "INTRINS.H"
#include "stdarg.h"

unsigned long static _main_fosc = 40e6; // 默认为40Mhz,主频率
char _set_eeprom_fosc = 0;                //默认是没有更改的，设置主频后变为1，以屏蔽自动获取部分



struct EEPROM_Pack xdata EEPROM_Pack_List[EEPROM_Pack_Max];//用于存储节点信息
static unsigned int xdata EEPROM_Pack_Index = 0;//用于存储当前的节点索引
static long xdata EEPROM_Pack_Addr = -1;//当前数据操作到的位置

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // 查询一次当前的时钟频率，只能查询使用内部IRC的值，未查询到就不改变_main_fosc的值
{   //此函数会临时占用T4和T11定时器进行计算，使用完成后会进行释放
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L; //定时器部分缓存
    long _xM_Value = 0;//标准时钟和待测时钟下的值 
    if(_set_eeprom_fosc == 1)return;//如果已经设置过，则退出
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; _T4T3M = T4T3M;
    _IE2 = IE2; _IE = IE; EA = 0; _T4H = T4H; _T4L = T4L;
    _T11H = T11H; _T11L = T11L;//缓存所有参数
    if((_CLKSEL&0x0f)!=0)return;//当前选择的不是内部IRC时钟，则退出
    IRC48MCR = 0x80; while(!(IRC48MCR&1));//启动并等待内部48M时钟稳定
    CLKDIV = 2;//切换为2分频, 顺序不可以打乱，否则直接给48Mhz时钟会出现异常
    CLKSEL |= 0x0c;//选择内部48M时钟作为系统时钟源，此时sysclk=24Mhz
    T4T3M &= ~0xf0; ET4 = 0;//清除T4部分的寄存器到默认状态。关闭中断，防止跳转到中断部分后跑飞
    T11CR = 0x14;//切换为内部高速IRC，即待测时钟，1T模式
    T4x12 = 1;//使用1T模式
    T11CR &= ~0x01;//清除中断标志位
    T11H = T11L = 0;//从零开始计数
    T4H = 0x80; T4L = 0x00;//32768个clk后溢出
    T4R = 1;//启动T4计时器
    T11CR |= 0x80;//启动T11计时器
    while(!(T4IF));//等待T4中断来临
    T11CR &= ~0x81;//清除中断标志位，同时关闭T11
    T4R = 0;//关闭T4计时器
    _xM_Value = (unsigned long)(((unsigned int)T11H<<8)|(unsigned int)T11L);//记录值
    T4IF = 0;//清除中断标志位
    if(_CLKDIV == 0)_CLKDIV = 1;//避免除数为0
    _main_fosc = (unsigned long)((732.421875f)*(float)((float)_xM_Value/(float)_CLKDIV));
    for(_xM_Value = 0; _xM_Value < 15; _xM_Value++){
        if(fabs((float)_main_fosc - fosc_base[_xM_Value])<3e5f){//差值范围在0.3Mhz内
            _main_fosc = fosc_base[_xM_Value];}}//替换成预制频率
    T4H = _T4H; T4L = _T4L; T11H = _T11H; T11L = _T11L;
    T11CR = _T11CR; T4T3M = _T4T3M;//恢复定时器设置
    CLKSEL = _CLKSEL; CLKDIV = _CLKDIV;//恢复时钟配置
    IE2 = _IE2; IE = _IE;//恢复中断部分配置
    _set_eeprom_fosc = 1;//获取一次后不再重复获取
    IAP_TPS = (char)(_main_fosc/1e6);//计算IAP时钟频率
}

//用于快速计算0的个数，返回值0~8(代表几个零)
char add_zeros(unsigned char dat)
{
    char cnt = 0;
    if(dat == 0)return 8;
    if ((dat & 0xF0) == 0) {cnt += 4;dat <<= 4;} // 高4位全为0,左移4位继续检测剩余位
    if ((dat & 0xC0) == 0) {cnt += 2;dat <<= 2;}// 高2位全为0
    if ((dat & 0x80) == 0) {cnt += 1;}// 最高位为0
    return cnt;
}

//基础使用函数
unsigned char set_eeprom_base(eeprom_mode mode, unsigned long addr, ...)
{
    unsigned char dat;
    unsigned int len, cnt = 0;
    char *arg;
    va_list args;         // 可变参数列表
    va_start(args, addr); // 初始化可变参数列表
    switch (mode)
    {
    case Write_Byte:dat = va_arg(args, char);len = 1;break;
    case Write_Buff:case Read_Buff:arg = va_arg(args, char *);len = va_arg(args, int); break;
    case Erase_Sectors:len = va_arg(args, int);break;
    case Read_Byte:case Erase_Sector:len = 1;break;//单次执行
    default:return 0;// 不支持的指令
    }
    va_end(args); // 清理可变参数列表
    IAP_CONTR = 0x80;//使能IAP功能
    IAP_CMD = (mode/10)&3;
    for(cnt = 0; cnt < len; cnt++)
    {
        IAP_ADDRL = (addr+cnt);//设置 IAP 低地址
        if(mode == Erase_Sectors){
            IAP_ADDRH = (addr+(cnt<<9)) >> 8;//设置 IAP 高地址
        }else{
            IAP_ADDRH = (addr+cnt) >> 8;//设置 IAP 高地址
            // IAP_ADDRE = (addr+cnt) >> 16;//设置 IAP 最高地址
        }
        IAP_ADDRE = 0;//设置 IAP 最高地址
        if(mode == Write_Byte)IAP_DATA = dat;//写入数据
        if(mode == Write_Buff)IAP_DATA = arg[cnt];//写入数据
        IAP_TRIG = 0x5A;//触发IAP
        IAP_TRIG = 0xA5;//触发IAP
        NOP4();//等待IAP执行完成
        if(mode == Read_Byte)dat = IAP_DATA;//读取数据
        if(mode == Read_Buff)arg[cnt] = IAP_DATA;//读取数据
    }
    IAP_CONTR = 0;//关闭IAP功能
    return dat;
}

void set_eeprom_mode(const char *mode, void *value_addr, unsigned int len)
{
    get_main_fosc();//查询一次主时钟频率
    if(EEPROM_Pack_Index >= EEPROM_Pack_Max)return;//节点超过最大值不处理
    if(len == 0)return;//0长度不处理
    if(mode[0] == 0x01){//hex mode
        EEPROM_Pack_List[EEPROM_Pack_Index].addr = value_addr;//存储节点地址
        EEPROM_Pack_List[EEPROM_Pack_Index].len = len;//存储节点长度
        if(EEPROM_Pack_Index < EEPROM_Pack_Max)EEPROM_Pack_Index++;//节点索引加一
    }else{//buff mode
        //暂时保留，先不写这个
    }
}

int last_len = 0;
void set_eeprom_sync(const char *sync)
{
    int num, zero_addr = 0;
    unsigned char dat, dat_cnt, ff_cnt = 0, zero_cnt = 0;
    if(EEPROM_Pack_Addr == -1){//如果起始地址为-1，则需要查询一次
        for(num = 0; num < EEPROM_Pack_Len; num++){//首先查询一次起始地址
            dat = set_eeprom_base(Read_Byte, (unsigned long)(EEPROM_Offset+num));
            if(dat == 0xff)ff_cnt++;//寻找空地
            if(ff_cnt >= 32){ff_cnt = 0;EEPROM_Pack_Addr = num-31;break;}//加速查找
            if(dat == 0xaa){//寻找标志位
                dat = set_eeprom_base(Read_Byte, (unsigned long)(EEPROM_Offset+num+1));
                EEPROM_Pack_Addr = num+dat+3; //查找到则进行赋值
                last_len = (dat+3);break;}}
        if(num == EEPROM_Pack_Len){
            EEPROM_Pack_Addr = EEPROM_Offset;//没找到，则进行初始化
            set_eeprom_base(Erase_Sectors, (unsigned long)(EEPROM_Offset), EEPROM_Pack_Len/512);}}//没查找到空地，进行擦除
    if(sync[0]== 0x03){//push,推送节点数据到EEPROM
        for(dat_cnt = 0; dat_cnt < last_len; dat_cnt++)
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr-last_len+dat_cnt),0x00);//最后再将前面的数据清空
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),0xaa);//写入包头
        dat = 0;
        for(num = 0; num < EEPROM_Pack_Index; num++){dat += (unsigned char)(EEPROM_Pack_List[num].len);}
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//写入包长度
        last_len = (int)(dat+3);//记录历史长度
        //判断是否超过允许长度，超过的话则进行重新初始化
        if((EEPROM_Pack_Addr+dat+4) > (EEPROM_Offset+EEPROM_Pack_Len))//预留4的安全边界
        {
            set_eeprom_base(Erase_Sectors, (unsigned long)(EEPROM_Offset), EEPROM_Pack_Len/512);//没查找到空地，进行擦除
            EEPROM_Pack_Addr = EEPROM_Offset;
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),0xaa);//重新写入包头
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//重新写入包长度
        }
        dat = 0;//用于作为add8的计算
        for(num = 0; num < EEPROM_Pack_Index; num++){
            set_eeprom_base(Write_Buff,(unsigned long)(EEPROM_Pack_Addr),
            EEPROM_Pack_List[num].addr, (int)(EEPROM_Pack_List[num].len));//写入数据
            for(dat_cnt = 0; dat_cnt < EEPROM_Pack_List[num].len; dat_cnt++){
                dat += ((char *)EEPROM_Pack_List[num].addr)[dat_cnt];}
            EEPROM_Pack_Addr += EEPROM_Pack_List[num].len;//地址加长
            }//计算add8
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//写入add8作为包尾
        }
    else{//pull,从EEPROM中读取数据到节点中
        zero_addr = (EEPROM_Pack_Addr-last_len);
        dat = set_eeprom_base(Read_Byte,(unsigned long)(zero_addr++));
        if(dat != 0xaa)return;//读取包头，异常退出
        dat_cnt = set_eeprom_base(Read_Byte,(unsigned long)(zero_addr++));//读取包长度，防止后面读溢出
        zero_cnt = 0;dat = 0;//清空计数器，dat作为ADD8校验，清空计数
        for(num = 0; num < dat_cnt; num++)//遍历包长度
            dat += set_eeprom_base(Read_Byte,(unsigned long)(zero_addr+zero_cnt++));//读取数据
        if(dat != set_eeprom_base(Read_Byte,(unsigned long)(zero_addr+zero_cnt)))return;//校验add8不通过则退出
        // EEPROM_Pack_Addr += (dat_cnt+3);//地址加长,方便Push操作数据推送
        last_len = (int)(dat_cnt+3);//记录历史长度
        zero_cnt = 0;//清空计数器
        for(num = 0; num < EEPROM_Pack_Index; num++){//遍历节点
            for(dat_cnt = 0; dat_cnt < EEPROM_Pack_List[num].len; dat_cnt++)
            {((char *)EEPROM_Pack_List[num].addr)[dat_cnt] = set_eeprom_base(Read_Byte,
                    (unsigned long)(zero_addr+zero_cnt++));
                if(zero_cnt >= (last_len-3))return;}}//读取溢出则退出
    }
}