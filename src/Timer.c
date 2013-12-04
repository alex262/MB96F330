#include "mb96338us.h"
#include "timer.h"
#include "global.h"
#include "uart.h"

TSystemTimers SystemTimers;

void InitTimerService(void);
void TimerService(void);



void SetupTime(void)
{ 
	InitTimerService();

}

//=============================================================================================================
TYPE_DATA_TIMER getTimer(TYPE_DATA_TIMER * time )
{
	TYPE_DATA_TIMER wReturnValue;
	TYPE_DATA_TIMER wTestTime;

	wReturnValue = *time;
	wTestTime = *time;
	if( wTestTime != wReturnValue )
	{
		// maybe an overflow occured. call this function again (recursive)
		wReturnValue = getTimer( time );
	}
	return wReturnValue;
}
//=====================================================
void setTimer( TYPE_DATA_TIMER * time, TYPE_DATA_TIMER value )
{
	*time = value;
	if( *time != value )
	{ 
		// it seems that the irq-serviceroutine was called during setting the
		// value, so try again
		setTimer( time, value );
	}
}
//=============================================================================================================

void InitTimerService(void)
{
	BYTE cnt;

	SystemTimers.count = 0;
	cnt = COUNT_USER_TIMERS;
	
	while (cnt--)
	{				
		SystemTimers.timers[cnt] = 0;
	}
}
void PrintTimerService(void)
{
	BYTE cnt, i;

	puts("Timers ");
	putdec(SystemTimers.count,2);
	puts(" \n\r");
	
	cnt = COUNT_USER_TIMERS;
	
	i = 0;
	while (cnt--)
	{				
		i++;
		putdec((DWORD)cnt, 2);
		puts("-");
		puthex((U32)SystemTimers.timers[cnt], 6);
		if(i>3) {i = 0; puts(" \n\r");}
		else puts(" | ");
	}
	puts(" \n\r");
}

//регистраци€ пользовательского таймера в системе
//¬ход: указатель на таймер usrtmr
//
void add_timer(TYPE_DATA_TIMER * tmr)
{
	BYTE cnt;
	cnt = COUNT_USER_TIMERS;


	while (cnt--)
	{															//данный таймер уже зарегистрирован
		if (SystemTimers.timers[cnt]==tmr) 	return;				//выход
	}
	
	cnt = COUNT_USER_TIMERS;
	while (cnt--)
	{		
		if (SystemTimers.timers[cnt] == 0) //место свободно
		{
			DisInterrupt();
			SystemTimers.timers[cnt] = tmr;
			SystemTimers.count++;  
			EnInterrupt();
			return;
		}
	}
}
//удаление таймера 
void del_timer(TYPE_DATA_TIMER * tmr)
{
	BYTE cnt;

	cnt = COUNT_USER_TIMERS;

	while (cnt--)
	{				
		if (SystemTimers.timers[cnt]==tmr) // таймер найден
		{
			DisInterrupt();
			SystemTimers.timers[cnt] = 0;
			SystemTimers.count--;  
			EnInterrupt();
			return;	
		}
	}
}
void TimerService(void)
{
	BYTE i;
	
	for (i=0; i<COUNT_USER_TIMERS; i++)	// декрементирование пользовательских таймеров	
	{
		if (SystemTimers.timers[i] != 0) 
		{
			if (*(SystemTimers.timers[i])>0) 
				(*(SystemTimers.timers[i]))--;
		}
	}
	
	program.ms++;
	if(program.ms == 1000)
	{
		program.ms = 0;
		program.sec++;
		if(program.sec == 60)
		{
			program.sec = 0;
			program.min++;
			if(program.min == 60)
			{
				program.min = 0;
				program.hour++;
				if(program.hour == 24)
				{
					program.hour = 0;
					program.day++;
				}
			}
		}
	}
}

//=============================================================================================================
