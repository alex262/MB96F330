#ifndef _PAKUART_H_
#define _PAKUART_H_

#include "global.h" 
							
	

void InitServiceUart0(void);

void IRQ_RX0(BYTE data);
BYTE IRQ_TX0(void);
BYTE GetRxByte(BYTE * data);

void AddDataToSend0(BYTE * pData, WORD Len);
void GetPak_Uart(WORD * CountByte, BYTE * pData);
void CreateAndSend_Pkt_UART0(U8 *sDataBuf, U16 nLen, U8 nReceiver, U8 nCodOp);


#endif