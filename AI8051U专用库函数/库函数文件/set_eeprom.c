#include "set_eeprom.h"
#include "math.h"
#include "INTRINS.H"
#include "stdarg.h"

unsigned long static _main_fosc = 40e6; // Ĭ��Ϊ40Mhz,��Ƶ��
char _set_eeprom_fosc = 0;                //Ĭ����û�и��ĵģ�������Ƶ���Ϊ1���������Զ���ȡ����



struct EEPROM_Pack xdata EEPROM_Pack_List[EEPROM_Pack_Max];//���ڴ洢�ڵ���Ϣ
static unsigned int xdata EEPROM_Pack_Index = 0;//���ڴ洢��ǰ�Ľڵ�����
static long xdata EEPROM_Pack_Addr = -1;//��ǰ���ݲ�������λ��

float static fosc_base[16] = {5.5296e6f, 6e6f, 11.0592e6f, 12e6f, 18.432e6f, 20e6f,
    22.1184e6f, 24e6f, 27e6f, 30e6f, 33.1176e6f, 35e6f, 36.864e6f, 40e6f, 42e6f, 43e6f};
static void get_main_fosc(void) // ��ѯһ�ε�ǰ��ʱ��Ƶ�ʣ�ֻ�ܲ�ѯʹ���ڲ�IRC��ֵ��δ��ѯ���Ͳ��ı�_main_fosc��ֵ
{   //�˺�������ʱռ��T4��T11��ʱ�����м��㣬ʹ����ɺ������ͷ�
    char _CLKDIV, _CLKSEL, _T11CR, _T4T3M, _IE, _IE2, _T4H, _T4L, _T11H, _T11L; //��ʱ�����ֻ���
    long _xM_Value = 0;//��׼ʱ�Ӻʹ���ʱ���µ�ֵ 
    if(_set_eeprom_fosc == 1)return;//����Ѿ����ù������˳�
    _CLKDIV = CLKDIV; _CLKSEL = CLKSEL; _T11CR = T11CR; _T4T3M = T4T3M;
    _IE2 = IE2; _IE = IE; EA = 0; _T4H = T4H; _T4L = T4L;
    _T11H = T11H; _T11L = T11L;//�������в���
    if((_CLKSEL&0x0f)!=0)return;//��ǰѡ��Ĳ����ڲ�IRCʱ�ӣ����˳�
    IRC48MCR = 0x80; while(!(IRC48MCR&1));//�������ȴ��ڲ�48Mʱ���ȶ�
    CLKDIV = 2;//�л�Ϊ2��Ƶ, ˳�򲻿��Դ��ң�����ֱ�Ӹ�48Mhzʱ�ӻ�����쳣
    CLKSEL |= 0x0c;//ѡ���ڲ�48Mʱ����Ϊϵͳʱ��Դ����ʱsysclk=24Mhz
    T4T3M &= ~0xf0; ET4 = 0;//���T4���ֵļĴ�����Ĭ��״̬���ر��жϣ���ֹ��ת���жϲ��ֺ��ܷ�
    T11CR = 0x14;//�л�Ϊ�ڲ�����IRC��������ʱ�ӣ�1Tģʽ
    T4x12 = 1;//ʹ��1Tģʽ
    T11CR &= ~0x01;//����жϱ�־λ
    T11H = T11L = 0;//���㿪ʼ����
    T4H = 0x80; T4L = 0x00;//32768��clk�����
    T4R = 1;//����T4��ʱ��
    T11CR |= 0x80;//����T11��ʱ��
    while(!(T4IF));//�ȴ�T4�ж�����
    T11CR &= ~0x81;//����жϱ�־λ��ͬʱ�ر�T11
    T4R = 0;//�ر�T4��ʱ��
    _xM_Value = (unsigned long)(((unsigned int)T11H<<8)|(unsigned int)T11L);//��¼ֵ
    T4IF = 0;//����жϱ�־λ
    if(_CLKDIV == 0)_CLKDIV = 1;//�������Ϊ0
    _main_fosc = (unsigned long)((732.421875f)*(float)((float)_xM_Value/(float)_CLKDIV));
    for(_xM_Value = 0; _xM_Value < 15; _xM_Value++){
        if(fabs((float)_main_fosc - fosc_base[_xM_Value])<3e5f){//��ֵ��Χ��0.3Mhz��
            _main_fosc = fosc_base[_xM_Value];}}//�滻��Ԥ��Ƶ��
    T4H = _T4H; T4L = _T4L; T11H = _T11H; T11L = _T11L;
    T11CR = _T11CR; T4T3M = _T4T3M;//�ָ���ʱ������
    CLKSEL = _CLKSEL; CLKDIV = _CLKDIV;//�ָ�ʱ������
    IE2 = _IE2; IE = _IE;//�ָ��жϲ�������
    _set_eeprom_fosc = 1;//��ȡһ�κ����ظ���ȡ
    IAP_TPS = (char)(_main_fosc/1e6);//����IAPʱ��Ƶ��
}

//���ڿ��ټ���0�ĸ���������ֵ0~8(��������)
char add_zeros(unsigned char dat)
{
    char cnt = 0;
    if(dat == 0)return 8;
    if ((dat & 0xF0) == 0) {cnt += 4;dat <<= 4;} // ��4λȫΪ0,����4λ�������ʣ��λ
    if ((dat & 0xC0) == 0) {cnt += 2;dat <<= 2;}// ��2λȫΪ0
    if ((dat & 0x80) == 0) {cnt += 1;}// ���λΪ0
    return cnt;
}

//����ʹ�ú���
unsigned char set_eeprom_base(eeprom_mode mode, unsigned long addr, ...)
{
    unsigned char dat;
    unsigned int len, cnt = 0;
    char *arg;
    va_list args;         // �ɱ�����б�
    va_start(args, addr); // ��ʼ���ɱ�����б�
    switch (mode)
    {
    case Write_Byte:dat = va_arg(args, char);len = 1;break;
    case Write_Buff:case Read_Buff:arg = va_arg(args, char *);len = va_arg(args, int); break;
    case Erase_Sectors:len = va_arg(args, int);break;
    case Read_Byte:case Erase_Sector:len = 1;break;//����ִ��
    default:return 0;// ��֧�ֵ�ָ��
    }
    va_end(args); // ����ɱ�����б�
    IAP_CONTR = 0x80;//ʹ��IAP����
    IAP_CMD = (mode/10)&3;
    for(cnt = 0; cnt < len; cnt++)
    {
        IAP_ADDRL = (addr+cnt);//���� IAP �͵�ַ
        if(mode == Erase_Sectors){
            IAP_ADDRH = (addr+(cnt<<9)) >> 8;//���� IAP �ߵ�ַ
        }else{
            IAP_ADDRH = (addr+cnt) >> 8;//���� IAP �ߵ�ַ
            // IAP_ADDRE = (addr+cnt) >> 16;//���� IAP ��ߵ�ַ
        }
        IAP_ADDRE = 0;//���� IAP ��ߵ�ַ
        if(mode == Write_Byte)IAP_DATA = dat;//д������
        if(mode == Write_Buff)IAP_DATA = arg[cnt];//д������
        IAP_TRIG = 0x5A;//����IAP
        IAP_TRIG = 0xA5;//����IAP
        NOP4();//�ȴ�IAPִ�����
        if(mode == Read_Byte)dat = IAP_DATA;//��ȡ����
        if(mode == Read_Buff)arg[cnt] = IAP_DATA;//��ȡ����
    }
    IAP_CONTR = 0;//�ر�IAP����
    return dat;
}

void set_eeprom_mode(const char *mode, void *value_addr, unsigned int len)
{
    get_main_fosc();//��ѯһ����ʱ��Ƶ��
    if(EEPROM_Pack_Index >= EEPROM_Pack_Max)return;//�ڵ㳬�����ֵ������
    if(len == 0)return;//0���Ȳ�����
    if(mode[0] == 0x01){//hex mode
        EEPROM_Pack_List[EEPROM_Pack_Index].addr = value_addr;//�洢�ڵ��ַ
        EEPROM_Pack_List[EEPROM_Pack_Index].len = len;//�洢�ڵ㳤��
        if(EEPROM_Pack_Index < EEPROM_Pack_Max)EEPROM_Pack_Index++;//�ڵ�������һ
    }else{//buff mode
        //��ʱ�������Ȳ�д���
    }
}

int last_len = 0;
void set_eeprom_sync(const char *sync)
{
    int num, zero_addr = 0;
    unsigned char dat, dat_cnt, ff_cnt = 0, zero_cnt = 0;
    if(EEPROM_Pack_Addr == -1){//�����ʼ��ַΪ-1������Ҫ��ѯһ��
        for(num = 0; num < EEPROM_Pack_Len; num++){//���Ȳ�ѯһ����ʼ��ַ
            dat = set_eeprom_base(Read_Byte, (unsigned long)(EEPROM_Offset+num));
            if(dat == 0xff)ff_cnt++;//Ѱ�ҿյ�
            if(ff_cnt >= 32){ff_cnt = 0;EEPROM_Pack_Addr = num-31;break;}//���ٲ���
            if(dat == 0xaa){//Ѱ�ұ�־λ
                dat = set_eeprom_base(Read_Byte, (unsigned long)(EEPROM_Offset+num+1));
                EEPROM_Pack_Addr = num+dat+3; //���ҵ�����и�ֵ
                last_len = (dat+3);break;}}
        if(num == EEPROM_Pack_Len){
            EEPROM_Pack_Addr = EEPROM_Offset;//û�ҵ�������г�ʼ��
            set_eeprom_base(Erase_Sectors, (unsigned long)(EEPROM_Offset), EEPROM_Pack_Len/512);}}//û���ҵ��յأ����в���
    if(sync[0]== 0x03){//push,���ͽڵ����ݵ�EEPROM
        for(dat_cnt = 0; dat_cnt < last_len; dat_cnt++)
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr-last_len+dat_cnt),0x00);//����ٽ�ǰ����������
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),0xaa);//д���ͷ
        dat = 0;
        for(num = 0; num < EEPROM_Pack_Index; num++){dat += (unsigned char)(EEPROM_Pack_List[num].len);}
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//д�������
        last_len = (int)(dat+3);//��¼��ʷ����
        //�ж��Ƿ񳬹������ȣ������Ļ���������³�ʼ��
        if((EEPROM_Pack_Addr+dat+4) > (EEPROM_Offset+EEPROM_Pack_Len))//Ԥ��4�İ�ȫ�߽�
        {
            set_eeprom_base(Erase_Sectors, (unsigned long)(EEPROM_Offset), EEPROM_Pack_Len/512);//û���ҵ��յأ����в���
            EEPROM_Pack_Addr = EEPROM_Offset;
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),0xaa);//����д���ͷ
            set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//����д�������
        }
        dat = 0;//������Ϊadd8�ļ���
        for(num = 0; num < EEPROM_Pack_Index; num++){
            set_eeprom_base(Write_Buff,(unsigned long)(EEPROM_Pack_Addr),
            EEPROM_Pack_List[num].addr, (int)(EEPROM_Pack_List[num].len));//д������
            for(dat_cnt = 0; dat_cnt < EEPROM_Pack_List[num].len; dat_cnt++){
                dat += ((char *)EEPROM_Pack_List[num].addr)[dat_cnt];}
            EEPROM_Pack_Addr += EEPROM_Pack_List[num].len;//��ַ�ӳ�
            }//����add8
        set_eeprom_base(Write_Byte,(unsigned long)(EEPROM_Pack_Addr++),(char)dat);//д��add8��Ϊ��β
        }
    else{//pull,��EEPROM�ж�ȡ���ݵ��ڵ���
        zero_addr = (EEPROM_Pack_Addr-last_len);
        dat = set_eeprom_base(Read_Byte,(unsigned long)(zero_addr++));
        if(dat != 0xaa)return;//��ȡ��ͷ���쳣�˳�
        dat_cnt = set_eeprom_base(Read_Byte,(unsigned long)(zero_addr++));//��ȡ�����ȣ���ֹ��������
        zero_cnt = 0;dat = 0;//��ռ�������dat��ΪADD8У�飬��ռ���
        for(num = 0; num < dat_cnt; num++)//����������
            dat += set_eeprom_base(Read_Byte,(unsigned long)(zero_addr+zero_cnt++));//��ȡ����
        if(dat != set_eeprom_base(Read_Byte,(unsigned long)(zero_addr+zero_cnt)))return;//У��add8��ͨ�����˳�
        // EEPROM_Pack_Addr += (dat_cnt+3);//��ַ�ӳ�,����Push������������
        last_len = (int)(dat_cnt+3);//��¼��ʷ����
        zero_cnt = 0;//��ռ�����
        for(num = 0; num < EEPROM_Pack_Index; num++){//�����ڵ�
            for(dat_cnt = 0; dat_cnt < EEPROM_Pack_List[num].len; dat_cnt++)
            {((char *)EEPROM_Pack_List[num].addr)[dat_cnt] = set_eeprom_base(Read_Byte,
                    (unsigned long)(zero_addr+zero_cnt++));
                if(zero_cnt >= (last_len-3))return;}}//��ȡ������˳�
    }
}