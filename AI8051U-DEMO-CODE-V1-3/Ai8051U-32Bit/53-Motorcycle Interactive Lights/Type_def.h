#ifndef		__TYPE_DEF_H
#define		__TYPE_DEF_H

//========================================================================
//                               类型定义
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

//===================================================

#define	TRUE	1
#define	FALSE	0

//===================================================

#define	NULL	0

//===================================================

#define	Priority_0			0	//中断优先级为 0 级（最低级）
#define	Priority_1			1	//中断优先级为 1 级（较低级）
#define	Priority_2			2	//中断优先级为 2 级（较高级）
#define	Priority_3			3	//中断优先级为 3 级（最高级）

#define ENABLE		1
#define DISABLE		0

#define SUCCESS		0
#define FAIL		-1


#endif
