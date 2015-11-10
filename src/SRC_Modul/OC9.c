#include "mb96338us.h"
#include "timer.h"
#include "appli.h"
#include "global.h"
#include "uart.h"
#include "pakuart.h"
#include "fifo.h"
#include "dma.h"

#ifdef PLATA_OC9
#include "OC9.h"
//====================================================================

COC9 OC9;
//====================================================================

const char SoftwareVer[20] = { __TIME__" " __DATE__}; 

//====================================================================
void	(*INIT_BLOK)(void)	=InitOC9;
void	(*DRIVER_BLOK)(void)=DriverOC9;
void	(*SERVICE_PAK_UART)(BYTE, BYTE*, WORD) = ServiceUart;

WORD	(*STATE_BLOCK)		= &OC9.Info.word;
//====================================================================
void AddDataToSendUart(BYTE ch, BYTE * pData, WORD Len);
//void StartSendUart(BYTE ch);
void InitExtUart(BYTE ch, WORD div);
BYTE ReadExtUartData(BYTE ch, BYTE * Data);
void SendExtUartData(BYTE ch, BYTE Data);
BYTE TX_EmptyExtUart(BYTE ch);
BYTE ReadIIR_ExtUart(BYTE ch);
BYTE ReadLSR_ExtUart(BYTE ch);
BYTE GetPakUartOC9(WORD * CountByte, BYTE * pData);
WORD GetPakID(BYTE * pData);
void DelPak(WORD * CountByte, BYTE * pData);
WORD GetLenPak(BYTE * pData);

void CreatePkt(U8 ch, U8 *sDataBuf, U16 nLen, U8 nReceiver, U8 nCodOp);

//====================================================================
static TYPE_DATA_TIMER TimerStartBlock = 1000/TIMER_RESOLUTION_MS;
static BYTE stStartBlock = FALSE;
//====================================================================
static DWORD ADDR_MEM_EXT_UART[4] = {START_ADDR_MEM_CS0, START_ADDR_MEM_CS1, START_ADDR_MEM_CS2, START_ADDR_MEM_CS3};
//====================================================================
//                                      0  1  2  3  4                   
static BYTE NumChUart[COUNT_UART_MK] = {2, 5, 9, 1, 3}; // соответствие каналов блока, каналам микроконтроллера, 0 - канал для связи с МП
//                              0, 1, 2, 3, 4, 5, 6, 7, 8, 9
static BYTE NumChUartInv[10] = {0, 3, 0, 4, 0, 1, 0, 0, 0, 2}; // соответствие каналов микроконтроллера, каналам блока
static BYTE NumChDMA_RX[5]   = {0, 1, 2, 3, 4}; // соответствие каналов UART каналам DMA приема
static BYTE NumChDMA_TX[5]   = {5, 6, 7, 8, 9}; // соответствие каналов UART каналам DMA передачи

#define TIMEOUT_TX_UART	10	// ожидание освобождения буфера 

// FIFO буфер драйвера 
//static BYTE BufferFifoUartRX[COUNT_UART_MK+COUNT_UART_EXT][SIZE_FIFO_BUFFER_UART_RX_OC9];
//static BYTE BufferFifoUartTX[COUNT_UART_MK+COUNT_UART_EXT][SIZE_FIFO_BUFFER_UART_TX_OC9];

// буфер для поиска принятых пакетов
static BYTE BufferUartRX[COUNT_UART_MK+COUNT_UART_EXT][SIZE_BUFFER_UART_RX_OC9];
//!static BYTE BufferUartTX[COUNT_UART_MK+COUNT_UART_EXT][SIZE_BUFFER_UART_TX_OC9];

//static fifo_desc_t fifo_rx_desk_oc[COUNT_UART_MK+COUNT_UART_EXT];
//static fifo_desc_t fifo_tx_desk_oc[COUNT_UART_MK+COUNT_UART_EXT];

static BYTE sSendEmptyUartTX[COUNT_UART_MK+COUNT_UART_EXT];
static WORD CountDataUart[COUNT_UART_MK+COUNT_UART_EXT];
static WORD OverflowDataUartRX[COUNT_UART_MK+COUNT_UART_EXT];
static WORD OverflowDataUartTX[COUNT_UART_MK+COUNT_UART_EXT];
//====================================================================
static TDmaRxItems DmaRxItems[DMA_COUNT_BUFF_TX];			// структура для работы с пприемным буфером	
static TDmaTxItems DmaTxItems[DMA_COUNT_BUFF_TX];			// структура для работы с передающим буфером

static BYTE DMA_BUFF_TX[DMA_COUNT_BUFF_TX][DMA_SIZE_BUFF_TX];// передающий буфер	
static BYTE DMA_BUFF_RX[DMA_COUNT_BUFF_RX][DMA_SIZE_BUFF_RX];// приемный буфер	

void DMA_RestartRxBuff(BYTE nCH);
void DMA_InitRxBuff(BYTE nCH, DWORD AddrBuff, WORD SizeBuff);
void DMA_InitTxBuff(BYTE nCH, DWORD AddrBuff, WORD SizeBuff);
WORD DMA_GetCountRxBuff(BYTE nCH);
WORD DMA_GetDataRxBuff(BYTE nCH, BYTE * pData, WORD Count);
BYTE DMA_ServiceIrqTX(BYTE nCH) ;
void DMA_ServiceTX(BYTE nCH); 
BYTE DMA_AddDataToTxBuff(BYTE nCH, BYTE *pData, WORD Size, WORD TimeOut);
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
/*void IrqExtUart(BYTE ch)
{
	BYTE Data, i, iir;
	if(ch>=COUNT_UART_EXT) return;
	
	CNC2 = 1;
	
	if(fifo_is_empty(&fifo_tx_desk_oc[COUNT_UART_MK+ch]) == FALSE)// есть данные на отправку
	{
		if(TX_EmptyExtUart(ch)) // входной буфер UART пуст
		{ //можно добавить 64 байта
			i = 0;
			while(1)
			{
				if(fifo_pull_uint8(&fifo_tx_desk_oc[COUNT_UART_MK+ch], &Data) == FIFO_OK)
				{
					i++;
					SendExtUartData(ch, Data);
					if(i>=64) break;
				}else
				{ // больше данных нет
					sSendEmptyUartTX[COUNT_UART_MK+ch] = TRUE;
					iir = ReadIIR_ExtUart(ch);
					break;
				}
			}
		}
	}else
	{ // больше данных нет
		sSendEmptyUartTX[COUNT_UART_MK+ch] = TRUE;
		iir = ReadIIR_ExtUart(ch);
	}
	while(ReadExtUartData(ch, &Data))	// забираем все данные
	{
		if(fifo_push_uint8(&fifo_rx_desk_oc[COUNT_UART_MK+ch], Data) == FIFO_ERROR_OVERFLOW)
		{
			if(OverflowDataUartRX[COUNT_UART_MK+ch]<0xFFFF)
				OverflowDataUartRX[COUNT_UART_MK+ch]++;	
		}
	}
	CNC2 = 0;
}*/
__interrupt void IRQHandler_EI1 (void)
{
	if (EIRR0_ER1)			// check valid interrupt
	{
		EIRR0_ER1 = 0;
	//	IrqExtUart(0);
	}
}
__interrupt void IRQHandler_EI2 (void)
{
	if (EIRR0_ER2)			// check valid interrupt
	{
		EIRR0_ER2 = 0;
		//IrqExtUart(1);
	}
}
__interrupt void IRQHandler_EI3 (void)
{
	if (EIRR0_ER3)			// check valid interrupt
	{
		EIRR0_ER3 = 0;
		//IrqExtUart(2);
	}
}
__interrupt void IRQHandler_EI4 (void)
{
	if (EIRR0_ER4)			// check valid interrupt
	{
		EIRR0_ER4 = 0;
		//IrqExtUart(3);
	}
}

//====================================================================
void InitOC9()
{
	BYTE i;
	DmaConfig_t DmaConfig;
	//DDR00	= 0xFF;
	//DDR01	= 0x00;
	//DDR02	= 0x0;
	DDR03	= 0x6;
	DDR06	= 0x18;
	DDR07	= 0x0;
	DDR09	= 0xF1;
	DDR13	= 0x0;
	DDR14	= 0x0;
	DDR15	= 0xC;
	DDR17	= 0x0;
	DE = 1;
	RE = 0;

	CNC2 = 0;
	CNC2_DIR = 1;

	//------------------------------------
	// настраиваем внешние прерывания
	ADER0 = 0;
	ADER1 = 0;
	ADER2 = 0;
	ADER3 = 0;
	
	
	PIER07_IE1 = 1;		// Enable Port 07_1 Input
	PIER07_IE2 = 1;		// Enable Port 07_2 Input
	PIER07_IE3 = 1;		// Enable Port 07_3 Input
	PIER07_IE4 = 1;		// Enable Port 07_4 Input

	ELVR0_LB1 = 1;		// LB0, LA0 = Falling edge
	ELVR0_LA1 = 0;
	EIRR0_ER1 = 0;		// reset interrupt request
	ENIR0_EN1 = 1;		// enable interrupt request
	
	ELVR0_LB2 = 1;		// LB0, LA0 = Falling edge
	ELVR0_LA2 = 0;
	EIRR0_ER2 = 0;		// reset interrupt request
	ENIR0_EN2 = 1;		// enable interrupt request

	ELVR0_LB3 = 1;		// LB0, LA0 = Falling edge
	ELVR0_LA3 = 0;
	EIRR0_ER3 = 0;		// reset interrupt request
	ENIR0_EN3 = 1;		// enable interrupt request

	ELVR0_LB4 = 1;		// LB0, LA0 = Falling edge
	ELVR0_LA4 = 0;
	EIRR0_ER4 = 0;		// reset interrupt request
	ENIR0_EN4 = 1;		// enable interrupt request
	//------------------------------------
	for(i=0; i<DMA_COUNT_BUFF_RX; i++)
	{
		DMA_InitRxBuff(i, (DWORD)(DMA_BUFF_RX[i]), DMA_SIZE_BUFF_RX);
	
		DmaConfig.ch	= NumChDMA_RX[i];				// номер канала DMA 0..10
		DmaConfig.nIRQ	= GetNumRxIRQUart(NumChUart[i]);// Номер прерывания на которое привязывается данный канал DMA 
		DmaConfig.adrIOA= GetUartRDR_addr(NumChUart[i]);// адрес регистра ввода вывода
		DmaConfig.adrBAP= (DWORD)(DMA_BUFF_RX[i]);		// адрес буфера
		DmaConfig.Count	= DMA_SIZE_BUFF_RX;				// количество пересылаемых символов
		DmaConfig.BPD 	= DMA_BPD_INC;					// имеет значение при BF=0, 0 - инкремент BAP, 1 - декремент BAP
		DmaConfig.IF 	= DMA_IF_IOA_NOT_INCR;			// 	0 - инкремент IOA песле каждой передачи
		DmaConfig.BW 	= DMA_LEN_BYTE;					// размер передаваемых данных 0 - byte 1-word
		DmaConfig.BF 	= DMA_BF_BAP_CHANGE;			// 0 - изменение BAP после каждой передачи(см. BPD) 1 - BAP неменяется
		DmaConfig.DIR 	= DMA_IOA_TO_BAP;				// направление передачи, 0 - IOA->BAP, 1 - BAP->IOA
		DmaConfig.SE 	= DMA_SE_IGNOR;					// реакция на сигнал DMA STOP; 0 - нет реакции, 1 - передача останавливается

		InitDAM_ch(&DmaConfig);
		DMA_Enable(NumChDMA_RX[i]);
	}
	for(i=0; i<DMA_COUNT_BUFF_TX; i++)
	{
		DMA_InitTxBuff(i, (DWORD)(DMA_BUFF_TX[i]), DMA_SIZE_BUFF_TX);

		DmaConfig.ch	= NumChDMA_TX[i];				// номер канала DMA 0..10
		DmaConfig.nIRQ	= GetNumTxIRQUart(NumChUart[i]);// Номер прерывания на которое привязывается данный канал DMA 
		DmaConfig.adrIOA= GetUartTDR_addr(NumChUart[i]);// адрес регистра ввода вывода
		DmaConfig.adrBAP= (DWORD)(DMA_BUFF_TX[i]);		// адрес буфера
		DmaConfig.Count	= 0;							// количество пересылаемых символов
		DmaConfig.BPD 	= DMA_BPD_INC;					// имеет значение при BF=0, 0 - инкремент BAP, 1 - декремент BAP
		DmaConfig.IF 	= DMA_IF_IOA_NOT_INCR;			// 	0 - инкремент IOA песле каждой передачи
		DmaConfig.BW 	= DMA_LEN_BYTE;					// размер передаваемых данных 0 - byte 1-word
		DmaConfig.BF 	= DMA_BF_BAP_CHANGE;			// 0 - изменение BAP после каждой передачи(см. BPD) 1 - BAP неменяется
		DmaConfig.DIR 	= DMA_BAP_TO_IOA;				// направление передачи, 0 - IOA->BAP, 1 - BAP->IOA
		DmaConfig.SE 	= DMA_SE_IGNOR;					// реакция на сигнал DMA STOP; 0 - нет реакции, 1 - передача останавливается

		InitDAM_ch(&DmaConfig);
	}
	
	for(i=0; i<(COUNT_UART_MK+COUNT_UART_EXT); i++)
	{
		// инициализация буферов
		//fifo_init(&fifo_rx_desk_oc[i], &BufferFifoUartRX[i], SIZE_FIFO_BUFFER_UART_RX_OC9);
		//fifo_flush(&fifo_rx_desk_oc[i]);
		//fifo_init(&fifo_tx_desk_oc[i], &BufferFifoUartTX[i], SIZE_FIFO_BUFFER_UART_TX_OC9);
		//fifo_flush(&fifo_tx_desk_oc[i]);
		
		sSendEmptyUartTX[i]	= TRUE;
		CountDataUart[i]	= 0;
		OverflowDataUartRX[i] = 0;
		OverflowDataUartTX[i] = 0;
		
		if(i<COUNT_UART_MK)	
		{// инициализируем UART микроконтроллера 
			InitUART(NumChUart[i]);		
			SetUartESIR_Reg(NumChUart[i], 1);// Disable USART automatic interrupt clear, clear TDRE and RDRF flags(for DMA)
			SetUartSMR_UPCL(NumChUart[i], 1); // set AICD via UART reset for DMA
			UART_RX_IntSet(NumChUart[i], TRUE);
		}else
		{// инициализируем внешние UART
			InitExtUart(i-COUNT_UART_MK, DIV_BAUD_EXT_UART);
		}
	}
	OC9.Info.word = 0;
	add_timer(&TimerStartBlock);
	
	OC9.stRecvPak	= FALSE;
	OC9.SendPak		= FALSE;
}
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
BYTE Test[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
void DriverOC9()
{
	BYTE i;
	WORD wData;
	WORD wLen, wLen_free;
	//------------------------------------------------
	// обновление адреса блока
	OC9.Info.bits.Addr	=ADDR;
	if(stStartBlock == FALSE)
	{
		if(getTimer(&TimerStartBlock) == 0)	
		{
			stStartBlock = TRUE;
		}
	}else
	{
		//if(getTimer(&TimerStartBlock) == 0)	
		//	setTimer(&TimerStartBlock,20);
		//AddDataToSendUart(0, Test, 16);
		//StartSendUart(0);
		//		CreatePkt(1, Test, 6, 1, 2);
		//		StartSendUart(1);
		//========================================================
		for(i=0; i<COUNT_UART_MK; i++)	DMA_ServiceTX(i);	
					
		if(getTimer(&TimerStartBlock) == 0)	
		{
			setTimer(&TimerStartBlock,5);
			for(i=0; i<(COUNT_UART_MK+COUNT_UART_EXT); i++)
			{
				if(i<COUNT_UART_MK)
				{
					wLen = DMA_GetCountRxBuff(i);
					wLen_free = SIZE_BUFFER_UART_RX_OC9-CountDataUart[i];
					if(wLen>wLen_free)	wLen=wLen_free;
					
					if(wLen>0)
					{
						CountDataUart[i]+=DMA_GetDataRxBuff(i, &BufferUartRX[i][CountDataUart[i]], wLen);
					}
				}
			}
			//==========================================================
			// Ишем пакеты в принятых каналах
			for(i=0; i<(COUNT_UART_MK+COUNT_UART_EXT); i++)
			{
				if(GetPakUartOC9(&CountDataUart[i], BufferUartRX[i]) == TRUE) // Нашли Орионовский пакет
				{
					if(i == MASTER_CH_UART) // пришол пакет из мастер канала (от МП)
					{
						wData = GetPakID(BufferUartRX[i]);
						wData = (wData>>4)&0xF;	// здесь номер канала в который нужно отправить
						if(wData<8) // номер канала указан верно
						{
							//wData++; // так как 0 это наш мастер канал
							// перекидываем данные в исходящий fifo буфер
							wLen = GetLenPak(BufferUartRX[i]);
/*TEST*/					//DMA_AddDataToTxBuff(MASTER_CH_UART, BufferUartRX[i], wLen, TIMEOUT_TX_UART);
/*WORK*/					DMA_AddDataToTxBuff(wData, BufferUartRX[i], wLen, TIMEOUT_TX_UART);
						}else // не корректный номер канала
						{
							
						}
						DelPak(&CountDataUart[i], BufferUartRX[i]);
					}else
					{ //пакет перенаправляем в мастер канал (0, в МП)
						wLen = GetLenPak(BufferUartRX[i]);
/*Для Патрина*/			BufferUartRX[i][5] = (i<<4) | (BufferUartRX[i][5]&0xF); //добавляем номер канала
						//AddDataToSendUart(MASTER_CH_UART, BufferUartRX[i], wLen);
						DMA_AddDataToTxBuff(MASTER_CH_UART, BufferUartRX[i], wLen, TIMEOUT_TX_UART);
						DelPak(&CountDataUart[i], BufferUartRX[i]);	
					}
				}
			}
			//==========================================================	
		}
	}
}
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//*******************************************************************************************************************
//  Добавляем данные в исходящий буфер fifo
/*void AddDataToSendUart(BYTE ch, BYTE * pData, WORD Len)
{
	ch;
	while(Len>0)
	{
		DisInterrupt();
		//if(fifo_push_uint8(&fifo_tx_desk_oc[ch], *pData) == FIFO_OK)
		{
			pData++;
			Len--;
		}
		EnInterrupt();
		clrwdt;
	}
}*/
void CreatePkt(U8 ch, U8 *sDataBuf, U16 nLen, U8 nReceiver, U8 nCodOp)
{
	
	BYTE tx_pak_uart[6];
	U16 nCRC;
	U8 nOurID=1;
		
	nCRC=nReceiver+(U8)((nLen)&0x00FF)+(U8)(((nLen)&0xFF00)>>8);
	nCRC+=nCodOp+nOurID+1;
	
	tx_pak_uart[0] = 0x81;
	tx_pak_uart[1] = nReceiver;			
	tx_pak_uart[2] = nOurID;			
	tx_pak_uart[3] = (U8)((nLen+1)&0x00FF);	
	tx_pak_uart[4] = (U8)(((nLen+1)&0xFF00)>>8);
	tx_pak_uart[5] = nCodOp;					
	
	//AddDataToSendUart(ch,tx_pak_uart, 6);
	DMA_AddDataToTxBuff(ch, tx_pak_uart, 6, 10);
	//AddDataToSendUart(ch,sDataBuf, nLen);
	DMA_AddDataToTxBuff(ch, sDataBuf, nLen, 10);
	

	nCRC += CRC(sDataBuf,nLen);

	tx_pak_uart[0] = (U8)(nCRC&0x00FF);      
	tx_pak_uart[1] = (U8)((nCRC&0xFF00)>>8);
	tx_pak_uart[2] = 0x81;						
	tx_pak_uart[3] = 0x0;	

	//AddDataToSendUart(ch,tx_pak_uart, 4);
	DMA_AddDataToTxBuff(ch, tx_pak_uart, 4, 10);
}

//====================================================================
/*void StartSendUart(BYTE ch)
{
	BYTE data, N;
	if(ch>=(COUNT_UART_MK+COUNT_UART_EXT))return;
	
	DisInterrupt();
	if(sSendEmptyUartTX[ch] == TRUE)
	{
		if(fifo_pull_uint8(&fifo_tx_desk_oc[ch], &data) == FIFO_OK)
		{
			if(ch<COUNT_UART_MK)
			{	// UART микроконтроллера 
				N = NumChUart[ch];
				SetUartTRD_Reg(N, data);	// put ch into buffer
				sSendEmptyUartTX[ch] = FALSE;
				UART_TX_IntSet(N, TRUE); // start send
			}else
			{ // внешние UART //  4<ch<9 (5, 6, 7, 8)
				sSendEmptyUartTX[ch] = FALSE;
				SendExtUartData(ch-COUNT_UART_MK, data);
			}
		}
	}
	EnInterrupt();
}*/
//====================================================================
BYTE IRQ_TX_OC9(BYTE ch)
{
	BYTE N;
	
	N = NumChUartInv[ch];
	
	UART_TX_IntSet(ch, FALSE);
	
	DMA_ServiceIrqTX(N);
	return TRUE;
}
void IRQ_RX_OC9(BYTE ch, BYTE data)
{
	//UartESIR_RDRF_Clear(ch);
	data;
	DMA_RestartRxBuff(NumChUartInv[ch]); // переназначаем буфер DMA, и запускаем повторно
}
#pragma inline IRQ_RX_OC9
//====================================================================
void ServiceUart(BYTE Id, BYTE* pData, WORD Len)
{
	pData;
	Len;
	
	if(Id == 0x01)
	{	

		return;
	}
}
//====================================================================
WORD GetPakID(BYTE * pData)
{
	return 	pData[5];
}
//====================================================================
void DelPak(WORD * CountByte, BYTE * pData)
{
	WORD nLen;

	nLen=pData[3]+(WORD)(pData[4]<<8)+11;
	if(nLen<=(*CountByte))
	{
		(*CountByte)-= nLen;
		memmove(pData,pData+nLen,(*CountByte));
	}
}
//====================================================================
WORD GetLenPak(BYTE * pData)
{
	WORD nLen;

	nLen=pData[3]+(WORD)(pData[4]<<8)+11;
	
	return nLen;
}
//====================================================================
DWORD crc_temp;
BYTE GetPakUartOC9(WORD * CountByte, BYTE * pData)
{
	WORD i,nDataLen,nLen;
	DWORD *pDW;
	//------------------------------Обрабатываются входящие пакеты------------------------	
	i=0;
	if((*CountByte)>4)
	{
		while(i<=(*CountByte))
		{
			if((pData[i]==0x81))// && (program.Pack_IN_[i+1]!=0))
			//Ишем начало пакета
			{
				//-Перетаскиваем 81 к начaлу буфера
				(*CountByte)-= i;
				memmove(pData,pData+i,(*CountByte));
				i=0;
				if((*CountByte)<5) break;
				
				//если длина ещё непришла выход
				nDataLen=pData[3]+(WORD)(pData[4]<<8);
				nLen=nDataLen+11; 
				if(nLen<MAX_IN_PAK_OC9)
				//Слишком большая длина пакета
				{
					if(nLen>(SIZE_BUFFER_UART_RX_OC9-1))	//Если пакет не помещяется в оставшейся чачти буфера
					{							//перетаскиваем пакет к началу
						(*CountByte)= 0;
						break;
					}
					if(nLen>(*CountByte)) break;
					//Если пришёл ещё не весь пакет выходим
					if(pData[nLen-2]==0x81 && pData[nLen-1]==0x0)
					//Ищем конец пакета
					{
						//if(CRC(pData+1,nLen-5)==Calc(pData[nLen-3],pData[nLen-4]))
						pDW = (DWORD *)(&pData[nDataLen+5]);
						//if(crc_temp == (*pDW))
						if(Crc32Sum_Check(pData+1,nDataLen+4,(*pDW)))
						//Проверяем контрольную сумму
						{
							return TRUE; // нашли пакет
						}else
						//Контрольная сумма не совпадает
						{
							(*CountByte) -= nLen;
							memmove(pData,pData+nLen,(*CountByte));
							break;
						}
					}else
					//Нашли не начало пакета
					{
						(*CountByte) -= 1;
						memmove(pData, pData+1, (*CountByte));
						break;
					}
				}else 
				{//Пришла слишком большая длина пакет затираем символ 0x81
					(*CountByte) -= 1;
					memmove(pData, pData+1, (*CountByte));
					break;
				}
			}
			i++;
		}
		if(((*CountByte)>=(SIZE_BUFFER_UART_RX_OC9-2)) && (i>=(SIZE_BUFFER_UART_RX_OC9-1)))	// буфер забит пакета нет
			(*CountByte)=0;
	}
	return FALSE;
}
//====================================================================
//====================================================================
 // init 16c754
#define RHR 0X00	
#define THR 0X00	
#define IER 0X01	
#define FCR 0X02	
#define IIR 0X02	
#define EFR 0X02	
#define LCR 0X03	
#define MCR 0X04	
#define LSR 0X05	
#define MSR 0X06	
#define SPR 0X07	
#define DLL 0X00
#define DLM 0X01

#define LCR_WORD_LEN5 	0X00
#define LCR_WORD_LEN6 	0X01
#define LCR_WORD_LEN7 	0X02
#define LCR_WORD_LEN8 	0X03
#define LCR_STOP_LEN1 	0X00
#define LCR_STOP_LEN2 	0X04
#define LCR_PARITY_EN 	0X08
#define LCR_PARITY_EVEN 0X10
#define LCR_PARITY_SET 	0X20
#define LCR_OUT_BREAK 	0X40
#define LCR_DIV_EN 		0X80
#define LCR_DIV_Dis		0X00

#define MCR_INTX_EN 	0X08
#define MCR_INTX_Dis 	0X00

#define IER_MODEM_INT 			0X08
#define IER_RECEIVELINE_INT 	0X04
#define IER_TRANSMITHOLD_INT 	0X02
#define IER_RECEIVEHOLD_INT 	0X01

#define FCR_DMA_MODE1	0x08
#define FCR_RST_TX_FIFO	0x04
#define FCR_RST_RX_FIFO	0x02
#define FCR_ENABLE_FIFO 0x01

#define LSR_DATA_RX		0x01
#define LSR_THR_EMPTY	0x20

#define MCR_LOOP_EN		0x10

//---------------------------------------
BYTE ReadIIR_ExtUart(BYTE ch)
{
	__far BYTE *pMem;
	BYTE reg;
	
	if(ch>3) exit;
	
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];
	reg = *(pMem+IIR);
	
	return reg;
}
BYTE ReadLSR_ExtUart(BYTE ch)
{
	__far BYTE *pMem;
	BYTE reg;
	
	if(ch>3) exit;
	
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];
	reg = *(pMem+LSR);
	
	return reg;
}

void InitExtUart(BYTE ch, WORD div)
{
	__far BYTE *pMem;
	
	if(ch>3) exit;
	
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];
	
	*(pMem+LCR) = LCR_DIV_EN;	/* Enable divisor access */
	*(pMem+DLL) = div;			/* set divisor     */
	*(pMem+DLM) = div>>8;
	*(pMem+LCR) = LCR_WORD_LEN8 | LCR_STOP_LEN1;     /* Set 8,N,1 data        */
	*(pMem+FCR) = FCR_DMA_MODE1 | FCR_RST_TX_FIFO | FCR_RST_RX_FIFO | FCR_ENABLE_FIFO;     /* Enable and clear FIFOs, Rcv thresh=60  */
	*(pMem+IER) = IER_TRANSMITHOLD_INT|IER_RECEIVEHOLD_INT;	
}
//---------------------------------------
BYTE TX_EmptyExtUart(BYTE ch)
{
	BYTE reg;
	__far BYTE *pMem;
	if(ch>3) return FALSE;
	
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];

	reg = *(pMem+LSR);
	if (reg & LSR_THR_EMPTY ) return TRUE;
	else return FALSE;
}
BYTE ReadExtUartData(BYTE ch, BYTE * Data)
{
	__far BYTE *pMem;
	BYTE c;
	
	if(ch>3) return FALSE;
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];
	
	c = *(pMem+LSR);
	if (c & LSR_DATA_RX )
	{
		*Data = *(pMem+RHR);
		return TRUE;
	}
	return FALSE;
}
//---------------------------------------
void SendExtUartData(BYTE ch, BYTE Data)
{
	__far BYTE *pMem;
	
	if(ch>3) return;
	pMem = (__far BYTE *)ADDR_MEM_EXT_UART[ch];

	*(pMem+THR) = Data;
}	
//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************
//**********************************************************************************************************************
//			D M A
//**********************************************************************************************************************
// nCH - 0,1,2,3,4
WORD DMA_GetDataRxBuff(BYTE nCH, BYTE * pData, WORD Count)
{
	WORD i=0;
	if(nCH >= DMA_COUNT_BUFF_RX) return 0;
	
	for(i=0; i<Count; i++)
	{
		pData[i] = DMA_BUFF_RX[nCH][DMA_SIZE_BUFF_RX - DmaRxItems[nCH].Start];
		DmaRxItems[nCH].Start--;
		if(DmaRxItems[nCH].Start == 0) 
		{
			DmaRxItems[nCH].Start = DMA_SIZE_BUFF_RX;
			DmaRxItems[nCH].Over = 0;
		}
	}
	return i;
}
//---------------------------------------------------------------------------
// nCH - 0,1,2,3,4
WORD DMA_GetCountRxBuff(BYTE nCH)// возвращает количество принятых байт в RX буфер DMA 
{
	WORD cnt, get;
	if(nCH >= DMA_COUNT_BUFF_RX) return 0;
	
	cnt = DMA_GetCnt(nCH);
	
	if(DmaRxItems[nCH].Start == cnt) return 0;
	
	if(DmaRxItems[nCH].Over)
	{
		get = DmaRxItems[nCH].SizeBuff-cnt+DmaRxItems[nCH].Start;
	}else
	{
		get = DmaRxItems[nCH].Start - cnt;
	}
	
	return get;
}
//---------------------------------------------------------------------------
// nCH - 0,1,2,3,4
void DMA_RestartRxBuff(BYTE nCH)// функция предназначена для рестарта  буфера в DMA
{
	if(nCH >= DMA_COUNT_BUFF_RX) return;
	
	DmaRxItems[nCH].Over = 1;
	
	DMA_change_BAP_restart(nCH,(DWORD)(&DMA_BUFF_RX[nCH][0]),DMA_SIZE_BUFF_RX);
}
//---------------------------------------------------------------------------
// nCH - 0,1,2,3,4
void DMA_InitRxBuff(BYTE nCH, DWORD AddrBuff, WORD SizeBuff)
{
	if(nCH >= DMA_COUNT_BUFF_RX) return;
	
	DisInterrupt();
	DmaRxItems[nCH].AddrBuff = AddrBuff;
	DmaRxItems[nCH].SizeBuff = SizeBuff;
	DmaRxItems[nCH].Start = SizeBuff;
	DmaRxItems[nCH].Over = 0;
	EnInterrupt();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// nCH - 0,1,2,3,4
void DMA_InitTxBuff(BYTE nCH, DWORD AddrBuff, WORD SizeBuff)
{
	DisInterrupt();
	DmaTxItems[nCH].AddrBuff = AddrBuff;
	DmaTxItems[nCH].SizeBuff = SizeBuff;
	DmaTxItems[nCH].Start	 = 0;
	DmaTxItems[nCH].Stop	 = 0;
	DmaTxItems[nCH].SizeDMA	 = 0;
	DmaTxItems[nCH].Run		 = FALSE;
	EnInterrupt();
}
//---------------------------------------------------------------------------
// процедура вызывается в прерывании для назначения нового буфера передачи если есть в очереди
// nCH - 0,1,2,3,4
BYTE DMA_ServiceIrqTX(BYTE nCH) 
{
	if(nCH>=DMA_COUNT_BUFF_TX) return FALSE;
	
	DMA_ClearRequest(NumChDMA_TX[nCH]);
	
	__wait_nop();
	__wait_nop();
	
	DmaTxItems[nCH].Start += DmaTxItems[nCH].SizeDMA;		
	if(DmaTxItems[nCH].Start >= DMA_SIZE_BUFF_TX) DmaTxItems[nCH].Start = 0;

	DmaTxItems[nCH].Run = FALSE;
}
//---------------------------------------------------------------------------
// функция отслеживает наличие данных в буфере и инициирует прередач резрешением прерывания
// nCH - 0,1,2,3,4
void DMA_ServiceTX(BYTE nCH) 
{
	if(nCH>=DMA_COUNT_BUFF_TX) return;
		
	if(DmaTxItems[nCH].Run == FALSE) // если сервис DMA закончил передачю или неактивен
	{
		if(DmaTxItems[nCH].Start == DmaTxItems[nCH].Stop) return;	// нет данных на отправку

		DisInterrupt();
		
		UART_ESIR_SSR_TDRE(NumChUart[nCH]);
		
		DmaTxItems[nCH].Run = TRUE;
		
		if(DmaTxItems[nCH].Start < DmaTxItems[nCH].Stop) 
		{
			DmaTxItems[nCH].SizeDMA = DmaTxItems[nCH].Stop-DmaTxItems[nCH].Start;
			DMA_change_BAP_restart(NumChDMA_TX[nCH], (DmaTxItems[nCH].AddrBuff+DmaTxItems[nCH].Start), DmaTxItems[nCH].SizeDMA);// установили адрес // запустили передачю
			UART_TX_IntSet(NumChUart[nCH], TRUE);
		}else
		{
			DmaTxItems[nCH].SizeDMA = DMA_SIZE_BUFF_TX-DmaTxItems[nCH].Start;
			DMA_change_BAP_restart(NumChDMA_TX[nCH], (DmaTxItems[nCH].AddrBuff+DmaTxItems[nCH].Start), DmaTxItems[nCH].SizeDMA);// установили адрес // запустили передачю
			UART_TX_IntSet(NumChUart[nCH], TRUE);
		}
		EnInterrupt();
	}
}
//---------------------------------------------------------------------------
// TimeOut - время в мс которое ожидаем освобождения буфера
// nCH - 0,1,2,3,4
BYTE DMA_AddDataToTxBuff(BYTE nCH, BYTE *pData, WORD Size, WORD TimeOut)
{
	WORD nStop;
	
	if(nCH >= DMA_COUNT_BUFF_TX) return FALSE;
	if(Size>DMA_SIZE_BUFF_TX) return FALSE; // не можем поместить данные в буфер
	
	if(DmaTxItems[nCH].Run == FALSE)	// DMA передачи неактивен
	{
		if(DmaTxItems[nCH].Start == DmaTxItems[nCH].Stop) // буфер пуст
		{
			DmaTxItems[nCH].Stop = 0;		// сбрасываем счетчики на начало буфера
			DmaTxItems[nCH].Start= 0;
		}
	}
	
	while(Size)	
	{
		nStop = DmaTxItems[nCH].Stop+1;
		if(nStop >= DMA_SIZE_BUFF_TX) nStop = 0;
		if(nStop == DmaTxItems[nCH].Start) // буфер заполнен нет места
		{
			if(TimeOut>0) // если установлен таймаут ждем освобождения буфера
			{
				TimeOut--;
				msDelay(1);
			}else return FALSE;
		}else
		{ // есть место кладем данные
			DMA_BUFF_TX[nCH][DmaTxItems[nCH].Stop] = *pData;
			pData++;
			Size--;
			DisInterrupt();
			DmaTxItems[nCH].Stop++;
			if(DmaTxItems[nCH].Stop >= DMA_SIZE_BUFF_TX) DmaTxItems[nCH].Stop = 0;
			EnInterrupt();
		}
	}
	return TRUE;
}
//**********************************************************************************************************************
#endif;
