/*---------------------------------------------------------------------------
  MAIN.C
/*---------------------------------------------------------------------------*/
#include "mb96338us.h"
#include "global.h"
#include "uart.h"
#include "rlt.h"
#include "timer.h"
#include "pakuart.h"
#include "can.h"
#include "i2c.h"
#include "D1Ware.h"

#ifdef CAN_OPEN_ENABLE
	#include "objacces.h"
	#include "init.h"
	#include "appli.h"
#endif

#include INCLUDE_H


#ifdef EN_SR_ZN_CYKL
	TYPE_DATA_TIMER TimerSrZnC=1000;
#endif
	
#ifdef CASH_PDO_DATA
	TYPE_DATA_TIMER TimerCashCan=0;
#endif

void wait_(unsigned long a)
{
	unsigned long i;
	
	for (i = 0; i < a; i++)
	{
		__wait_nop();
	}
}

void main(void)
{
	#ifdef CAN_OPEN_ENABLE
		BYTE i;
	#endif
	//===================================
	//#if ((BUFFER_LEN_UART>1000)||(COUNT_UART_BUF>4))
	InitWatchDog();
	//#endif
	
	I2C_Init(0);
	I2C_Init(1);
	
	InitUART(0);
	SetupTime();
	InitReloadTimer0();
	InitServiceUart0();
	InitIrqLevels(); 
	__set_il(7);                /* allow all levels           */
	__EI();                     /* globally enable interrupts */
	//===================================
	InitProgrammVar();
	//===================================
	if(INIT_BLOK!=NULL)
		INIT_BLOK();
	//===================================
	#ifdef CAN_OPEN_ENABLE
		setState(Initialisation);
	#else
		SetupCAN(SPEED_CAN_0, SPEED_CAN_1, SPEED_CAN_2);
	#endif
	//===================================
	#ifdef EN_SR_ZN_CYKL
		add_timer(&TimerSrZnC);
	#endif
	#ifdef CASH_PDO_DATA
		add_timer(&TimerCashCan);
	#endif
	puts((BYTE *)"===============================================================================\r\n");
	puts((BYTE *)"                        Terminal Fujitsu MB96F338                              \r\n");
	puts((BYTE *)"===============================================================================\r\n$>");
				
	while(1)
	{
		//========================================
		clrwdt;					// обнуление WatchDog таймера 
		//========================================
		CyclicModules();		// цикличные операции выполняемые в каждом блоке
		//========================================
		if(DRIVER_BLOK!=NULL)	// Функционал конкретного блока
			DRIVER_BLOK();
		//========================================
		#ifdef CAN_OPEN_ENABLE
			
			receiveMsgHandler(0);
			receiveMsgHandler(1);
			receiveMsgHandler(2);
			DriverCanOpen();
			#ifdef CASH_PDO_DATA
				if(getTimer(&TimerCashCan)==0)
				{
					setTimer(&TimerCashCan, 30);	// обновляем данные в объектном словаре 1 раз в 30 мс
					for(i=0;i<30;i++) buildPDO_CASH(0x1800+i);
					
					if(program.Cnt1WareDev == 0)
						GetDataWare(0);
				}
			#endif
		#endif
		//========================================
		#ifdef TERMINAL_EN
			Terminal();
		#endif
		//========================================
		// подсчёт среднего времени цикла в мкс
		#ifdef EN_SR_ZN_CYKL
			program.CountCikl_1s++;
			if(getTimer(&TimerSrZnC)==0)
			{
				program.SrCikl_mks = 1000000/program.CountCikl_1s; //	среднее значение цикла в мкс
				program.CountCikl_1s = 0;
				setTimer(&TimerSrZnC, 1000);
			}
		#endif
		//========================================
	}
}