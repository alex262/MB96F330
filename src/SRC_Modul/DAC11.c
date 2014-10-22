#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"

#ifdef PLATA_DAC11

#include "uart.h"
#include "pakuart.h"
#include "fifo.h"
#include "i2c.h"
#include "spi.h"

#include "objdictdef.h"
#include "dac11.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 
//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//==============================================================================
// макимальное значение отклонения ЦАПа от значения АЦП
#define	MAX_ERROR_DAC_mA	0.5
//==============================================================================
//Максимальное значение которое можно записать в ЦАП 
//Минимальное значение которое можно записать в ЦАП    
const static float	MinValueDac[COUNT_DAC_CH] = {4.0,   4.0,  4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
const static float	MaxValueDac[COUNT_DAC_CH] = {20.0, 20.0, 20.0, 20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0,20.0};
//const static float	MinValueDac[COUNT_DAC_CH] = {0,   0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//const static float	MaxValueDac[COUNT_DAC_CH] = {4000.0, 4000.0, 4000.0, 4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0,4000.0};
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
//==============================================================================
TTar	TarRam[2*COUNT_DAC_CH];										// Тарировки ЦАПа и АЦП скопированные в RAM  
TTar	TarrEEPROM[2*COUNT_DAC_CH];									// тарировки каналов в EEPROM
//==============================================================================
#define SETTING_0	0xF7	
#define SETTING_1	0x0E	
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
const WORD PeriodPing			= 50/TIMER_RESOLUTION_MS;

const WORD PeriodTimeoutADC		= 40/TIMER_RESOLUTION_MS; // задержка измерения АЦП после выдачи нового значения в ЦАП или Коммутация канала

#define MAX_COUNT_PING_LOST		(5)	// максимальное количество потеряных пингов после чего блок считаем исключеным из системы
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
void ClearErrorDAC(BYTE ch, BYTE n);
//------------------------------------------------------------------------------
BYTE ServiceMaster(BYTE bus_id, Message *m)
{
	BYTE i, st, j;
	if(Dac11.StNeigbor[0] == FALSE)
	{
		if(program.Cnt1WareDev > 0)
		{
			Dac11.StNeigbor[0]	= TRUE;
			Dac11.PingBlock[0]	= 0;	//пришол пинг 
			for(i=0; i<8; i++)
				Dac11.SerN[0][i] = program.SN_1Ware_Dev[0][i];
		}else
		{
			for(i=0; i<8; i++)
				Dac11.SerN[0][i] = 0;
		}
	}
	if(m->len == 8)
	{
		for(i=1; i<3; i++)
		{
			st = 0;
			if(Dac11.StNeigbor[i] == TRUE) // если блок на связи
			{
				for(j=0; j<8; j++)
					if(Dac11.SerN[i][j] != m->data[j]) st = 1; 
				if(st == 0)	// если это этот блок
				{
					Dac11.PingBlock[i]	= 0;	//пришол пинг 
					Dac11.StNeigbor[i]	= TRUE;
					st = 2;
					break;
				}		
			}
		}
		if(st != 2) // данный блок пропадал со связи либо ещё не добавлен
		{
			for(i=1; i<3; i++)
			{
				if(Dac11.StNeigbor[i] == FALSE) //место свободно
				{
					for(j=0; j<8; j++)
						Dac11.SerN[i][j] = m->data[j];
					Dac11.PingBlock[i]	= 0;	//пришол пинг 
					Dac11.StNeigbor[i]			= TRUE;
					break;
				}	
			}	
		}
	}
	bus_id;
	return 0;
}
BYTE ServiceObmenData(BYTE bus_id, Message *m)
{
	DWORD dwData;
	BYTE n, addr;
	
	if(m->len != 8) return 0;
	
	addr = m->data[0];
	
	if(addr == Dac11.AddrBl[0]) // если произошол сбой в формировании адреса(кто-то неверно назначил адрес) 
	{
		return 0;	
	}
	
	if(addr > 2) // некорректный адрес
	{
		return 0;	
	}
	// ищем индекс в нашей базе с таким адресом
	if((Dac11.AddrBl[1] == addr)||(Dac11.AddrBl[2] == addr))
	{
		if(Dac11.AddrBl[1] == addr) n = 1;
		if(Dac11.AddrBl[2] == addr) n = 2;
	}else
	{
		return 0;
	}
	
	dwData = m->data[1];
	dwData |= ((DWORD)(m->data[2]))<<8;
	dwData |= ((DWORD)(m->data[3]))<<16;
	
	Dac11.ErrorDAC[n]		= dwData;
	Dac11.StNeigborData[n]	= TRUE;
	Dac11.PingDataBlock[n]	= 0;
			
	Dac11.OutDac[n] = m->data[4];
	Dac11.OutDac[n] |=((WORD)(m->data[5]))<<8;
			
	Dac11.Master[n] = m->data[6];
	Dac11.Master[n] |=((WORD)(m->data[7]))<<8;
	
	bus_id;
	
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
	for(i=0; i<3; i++)
	{
		Dac11.PingBlock[i]			= MAX_COUNT_PING_LOST;
		Dac11.PingDataBlock[i]		= MAX_COUNT_PING_LOST;	
		Dac11.StNeigbor[i]			= FALSE;
		Dac11.StNeigborData[i]		= FALSE;
		Dac11.AddrBl[i]				= 0;
		Dac11.StNeigborData[i]		= FALSE;
		Dac11.ErrorDAC[i]			= 0;
		Dac11.Master[i]				= 0;
		Dac11.OutDac[i]				= 0;
	}
	//==========================================
	Dac11.EnOutDac		= 0;
	Dac11.Info.word		= 0;
	Dac11.WriteTar		= 0;
	Dac11.StatusMaster	= 0;
	Dac11.HiLoDec		= false;
	Dac11.HiLo			= 0;
	Dac11.DiagRele		= 0;
	Dac11.ErrorSet		= false;
	Dac11.TimerMasterError=0;
	Dac11.DiagMAX		= 0;
	Dac11.ErrorUP		= 0;
	Dac11.ErrorDOWN		= 0;
	Dac11.NewOutDac		= 0;
	//==========================================
	Dac11.SendPak		= FALSE;
	Dac11.SendPakTar	= FALSE;
	//==========================================
	Dac11.TimerDout = 0;
	Dac11.TimerAin  = 0;
	Dac11.TimerTemp = 100;
	Dac11.TimerDin	= 0;
	Dac11.TimerPingNeighbor = 0;
	add_timer(&TimerStartBlock);
	add_timer(&Dac11.TimerDout);
	add_timer(&Dac11.TimerAin);
	add_timer(&Dac11.TimerTemp);
	add_timer(&Dac11.TimerDin);
	add_timer(&Dac11.TimerPingNeighbor);
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
	if(Mask&0x0001) {CS11 = CS_ON; }
	if(Mask&0x0002) {CS12 = CS_ON; }
	if(Mask&0x0004) {CS13 = CS_ON; }
	if(Mask&0x0008) {CS14 = CS_ON; }
	if(Mask&0x0010) {CS15 = CS_ON; }
	if(Mask&0x0020) {CS16 = CS_ON; }
	if(Mask&0x0040) {CS21 = CS_ON; }
	if(Mask&0x0080) {CS22 = CS_ON; }
	if(Mask&0x0100) {CS23 = CS_ON; }
	if(Mask&0x0200) {CS24 = CS_ON; }
	if(Mask&0x0400) {CS25 = CS_ON; }
	if(Mask&0x0800) {CS26 = CS_ON; }
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

	res=TarRam[ch].k*data+TarRam[ch].ofs;

	if(res>MAX_COD_DAC) res=(float)MAX_COD_DAC;
	if(res<MIN_COD_DAC) res=(float)MIN_COD_DAC;

	out=(WORD)res;
	Operate_Max1329_DAC_Write(ch, 0,out);
}
//=========================================================================
void DriverDAC11(void)
{
	BYTE	i, st, bData;
	WORD	tmp, wData;
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
			HighDensSequentialRead(0, (BYTE *)(&TarRam),2*8*COUNT_DAC_CH);
			st =0;
			
			for(i=0; i<2*COUNT_DAC_CH; i++)
			{
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
				TarrEEPROM[i].k		= TarRam[i].k;
				TarrEEPROM[i].ofs	= TarRam[i].ofs;
			}
			
			if(st == 1)	HighDensPageWrite(0, (BYTE*)(&TarRam), 2*8*COUNT_DAC_CH);
			
			//=============================================================
			// если хотябы один цап имеет ед тарировки разрешаем коммм реле
			Dac11.TarrStatus		=true;
			for(i=0; i<COUNT_DAC_CH; i++)
			{
				if(TarRam[i].k == 1.0)
				{
					Dac11.TarrStatus = false;
					break;
				}
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
			Spi_Buf_Out[0] = SETTING_0;
			Operate_Max1329_RegMode(0xFF, WRITE_MODE_SPI, 0x0,1);
			Spi_Buf_Out[0] = SETTING_1;
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
		//=======================================================================================
		// определение кто какими каналами будет управлять
		SelectMasterDAC();
		//=======================================================================================
		// Запись тарировок по изменениям
		if(Dac11.WriteTar == 1)
		{
			st = 0;
			for(i=0; i<2*COUNT_DAC_CH; i++)
			{
				if(TarrEEPROM[i].k	!= TarRam[i].k)
				{
					st = 1;
					TarrEEPROM[i].k	= TarRam[i].k;
				}
				if(TarrEEPROM[i].ofs	!= TarRam[i].ofs)
				{
					st = 1;
					TarrEEPROM[i].ofs = TarRam[i].ofs;
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
		
		if(Dac11.EnOutDac != Dac11.NewOutDac)
		{
			Dac11.EnOutDac = Dac11.NewOutDac;
			setTimer(&Dac11.TimerAin, PeriodTimeoutADC);// задержка измерения АЦП после выдачи нового значения в ЦАП или Коммутация канала
		}
	
		Dac11.OutDac[0] = Dac11.Master[0]&Dac11.EnOutDac;
		if(Dac11.OutDac[0] != 0)
		{
			if(Dac11.HiLoDec == true)
			{
				if(Dac11.HiLo == 0)
					Spi_Buf_Out[0]=0;
				else
					Spi_Buf_Out[0]=8;
					
				Operate_Max1329_RegMode_Mask(Dac11.OutDac[0], 0x11,1);
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
				
				Dac11.fADC[i] = TarRam[COUNT_DAC_CH+i].k*(float)iData + TarRam[COUNT_DAC_CH+i].ofs;
				
				if(digit(Dac11.OutDac[0], i) == 1) // если соответствующий выход скомутирован наружу
				{
					if(Dac11.fADC[i]>(Dac11.fDAC_Set[i]+MAX_ERROR_DAC_mA)) SETBIT(Dac11.ErrorUP, i); else CLEARBIT(Dac11.ErrorUP, i);
					if(Dac11.fADC[i]<(Dac11.fDAC_Set[i]-MAX_ERROR_DAC_mA)) SETBIT(Dac11.ErrorDOWN, i); else CLEARBIT(Dac11.ErrorDOWN, i);
				}
			}
		}
		//=============================
		// Запись в ЦАП по изменениям
		//=============================
		for(i=0;i<COUNT_DAC_CH;i++)
		{
			if(Dac11.fDAC_New[i] != Dac11.fDAC_Set[i])
			{
				if(Dac11.TarrStatus == false)
				{
					Dac11.fDAC_Set[i] = Dac11.fDAC_New[i];
					WriteNormalDataDac(i, Dac11.fDAC_Set[i]);
					setTimer(&Dac11.TimerAin, PeriodTimeoutADC);// задержка измерения АЦП после выдачи нового значения в ЦАП или Коммутация канала
				}else
				{
					{
						fData = Dac11.fDAC_New[i];
							
						if(fData>MaxValueDac[i])	fData = MaxValueDac[i];
						if(fData<MinValueDac[i])	fData = MinValueDac[i];
						if(fData != Dac11.fDAC_Set[i])
						{
							Dac11.fDAC_Set[i] = fData;
							WriteNormalDataDac(i, fData);
							setTimer(&Dac11.TimerAin, PeriodTimeoutADC);
						}
					}
				}
			}
		}
		//---------------------------------------------
		// собираем все ошибки
		Dac11.ErrorDAC[0] = 0;
		for(i=0;i<COUNT_DAC_CH;i++)
		{
			st = 0;
			if(digit(Dac11.ErrorUP, i)   == 1) { Dac11.ErrorDAC[0]|= ((DWORD)ERROR_KZ)<<(2*i); st = 1;}
			if(digit(Dac11.ErrorDOWN, i) == 1) { Dac11.ErrorDAC[0]|= ((DWORD)ERROR_ZERO)<<(2*i); st = 2;}
			if(digit(Dac11.DiagMAX, i)   == 0) { Dac11.ErrorDAC[0]|= ((DWORD)ERROR_MAX)<<(2*i); st = 3;}
			if( st == 0) ClearErrorDAC(0, i);

		}
		//=============================================================================================
		// Опрашиваем дискретные входы/ определяем состояние выходных реле
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
				if(Spi_Buf_In[0] == SETTING_7) SETBIT(Dac11.DiagMAX, i); else CLEARBIT(Dac11.DiagMAX, i);
			}
			Dac11.StNeigborData[0]	= TRUE;
			Dac11.PingDataBlock[0]	= 0;
		}		
		//===================================================
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
		CreateAndSend_Pkt_UART0((U8 *)(&Dac11.fDAC_Set), 176, 2, 1);
	}
	if(Dac11.SendPakTar == TRUE)
	{
		Dac11.SendPakTar = FALSE;
		CreateAndSend_Pkt_UART0((U8 *)(&TarRam), COUNT_DAC_CH*4*2*2, 2, 2);
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

	Len;
	
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
			TarRam[i].k	= pTar[i].k;
			TarRam[i].ofs	= pTar[i].ofs;
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
		
		Dac11.NewOutDac = *pW;
	}
}
//========================================================================
BYTE GetErrorDAC(BYTE ch, BYTE n)
{
	DWORD Err;
	
	Err = Dac11.ErrorDAC[ch];
	Err = Err>>(2*n);
	Err = Err&3;
	
	return (BYTE)Err;
} 
void ClearErrorDAC(BYTE ch, BYTE n)
{
	DWORD t=3;
	Dac11.ErrorDAC[ch] &= ~(t<<(2*n));
}
//========================================================================
BYTE Max3(BYTE a, BYTE b, BYTE c)
{	
	if ((a>b) && (a>c)) return 1;
	if ((b>a) && (b>c)) return 2;
	if ((c>a) && (c>b)) return 3;
	return 0;
}
	
// Процедура по выбору мастера из 3х ЦАПов
static BYTE	StartFindMaster = 0;
void SelectMasterDAC(void)
{
	BYTE	i, addr, n, err, err1, err2;
	Message	msg;
	//WORD	wData;
	//========================================================
	// разрешена запись тарировок считаем что мы в стенде спб
	if(Dac11.WriteTar == 1)
	{
		Dac11.Master[0] = 0xFFF;
		StartFindMaster = 0;
		return;
	}
	//========================================================
	// если тарировки не прописаны разрешаем замыкание реле
	if(Dac11.TarrStatus == false)	
	{
		Dac11.Master[0] = 0xFFF;
		StartFindMaster = 0;
		return;
	}
	//========================================================
	// Если нет связи с УСО выходим 
	if((getTimer(&program.TimerCan1)==0)&&(getTimer(&program.TimerCan2)==0))
	{
		Dac11.Master[0] = 0;
		StartFindMaster = 0;
		return;
	}
	//========================================================
	// Если блок не проинициализирован УСО выходим 
	if(getState() != Operational)
	{
		Dac11.Master[0] = 0;
		StartFindMaster = 0;
		return;
	}
	//========================================================
	if(getTimer(&Dac11.TimerPingNeighbor) == 0) // пришло время выдать пинг соседям
	{
		setTimer(&Dac11.TimerPingNeighbor, PeriodPing);
		StartFindMaster++;
		
		for(i=1; i<3; i++)
		{
			Dac11.PingBlock[i]++;
			if(Dac11.PingBlock[i] >= MAX_COUNT_PING_LOST)
			{
				Dac11.PingBlock[i] = MAX_COUNT_PING_LOST;
				Dac11.StNeigbor[i] = FALSE;
				Dac11.Master[i] = 0;
				for(n=0; n<8; n++)
					Dac11.SerN[i][n] = 0;
			}
			//--------------------------------------------
			Dac11.PingDataBlock[i]++;
			if(Dac11.PingDataBlock[i] >= MAX_COUNT_PING_LOST)
			{
				Dac11.PingDataBlock[i] = MAX_COUNT_PING_LOST;
				Dac11.StNeigborData[i] = FALSE;
				Dac11.Master[i] = 0;
			}
		}
		
		msg.cob_id	= KF_TO_COB_ID(((WORD)0x3F))|ADDR_NODE;
			
		msg.rtr 	= 0;
		msg.len 	= 8;
		
		if(program.Cnt1WareDev > 0)
		{
			for(i=0; i<8; i++) msg.data[i] = program.SN_1Ware_Dev[0][i];
		}else
		{
			for(i=0; i<8; i++) msg.data[i] = 0;
		}
				
		CAN_SendMessage(NUM_CAN_FOR_SELECT_MASTER, &msg);
		//===============================================================
		// назначем адреса блокам
		if(StartFindMaster >= 3)
		{
			Dac11.AddrBl[0] = 0;
			Dac11.AddrBl[1] = 0;
			Dac11.AddrBl[2] = 0;
			
			if((Dac11.StNeigbor[1]==TRUE)||(Dac11.StNeigbor[2] == TRUE)) // если мы видим хотябы одного соседа назначаем адреса
			{
				if(Dac11.StNeigbor[0] == TRUE) // если мы знаем свой серийник то учавствуем назначении приоритета
				{
					if((Dac11.StNeigbor[1] == TRUE)&&(Dac11.StNeigbor[2] == TRUE)) // оба соседа видны
					{
						addr = 2;
						for(i=1; i<8; i++)
						{
							if(addr == 2)
							{
								n = Max3(Dac11.SerN[0][i], Dac11.SerN[1][i], Dac11.SerN[2][i]);
								if(n>0)
								{
									Dac11.AddrBl[n-1] = 0;
									addr = 1;
								}
							}
							if(addr == 1)
							{
								if(n == 1)
								{
									if(Dac11.SerN[1][i]> Dac11.SerN[2][i])
									{
										Dac11.AddrBl[1] = 1;
										Dac11.AddrBl[2] = 2;
										addr = 0;
										break;
									}else
									{
										if(Dac11.SerN[1][i]< Dac11.SerN[2][i])
										{
											Dac11.AddrBl[1] = 2;
											Dac11.AddrBl[2] = 1;
											addr = 0;
											break;
										}	
									}
								}
								if(n == 2)
								{
									if(Dac11.SerN[0][i]> Dac11.SerN[2][i])
									{
										Dac11.AddrBl[0] = 1;
										Dac11.AddrBl[2] = 2;
										addr = 0;
										break;
									}else
									{
										if(Dac11.SerN[0][i] < Dac11.SerN[2][i])
										{
											Dac11.AddrBl[0] = 2;
											Dac11.AddrBl[2] = 1;
											addr = 0;
											break;
										}
									}
								}
								if(n == 3)
								{
									if(Dac11.SerN[0][i]> Dac11.SerN[1][i])
									{
										Dac11.AddrBl[0] = 1;
										Dac11.AddrBl[1] = 2;
										addr = 0;
										break;
									}else
									{
										if(Dac11.SerN[0][i] < Dac11.SerN[1][i])
										{
											Dac11.AddrBl[0] = 2;
											Dac11.AddrBl[1] = 1;
											addr = 0;
											break;
										}	
									}
								}
							}
						}
					}else
					{// виден только один сосед
						if(Dac11.StNeigbor[1] == FALSE) Dac11.AddrBl[1] = 2; else n = 1;
						if(Dac11.StNeigbor[2] == FALSE) Dac11.AddrBl[2] = 2; else n = 2;
					
						for(i=1; i<8; i++)
						{
							if(Dac11.SerN[0][i]> Dac11.SerN[n][i])
							{
								Dac11.AddrBl[0] = 0;
								Dac11.AddrBl[n] = 1;
								break;
							}else
							{
								if(Dac11.SerN[0][i]< Dac11.SerN[n][i])
								{
									Dac11.AddrBl[0] = 1;
									Dac11.AddrBl[n] = 0;
									addr = 0;
									break;
								}	
							}
						}
					}
				}else
				{// если видны соседи а наш серийник не определился то мы рулим в последнюю очередь
					Dac11.AddrBl[0] = 2;
					Dac11.AddrBl[1] = 1;
					Dac11.AddrBl[2] = 0;
				}
			}else
			{// мы никого невидим значит будем рулить сами
				Dac11.AddrBl[0] = 0;
				Dac11.AddrBl[1] = 1;
				Dac11.AddrBl[2] = 2;
			}
			//==============================================================
			// отправляем данные о своем состоянии (реле, ошибки)
			if((Dac11.AddrBl[0]+Dac11.AddrBl[1]+Dac11.AddrBl[2])>0)	// если расставелны адреса блокам
			{
				msg.cob_id	= KF_TO_COB_ID(((WORD)0x3E))|ADDR_NODE;
				msg.rtr 	= 0;
				msg.len 	= 8;
				
				msg.data[0] = Dac11.AddrBl[0];
				
				msg.data[1] = Dac11.ErrorDAC[0];
				msg.data[2] = Dac11.ErrorDAC[0]>>8;
				msg.data[3] = Dac11.ErrorDAC[0]>>16;
				
				msg.data[4] = Dac11.OutDac[0];
				msg.data[5] = Dac11.OutDac[0]>>8;		
				
				msg.data[6] = Dac11.Master[0];
				msg.data[7] = Dac11.Master[0]>>8;		
				CAN_SendMessage(NUM_CAN_FOR_SELECT_MASTER, &msg);
			}
			//==============================================================
			//==============================================================
			// захват управления каналами
			if(StartFindMaster >= 6)
			{
				StartFindMaster = 6;
				
				for(i=0; i<COUNT_DAC_CH; i++)
				{
					if(digit(Dac11.Master[0], i) == 0) // если мы не управляем данным каналом, проверим можноли захватить уравление
					{
						//Если ни один из каналов который на связи не захватил управление данным каналом
						if(digit(Dac11.Master[1], i) == 0)
						{
							if(digit(Dac11.Master[2], i) == 0)
							{
								err = GetErrorDAC(0,i);	// Запрос ошибки выхода
								if(err == ERROR_OK)		// у нас канал рабочий
								{
									if(Dac11.AddrBl[0] == 0) // если мы мастер забираем канал без вопросов
									{
										SETBIT(Dac11.Master[0], i);
									}else
									{ // если мы не мастер забираем только по приоритету, если у высоко приоритетного ошибка
										err1 = GetErrorDAC(1,i); // Запрос ошибки выхода
										err2 = GetErrorDAC(2,i); // Запрос ошибки выхода
										
										if((Dac11.StNeigborData[1] == TRUE)&&(err1 == ERROR_OK)&&(Dac11.AddrBl[1]<Dac11.AddrBl[0]))
										{// блок 1 имеет больше преимуществ на захват канала
											
										}else 
										{
											if((Dac11.StNeigborData[2] == TRUE)&&(err1 == ERROR_OK)&&(Dac11.AddrBl[2]<Dac11.AddrBl[0]))
											{// блок 2 имеет больше преимуществ на захват канала
											
											}else 
											{ // Мы можем забрать управление каналом
												SETBIT(Dac11.Master[0], i);
											}
										}
									}
								}else // есть ошибки 
								{
									if(err == ERROR_ZERO) // проседание тока не критично может нет питания
									{
										//если мы управляем каналом отдаем управление
										//если только по остальным каналам нет ошибок любых
										err1 = GetErrorDAC(1,i); // Запрос ошибки выхода
										err2 = GetErrorDAC(2,i); // Запрос ошибки выхода
										
										// если есть блок с более высоким приоритетом который который на связи с такой же ошибкой забирает он
										if((err1 >= ERROR_ZERO)&&(err2 >= ERROR_ZERO))
										{
											if((Dac11.AddrBl[0]<Dac11.AddrBl[1])&&(Dac11.AddrBl[0]<Dac11.AddrBl[2]))
												SETBIT(Dac11.Master[0], i);
										}
									}
									if(err == ERROR_KZ) // превышение тока критичная ошибка канал не забираем
									{
										
									}
									if(err == ERROR_MAX) // не видна микросхема соответсвенно не забираем управление
									{
										
									}
								}
							}else 
							{	// канал захвачен блоком 2 
							}
						}else
						{	// канал захвачен блоком 1
						}
					}else // мы управляем данным каналом
					{
						err = GetErrorDAC(0,i); // Запрос ошибки выхода
						if(err == ERROR_ZERO)	// проседание тока не критично может нет питания
						{						// проверяем если у других блоков данный канал работает отдаем управление	
							err1 = GetErrorDAC(1,i); // Запрос ошибки выхода
							err2 = GetErrorDAC(2,i); // Запрос ошибки выхода
							if(((Dac11.StNeigborData[1] == TRUE)&&(err1 == 0))||(((Dac11.StNeigborData[2] == TRUE)&&(err2 == 0))))
							{// если хотябы один блок на связи и исправен то управление отдаем
								CLEARBIT(Dac11.Master[0], i);
							}	
						}	
						if(err == ERROR_KZ) // превышение тока критичная ошибка ОТДАЕМ УПРАВЛЕНИЕ В любом случае
						{
							CLEARBIT(Dac11.Master[0], i);
						}
						if(err == ERROR_MAX) // ПРОПАЛА  связь с микросхемой отдаем управление
						{
							CLEARBIT(Dac11.Master[0], i);
						}
						
						// если есть блок с более высоким приоритетом который скомутировал реле отключаем свое
						if(digit(Dac11.Master[2], i) == 1)
						{
							if(Dac11.AddrBl[2] < Dac11.AddrBl[0]) CLEARBIT(Dac11.Master[0], i);
						}
						// если есть блок с более высоким приоритетом который скомутировал реле отключаем свое
						if(digit(Dac11.Master[1], i) == 1)
						{
							if(Dac11.AddrBl[1] < Dac11.AddrBl[0]) CLEARBIT(Dac11.Master[0], i);
						}
					}
				}
			}
		}	
		//===============================================================
	}
}
//========================================================================
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


#endif