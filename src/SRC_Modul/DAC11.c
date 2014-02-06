#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"
#include "uart.h"
#include "pakuart.h"
#include "fifo.h"
#include "i2c.h"
#include "spi.h"

#ifdef PLATA_DAC11

#include "dac11.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 
//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//==============================================================================
//Максимальное значение которое можно записать в ЦАП 
//Минимальное значение которое можно записать в ЦАП    
//const static float	MinValueDac[COUNT_DAC_CH] = {4.0,   4.0,  4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
//const static float	MaxValueDac[COUNT_DAC_CH] = {20.0, 20.0, 20.0, 20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0};
const static float	MinValueDac[COUNT_DAC_CH] = {0,   0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const static float	MaxValueDac[COUNT_DAC_CH] = {4000.0, 4000.0, 4000.0, 4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0};
//==============================================================================
CDAC11 Dac11;
//==============================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
//==============================================================================
WORD	(*STATE_BLOCK) 		= &Dac11.Info.word;
void	(*INIT_BLOK)(void)	= InitDAC11;
void 	(*DRIVER_BLOK)(void)= DriverDAC11;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;
//==============================================================================
WORD TIME_WAIT_SELECT_MASTER = 30/TIMER_RESOLUTION_MS;				// Время задержки перед выставлением статуса мастер
WORD TIME_WAIT_SELECT_MASTER_IF_ERROR = 3000/TIMER_RESOLUTION_MS;	// Время задержки перед выставлением статуса мастер при наличии ощибок и отсутствия другого мастера на шине
float	dERR_DAC_mA = 0.05;											//Погрешность измерения ЦАП ->АЦП
//==============================================================================
TTar	TarrRAM[2*COUNT_DAC_CH];		// Тарировки ЦАПа и АЦП скопированные в RAM  
TTar	TarrEEPROM[2*COUNT_DAC_CH];		// тарировки каналов в EEPROM
//==============================================================================
#define SETTING_7	0x57	// настройки цапа
//==============================================================================
TPAK_SPI_RM		pak_spi_rm;
TPAK_SPI_ADC	pak_spi_adc;
TPAK_SPI_DAC	pak_spi_dac;
//==============================================================================
BYTE	Spi_Buf_In[20];
BYTE	Spi_Buf_Out[20];
//==============================================================================
const WORD PeriodDout			= 2/TIMER_RESOLUTION_MS;
const WORD PeriodOprosDin		= 25/TIMER_RESOLUTION_MS;
const WORD PeriodOprosAin		= 20/TIMER_RESOLUTION_MS;
const WORD PeriodOprosTemp		= 1000/TIMER_RESOLUTION_MS;
//==============================================================================
void Operate_Max1329_DAC_Write(BYTE Num, BYTE ch, WORD data);
void Operate_Max1329_ADC_Convert(BYTE Num, BYTE Mux, BYTE Gain, BYTE Bip);
void Operate_Max1329_RegMode(BYTE Num, BYTE RW, BYTE Adr, BYTE Count);
WORD Operate_Max1329_DAC_Read(BYTE Num, BYTE ch);
void Operate_Max1329_Reset(BYTE Num);
void Operate_Max1329_RegMode_Mask(WORD Mask, BYTE Adr, BYTE Count);// только запись
//==============================================================================
void SelectAll_DAC(void);
void UnSelectAll_DAC(void);
void SelectSPI_DAC(BYTE ch);
void SelectMask_DAC(WORD Mask);
void SelectMasterDAC(void);
//------------------------------------------------------------------------------
static BYTE	SerN[3][8];	// серийные номера тройки блоков, по днулевым индексом свой
static BYTE CountBl=0;	// количество увиденных блоков, минимум один мы
static WORD CounterPingBlock[2];

BYTE ServiceMaster(BYTE bus_id, Message *m)
{
	BYTE i, st;
	if(CountBl == 0)
	{
		CountBl = 1;
		if(program.Cnt1WareDev > 0)
		{
			for(i=0; i<8; i++)
				SerN[0][i] = program.SN_1Ware_Dev[0][i];
		}else
		{
			for(i=0; i<8; i++)
				SerN[0][i] = 0;
		}
	}
	if(m->len == 8)
	{
		if(CountBl == 1)
		{
			for(i=0; i<8; i++)
				SerN[CountBl][i] = m->data[i];
			CountBl++;
		}
		if(CountBl == 2)
		{
			// сначала проверим с уже принятым серийником
			st = 0;
			for(i=0; i<8; i++)
			{
				if(SerN[1][i] != m->data[i]) st = 1; 
			}
			if(st = 1)
			{
				for(i=0; i<8; i++)
					SerN[CountBl][i] = m->data[i];
				CountBl++;
			}
		}
		if(CountBl == 3) // контролируем наличие блоков на линии
		{
			st = 0;
			for(i=0; i<8; i++)
			{
				if(SerN[1][i] != m->data[i]) st = 1; 
			}
			if(st == 0) CounterPingBlock[0]++;	//пришол пинг 
			st = 0;
			for(i=0; i<8; i++)
			{
				if(SerN[2][i] != m->data[i]) st = 1; 
			}
			if(st == 0) CounterPingBlock[1]++;	//пришол пинг 
		}
	}
	return 0;
}
BYTE ServiceObmenData(BYTE bus_id, Message *m)
{
	return 0;
}
//------------------------------------------------------------------------------
void InitDAC11(void)
{
	BYTE i;
	//==========================================
	DDR00=0xFF;
	DDR02=0xFF;
	DDR06=0x00;
	DDR07=0x07;
	DDR14=0xFF;
	//==========================================
	UnSelectAll_DAC();
	//==========================================
	for(i=0;i<COUNT_DAC_CH;i++)
	{
		Dac11.fDAC_New[i]	=0;
		Dac11.fDAC_Set[i]	=0;
	}
	//==========================================
	Dac11.EnOutDac		= 0;
	Dac11.Info.word		= 0;
	Dac11.WriteTar		= 0;
	Dac11.Master		= false;
	Dac11.StatusMaster	= 0;
	Dac11.HiLoDec		= false;
	Dac11.HiLo			= 0;
	Dac11.DiagRele		= 0;
	Dac11.ErrorSet		= false;
	Dac11.TimerMasterError=0;
	Dac11.wOldError		= 0;
	Dac11.wError		= 0;
	Dac11.DiagDAC		= 0;
	//==========================================
	Dac11.SendPak		= FALSE;
	Dac11.SendPakTar	= FALSE;
	//==========================================
	Dac11.TimerDout = 0;
	Dac11.TimerAin  = 0;
	Dac11.TimerTemp = 100;
	Dac11.TimerDin	= 0;
	add_timer(&TimerStartBlock);
	add_timer(&Dac11.TimerDout);
	add_timer(&Dac11.TimerAin);
	add_timer(&Dac11.TimerTemp);
	add_timer(&Dac11.TimerDin);
	//==========================================
	InitSPI_1_inv();
	InitSPI_2_inv();
	//==========================================
}
//========================================================================
int ConvertADCtoINT(WORD d)
{
	int c;
	if((d&0x800)==0)
	{
		c = d; 
		return c;
	}else
	{
		c=(int)((~d+1)&0xEFF);
		c*=-1;
		return c;
	}
}
//========================================================================
// Выбор SPI 
void SelectMask_DAC(WORD Mask)
{
	if(Mask&0x0001) CS11 = CS_ON;
	if(Mask&0x0002) CS12 = CS_ON;
	if(Mask&0x0004) CS13 = CS_ON;
	if(Mask&0x0008) CS14 = CS_ON;
	if(Mask&0x0010) CS15 = CS_ON;
	if(Mask&0x0020) CS16 = CS_ON;
	if(Mask&0x0040) CS21 = CS_ON;
	if(Mask&0x0080) CS22 = CS_ON;
	if(Mask&0x0100) CS23 = CS_ON;
	if(Mask&0x0200) CS24 = CS_ON;
	if(Mask&0x0400) CS25 = CS_ON;
	if(Mask&0x0800) CS26 = CS_ON;
}

void SelectAll_DAC(void)
{
	CS11 = CS_ON;
	CS12 = CS_ON;
	CS13 = CS_ON;
	CS14 = CS_ON;
	CS15 = CS_ON;
	CS16 = CS_ON;
	CS21 = CS_ON;
	CS22 = CS_ON;
	CS23 = CS_ON;
	CS24 = CS_ON;
	CS25 = CS_ON;
	CS26 = CS_ON;
}
void UnSelectAll_DAC(void)
{
	CS11 = CS_OFF;
	CS12 = CS_OFF;
	CS13 = CS_OFF;
	CS14 = CS_OFF;
	CS15 = CS_OFF;
	CS16 = CS_OFF;
	CS21 = CS_OFF;
	CS22 = CS_OFF;
	CS23 = CS_OFF;
	CS24 = CS_OFF;
	CS25 = CS_OFF;
	CS26 = CS_OFF;
}
void SelectSPI_DAC(BYTE ch)
{
	if(ch>(COUNT_DAC_CH-1)) return;

	UnSelectAll_DAC();
	
	if(ch<6)
	{
		ADDR10 = ch&1;
		ADDR11 = (ch>>1)&1;
		ADDR12 = (ch>>2)&1;
	}else
	{
		ADDR20 = ((ch-6)>>0)&1;
		ADDR21 = ((ch-6)>>1)&1;
		ADDR22 = ((ch-6)>>2)&1;
	}
	
	SelectMask_DAC(((WORD)1)<<ch);
}
//========================================================================
//Масштабирование записываемого значения запись в ЦАП     --
//========================================================================
void WriteNormalDataDac(BYTE ch, float data)
{
	float res;
	WORD out;

	if(ch>(COUNT_DAC_CH-1)) return;

	res=TarrRAM[ch].k*data+TarrRAM[ch].ofs;

	if(res>MAX_COD_DAC) res=(float)MAX_COD_DAC;
	if(res<MIN_COD_DAC) res=(float)MIN_COD_DAC;

	out=(WORD)res;
	Operate_Max1329_DAC_Write(ch, 0,out);
}
//=========================================================================
void DriverDAC11(void)
{
	BYTE	i, st, bData;
	WORD	wData, tmp;
	float	fData;
	int		iData;
	//------------------------------------------------
	// обновление адреса блока
	Dac11.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			del_timer(&TimerStartBlock);
			stStartBlock = TRUE;
			//========================================================
			// Читаем тарировки из EEPROM в RAM
			SetWorkChI2C(1);
			HighDensSequentialRead(0, (BYTE *)(&TarrRAM),2*8*COUNT_DAC_CH);
			st =0;
			
			for(i=0; i<2*COUNT_DAC_CH; i++)
			{
				if((check_NaN_Inf(TarrRAM[i].k)) == TRUE)
				{
					TarrRAM[i].k = 1.0;
					st = 1;
				}
				if(check_NaN_Inf(TarrRAM[i].ofs) == TRUE)
				{
					TarrRAM[i].ofs = 0.0;
					st = 1;
				}
				TarrEEPROM[i].k		= TarrRAM[i].k;
				TarrEEPROM[i].ofs	= TarrRAM[i].ofs;
			}
			
			if(st == 1)	HighDensPageWrite(0, (BYTE*)(&TarrRAM), 2*8*COUNT_DAC_CH);
			
			//=============================================================
			// если хотябы один цап имеет ед тарировки разрешаем коммм реле
			Dac11.TarrStatus		=true;
			for(i=0; i<COUNT_DAC_CH; i++)
			{
				if(TarrRAM[i].k == 1.0)
					Dac11.TarrStatus = false;
			}
			//=============================================================
			// Reset всех ЦАПов
			Operate_Max1329_Reset(0xFF);
			
			msDelay(5);
		
			// CP/VM 
			Spi_Buf_Out[0]=0x00;
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0xE,1);
		
			// инициализируем входы/выходы 
			Spi_Buf_Out[0]=0x80;
			Spi_Buf_Out[1]=0x00;
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0x10,2);
			
			
			// инициализируем АЦП
			//Spi_Buf_Out[0]=0x17;
			Spi_Buf_Out[0]=0xF7;
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0x0,1);
			Spi_Buf_Out[0]=0x0E;
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0x1,1);
			
			// инициализируем ЦАП 
			Spi_Buf_Out[0] = SETTING_7;		
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0x7,1);
			
			Operate_Max1329_DAC_Write(0xFF, 0, 0);
			Operate_Max1329_DAC_Write(0xFF, 1, 100);
			
			Operate_Max1329_ADC_Convert(0xFF,1, 3, 0);
		}
	}
	//========================================================
	if(stStartBlock == TRUE)
	{
		if(Dac11.WriteTar == 1)
		{
			st = 0;
			for(i=0; i<2*COUNT_DAC_CH; i++)
			{
				if(TarrEEPROM[i].k	!= TarrRAM[i].k)
				{
					st = 1;
					TarrEEPROM[i].k	= TarrRAM[i].k;
				}
				if(TarrEEPROM[i].ofs	!= TarrRAM[i].ofs)
				{
					st = 1;
					TarrEEPROM[i].ofs = TarrRAM[i].ofs;
				}
			}
			if(st == 1)
			{
				SetWorkChI2C(1);
				HighDensPageWrite(0, (BYTE*)(&TarrEEPROM), 2*8*COUNT_DAC_CH);
			}
		}
		//=======================================================================================
		// Управление выходным реле
		// Запись дискретных выходов
		// коммутация на выход
		// коммутируем только если мы мастер
		//=======================================================================================
		if(getTimer(&Dac11.TimerDout) == 0)	
		{
			setTimer(&Dac11.TimerDout, PeriodDout);
			Dac11.HiLoDec = true;
			Dac11.HiLo = ~Dac11.HiLo;
		}
		
		if(Dac11.wOldError != Dac11.wError)
		{
			setTimer(&Dac11.TimerMasterError, TIME_WAIT_SELECT_MASTER_IF_ERROR);
			Dac11.ErrorSet = true;
		}
		wData = Dac11.EnOutDac&(~Dac11.wError);
		if((Dac11.Master == true)||(Dac11.TarrStatus==false))
		{
			if((wData != 0)&&(Dac11.HiLoDec == true))
			{
				if(Dac11.HiLo == 0)
					Spi_Buf_Out[0]=0;
				else
					Spi_Buf_Out[0]=8;
					
				Operate_Max1329_RegMode_Mask(wData, 0x11,1);
			}
		}
		Dac11.HiLoDec = false;
		//===================================================
		// Опрашиваем АЦП 
		//===================================================
		if(getTimer(&Dac11.TimerAin) == 0)	
		{
			setTimer(&Dac11.TimerAin, PeriodOprosAin);
			for(i=0;i<COUNT_DAC_CH;i++)
			{
				Operate_Max1329_RegMode(i, READ_MODE_SPI, 2,2);
				tmp		= Spi_Buf_In[0];
				wData	= (tmp<<4)&0x0FF0;
				wData	|= (Spi_Buf_In[1]>>4)&0xF;
					
				iData = wData&0xFFF;
				
				Dac11.fADC[i] = TarrRAM[COUNT_DAC_CH+i].k*(float)iData + TarrRAM[COUNT_DAC_CH+i].ofs;
			}
		}
		//=============================
		// Запись в ЦАП по изменениям
		//=============================
		if((Dac11.fDAC_New[0] != Dac11.fDAC_Set[0])||(Dac11.fDAC_New[1] != Dac11.fDAC_Set[1]))
		{
			if(Dac11.TarrStatus == false)
			{
				Dac11.fDAC_Set[0] = Dac11.fDAC_New[0];
				WriteNormalDataDac(0, Dac11.fDAC_Set[0]);
			}else
			{
				{
					fData = Dac11.fDAC_New[0];
						
					if(fData>MaxValueDac[0])	fData = MaxValueDac[0];
					if(fData<MinValueDac[0])	fData = MinValueDac[0];
					if(fData != Dac11.fDAC_Set[0])
					{
						Dac11.fDAC_Set[0] = fData;
						WriteNormalDataDac(0, fData);
					}
				}	
			}
		}
		//=============================================================================================
		// Опрашиваем дискретные входы
		//=============================================================================================
		if(getTimer(&Dac11.TimerDin) == 0)
		{
			setTimer(&Dac11.TimerDin, PeriodOprosDin);
			for(i=0;i<COUNT_DAC_CH;i++)
			{
				Operate_Max1329_RegMode(i, READ_MODE_SPI, 0x11,1);
				bData = Spi_Buf_In[0];
							
				if((bData&1) == 1) SETBIT(Dac11.DiagRele,i);
				else CLEARBIT(Dac11.DiagRele,i);
				
				Operate_Max1329_RegMode(i, READ_MODE_SPI, 0x7,1);
				if(Spi_Buf_In[0] == SETTING_7) SETBIT(Dac11.DiagDAC, i);
				else CLEARBIT(Dac11.DiagDAC, i);
			}
		}		//===================================================
		// Опрашиваем температурные датчики
		/*if(getTimer(&Dac11.TimerTemp) == 0)
		{
			setTimer(&Dac11.TimerTemp, PeriodOprosTemp);
			
			
			Operate_Max1329_RegMode(0,READ_MODE_SPI, 2,2);
			
			tmp		= Spi_Buf_In[0];
			wData	= (tmp<<4)&0x0FF0;
			wData	|= (Spi_Buf_In[1]>>4)&0xF;
			
			iData = ConvertADCtoINT(wData);
			
			Dac11.Temperature[i] = iData*0.125;
			//Dac11.fADC[0] = iData*0.125;
			
			Operate_Max1329_ADC_Convert(0xFF,0x8, 0, 0);
		}*/
		
	}
	//========================================================
	// обмен данными по COM
	if (GetRxByte(&i) == FIFO_OK)
	{
		BuffUart[CountDataUart] = i;
		if(CountDataUart < BUFFER_LEN_UART-1)	
			CountDataUart++;
	} 
	
	GetPak_Uart(&CountDataUart, BuffUart);
	
	if(Dac11.SendPak == TRUE)
	{
		Dac11.SendPak	= FALSE;
		CreateAndSend_Pkt_UART0((U8 *)(&Dac11.fDAC_Set), COUNT_DAC_CH*4*2+2+2+2, 2, 1);
	}
	if(Dac11.SendPakTar == TRUE)
	{
		Dac11.SendPakTar = FALSE;
		CreateAndSend_Pkt_UART0((U8 *)(&TarrRAM), COUNT_DAC_CH*4*2*2, 2, 2);
	}
	//----------------------------------------------------
}
//========================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	TTar *pTar;
	TPak3 *pPak3;
	BYTE i;
	WORD *pW;
	
	if(Id == 0x01)
	{	
		Dac11.SendPak = TRUE;
	}
	if(Id == 0x02)
	{
		Dac11.SendPakTar = TRUE;
	}
	if(Id == 0x03)	// Запись тарировок
	{
		pTar = (TTar *)pData;
		for(i=0; i<2*COUNT_DAC_CH; i++)
		{
			TarrRAM[i].k	= pTar[i].k;
			TarrRAM[i].ofs	= pTar[i].ofs;
		}
		Dac11.WriteTar = 1;
	}
	if(Id == 0x04)	// запись в ЦАП
	{
		pPak3 = (TPak3 *)pData;
		if(pPak3->SelectDAC<COUNT_DAC_CH)
			Dac11.fDAC_New[pPak3->SelectDAC] =  pPak3->fWriteDAC;
			
	}
	if(Id == 0x05)	// Коммутируе реле ЦАПа
	{
		pW = (WORD *)pData;
		
		Dac11.EnOutDac = *pW;
	}
}
//========================================================================
BYTE Send_SIO(BYTE ch, BYTE data)
{
	BYTE ret;
	if(ch == 0xFF)
	{
		while (SSR9_TDRE == 0);		
		TDR9 = data;
		while (SSR9_TDRE == 0);		
		while (ECCR9 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR9 & 0x01)); // wait for transmission finished
		while (SSR9_RDRF == 0);
		
		ret = RDR9;				
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine

		while (SSR3_TDRE == 0);		
		TDR3 = data;
		while (SSR3_TDRE == 0);		
		while (ECCR3 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR3 & 0x01)); // wait for transmission finished
		while (SSR3_RDRF == 0);
		ret = RDR3;				
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
		return 0;
	}
	if(ch<6)
	{
		while (SSR9_TDRE == 0);		
		TDR9 = data;
		while (SSR9_TDRE == 0);		
		while (ECCR9 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR9 & 0x01)); // wait for transmission finished
		while (SSR9_RDRF == 0);
		
		ret = RDR9;				
		SCR9_CRE = 1;			// Clear possible errors, reset reception state machine
		return ret;
	}else
	{
		while (SSR3_TDRE == 0);		
		TDR3 = data;
		while (SSR3_TDRE == 0);		
		while (ECCR3 & 0x01);    // wait for start of transmission (or ongoing)
		while (!(ECCR3 & 0x01)); // wait for transmission finished
		while (SSR3_RDRF == 0);
		ret = RDR3;				
		SCR3_CRE = 1;			// Clear possible errors, reset reception state machine
		return ret;
	}
}
//========================================================================
void Operate_Max1329_DAC_Write(BYTE Num, BYTE ch, WORD data)
{
	if(Num == 0xFF) SelectAll_DAC();
	else SelectSPI_DAC(Num);
	DisInterrupt();
					
	pak_spi_dac.bits.ch 	= ch;
	pak_spi_dac.bits.d1 	= 1;
	pak_spi_dac.bits.start	= 0;
	pak_spi_dac.bits.dac 	= data;
	pak_spi_dac.bits.rw 	= WRITE_MODE_SPI;
	Send_SIO(Num, pak_spi_dac.data[1]);
	Send_SIO(Num, pak_spi_dac.data[0]);
	EnInterrupt();
	UnSelectAll_DAC();
}
WORD Operate_Max1329_DAC_Read(BYTE Num, BYTE ch)
{
	WORD d;
	
	if(Num == 0xFF) SelectAll_DAC();
	else SelectSPI_DAC(Num);
	DisInterrupt();
	
	pak_spi_dac.bits.ch 	= ch;
	pak_spi_dac.bits.d1 	= 1;
	pak_spi_dac.bits.start	= 0;
	pak_spi_dac.bits.dac 	= 0;
	pak_spi_dac.bits.rw 	= READ_MODE_SPI;
	d=Send_SIO(Num, pak_spi_dac.data[1]);
	d = (d&0xF)<<8;
	d |= Send_SIO(Num, pak_spi_dac.data[0]);
	EnInterrupt();
	UnSelectAll_DAC();
	return d;
}


void Operate_Max1329_ADC_Convert(BYTE Num, BYTE Mux, BYTE Gain, BYTE Bip)
{
	if(Num == 0xFF) SelectAll_DAC();
	else SelectSPI_DAC(Num);
	DisInterrupt();
	
	pak_spi_adc.bits.Mux	= Mux;
	pak_spi_adc.bits.Gain	= Gain;
	pak_spi_adc.bits.Bip	= Bip;
	pak_spi_adc.bits.start	= 1;
	
	Send_SIO(Num, pak_spi_adc.data);
	EnInterrupt();
	UnSelectAll_DAC();
}

void Operate_Max1329_RegMode(BYTE Num, BYTE RW, BYTE Adr, BYTE Count)
{
	BYTE i;
	
	if(Num == 0xFF) SelectAll_DAC();
	else SelectSPI_DAC(Num);
	DisInterrupt();
	
	pak_spi_rm.data = 0;
	pak_spi_rm.bits.RW	= RW;
	pak_spi_rm.bits.adr	= Adr;
	Send_SIO(Num, pak_spi_rm.data);
		
	if(RW == READ_MODE_SPI)
	{
		for(i=0;i<Count;i++)
			Spi_Buf_In[i] = Send_SIO(Num, 0);
	}else
	{
		for(i=0;i<Count;i++)
			Send_SIO(Num, Spi_Buf_Out[i]);
	}
	EnInterrupt();
	UnSelectAll_DAC();
}
void Operate_Max1329_RegMode_Mask(WORD Mask, BYTE Adr, BYTE Count)// только запись
{
	BYTE i;
	
	SelectMask_DAC(Mask);
	
	DisInterrupt();
	
	pak_spi_rm.data = 0;
	pak_spi_rm.bits.RW	= WRITE_MODE_SPI;;
	pak_spi_rm.bits.adr	= Adr;
	if(Mask&0x03F) Send_SIO(0, pak_spi_rm.data);
	if(Mask&0xFC0) Send_SIO(6, pak_spi_rm.data);
		
	for(i=0;i<Count;i++)
	{
		if(Mask&0x03F) Send_SIO(0, Spi_Buf_Out[i]);
		if(Mask&0xFC0) Send_SIO(6, Spi_Buf_Out[i]);
	}
	
	EnInterrupt();
	UnSelectAll_DAC();
}
void Operate_Max1329_Reset(BYTE Num)
{
	if(Num == 0xFF) SelectAll_DAC();
	else SelectSPI_DAC(Num);
	DisInterrupt();
	
	pak_spi_rm.data = 0;
	pak_spi_rm.bits.RW	= WRITE_MODE_SPI;
	pak_spi_rm.bits.adr	= 0x1F;
	Send_SIO(Num, pak_spi_rm.data);
		
	Send_SIO(Num, 0xFF);
	EnInterrupt();
	UnSelectAll_DAC();
}

//========================================================================
// Процедура по выбору мастера из 3х ЦАПов
void SelectMasterDAC(void)
{
/*
	//-------------------------------------------------
	// Проверяем есть ли в данный момент мастер на шине
	if(DAC_STATUS_NEIGHBOUR != 3)
	{
		// Мастер на шине уже есть выходим из процедуры определения мастера
		Dac11.Master=false;
		DAC_MASTER_SET_OFF;
		Dac11.TimerMasterError = 0;
		return;
	}
	// Если хотя бы по одному выходу есть проблемы, с выдачей команд, то данный цап не может быть мастером
	if(Dac11.ErrorSet == true)	
	{
		if(Dac11.TimerMasterError<TIME_WAIT_SELECT_MASTER_IF_ERROR)// скорее всего есть ошибки по и в других канала поэтом выбираем хотябы одного мастера
		{
			Dac11.wOldError = Dac11.wError;
			Dac11.Master=false;
			DAC_MASTER_SET_OFF;	
			return;	
		}
		Dac11.TimerMasterError = TIME_WAIT_SELECT_MASTER_IF_ERROR+1;
		
	}
	//-------------------------------------------------
	// Если нет связи с УСО выходим 
	if((program.TimerCan0>=TIME_OUT_CAN)&&(program.TimerCan1>=TIME_OUT_CAN))
	{
		Dac11.Master=false;
		DAC_MASTER_SET_OFF;
		return;
	}
	//-------------------------------------------------
	// Если блок не проинициализирован УСО выходим 
	if(getState() != Operational)
	{
		Dac11.Master=false;
		DAC_MASTER_SET_OFF;
		return;
	}
	//-------------------------------------------------
	// Если данный блок уже является мастером то выходим из процедуры выявления мастера
	if(Dac11.Master == true)
	{
		return;
	}
	//-----------------------------------------------------------------
	// БЛОК НЕ ЯВЛЯЕТСЯ МАСТЕРОМ, И В ДАННЫЙ МОМЕНТ НА ШИНЕ НЕТ МАСТЕРА
	//-----------------------------------------------------------------
	if(Dac11.StatusMaster == 0)
	{
		DAC_MASTER_SET_ON;
		Dac11.TimerMaster=0;
		Dac11.StatusMaster = 1;
		return;	
	}
	if(Dac11.StatusMaster == 1)
	{
		if(Dac11.TimerMaster<TIME_WAIT_SELECT_MASTER)
		{	// выжидаем реакции соседей
			return;
		}
		//==============================
		// Блок можем стать мастером
		Dac11.Master = true;
		Dac11.StatusMaster = 0;
		return;	
	}
	//-----------------------------------------------------------------
	*/
}
//========================================================================

#endif