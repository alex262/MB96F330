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

#ifdef PLATA_NDD22
#include "NDD22.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

CNDD Ndd;

void 	(*INIT_BLOK)(void)	=InitNDD;
void 	(*DRIVER_BLOK)(void)=DriverNDD;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;


WORD (*STATE_BLOCK) = &Ndd.Info.word;

//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
static TYPE_DATA_TIMER TimerSpi;
//====================================================================
static BYTE RET_DATA_SPI[10];
BYTE write_spi_reg(BYTE ch, BYTE Command, WORD Data, BYTE * ret, BYTE Test);
//void write_sec(BYTE ch, BYTE Len, BYTE * ret, BYTE * send);
BYTE ReadAdc(BYTE ch);
void GoWorkAdc(BYTE ch, BYTE st);
//====================================================================

void InitNDD()
{
	BYTE i, j;
	
	
	DDR00	= 0x10;
	DDR01	= 0xFD;
	DDR07	= 0;
	DDR14	= 0x87;
	DDR09	= 0xFF;
	DDR15_D0= 0;
	DDR07_D0= 0;
	
	PILR15_IL0	= 1;
	EPILR15_EIL0= 0;
	PUCR15_PU0	= 1;
	
	PILR07_IL0	= 1;
	EPILR07_EIL0= 0;
	PUCR07_PU0	= 1;
	
	PILR00_IL0	= 1;
	EPILR00_EIL0= 0;
	PUCR00_PU0	= 1;
	
	
	PIER00	= 0xFF;	
	PIER01	= 0xFF;	
	PIER07	= 0xFF;	
	PIER14	= 0xFF;	
	PIER15_IE0 = 1;
	//------------------------------------
	CS11	= CS_OFF;
	CS12	= CS_OFF;
	CS13	= CS_OFF;
	CS21	= CS_OFF;
	CS22	= CS_OFF;
	CS23	= CS_OFF;
	//------------------------------------
	for(i=0; i<4; i++)
	{
		Ndd.Din[i] = 0;
		Ndd.test[i] = NOT_VALID_DIN;
		Ndd.Init[i] = FALSE;		
		for(j=0; j<3; j++)
			Ndd.State[i][j] = 0;
	}
	Ndd.CurrentAmuxSet	= 0;
	Ndd.CurrentSelect	= 2;  // ����� ��� ���� 0 - high-impedance; 1 - 2mA; 2 - 16mA
	Ndd.CurrentPart		= 0;
	//------------------------------------
	Ndd.Info.word = 0;
	add_timer(&TimerStartBlock);

	Ndd.SendPak		= FALSE;
	Ndd.stWrReg		= FALSE;
	
	//-------------------------
	add_timer(&TimerSpi);
	//------------------------------------
	InitSPI_1();
	InitSPI_2();
//	InitFreeTimer16_0();
//	InitOCU_01(11, 25) ;
}
//====================================================================
//-------------------------------------
void ChipSelektDIN(BYTE n, BYTE State)
{
	if(n == 0)
		CS11 = State;
	if(n == 1)
		CS12 = State;
	if(n == 2)
		CS21 = State;
	if(n == 3)
		CS22 = State;
}
void ChipSelektADC(BYTE n, BYTE State)
{
	if(n == 0)
		CS13 = State;
	if(n == 1)
		CS23 = State;
}
//-------------------------------------
void DriverNDD()
{
	BYTE i, tmp[2];
	WORD d;	
	//------------------------------------------------
	// ���������� ������ �����
	Ndd.Info.bits.Addr	=ADDR;
			
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	// ���� ����� ��������� � ������� ������ ������ �� ������������������ ����
		{
			//del_timer(&TimerStartBlock);
			stStartBlock = TRUE;
			//----------------------------------
			// ������������� ��������� DIN
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 1, 0, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 4, 0xFFFF, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 5, 0xFFFF, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 7, 0xFFFF, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 8, 0xFFFF, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 9, 0xFFFF, RET_DATA_SPI, FALSE);	// �������� ������ ����������� � ������ ��������� ��� ��������� ���� 16��
				ChipSelektDIN(i, CS_OFF);
			}
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				write_spi_reg(i, 10, 0, RET_DATA_SPI, FALSE);
				ChipSelektDIN(i, CS_OFF);
			}
		}
	}
	//========================================================
	if(stStartBlock == TRUE)
	{
		if(getTimer(&TimerStartBlock) == 0)
		{
			setTimer(&TimerStartBlock,20);
			//--------------------------------------
			for(i=0; i<4; i++)
			{
				if((Ndd.Init[i] == TRUE)&&(Ndd.test[i] == VALID_DIN))
				{
					Ndd.Init[i] = FALSE;
					
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 1, 0, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 4, 0xFFFF, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 5, 0xFFFF, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 7, 0xFFFF, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 8, 0xFFFF, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 9, 0xFFFF, RET_DATA_SPI, FALSE);	// �������� ������ ����������� � ������ ��������� ��� ��������� ���� 16��
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, 10, 0, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
				}
			}
			//----------------------------------------		
			if(Ndd.stWrReg	== TRUE)
			{
				Ndd.stWrReg = FALSE;
				
				d = (Ndd.Data[1]<<8)|(Ndd.Data[0]);
				for(i=0; i<4; i++)
				{
					msDelay(1);
					ChipSelektDIN(i, CS_ON);
					write_spi_reg(i, Ndd.NumReg, d, RET_DATA_SPI, FALSE);
					ChipSelektDIN(i, CS_OFF);
					msDelay(1);
				}
			}
			//-------------------------------------------
			for(i=0; i<4; i++)
			{
				ChipSelektDIN(i, CS_ON);
				if(write_spi_reg(i, 0, 0, RET_DATA_SPI, TRUE) == FALSE)	// ��� �����
				{
					Ndd.test[i] = NOT_VALID_DIN;
					Ndd.Init[i] = TRUE;
				}
				else
					Ndd.test[i] = VALID_DIN;
				
				ChipSelektDIN(i, CS_OFF);
				
				Ndd.State[i][0] = RET_DATA_SPI[0];
				Ndd.State[i][1] = RET_DATA_SPI[1];
				Ndd.State[i][2] = RET_DATA_SPI[2];
			}
			for(i=0; i<4; i++)
			{
				tmp[0]= 0;
				tmp[1]= 0;
				
				Ndd.Term[i] = digit(Ndd.State[i][0],7);
				if(Ndd.Term[i] != 0) Ndd.test[i] = NOT_VALID_DIN;
				
				tmp[0]= digit(Ndd.State[i][0],2) | (digit(Ndd.State[i][0],3)<<1)|(digit(Ndd.State[i][0],4)<<2)|(digit(Ndd.State[i][0],5)<<3)
						 |(digit(Ndd.State[i][1],6)<<4)|(digit(Ndd.State[i][1],7)<<5)|(digit(Ndd.State[i][0],0)<<6)|(digit(Ndd.State[i][0],1)<<7);

				tmp[1]= digit(Ndd.State[i][2],7) | (digit(Ndd.State[i][1],0)<<1)|(digit(Ndd.State[i][1],1)<<2)|(digit(Ndd.State[i][1],2)<<3)
						 |(digit(Ndd.State[i][2],0)<<4)|(digit(Ndd.State[i][2],1)<<5)|(digit(Ndd.State[i][2],2)<<6)|(digit(Ndd.State[i][2],3)<<7);

				Ndd.Din[i] = tmp[0]&tmp[1];
			}
				
		/*		GoWorkAdc(0, TRUE);
				ChipSelektADC(0, CS_ON);
				Ndd.ADC[0][0] = ReadAdc(0);	
				ChipSelektADC(0, CS_OFF);
				GoWorkAdc(0, FALSE);*/
		//}
			//--------------------------------------------------------
			// ����� ���������� ������
/*			if(Ndd.CurrentAmuxSet == 0)	// �� �������
			{
				Ndd.CurrentAmuxSet	= 1;
				Ndd.CurrentPart		= 0;
				//Ndd.CurrentSelect
				d = Ndd.CurrentAmuxSet|(Ndd.CurrentSelect<<5);
				for(i=0; i<2; i++)
				{
					ChipSelektDIN(i*2, CS_ON);
					write_spi_reg(i*2, 6, d, RET_DATA_SPI);
					ChipSelektDIN(i*2, CS_OFF);
				}
			}else
			{
				// ������ ���
				
				if(Ndd.CurrentPart == 0)
				{
					for(i=0; i<2; i++)
					{
						GoWorkAdc(i, TRUE);
						ChipSelektADC(i, CS_ON);
						ReadAdc(i);
						ChipSelektADC(i, CS_OFF);
						ChipSelektADC(i, CS_ON);
						Ndd.ADC[2*i][Ndd.CurrentAmuxSet-1] = ReadAdc(i);
						ChipSelektADC(i, CS_OFF);
						GoWorkAdc(i, FALSE);
					}
					Ndd.CurrentAmuxSet++;
					if(Ndd.CurrentAmuxSet>=23)
					{
						Ndd.CurrentAmuxSet	= 1;
						Ndd.CurrentPart		= 1;
						d = Ndd.CurrentAmuxSet|(Ndd.CurrentSelect<<5);
						
						for(i=0; i<2; i++)
						{
							ChipSelektDIN(i*2+1, CS_ON);
							write_spi_reg(i*2+1, 6, d, RET_DATA_SPI);
							ChipSelektDIN(i*2+1, CS_OFF);
						}
					}else
					{
						d = Ndd.CurrentAmuxSet|(Ndd.CurrentSelect<<5);
						for(i=0; i<2; i++)
						{
							ChipSelektDIN(i*2, CS_ON);
							write_spi_reg(i*2, 6, d, RET_DATA_SPI);
							ChipSelektDIN(i*2, CS_OFF);
						}
					}
				}else
				{
					for(i=0; i<2; i++)
					{
						GoWorkAdc(i, TRUE);
						ChipSelektADC(i, CS_ON);
						ReadAdc(i);
						ChipSelektADC(i, CS_OFF);
						ChipSelektADC(i, CS_ON);
						Ndd.ADC[2*i+1][Ndd.CurrentAmuxSet-1] = ReadAdc(i);	
						ChipSelektADC(i, CS_OFF);
						GoWorkAdc(i, FALSE);
					}
					
					Ndd.CurrentAmuxSet++;
					if(Ndd.CurrentAmuxSet>=23)
					{
						Ndd.CurrentAmuxSet	= 1;
						Ndd.CurrentPart		= 0;
						d = Ndd.CurrentAmuxSet|(Ndd.CurrentSelect<<5);
						
						for(i=0; i<2; i++)
						{
							ChipSelektDIN(i*2, CS_ON);
							write_spi_reg(i*2, 6, d, RET_DATA_SPI);
							ChipSelektDIN(i*2, CS_OFF);
						}
					}else
					{
						d = Ndd.CurrentAmuxSet|(Ndd.CurrentSelect<<5);
						for(i=0; i<2; i++)
						{
							ChipSelektDIN(i*2+1, CS_ON);
							write_spi_reg(i*2+1, 6, d, RET_DATA_SPI);
							ChipSelektDIN(i*2+1, CS_OFF);
						}
					}
				}
			}*/
		}
	}
	//========================================================
	Ndd.Info.bits.Flt1 = program.stFLT1;
	Ndd.Info.bits.Flt2 = program.stFLT2;
	//========================================================
	// ����� ������� �� COM
	if (GetRxByte(&i) == FIFO_OK)
	{
		BuffUart[CountDataUart] = i;
		if(CountDataUart < BUFFER_LEN_UART-1)	
			CountDataUart++;
	}
	
	GetPak_Uart(&CountDataUart, BuffUart);
	
	if(Ndd.SendPak == TRUE)
	{
		Ndd.SendPak	= 0;
		CreateAndSend_Pkt_UART0(&Ndd.State[0][0], 24+88, 2, 1);
	}
	//----------------------------------------------------
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	if(Id == 0x01)
	{	
		Ndd.SendPak = TRUE;
		return;
	}
	if(Id == 0x03)//Write
	{
		Ndd.stWrReg = TRUE;
		Ndd.NumReg	= pData[1];
		Ndd.Data[1]	= pData[2];
		Ndd.Data[0]	= pData[3];

		return;
	}
}

//====================================================================
/*
void write_sec(BYTE ch, BYTE Len, BYTE * ret, BYTE * send)
{
	BYTE i;
	if(ch<2)
	{
		for(i=0; i<Len; i++)
		{
			TDR9 = send[i];
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			ret[i] = RDR9;
		}
	}else
	{
		for(i=0; i<Len; i++)
		{
			TDR3 = send[i];
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			ret[i] = RDR3;
		}
	}
}
*/
static BYTE RegSPI1,RegSPI2;

void GoWorkAdc(BYTE ch, BYTE st)
{
	if(ch == 0)
	{
		if(st == TRUE)
		{
			RegSPI1 = ESCR9;
			ESCR9 = 0;
		}else
		{
			ESCR9 = RegSPI1;
		}
	}else
	{
		if(st == TRUE)
		{
			RegSPI2 = ESCR3;
			ESCR3 = 0;
		}else
		{
			ESCR3 = RegSPI2;
		}
	}
}

BYTE ReadAdc(BYTE ch)
{
	WORD ret;
	BYTE reg;
	if(ch == 0)
	{
		TDR9 = 0;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return 0;
		}
		ret = RDR9<<8;

		TDR9 = 0;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return 0;
		}
		ret |= RDR9;
		reg = ret>>5;
		return reg;
	}else
	{
		TDR3 = 0;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return 0;
		}
		ret = RDR3<<8;

		TDR3 = 0;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return 0;
		}
		ret |= RDR3;
	
		reg = ret>>5;
		return reg;
	}
}
//---------------------------------------------------------------
static BYTE TestSeqDin[3] = {0xA5,0x5A,0x81};
static BYTE SendSeqDin[3] = {0,0,0};

BYTE write_spi_reg(BYTE ch, BYTE Command, WORD Data, BYTE * ret, BYTE Test)
{
	BYTE i, d;
	
	SendSeqDin[0] = Command;
	if(ch<2)
	{
		for(i=0; i<3; i++)
		{
			while (SSR9_TDRE == 0);		
			TDR9 = SendSeqDin[i];
		
			while (SSR9_RDRF == 0);
			ret[i] = RDR9;
		}
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine
		//--------------------------------------------------------
		if(Test == TRUE)
		{
			for(i=0; i<3; i++)
			{
				while (SSR9_TDRE == 0);		
				TDR9 = TestSeqDin[i];
				while (SSR9_RDRF == 0);		
				d = RDR9;
			}
			for(i=0; i<3; i++)
			{
				while (SSR9_TDRE == 0);		
				TDR9 = 0;
				while (SSR9_RDRF == 0);
				
				if(TestSeqDin[i] != RDR9)
					return FALSE;
			}
		}
	}else
	{
		for(i=0; i<3; i++)
		{
			while (SSR3_TDRE == 0);		
			TDR3 = SendSeqDin[i];
		
			while (SSR3_RDRF == 0);
			ret[i] = RDR3;
		}
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
		//--------------------------------------------------------
		if(Test == TRUE)
		{
			for(i=0; i<3; i++)
			{
				while (SSR3_TDRE == 0);		
				TDR3 = TestSeqDin[i];
				while (SSR3_RDRF == 0);		
				d = RDR3;
			}
			for(i=0; i<3; i++)
			{
				while (SSR3_TDRE == 0);		
				TDR3 = 0;
				while (SSR3_RDRF == 0);
				
				if(TestSeqDin[i] != RDR9)
					return FALSE;
			}
		}
	}
	return TRUE;
}

/*BYTE write_spi_reg(BYTE ch, BYTE Command, WORD Data, BYTE * ret, BYTE Test)
{
	BYTE i, d;
	if(ch<2)
	{
		TDR9 = Command;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		ret[0] = RDR9;

		TDR9 = Data>>8;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		ret[1] = RDR9;
		
		TDR9 = (BYTE)Data;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		ret[2] = RDR9;
		
		//--------------------------------------------------------
		if(Test == TRUE)
		{
			for(i=0; i<3; i++)
			{
				TDR9 = TestSeqDin[i];
				setTimer(&TimerSpi, 3);
				while (SSR9_RDRF == 0)		
				{
					if(getTimer(&TimerSpi) == 0) return FALSE;
				}
				d = RDR9;
			}
			for(i=0; i<3; i++)
			{
				TDR9 = 0;
				setTimer(&TimerSpi, 3);
				while (SSR9_RDRF == 0)		
				{
					if(getTimer(&TimerSpi) == 0) return FALSE;
				}
				
				if(TestSeqDin[i] != RDR9)
					return FALSE;
			}
		}
	}else
	{
		TDR3 = Command;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return  FALSE;
		}
		ret[0] = RDR3;

		TDR3 = Data>>8;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		ret[1] = RDR3;
		
		TDR3 = (BYTE)Data;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		ret[2] = RDR3;
		//--------------------------------------------------------
		if(Test == TRUE)
		{
			for(i=0; i<3; i++)
			{
				TDR3 = TestSeqDin[i];
				setTimer(&TimerSpi, 3);
				while (SSR3_RDRF == 0)		
				{
					if(getTimer(&TimerSpi) == 0) return FALSE;
				}
				d = RDR3;
			}
			for(i=0; i<3; i++)
			{
				TDR3 = 0;
				setTimer(&TimerSpi, 3);
				while (SSR3_RDRF == 0)		
				{
					if(getTimer(&TimerSpi) == 0) return FALSE;
				}
				
				if(TestSeqDin[i] != RDR3)
					return FALSE;
			}
		}
	}
	return TRUE;
}*/

#endif;
