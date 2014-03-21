#ifndef __TIMERMB
#define __TIMERMB
#include "global.h"




#define COUNT_USER_TIMERS	15
//#define TYPE_DATA_TIMER		WORD

typedef struct
{
	BYTE count;
	TYPE_DATA_TIMER * timers[COUNT_USER_TIMERS];
}TSystemTimers;

void add_timer(TYPE_DATA_TIMER * tmr);
void del_timer(TYPE_DATA_TIMER * tmr);
TYPE_DATA_TIMER getTimer(TYPE_DATA_TIMER * time );
void setTimer( TYPE_DATA_TIMER * time, TYPE_DATA_TIMER value );



#define  	TIMER_RESOLUTION_MS		1
#define 	TIMER_RESOLUTION_US 	1000


extern WORD TIME_OUT_CAN;
void SetupTime(void);




#endif
