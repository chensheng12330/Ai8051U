
#include	"AI8051U.h"

/*************  FLASH��ر�������   **************/
sbit    P_FLASH_CE   = P4^0;     //PIN1
sbit    P_FLASH_MOSI = P4^1;     //PIN5
sbit    P_FLASH_MISO = P4^2;     //PIN2
sbit    P_FLASH_SCLK = P4^3;     //PIN6
sbit    P_FLASH_WP   = P5^2;     //PIN3
sbit    P_FLASH_HOLD = P5^3;     //PIN7

#define	SPI_CE_High()	P_FLASH_CE		= 1		// set CE high
#define	SPI_CE_Low()	P_FLASH_CE		= 0		// clear CE low
#define	SPI_Hold()		P_FLASH_Hold	= 0		// clear Hold pin
#define	SPI_UnHold()	P_FLASH_Hold	= 1		// set Hold pin
#define	SPI_WP()		P_FLASH_WP		= 0		// clear WP pin
#define	SPI_UnWP()		P_FLASH_WP		= 1		// set WP pin


//ID: W25Q40=0x12,  W25Q80=0x13, W25Q16=0x14, W25Q32=0x15, W25Q64=0x16, W25Q128=0x17, W25Q256=0x18, W25Q512=0x19,

/******************* FLASH��س��� ************************/
	#define SFC_WREN        0x06	//����д Write Enable
	#define SFC_WRDI        0x04	//��ֹд Write Disable
	#define SFC_VSRWREN     0x50	//Volatile Write Enable
	#define SFC_RDSR1       0x05	//��״̬�Ĵ���1  S7~S0
	#define SFC_WRSR1       0x01	//д״̬�Ĵ���1  S7~S0
	#define SFC_RDSR2       0x35	//��״̬�Ĵ���2  S15~S8
	#define SFC_WRSR2       0x31	//д״̬�Ĵ���2  S15~S8
	#define SFC_RDSR3       0x15	//��״̬�Ĵ���3  S23~S16
	#define SFC_WRSR3       0x11	//д״̬�Ĵ���3  S23~S16
	#define SFC_RDSFDPR     0x5A	//Read SFDP Register

	#define SFC_READ         0x03	//������
	#define SFC_FastRead     0x0B	//���ٶ�
	#define SFC_FastReadDual 0x3B	//���ٶ�2��
	#define SFC_FastReadQuad 0x6B	//���ٶ�4��
	#define SFC_FastReadDualIO 0xBB	//���ٶ�2��
	#define SFC_FastReadQuadIO 0xEB	//���ٶ�4��

	#define SFC_RDID        0xAB	//��ID
	#define SFC_RDMFID      0x90	//��MF ID
	#define SFC_PAGEPROG    0x02	// ҳд��, ���24λ��ַA23~A16 A15~A8 A7~A0 D7~D0 D7~D0.....
	#define SFC_QPAGEPROG   0x32	//4ҳд��, ���24λ��ַA23~A16 A15~A8 A7~A0 D7~D0 D7~D0.....
	#define SFC_SECTORER4K  0x20    // 4KB ��������ָ��
	#define SFC_SECTORER32K 0x52    //32KB ��������ָ��
	#define SFC_SECTORER64K 0xd8    //64KB ��������ָ��
	#define SFC_CHIPER      0xC7	//Ƭ����  0xC7��0x60


bit	B_FlashOK;
bit	B_SPI_DMA_busy;
u8	FLASH_ID;


/************************************************************************/
//========================================================================
// ����: void  SPI_Config(u8 SPI_io, u8 SPI_speed)
// ����: SPI��ʼ��������
// ����: io: �л�����IO,            SS  MOSI MISO SCLK
//                       0: �л��� P1.4 P1.5 P1.6 P1.7
//                       1: �л��� P2.4 P2.5 P2.6 P2.7
//                       2: �л��� P4.0 P4.1 P4.2 P4.3
//                       3: �л��� P3.5 P3.4 P3.3 P3.2
//       SPI_speed: SPI���ٶ�, 0: fosc/4,  1: fosc/8,  2: fosc/16,  3: fosc/2
// ����: none.
// �汾: VER1.0
// ����: 2024-8-13
// ��ע:
//========================================================================
void  SPI_Config(u8 SPI_io, u8 SPI_speed)
{
	SPI_io &= 3;

	SPCTL = SPI_speed & 3;	//����SPI �ٶ�, ����ָ����ִ��, ˳��Bit7~Bit2��0
	SSIG = 1;	//1: ����SS�ţ���MSTRλ�����������Ǵӻ�		0: SS�����ھ����������Ǵӻ���
	SPEN = 1;	//1: ����SPI��								0����ֹSPI������SPI�ܽž�Ϊ��ͨIO
	DORD = 0;	//1��LSB�ȷ���								0��MSB�ȷ�
	MSTR = 1;	//1����Ϊ����								0����Ϊ�ӻ�
	CPOL = 1;	//1: ����ʱSCLKΪ�ߵ�ƽ��					0������ʱSCLKΪ�͵�ƽ
	CPHA = 1;	//1: ������SCLKǰ������,���ز���.			0: ������SCLKǰ�ز���,��������.
//	SPR1 = 0;	//SPR1,SPR0   00: fosc/4,     01: fosc/8
//	SPR0 = 0;	//            10: fosc/16,    11: fosc/2
	P_SW1 = (P_SW1 & ~0x0c) | ((SPI_io<<2) & 0x0c);		//�л�IO

	B_SPI_DMA_busy = 0;
	HSCLKDIV   = 1;					//HSCLKDIV��ʱ�ӷ�Ƶ
	SPI_CLKDIV = 1;					//SPI_CLKDIV��ʱ�ӷ�Ƶ
	SPSTAT = 0x80 + 0x40;			//��0 SPIF��WCOL��־

	if(SPI_io == 0)
	{
		P1n_standard(0xf0);		//�л��� P1.4(SS) P1.5(MOSI) P1.6(MISO) P1.7(SCLK), ����Ϊ׼˫���
		PullUpEnable(P1PU, 0xf0);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P1n_push_pull(Pin7+Pin5);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P1SR, Pin7+Pin5);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 1)
	{
		P2n_standard(0xf0);			//�л���P2.4(SS) P2.5(MOSI) P2.6(MISO) P2.7(SCLK), ����Ϊ׼˫���
		PullUpEnable(P2PU, 0xf0);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P2n_push_pull(Pin7+Pin5);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P2SR, Pin7+Pin5);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 2)
	{
		P4n_standard(0x0f);			//�л���P4.0(SS) P4.1(MOSI) P4.2(MISO) P4.3(SCLK), ����Ϊ׼˫���
		PullUpEnable(P4PU, 0x0f);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P4n_push_pull(Pin3+Pin1);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P4SR, Pin3+Pin1);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}
	else if(SPI_io == 3)
	{
		P3n_standard(0x3C);		//�л���P3.5(SS) P3.4(MOSI) P3.3(MISO) P3.2(SCLK), ����Ϊ׼˫���
		PullUpEnable(P3PU, 0x3c);	//������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
		P3n_push_pull(Pin4+Pin2);	//MOSI SCLK����Ϊ�������
		SlewRateHigh(P3SR, Pin4+Pin2);	//MOSI SCLK�˿��������Ϊ����ģʽ   PxSR, Ҫ���õĶ˿ڶ�ӦλΪ1.    ����ģʽ��3.3V����ʱ�ٶȿ��Ե�13.5MHz(27MHz��Ƶ��SPI�ٶ�2��Ƶ)
	}

	P5n_standard(Pin3+Pin2);		//WP HOLD����Ϊ׼˫���
	PullUpEnable(P5PU, Pin3+Pin2);	//WP HOLD������������    ����˿��ڲ���������   PxPU, Ҫ���õĶ˿ڶ�ӦλΪ1
	P_FLASH_CE		= 1;		//PIN1		PIN8--VDD
	P_FLASH_MISO	= 1;		//PIN2      PIN7--HOLD
	P_FLASH_WP		= 1;		//PIN3
	P_FLASH_MOSI	= 1;		//PIN5      PIN4--GND
	P_FLASH_SCLK	= 1;		//PIN6
	P_FLASH_HOLD	= 1;		//PIN7
}


/************************************************************************/
void	SPI_WriteByte(u8 dat)
{
	SPDAT = dat;		//����һ���ֽ�
	while(SPIF == 0)	;			//�ȴ��������
	SPSTAT = 0x80 + 0x40;			//��0 SPIF��WCOL��־
}

/************************************************************************/
u8 SPI_ReadByte(void)
{
	u8	i;
	SPDAT = 0xff;		//����һ�����ֽ�
	while(SPIF == 0)	;			//�ȴ��������
	i = SPDAT;
	SPSTAT = 0x80 + 0x40;			//��0 SPIF��WCOL��־
	return i;//SPDAT;		//���շ��ص��ֽ�
}



/************************************************
���Flash�Ƿ�׼������, �κ�ʱ����ɶ�
��ڲ���: ��
���ڲ���:
    0 : û�м�⵽��ȷ��Flash
    1 : Flash׼������
************************************************/
void	FlashCheckID(void)
{
	SPI_CE_Low();
	FLASH_ID = 0;
	SPI_WriteByte(SFC_RDID);		//���Ͷ�ȡID����
	SPI_WriteByte(0x00);			//�ն�3���ֽ�
	SPI_WriteByte(0x00);
	SPI_WriteByte(0x00);			//ID ��ַ0x00��0x01
	FLASH_ID = SPI_ReadByte();		//��ȡ������ID
	SPI_CE_High();
}


/************************************************
���Flash��æ״̬
��ڲ���: ��
���ڲ���:
    0 : Flash���ڿ���״̬
    1 : Flash����æ״̬
************************************************/
u8	FlashCheckBusy(void)
{
	u8	dat;

	SPI_CE_Low();
	SPI_WriteByte(SFC_RDSR1);	//���Ͷ�ȡ״̬�Ĵ���1����
	dat = SPI_ReadByte();		//��ȡ״̬�Ĵ���1
	SPI_CE_High();

	return (dat & 1);			//״ֵ̬��Bit0��Ϊæ��־
}

/************************************************
ʹ��Flashд����
��ڲ���: ��
���ڲ���: ��
************************************************/
void	FlashWriteEnable(void)
{
	while(FlashCheckBusy() != 0);	//Flashæ���
	SPI_CE_Low();
	SPI_WriteByte(SFC_WREN);		//����дʹ������
	SPI_CE_High();
}

/************************************************
������ƬFlash
��ڲ���: ��
���ڲ���: ��
************************************************/
/*
void	FlashChipErase(void)
{
	if(B_FlashOK)
	{
		FlashWriteEnable();				//ʹ��Flashд����
		SPI_CE_Low();
		SPI_WriteByte(SFC_CHIPER);		//����Ƭ��������
		SPI_CE_High();
	}
}
*/

/************************************************
��������,
��ڲ���: u32 addr: ������ַ, u8 sec:������С4 32 64
���ڲ���: ��
************************************************/
void	FlashSectorErase(u32 addr, u8 sec)
{
	if(B_FlashOK)
	{
		FlashWriteEnable();				//ʹ��Flashд����
		SPI_CE_Low();
			 if(sec == 32)	SPI_WriteByte(SFC_SECTORER32K);	//����������������
		else if(sec == 64)	SPI_WriteByte(SFC_SECTORER64K);	//����������������
		else 				SPI_WriteByte(SFC_SECTORER4K);	//����������������
		SPI_WriteByte(((u8 *)&addr)[1]);           //������ʼ��ַ
		SPI_WriteByte(((u8 *)&addr)[2]);
		SPI_WriteByte(((u8 *)&addr)[3]);
		SPI_CE_High();
	}
}

/************************************************
��Flash�ж�ȡ����
��ڲ���:
    addr   : ��ַ����
    buffer : �����Flash�ж�ȡ������
    size   : ���ݿ��С
���ڲ���:
    ��
************************************************/
void	FlashRead_Nbytes(u32 addr, u8 *buffer, u16 size)
{
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	while(FlashCheckBusy() != 0);		//Flashæ���
	SPI_CE_Low();						//enable device
	SPI_WriteByte(SFC_READ); 			//read command
	SPI_WriteByte(((u8 *)&addr)[1]);	//������ʼ��ַ
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	do{
		*buffer = SPI_ReadByte();		//receive byte and store at buffer
		buffer++;
	}while(--size);						//read until no_bytes is reached
	SPI_CE_High();						//disable device
}


/************************************************
д���ݵ�Flash��
��ڲ���:
    addr   : ��ַ����
    buffer : ������Ҫд��Flash������
    size   : ���ݿ��С
���ڲ���: ��
************************************************/
void	FlashWrite_Nbytes(u32 addr, u8 *buffer, u16 size)
{
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	FlashWriteEnable();					//ʹ��Flashд����

	SPI_CE_Low();						// enable device
	SPI_WriteByte(SFC_PAGEPROG);		// ����ҳ�������
	SPI_WriteByte(((u8 *)&addr)[1]);	//������ʼ��ַ
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);
	do{
		SPI_WriteByte(*buffer++);		//����ҳ��д
		addr++;
		if ((addr & 0xff) == 0) break;
	}while(--size);
	SPI_CE_High();						// disable device
}



//DMA_SPI_CR 	SPI_DMA���ƼĴ���
#define		DMA_ENSPI		(1<<7)	// SPI DMA����ʹ�ܿ���λ��    bit7, 0:��ֹSPI DMA���ܣ�  1������SPI DMA���ܡ�
#define		SPI_TRIG_M		(1<<6)	// SPI DMA����ģʽ��������λ��bit6, 0:д0��Ч��          1��д1��ʼSPI DMA����ģʽ������
#define		SPI_TRIG_S		(0<<5)	// SPI DMA�ӻ�ģʽ��������λ��bit5, 0:д0��Ч��          1��д1��ʼSPI DMA�ӻ�ģʽ������
#define		SPI_CLRFIFO			0	// ���SPI DMA����FIFO����λ��bit0, 0:д0��Ч��          1��д1��λFIFOָ�롣


//DMA_SPI_CFG 	SPI_DMA���üĴ���
#define		DMA_SPIIE	(1<<7)	// SPI DMA�ж�ʹ�ܿ���λ��bit7, 0:��ֹSPI DMA�жϣ�     1�������жϡ�
#define		SPI_ACT_TX	(1<<6)	// SPI DMA�������ݿ���λ��bit6, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1�������͡�
#define		SPI_ACT_RX	(1<<5)	// SPI DMA�������ݿ���λ��bit5, 0:��ֹSPI DMA�������ݣ�����ֻ��ʱ�Ӳ������ݣ��ӻ�Ҳ����. 1��������ա�
#define		DMA_SPIIP	(0<<2)	// SPI DMA�ж����ȼ�����λ��bit3~bit2, (���)0~3(���).
#define		DMA_SPIPTY		0	// SPI DMA�������߷������ȼ�����λ��bit1~bit0, (���)0~3(���).

//DMA_SPI_CFG2 	SPI_DMA���üĴ���2
#define		SPI_WRPSS	(0<<2)	// SPI DMA������ʹ��SS�ſ���λ��bit2, 0: SPI DMA������̲��Զ�����SS�š�  1���Զ�����SS�š�
#define		SPI_SSS	    	0	// SPI DMA�������Զ�����SS��ѡ��λ��bit1~bit0, 0: P1.4,  1��P2.4,  2: P4.0,  3:P3.5��

//DMA_SPI_STA 	SPI_DMA״̬�Ĵ���
#define		SPI_TXOVW	(1<<2)	// SPI DMA���ݸ��Ǳ�־λ��bit2, �����0.
#define		SPI_RXLOSS	(1<<1)	// SPI DMA�������ݶ�����־λ��bit1, �����0.
#define		DMA_SPIIF		1	// SPI DMA�ж������־λ��bit0, �����0.

//HSSPI_CFG  ����SPI���üĴ���
#define		SS_HOLD		(3<<4)	//����ģʽʱSS�����źŵ�HOLDʱ�䣬 0~15, Ĭ��3. ��DMA�л�����N��ϵͳʱ�ӣ���SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.
#define		SS_SETUP		3	//����ģʽʱSS�����źŵ�SETUPʱ�䣬0~15, Ĭ��3. ��DMA�в�Ӱ��ʱ�䣬       ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.

//HSSPI_CFG2  ����SPI���üĴ���2
#define		SPI_IOSW	(0<<6)	//bit6:����MOSI��MISO��λ��0����������1������
#define		HSSPIEN		(0<<5)	//bit5:����SPIʹ��λ��0���رո���ģʽ��1��ʹ�ܸ���ģʽ
#define		FIFOEN		(0<<4)	//bit4:����SPI��FIFOģʽʹ��λ��0���ر�FIFOģʽ��1��ʹ��FIFOģʽ��ʹ��FIFOģʽ��DMA�м���13��ϵͳʱ�䡣
#define		SS_DACT			3	//bit3~0:����ģʽʱSS�����źŵ�DEACTIVEʱ�䣬0~15, Ĭ��3. ��SPI�ٶ�Ϊϵͳʱ��/2ʱִ��DMA��SS_HOLD��SS_SETUP��SS_DACT���������ô���2��ֵ.

void	SPI_DMA_RxTRIG(u32 addr, u8 *buffer, u16 size)	//ע�⣺����FIFO�ᵼ�¶���������ȫ����0xff.
{
	u16	i;		//@40MHz, Fosc/4, 200�ֽ�258us��100�ֽ�  130us��50�ֽ�66us��N���ֽں�ʱ N*1.280+2 us, 51Tһ���ֽڣ�����״̬��19T, �����ʱ32T.
				//@40MHz, Fosc/2, 200�ֽ�177us��100�ֽ� 89.5us��50�ֽ�46us��N���ֽں�ʱ N*0.875+2 us, 35Tһ���ֽڣ�����״̬��19T, �����ʱ16T.
				//@40MHz, Fosc/2, SPI DMA����һ���ֽ�, FIFO=1, HOLD=0����ʱ16+3=19T(0.475us), HOLD=3����ʱ16+6=22T(0.55us).
				//@40MHz, Fosc/4, SPI DMA����һ���ֽ�, FIFO=1, HOLD=0����ʱ32+3=35T(0.875us), HOLD=3����ʱ32+6=38T(0.95us).
	if(size == 0)	return;
	if(!B_FlashOK)	return;
	while(FlashCheckBusy() != 0);		//Flashæ���

	SPI_CE_Low();						//enable device
	SPI_WriteByte(SFC_READ); 			//read command
	SPI_WriteByte(((u8 *)&addr)[1]);	//������ʼ��ַ
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD������N��ϵͳʱ��, SS_SETUPû������ʱ�ӡ�
	HSSPI_CFG2 = SPI_IOSW | HSSPIEN | FIFOEN | SS_DACT;	//FIFOEN����FIFO���С13��ʱ��. @40MHz FIFOEN=1, SS_HOLD=0ʱ523us @2T, 943us @4T;    FIFOEN=1, SS_HOLD=3ʱ600us @2T, 1020us @4T.

	i = (u16)buffer;	//ȡ�׵�ַ
	DMA_SPI_RXAH = (u8)(i >> 8);		//���յ�ַ�Ĵ������ֽ�
	DMA_SPI_RXAL = (u8)i;				//���յ�ַ�Ĵ������ֽ�
	DMA_SPI_AMTH = (u8)((size-1)/256);	//���ô������ֽ��� = n+1
	DMA_SPI_AMT  = (u8)((size-1)%256);	//���ô������ֽ��� = n+1
	DMA_SPI_ITVH = 0;					//���ӵļ��ʱ�䣬N+1��ϵͳʱ��
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_RX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
	DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;
	B_SPI_DMA_busy = 1;	//��־SPI-DMAæ��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0
}

void	SPI_DMA_TxTRIG(u32 addr, u8 *buffer, u16 size)	//
{
	u16	i;
	if(size == 0)	return;
	if(!B_FlashOK)	return;

	FlashWriteEnable();					//ʹ��Flashд����
	SPI_CE_Low();						// enable device
	SPI_WriteByte(SFC_PAGEPROG);		// ����ҳ�������
	SPI_WriteByte(((u8 *)&addr)[1]);	//������ʼ��ַ
	SPI_WriteByte(((u8 *)&addr)[2]);
	SPI_WriteByte(((u8 *)&addr)[3]);

	HSSPI_CFG  = SS_HOLD | SS_SETUP;	//SS_HOLD������N��ϵͳʱ��, SS_SETUPû������ʱ�ӡ�
	HSSPI_CFG2 = SPI_IOSW | HSSPIEN | FIFOEN | SS_DACT;	//FIFOEN����FIFO���С13��ʱ��. @40MHz FIFOEN=1, SS_HOLD=0ʱ523us @2T, 943us @4T;    FIFOEN=1, SS_HOLD=3ʱ600us @2T, 1020us @4T.

	i = (u16)buffer;	//ȡ�׵�ַ
	DMA_SPI_TXAH = (u8)(i >> 8);		//���յ�ַ�Ĵ������ֽ�
	DMA_SPI_TXAL = (u8)i;				//���յ�ַ�Ĵ������ֽ�
	DMA_SPI_AMTH = (u8)((size-1)/256);	//���ô������ֽ��� = n+1
	DMA_SPI_AMT  = (u8)((size-1)%256);	//���ô������ֽ��� = n+1
	DMA_SPI_ITVH = 0;					//���ӵļ��ʱ�䣬N+1��ϵͳʱ��
	DMA_SPI_ITVL = 0;
	DMA_SPI_STA  = 0x00;
	DMA_SPI_CFG  = DMA_SPIIE | SPI_ACT_TX | DMA_SPIIP | DMA_SPIPTY;
	DMA_SPI_CFG2 = SPI_WRPSS | SPI_SSS;
	DMA_SPI_CR   = DMA_ENSPI | SPI_TRIG_M | SPI_TRIG_S | SPI_CLRFIFO;
	B_SPI_DMA_busy = 1;	//��־SPI-DMAæ��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0
}

//=====================================40.96/4====================================
// ����: void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
// ����:  SPI_DMA�жϺ���.
// ����: none.
// ����: none.
// �汾: V1.0, 2024-1-5
//========================================================================
void SPI_DMA_ISR (void) interrupt DMA_SPI_VECTOR
{
	DMA_SPI_STA = 0;		//����жϱ�־
	DMA_SPI_CR  = 0;		//��ֹDMA����
	SPSTAT = 0x80 + 0x40;	//��0 SPIF��WCOL��־
	B_SPI_DMA_busy = 0;		//���SPI-DMAæ��־��SPI DMA�ж�������˱�־��ʹ��SPI DMAǰҪȷ�ϴ˱�־Ϊ0
	HSSPI_CFG2 = SPI_IOSW | SS_DACT;	//ʹ��SPI��ѯ���жϷ�ʽʱ��Ҫ��ֹFIFO
	SPI_CE_High();						// disable device
}
