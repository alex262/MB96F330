#ifndef __NDD22__
#define __NDD22__

#ifdef 	PLATA_NDD22

#define DEVICE_TYPE 1
#define NAME_MODUL "NDD22"

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
	BYTE	test[4];		// достоверность сигналов
	BYTE	Term[4];		// флаг перегрева в ответе, учитываем его в достоверности сигналов
	BYTE	ADC[4][22];		// опрос входов использу€ ј÷ѕ
	BYTE	CurrentAmuxSet; //
	BYTE	CurrentSelect;  // выбор изм тока 0 - high-impedance; 1 - 2mA; 2 - 16mA
	BYTE	CurrentPart;  	// текуща€ половина 0 - перва€ 1 - втора€
	BYTE	Init[4];  		// TRUE необходимо заново инициализировать микросхемы
	
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


extern CNDD Ndd;

#endif
#endif