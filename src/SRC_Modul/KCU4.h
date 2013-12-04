#ifndef __KCU__
#define __KCU__

#ifdef 	PLATA_KCU4

#define DEVICE_TYPE 3
#define NAME_MODUL "KCU4"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//Указатель на функции инициализации блока
extern void (*DRIVER_BLOK)(void);	//Указатель на функции драйвера блока
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// Указатель на функции обработки входящих пакетов по UART

extern WORD (*STATE_BLOCK);			//Уазатель на поле Info блока

//=============================================================
// CC //включение тока обтекания	0-выкл 1-вкл
// FC //при CP = 0 -> наличие тока обтекания		0 - тока нет обрыв, 1 - есть ток / при CP = 1 -> 0 - наличие 27В, 1 - отсутст 27В
// FO //защита выхода по току		0 - ток В НОРМЕ, 1 - сработала защита

#define	CP1		PDR09_P0
#define	CP2		PDR09_P2
#define	CP3		PDR09_P6


#define	CC1		PDR09_P1
#define	CC2		PDR09_P5
#define	CC3		PDR09_P7


#define	FO_1	EPSR14_PS6
#define	FO_2	EPSR14_PS4
#define	FO_3	EPSR14_PS2
#define	FO_4	EPSR14_PS0

#define	FO_5	EPSR07_PS6
#define	FO_6	EPSR07_PS4
#define	FO_7	EPSR07_PS2
#define	FO_8	EPSR07_PS0

#define	FO_9	EPSR06_PS6
#define	FO_10	EPSR06_PS4
#define	FO_11	EPSR06_PS2
#define	FO_12	EPSR06_PS0

#define	FO_13	EPSR03_PS6
#define	FO_14	EPSR03_PS4
#define	FO_15	EPSR03_PS2
#define	FO_16	EPSR03_PS0

#define	FO_17	EPSR01_PS6
#define	FO_18	EPSR01_PS4
#define	FO_19	EPSR01_PS2
#define	FO_20	EPSR01_PS0

#define	FO_21	EPSR00_PS6
#define	FO_22	EPSR00_PS4
#define	FO_23	EPSR00_PS2
#define	FO_24	EPSR00_PS0
//--------------------------
#define	FC_1	EPSR14_PS7
#define	FC_2	EPSR14_PS5
#define	FC_3	EPSR14_PS3
#define	FC_4	EPSR14_PS1

#define	FC_5	EPSR07_PS7
#define	FC_6	EPSR07_PS5
#define	FC_7	EPSR07_PS3
#define	FC_8	EPSR07_PS1

#define	FC_9	EPSR06_PS7
#define	FC_10	EPSR06_PS5
#define	FC_11	EPSR06_PS3
#define	FC_12	EPSR06_PS1

#define	FC_13	EPSR03_PS7
#define	FC_14	EPSR03_PS5
#define	FC_15	EPSR03_PS3
#define	FC_16	EPSR03_PS1

#define	FC_17	EPSR01_PS7
#define	FC_18	EPSR01_PS5
#define	FC_19	EPSR01_PS3
#define	FC_20	EPSR01_PS1

#define	FC_21	EPSR00_PS7
#define	FC_22	EPSR00_PS5
#define	FC_23	EPSR00_PS3
#define	FC_24	EPSR00_PS1

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
	BYTE	Regim;
	BYTE	FC[3];		// опрашиваем только при СС=1 -- даёт онформацию об обрыве
	BYTE	FO[3];		// не зависит от СС имеет значение только при замкнутом ФСУ
	BYTE	St27V[3];	// наличие 27В 1 - 27В есть 0 - отсутствует
	BYTE	TmpData;
	BYTE	TmpData1;

	 
	BYTE	SendPak;
}CKCU;

void DriverKCU();
void InitKCU();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);


extern CKCU Kcu;

#endif
#endif