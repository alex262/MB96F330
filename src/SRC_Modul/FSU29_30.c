#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"
#include "uart.h"
#include "pakuart.h"
#include "fifo.h"
#include "i2c.h"
#include "spi.h"
#include "can.h"

#ifdef PLATA_FSU_29_30
#include "FSU29_30.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

CFSU Fsu;
//====================================================================
void	(*INIT_BLOK)(void)	=InitFSU;
void	(*DRIVER_BLOK)(void)=DriverFSU;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD	(*STATE_BLOCK)		= &Fsu.Info.word;
//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
//====================================================================
void ChipSelektAll(BYTE State);
void ChipSelekt(BYTE n, BYTE State);
//====================================================================
static TYPE_DATA_TIMER TimerSpi;
int obmen_spi(BYTE ch, BYTE Send);

static BYTE NumPak=0;
//====================================================================

void InitFSU()
{
	BYTE i;
	
	
	DDR00	= 0xFF;
	DDR01	= 0xE0;
	DDR02	= 0x0;
	DDR03	= 0x0;
	DDR06	= 0x0;
	DDR07	= 0x7;
	DDR09	= 0xFF;
	DDR14	= 0x0;
	DDR17	= 0xFF;
	
	PIER02	= 0xF0;	
	PIER03	= 0xFF;	
	PIER06	= 0xFC;	
	PIER14	= 0xFC;	
	//------------------------------------
	ChipSelektAll(CS_OFF);
	
	OUT1	=0;
	OUT2	=0;
	OUT3	=0;
	OUT4	=0;
	OUT5	=0;
	OUT6	=0;
	
	//------------------------------------
	
	for(i=0; i<3; i++)
	{
		Fsu.K[i]	=0;		
		Fsu.OL[i]	=0;		
		Fsu.Diag[i]	=0;		
		Fsu.DiagSum[i]	=0;		
		Fsu.SetData[i]	=0;
		Fsu.NewData[i]	=0;
	}

	Fsu.Info.word = 0;
	add_timer(&TimerStartBlock);
	add_timer(&TimerSpi);
	
	Fsu.stRecvPak	= FALSE;
	Fsu.SendPak		= FALSE;
	//------------------------------------
	InitSPI_1();
	InitSPI_2();
}
//====================================================================
void ChipSelektAll(BYTE State)
{
	CS11A = State;
	CS12A = State;
	CS13A = State;
	CS21A = State;
	CS22A = State;
	CS23A = State;
}
void ChipSelekt(BYTE n, BYTE State)
{
	if(n == 0)
		CS11A = State;
	if(n == 1)
		CS12A = State;
	if(n == 2)
		CS13A = State;
	if(n == 3)
		CS21A = State;
	if(n == 4)
		CS22A = State;
	if(n == 5)
		CS23A = State;
}
//-------------------------------------
void DriverFSU()
{
	BYTE i, j, k,  send, tmp;
	int data;
	//------------------------------------------------
	// обновление адреса блока
	Fsu.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	// если адрес изменился в течение первой секнды то переинициализируем блок
		{
			stStartBlock = TRUE;
			
			for(i=0; i<6; i++)
			{
				ChipSelekt(i, CS_ON);
				obmen_spi(i/3, 0xFF);
				ChipSelekt(i, CS_OFF);
			}
		}
	}
	//========================================================
	if(stStartBlock == TRUE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
		
			setTimer(&TimerStartBlock, 10);
		
			i = K_1 | (K_2<<1) | (K_3<<2) | (K_4<<3) | (K_5<<4) | (K_6<<5) | (K_7<<6) | (K_8<<7);
			Fsu.K[0] = ~i;
			
			i = K_9 | (K_10<<1)| (K_11<<2)| (K_12<<3)| (K_13<<4)| (K_14<<5)| (K_15<<6)| (K_16<<7);
			Fsu.K[1] = ~i;
			
			i = K_17| (K_18<<1)| (K_19<<2)| (K_20<<3)| (K_21<<4)| (K_22<<5)| (K_23<<6)| (K_24<<7);
			Fsu.K[2] = ~i;
			//-------------------------------------------------------------------------------------------
			for(i=0; i<3; i++)
			{
				Fsu.SetData[i] = Fsu.NewData[i];
				Fsu.OL[i]	= 0;
				Fsu.Diag[i]	= 0;
				Fsu.DiagSum[i]	=0;
						
				for(j=0; j<2; j++)
				{
					ChipSelekt(2*i+j, CS_ON);
					
					// 0x2 - ON  0x3 - OFF
					send = 0;
					tmp = Fsu.SetData[i]>>(4*j);
					
					for(k=0; k<4; k++)
					{
						if(digit(tmp,k) == 0)
							send|=0x3<<(2*k);
						else
							send|=0x2<<(2*k);		
					}
					
					data = obmen_spi((i+j)/2, send);
					
					if(data >= 0)
					{
						tmp = data;
						for(k=0; k<4; k++)
						{
							Fsu.OL[i]	|= digit(tmp,1+2*k)<<(4*j+k);
							Fsu.Diag[i]	|= digit(tmp,2*k)<<(4*j+k);
						}
						
						Fsu.DiagSum[i]	=Fsu.OL[i]|Fsu.Diag[i];
					}else
					{
						Fsu.DiagSum[i]	=0xFF;
					}
					ChipSelekt(2*i+j, CS_OFF);
				}
			}
		}
	}	
	//========================================================
	// обмен данными по COM
	/*if (GetRxByte(&i) == FIFO_OK)
	{
		BuffUart[CountDataUart] = i;
		if(CountDataUart < BUFFER_LEN_UART-1)	
			CountDataUart++;
	}
	
	GetPak_Uart(&CountDataUart, BuffUart);
	
	if(Fsu.SendPak == TRUE)
	{
		Fsu.SendPak	= FALSE;
		CreateAndSend_Pkt_UART0(&Fsu.K[0], 15, NumPak++, 1);
	}*/
	//--------------------------------------------------
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	if(Id == 0x01)
	{	
		Fsu.SendPak = TRUE;
		return;
	}
	if(Id == 0x03)//Write
	{
		Fsu.NewData[0] = pData[1];
		Fsu.NewData[1] = pData[2];
		Fsu.NewData[2] = pData[3];
		return;
	}
}
//====================================================================



int obmen_spi(BYTE ch, BYTE Send)
{
	BYTE  din;

	if(ch == 0)
	{
		TDR9 = Send;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return (-1);
		}
		din = RDR9;
		return din;
	}else
	{
		TDR3 = Send;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return (-1);
		}
		din = RDR3;
		return din;
	}
}



#endif;
