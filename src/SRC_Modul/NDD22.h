#ifndef __NDD22__
#define __NDD22__

#ifdef 	PLATA_NDD22

#include "can.h"

#define PLATA_NDD22_ 
//#define PLATA_NDD23_ 

#define DEVICE_TYPE 1

#ifdef PLATA_NDD22_ 
	#define NAME_MODUL "NDD22"
#endif
#ifdef PLATA_NDD23_ 
	#define NAME_MODUL "NDD23"
#endif

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//”казатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//”казатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ”казатель на функции обработки вход€щих пакетов по UART

extern WORD (*STATE_BLOCK);			//”азатель на поле Info блока

//=============================================================
//=============================================================

#define	SRTA	PDR09_P7
#define CLKA	PDR09_P4

#define CS11	PDR14_P0
#define CS12	PDR14_P1
#define CS13	PDR14_P2

#define CS21	PDR01_P5
#define CS22	PDR01_P6
#define CS23	PDR01_P7

#define CS_ON	0
#define CS_OFF	1

#define VALID_DIN	0
#define NOT_VALID_DIN	0xFF


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
		WORD Flt1	:1;
		WORD Flt2	:1;
		WORD Res	:1;
	}bits;
}TInfo;

typedef struct
{
	TInfo	Info;	
	//------------------------------------------------
	BYTE	State[4][3];	// три байта ответа микросхемы
	BYTE	Din[4];			// состо€ние дискретных входов полученые по схеме "и"
	BYTE	valid[4];		// достоверность сигналов обобщенна€
	BYTE	Term[4];		// флаг перегрева в ответе, учитываем его в достоверности сигналов
	BYTE	ADC[4][22];		// опрос входов использу€ ј÷ѕ
	BYTE	CurrentAmuxSet; //
	BYTE	CurrentSelect;  // выбор изм тока 0 - high-impedance; 1 - 2mA; 2 - 16mA
	BYTE	CurrentPart;  	// текуща€ половина 0 - перва€ 1 - втора€
	BYTE	Init[4];  		// TRUE необходимо заново инициализировать микросхемы
	
	BYTE	Link[4];		// отображает только наличие св€зи с микросхемой
	BYTE	Reboot[2];		// нужно перезапустить питание 3.3 V
	
	//-------------------------------------------------------- 
	BYTE	SendPak;
	//-------------------------------------------------------- 
	BYTE stWrReg;
	BYTE NumReg;
	BYTE Data[2];
	//-------------------------------------------------------- 
	//--------------------------------------------------------
	//--------------------------------------------------------
}CNDD;

void DriverNDD();
void InitNDD();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);


extern CNDD Ndd;

#endif
#endif