#ifndef __SET_EEPROM_H__
#define __SET_EEPROM_H__

#include "AI8051U.H"

//����ĥ���Զ�ռ��EEPROM�ĵ�һ������(512Byte)�������Զ���������ʼֵ�ͳ���ֵ
#define EEPROM_Offset 0
#define EEPROM_Pack_Len 512
#define EEPROM_Pack_Max 10 //���������԰󶨶��ٸ�����������������Ե���û�а�ʹ�õĲ��ֲ������EEPROM�洢
//EEPROM��������������ģʽ
typedef enum
{
    Read_Byte = 10,//��ȡEEPROM,1���ֽ�(������ЧΪ:char set_eeprom_base(Read_Byte, unsigned long addr);)
    Read_Buff = 11,//��ȡEEPROM,����ֽ�(������ЧΪ:void set_eeprom_base(Read_Buff, unsigned long addr, char* buf, int len);)
    Write_Byte = 20,//д��EEPROM,1���ֽ�(������ЧΪ:void set_eeprom_base(Write_Byte, unsigned long addr, char value);)
    Write_Buff = 21,//д��EEPROM,����ֽ�(������ЧΪ:void set_eeprom_base(Write_Buff, unsigned long addr, char* buf, int len);)
    Erase_Sector = 30,//����EEPROM,1������(������ЧΪ:void set_eeprom_base(Erase_Sector, unsigned long addr);)
    Erase_Sectors = 31//����EEPROM,�������(������ЧΪ:void set_eeprom_base(Erase_Sectors, unsigned long addr, int len);)
} eeprom_mode;


struct EEPROM_Pack 
{
    void *addr;//���ݵ�ַ
    unsigned char len;//���ݳ���
};

#define Hex_Mode "\x01 hex"   // ���ֽ�ģʽ, ���ڰ󶨵�������
// #define Buff_Mode "\x02 buff" // ����ģʽ������ͬʱ�󶨶��������Ŀǰ��δ֧�֣��ȴ���������

#define Push "\x03 push" //���ڽ��Ѿ��󶨵ı���ֵ���͵�EEPROM�У������Ǵ�RAM->EEPROM
#define Pull "\x04 pull" //���ڽ�EEPROM�е�ֵ���͵�RAM�У������Ǵ�EEPROM->RAM

//EEPROM����ʹ�ú�����������ɵײ��������������ʹ�ú����ı����󶨷�ʽ
//�����ĺ������о���ĥ���ܣ����Ը������ʹ��
//����������ʹ�÷�ʽ��鿴eeprom_mode��ö�ٶ���ע�ͣ��ж�Ӧ�ĵ�Ч����
unsigned char set_eeprom_base(eeprom_mode mode, unsigned long addr, ...);

//���ڰ󶨱�����EEPROM����ʵ�����Ƶ��粻�仯�ı�����Ч����
//��֮�󣬵��ú����set_eeprom_sync()����������ʵ�ִ�EEPROM->RAM������ߴ�RAM->EEPROM�����ͬ��
//ʹ��ʾ��������һ��int������int num; Ȼ����������󶨽�EEPROM��
//set_eeprom_mode(Hex_Mode,&num,sizeof(num));
void set_eeprom_mode(const char *mode, void *value_addr, unsigned int len);

//����ͬ��EEPROM�е�ֵ��RAM�У����ߴ�RAM��ͬ����EEPROM�У�ͨ������Push����Pull��ѡ����
void set_eeprom_sync(const char *sync);

//����ĥ������ݴ洢��ʽ��0xaa|len|���ݲ���|add8
//�洢���Զ������ղ������洢���ݣ�����洢λ�ò��㣬���Զ��������������´洢����
//ÿ��pull��ʱ�򣬷�����洢��ʽ��ƥ�䣬����˳�ƥ�䣬֮ǰƥ��Ĳ���Ӱ��
//ÿ��push��ʱ�򣬻��Զ�����һ�ε����ݰ��е��������ݸ�д��0x00
//�Է��������ݸ�������

#endif