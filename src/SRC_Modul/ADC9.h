#ifndef __ADC9__
#define __ADC9__

#ifdef 	PLATA_ADC9
#include "can.h"

#define DEVICE_TYPE 4
#define NAME_MODUL "ADC9"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//Указатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//Указатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// Указатель на функции обработки входящих пакетов по UART

extern WORD (*STATE_BLOCK);			//Уазатель на поле Info блока

BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData(BYTE bus_id, Message *m);

//=============================================================
#define ADC_CHIP_COUNT	4
#define ADC_CH_ON_CHIP	8
#define ADC_CH			32

#define ADC_RDATA	0x12
#define ADC_RDATAC	0x10
#define ADC_SDATAC	0x11
#define ADC_START	0x08
#define ADC_RREG	0x20
#define ADC_WREG	0x40

#define MUX_INPUT			0
#define MUX_INPUT_SHORTED	1
#define MUX_MVDD			3
#define MUX_TEMPERATURE		4
#define MUX_TEST			5

//=============================================================

#define	RES1A	PDR09_P5
#define	RES2A	PDR09_P6

#define	SRTA	PDR09_P7
#define CLKA	PDR09_P4

#define CS11	PDR14_P0
#define CS12	PDR14_P1
#define CS21	PDR01_P5
#define CS22	PDR01_P6

#define CS_ON	0
#define CS_OFF	1

#define	DRDY11	EPSR07_PS1
#define	DRDY12	EPSR07_PS2
#define	DRDY21	EPSR00_PS2
#define	DRDY22	EPSR00_PS3


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
	TInfo	Info;	
	
	BYTE	stFLT1;
	BYTE	stFLT2;
	BYTE	setON1;
	BYTE	setON2;
	BYTE	setPPG1ch_en;
	BYTE	setPPG2ch_en;

	float	f_adc_data[ADC_CH];	// -> can
	DWORD	stADC[ADC_CHIP_COUNT];
	
	BYTE	reg_adc[ADC_CHIP_COUNT][16];	// регистры АЦП ->can
	//--------------------------------------------------------
	BYTE	SendPak;
	BYTE	RecvPak[20];
	BYTE	lenRecvPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	BYTE	stWR_Reg;
	BYTE	NumReg;
	BYTE	ValReg;
	//-------------------------------------------------------- 
	//BYTE	UpdTar
	//--------------------------------------------------------
	BYTE	adc_mux_set[ADC_CHIP_COUNT][ADC_CH_ON_CHIP];
	BYTE	adc_mux_new[ADC_CHIP_COUNT][ADC_CH_ON_CHIP];	// -> can
	//--------------------------------------------------------
	BYTE	reg_adc_set[ADC_CHIP_COUNT][13];	
	BYTE	reg_adc_new[ADC_CHIP_COUNT][13];	// -> can
	BYTE	WriteTar;	// 1 - при изменении тарировки прописываются в EEPROM
}CADC9;

void DriverADC9();
void InitADC9();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);


extern CADC9 Adc9;
extern TTar TarRam[ADC_CH];	// тарировки каналов в ОЗУ

#endif
#endif