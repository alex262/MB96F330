#ifndef __DMA__
#define __DMA__
#include "mb96338us.h"

#define COUNT_DMA_CH	10		//  оличество каналов DMA

#define DMA_BPD_DEC				1
#define DMA_BPD_INC				0
#define DMA_IF_IOA_INCR			0
#define DMA_IF_IOA_NOT_INCR		1
#define DMA_LEN_BYTE			0
#define DMA_LEN_WORD			1
#define DMA_IOA_TO_BAP			0
#define DMA_BAP_TO_IOA			1
#define DMA_SE_IGNOR			0
#define DMA_SE_STOP				1
#define DMA_BF_BAP_CHANGE		0
#define DMA_BF_BAP_NOT_CHANGE	1

typedef struct 
{
	BYTE BAPL;
	BYTE BAPM;
	BYTE BAPH;
	union{  
		BYTE	byte;
		struct{
			BYTE _SE :1;
			BYTE _DIR :1;
			BYTE _BF :1;
			BYTE _BW :1;
			BYTE _IF :1;
			BYTE _BPD :1;
			BYTE  :1;
			BYTE  :1;
		}bit;
	}DMACS;
	BYTE IOAL;
	BYTE IOAH;
	BYTE DCTL;
	BYTE DCTH;
}DMA_Descriptor;	

#define DMA0  (volatile DMA_Descriptor *)(&BAPL0)
#define DMA1  (volatile DMA_Descriptor *)(&BAPL1)
#define DMA2  (volatile DMA_Descriptor *)(&BAPL2)
#define DMA3  (volatile DMA_Descriptor *)(&BAPL3)
#define DMA4  (volatile DMA_Descriptor *)(&BAPL4)
#define DMA5  (volatile DMA_Descriptor *)(&BAPL5)
#define DMA6  (volatile DMA_Descriptor *)(&BAPL6)
#define DMA7  (volatile DMA_Descriptor *)(&BAPL7)
#define DMA8  (volatile DMA_Descriptor *)(&BAPL8)
#define DMA9  (volatile DMA_Descriptor *)(&BAPL9)
#define DMA10 (volatile DMA_Descriptor *)(&BAPL10)
	
	
typedef struct
{
	BYTE	ch;  	// номер канала DMA 0..10
	BYTE	nIRQ;	// Ќомер прерывани€ на которое прив€зываетс€ данный канал DMA 
	WORD	adrIOA;	// адрес регистра ввода вывода
	DWORD	adrBAP;	// адрес буфера
	WORD	Count;	// количество пересылаемых символов
	BYTE	BPD;	// имеет значение при BF=0, 0 - инкремент BAP, 1 - декремент BAP
	BYTE	IF;		// 	0 - инкремент IOA песле каждой передачи
	BYTE	BW;		// размер передаваемых данных 0 - byte 1-word
	BYTE	BF;		// 0 - изменение BAP после каждой передачи(см. BPD) 1- BAP немен€етс€
	BYTE	DIR;	// направление передачи, 0 - IOA->BAP, 1 - BAP->IOA
	BYTE	SE;		// реакци€ на сигнал DMA STOP; 0 - нет реакции, 1 - передача останавливаетс€
}DmaConfig_t;
  
//---------------------------------------------------------------------------
void DMA_change_BAP(BYTE nCH, DWORD	adrBAP);	// мен€ем адрес буфера
void DMA_restart(BYTE nCH, WORD	Count);			// перезапуск канала с установкой нового значени€ кол-ва байт
void InitDAM_ch(DmaConfig_t  * DmaConfig);		// »нициализаци€ DMA 
void DMA_Enable(BYTE nCH);
void DMA_Disable(BYTE nCH);
void DMA_change_BAP_restart(BYTE nCH, DWORD	adrBAP, WORD Count);
void DrawDMAStatus(void);
WORD DMA_GetCnt(BYTE nCH);
void DMA_ClearRequest(BYTE nCH);


//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------
#endif
