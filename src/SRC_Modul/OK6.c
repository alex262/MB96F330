#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"
#include "uart.h"
#include "pakuart.h"
#include "fifo.h"

#ifdef PLATA_OK6
#include "OK6.h"
//====================================================================

COK6 OK6;
//====================================================================

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

//====================================================================
void	(*INIT_BLOK)(void)	=InitOK6;
void	(*DRIVER_BLOK)(void)=DriverOK6;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD	(*STATE_BLOCK)		= &OK6.Info.word;
//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================

static BYTE NumChADC[4] = {15, 21, 13, 24};
			
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
//====================================================================
//====================================================================
//static BYTE NumPak=0;
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m)
{
	bus_id;
	m;
	return 0;
}
BYTE ServiceObmenData1(BYTE bus_id, Message *m)
{
	bus_id;
	m;
	return 0;
}
BYTE ServiceObmenData2(BYTE bus_id, Message *m)
{
	bus_id;
	m;
	return 0;
}
//====================================================================
void InitOK6()
{
//	BYTE i;

	DDR00	= 0xFF;
	DDR01	= 0x00;
	DDR02	= 0x0;
	DDR03	= 0x0;
	DDR05	= 0x0;
	DDR06	= 0xFF;
	DDR07	= 0x0;
	DDR09	= 0xFF;
	DDR13	= 0x1;	//CNC
	DDR14	= 0x0;
	DDR17	= 0xFF;
	
	PIER01	= 0xE1;	
	PIER05	= PIER05|4;	
	//------------------------------------
	PDR13	= 0;	// CNC
	
	PRRR10_PPG8_R = 1;
	PRRR10_PPG9_R = 1;
	
	PCN2_RTRG	= 1;	// 1 tic 20.83 ns
	PCN2_MDSE	= 1; 	// one shot
	PCN2_PGMS	= 0;
	PCN2_OE		= 1;
	PCN2_OSEL	= 0;
	PCN2_EGS	= 1;
	PCSR2		= 72;	// 2 мкс
	PDUT2		= 72;	// 2 мкс
	PCN2_CNTE	= 0;
		
	PCN3_RTRG	= 1;	// 1 tic 20.83 ns
	PCN3_MDSE	= 1; 	// one shot
	PCN3_PGMS	= 0;
	PCN3_OE		= 1;
	PCN3_OSEL	= 0;
	PCN3_EGS	= 1;
	PCSR3		= 72;	// 9 мкс
	PDUT3		= 72;	// 7 мкс
	PCN3_CNTE	= 0;
	//---------------------
	PCN8_RTRG	= 1;	// 1 tic 20.83 ns
	PCN8_MDSE	= 1; 	// one shot
	PCN8_PGMS	= 0;
	PCN8_OE		= 1;
	PCN8_OSEL	= 0;
	PCN8_EGS	= 1;
	PCSR8		= 72; 
	PDUT8		= 72; 
	PCN8_CNTE	= 0;
	
	PCN9_RTRG	= 1;	// 1 tic 20.83 ns
	PCN9_MDSE	= 1;	// one shot
	PCN9_PGMS	= 0;
	PCN9_OE		= 1;
	PCN9_OSEL	= 0;
	PCN9_EGS	= 1;
	PCSR9		= 72; 
	PDUT9		= 72; 
	PCN9_CNTE	= 0;
	
	GCN12	= 0x5555;	// Trigger Source: Reload Timer 1
	GCN10	= 0x5555;	// Trigger Source: Reload Timer 1
	TMRLR1	= 60;		// set reload value
	TMCSR1	= 0x1053;	// prescaler 1:1, no interrupts, output enable 83,3 ns

	PCN2_CNTE	= 1;
	PCN3_CNTE	= 1;
	PCN8_CNTE	= 1;
	PCN9_CNTE	= 1;
	
	ADCS = 0x8000;   // stop mode, 10 bit
	ADSR = 0x9000;   // 24 cy. sampling; 88 cy. conversion; channel 0
	ADER0 = 0x00;
	ADER1 = 0xA0;
	ADER2 = 0x20;
	ADER3 = 0x01;
	
	
	OK6.Info.word = 0;
	add_timer(&TimerStartBlock);
	
	OK6.stRecvPak	= FALSE;
	OK6.SendPak		= FALSE;
}
//====================================================================
void SelectChAdc(BYTE ch)
{
	WORD tmp, reg;
	if(ch>32)return;
	
	tmp = ch;
	tmp = tmp<<5;
	tmp |= ch;
	
	reg = ADSR;
	reg &= 0xFC00;  
	reg |= tmp;
	ADSR = reg;  
}
//-------------------------------------
WORD ReadAdcCh(BYTE ch)
{
	WORD tmp;
	if(ch>32) return 0;
	
	SelectChAdc(ch);
	ADCS_STRT = 1;
	while(ADCS_BUSY == 1) // конвертирование ещё не завершено
	{
		ADCS_INT = 0;
	}
	
	
	tmp = ADCR;
	return tmp;
}
//=============================================================
WORD MassADC[4][20];
BYTE posMas[4] = {0,0,0,0};

BYTE LenDel=0;
void DriverOK6()
{
	BYTE i, j;
//	BYTE buf[80];
//	int data;
	DWORD dwData;
	//------------------------------------------------
	// обновление адреса блока
	OK6.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			stStartBlock = TRUE;
			//puts((BYTE *)"\n\rAIN1 AIN2 AIN2 AIN4 | AIN1 AIN2 AIN2 AIN4\n\r");
			
		}
	}
	OK6.sDK		=DK;
	
	if(OK6.sDK == 1) SetError(ind_BREST);
	else ClearError(ind_BREST);
		
	OK6.sDK1A	=(~DK1A)&1;
	OK6.sDK2A	=(~DK2A)&1;
	OK6.sDK3A	=(~DK3A)&1;
	OK6.sPG24V	=PG24V;

	//========================================================
	if(stStartBlock == TRUE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			setTimer(&TimerStartBlock, 10);
			
			for(i=0; i<4; i++)
			{
				MassADC[i][posMas[i]] =  ReadAdcCh(NumChADC[i]);
				posMas[i]++;
				if(posMas[i] == 20) posMas[i]=0;
			}
			for(i=0; i<4; i++)
			{
				dwData = 0;
				for(j=0; j<20; j++)
				{
					dwData+=MassADC[i][j];
				}
				dwData = dwData/20;
				OK6.AIN[i] =  dwData;
				OK6.fAIN[i]= (float)OK6.AIN[i];
				OK6.fAIN[i]= OK6.fAIN[i]*0.02597-2.88312;
			}
			
//			for(i=0; i<LenDel; i++)
//			{
//				puts((BYTE *)"\010");
//			}
//			LenDel = 0;
//			sprintf(buf, "%04d %04d %04d %04d | ",OK6.AIN[0]  ,OK6.AIN[1] ,OK6.AIN[2] ,OK6.AIN[2]);
//			LenDel = strlen(buf);
//			puts(buf);
//			sprintf(buf, "%1.2f %1.2f %1.2f %1.2f" ,OK6.fAIN[0] ,OK6.fAIN[1],OK6.fAIN[2],OK6.fAIN[2]);
//			LenDel += strlen(buf);
//			puts(buf);
		}
	}	
	//========================================================
	// обмен данными по COM
	#ifndef TERMINAL_EN
		if (GetRxByte(&i) == FIFO_OK)
		{
			BuffUart[CountDataUart] = i;
			if(CountDataUart < BUFFER_LEN_UART-1)	
				CountDataUart++;
		}
		
		GetPak_Uart(&CountDataUart, BuffUart);
		
		if(OK6.SendPak == TRUE)
		{
			OK6.SendPak	= FALSE;
			//CreateAndSend_Pkt_UART0(&Fsu.K[0], 15, NumPak++, 1);
		}
	#endif
	//--------------------------------------------------
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	pData;
	Len;
	if(Id == 0x01)
	{	
	//	OK6.SendPak = TRUE;
		return;
	}
}
//====================================================================

#endif;
