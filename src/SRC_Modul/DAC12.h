#ifndef __DAC12__
#define __DAC12__
#ifdef 	PLATA_DAC12

#include "can.h"
//=============================================================
// ����� ��� ���� ������
//=============================================================
#define DEVICE_TYPE 8
#define NAME_MODUL "DAC12"

extern const char SoftwareVer[20]; 

extern void (*INIT_BLOK)(void);		//��������� �� ������� ������������� �����
extern void (*DRIVER_BLOK)(void);	//��������� �� ������� �������� �����
extern void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD);// ��������� �� ������� ��������� �������� ������� �� UART

BYTE ServiceMaster(BYTE bus_id, Message *m);
BYTE ServiceObmenData1(BYTE bus_id, Message *m);
BYTE ServiceObmenData2(BYTE bus_id, Message *m);
void ServiceUart(BYTE Id, BYTE* pData, WORD Len);

extern WORD (*STATE_BLOCK);			//�������� �� ���� Info �����
//=============================================================
//=============================================================
//=============================================================
//=============================================================
#define MAX_COD_DAC		0xFFF	//������������ �������� ������� ����� �������� � ���
#define MIN_COD_DAC		0		//����������� �������� ������� ����� �������� � ���    

#define COUNT_DAC_CH	16
#define NUM_REGIM_DAC	3		// ���- ������� ���, 0 - ������� 0-5��, 1 - ������� 0-20��, 2 - ���������� -10 - 10�

//=============================================================
//=============================================================
#define LADDR		((PDR12>>2)&0x1F)	// �������� ������������� �� �����

#define CNC			PDR13_P0	//���������� ������ CAN
#define CNC_DIR		DDR13_D0

#define EOT			PDR05_P1	//����������� ��������� ������ �� ������� 0- ������
#define OD			PDR05_P3	//��������� ������� ������ 0 - ���� 1 - ���
#define PG			PDR05_P5	//PowerGood 0 - ������� ����� � ����� 1 - �������� � ��������
#define CVI_1_8		PDR06		//����� ������ ��� ������ 0 - Vout 1 - Iout
#define CVI_9_16	PDR00		//����� ������ ��� ������ 0 - Vout 1 - Iout
#define ELD_1_8		PDR07		// ���������� �� ���� ��� ����� 0 - error
#define ELD_9_16	PDR02		
#define ECM_1_8		PDR14		// ���������� ���������  0 - error
#define ECM_9_16	PDR01		
#define SYNCN		PDR10_P0	// ����� ��� 0 - �����
#define CLRN		PDR08_P7	// Reset ���������� ��� 0 - �����
#define LOADN		PDR17_P4=0;PDR17_P4=1;	//������ ������ � ��� 
		
//==============================================================
typedef struct
{
	float	k;
	float	ofs;
}TTar;
//==============================================================
typedef union
{
    BYTE    data[3];
    struct  
    {
        DWORD   tmp :2;
        DWORD   DB  :12;
        DWORD   REG :2;
        DWORD   A   :5;
        DWORD   zero:1;
        DWORD   RW  :1;
        DWORD   AB  :1;    
    }bits;
}TPAK_SPI;
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
//==============================================================
typedef struct
{
	//--------------------------------------------------------
	float	fDAC_New[COUNT_DAC_CH];
	float	fDAC_Set[COUNT_DAC_CH];								//48
	BYTE	SetRegim[COUNT_DAC_CH]; // ����� ������ ����������� ���
	BYTE	Diagn[5]; // ���������� �� �������
	
	BYTE	NewRegim[COUNT_DAC_CH]; // ����� ������ ����������� ���
	TInfo	Info;	
	BYTE	SendPak;
		
	//--------------------------------------------------------
	BYTE	WriteTar;		// 1 - ��� ��������� ��������� ������������� � EEPROM
	

}CDAC12;

void DriverDAC12();
void InitDAC12();

extern CDAC12 Dac12;

#endif
#endif