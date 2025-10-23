/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#ifndef __TYPE_DEF_H
#define __TYPE_DEF_H

//========================================================================
//                               ���Ͷ���
//========================================================================

typedef unsigned char   u8;     //  8 bits 
typedef unsigned int    u16;    // 16 bits 
typedef unsigned long   u32;    // 32 bits 

typedef signed char     int8;   //  8 bits 
typedef signed int      int16;  // 16 bits 
typedef signed long     int32;  // 32 bits 

typedef unsigned char   uint8;  //  8 bits 
typedef unsigned int    uint16; // 16 bits 
typedef unsigned long   uint32; // 32 bits 

typedef bit BOOL;               //  1 bits 
typedef unsigned char   BYTE;   //  8 bits 
typedef unsigned int    WORD;   // 16 bits
typedef unsigned long   DWORD;  // 32 bits 

//===================================================

#define	TRUE	1
#define	FALSE	0

//===================================================

#define	NULL	0

//===================================================

#define	Priority_0			0	//�ж����ȼ�Ϊ 0 ������ͼ���
#define	Priority_1			1	//�ж����ȼ�Ϊ 1 �����ϵͼ���
#define	Priority_2			2	//�ж����ȼ�Ϊ 2 �����ϸ߼���
#define	Priority_3			3	//�ж����ȼ�Ϊ 3 ������߼���

#define ENABLE		1
#define DISABLE		0

#define SUCCESS		0
#define FAIL		-1


#endif
