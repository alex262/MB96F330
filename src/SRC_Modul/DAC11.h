#ifndef __DAC11__
#define __DAC11__

#ifdef 	PLATA_DAC11

#include "can.h"


#define DEVICE_TYPE 8
#define NAME_MODUL "DAC11"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//Указатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//Указатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// Указатель на функции обработки входящих пакетов по UART

BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData(BYTE bus_id, Message *m);

extern WORD (*STATE_BLOCK);			//Уазатель на поле Info блока
//=============================================================
#define MAX_COD_DAC		0xFFF	//Максимальное значение которое можно записать в ЦАП
#define MIN_COD_DAC		0		//Минимальное значение которое можно записать в ЦАП    

#define COUNT_DAC_CH	12
//=============================================================
#define READ_MODE_SPI	1 
#define WRITE_MODE_SPI	0 
//=============================================================
#define NUM_CAN_FOR_SELECT_MASTER	0 	// номер can по которому блоки будут определять мастера
//=============================================================
#define ADDR10	PDR07_P0
#define ADDR11	PDR07_P1
#define ADDR12	PDR07_P2

#define ADDR20	PDR00_P1
#define ADDR21	PDR00_P2
#define ADDR22	PDR00_P3

#define CS11	PDR14_P5
#define CS12	PDR14_P4
#define CS13	PDR14_P3
#define CS14	PDR14_P2
#define CS15	PDR14_P1
#define CS16	PDR14_P0

#define CS21	PDR02_P5
#define CS22	PDR02_P4
#define CS23	PDR02_P3
#define CS24	PDR02_P2
#define CS25	PDR02_P1
#define CS26	PDR02_P0

#define CS_ON	0
#define CS_OFF	1
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
	float	k;
	float	ofs;
}TTar;
typedef struct
{
	BYTE	SelectDAC;	
	BYTE	DOUT;
	BYTE	Control1;
	BYTE	Control2;
	float	fWriteDAC;// Значение записываемое в ЦАП
}TPak3;

//==============================================================
//формат выходного пакета для SPI для ЦАПа
typedef union
{
    BYTE    data;
    struct  
    {
        BYTE   adr		:5;
        BYTE   RW		:1;
        BYTE   start1	:1;
        BYTE   start0	:1;
    }bits;
}TPAK_SPI_RM;
typedef union
{
    BYTE    data;
    struct  
    {
        BYTE   Bip		:1;
        BYTE   Gain		:2;
        BYTE   Mux		:4;
        BYTE   start	:1;
    }bits;
}TPAK_SPI_ADC;
typedef union
{
    BYTE    data[2];
    struct  
    {
        WORD   dac		:12;
        WORD   ch		:1;
        WORD   rw		:1;
        WORD   d1		:1;
        WORD   start	:1;
    }bits;
}TPAK_SPI_DAC;
//==============================================================

typedef struct
{
	//--------------------------------------------------------
	float	fDAC_New[COUNT_DAC_CH];
	float	fDAC_Set[COUNT_DAC_CH];
	float	fADC[COUNT_DAC_CH];
	WORD	DiagRele;		// диагностика состояния реле
	WORD	EnOutDac;		// используются первые 12 бит, 1 - выход цапа коммутируется
	WORD	DiagDAC;		// диагностика наличия микросхем MAX
	
	//--------------------------------------------------------
	TInfo	Info;	
	BYTE	SendPak;
	BYTE	SendPakTar;
	BYTE	RecvPak[20];
	BYTE	lenRecvPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	BYTE	stWR_Reg;
	BYTE	NumReg;
	BYTE	ValReg;
	//--------------------------------------------------------
	WORD	wOldError;		// старые ошибки
	WORD	wError;			// неисправность выхода цапа 1 - выход неисправен
	BYTE	HiLo;			// выдача меанднра для выдачи ЦАП
	BYTE	HiLoDec;		// HiLo изменился 
	BYTE	Master;			//Является ли данный ЦАП мастером
	BYTE	StatusMaster;	//Статус выбора
	BYTE	ErrorSet;		// 1- били ошибки при включении реле больше не пытаемся стать мастером
	
	TYPE_DATA_TIMER	TimerMasterError;		//Таймер для выбора мастера
	TYPE_DATA_TIMER	TimerDout;		
	TYPE_DATA_TIMER	TimerDin;
	TYPE_DATA_TIMER	TimerAin;
	TYPE_DATA_TIMER	TimerTemp;
	//--------------------------------------------------------
	BYTE	WriteTar;		// 1 - при изменении тарировки прописываются в EEPROM
	BYTE	TarrStatus;		//Оттарирован блок или нет, если не оттарирован то можно выставить мастера

}CDAC11;

void DriverDAC11();
void InitDAC11();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);

extern CDAC11 Dac11;
extern TTar TarRam[COUNT_DAC_CH];	// тарировки каналов в ОЗУ



#endif
#endif