#ifndef __GLOBAL__
#define __GLOBAL__
#include "stdlib.h"
#include "mb96338us.h"
//==============================================================================
//Кеширование данных TX PDO для 
//ускорения отправки при приходе запроса
#define	CASH_PDO_DATA
#define MAX_COUNT_OF_PDO_RECEIVE	31
#define MAX_COUNT_OF_PDO_TRANSMIT	31
//--------------------------------
//Включаем работу по протоколу CanOpen
#define CAN_OPEN_ENABLE
//---------------------------------------
// резрешение работы терминала по COM1 
// если резрешон терминал запрещен прием пакетов в блоке
//#define TERMINAL_EN	
//---------------------------------------
#define ADDR		((EPSR12>>2)&0x1F)
#define ADDR_DIR_IN	DDR12 = DDR12&0x83;
#define ADDR_EN		PIER12 = PIER12|0x7C;
//------------------------------------------------------------------------------
//#define EN_SR_ZN_CYKL // разрешает производит среднее значение цикла в мкс
//==============================================================================
#define ADDR_RAM_WD			0x018FF0
#define ADDR_EEPROM_ST_PWR	0xFFF0
//==============================================================================
//Выбираем блок для  конкретной реализации
//#define	PLATA_KCU4
//#define	PLATA_ADC8
//#define 	PLATA_FSU_29_30
#define	PLATA_NDD22			// НДД22 НДД23
//#define	PLATA_DAC11
//#define	PLATA_OK6
//#define	PLATA_OC9	// вкл в Start.asm BUSMODE INTROM_EXTBUS строка 740
//#define	PLATA_RAV7
//#define	PLATA_DAC12
//#define	PLATA_ADC9
//==============================================================================
#define BUFFER_LEN_UART		300				//Размер буффера порта
#define MAX_IN_PAK 			290				//Макcимальна  длина входящего пакета
#define COUNT_UART_BUF		1				//количесто портов

#define MAX_STACK_MSG_RX	50
#define MAX_STACK_MSG_TX	50
#define NB_LINE_CAN			3

#define MAX_COUNT_1WARE_DEV  5
//==============================================================================
// В  соответствии с выбранным 
// подкллючаем header
#ifdef	PLATA_KCU4
	#define	INCLUDE_H	"KCU4.h"
	#define	NEED_WORK_CAN	3	//необходимое количество работающих CAN - для отображения индикации
#endif

#ifdef	PLATA_ADC8
	#define	INCLUDE_H	"ADC8.h"
	#define POWER_BLOCK_ENABLE
	#define	NEED_WORK_CAN	2	//необходимое количество работающих CAN - для отображения индикации
#endif
#ifdef 	PLATA_FSU_29_30
	#define	INCLUDE_H	"FSU29_30.h"
	#define	NEED_WORK_CAN	2	//необходимое количество работающих CAN - для отображения индикации
#endif
#ifdef 	PLATA_NDD22
	#define	INCLUDE_H	"NDD22.h"
	#define POWER_BLOCK_ENABLE
	#define	NEED_WORK_CAN	2	//необходимое количество работающих CAN - для отображения индикации
#endif
#ifdef 	PLATA_DAC11
	#define	INCLUDE_H		"DAC11.h"
	#define	NEED_WORK_CAN	2	//необходимое количество работающих CAN - для отображения индикации
#endif
#ifdef 	PLATA_OK6
	#define	INCLUDE_H		"OK6.h"
	#define	NEED_WORK_CAN	1	//необходимое количество работающих CAN - для отображения индикации
	#define	SPEED_CAN_0	0
	#define	SPEED_CAN_1	0
#endif
#ifdef 	PLATA_OC9
	#define	INCLUDE_H		"OC9.h"
	#define	NEED_WORK_CAN	1	//необходимое количество работающих CAN - для отображения индикации
	#define	SPEED_CAN_0	0
	#define	SPEED_CAN_2	0
#endif
#ifdef 	PLATA_RAV7
	#define	INCLUDE_H		"RAV7.h"
	#define	NEED_WORK_CAN	1	//необходимое количество работающих CAN - для отображения индикации
	#define	SPEED_CAN_0	0
	#define	SPEED_CAN_1	0
#endif
#ifdef 	PLATA_DAC12
	#define	INCLUDE_H		"DAC12.h"
	#define	NEED_WORK_CAN	1	//необходимое количество работающих CAN - для отображения индикации
	#define	SPEED_CAN_0	0
	#define	SPEED_CAN_1	0
#endif
#ifdef	PLATA_ADC9
	#define	INCLUDE_H	"ADC9.h"
	#define POWER_BLOCK_ENABLE
	#define	NEED_WORK_CAN	2	//необходимое количество работающих CAN - для отображения индикации
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
// разрешение CAN 
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

#ifndef LEDT1
	#define LEDT1		PDR15_P2
	#define LEDT1_DIR	DDR15_D2
	#define LEDT1_ON	LEDR = 0;
	#define LEDT1_OFF	LEDR = 1;
#endif

#ifndef LEDT2
	#define LEDT2		PDR15_P3
	#define LEDT2_DIR	DDR15_D3
	#define LEDT2_ON	LEDT2 = 0;
	#define LEDT2_OFF	LEDT2 = 1;
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
//	Индикаци 
#define	ind_WORK		0		//!Блок работает нормально, неисправностей нет
#define	ind_COM1		2		//Нарушение соединени  хотябы по одному каналу связи
#define	ind_ERROR		3		//Ошибка,  пецифична  дл  конкретного типа блоков, допу ка  а  функционирование блока   ограничением тех. параметров 
#define	ind_COMA		4		//! арушение  оединени  по в ем каналам  в зи
#define	ind_TEST		5		//Блок выведен из работы: те тирование, отладка ПО блока
#define	ind_FATERR		6		//Обнаружение неу транимо  ошибки, не допу ка  е  продолжение работы блока.
#define	ind_RESET		7		//Блок в режиме программировани  
#define	ind_BREST		8		//Индикация брест

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
#define digit(DATA,BIT)			((DATA&(1<<BIT)) ? 1 : 0)	//Возвращет значение BIT -го разряда в числе DATA
#define MAX(a,b)				(a>b ? a : b)

extern BYTE	WATCH_DOG_CODE;		
#define clrwdt 			WDTCP = WATCH_DOG_CODE; WATCH_DOG_CODE = ~WATCH_DOG_CODE; // очистка WatchDog

#define RESET 			RCR_SRSTG = 1		//Перезагрузка микроконтроллера

#define RST_UNKNOWN		0x0			// еизве тна причина перезагрузки
#define RST_POWERON		0x1			//Пропало питание
#define RST_STANDBY		0x2			//аппаратное откл чение
#define RST_WATCHDOG	0x3			//превышен временно  интервал
#define RST_RSTPIN		0x4			//от внешнего и точника
#define RST_RSTBIT		0x5			//RST bit

#define	ON1_PWR		PDR14_P7		// 0 - вкл блоков питания, 1 - выкл
#define	ON2_PWR		PDR00_P4
#define	ON1_PWR_OFF	ON1_PWR=1;
#define	ON2_PWR_OFF	ON2_PWR=1;
#define	ON1_PWR_ON	ON1_PWR=0;
#define	ON2_PWR_ON	ON2_PWR=0;

#define	FLT1_PWR	EPSR15_PS0		// неисправность блока питания	1-исправен
#define	FLT2_PWR	EPSR00_PS0

#define	ERR1_PWR	EPSR07_PS3		// 1 - все хорошо
#define	ERR2_PWR	EPSR00_PS1		//выход за пределы стабилизации выходного напряжения (3.3В)

typedef struct
{
	WORD 	ms;				//мили екундны   чётчик
	BYTE	sec;
	BYTE	min;
	BYTE	hour;
	DWORD	day;
	
	BYTE 	Device_ReStart;			//причина перезапу ка микроконтроллера
	BYTE 	watch_dog;
	BYTE	REGIM;
	WORD	Error;
	TYPE_DATA_TIMER	TimerCan0;
	TYPE_DATA_TIMER	TimerCan1;
	TYPE_DATA_TIMER	TimerCan2;
	
	WORD	COUNT;
	
	BYTE	Cnt1WareDev;							// количество найденых устройств
	BYTE	SN_1Ware_Dev[MAX_COUNT_1WARE_DEV][8];	// серийные номера устройств на шине 1Ware 
	
	WORD	WatchDogRam;	
	//-------------------------------------
	// EEPROM
	WORD	Addr;
	WORD	Len;
	BYTE	StEEPROM;	// 0, 1 пришла команда
	BYTE	RW_EEPROM;	// 0 - чтение , 1 запись
	BYTE	BufEEPROM[300];
	BYTE	StEEPROM_Write;	// 0, 1 произошла запись в eeprom 
	
	
#ifdef EN_SR_ZN_CYKL
	WORD	CountCikl_1s;
	WORD	SrCikl_mks;
#endif

#ifdef POWER_BLOCK_ENABLE
	TYPE_DATA_TIMER	TimerPwr;
	BYTE	stERR1;	//0 – провал напряжения питания 
					//1 — все в порядке
	BYTE	stERR2; // 17-32 канал
	BYTE	stFLT1; //неисправность блока питания	1-исправен
	BYTE	stFLT2;
	BYTE	setON1;
	BYTE	setON2;
	BYTE	setPPG1ch_en;
	BYTE	setPPG2ch_en;
	DWORD	EE_StPwr;
	DWORD	StPwr; //3 - блок питания не включаем есть неисправность 
				   //2 - блок питания включился
				   //1 - идет процесс включения
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
