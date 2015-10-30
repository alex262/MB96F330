#include "mb96338us.h"
#include "global.h"
#include "timer.h"
#include "can.h"
#include "appli.h"

TProgram program;
char_t B_Out[255];

BYTE	WATCH_DOG_CODE = 0x55;		

TYPE_DATA_TIMER TIME_OUT_CAN=2000/TIMER_RESOLUTION_MS;
static TYPE_DATA_TIMER TimerDelay;
static TYPE_DATA_TIMER TimerStart;
static BYTE	StartB;

static WORD	DisInt = 0;
//------------------------------------------------------------------------------
#pragma inline DisInterrupt
void DisInterrupt(void)
{
	DisInt++;
	__DI();
}
#pragma inline EnInterrupt
void EnInterrupt(void)
{
	if(DisInt>0) DisInt--;
	
	if(DisInt == 0) __EI();
}
//--------------------------------------------------------------------
BYTE check_NaN_Inf( float x )
{
	DWORD * iX = (DWORD*)(&x);
	if ( ((*iX) & 0x7F800000) == 0x7F800000 )
	{
		return TRUE;
	}
	// ¬озвращает true, если x = NaN или +-Inf
	return FALSE;
}
//------------------------------------------------------------------------------
void msDelay(WORD ms)
{
	TimerDelay = ms/TIMER_RESOLUTION_MS;
	add_timer(&TimerDelay);
	while(getTimer(&TimerDelay)>0) clrwdt;	
	del_timer(&TimerDelay);
}
//------------------------------------------------------------------------------
void InitProgrammVar(void)
{
	//----------------------------------------------
	#ifdef POWER_BLOCK_ENABLE
		WORD	*pRamWD, tmp;
		BYTE	cnt;
		
		pRamWD = (WORD *)(ADDR_RAM_WD);
		
		program.StPwr = 0;
		//-----------------------------------
		// «ачитываем из EEPROM статус работы блока питани€ и был ли перезапуск по вкл питани€
		
		// вставл€ем задержку дл€ запуска EEPROM
		msDelay(500);
		tmp = pRamWD[0];
		
		SetWorkChI2C(1);
		HighDensSequentialRead(ADDR_EEPROM_ST_PWR, (BYTE *)(&program.EE_StPwr), 4);
		
		if((tmp == 0xA55A)&&((program.EE_StPwr&0xFF000000)==0xA5000000)) // блок уже запускалс€
		{
			if((program.EE_StPwr&0x00010000)==0x00000000) // блок уже запускалс€ но упал по питанию
			{
				cnt = program.EE_StPwr&0xFF;	// количество перезапусков
				if(cnt>3)
				{
					program.StPwr = 3;	// блок питани€ не включаем есть неисправность
				}else // пытаемс€ включитс€ снова
				{
					cnt++;
					program.EE_StPwr = (program.EE_StPwr&0xFFFFFF00)|cnt;
					program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
					add_timer(&program.TimerPwr);
					program.StPwr = 1;
				
					HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
				}
			}else	//в прошлый раз блок запустилс€ корректно
			{		// разрешаем вкл питание
				program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
				add_timer(&program.TimerPwr);
				program.StPwr = 1;
				
				program.EE_StPwr = 0xA5000000;	// включаем питание попытка перва€
				HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
			}
		}else 
		{
			program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
			add_timer(&program.TimerPwr);
			program.StPwr = 1;
			
			program.EE_StPwr = 0xA5000000;	// включаем питание попытка перва€
			HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
		}
		pRamWD[0] = 0xA55A;
		//----------------------------------------
		// инициализаци и подготовка к запуску блока питани€
		PCN8_RTRG	= 1;	// 1 tic 20.83 ns
		PCN8_MDSE	= 1; 	// one shot
		PCN8_PGMS	= 0;
		PCN8_OE		= 1;
		PCN8_OSEL	= 0;
		PCN8_EGS	= 1;
		PCSR8		= 336;	// 7 мкс
		PDUT8		= 144;	// 3 мкс
		PCN8_CNTE	= 0;
		
		PCN9_RTRG	= 1;	// 1 tic 20.83 ns
		PCN9_MDSE	= 1; 	// one shot
		PCN9_PGMS	= 0;
		PCN9_OE		= 1;
		PCN9_OSEL	= 1;
		PCN9_EGS	= 1;
		PCSR9		= 432;	// 9 мкс
		PDUT9		= 336;	// 7 мкс
		PCN9_CNTE	= 0;
		//---------------------
		PCN10_RTRG	= 1;	// 1 tic 20.83 ns
		PCN10_MDSE	= 1; 	// one shot
		PCN10_PGMS	= 0;
		PCN10_OE	= 1;
		PCN10_OSEL	= 0;
		PCN10_EGS	= 1;
		PCSR10		= 336; 
		PDUT10		= 144; 
		PCN10_CNTE	= 0;
		
		PCN11_RTRG	= 1;	// 1 tic 20.83 ns
		PCN11_MDSE	= 1;	// one shot
		PCN11_PGMS	= 0;
		PCN11_OE	= 1;
		PCN11_OSEL	= 1;
		PCN11_EGS	= 1;
		PCSR11		= 432; 
		PDUT11		= 336; 
		PCN11_CNTE	= 0;
		
		GCN12	= 0x5555;	// Trigger Source: Reload Timer 1
		TMRLR1	= 120;		// set reload value
		TMCSR1	= 0x1053;	// prescaler 1:1, no interrupts, output enable 83,3 ns
		ON1_PWR_OFF;
		ON2_PWR_OFF;
		
		program.setON1 = 1;
		program.setON2 = 1;
		
		program.setPPG1ch_en = TRUE;
		program.setPPG2ch_en = TRUE;
	#endif
	//===================================
	LEDR_DIR	= PIN_DIR_OUT;
	LEDG_DIR	= PIN_DIR_OUT;
	ADDR_DIR_IN;
	ADDR_EN;
	//===================================
	StartB = FALSE;
	TimerStart = 1000/TIMER_RESOLUTION_MS;
	add_timer(&TimerStart);
	//===================================
	program.StEEPROM_Write = FALSE;
	program.StEEPROM	= FALSE;
	//===================================
	program.ms				= 0;
	program.sec				= 0;
	program.min				= 0;
	program.hour			= 0;
	program.day				= 0;
	//===================================
	program.Device_ReStart	= 0;	//причина перезапу ка микроконтроллера
	program.watch_dog		= 0;
	program.REGIM			= 0;
	program.Error			= 0;
	program.TimerCan0		= 0;
	program.TimerCan1		= 0;
	program.TimerCan2		= 0;
	program.COUNT			= 0;
	//===================================
//	program.serial_number_1 = 0;	// серийный номер блока .. зачитываетс€ из чипа
//	program.serial_number_2 = 0;	

	program.Cnt1WareDev = 0;

#ifdef EN_SR_ZN_CYKL
	program.CountCikl_1s	= 0;
	program.SrCikl_mks		= 0;
#endif
	
	if(SPEED_CAN_0)
		add_timer(&program.TimerCan0);
	if(SPEED_CAN_1)
		add_timer(&program.TimerCan1);
	if(SPEED_CAN_2)
		add_timer(&program.TimerCan2);
}
//------------------------------------------------------------------------------
void CyclicModules(void)
{
	BYTE i;
	//----------------------------------------------------
	// если адрес изменилс€ в течение первой секнды то переинициализируем блок
	if(StartB == FALSE)
	{
		if(getTimer(&TimerStart))	
		{
			i=ADDR;	
			if(i!=ADDR_NODE)
			{
				setTimer(&TimerStart,1000);
				setState(Initialisation);
				ADDR_NODE			=ADDR;
			}
		}else
		{
			del_timer(&TimerStart);
			StartB = TRUE;
		}
	}
	//----------------------------------------------------
	// работа с eeprom 
	if(program.StEEPROM == TRUE)
	{
		program.StEEPROM = FALSE;
		SetWorkChI2C(1);
		if(program.RW_EEPROM == 0) // read
		{
			if(program.Len>0)
			{
				HighDensSequentialRead(program.Addr, program.BufEEPROM, program.Len);
				CreateAndSend_Pkt_UART0(program.BufEEPROM, program.Len, 2, 0xFD);
			}
		}
		if(program.RW_EEPROM == 1)
		{
			if(program.Len>0)
			{
				HighDensPageWrite(program.Addr, program.BufEEPROM, program.Len);
				program.StEEPROM_Write = TRUE;
			}
		}
	}

	// управление блоком питани€
	#ifdef POWER_BLOCK_ENABLE
		//--------------------------------------------
		// включение блока питани€ прошло успешно
		if(program.StPwr == 1)
		{
			if(getTimer(&program.TimerPwr) == 0)
			{
				SetWorkChI2C(1);
				program.EE_StPwr = 0xA5010000;	// питание включено 
				HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
				program.StPwr = 2;	
			}	
		}
		if(program.StPwr == 3)
		{
			program.stFLT1		= 0;
			program.stFLT2		= 0;
			program.stERR1		= 0;
			program.stERR2		= 0;
		}else
		{
			program.stFLT1		= FLT1_PWR;
			program.stFLT2		= FLT2_PWR;
			program.stERR1		= ERR1_PWR;
			program.stERR2		= ERR2_PWR;
		}
		if((program.StPwr == 1)||(program.StPwr == 2))
		{
			ON1_PWR = (~program.setON1)&1;
			ON2_PWR = (~program.setON2)&1;
		
			if((program.setPPG1ch_en == TRUE)&&((PCN8_CNTE == 0)||(PCN9_CNTE == 0)))
			{
				PCN8_CNTE	= 1;
				PCN9_CNTE	= 1;
			}
			if(program.setPPG1ch_en == FALSE)
			{
				PCN8_CNTE	= 0;
				PCN9_CNTE	= 0;
			}
			if((program.setPPG2ch_en == TRUE)&&((PCN10_CNTE == 0)||(PCN11_CNTE == 0)))
			{
				PCN10_CNTE	= 1;
				PCN11_CNTE	= 1;
			}
			if(program.setPPG2ch_en == FALSE)
			{
				PCN10_CNTE	= 0;
				PCN11_CNTE	= 0;
			}
		}
	#endif
}
//------------------------------------------------------------------------------
void InitWatchDog(void)
{
	WDTC = 0x2F;       // 2^15/CLKWT, CLKWT = CLKMC 2 s
	WDTCP = WATCH_DOG_CODE;      // Activate Watchdog (Pattern A)
	WATCH_DOG_CODE = ~WATCH_DOG_CODE;
}
//------------------------------------------------------------------------------
void SetError(BYTE Error)
{
	SETBIT(program.Error,Error);
}
//------------------------------------------------------------------------------
void ClearError(BYTE Error)
{
	CLEARBIT(program.Error,Error);
}
//------------------------------------------------------------------------------
BYTE GetError()
{
	BYTE i;
	
	for(i=0;i<16;i++)
	{
		if(digit(program.Error,15-i)==1)
			return (15-i);
	}
	return 0;
}
//------------------------------------------------------------------------------
void Indicate(WORD *pCount, WORD BLINK)
{
	if ((*pCount) < BLINK ) return;
	switch (GetError())
	{
		case ind_WORK:
			LEDR_OFF;
			LEDG_ON;	
			break;
		case ind_COMA:
			LEDR_ON;
			if((*pCount)>=BLINK)
			{
				(*pCount)=0;
				LEDG=~LEDG;
			}
			break;
		case ind_COM1:
			if((*pCount)>=BLINK)
			{
				(*pCount)=0;
				LEDG=~LEDG;
				LEDR=~LEDR;
			}
			break;
		case ind_FATERR:
			LEDG_OFF;
			if((*pCount)>=BLINK)
			{
				(*pCount)=0;
				LEDR=~LEDR;
			}
			break;
		case ind_RESET:
			LEDR_ON;
			LEDG_OFF;
			break;
		case ind_TEST:
			LEDR_OFF;
			if((*pCount)>=BLINK)
			{
				(*pCount)=0;
				LEDG=~LEDG;
			}
			break;
		case ind_ERROR:
			LEDR_ON;
			if((*pCount)>=BLINK)
			{
				(*pCount)=0;
				LEDR=~LEDR;
			}
			break;
		case ind_BREST:
			LEDR_ON;
			LEDG_ON;
			break;		
	}	
}

