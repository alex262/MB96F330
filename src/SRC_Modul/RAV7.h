#ifndef __RAV7__
#define __RAV7__

#ifdef 	PLATA_RAV7

#include "can.h"

#define DEVICE_TYPE 2

#define NAME_MODUL "RAV7"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//”казатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//”казатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ”казатель на функции обработки вход€щих пакетов по UART

extern WORD (*STATE_BLOCK);			//”азатель на поле Info блока
//=============================================================

#define DK1	PDR07_P7
#define DK2	PDR07_P6
#define PG24V	PDR05_P2

#define SW1	PDR01_P0
#define SW2	PDR01_P2
#define SW3	PDR01_P4
#define SW4	PDR01_P6

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
	
	BYTE	setSW;
	BYTE	newSW;
	BYTE	sDK1;
	BYTE	sDK2;
	BYTE	sPG24V;
	WORD	AIN[2];
	float	fAIN[2];
//-------------------------------------------------------- 
	BYTE	SendPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	
}CRAV7;

void DriverRAV7();
void InitRAV7();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);

extern CRAV7 RAV7;

#endif
#endif