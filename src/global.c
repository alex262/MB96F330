#include "mb96338us.h"
#include "global.h"
#include "timer.h"
#include "can.h"
#include "appli.h"
#include "flash2.h"

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
	//LEDT1 = 1;
}
#pragma inline EnInterrupt
void EnInterrupt(void)
{
	if(DisInt>0) DisInt--;
	
	if(DisInt == 0) 
	{
		__EI();
		//LEDT1 = 0;
	}
}
//------------------------------------------------------------------------------
// Возвращает true, если x = NaN или +-Inf
BYTE check_NaN_Inf( float x )
{
	DWORD * iX = (DWORD*)(&x);
	if ( ((*iX) & 0x7F800000) == 0x7F800000 )
	{
		return TRUE;
	}
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
		// Зачитываем из EEPROM статус работы блока питания и был ли перезапуск по вкл питания
		
		// вставляем задержку для запуска EEPROM
		msDelay(500);
		tmp = pRamWD[0];
		
		SetWorkChI2C(1);
		HighDensSequentialRead(ADDR_EEPROM_ST_PWR, (BYTE *)(&program.EE_StPwr), 4);
		
		if((tmp == 0xA55A)&&((program.EE_StPwr&0xFF000000)==0xA5000000)) // блок уже запускался
		{
			if((program.EE_StPwr&0x00010000)==0x00000000) // блок уже запускался но упал по питанию
			{
				cnt = program.EE_StPwr&0xFF;	// количество перезапусков
				if(cnt>3)
				{
					program.StPwr = 3;	// блок питания не включаем есть неисправность
				}else // пытаемся включится снова
				{
					cnt++;
					program.EE_StPwr = (program.EE_StPwr&0xFFFFFF00)|cnt;
					program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
					add_timer(&program.TimerPwr);
					program.StPwr = 1;
				
					HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
				}
			}else	//в прошлый раз блок запустился корректно
			{		// разрешаем вкл питание
				program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
				add_timer(&program.TimerPwr);
				program.StPwr = 1;
				
				program.EE_StPwr = 0xA5000000;	// включаем питание попытка первая
				HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
			}
		}else 
		{
			program.TimerPwr = 1000/TIMER_RESOLUTION_MS;
			add_timer(&program.TimerPwr);
			program.StPwr = 1;
			
			program.EE_StPwr = 0xA5000000;	// включаем питание попытка первая
			HighDensPageWrite(ADDR_EEPROM_ST_PWR, (BYTE*)(&program.EE_StPwr), 4);
		}
		pRamWD[0] = 0xA55A;
		//----------------------------------------
		// инициализаци и подготовка к запуску блока питания
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
	#ifdef LEDR
		LEDR_DIR	= PIN_DIR_OUT;
	#endif
	#ifdef LEDG
		LEDG_DIR	= PIN_DIR_OUT;
	#endif
	#ifdef LEDT1
		LEDT1_DIR	= PIN_DIR_OUT;
	#endif
	#ifdef LEDT2
		LEDT2_DIR	= PIN_DIR_OUT;
	#endif
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
//	program.serial_number_1 = 0;	// серийный номер блока .. зачитывается из чипа
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
	// если адрес изменился в течение первой секнды то переинициализируем блок
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

	// управление блоком питания
	#ifdef POWER_BLOCK_ENABLE
		//--------------------------------------------
		// включение блока питания прошло успешно
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
	#ifdef LEDG
	#ifdef LEDR
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
			LEDG_ON;
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
	#endif
	#endif
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
// Обновление Bootloadera
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
static const char_t STR_OK_R[10] = {'O','K',0,0,0,0,0,0,0,0};
static const char_t STR_OK_A_R[5] = {'O','K',' ','A',0};
static const char_t STR_OK_B_R[5] = {'O','K',' ','B',0};
//----------------------------------------------------------------------------------------------------
void CAN0_buffer_R(U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB0 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB0_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB0_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK0			= MSG2STD(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR0_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR0_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR0_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR0_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR0_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR0_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR0_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR0_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK0  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ0 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
void CAN1_buffer_R(U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB1 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB1_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB1_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK1			= MSG2STD(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK21_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR1_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR1_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR1_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR1_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR1_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR1_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR1_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR1_EOB	= 1;	// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK1  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ1 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
void CAN2_buffer_R(U8 Num, U8 Dir, U16 setID, U32 mask)
{

	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB2 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB2_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB2_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK2			= MSG2STD(mask);		// <<< Setup Mask corresponding to your application 
	IF1MSK22_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR2_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR2_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR2_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR2_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR2_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR2_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR2_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR2_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK2  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ2 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//-------------------------------------------------------------------------------------------
void CAN_buffer_R(U8 NumCan, U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if(NumCan == 0)
		CAN0_buffer_R(Num, Dir, setID, mask);
	else if(NumCan == 1)
		CAN1_buffer_R(Num, Dir, setID, mask);
	else if(NumCan == 2)
		CAN2_buffer_R(Num, Dir, setID, mask);
}
//-------------------------------------------------------------------------------------------
void CAN0_buffer_off_R(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB0_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR0_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR0_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR0_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK0  = 0xF0;

	IF1CREQ0 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//-------------------------------------------------------------------------------------------
void CAN1_buffer_off_R(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB1_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR1_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR1_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR1_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK1  = 0xF0;

	IF1CREQ1 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//-------------------------------------------------------------------------------------------
void CAN2_buffer_off_R(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB2_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR2_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR2_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR2_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK2  = 0xF0;

	IF1CREQ2 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//-------------------------------------------------------------------------------------------
void CAN_buffer_off_R(U8 NumCan, U8 Num)
{
	if(NumCan == 0)
		CAN0_buffer_off_R(Num);
	else if(NumCan == 1)
		CAN0_buffer_off_R(Num);
	else if(NumCan == 2)
		CAN0_buffer_off_R(Num);
}
//-------------------------------------------------------------------------------------------
void CAN_ConfigMsgBox_R(U8 NumCan, U16 ID)
{
	U8 i;
	
	for(i=0;i<31;i++) // первые 31 ящик работают на приём
	{
		if(i==0)
			CAN_buffer_R(NumCan, i+1, 0, ID, 0x7FF); // настраиваем один ящик на прием
		else
			CAN_buffer_off_R(NumCan, i+1);	// остальные ящики отключаем
	}
}
//-------------------------------------------------------------------------------------------
U8 CAN0_SendMessage_R(Message *msg)
{
	U32 timeout=0;
	
	// Prepare Arbitration Interface Register 
	IF1ARB0 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB0_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB0_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR0		=(msg->len&0xF) | (1<<7);

	IF1DTA0 = *((U32 *)(msg->data));
	IF1DTB0 = *((U32 *)(msg->data+4));

	IF1CMSK0  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR0 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR0_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR0_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK0  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR0_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK0_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CAN1_SendMessage_R(Message *msg)
{
	U32 timeout=0;
	// Prepare Arbitration Interface Register 
	IF1ARB1 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB1_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB1_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR1		=(msg->len&0xF) | (1<<7);

	IF1DTA1 = *((U32 *)(msg->data));
	IF1DTB1 = *((U32 *)(msg->data+4));

	IF1CMSK1  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR1 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR1_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR1_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK1  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR1_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK1_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CAN2_SendMessage_R(Message *msg)
{
	U32 timeout=0;
	// Prepare Arbitration Interface Register 
	IF1ARB2 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB2_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB2_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR2		=(msg->len&0xF) | (1<<7);

	IF1DTA2 = *((U32 *)(msg->data));
	IF1DTB2 = *((U32 *)(msg->data+4));

	IF1CMSK2  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR2 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR2_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR2_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK2  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR2_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK2_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CanSendMsg_R(U8 nCAN, Message *msg)
{
	if(nCAN == 0) return CAN0_SendMessage_R(msg);
	if(nCAN == 1) return CAN1_SendMessage_R(msg);
	if(nCAN == 2) return CAN2_SendMessage_R(msg);
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
void SendCanBuf_R(U8 nCAN, U8 *buf, U8 len, U16 ID)
{
	Message msg;
	U8 i;
	
	msg.rtr = 0;
	msg.cob_id = ID;
	
	if(len>0)
	{
		while(len>0)
		{
			clrwdt;	// обнуление WatchDog таймера 
			
			if(len>8) msg.len = 8;
			else msg.len = len;
			
			len -= msg.len;
			
			for(i=0; i<msg.len; i++)
				msg.data[i] = *buf++;
				
			CanSendMsg_R(nCAN, &msg);
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
void ResetCan_R(U8 nCan)
{
	if(nCan == 0)
	{
		/*Restart bus*/
		CTRLR0_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT0_TEC!=0)||(ERRCNT0_REC!=0));//see if recovered
	}
	if(nCan == 1)
	{
		/*Restart bus*/
		CTRLR1_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT1_TEC!=0)||(ERRCNT1_REC!=0));//see if recovered
	}
	if(nCan == 2)
	{
		/*Restart bus*/
		CTRLR2_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT2_TEC!=0)||(ERRCNT2_REC!=0));//see if recovered
	}
}
//------------------------------------------------------------------------------------------------------------------
U8 CanReciveMsg0_R(Message *msg)
{
	if(INTR0 == 0x8000)		/* status int */
	{
		if(STATR0_BOFF == 1)		//bus off
		{
			ResetCan_R(0);
		}
		if(STATR0_EWARN==0x01)		//error warning
		{
			ResetCan_R(0);
		}
		if(!((STATR0_BOFF)|(STATR0_EWARN)|(STATR0_EPASS))) //error active
		{
			ResetCan_R(0);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT0 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK0	= 0x003F;	
		IF2CREQ0	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR0_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR0_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK0		= 0x0090;	//WRRD=1 CONTROL=1 other=0
										//for clear MSGLST
			IF2CREQ0 = 1;
		}
			
		msg->rtr		= IF2ARB0_DIR;
		msg->len		= IF2MCTR0_DLC;
		msg->cob_id		= STD2MSG(IF2ARB0);
		*((U32 *)msg->data)     = IF2DTA0;
		*((U32 *)(msg->data+4)) = IF2DTB0;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CanReciveMsg1_R(Message *msg)
{
	if(INTR1 == 0x8000)		/* status int */
	{
		if(STATR1_BOFF == 1)		//bus off
		{
			ResetCan_R(1);
		}
		if(STATR1_EWARN==0x01)		//error warning
		{
			ResetCan_R(1);
		}
		if(!((STATR1_BOFF)|(STATR1_EWARN)|(STATR1_EPASS))) //error active
		{
			ResetCan_R(1);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT1 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK1	= 0x003F;	
		IF2CREQ1	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR1_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR1_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK1		= 0x0090;	//WRRD=1 CONTROL=1 other=0
											//for clear MSGLST
			IF2CREQ1 = 1;
		}
			
		msg->rtr		= IF2ARB1_DIR;
		msg->len		= IF2MCTR1_DLC;
		msg->cob_id		= STD2MSG(IF2ARB1);
		*((U32 *)msg->data)     = IF2DTA1;
		*((U32 *)(msg->data+4)) = IF2DTB1;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CanReciveMsg2_R(Message *msg)
{
	if(INTR2 == 0x8000)		/* status int */
	{
		if(STATR2_BOFF == 1)		//bus off
		{
			ResetCan_R(2);
		}
		if(STATR2_EWARN==0x01)		//error warning
		{
			ResetCan_R(2);
		}
		if(!((STATR2_BOFF)|(STATR2_EWARN)|(STATR2_EPASS))) //error active
		{
			ResetCan_R(2);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT2 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK2	= 0x003F;	
		IF2CREQ2	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR2_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR2_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK2		= 0x0090;	//WRRD=1 CONTROL=1 other=0
										//for clear MSGLST
			IF2CREQ2 = 1;
		}
			
		msg->rtr		= IF2ARB2_DIR;
		msg->len		= IF2MCTR2_DLC;
		msg->cob_id		= STD2MSG(IF2ARB2);
		*((U32 *)msg->data)     = IF2DTA2;
		*((U32 *)(msg->data+4)) = IF2DTB2;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
U8 CanReciveMsg_R(U8 nCAN, Message *msg)
{
	if(nCAN == 0) return CanReciveMsg0_R(msg);
	if(nCAN == 1) return CanReciveMsg1_R(msg);
	if(nCAN == 2) return CanReciveMsg2_R(msg);
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
static BOOL cmd = FALSE;

#define CMD_NONE					0x00
#define CMD_EXIT					0x01
#define CMD_ERASE					0x03
#define CMD_REBOOT					0x04
#define CMD_READ					0x05
#define CMD_WRITE					0x06

#define STR_EXIT					"EXIT"
#define STR_ERASE					"ERASE"
#define STR_REBOOT					"REBOOT"
#define STR_READ					"READ"
#define STR_WRITE					"WRITE"

static BYTE   cmd_type = CMD_NONE;

#define END_LINE	'\0'

//! flag for first ls : mount if set
//static bool first_ls = true ;
//! string length
static BYTE   i_str = 0;

//! string for command
#define MAX_LEN_CMD_LINE	50

static char cmd_str[MAX_LEN_CMD_LINE];
//! string for first arg
static char par_str1[20];
//! string for second arg
static char par_str2[20];

static void parse_cmd(void)
{
	BYTE i, j;

	// Get command type.
	for (i = 0; cmd_str[i] != ' ' && i < i_str; i++);

	if (i)
	{
		cmd = TRUE;
		// Save last byte
		j = cmd_str[i];
		// Reset vars
		cmd_str[i] = '\0';
		par_str1[0] = '\0';
		par_str2[0] = '\0';

		// Decode command type.
		if      (!strcmp(cmd_str, STR_EXIT		)) cmd_type = CMD_EXIT;
		else if (!strcmp(cmd_str, STR_ERASE		)) cmd_type = CMD_ERASE;
		else if (!strcmp(cmd_str, STR_REBOOT	)) cmd_type = CMD_REBOOT;
		else if (!strcmp(cmd_str, STR_READ		)) cmd_type = CMD_READ;
		else if (!strcmp(cmd_str, STR_WRITE		)) cmd_type = CMD_WRITE;


		else
		{
			cmd = FALSE;
		}
		// restore last byte
		cmd_str[i] = j;
	}
	// if command isn't found, display prompt
	if (!cmd)
	{
		return;
	}

	// Get first arg (if any).
	if (++i < i_str)
	{
		j = 0;
		// remove " if used
		if (cmd_str[i] == '"')
		{
			i++;
			for (; cmd_str[i] != '"' && i < i_str; i++, j++)
			{
				par_str1[j] = cmd_str[i];
			}
			i++;
		}
		// get the arg directly
		else
		{
			for(; cmd_str[i] != ' ' && i < i_str; i++, j++)
			{
				par_str1[j] = cmd_str[i];
			}
		}
		// null terminated arg
		par_str1[j] = '\0';
	}

	// Get second arg (if any).
	if (++i < i_str)
	{
		j = 0;
		// remove " if used
		if (cmd_str[i] == '"')
		{
			i++;
			for (; cmd_str[i] != '"' && i < i_str; i++, j++)
			{
				par_str2[j] = cmd_str[i];
			}
			i++;
		}
		// get the arg directly
		else
		{
			for (; cmd_str[i] != ' ' && i < i_str; i++, j++)
			{
				par_str2[j] = cmd_str[i];
			}
		}
		// null terminated arg
		par_str2[j] = '\0';
	}
}

//------------------------------------------------------------------------------------------------------------------

void build_cmd_b_R(int c)
{
//	if((StBootloader == 0)&&(i_str == 0))// загрузчик не активирован
//	{
//		if(c != 'B') return;
//	}
	if(c == END_LINE)
	{
		// Add NUL char.
		//i_str--;
		cmd_str[i_str] = '\0';
		// Decode the command.
		parse_cmd();
		i_str = 0;
	}else
	{	// Echo.
		// Append to cmd line.
		cmd_str[i_str++] = c;
		if(i_str >= MAX_LEN_CMD_LINE) i_str=0;
	}
}

//------------------------------------------------------------------------------------------------------------------
void ObrCmd_R(U8 nCAN, U16 id);
BYTE ServiceBootloadUpd(BYTE bus_id, Message *m)
{
	Message msg;
	U8 i;
	U16 ID_;
	
	/* =============================================================== */
	// проверяем сообщение 
	/* =============================================================== */
	if(m->len == 7)
	if(m->data[0] == 'B')
	if(m->data[1] == 'O')
	if(m->data[2] == 'O')
	if(m->data[3] == 'T')
	if(m->data[4] == 'U')
	if(m->data[5] == 'P')
	if(m->data[6] == 'D')
	{
		/* =============================================================== */
		__DI();	//запрещяем все прерывания
		/* =============================================================== */
		#ifdef LEDG
		LEDR_ON;
		LEDG_OFF;
		#endif
		/* =============================================================== */
		/* Enable Sectors for FLASH writing and erasing                    */
		/* =============================================================== */
		FMWC1 = 0xFF;       // enable sector for FLASH writing
		FMWC5 = 0xFF;       // enable sector for FLASH writing 
		/* =============================================================== */
		/*    НАСТРАИВАЕМ CAN ДЛЯ ПРОГРАММАТОРА                            */
		/* =============================================================== */
		ID_	= ADDR|(((WORD)0xD)<<7);
		CAN_ConfigMsgBox_R(bus_id, ID_);	// Конфигурим наш CAN на прием сообщений с нашим адресом .. младшие 5 бит адресс остальные нули
		/* =============================================================== */
		SendCanBuf_R(bus_id, (U8 *)(STR_OK_R), 3, ID_);		// подтверждаем что загрузчик загрузился
		/* =============================================================== */
		// переходим к обнвлению загрузчика	
		/* =============================================================== */
		while(1)// принимаем сообщения 
		{
			clrwdt;					// обнуление WatchDog таймера 
			//----------------------------------------------------
			if(CanReciveMsg_R(bus_id, &msg) == TRUE)	
			{// приняли сообщение, обрабатываем
				if(msg.cob_id == ID_)
				{// обрабатываем только наши пакеты, хотя они и так фильтруются на входе
					
					for(i=0; i<msg.len; i++)
					{
						if (!cmd)
						{
							build_cmd_b_R(msg.data[i]);
						}
						// perform the command
						if (cmd)
						{
							ObrCmd_R(bus_id, ID_);
							
							// Reset vars.
							cmd_type = CMD_NONE;
							cmd = FALSE;
						}
					}
				}
			}
			//----------------------------------------------------
		}
		/* =============================================================== */
		//return 0;
	}
	/* =============================================================== */
	// Переходим к обновлению прошивки 
	ServiceBootloadCan(bus_id, m);
	/* =============================================================== */
	return 0;
}
//------------------------------------------------------------------------------------------------------------------
// par1 - номер CAN U16 - ID CAN сообщения 
#define COUNT_MEM_PART				2
const static unsigned long ADDR_START_MEM[COUNT_MEM_PART]	= {0xDF0000,0xF80000};
const static unsigned long ADDR_END_MEM[COUNT_MEM_PART]	= {0xDF7FFF,0xFFFFFF};
static BYTE BufferCAN[50];	// буфер 
static BYTE BufferReadCom[44];
static BYTE CountReadComm=0;
const char ASCII_R[] = "0123456789ABCDEF";
#define SWAP_USHORT_R(x) ( ((x) >> 8) | ((x) << 8) )	// Swap 2 bytes of a word
//------------------------------------------------------------------------------------------------------------------
void conv_hex_R(unsigned long n, char digits)
{
	char i,ch,div=0;

	div=(digits-1) << 2;		/* init shift divisor */

	for (i=0; i<digits; i++)
	{
		ch = (n >> div) & 0xF;	/* get hex-digit value */
		BufferCAN[i] =ASCII_R[ch];		/* prompt to terminal as ASCII */
		div-=4;					/* next digit shift */
	}
}
//------------------------------------------------------------------------------------------------------------------
void buf_8bit_hex_R(U8 num, U8 n)
{
	BufferCAN[num]		= ASCII_R[(n>>4)&0xF];
	BufferCAN[num+1]	= ASCII_R[n&0xF];
}
//------------------------------------------------------------------------------------------------------------------
void wait_mks_R(WORD n)
{
	WORD i;
	for(i=0; i<n; i++) 
	{
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
		__wait_nop();
	}
}
//------------------------------------------------------------------------------------------------------------------
DWORD StrHexToLong_R(BYTE *inputStr, BYTE lenStr)
/* 
const char *inputStr: input string
unsigned char lenStr: length of hey value
*/
{
	BYTE i;
	BYTE c;
	DWORD sum = 0;
	for(i=0;i<lenStr;i++)
	{
		c = inputStr[i];
		if ((c > 96) & (c < 123)) c = c - 32;
		if ((inputStr[i] >= '0') & (inputStr[i] <= '9'))
		{
			sum = sum * 16;
			sum += inputStr[i] - '0';
		} else if ((c >= 'A') & (c <= 'F'))
		{
			sum = sum * 16;
			sum += (c - 'A') + 10;
		} else
		{
			return sum;
		}
	}
	return sum;
}
//------------------------------------------------------------------------------------------------------------------
int write_MHX_com_R(BYTE * pData)
{
	__far unsigned int *adr;
	DWORD data,crc;
	BYTE crcbyte;
	BYTE ofs, i;

	if(pData[0] != 'S') return (-1); // error input
		
		
	if (pData[1] == '2')
	{// S-Record type 2
		// reading record length 
		i = (DWORD)StrHexToLong_R(&pData[2],2) & 0xFF; // storing record length
		if(i != 0x14) return (-3);
		crc=0x14;			// building CRC
			
		// storing addresslength and converting it into a even word address 
		adr = (__far unsigned int *)((unsigned long)StrHexToLong_R(&pData[4],6));
		
		if((DWORD)adr >= 0xDF0000)
		{
			// building crc 
			crc +=(DWORD)((((DWORD)adr & 0xFF0000) >> 16) + (((DWORD)adr & 0x00FF00) >> 8) + ((DWORD)adr & 0x0000FF));
			//crc +=pData[4]+pData[5]+pData[6]+pData[7]+pData[8]+pData[9];
				
			ofs = 10;
			for(i=0;(i<8);i++) // check if main app is located in bootloader section
			{
				// reading data
				data = StrHexToLong_R(&pData[ofs+4*i],4);	// conversion
		
				// building crc
				crc += (data & 0x00FF) + ((data & 0xFF00) >> 8);
				// swap data
				if(data != 0xFFFF)
				{
					data = SWAP_USHORT_R(data);	
					write2(adr,data);
				}
						
				adr++;
			}
			data = StrHexToLong_R(&pData[42],2);	// conversion
			crcbyte = (BYTE)data; /* data is CRC byte */
					
			// CRC check 
			if ((0xFF ^ (BYTE)crc) != crcbyte) 
			{
				// CRC error 
				//write((__far unsigned int *)RESETVECT,0x0000);	// deleting everything in the resetvector
				//write((__far unsigned int *)(RESETVECT+2),0x0000); // deleting everything in the resetvector
				return (-4);
			}
		}else return 0;
	}else return (-2);
	return (1);
}
//------------------------------------------------------------------------------------------------------------------
void conv_dec_R(uint32_t x)
{
	int16_t i;
	char_t buf[9];
	buf[8]='\0';				/* end sign of string */

	for (i=8; i>0; i--) 
	{
		buf[i-1] = ASCII_R[x % 10];
		x = x/10;
	}

	for (i=0; buf[i]=='0'; i++)	// no print16_t of zero 
	{
		buf[i] = ' ';
	}
	for(i=0;i<9;i++)
		BufferCAN[i] = buf[i];
}
//------------------------------------------------------------------------------------------------------------------
#define read_byte2(adr) (*(__far unsigned char *)(adr))

void ObrCmd_R(U8 nCAN, U16 id)
{
	Message msg;
	//char ch;
	BYTE i, j, Data[0x10], st_send, crc;
	DWORD adr, adr_s;
	int ret;
	
	switch (cmd_type)
	{
		case CMD_ERASE:
			//if(StBootloader == 1)
			/* =============================================================== */
			/* Erasing flash sectors                                     +++++ */
			/* =============================================================== */
			__DI();				// disables interrupts - needed for erasing flash
			erase2((__far unsigned int *)0xDF0000);	// erases SA32
			clrwdt;
			erase2((__far unsigned int *)0xDF2000);	// erases SA32
			clrwdt;
			erase2((__far unsigned int *)0xDF4000);	// erases SA32
			clrwdt;
			erase2((__far unsigned int *)0xDF6000);	// erases SA32
			clrwdt;
			
			SendCanBuf_R(nCAN, (U8 *)STR_OK_R, 3, id);
			/* =============================================================== */
			break;
		case CMD_REBOOT:
			/* =============================================================== */
			/* Reset microcontroller                                      +++++*/
			/* =============================================================== */
			SendCanBuf_R(nCAN, (U8 *)STR_OK_R, 8, id);
			RCR_SRSTG = 1;	// software - reset
			/* =============================================================== */			
			break;
		case CMD_READ:
			/* =============================================================== */			
			/*   READ FLASH MEMORY                                        +++++*/
			/* =============================================================== */			
			for(i=0; i<COUNT_MEM_PART; i++)
			{
				adr = ADDR_START_MEM[i];
				while(1)
				{
					st_send=0;
					adr_s = adr;
					for(j=0; j<0x10; j++)
					{
						Data[j] = read_byte2(adr++);	
						if(Data[j] != 0xFF) st_send=1;
					}
					if(st_send)
					{
						crc = 0;
						SendCanBuf_R(nCAN, (U8 *)"S214", 4, id);
						conv_hex_R(adr_s,6);
						SendCanBuf_R(nCAN, BufferCAN, 6, id);
						for(j=0; j<0x10; j++)
						{
							buf_8bit_hex_R(2*j, Data[j]);
							crc+=Data[j];
						}
						SendCanBuf_R(nCAN, BufferCAN, 0x10*2, id);
			
						crc+=0x14;
						crc+=adr_s&0xFF;
						crc+=(adr_s>>8)&0xFF;
						crc+=(adr_s>>16)&0xFF;
						crc = crc^0xFF;
			
						buf_8bit_hex_R(0, crc);
						SendCanBuf_R(nCAN, BufferCAN, 2, id);
						//------------------------
						// add timeout for MP3
						wait_mks_R(2000);
					}
					if(adr >= ADDR_END_MEM[i]) break;
				}
			}
			BufferCAN[0] = 'E';
			BufferCAN[1] = 'N';
			BufferCAN[2] = 'D';
			BufferCAN[3] = ' ';
			BufferCAN[4] = 'R';
			BufferCAN[5] = 'E';
			BufferCAN[6] = 'A';
			BufferCAN[7] = 'D';
			BufferCAN[8] = 0;
			
			SendCanBuf_R(nCAN, BufferCAN, 9, id);
			break;
		//--------------------------------------------------
		case CMD_WRITE:
			/* =============================================================== */
			/* Programming microcontroller                               ++++++*/
			/* =============================================================== */
			CountReadComm=0;
			BufferCAN[0] = 'R';
			BufferCAN[1] = 'E';
			BufferCAN[2] = 'A';
			BufferCAN[3] = 'D';
			BufferCAN[4] = 'Y';
			BufferCAN[5] = 0;
			
			SendCanBuf_R(nCAN, BufferCAN, 6, id);
			while(1)
			{
				clrwdt;					// обнуление WatchDog таймера 

				if(CanReciveMsg_R(nCAN, &msg) == TRUE)	
				{// приняли сообщение, обрабатываем
					if(msg.cob_id == id)// обрабатываем только наши пакеты, хотя они и так фильтруются на входе
					{
						for(i=0; i<msg.len; i++)
							BufferReadCom[CountReadComm++] = msg.data[i];
					}	
				}	
				if(CountReadComm == 44)
				{
					ret = write_MHX_com_R(BufferReadCom);
					if(ret<0)
					{
						SendCanBuf_R(nCAN, (U8 *)"ERORR ", 6, id);
						conv_dec_R(ret*(-1));
						SendCanBuf_R(nCAN, BufferCAN, 9, id);
						break;
					}
					CountReadComm = 0;
					if(ret == 0)	SendCanBuf_R(nCAN, (U8 *)STR_OK_B_R, 5, id);
					if(ret == 1)	SendCanBuf_R(nCAN, (U8 *)STR_OK_A_R, 5, id);
				}
				if(CountReadComm == 4)
				{
					if((BufferReadCom[0] == 'E')&&(BufferReadCom[1] == 'N')&&(BufferReadCom[2] == 'D')&&(BufferReadCom[3] == 0))
					{
						CountReadComm = 0;
						SendCanBuf_R(nCAN, (U8 *)STR_OK_R, 3, id);
						break;
					}
				}
			}
			/* =============================================================== */			
			break;
		//--------------------------------------------------
		// Unknown command.
		default:
			break;
	}
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
