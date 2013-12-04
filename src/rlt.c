#include "mb96338us.h"  /* include all IO-Register definitions */
#include "global.h"

/* Initialization of a reload timer*/
void InitReloadTimer0(void) 
{
	TMCSR0=0;
	// prescaler now 2*TMSR_PRESCALE_2=4
	TMCSR0_UF=1; 
	//timer operates in reload mode
	TMCSR0_RELD=1; 
	// Reload value 12Mhz/0,001Mhz - 1=12000-1=11999	// 1 ms
	TMRLR0 = 11999; 
	//timer enabled
	TMCSR0_CNTE=1; 
	//start timer
	TMCSR0_TRG=1;  
	//enable the timer interrupt
	TMCSR0_INTE=1; 
}
//--------------------------------------------------------------------------------
static WORD CountMs=0;
__interrupt void ReloadTimer0(void) 
{
	//BYTE i;
	if(TMCSR0_UF) // Reload Timer interrupt request
	{	
		TMCSR0_UF=0; // clear interrupt flag
		//-----------------------------------
		// светодиоды
		CountMs++;
		Indicate(&CountMs,200);
		//-----------------------------------
		// Декрементирование пользовательских таймеров
		TimerService();
	}
}