#ifndef __GLOBAL__
#define __GLOBAL__
#include "stdlib.h"
#include "mb96338us.h"
//==============================================================================
//����������� ������ TX PDO ��� 
//��������� �������� ��� ������� �������
#define	CASH_PDO_DATA
//--------------------------------
//�������� ������ �� ��������� CanOpen
#define CAN_OPEN_ENABLE
//---------------------------------------
//#define TERMINAL_EN	// ���������� ������ ��������� �� COM1
//---------------------------------------
#define ADDR		((EPSR12>>2)&0x1F)
#define ADDR_DIR_IN	DDR12 = DDR12&0x83;
#define ADDR_EN		PIER12 = PIER12|0x7C;
//------------------------------------------------------------------------------
//#define EN_SR_ZN_CYKL // ��������� ���������� ������� �������� ����� � ���
//==============================================================================
#define ADDR_RAM_WD			0x018FF0
#define ADDR_EEPROM_ST_PWR	0xFFF0
//==============================================================================
//�������� ���� ���  ���������� ����������
//#define	PLATA_KCU4
//#define	PLATA_ADC8
//#define 	PLATA_FSU_29_30
//#define	PLATA_NDD22
#define	PLATA_DAC11
//==============================================================================
#define BUFFER_LEN_UART		300				//������ ������� �����
#define MAX_IN_PAK 			290				//���c�������  ����� ��������� ������
#define COUNT_UART_BUF		1				//��������� ������

#define MAX_STACK_MSG_RX	50
#define MAX_STACK_MSG_TX	50
#define NB_LINE_CAN			3

#define MAX_COUNT_1WARE_DEV  5
//==============================================================================
// �  ������������ � ��������� 
// ����������� header
#ifdef	PLATA_KCU4
	#define	INCLUDE_H	"KCU4.h"
	#define	NEED_WORK_CAN	3	//����������� ���������� ���������� CAN - ��� ����������� ���������
#endif

#ifdef	PLATA_ADC8
	#define	INCLUDE_H	"ADC8.h"
	#define POWER_BLOCK_ENABLE
	#define	NEED_WORK_CAN	2	//����������� ���������� ���������� CAN - ��� ����������� ���������
#endif

#ifdef 	PLATA_FSU_29_30
	#define	INCLUDE_H	"FSU29_30.h"
	#define	NEED_WORK_CAN	2	//����������� ���������� ���������� CAN - ��� ����������� ���������
#endif
#ifdef 	PLATA_NDD22
	#define	INCLUDE_H	"NDD22.h"
	#define POWER_BLOCK_ENABLE
	#define	NEED_WORK_CAN	2	//����������� ���������� ���������� CAN - ��� ����������� ���������
#endif
#ifdef 	PLATA_DAC11
	#define	INCLUDE_H		"DAC11.h"
	#define	NEED_WORK_CAN	2	//����������� ���������� ���������� CAN - ��� ����������� ���������
#endif
//------------------------------------------------------------------------------
#ifndef	INCLUDE_H
	#error "Not Include File "
#endif
//------------------------------------------------------------------------------
#ifndef SPEED_CAN_0
	#define	SPEED_CAN_0	CAN_1000
#endif
#ifndef SPEED_CAN_1
	#define	SPEED_CAN_1	CAN_1000
#endif
#ifndef SPEED_CAN_2
	#define	SPEED_CAN_2	CAN_1000
#endif
//------------------------------
#define PIN_DIR_IN	0
#define PIN_DIR_OUT	1
//------------------------------
// ���������� CAN 
#define CNC1		PDR05_P4
#define CNC1_DIR	DDR05_D4

#define CNC2		PDR05_P2
#define CNC2_DIR	DDR05_D2

#define CNC3		PDR05_P3
#define CNC3_DIR	DDR05_D3

#define CNC_CAN_EN	0
#define CNC_CAN_DIS	1
//==============================================================================
#ifndef LEDR
	#define LEDR		PDR15_P2
	#define LEDR_DIR	DDR15_D2
	#define LEDR_ON		LEDR = 0;
	#define LEDR_OFF	LEDR = 1;
#endif

#ifndef LEDG
	#define LEDG		PDR15_P3
	#define LEDG_DIR	DDR15_D3
	#define LEDG_ON		LEDG = 0;
	#define LEDG_OFF	LEDG = 1;
#endif
//------------------------------------------------------------------------------
typedef unsigned char	BOOL;
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

typedef signed char		int8_t;
typedef signed short	int16_t;
typedef signed long		int32_t;

typedef signed char		S8 ;  //!< 8-bit signed integer.
typedef unsigned char	U8 ;  //!< 8-bit unsigned integer.
typedef	signed short	S16;  //!< 16-bit signed integer.
typedef unsigned short	U16;  //!< 16-bit unsigned integer.
typedef unsigned long	S32;  //!< 32-bit signed integer.
typedef signed long		U32;  //!< 32-bit unsigned integer.

typedef float			F32;  //!< 32-bit floating-point number.
typedef double			F64;  //!< 64-bit floating-point number.

//! Union of pointers to 64-, 32-, 16- and 8-bit unsigned integers.
typedef union
{
	S32 *s32ptr;
	U32 *u32ptr;
	S16 *s16ptr;
	U16 *u16ptr;
	S8  *s8ptr ;
	U8  *u8ptr ;
} UnionPtr;

/* Unsigned types */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

/** single precision floating point number (4 byte) */
typedef float        float32_t;

/** double precision floating point number (8 byte) */
typedef double       float64_t;

/** ASCCI character for string generation (8 bit) */
typedef char         char_t;

//==============================================================================
#define TRUE	1
#define FALSE	0
#define true	1
#define false	0
//==============================================================================
//	�������� 
#define	ind_WORK		0		//!���� �������� ���������, �������������� ���
#define	ind_COM1		2		//��������� ���������  ������ �� ������ ������ �����
#define	ind_ERROR		3		//������,  ���������  ��  ����������� ���� ������, ���� ��  �  ���������������� �����   ������������ ���. ���������� 
#define	ind_COMA		4		//! ��������  ��������  �� � �� �������  � ��
#define	ind_TEST		5		//���� ������� �� ������: �� ���������, ������� �� �����
#define	ind_FATERR		6		//����������� ��� �������  ������, �� ���� ��  �  ����������� ������ �����.
#define	ind_RESET		7		//���� � ������ ���������������  

#define	Err_CAN1		1		//
#define	Err_CAN2		2		//
#define	Err_CAN3		4		//
//==============================================================================

extern WORD TIME_OUT_CAN;

#define TYPE_DATA_TIMER		WORD
//==============================================================================
//STATE BLOCK
#define	ST_Addr	0x001F	
#define	ST_Mode	0x01E0
#define	ST_Can1	0x0200		// can3
//#define	ST_F2	0x0400
#define	ST_Can2	0x0800		// can1
#define	ST_Can3	0x1000		// can2 
#define	ST_Pwr1	0x2000
#define	ST_Pwr2	0x4000
#define	ST_Res	0x8000
//==============================================================================
#define SETBIT(ADDRESS,BIT) 	((ADDRESS) |= (1<<(BIT)))	//Set BIT in ADDRESS 
#define CLEARBIT(ADDRESS,BIT)  	((ADDRESS) &= ~(1<<(BIT)))	//Clear BIT in ADDRESS
#define digit(DATA,BIT)			((DATA&(1<<BIT)) ? 1 : 0)	//��������� �������� BIT -�� ������� � ����� DATA
#define MAX(a,b)				(a>b ? a : b)

extern BYTE	WATCH_DOG_CODE;		
#define clrwdt 			WDTCP = WATCH_DOG_CODE; WATCH_DOG_CODE = ~WATCH_DOG_CODE; // ������� WatchDog

#define RESET 			RCR_SRSTG = 1		//������������ ����������������

#define RST_UNKNOWN		0x0			// ����� ��� ������� ������������
#define RST_POWERON		0x1			//������� �������
#define RST_STANDBY		0x2			//���������� ���� �����
#define RST_WATCHDOG	0x3			//�������� ��������  ��������
#define RST_RSTPIN		0x4			//�� �������� � �������
#define RST_RSTBIT		0x5			//RST bit

#define	ON1_PWR		PDR14_P7		// 0 - ��� ������ �������, 1 - ����
#define	ON2_PWR		PDR00_P4
#define	ON1_PWR_OFF	ON1_PWR=1;
#define	ON2_PWR_OFF	ON2_PWR=1;
#define	ON1_PWR_ON	ON1_PWR=0;
#define	ON2_PWR_ON	ON2_PWR=0;

#define	FLT1_PWR	EPSR15_PS0		// ������������� ����� �������	1-��������
#define	FLT2_PWR	EPSR00_PS0

#define	ERR1_PWR	EPSR07_PS3		// 1 - ��� ������
#define	ERR2_PWR	EPSR00_PS1		//����� �� ������� ������������ ��������� ���������� (3.3�)

typedef struct
{
	WORD 	ms;				//���� �������   ������
	BYTE	sec;
	BYTE	min;
	BYTE	hour;
	DWORD	day;
	
	BYTE 	Device_ReStart;			//������� �������� �� ����������������
	BYTE 	watch_dog;
	BYTE	REGIM;
	WORD	Error;
	TYPE_DATA_TIMER	TimerCan0;
	TYPE_DATA_TIMER	TimerCan1;
	TYPE_DATA_TIMER	TimerCan2;
	
	WORD	COUNT;
	
	BYTE	Cnt1WareDev;							// ���������� �������� ���������
	BYTE	SN_1Ware_Dev[MAX_COUNT_1WARE_DEV][8];	// �������� ������ ��������� �� ���� 1Ware 
	
	WORD	WatchDogRam;	
	//-------------------------------------
	// EEPROM
	WORD	Addr;
	WORD	Len;
	BYTE	StEEPROM;	// 0, 1 ������ �������
	BYTE	RW_EEPROM;	// 0 - ������ , 1 ������
	BYTE	BufEEPROM[300];
	BYTE	StEEPROM_Write;	// 0, 1 ��������� ������ � eeprom 
	
	
#ifdef EN_SR_ZN_CYKL
	WORD	CountCikl_1s;
	WORD	SrCikl_mks;
#endif

#ifdef POWER_BLOCK_ENABLE
	TYPE_DATA_TIMER	TimerPwr;
	BYTE	stERR1;	//0 � ������ ���������� ������� ��� // 1-16 ����� 
					//1 � ��� � �������
	BYTE	stERR2; // 17-32 �����
	BYTE	stFLT1;
	BYTE	stFLT2;
	BYTE	setON1;
	BYTE	setON2;
	BYTE	setPPG1ch_en;
	BYTE	setPPG2ch_en;
	DWORD	EE_StPwr;
	DWORD	StPwr; //3 - ���� ������� �� �������� ���� ������������� 
				   //2 - ���� ������� ���������
				   //1 - ���� ������� ���������
#endif

}TProgram;

typedef struct 
{
	U16 st ; /* received */
	U16 end; /* To transmit */
	U16	count;
} t_pointerStack;

//--------------------------------------------------------
void InitWatchDog(void);
void SetError(BYTE Error);
void ClearError(BYTE Error);
BYTE GetError();
void InitProgrammVar(void);
void Indicate(WORD *pCount, WORD BLINK);
void msDelay(WORD ms);
BYTE check_NaN_Inf( float x );
void CyclicModules(void);

//-----------------------------------------------
//---------------------------------------------------
extern char_t B_Out[255];

extern TProgram program;

#endif
