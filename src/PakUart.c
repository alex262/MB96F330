#include "fifo.h" 

#include INCLUDE_H


//--------------------------------------------------------------------------------
#define TEST_UART_FIFO_RX_SIZE	128
#define TEST_UART_FIFO_TX_SIZE	256

#define nOurID 1

static BYTE fifo_rx_buf[TEST_UART_FIFO_RX_SIZE];
static BYTE fifo_tx_buf[TEST_UART_FIFO_TX_SIZE];

static fifo_desc_t fifo_rx_desk;
static fifo_desc_t fifo_tx_desk;

static BYTE sSend0Empty = TRUE;

//=======================================================================
void InitServiceUart0(void)
{
	fifo_init(&fifo_rx_desk, &fifo_rx_buf, TEST_UART_FIFO_RX_SIZE);
	fifo_flush(&fifo_rx_desk);
	
	fifo_init(&fifo_tx_desk, &fifo_tx_buf, TEST_UART_FIFO_TX_SIZE);
	fifo_flush(&fifo_rx_desk);
	
	UART_RX_IntSet(0, TRUE);
	sSend0Empty = TRUE;
}
//=======================================================================
void IRQ_RX0(BYTE data)
{
	fifo_push_uint8(&fifo_rx_desk, data);
}
#pragma inline IRQ_RX0
//=======================================================================
BYTE GetRxByte(BYTE * data)
{
	return fifo_pull_uint8(&fifo_rx_desk, data);
}
#pragma inline GetRxByte
//=======================================================================
BYTE IRQ_TX0(void)
{
	BYTE data;
	
	if(fifo_pull_uint8(&fifo_tx_desk, &data) == FIFO_OK)
	{
		TDR0 = data;	// put ch into buffer
		return TRUE;
	}
	else
	{
		sSend0Empty = TRUE;
		/* Disable transmit interrupts */
		UART_TX_IntSet(0, FALSE);
		return FALSE;
	}
}
//=======================================================================
void StartSend0(void)
{
	BYTE data;
	if(sSend0Empty == TRUE)
	{
		if(fifo_pull_uint8(&fifo_tx_desk, &data) == FIFO_OK)
		{
			TDR0 = data;	// put ch into buffer
			sSend0Empty = FALSE;
			UART_TX_IntSet(0, TRUE); // start send
		}
	}
}
//=======================================================================
void AddDataToSend0(BYTE * pData, WORD Len)
{
	while(Len>0)
	{
		DisInterrupt();
		if(fifo_push_uint8(&fifo_tx_desk, *pData) == FIFO_OK)
		{
			pData++;
			Len--;
		}
		EnInterrupt();
		clrwdt;
	}
	
	/*while(Len>0)
	{
		DisInterrupt();
		if(sSend0Empty == TRUE)
		{
			TDR0 = (*pData);
			pData++;
			sSend0Empty = FALSE;
			Len--;
		}else
		{
			if(fifo_push_uint8(&fifo_tx_desk, *pData) == FIFO_OK)
			{
				pData++;
				Len--;
			}
			//clrwdt;
		}
		UART_TX_IntSet(0, TRUE);
		EnInterrupt();
	}*/
}
//=======================================================================
WORD Calc(BYTE H,BYTE L)
{
	WORD   N = 0;
	N = (H << 8) + L;
	return N;
}
#pragma inline Calc
//=======================================================================
WORD CRC( BYTE *sBuf, WORD nLen )
{
	WORD i,CRC = 0;

	for(i = 0; i < nLen; i++ )
	{
		CRC += sBuf[i];
	}
	return CRC;
}
#pragma inline CRC
//=======================================================================
BYTE tx_pak_uart[6];
void CreateAndSend_Pkt_UART0(U8 *sDataBuf, U16 nLen, U8 nReceiver, U8 nCodOp)
{
	U16 nCRC;
		
	nCRC=nReceiver+(U8)((nLen)&0x00FF)+(U8)(((nLen)&0xFF00)>>8);
	nCRC+=nCodOp+nOurID+1;
	
	tx_pak_uart[0] = 0x81;
	tx_pak_uart[1] = nReceiver;			
	tx_pak_uart[2] = nOurID;			
	tx_pak_uart[3] = (U8)((nLen+1)&0x00FF);	
	tx_pak_uart[4] = (U8)(((nLen+1)&0xFF00)>>8);
	tx_pak_uart[5] = nCodOp;					
	
	AddDataToSend0(tx_pak_uart, 6);
	AddDataToSend0(sDataBuf, nLen);
	
	nCRC += CRC(sDataBuf,nLen);

	tx_pak_uart[0] = (U8)(nCRC&0x00FF);      
	tx_pak_uart[1] = (U8)((nCRC&0xFF00)>>8);
	tx_pak_uart[2] = 0x81;						
	tx_pak_uart[3] = 0x0;	

	AddDataToSend0(tx_pak_uart, 4);
	StartSend0();
}
//=======================================================================

//----------------------------------------------------------------------------------------------
// поиск орионовксих пакетов в буфере
void GetPak_Uart(WORD * CountByte, BYTE * pData)
{
	WORD i,nDataLen,nLen;
	//------------------------------Обрабатываются входящие пакеты------------------------	
	i=0;
	if((*CountByte)>4)
	{
		while(i<=(*CountByte))
		{
		//	clr_wdt;
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
				nLen=nDataLen+9;
				if(nLen<MAX_IN_PAK)
				//Слишком большая длина пакета
				{
					if(nLen>(BUFFER_LEN_UART-1))	//Если пакет не помещяется в оставшейся чачти буфера
					{							//перетаскиваем пакет к началу
						(*CountByte)= 0;
						break;
					}
					if(nLen>(*CountByte)) break;
					//Если пришёл ещё не весь пакет выходим
					if(pData[nLen-2]==0x81 && pData[nLen-1]==0x0)
					//Ищем конец пакета
					{
						if(CRC(pData+1,nLen-5)==Calc(pData[nLen-3],pData[nLen-4]))
						//Проверяем контрольную сумму
						{
							//Проверяем код операции
							switch(pData[5])
							{
								//=======================================================================================
								case 0xFE:	// запрос SN
								{	
									CreateAndSend_Pkt_UART0(&program.Cnt1WareDev, 1+8*program.Cnt1WareDev, nOurID, 0xFE);
									//---------------------
									(*CountByte)-= nLen;
									memmove(pData,pData+nLen,BUFFER_LEN_UART-nLen);
									break;
								}
								//=======================================================================================
								case 0xFD:// Read/Write EEPROM
								{	
	
									program.RW_EEPROM	= pData[6];
									program.Addr		= pData[7];
									program.Addr		|= pData[8]<<8;
									program.Len			= pData[9];
									program.Len			|= pData[10]<<8;
									
									if(program.RW_EEPROM == 1) // запись
										memmove(program.BufEEPROM, &pData[11], program.Len);
									
									program.StEEPROM = 1;
									//---------------------
									(*CountByte)-= nLen;
									memmove(pData,pData+nLen,BUFFER_LEN_UART-nLen);
									break;
								}
								//=======================================================================================
								//=======================================================================================
								default:	//пришёл неизвестный идентификатор
								{
									if(SERVICE_PAK_UART!=(void *)(NULL))	// обработка пакетов блоками
										SERVICE_PAK_UART(pData[5], &pData[6], nDataLen);
						
									(*CountByte)-= nLen;
									memmove(pData,pData+nLen,BUFFER_LEN_UART-nLen);
									break;
								}
							}
						//-----------------------------------------------------------
						}else
						//Контрольная сумма не совпадает
						{
							memmove(pData,pData+i+nLen,pData-nLen);
							(*CountByte) -= nLen;
							break;
						}
					}else
					//Нашли не начало пакета
					{
						memmove(pData, pData+1, (*CountByte));
						(*CountByte) -= 1;
						break;
					}
				}else 
				{//Пришла слишком большая длина пакет затираем символ 0x81
					memmove(pData, pData+1, (*CountByte));
					(*CountByte) -= 1;
					break;
				}
			}
			i++;
			if(((*CountByte)>=(BUFFER_LEN_UART-2)) && (i>=(BUFFER_LEN_UART-1)))
				(*CountByte)=0;
		}
	}
}
/*void Send_Pkt(BYTE nCH,BYTE *sDataBuf,WORD nLen,BYTE nReceiver,BYTE nCodOp)
{
	WORD nCRC,i;
	
	nCRC=nReceiver+(BYTE)((nLen)&0x00FF)+(BYTE)(((nLen)&0xFF00)>>8);
	nCRC+=nCodOp+nOurID+1;
	
	Putch_N(nCH,0x81);				//				[0]
	Putch_N(nCH,nReceiver);			//Кому			[1]
	Putch_N(nCH,nOurID);			//Наш адрес		[2]
	Putch_N(nCH,(BYTE)((nLen+1)&0x00FF));	//		[3]          
	Putch_N(nCH,(BYTE)(((nLen+1)&0xFF00)>>8));//	[4]   
    Putch_N(nCH,nCodOp);					//		[5]	

	for(i=0;i<nLen; i++)
	{
		Putch_N(nCH,sDataBuf[i]);			//		[6]
		clrwdt;
	}
	nCRC += CRC(sDataBuf,nLen);

	Putch_N(nCH,(BYTE)(nCRC&0x00FF));         	//	[nLen+6]
	Putch_N(nCH,(BYTE)((nCRC&0xFF00)>>8));  	//	[nLen+7]
	Putch_N(nCH,0x81);							//	[nLen+8]
	Putch_N(nCH,0x0);							//	[nLen+9]
}
	//====================================
	putch(0x81);
	putch(nReceiver);			
	putch(nOurID);			
	putch((U8)((nLen+1)&0x00FF));	
	putch((U8)(((nLen+1)&0xFF00)>>8));
	putch(nCodOp);					
	
	nCRC += CRC(sDataBuf,nLen);
	while(nLen)
	{
		putch(*sDataBuf);
		nLen--;
		sDataBuf++;
	}
	
	
	putch((U8)(nCRC&0x00FF));
	putch((U8)((nCRC&0xFF00)>>8));
	putch(0x81);						
	putch(0x0);	*/
//=======================================================================
