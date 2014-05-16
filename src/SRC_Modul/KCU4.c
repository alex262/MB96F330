#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"
#include "uart.h"
#include "pakuart.h"
#include "fifo.h"
#include "i2c.h"

#ifdef PLATA_KCU4
#include "KCU4.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

CKCU Kcu;

void 	(*INIT_BLOK)(void)	=InitKCU;
void 	(*DRIVER_BLOK)(void)=DriverKCU;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD (*STATE_BLOCK) = &Kcu.Info.word;

//====================================================================
BYTE Convert_8(BYTE n);
void Read_FO(BYTE *pData);
void Read_FC(BYTE *pData);
//====================================================================

static TYPE_DATA_TIMER TimerWaitOpros	= 0;
//====================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m)
{
	return 0;
}
BYTE ServiceObmenData(BYTE bus_id, Message *m)
{
	return 0;
}
//====================================================================
void InitKCU()
{
	BYTE i;

	CP1 = 0;
	CP2 = 0;
	CP3 = 0;

	CC1 = 0;
	CC2 = 0;
	CC3 = 0;
	
	DDR00	= 0;
	DDR01	= 0;
	DDR03	= 0;
	DDR06	= 0;
	DDR07	= 0;
	DDR14	= 0;
	
	
	PIER00	= 0xFF;	
	PIER01	= 0xFF;	
	PIER03	= 0xFF;	
	PIER06	= 0xFF;	
	PIER07	= 0xFF;	
	PIER14	= 0xFF;	

	DDR09	= 0xFF;
	//------------------------------------
	Kcu.Info.word = 0;

	Kcu.SendPak	= 0;
	Kcu.Regim = 0;
	for(i=0;i<3;i++)
	{
		Kcu.FC[i]		= 0;
		Kcu.FO[i]		= 0;
		Kcu.St27V[i]	= 0;
	}
	
	Kcu.TmpData = 0;
	
	setTimer(&TimerWaitOpros, 1000);
	add_timer(&TimerWaitOpros);
}
//====================================================================
void DriverKCU()
{
	BYTE i;
	//------------------------------------------------
	// обновление адреса блока
	Kcu.Info.bits.Addr	=ADDR;
	//----------------------------------------------------
	// Функционал блока
	if(getTimer(&TimerWaitOpros) == 0)
	{
		if(Kcu.Regim == 0)	// включаем токи обтекания
		{
			//---------------------------------------
			// в предыущем режиме ничего не проверяли
			//---------------------------------------
			setTimer(&TimerWaitOpros, 50);
			Kcu.Regim = 1;
			CC1	= 1;
			CC2	= 1;
			CC3	= 1;
			CP1	= 0;
			CP2	= 0;
			CP3	= 0;
		}else
		{
			if(Kcu.Regim == 1)	// отключаем токи обтекания // вкл  проверку наличие 27В
			{
				//----------------------------------------------------------------
				// в предыущем режиме проверяли токи обтекания - зачитываем данные FC  
				Read_FC(Kcu.FC);
				//----------------------------------------------------------------
				CC1	= 0;
				CC2	= 0;
				CC3	= 0;
				CP1	= 1;
				CP2	= 1;
				CP3	= 1;
				
				setTimer(&TimerWaitOpros, 1000);
				Kcu.Regim = 0;
			}
		}
	}
	
	if((Kcu.Regim == 0)&&(getTimer(&TimerWaitOpros) < 995))
	{
		// Зачитываем наличие 27В
		Read_FC(Kcu.St27V);
	
		for(i=0;i<3;i++)
			Kcu.St27V[i]	= ~Kcu.St27V[i];
		
		// зачитываем защиту по току
		Read_FO(Kcu.FO);
		
		// только при наличии 27В анализируем защиту по току
	}
	//========================================================
	// обмен данными по COM
	if (GetRxByte(&i) == FIFO_OK)
	{
		BuffUart[CountDataUart] = i;
		if(CountDataUart < (BUFFER_LEN_UART-1))	
			CountDataUart++;
	} 
	
	GetPak_Uart(&CountDataUart, BuffUart);
	
	if(Kcu.SendPak == TRUE)
	{
		Kcu.SendPak	= FALSE;
		CreateAndSend_Pkt_UART0(Kcu.FC, 11, 2, 1);
	}
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	if(Id == 0x01)
	{	
		Kcu.SendPak = TRUE;
		return;
	}
	if(Id == 0x03)//Write
	{
		Kcu.TmpData = pData[0];
		return;
	}
}
//====================================================================
void Read_FO(BYTE *pData)
{
	pData[0] = 0;
	pData[1] = 0;
	pData[2] = 0;
	
	if(FO_1) SETBIT(pData[0],0);
	if(FO_2) SETBIT(pData[0],1);
	if(FO_3) SETBIT(pData[0],2);
	if(FO_4) SETBIT(pData[0],3);
	if(FO_5) SETBIT(pData[0],4);
	if(FO_6) SETBIT(pData[0],5);
	if(FO_7) SETBIT(pData[0],6);
	if(FO_8) SETBIT(pData[0],7);

	if(FO_9)	SETBIT(pData[1],0);
	if(FO_10)	SETBIT(pData[1],1);
	if(FO_11)	SETBIT(pData[1],2);
	if(FO_12)	SETBIT(pData[1],3);
	if(FO_13)	SETBIT(pData[1],4);
	if(FO_14)	SETBIT(pData[1],5);
	if(FO_15)	SETBIT(pData[1],6);
	if(FO_16)	SETBIT(pData[1],7);

	if(FO_17)	SETBIT(pData[2],0);
	if(FO_18)	SETBIT(pData[2],1);
	if(FO_19)	SETBIT(pData[2],2);
	if(FO_20)	SETBIT(pData[2],3);
	if(FO_21)	SETBIT(pData[2],4);
	if(FO_22)	SETBIT(pData[2],5);
	if(FO_23)	SETBIT(pData[2],6);
	if(FO_24)	SETBIT(pData[2],7);
}
void Read_FC(BYTE *pData)
{
	pData[0] = 0;
	pData[1] = 0;
	pData[2] = 0;
	
	if(FC_1) SETBIT(pData[0],0);
	if(FC_2) SETBIT(pData[0],1);
	if(FC_3) SETBIT(pData[0],2);
	if(FC_4) SETBIT(pData[0],3);
	if(FC_5) SETBIT(pData[0],4);
	if(FC_6) SETBIT(pData[0],5);
	if(FC_7) SETBIT(pData[0],6);
	if(FC_8) SETBIT(pData[0],7);

	if(FC_9)	SETBIT(pData[1],0);
	if(FC_10)	SETBIT(pData[1],1);
	if(FC_11)	SETBIT(pData[1],2);
	if(FC_12)	SETBIT(pData[1],3);
	if(FC_13)	SETBIT(pData[1],4);
	if(FC_14)	SETBIT(pData[1],5);
	if(FC_15)	SETBIT(pData[1],6);
	if(FC_16)	SETBIT(pData[1],7);

	if(FC_17)	SETBIT(pData[2],0);
	if(FC_18)	SETBIT(pData[2],1);
	if(FC_19)	SETBIT(pData[2],2);
	if(FC_20)	SETBIT(pData[2],3);
	if(FC_21)	SETBIT(pData[2],4);
	if(FC_22)	SETBIT(pData[2],5);
	if(FC_23)	SETBIT(pData[2],6);
	if(FC_24)	SETBIT(pData[2],7);
}
//====================================================================
BYTE Convert_8(BYTE n)
{
	BYTE i,s=0;
	for(i=0;i<8;i++)
	{
		s|=digit(n,i)<<(7-i);
	}
	return s;
}
//====================================================================

#endif;
