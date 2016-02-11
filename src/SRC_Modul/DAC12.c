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

#ifdef PLATA_DAC12
#include "DAC12.h"

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

CDAC12 Dac12;

void 	(*INIT_BLOK)(void)	=InitDAC12;
void 	(*DRIVER_BLOK)(void)=DriverDAC12;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD (*STATE_BLOCK) = &Dac12.Info.word;

//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================
static BYTE BuffUart[BUFFER_LEN_UART];
static WORD CountDataUart = 0;
TPAK_SPI    pak_spi;
TPAK_SPI    tmp_pak_spi;
static BYTE TarEd=0;	// флаг единичных тарировок
//====================================================================
const float   MIN_Regim[NUM_REGIM_DAC]={  0,   0, -10.0};//ћинимальные значени€ дл€ соответствующих диапазонов
const float   MAX_Regim[NUM_REGIM_DAC]={5.0,20.0,  10.0};//ћаксимальные значени€ дл€ соответствующих диапазонов

//                     1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16                    
const BYTE  OUT_V[16]={7, 5, 3, 1,31,29,27,25,23,21,19,17,15,13,11,9};//сопоставление каналов
const BYTE  OUT_R[16]={6, 4, 2, 0,30,28,26,24,22,20,18,16,14,12,10,8};//сопоставление каналов

TTar TarEEPROM[2][NUM_REGIM_DAC][COUNT_DAC_CH];		// тарировки каналов в EEPROM
TTar TarRam[2][NUM_REGIM_DAC][COUNT_DAC_CH];		// тарировки каналов в ќ«”
//====================================================================
void WritePakSpi(void);
void WriteControlRegister(WORD RG);
void ReadDiagn(void);
void WriteNormalDataDac(BYTE ch, float data);
//====================================================================
BYTE ServiceMaster(BYTE bus_id, Message *m)
{
	m;
	bus_id;
	return 0;
}
BYTE ServiceObmenData1(BYTE bus_id, Message *m)
{
	m;
	bus_id;
	return 0;
}
BYTE ServiceObmenData2(BYTE bus_id, Message *m)
{
	m;
	bus_id;
	return 0;
}
//====================================================================
void InitDAC12()
{
	BYTE i;
	
	DDR00	= 0xFF;
	DDR01	= 0;
	DDR02	= 0;
	DDR05	= 0x08;
	DDR06	= 0xFF;
	DDR07	= 0x0;
	DDR14	= 0x0;
	DDR11	= 0;
	DDR10_D0= 1;
	DDR17	= 0;
	DDR17_D4= 1;
	
	PIER01	= 0xFF;	
	PIER02	= 0xFF;	
	PIER05_IE1 = 1;
	PIER05_IE5 = 1;
	PIER07	= 0xFF;	
	PIER11	= 0xFF;	
	PIER14	= 0xFF;	
	PIER17	= 0xFF;	
	//------------------------------------
	OD=0;
	CNC		= CNC_CAN_EN;
	CNC_DIR	= PIN_DIR_OUT;
	//------------------------------------
	for(i= 0; i<COUNT_DAC_CH; i++)
	{
		Dac12.fDAC_New[i] = 0;
		Dac12.fDAC_Set[i] = 0;
		Dac12.SetRegim[i] = 0;
		Dac12.NewRegim[i] = 0;
	}
	Dac12.WriteTar = 0;
	Dac12.Info.word = 0;
	add_timer(&TimerStartBlock);

	Dac12.SendPak	= FALSE;

	InitSPI_5();
}
//====================================================================
//-------------------------------------
void DriverDAC12()
{
	BYTE j,i, k;
	BYTE *pData, st;
	
	//------------------------------------------------
	// обновление адреса блока
	Dac12.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			//del_timer(&TimerStartBlock);
			stStartBlock = TRUE;
			//========================================================
			// „итаем тарировки из EEPROM в RAM
			SetWorkChI2C(1);
			HighDensSequentialRead(0, (BYTE*)(&TarRam), sizeof(TarRam));
			pData = (BYTE*)(&TarRam);
			st =0;
			
			for(k=0; k<2; k++)
			{
				for(i=0; i<NUM_REGIM_DAC; i++)
				{
					for(j=0; j<COUNT_DAC_CH; j++)
					{
						clrwdt;
						if((check_NaN_Inf(TarRam[k][i][j].k)) == TRUE)
						{
							TarRam[k][i][j].k = 1.0;
							st = 1;
						}
						if(check_NaN_Inf(TarRam[k][i][j].ofs) == TRUE)
						{
							TarRam[k][i][j].ofs = 0.0;
							st = 1;
						}
						TarEEPROM[k][i][j].k	= TarRam[k][i][j].k;
						TarEEPROM[k][i][j].ofs	= TarRam[k][i][j].ofs;
						if(TarEEPROM[k][i][j].k == 1.0)	TarEd = 1;
					}
				}
			}
			if(st == 1)
			{
				//puts("Write trarrirovki!!!");
				HighDensPageWrite(0, (BYTE*)(&TarRam), sizeof(TarRam));
			}
			//------------------------------------
			// ¬ыставл€ем опорное напр€жение
			WriteControlRegister(0x100);    // опора 1.5¬
			//========================================================
		}
	}
	//========================================================
	if(stStartBlock == TRUE)
	{
		//==============================================
		ReadDiagn();
		//ReadADC();
		if(getTimer(&TimerStartBlock) == 0)	
		{
			setTimer(&TimerStartBlock,10);
					
			for(i=0;i<COUNT_DAC_CH;i++)
			{
				if(Dac12.NewRegim[i]>=NUM_REGIM_DAC) Dac12.NewRegim[i] = Dac12.SetRegim[i];
				if(TarEd  == 0)
				{
					if(Dac12.fDAC_New[i]<MIN_Regim[Dac12.NewRegim[i]])	Dac12.fDAC_New[i] = MIN_Regim[Dac12.NewRegim[i]];
					if(Dac12.fDAC_New[i]>MAX_Regim[Dac12.NewRegim[i]])	Dac12.fDAC_New[i] = MAX_Regim[Dac12.NewRegim[i]];
				}
					
				if((Dac12.fDAC_New[i] != Dac12.fDAC_Set[i])||(Dac12.SetRegim[i]!=Dac12.NewRegim[i]))
				{
					Dac12.SetRegim[i]=Dac12.NewRegim[i];
					Dac12.fDAC_Set[i]=Dac12.fDAC_New[i];
					WriteNormalDataDac(i, Dac12.fDAC_Set[i]);
					OD=1;
				}
			}
		}
		//==============================================
		if(Dac12.WriteTar == 1)
		{
			st = 0;
			for(k=0; k<2; k++)
			{
				for(i=0; i<NUM_REGIM_DAC; i++)
				{
					for(j=0; j<COUNT_DAC_CH; j++)
					{
						clrwdt;
						if(TarEEPROM[k][i][j].k	!= TarRam[k][i][j].k)
						{
							st = 1;
							TarEEPROM[k][i][j].k	= TarRam[k][i][j].k;
						}
						if(TarEEPROM[k][i][j].ofs	!= TarRam[k][i][j].ofs)
						{
							st = 1;
							TarEEPROM[k][i][j].ofs = TarRam[k][i][j].ofs;
						}
					}
				}
			}
			if(st == 1)
			{
				SetWorkChI2C(1);
				HighDensPageWrite(0, (BYTE*)(&TarEEPROM), sizeof(TarRam));
			}
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
	
	if(Dac12.SendPak == TRUE)
	{
		Dac12.SendPak	= FALSE;
		
		CreateAndSend_Pkt_UART0((BYTE *)(&Dac12.fDAC_Set[0]), 5+COUNT_DAC_CH+COUNT_DAC_CH*4, 2, 1);
	}
#endif
	//----------------------------------------------------
	// работа с eeprom 
	if(program.StEEPROM_Write == TRUE)	// были записаны новые данные в EEprom
	{
		program.StEEPROM_Write = FALSE;
		// „итаем тарировки из EEPROM в RAM
		SetWorkChI2C(1);
		HighDensSequentialRead(0, (BYTE*)(&TarRam), sizeof(TarRam));
		pData = (BYTE*)(&TarRam);
		st =0;
			
		for(k=0; k<2; k++)
		{
			for(i=0; i<NUM_REGIM_DAC; i++)
			{
				for(j=0; j<COUNT_DAC_CH; j++)
				{
					clrwdt;
					if((check_NaN_Inf(TarRam[k][i][j].k)) == TRUE)
					{
						TarRam[k][i][j].k = 1.0;
						st = 1;
					}
					if(check_NaN_Inf(TarRam[k][i][j].ofs) == TRUE)
					{
						TarRam[k][i][j].ofs = 0.0;
						st = 1;
					}
					TarEEPROM[k][i][j].k	= TarRam[k][i][j].k;
					TarEEPROM[k][i][j].ofs	= TarRam[k][i][j].ofs;
				}
			}
		}
	}
}
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	float * pF;
	Len;
	if(Id == 0x01)
	{	
		Dac12.SendPak = TRUE;
		return;
	}
	if(Id == 0x04)//Write
	{
		if(pData[0]<16)
		{
			pF = (float *)(&pData[1]);
			Dac12.fDAC_New[pData[0]] =pF[0]; 
		}
		return;
	}
	if(Id == 0x03)//regim
	{
		if((pData[0]<16)&&(pData[1]<3))
		{
			Dac12.NewRegim[pData[0]] = pData[1];
		}
		return;
	}

}
//=========================================================================================
void WriteControlRegister(WORD RG)
{
	RG&=0xFFF;
	
	pak_spi.data[0]=0;
	pak_spi.data[1]=0;
	pak_spi.data[2]=0;
	
	pak_spi.bits.DB =RG;
	pak_spi.bits.A =0xC;
	WritePakSpi();
}
//================================================
//читаем диагностику
void ReadDiagn(void)
{
	Dac12.Diagn[0] = EOT;
	Dac12.Diagn[0] |= PG<<1;
	
	Dac12.Diagn[1] = ELD_1_8;		// перегрузка по току или обрыв 0 - error
	Dac12.Diagn[2] = ELD_9_16;	
	Dac12.Diagn[3] = ECM_1_8;		// превышение диапазона  0 - error
	Dac12.Diagn[4] = ECM_9_16;		
}
//================================================
void WriteDac(BYTE nCH,WORD data)
{
    if(nCH>31) return;
    
    pak_spi.data[0]=0;
    pak_spi.data[1]=0;
    pak_spi.data[2]=0;
    
    pak_spi.bits.DB =data;
    pak_spi.bits.A  =nCH;
    pak_spi.bits.REG =3;
    
    WritePakSpi();
}
//============================================
//==========================================================
// ¬ыставл€ем режим дл€ канала
// 0 - Vout 1 - Iout
void SetRegimForDac(BYTE ch, BYTE Regim)
{
    Regim=Regim&1;
    
    if(ch<8)
    {
        if(Regim==0)
            CVI_1_8 = CVI_1_8 & (~(1<<ch));
        if(Regim==1)
            CVI_1_8 = CVI_1_8 | (1<<ch);
    }else
    {
        ch=ch-8;
        if(Regim==0)
            CVI_9_16 = CVI_9_16 & (~(1<<ch));
        if(Regim==1)
            CVI_9_16 = CVI_9_16 | (1<<ch);
    }
}
//==========================================================
void WriteNormalDataDac(BYTE ch, float data)
{
    float k_V,ofs_V,k_R,ofs_R,res_V,res_R;
    WORD out_V, out_R;
    BYTE r; 

	if(Dac12.SetRegim[ch] >= (NUM_REGIM_DAC))return;
	r = Dac12.SetRegim[ch];
    k_V   =TarRam[0][r][ch].k;
    ofs_V =TarRam[0][r][ch].ofs;
    k_R   =TarRam[1][r][ch].k;
    ofs_R =TarRam[1][r][ch].ofs;
    
	if(Dac12.SetRegim[ch] == 2) r = 0; else r = 1;
    
    res_V=k_V*data+ofs_V;
    res_R=k_R*data+ofs_R;
    
    
    if(res_V>MAX_COD_DAC) res_V=(float)MAX_COD_DAC;
    if(res_V<MIN_COD_DAC) res_V=(float)MIN_COD_DAC;
    if(res_R>MAX_COD_DAC) res_R=(float)MAX_COD_DAC;
    if(res_R<MIN_COD_DAC) res_R=(float)MIN_COD_DAC;
    
    out_V=(WORD)res_V;
    out_R=(WORD)res_R;
    
    WriteDac(OUT_V[ch],out_V);
    WriteDac(OUT_R[ch],out_R);
    //WriteDac(OUT_R[ch],2047);
    SetRegimForDac(ch, r);
    LOADN;
}
//==========================================================
void WritePakSpi(void)
{
	BYTE i;
	
	SYNCN	= 0;
	
	while (SSR5_TDRE == 0);		
	TDR5 = pak_spi.data[2];	
	
	while (SSR5_TDRE == 0);		
	TDR5 = pak_spi.data[1];	
	
	while (SSR5_TDRE == 0);		
	TDR5 = pak_spi.data[0];	
	
	while (ECCR5 & 0x01);    // ожидаем начала передачи
	while (!(ECCR5 & 0x01)); // ожидаем завершени€ передачи .. делаетс€ дл€ того чтобы при выходе из процедуры можно было бросать чипселект
	i = RDR5;				
	SCR5_CRE = 1;			// Clear possible errors, reset reception state machine
	
	SYNCN	= 1;
}
//=========================================================================================
BYTE SPI_Read(BYTE Data)
{
	BYTE din;
	
	TDR5 = Data;
	
	while (SSR5_RDRF == 0);
	din = RDR9;

	SCR5_CRE = 1;			// Clear possible errors, reset reception state machine
	
	return din;
}
//=========================================================================================

#endif;
