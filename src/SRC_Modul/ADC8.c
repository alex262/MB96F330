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

#ifdef PLATA_ADC8
#include "ADC8.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

CADC8 Adc8;

void 	(*INIT_BLOK)(void)	=InitADC8;
void 	(*DRIVER_BLOK)(void)=DriverADC8;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD (*STATE_BLOCK) = &Adc8.Info.word;

//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
static TYPE_DATA_TIMER TimerSpi;

TTar TarEEPROM[ADC_CH];	// тарировки каналов в EEPROM
TTar TarRam[ADC_CH];	// тарировки каналов в ОЗУ

const BYTE RegDefADC[12] = {0x01, 0x60, 0x40, 0x00, 0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10};
// сопосталение номеров каналов 
						//	0  1  2  3  4   5    6  7    8   9  10  11 12 13  14  15 |+16   17
const BYTE ChipToCh[32] =  {0, 2, 4, 6, 8,  10, 12, 14, 15, 13, 11, 9, 7,  5,  3,  1,   16, 18, 20, 22, 24,  26, 28, 30, 31, 29, 27, 25, 23,  21,  19,  17};
//====================================================================
BYTE read_spi_rdata(BYTE ch, DWORD *status, WORD * pDataADC);
void write_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData);
void read_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData);
void StopContMode(BYTE ch);
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
void InitADC8()
{
	BYTE i, j;
	
	DDR00	= 0x10;
	DDR01	= 0x7D;
	DDR07	= 0;
	DDR14	= 0x83;
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
	RES1A	= 1;
	RES2A	= 1;

	SRTA	= 0;
	CS11	= CS_OFF;
	CS12	= CS_OFF;
	CS21	= CS_OFF;
	CS22	= CS_OFF;
	
	//------------------------------------
	Adc8.WriteTar = 0;
	for(i=0; i<ADC_CHIP_COUNT; i++)
	{
		for(j=0; j<ADC_CH_ON_CHIP; j++)
		{
			Adc8.adc_mux_set[i][j] = MUX_INPUT;
			Adc8.adc_mux_new[i][j] = MUX_INPUT;
		}
	}
	//--------------------------------------------------------
	for(i=0; i<ADC_CHIP_COUNT; i++)
	{
		for(j=0; j<12; j++)
		{
			Adc8.reg_adc_set[i][j] = RegDefADC[j];
			Adc8.reg_adc_new[i][j] = RegDefADC[j];
		}
	}
	//------------------------------------
	Adc8.Info.word = 0;
	add_timer(&TimerStartBlock);

	Adc8.SendPak	= FALSE;
	Adc8.stRecvPak	= FALSE;
	Adc8.stWR_Reg	= FALSE;
	//----------------------------------------
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
	PCN11_MDSE	= 1; 	// one shot
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
	//-------------------------
	add_timer(&TimerSpi);
	//------------------------------------
	InitSPI_1();
	InitSPI_2();
	InitFreeTimer16_0();
	InitOCU_01(11, 25) ;
}
//====================================================================
static WORD	w_adc_data[ADC_CH_ON_CHIP];

S16 ConvertCodToInt(WORD Cod)
{
	S16 data;
	
	if(Cod&0x8000)
	{
		Cod = (~Cod)&0x7FFF;
		data = (S16)Cod;
		data *= (-1);
		return data;
	}else
	{
		data = (S16)Cod;
		return data;
	}
}
//-------------------------------------
void ChipSelekt(BYTE nADC, BYTE State)
{
	if(nADC == 0)
		CS11 = State;
	if(nADC == 1)
		CS12 = State;
	if(nADC == 2)
		CS21 = State;
	if(nADC == 3)
		CS22 = State;
}
//-------------------------------------
void DriverADC8()
{
	BYTE j,i, READY_ADC[ADC_CHIP_COUNT];
	BYTE *pData, st;
	float input;
	TTar *pTar;
	
	BYTE NumCh;
	//------------------------------------------------
	// обновление адреса блока
	Adc8.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			del_timer(&TimerStartBlock);
			stStartBlock = TRUE;
			//========================================================
			// Читаем тарировки из EEPROM в RAM
			SetWorkChI2C(1);
			HighDensSequentialRead(0, (BYTE*)(&TarRam), 256);
			pData = (BYTE*)(&TarRam);
			st =0;
			
			for(i=0; i<ADC_CH; i++)
			{
				clrwdt;
				Adc8.f_adc_data[i] = 0;
				if((check_NaN_Inf(TarRam[i].k)) == TRUE)
				{
					TarRam[i].k = 1.0;
					st = 1;
				}
				if(check_NaN_Inf(TarRam[i].ofs) == TRUE)
				{
					TarRam[i].ofs = 0.0;
					st = 1;
				}
				TarEEPROM[i].k	= TarRam[i].k;
				TarEEPROM[i].ofs= TarRam[i].ofs;
			}
			if(st == 1)
			{
				//puts("Write trarrirovki!!!");
				HighDensPageWrite(0, (BYTE*)(&TarRam), 256);
			}
			//------------------------------------
			msDelay(100);
			RES1A	= 0;
			RES2A	= 0;
			msDelay(5);
			RES1A	= 1;
			RES2A	= 1;
			msDelay(5);
			
			for(j=0;j<ADC_CHIP_COUNT;j++)	// прописываем значение регистров по умолчанию
			{
				ChipSelekt(j, CS_ON);
				write_spi_reg(j, 1, 12,(BYTE *)(&RegDefADC[0]));
				ChipSelekt(j, CS_OFF);
				msDelay(1);
				ChipSelekt(j, CS_ON);
				StopContMode(j);
				ChipSelekt(j, CS_OFF);
			}
			msDelay(2);
			SRTA	= 1;	// запуск конвертации всех АЦП
			//========================================================
		}
	}
	//========================================================
	if(stStartBlock == TRUE)
	{
		if(Adc8.WriteTar == 1)
		{
			st = 0;
			for(i=0; i<ADC_CH; i++)
			{
				if(TarEEPROM[i].k	!= TarRam[i].k)
				{
					st = 1;
					TarEEPROM[i].k	= TarRam[i].k;
				}
				if(TarEEPROM[i].ofs	!= TarRam[i].ofs)
				{
					st = 1;
					TarEEPROM[i].ofs = TarRam[i].ofs;
				}
			}
			if(st == 1)
			{
				SetWorkChI2C(1);
				HighDensPageWrite(0, (BYTE*)(&TarEEPROM), 256);
			}
		}

		//----------------------------------------------------------
		// запись регистров через COM 					++++++++++++
		//----------------------------------------------------------
		if(Adc8.stWR_Reg == TRUE)
		{
			Adc8.stWR_Reg = FALSE;
			if((Adc8.NumReg>4)&&(Adc8.NumReg<13))
			{
				for(i=0;i<ADC_CHIP_COUNT;i++)
				{
					j = Adc8.NumReg-5;
					Adc8.adc_mux_set[i][j] = Adc8.ValReg&7;
					Adc8.adc_mux_new[i][j] = Adc8.adc_mux_set[i][j];
				}
			}
			for(j=0;j<ADC_CHIP_COUNT;j++)
			{
				ChipSelekt(j, CS_ON);
				write_spi_reg(j, Adc8.NumReg, 1, &Adc8.ValReg);
				ChipSelekt(j, CS_OFF);
			}
		}
		//-------------------------------------------------------------
		for(i=0; i<ADC_CHIP_COUNT; i++) // для записи по CAN +++++++++
		{
			for(j=0;j<13;j++)
			{
				if(Adc8.reg_adc_set[i][j] != Adc8.reg_adc_new[i][j])
				{
					Adc8.reg_adc_set[i][j] = Adc8.reg_adc_new[i][j];
					st = Adc8.reg_adc_set[i][j];
					
					if((j>3)&&(j<12))
					{
						Adc8.adc_mux_set[i][j-4] = st&7;
						Adc8.adc_mux_new[i][j-4] = st&7;
					}
					

					ChipSelekt(i, CS_ON);
					if(i<12) write_spi_reg(i, j+1, 1, &st);
					else write_spi_reg(i, 0x14, 1, &st);
					ChipSelekt(i, CS_OFF);
				}
			}
		}
		//----------------------------------------------------------------
		for(i=0;i<ADC_CHIP_COUNT;i++) // для записи по CAN
		{
			for(j=0;j<ADC_CH_ON_CHIP;j++)
			{
				if(Adc8.adc_mux_set[i][j] != Adc8.adc_mux_new[i][j])
				{
					Adc8.adc_mux_set[i][j] = Adc8.adc_mux_new[i][j];

					st = Adc8.reg_adc_set[i][4+j]&0xF8;
					st |= Adc8.adc_mux_set[i][j]&7;
					Adc8.reg_adc_set[i][4+j] = st;
					Adc8.reg_adc_new[i][4+j] = st;
					
					ChipSelekt(i, CS_ON);
					write_spi_reg(i, 5+j, 1, &st);
					ChipSelekt(i, CS_OFF);
				}
			}
		}
		//-------------------------------------------------------------
		for(j=0;j<ADC_CHIP_COUNT;j++)
		{
			READY_ADC[0] = DRDY11;
			READY_ADC[1] = DRDY12;
			READY_ADC[2] = DRDY21;
			READY_ADC[3] = DRDY22;
			if(READY_ADC[j] == 0)
			{
				
				ChipSelekt(j, CS_ON);
				
				st = read_spi_rdata(j, &Adc8.stADC[j], &w_adc_data[0]);
				
				ChipSelekt(j, CS_OFF);
				
				if((st == TRUE)&&(Adc8.stADC[j]&0xF00000) == 0xC00000)
				{
					for(i=0;i<ADC_CH_ON_CHIP;i++)
					{
						input = (float)ConvertCodToInt(w_adc_data[i]);
						NumCh = ChipToCh[j*ADC_CH_ON_CHIP+i];
						pTar = &TarRam[NumCh];
						
						if(Adc8.adc_mux_set[j][i] == MUX_INPUT)	// работа по измерению входного напряжения
						{
							Adc8.f_adc_data[NumCh] = pTar->k*input + pTar->ofs;
						}else
							{
							if(Adc8.adc_mux_set[j][i] == MUX_TEMPERATURE)// измерение температуры
							{
								Adc8.f_adc_data[NumCh] = (input*2.5)/(32767*0.00049) - (0.145300/0.00049)+25.0;
							}else
							{
								if(Adc8.adc_mux_set[j][i] == MUX_TEST) // тестовый вход
								{
									Adc8.f_adc_data[NumCh] = input*(2.5/32767);
								}else
								{
									if(Adc8.adc_mux_set[j][i] == MUX_MVDD) // напряжение питания микросхемы
									{
										input *= (2.5/32767);
										if((i == 2)||(i == 3))
										{
											Adc8.f_adc_data[NumCh] = input*4.0;
										}else
										{
											Adc8.f_adc_data[NumCh] = input*2.0;
										}
									}else
									{
										if(Adc8.adc_mux_set[j][i] == MUX_INPUT_SHORTED) // закорачиваем входы
										{
											Adc8.f_adc_data[NumCh] = input;
										}
									}
								}
							}
						}
					}
				}
				
				msDelay(1);
				ChipSelekt(j, CS_ON);
				read_spi_reg(j, 0, 13, &Adc8.reg_adc[j][0]);
				ChipSelekt(j, CS_OFF);
				msDelay(1);
				
				ChipSelekt(j, CS_ON);
				read_spi_reg(j, 0x12, 3, &Adc8.reg_adc[j][13]);
				ChipSelekt(j, CS_OFF);
			}
		}
	}
	//========================================================
	Adc8.Info.bits.Flt1 = program.stFLT1;
	Adc8.Info.bits.Flt2 = program.stFLT1;
	//========================================================
	// обмен данными по COM
	if (GetRxByte(&i) == FIFO_OK)
	{
		BuffUart[CountDataUart] = i;
		if(CountDataUart < BUFFER_LEN_UART-1)	
			CountDataUart++;
	} 
	
	GetPak_Uart(&CountDataUart, BuffUart);
	
	if(Adc8.SendPak == TRUE)
	{
		Adc8.SendPak	= 0;
		
		Adc8.stFLT1 = program.stFLT1;
		Adc8.stFLT2 = program.stFLT2;
		Adc8.setON1 = program.setON1;
		Adc8.setON2 = program.setON2;
		Adc8.setPPG1ch_en = program.setPPG1ch_en;
		Adc8.setPPG2ch_en = program.setPPG2ch_en;
	
		CreateAndSend_Pkt_UART0(&Adc8.stFLT1, 150+64, 2, 1);
	}
	//----------------------------------------------------
	// работа с eeprom 
	if(program.StEEPROM_Write == TRUE)
	{
		program.StEEPROM_Write = FALSE;
		// Читаем тарировки из EEPROM в RAM
		SetWorkChI2C(1);
		HighDensSequentialRead(0, (BYTE*)(&TarRam), 256);
		pData = (BYTE*)(&TarRam);
		st =0;
			
		for(i=0; i<ADC_CH; i++)
		{
			clrwdt;
			if((check_NaN_Inf(TarRam[i].k)) == TRUE)
			{
				TarRam[i].k = 1.0;
				st = 1;
			}
			if(check_NaN_Inf(TarRam[i].ofs) == TRUE)
			{
				TarRam[i].ofs = 0.0;
				st = 1;
			}
			TarEEPROM[i].k	= TarRam[i].k;
			TarEEPROM[i].ofs= TarRam[i].ofs;
		}
	}
	//--------------------------------------------------
	if(Adc8.stRecvPak == TRUE)
	{
		Adc8.stRecvPak = FALSE;
		program.setON1 = Adc8.RecvPak[0]&1;
		program.setON2 = (Adc8.RecvPak[0]>>1)&1;
		program.setPPG1ch_en = (Adc8.RecvPak[0]>>2)&1;;
		program.setPPG2ch_en = (Adc8.RecvPak[0]>>3)&1;;
	}
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	if(Id == 0x01)
	{	
		Adc8.SendPak = TRUE;
		return;
	}
	if(Id == 0x03)//Write
	{
		memmove(Adc8.RecvPak, &pData[1], pData[0]);
		Adc8.lenRecvPak = pData[0];
		Adc8.stRecvPak = TRUE;
		return;
	}
	if(Id == 0x04)//Write reg
	{
		Adc8.NumReg 	= pData[0];
		Adc8.ValReg		= pData[1];
		Adc8.stWR_Reg	= TRUE;
		return;
	}	
}
//====================================================================
//=========================================================================================
BYTE read_spi_rdata(BYTE ch, DWORD *status, WORD * pDataADC)
{
	BYTE din, i;
		
	if(ch<2)
	{
		while (SSR9_TDRE == 0);		
		TDR9 = ADC_RDATA;		
	
		
		while (SSR9_RDRF == 0);
		din = RDR9;				// flush reception register
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine
		
		*status = 0;
		for(i = 0; i<3; i++)
		{
			while (SSR9_TDRE == 0);		
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR9_RDRF == 0);
			din = RDR9;				// MSB
			
			(*status) |= ((DWORD)din)<<(8*(2-i));
		}
		for(i = 0; i<8; i++)
		{
			while (SSR9_TDRE == 0);		
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR9_RDRF == 0);
			
			din = RDR9;				// MSB
			
			pDataADC[i] = ((WORD)din)<<8;
			
			while (SSR9_TDRE == 0);		
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR9_RDRF == 0);
			
			din = RDR9;				// MSB
			
			pDataADC[i] |= din;
		}
		return TRUE;
	}else
	{
		while (SSR3_TDRE == 0);		
		TDR3 = ADC_RDATA;		
	
		
		while (SSR3_RDRF == 0);
		din = RDR3;				// flush reception register
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
		
		*status = 0;
		for(i = 0; i<3; i++)
		{
			while (SSR3_TDRE == 0);		
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR3_RDRF == 0);
			din = RDR3;				// MSB
			
			(*status) |= ((DWORD)din)<<(8*(2-i));
		}
		for(i = 0; i<8; i++)
		{
			while (SSR3_TDRE == 0);		
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR3_RDRF == 0);
			
			din = RDR3;				// MSB
			
			pDataADC[i] = ((WORD)din)<<8;
			
			while (SSR3_TDRE == 0);		
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			while (SSR3_RDRF == 0);
			
			din = RDR3;				// MSB
			
			pDataADC[i] |= din;
		}

		return TRUE;
	}
	return FALSE;
}
//=========================================================================================
void write_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData)
{
	BYTE i;
	
	if(ch<2)
	{
		while (SSR9_TDRE == 0);		
		TDR9 = ADC_WREG | (Addr&0x1F);
		
		while (SSR9_TDRE == 0);		
		TDR9 = ((Len-1)&0x1F);
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			while (SSR9_TDRE == 0);		
			TDR9 = pData[i];	
		}
		while (ECCR9 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR9 & 0x01)); // wait for transmission finished
		i = RDR9;				
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine
	}else
	{
		while (SSR3_TDRE == 0);		
		TDR3 = ADC_WREG | (Addr&0x1F);
		
		while (SSR3_TDRE == 0);		
		TDR3 = ((Len-1)&0x1F);
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			while (SSR3_TDRE == 0);		
			TDR3 = pData[i];	
		}
		while (ECCR3 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR3 & 0x01)); // wait for transmission finished
		i = RDR3;
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
	}
}
//=========================================================================================
void read_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData)
{
	BYTE i, din;
	
	if(ch<2)
	{
		while (SSR9_TDRE == 0);
		TDR9 = ADC_RREG | (Addr&0x1F);
		
		while (SSR9_RDRF == 0);
		din = RDR9;

		while (SSR9_TDRE == 0);
		TDR9 = ((Len-1)&0x1F);

		while (SSR9_RDRF == 0);
		din = RDR9;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			while (SSR9_TDRE == 0);
			TDR9 = 0;	
			
			while (SSR9_RDRF == 0);
			pData[i] = RDR9;				// MSB
		}
	}else
	{
		while (SSR3_TDRE == 0);
		TDR3 = ADC_RREG | (Addr&0x1F);

		while (SSR3_RDRF == 0);
		din = RDR3;

		while (SSR3_TDRE == 0);
		TDR3 = ((Len-1)&0x1F);

		while (SSR3_RDRF == 0);
		din = RDR3;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			while (SSR3_TDRE == 0);
			TDR3 = 0;	
			
			while (SSR3_RDRF == 0);
			pData[i] = RDR3;				// MSB
		}
	}
}
//=========================================================================================
void StopContMode(BYTE ch)
{
	if(ch<2)
	{
		while (SSR9_TDRE == 0);		
		TDR9 = ADC_SDATAC;
		while (ECCR9 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR9 & 0x01)); // wait for transmission finished
	}else
	{
		while (SSR3_TDRE == 0);		
		TDR3 = ADC_SDATAC;
		while (ECCR3 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR3 & 0x01)); // wait for transmission finished
	}
}

/*
/*void write_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData)
{
	BYTE i, din;
	
	if(ch<2)
	{
		TDR9 = ADC_WREG | (Addr&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR9;
		TDR9 = ((Len-1)&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR9;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			TDR9 = pData[i];	
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			din = RDR9;
			setTimer(&TimerSpi, 3);
			while (SSR9_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
		}
	}else
	{
		TDR3 = ADC_WREG | (Addr&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)		
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR3;
		TDR3 = ((Len-1)&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR3;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			TDR3 = pData[i];	
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			din = RDR3;
			setTimer(&TimerSpi, 3);
			while (SSR3_TDRE == 0)	
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
		}
	}
}
void read_spi_reg(BYTE ch, BYTE Addr, BYTE Len, BYTE * pData)
{
	BYTE i, din;
	
	if(ch<2)
	{
		TDR9 = ADC_RREG | (Addr&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR9_TDRE == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR9;
		TDR9 = ((Len-1)&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR9_TDRE == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR9;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			TDR9 = 0;	
			
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0)
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			
			pData[i] = RDR9;				// MSB
		}
	}else
	{
		TDR3 = ADC_RREG | (Addr&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR3_TDRE == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR3;
		TDR3 = ((Len-1)&0x1F);
		setTimer(&TimerSpi, 3);
		while (SSR3_TDRE == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR3;
		
		for(i = 0; i<(Len&0x1F); i++)
		{
			TDR3 = 0;	
			
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0)
			{
				if(getTimer(&TimerSpi) == 0) return;
			}
			
			pData[i] = RDR3;				// MSB
		}
	}
}
void StopContMode(BYTE ch)
{
	BYTE din;
	
	if(ch<2)
	{
		TDR9 = ADC_SDATAC;
		setTimer(&TimerSpi, 3);
		while (SSR9_RDRF == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR9;
	}else
	{
		TDR3 = ADC_SDATAC;
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0)
		{
			if(getTimer(&TimerSpi) == 0) return;
		}
		din = RDR3;
	}
}
*/
/*BYTE read_spi_rdata(BYTE ch, DWORD *status, WORD * pDataADC)
{
	BYTE din, i;
		
	if(ch<2)
	{
		TDR9 = ADC_RDATA;		
	
		setTimer(&TimerSpi, 3);
		
		while (SSR9_RDRF == 0) // transmission finished (via reception)?
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		
		din = RDR9;				// flush reception register
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine
		
		*status = 0;
		for(i = 0; i<3; i++)
		{
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0) // transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			din = RDR9;				// MSB
			
			(*status) |= ((DWORD)din)<<(8*(2-i));
			
			setTimer(&TimerSpi, 3);
			while (SSR9_TDRE == 0)
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
		}
		for(i = 0; i<8; i++)
		{
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0)// transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			
			din = RDR9;				// MSB
			
			pDataADC[i] = ((WORD)din)<<8;
			
			setTimer(&TimerSpi, 3);
			while (SSR9_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			TDR9 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR9_RDRF == 0) // transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			
			din = RDR9;				// MSB
			
			pDataADC[i] |= din;
			setTimer(&TimerSpi, 3);
			while (SSR9_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
		}
		return TRUE;
	}else
	{
		TDR3 = ADC_RDATA;		
	
		setTimer(&TimerSpi, 3);
		while (SSR3_RDRF == 0) // transmission finished (via reception)?
		{
			if(getTimer(&TimerSpi) == 0) return FALSE;
		}
		din = RDR3;				// flush reception register
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
		
		*status = 0;
		for(i = 0; i<3; i++)
		{
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0) // transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			din = RDR3;				// MSB
			
			(*status) |= ((DWORD)din)<<(8*(2-i));
			
			setTimer(&TimerSpi, 3);
			while (SSR3_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
		}
		for(i = 0; i<8; i++)
		{
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0) // transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			din = RDR3;				// MSB
			
			pDataADC[i] = ((WORD)din)<<8;
			setTimer(&TimerSpi, 3);
			while (SSR3_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
			
			TDR3 = 0x00;			// set dummy byte to produce SCLK
			
			setTimer(&TimerSpi, 3);
			while (SSR3_RDRF == 0) // transmission finished (via reception)?
			{
				if(getTimer(&TimerSpi) == 0) return FALSE; 
			}
			din = RDR3;				// MSB
			
			pDataADC[i] |= din;
			setTimer(&TimerSpi, 3);
			while (SSR3_TDRE == 0)		
			{
				if(getTimer(&TimerSpi) == 0) return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
*/

#endif;
