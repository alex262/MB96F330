#ifndef __DAC11__
#define __DAC11__

#ifdef 	PLATA_DAC11

#include "can.h"


#define DEVICE_TYPE 8
#define NAME_MODUL "DAC11"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//��������� �� ������� ������������� �����
extern void (*DRIVER_BLOK)(void);	//��������� �� ������� �������� �����
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ��������� �� ������� ��������� �������� ������� �� UART

BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData(BYTE bus_id, Message *m);

extern WORD (*STATE_BLOCK);			//�������� �� ���� Info �����
//=============================================================
#define MAX_COD_DAC		0xFFF	//������������ �������� ������� ����� �������� � ���
#define MIN_COD_DAC		0		//����������� �������� ������� ����� �������� � ���    

#define COUNT_DAC_CH	12
//=============================================================
#define READ_MODE_SPI	1 
#define WRITE_MODE_SPI	0 
//=============================================================
#define NUM_CAN_FOR_SELECT_MASTER	0 	// ����� can �� �������� ����� ����� ���������� �������
//=============================================================
#define ERROR_OK	0	// ������ �� ������ ���
#define ERROR_ZERO	1	// �� ����������� �������� ���� �����
#define ERROR_KZ	2	// �� ����������� �������� ���� ��������� ���������� ��������
#define ERROR_MAX	3	// �� ����� ���������� 
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
	float	fWriteDAC;// �������� ������������ � ���
}TPak3;

//==============================================================
//������ ��������� ������ ��� SPI ��� ����
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
	//--------------------------------------------------------
	float	fDAC_Set[COUNT_DAC_CH];																			//48
	float	fADC[COUNT_DAC_CH];																				//48	
	WORD	DiagRele;			// ����������� ��������� ����	������ ����� ���������� �����				//2
	WORD	EnOutDac;			// ������������ ������ 12 ���, 1 - ����� ���� ���������� �������������		//2
	WORD	DiagMAX;			// ����������� ������� ��������� MAX										//2
	WORD	Master[3];			// ������� ����������� ������ �� �������� ��������� ������ ���				//2
	BYTE	TarrStatus;			// ����������� ���� ��� ���, ���� �� ����������� �� ����� ��������� �������	//1
	BYTE	SerN[3][8];			// �������� ������ ������ ������, �� �������� �������� ����					//24
	BYTE	res1;																							//1
	BYTE	PingBlock[3];		// ���� �� �������															//3
	BYTE	PingDataBlock[3];	// ������� ������ �� �������												//3
	BYTE	StNeigbor[3];		// true - ����� �����, false-������ �� �����								//3
	BYTE	AddrBl[3];			// ����������� ������ ������, � 0 ������� ��� �����							//3
	WORD	ErrorUP;			// ������ �� ���������� �������� 											//2
	WORD	ErrorDOWN;			// ������ �� ������� ����������� ���� 										//2
	DWORD	ErrorDAC[3];		// ������������� ������ ���(��� ���� �� �����), � 0 ������� ���� ������		//12
	WORD	OutDac[3];			// ������������ ������ 12 ���, 1 - ����� ���� �������������					//6
	BYTE	StNeigborData[3];	// true - ������� ������ �� �������
	//--------------------------------------------------------
	WORD	NewOutDac;			// ���� �������� ����� �������� ��� ���������� ����
	
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
	BYTE	HiLo;			// ������ �������� ��� ������ ���
	BYTE	HiLoDec;		// HiLo ��������� 
	BYTE	StatusMaster;	//������ ������
	BYTE	ErrorSet;		// 1- ���� ������ ��� ��������� ���� ������ �� �������� ����� ��������
	
	TYPE_DATA_TIMER	TimerMasterError;		//������ ��� ������ �������
	TYPE_DATA_TIMER	TimerPingNeighbor;		// ������ ������ ����� �������		
	TYPE_DATA_TIMER	TimerDout;		
	TYPE_DATA_TIMER	TimerDin;
	TYPE_DATA_TIMER	TimerAin;
	TYPE_DATA_TIMER	TimerTemp;
	//--------------------------------------------------------
	BYTE	WriteTar;		// 1 - ��� ��������� ��������� ������������� � EEPROM

}CDAC11;

void DriverDAC11();
void InitDAC11();
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);

extern CDAC11 Dac11;
extern TTar TarRam[2*COUNT_DAC_CH];	// ��������� ������� � ���



#endif
#endif