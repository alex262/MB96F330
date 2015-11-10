#ifndef __OC9__
#define __OC9__

#ifdef 	PLATA_OC9

#include "can.h"

//=============================================================
//#define BLOCK_OC9
#define BLOCK_OC10
//=============================================================
#define DEVICE_TYPE 2
//=============================================================
#ifdef BLOCK_OC10
	#define NAME_MODUL "OC10"
#endif
#ifdef BLOCK_OC9
	#define NAME_MODUL "OC9"
#endif
extern const char SoftwareVer[20]; 
//=============================================================
extern void (*INIT_BLOK)(void);		//��������� �� ������� ������������� �����
extern void (*DRIVER_BLOK)(void);	//��������� �� ������� �������� �����
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ��������� �� ������� ��������� �������� ������� �� UART
extern WORD (*STATE_BLOCK);			//�������� �� ���� Info �����
//=============================================================
#define COUNT_UART_MK	5 // UART �� ���� ��
#ifdef BLOCK_OC9
	#define COUNT_UART_EXT	4 // ������� UART
#endif
#ifdef BLOCK_OC10
	#define COUNT_UART_EXT	0 // ������� UART
#endif

#define MASTER_CH_UART	0 // ������ �����, ����� ��� ����� � ������������ ������

#define DIV_BAUD_EXT_UART 4

#define SIZE_FIFO_BUFFER_UART_RX_OC9	512// ���� ������ ������ ������� ������������ � ����������
#define SIZE_FIFO_BUFFER_UART_TX_OC9	512//1024 // ���� ������ ������ ������� ������������ � ����������

#define SIZE_BUFFER_UART_RX_OC9	512 // ������ ������ ������� ������������ ��� ������ ������
#define MAX_IN_PAK_OC9 (SIZE_BUFFER_UART_RX_OC9-12)
//#define SIZE_BUFFER_UART_TX	512 // ����� ������ ������� ������������ ��� ������ ������


#define START_ADDR_MEM_CS0		0xF0
#define START_ADDR_MEM_CS1		0xC00
#define START_ADDR_MEM_CS2		0x100000
#define START_ADDR_MEM_CS3		0x400000

#define CS0		PDR09_P7
#define CS1		PDR09_P6
#define CS2		PDR09_P5
#define CS3		PDR09_P4

#define DE		PDR06_P3
#define RE		PDR06_P4


#define RSTU	PDR09_P4

#define RXRDY	PDR13_P4
#define TXRDY	PDR13_P5

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
	
	BYTE	K[3];		// 1 - ���� ���
	BYTE	OL[3];		// 1 - ����� � ������� ����
	BYTE	Diag[3];	// 1 - ��������� ���� ���������� ��� ���������
	BYTE	DiagSum[3];	// 1 - ���� �������������
	
	BYTE	SetData[3];
	BYTE	NewData[3];
//-------------------------------------------------------- 
	BYTE	SendPak;
	BYTE	stRecvPak;
	//-------------------------------------------------------- 
	
}COC9;

void DriverOC9();
void InitOC9();

BYTE IRQ_TX_OC9(BYTE ch);
void IRQ_RX_OC9(BYTE ch, BYTE data);



void ServiceUart(BYTE Id, BYTE* pData, WORD Len);
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);

extern COC9 OC9;

//***********************************************************************
#define DMA_COUNT_BUFF_TX	5	// ���������� ������� ��������
#define DMA_COUNT_BUFF_RX	5	// ���������� ������� ������ 

#define DMA_SIZE_BUFF_TX	(1024)	// ������ ������ �������� �� �����
#define DMA_SIZE_BUFF_RX	(1024)	// ������ ������ ������ �� �����

typedef struct
{
	DWORD	AddrBuff;	// ����� ������������� ������ 
	WORD	SizeBuff;	// ������ �������
	WORD	Start;		// ������ ������ � ������
	WORD	Stop;		// �� ������� �������� ������ � �����
	WORD	SizeDMA;	// ������ ������ ���������� DMA �� ����� � ������� Start
	BYTE	Run;		// DMA �������
}TDmaTxItems;
//--------------------------------------------------------------------
typedef struct
{
	DWORD	AddrBuff;	// ����� ������������� ������ 
	WORD	SizeBuff;	// ������ �������
	WORD	Start;		// ������ ������ � ������ ������
	BYTE	Over;		// ��������� �������������� ������ �� ������		
}TDmaRxItems;
//***********************************************************************


#endif
#endif