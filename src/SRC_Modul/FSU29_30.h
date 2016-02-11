#ifndef __FSU2930__
#define __FSU2930__

#ifdef 	PLATA_FSU_29_30

#include "can.h"

#define DEVICE_TYPE 2

//#define NAME_MODUL "FSU29"
#define NAME_MODUL "FSU30"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//”казатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//”казатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ”казатель на функции обработки вход€щих пакетов по UART

extern WORD (*STATE_BLOCK);			//”азатель на поле Info блока
//=============================================================
#define CS_ON	0
#define CS_OFF	1

//=============================================================

#define OUT1	PDR17_P3
#define OUT2	PDR17_P4
#define OUT3	PDR17_P6
#define OUT4	PDR09_P4
#define OUT5	PDR09_P5
#define OUT6	PDR09_P6

#define CS11A	PDR07_P0
#define CS12A	PDR07_P1
#define CS13A	PDR07_P2

#define CS21A	PDR01_P5
#define CS22A	PDR01_P6
#define CS23A	PDR01_P7



#define	K_1		EPSR14_PS7		// 0 - реле вкл
#define	K_2		EPSR14_PS6
#define	K_3		EPSR14_PS5
#define	K_4		EPSR14_PS4
#define	K_5		EPSR14_PS3
#define	K_6		EPSR14_PS2

#define	K_7		EPSR06_PS7
#define	K_8		EPSR06_PS6
#define	K_9		EPSR06_PS5
#define	K_10	EPSR06_PS4
#define	K_11	EPSR06_PS3
#define	K_12	EPSR06_PS2

#define	K_13	EPSR03_PS7
#define	K_14	EPSR03_PS6
#define	K_15	EPSR03_PS5
#define	K_16	EPSR03_PS4
#define	K_17	EPSR03_PS3
#define	K_18	EPSR03_PS2
#define	K_19	EPSR03_PS1
#define	K_20	EPSR03_PS0

#define	K_21	EPSR02_PS7
#define	K_22	EPSR02_PS6
#define	K_23	EPSR02_PS5
#define	K_24	EPSR02_PS4

//==============================================================
typedef union
{
	WORD	word;
	struct
	{
		WORD Addr	:5;	
		WORD Mode	:4;
		WORD Can3	:1;
		WORD Res1	:1;
		WORD Can1	:1;
		WORD Can2	:1;
		WORD Res	:3;
	}bits;
}TInfo;


typedef struct
{
	TInfo	Info;	
	
	BYTE	K[3];		// 1 - реле вкл
	BYTE	OL[3];		// 1 - обрыв в обмотке реле
	BYTE	Diag[3];	// 1 - прегрузка либо отключение поп перегреву
	BYTE	DiagSum[3];	// 1 - есть неисправность
	
	BYTE	SetData[3];
	BYTE	NewData[3];
//-------------------------------------------------------- 
	BYTE	SendPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	
}CFSU;

void DriverFSU();
void InitFSU();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);

extern CFSU Fsu;

#endif
#endif