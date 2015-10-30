#ifndef __OK6__
#define __OK6__

#ifdef 	PLATA_OK6

#include "can.h"

#define DEVICE_TYPE 2

#define NAME_MODUL "OK6"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//”казатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//”казатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ”казатель на функции обработки вход€щих пакетов по UART

extern WORD (*STATE_BLOCK);			//”азатель на поле Info блока
//=============================================================

#define DK		PDR01_P0
#define DK1A	PDR01_P5
#define DK2A	PDR01_P6
#define DK3A	PDR01_P7

#define PG24V	PDR05_P2

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
	
	BYTE	sDK;
	BYTE	sDK1A;
	BYTE	sDK2A;
	BYTE	sDK3A;
	BYTE	sPG24V;
	WORD	AIN[4];
	float	fAIN[4];
//-------------------------------------------------------- 
	BYTE	SendPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	
}COK6;

void DriverOK6();
void InitOK6();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);

extern COK6 OK6;

#endif
#endif