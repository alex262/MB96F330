#include "mb96338us.h"
#include "can.h"
#include "uart.h"
#include "def.h"
//-----------------------------------------------------------------------------
void CAN_ConfigMsgBox(U8 NumCan);
U8 CAN0_SendMessage(U8 msgBox, Message *msg);
U8 CAN1_SendMessage(U8 msgBox, Message *msg);
U8 CAN2_SendMessage(U8 msgBox, Message *msg);

//-----------------------------------------------------------------------------
volatile static Message			stackMsgSend[NB_LINE_CAN][MAX_STACK_MSG_TX]; 
volatile static t_pointerStack	ptrMsgSend[NB_LINE_CAN];
 
volatile static Message			stackMsgRcv[NB_LINE_CAN][MAX_STACK_MSG_RX]; 
volatile static t_pointerStack	ptrMsgRcv[NB_LINE_CAN];

BYTE stInitCAN[NB_LINE_CAN];

/**************************************************************************/
WORD GetCountTX_MSG(U8 nCAN)
{
	if(nCAN>(NB_LINE_CAN-1)) return	0;
	return ptrMsgSend[nCAN].count;
}
WORD GetCountRX_MSG(U8 nCAN)
{
	if(nCAN>(NB_LINE_CAN-1)) return	0;
	return ptrMsgRcv[nCAN].count;
}

void ClearTX_BUFF(U8 nCAN)
{
	if(nCAN>(NB_LINE_CAN-1)) return;
	DisInterrupt();
	ptrMsgSend[nCAN].st=0;
	ptrMsgSend[nCAN].end=0;
	ptrMsgSend[nCAN].count=0;
	EnInterrupt();
}
void ClearRX_BUFF(U8 nCAN)
{ 
	if(nCAN>(NB_LINE_CAN-1)) return;
	DisInterrupt();
	ptrMsgRcv[nCAN].st=0;
	ptrMsgRcv[nCAN].end=0;
	ptrMsgRcv[nCAN].count=0;
	EnInterrupt();
}
void InitCanFifoBuf(void)
{
	BYTE i;
	
	for(i=0;i< NB_LINE_CAN;	i++)
	{
		ptrMsgSend[i].st=0;
		ptrMsgSend[i].end=0;
		ptrMsgSend[i].count=0;
		
		ptrMsgRcv[i].st=0;
		ptrMsgRcv[i].end=0;
		ptrMsgRcv[i].count=0;
	}
}
//-----------------------------------------------------------------------------
void SendFifoCanMsg(U8 nCAN)
{ 
	Message *pM;
	U8	ret;
	
	if(nCAN>(NB_LINE_CAN-1)) return;
	
	/* See if	a message is pending in	the	stack */
	if (ptrMsgSend[nCAN].count ==	0 )	return;
	
	pM = (Message	*)&stackMsgSend[nCAN][ptrMsgSend[nCAN].st];
	
	ret = FALSE;
	
	if(nCAN == 0) ret =  CAN0_SendMessage(31,pM);
	if(nCAN == 1) ret =  CAN1_SendMessage(31,pM);
	if(nCAN == 2) ret =  CAN2_SendMessage(31,pM);
	
	if(ret == TRUE)
	{
		ptrMsgSend[nCAN].st++;
		if (ptrMsgSend[nCAN].st == MAX_STACK_MSG_TX )	
			ptrMsgSend[nCAN].st = 0;
		ptrMsgSend[nCAN].count--;
	}
}

//-----------------------------------------------------------------------------
void CAN_init(U8 NumCan, U16 BaudRate)
{
	switch (NumCan)
	{
		case 0:
			COER0_OE	= 1;					// enable CAN0 output
			PIER10_IE0	= 1;					// enable CAN0 input
			CTRLR0_INIT	= 1;					// Stop CAN Operation
			// Setup CAN channel Configuration
			CTRLR0_CCE	= 1;					// Enable Configuration Change
			CTRLR0_IE	= 1;					// 0: Disable / 1: Enable Module Interrupt
			CTRLR0_SIE	= 0;					// 0: Disable / 1: Enable status change interrupt
			CTRLR0_EIE	= 1;					// 0: Disable / 1: Enable error interrupt
			BTR0 = BaudRate;					// <<< set bit timing
			CTRLR0_CCE	= 0;					// Disable Configuration Change
			CTRLR0_INIT	= 0;					// Start CAN Operation
			
			CNC1	= CNC_CAN_EN;
			CNC1_DIR= PIN_DIR_OUT;
		break;
		case 1: 
			COER1_OE	= 1;					// enable CAN1 output
			PIER04_IE2	= 1;					// enable CAN1 input
			CTRLR1_INIT	= 1;					// Stop CAN Operation
			// Setup CAN channel Configuration
			CTRLR1_CCE	= 1;					// Enable Configuration Change
			CTRLR1_IE	= 1;					// 0: Disable / 1: Enable Module Interrupt
			CTRLR1_SIE	= 0;					// 0: Disable / 1: Enable status change interrupt
			CTRLR1_EIE	= 1;					// 0: Disable / 1: Enable error interrupt
			BTR1 = BaudRate;					// <<< set bit timing
			CTRLR1_CCE	= 0;					// Disable Configuration Change
			CTRLR1_INIT	= 0;					// Start CAN Operation
			CNC2	= CNC_CAN_EN;
			CNC2_DIR= PIN_DIR_OUT;
		break;
		case 2: 
			PRRR7_RX2_R	= 1;
			PRRR7_TX2_R	= 1;

			COER2_OE	= 1;					// enable CAN2 output
			PIER12_IE0	= 1;					// enable CAN2 input
			CTRLR2_INIT	= 1;					// Stop CAN Operation
			// Setup CAN channel Configuration
			CTRLR2_CCE	= 1;					// Enable Configuration Change
			CTRLR2_IE	= 1;					// 0: Disable / 1: Enable Module Interrupt
			CTRLR2_SIE	= 0;					// 0: Disable / 1: Enable status change interrupt
			CTRLR2_EIE	= 1;					// 0: Disable / 1: Enable error interrupt
			BTR2 = BaudRate;					// <<< set bit timing
			CTRLR2_CCE	= 0;					// Disable Configuration Change
			CTRLR2_INIT	= 0;					// Start CAN Operation
			CNC3	= CNC_CAN_EN;
			CNC3_DIR= PIN_DIR_OUT;
		break;
	}
}
//---------------------------------------------------------------------------------
void SetupCAN(U16 BaudRate0, U16 BaudRate1, U16 BaudRate2)
{
	InitCanFifoBuf();

	if(BaudRate0)
	{
		stInitCAN[0] = TRUE;
		CAN_init(0, BaudRate0);
		CAN_ConfigMsgBox(0);
	}else
	{
		stInitCAN[0] = FALSE;
		COER0_OE	= 0;	// enable CAN0 output
		PIER10_IE0	= 0;	// enable CAN0 input
		CTRLR0_INIT	= 0;	// Stop CAN Operation
	}

	if(BaudRate1)
	{
		stInitCAN[1] = TRUE;
		CAN_init(1, BaudRate1);
		CAN_ConfigMsgBox(1);
	}else
	{
		stInitCAN[1] = FALSE;
		COER1_OE	= 0;	// enable CAN1 output
		PIER04_IE2	= 0;	// enable CAN1 input
		CTRLR1_INIT	= 0;	// Stop CAN Operation
	}
	if(BaudRate2)
	{
		stInitCAN[2] = TRUE;
		CAN_init(2, BaudRate2);
		CAN_ConfigMsgBox(2);
	}else
	{
		stInitCAN[2] = FALSE;
		COER2_OE	= 0;	// enable CAN2 output
		PIER12_IE0	= 0;	// enable CAN2 input
		CTRLR2_INIT	= 0;	// Stop CAN Operation
	}
}
//---------------------------------------------------------------------------------
void CAN0_buffer(U8 Num, U8 Dir, U16 setID, U8 irq_rx, U32 mask, U8 EndFifo)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB0 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB0_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB0_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK0			= MSG2STD(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR0_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR0_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR0_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR0_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR0_RXIE	= irq_rx&1;// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR0_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR0_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR0_EOB	= EndFifo;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK0  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ0 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
void CAN1_buffer(U8 Num, U8 Dir, U16 setID, U8 irq_rx, U32 mask, U8 EndFifo)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB1 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB1_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB1_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK1			= MSG2STD(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK21_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR1_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR1_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR1_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR1_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR1_RXIE	= irq_rx&1;// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR1_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR1_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR1_EOB	= EndFifo;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK1  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ1 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
void CAN2_buffer(U8 Num, U8 Dir, U16 setID, U8 irq_rx, U32 mask, U8 EndFifo)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB2 = MSG2STD(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB2_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB2_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK2			= MSG2STD(mask);		// <<< Setup Mask corresponding to your application 
	IF1MSK22_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR2_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR2_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR2_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR2_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR2_RXIE	= irq_rx&1;	// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR2_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR2_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR2_EOB	= EndFifo;	// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK2  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ2 = Num;		// Transfer the Interface Register Content to the Message Buffer
}

void CAN_buffer(U8 NumCan, U8 Num, U8 Dir, U16 setID, U8 irq_rx, U32 mask,  U8 EndFifo)
{
	if(NumCan == 0)
		CAN0_buffer(Num, Dir, setID, irq_rx, mask, EndFifo);
	else if(NumCan == 1)
		CAN1_buffer(Num, Dir, setID, irq_rx, mask, EndFifo);
	else if(NumCan == 2)
		CAN2_buffer(Num, Dir, setID, irq_rx, mask, EndFifo);
}
void CAN_ConfigMsgBox(U8 NumCan)
{
	U8 i;
	
	for(i=0;i<31;i++) // первые 31 ящик работают на приём
	{
		#ifdef CAN_OPEN_ENABLE	
			if(i == 0)
				CAN_buffer(NumCan, 1  , MSGBOX_RX, (NMT<<7)   , TRUE, 0x7FF, 1);// про маску - 1 - бит должен совпадать 0 - сравнивается
			else if(i == 1)
				CAN_buffer(NumCan, 2  , MSGBOX_RX, (SYNC<<7)  , TRUE, 0x7FF, 1); 
			else if(i > 1)
				{
					if(i<30)
						CAN_buffer(NumCan, i+1, MSGBOX_RX, getNodeId(), TRUE, 0x1F, 0); 
					else
						CAN_buffer(NumCan, i+1, MSGBOX_RX, getNodeId(), TRUE, 0x1F, 1); 
				}
		#else
			if(i<30)
				CAN_buffer(NumCan, i+1, MSGBOX_RX, 0, TRUE, 0, 0); // приём всего подряд
			else
				CAN_buffer(NumCan, i+1, MSGBOX_RX, 0, TRUE, 0, 1); // приём всего подряд
		#endif
	}
}
//--------------------------------------------------------------------------------------------------------
// msgBox - 0-31

U8 CAN0_SendMessage(U8 msgBox, Message *msg)
{
	//putch('S');	putch('0');
	//putch(0x0A);putch(0x0D);
	// Prepare Arbitration Interface Register 
	
	if (TREQR0 & ((U32)(0x80000000))) return FALSE;
	
	//putch('S');	putch('0');
	// Prepare Arbitration Interface Register 
	IF1ARB0 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB0_XTD    = 0;					// <<< 0: 11bit ID / 1: 29bit ID 
	if(msg->rtr == 0)
		IF1ARB0_DIR    = 1;				// <<< 0: RX Buffer / 1: TX Buffer 
	else
		IF1ARB0_DIR    = 0;				// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL = 1;					// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
//	IF1MSK0			= 0x1FFFFFFF;// <<< Setup Mask corresponding to your application 
//	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
/*	IF1MCTR0_NEWDAT	= 0;		// Clear NewDat Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MsgLst Flag 
	IF1MCTR0_INTPND	= 0;		// Clear IntPnd Flag 
	IF1MCTR0_UMASK	= 0;		// <<< 0: Not use Filter Mask (Full CAN Object)/ 1: Use Filter Mask 
	IF1MCTR0_TXIE	= 1;		// <<< 0: Disable Tx-Interrupt / 1: enable Tx-Interrupt 
	IF1MCTR0_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: enable Rx-Interrupt 
	IF1MCTR0_RMTEN	= 0;		// <<< 0: Disable Remote / 1: enable Remote 
	IF1MCTR0_TXRQST	= 0;		// Don't set Transmission Request here 
	IF1MCTR0_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer   
	IF1MCTR0_DLC	= msg->len;	// Set number of Data to be transmitted 
*/
	if(msg->rtr == 0)
		IF1MCTR0		=(msg->len&0xF) | (1<<7)|((U16)1<<11);
	else
		IF1MCTR0		=(msg->len&0xF) | (1<<7)|((U16)1<<8)|((U16)1<<11);

	// Set Data 
	IF1DTA0 = *((U32 *)(msg->data));
	IF1DTB0 = *((U32 *)(msg->data+4));

	IF1CMSK0  = 0xF7;			// Prepare Interface Command Mask Register: 
								// WRRD    = 1 : Write information from interface to object
								// MASK    = 1 : Write Mask regsiter
								// ARB     = 1 : Write Arbitration Register (ID Register)
								// CONTROL = 1 : Write object control register (e.g. Data lenght, IRQ enable, etc)
								// CIP     = 0 : (dont care for writing to object)
								// TXREQ   = 1 : Set Transmission Request
								// DATAA   = 1 : Write Data A register
								// DATAB   = 1 : Write Data B register

	IF1CREQ0 = msgBox+1;		// Transfer the Interface Register Content to the Message Buffer 
	
	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
// msgBox - 0-31

U8 CAN1_SendMessage(U8 msgBox, Message *msg)
{
	if (TREQR1 & ((U32)(0x80000000))) return FALSE;
	
	
	//putch('S');	putch('1');
	// Prepare Arbitration Interface Register 
	IF1ARB1 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB1_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	if(msg->rtr == 0)
	IF1ARB1_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	else
		IF1ARB1_DIR    = 0;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
//	IF1MSK0			= 0x1FFFFFFF;// <<< Setup Mask corresponding to your application 
//	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
/*	IF1MCTR1_NEWDAT	= 0;		// Clear NewDat Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MsgLst Flag 
	IF1MCTR1_INTPND	= 0;		// Clear IntPnd Flag 
	IF1MCTR1_UMASK	= 0;		// <<< 0: Not use Filter Mask (Full CAN Object)/ 1: Use Filter Mask 
	IF1MCTR1_TXIE	= 1;		// <<< 0: Disable Tx-Interrupt / 1: enable Tx-Interrupt 
	IF1MCTR1_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: enable Rx-Interrupt 
	IF1MCTR1_RMTEN	= 0;		// <<< 0: Disable Remote / 1: enable Remote 
	IF1MCTR1_TXRQST	= 0;		// Don't set Transmission Request here 
	IF1MCTR1_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer   
	IF1MCTR1_DLC	= msg->len;	// Set number of Data to be transmitted 
*/

	if(msg->rtr == 0)
		IF1MCTR1		=(msg->len&0xF) | (1<<7)|((U16)1<<11);
	else
		IF1MCTR1		=(msg->len&0xF) | (1<<7)|((U16)1<<8)|((U16)1<<11);

	// Set Data 
	IF1DTA1 = *((U32 *)(msg->data));
	IF1DTB1 = *((U32 *)(msg->data+4));

	IF1CMSK1  = 0xF7;			// Prepare Interface Command Mask Register: 
								// WRRD    = 1 : Write information from interface to object
								// MASK    = 1 : Write Mask regsiter
								// ARB     = 1 : Write Arbitration Register (ID Register)
								// CONTROL = 1 : Write object control register (e.g. Data lenght, IRQ enable, etc)
								// CIP     = 0 : (dont care for writing to object)
								// TXREQ   = 1 : Set Transmission Request
								// DATAA   = 1 : Write Data A register
								// DATAB   = 1 : Write Data B register

	IF1CREQ1 = msgBox+1;		// Transfer the Interface Register Content to the Message Buffer 
	
	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
// msgBox - 0-31

U8 CAN2_SendMessage(U8 msgBox, Message *msg)
{
	if (TREQR2 & ((U32)(0x80000000))) return FALSE;

	//putch('S');	putch('2');
	// Prepare Arbitration Interface Register 
	IF1ARB2 = MSG2STD(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB2_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	if(msg->rtr == 0)
	IF1ARB2_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	else
		IF1ARB2_DIR    = 0;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
//	IF1MSK0			= 0x1FFFFFFF;// <<< Setup Mask corresponding to your application 
//	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
/*	IF1MCTR2_NEWDAT	= 0;		// Clear NewDat Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MsgLst Flag 
	IF1MCTR2_INTPND	= 0;		// Clear IntPnd Flag 
	IF1MCTR2_UMASK	= 0;		// <<< 0: Not use Filter Mask (Full CAN Object)/ 1: Use Filter Mask 
	IF1MCTR2_TXIE	= 1;		// <<< 0: Disable Tx-Interrupt / 1: enable Tx-Interrupt 
	IF1MCTR2_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: enable Rx-Interrupt 
	IF1MCTR2_RMTEN	= 0;		// <<< 0: Disable Remote / 1: enable Remote 
	IF1MCTR2_TXRQST	= 0;		// Don't set Transmission Request here 
	IF1MCTR2_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer   
	IF1MCTR2_DLC	= msg->len;	// Set number of Data to be transmitted 
*/
	if(msg->rtr == 0)
		IF1MCTR2		=(msg->len&0xF) | (1<<7)|((U16)1<<11);
	else
		IF1MCTR2		=(msg->len&0xF) | (1<<7)|((U16)1<<8)|((U16)1<<11);

	// Set Data 
	IF1DTA2 = *((U32 *)(msg->data));
	IF1DTB2 = *((U32 *)(msg->data+4));

	IF1CMSK2  = 0xF7;			// Prepare Interface Command Mask Register: 
								// WRRD    = 1 : Write information from interface to object
								// MASK    = 1 : Write Mask regsiter
								// ARB     = 1 : Write Arbitration Register (ID Register)
								// CONTROL = 1 : Write object control register (e.g. Data lenght, IRQ enable, etc)
								// CIP     = 0 : (dont care for writing to object)
								// TXREQ   = 1 : Set Transmission Request
								// DATAA   = 1 : Write Data A register
								// DATAB   = 1 : Write Data B register

	IF1CREQ2 = msgBox+1;		// Transfer the Interface Register Content to the Message Buffer 
	
	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
U8 CAN_ReceiveMessage(U8 nCAN, Message *m)
{ 
	Message *pM;
	
	if(nCAN>2) return FALSE;
	
	/* See if a message is pending in the stack */
	if(ptrMsgRcv[nCAN].count == 0)
		return FALSE; // No new message
	
	/* Store the message from the stack*/
	pM = (Message *)&stackMsgRcv[nCAN][ptrMsgRcv[nCAN].st];
	m->cob_id = pM->cob_id;
	m->len 	= pM->len;
	m->rtr 	= pM->rtr;

	*((U32 *)(m->data))		= *((U32 *)(pM->data));
	*((U32 *)(m->data+4))	= *((U32 *)(pM->data+4));

	/* Increment the reading pointer of the stack */
	ptrMsgRcv[nCAN].st++;
	if (ptrMsgRcv[nCAN].st == MAX_STACK_MSG_RX) 
		ptrMsgRcv[nCAN].st = 0;

	ptrMsgRcv[nCAN].count--;
	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
U8 CAN_SendMessage(U8 NumCan, Message *msg)
{
	U8	ret; 
	Message *pM;

	if(NumCan>(NB_LINE_CAN-1)) return FALSE;
//-------------------------------------------------------------------------	
	if (ptrMsgSend[NumCan].count == 0)
	{
		DisInterrupt();
		if(NumCan == 0) ret = CAN0_SendMessage(31, msg);
		if(NumCan == 1) ret = CAN1_SendMessage(31, msg);
		if(NumCan == 2) ret = CAN2_SendMessage(31, msg);
		EnInterrupt();
		
		if(ret == TRUE) return TRUE;
	}
				
	if(ptrMsgSend[NumCan].count == (MAX_STACK_MSG_TX-2)) return FALSE;
			
	// добавляем в буфер на отправку
	pM = (Message *)&stackMsgSend[NumCan][ptrMsgSend[NumCan].end];
	pM->cob_id 		= msg->cob_id;
	pM->len 		= msg->len;
	pM->rtr 		= msg->rtr;
	*((U32 *)pM->data)		= *((U32 *)msg->data);
	*((U32 *)(pM->data+4))	= *((U32 *)(msg->data+4));
	
	DisInterrupt();
	ptrMsgSend[NumCan].count++;
	ptrMsgSend[NumCan].end++;
	if(ptrMsgSend[NumCan].end	==	MAX_STACK_MSG_TX)
		ptrMsgSend[NumCan].end = 0;
	EnInterrupt();		

	return TRUE;
}
//--------------------------------------------------------------------------------------------------------
void ResetCan(U8 nCan)
{
	if(nCan == 0)
	{
		/*Restart bus*/
		CTRLR0_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT0_TEC!=0)||(ERRCNT0_REC!=0));//see if recovered
	}
	if(nCan == 1)
	{
		/*Restart bus*/
		CTRLR1_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT1_TEC!=0)||(ERRCNT1_REC!=0));//see if recovered
	}
	if(nCan == 2)
	{
		/*Restart bus*/
		CTRLR2_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT2_TEC!=0)||(ERRCNT2_REC!=0));//see if recovered
	}
}
//--------------------------------------------------------------------------------------------------------
U8  CAN_GetRxBuffer(U8 nCan)
{
	U8 number=1;
	
	if(nCan == 0)
	{
		if (NEWDT0)
		{
			while ((NEWDT0 & ((unsigned long)1 << (number++ - 1)))==0);
			return (number-1);
		}
		else 
			return (0);
	}
	if(nCan == 1)
	{
		if (NEWDT1)
		{
			while ((NEWDT1 & ((unsigned long)1 << (number++ - 1)))==0);
			return (number-1);
		}
		else 
			return (0);
	}
	if(nCan == 2)
	{
		if (NEWDT2)
		{
			while ((NEWDT2 & ((unsigned long)1 << (number++ - 1)))==0);
			return (number-1);
		}
		else 
			return (0);
	}
	return (0);
}
//--------------------------------------
// Третий CAN блока

void __interrupt CAN_0_IRQ(void)
{
	U16 MsgNbr;
	U32 MsgBuffer;
	Message *pM;
	//Message msg;

	DisInterrupt();
	//putch('1');putch(0x20);
	
	MsgNbr = INTR0;			//stor MsgNbr
	
	if(INTR0 == 0x8000)		/* status int */
	{
		//state only when INTR==0x8000
		if(STATR0_BOFF == 1)		//bus off
		{
			//putch('B');
			/*Restart bus*/
			ResetCan(0);
		}
		if(STATR0_EWARN==0x01)		//error warning
		{
			//putch('W');
			/*Restart bus*/
			ResetCan(0);
		}
		if(!((STATR0_BOFF)|(STATR0_EWARN)|(STATR0_EPASS))) //error active
		{
			//putch('A');
			/*Restart bus*/
			ResetCan(0);
		}
	}
	else
	{
		if( (MsgNbr>=1) && (MsgNbr<=0x20) ) // valid buffer number
		{
			MsgBuffer = ((U32)0x01) << (MsgNbr-1);
			
			//Check whether the interrupt source is a valid buffer
			if(((MSGVAL0 & MsgBuffer) != 0) && ((INTPND0 & MsgBuffer) != 0))
			{
				// Check whether the interrupt cause is receive or transmit
				if( (NEWDT0 & MsgBuffer) != 0 ) // is a receive interrupt
				{
					while( (NEWDT0 & MsgBuffer) != 0 )
					{
//						putch('R');
//						putch('0');
//						putch(' ');
//						putch(0x30+MsgNbr/10);putch(0x30+MsgNbr%10);putch(0x20);	
						// call the receive handler (must clear NEWDAT & INTPND)
						//fetch data from msg RAM
						IF2CMSK0	= 0x003F;	
	                              //   Receive Control Information, Message data and Arbitration from Message Buffer
	                              // WRRD    = 0 : Write information from object to interface
	                              // MASK    = 0 : Do not transfer Mask regsiter
	                              // ARB     = 1 : Transfer Arbitration Register (ID Register)
	                              // CONTROL = 1 : Transfer object control register (e.g. Data lenght, IRQ enable, etc)
	                              // CIP     = 1 : (Clear Interrupt Pending flag)
	                              // TXREQ   = 1 : Set Transmission Request
	                              // DATAA   = 1 : Transfer Data A register
	                              // DATAB   = 1 : Transfer Data B register
						IF2CREQ0 = MsgNbr;		//transmit msgRAM to IF
						
						if(IF2MCTR0_MSGLST==0x01)
						{	// clear overflow
							__wait_nop();  //mag lost
							//IF1MCTR0	= 0x1488;		//NewDat=0 MSGLST=0 INTPND=0 UMSK=1 TXIE=0
														//RXIE=1 RMTEN=0 TXRQST=0 EOB=1
							IF2MCTR0_MSGLST	= 0;		// Clear MSGLST Flag
							IF2CMSK0		= 0x0090;	//WRRD=1 CONTROL=1 other=0
														//for clear MSGLST
							IF2CREQ0 = MsgNbr;
							//putch('O');
						}
						
						if (ptrMsgRcv[0].count <(MAX_STACK_MSG_RX-2)) 
						{
							pM = (Message *)&stackMsgRcv[0][ptrMsgRcv[0].end];
							
							pM->rtr		= IF2ARB0_DIR;
							pM->len		= IF2MCTR0_DLC;
							pM->cob_id	= STD2MSG(IF2ARB0);
							*((U32 *)pM->data)     = IF2DTA0;
							*((U32 *)(pM->data+4)) = IF2DTB0;
							
							ptrMsgRcv[0].end++;
							if(ptrMsgRcv[0].end == MAX_STACK_MSG_RX)
								ptrMsgRcv[0].end = 0;
								
							ptrMsgRcv[0].count++;
							
						}
						setTimer(&program.TimerCan0, TIME_OUT_CAN);
						//else // буфер полон
						if(IF2MCTR0_EOB == 1) break;
						
						MsgBuffer = MsgBuffer<<1;
						MsgNbr++;
					}
				}
				else 
				if ((TREQR0 & MsgBuffer) == 0 ) // is transmission done
				{
					IF1CMSK0 = 0x0018; // =>
					// WRRD = 0 // read
					// CONTROL = 1 // transfer control
					// CIP = 1 // clear INTPND bit
					IF1CREQ0 = MsgNbr; // start transfer
					//---------------------------------
					// send fifo msg
					SendFifoCanMsg(0);
				}
			}
		}
		else
		{
			//putch('!');
				__wait_nop();	/*    */
			__wait_nop();	
		}
	}
	//putch(0x0A);putch(0x0D);
	EnInterrupt();
	//receiveMsgHandler(0);
}
//--------------------------------------------------------------------------------------------------------
// Первый CAN блока
//----------
void __interrupt CAN_1_IRQ(void)
{
	U16 MsgNbr;
	U32 MsgBuffer;
	Message *pM;
//	Message msg;
//	U32 tmp;
	
//putch('0');
//	putch('2');putch(0x20);

	DisInterrupt();
	MsgNbr = INTR1;			//stor MsgNbr
	
	if(INTR1 == 0x8000)		/* status int */
	{
		//state only when INTR==0x8000
		if(STATR1_BOFF == 1)		//bus off
		{
			/*Restart bus*/
			//putch('B');
			ResetCan(1);
		}
		if(STATR1_EWARN==0x01)		//error warning
		{
			//putch('W');
			/*Restart bus*/
			ResetCan(1);
		}
		if(!((STATR1_BOFF)|(STATR1_EWARN)|(STATR1_EPASS))) //error active
		{
			//putch('A');
			/*Restart bus*/
			ResetCan(1);
		}
	}
			else
	{
		if( (MsgNbr>=1) && (MsgNbr<=0x20) ) // valid buffer number
		{
			MsgBuffer = ((U32)0x01) << (MsgNbr-1);
			//putch(0x30+MsgNbr/10);putch(0x30+MsgNbr%10);putch(0x20);
			// Check whether the interrupt source is a valid buffer
			if(((MSGVAL1 & MsgBuffer) != 0) && ((INTPND1 & MsgBuffer) != 0))
			{
				// Check whether the interrupt cause is receive or transmit
				if( (NEWDT1 & MsgBuffer) != 0 ) // is a receive interrupt
				{
					while( (NEWDT1 & MsgBuffer) != 0 )
					{
						
//						putch(0x30+MsgNbr);
//						putch('R');
//						putch('1');
//						putch(' ');
//						putch(0x30+MsgNbr/10);putch(0x30+MsgNbr%10);putch(0x20);	
						//putch('R');
						// call the receive handler (must clear NEWDAT & INTPND)
						/*fetch data from msg RAM*/
						IF2CMSK1	= 0x003F;	
	                              //   Receive Control Information, Message data and Arbitration from Message Buffer
	                              // WRRD    = 0 : Write information from object to interface
	                              // MASK    = 0 : Do not transfer Mask regsiter
	                              // ARB     = 1 : Transfer Arbitration Register (ID Register)
	                              // CONTROL = 1 : Transfer object control register (e.g. Data lenght, IRQ enable, etc)
	                              // CIP     = 1 : (Clear Interrupt Pending flag)
	                              // TXREQ   = 1 : Set Transmission Request
	                              // DATAA   = 1 : Transfer Data A register
	                              // DATAB   = 1 : Transfer Data B register
						IF2CREQ1 = MsgNbr;		//transmit msgRAM to IF
						
						if(IF2MCTR1_MSGLST==0x01)
						{	// clear overflow
							__wait_nop();  //mag lost
							//IF1MCTR0	= 0x1488;		//NewDat=0 MSGLST=0 INTPND=0 UMSK=1 TXIE=0
														//RXIE=1 RMTEN=0 TXRQST=0 EOB=1
							IF2MCTR1_MSGLST	= 0;		// Clear MSGLST Flag
							IF2CMSK1		= 0x0090;	//WRRD=1 CONTROL=1 other=0
														//for clear MSGLST
							IF2CREQ1 = MsgNbr;
							//putch('O');
						}
						
	
						if (ptrMsgRcv[1].count <(MAX_STACK_MSG_RX-2)) 
						{
							pM = (Message *)&stackMsgRcv[1][ptrMsgRcv[1].end];
							
							pM->rtr		= IF2ARB1_DIR;
							pM->len		= IF2MCTR1_DLC;
							pM->cob_id	= STD2MSG(IF2ARB1);


							*((U32 *)pM->data)     = IF2DTA1;
							*((U32 *)(pM->data+4)) = IF2DTB1;
							
							ptrMsgRcv[1].end++;
							if(ptrMsgRcv[1].end == MAX_STACK_MSG_RX)
								ptrMsgRcv[1].end = 0;
							ptrMsgRcv[1].count++;
						}
						/*msg.rtr		= IF2ARB1_DIR;
						msg.len		= IF2MCTR1_DLC;
						msg.cob_id	= STD2MSG(IF2ARB1);
						*((U32 *)msg.data)     = IF2DTA1;
						*((U32 *)(msg.data+4)) = IF2DTB1;
					
						receiveMsgHandler_(1, &msg);
						*/
						//else // буфер полон
						setTimer(&program.TimerCan1, TIME_OUT_CAN);
						
						if(IF2MCTR1_EOB == 1) break;
						
						MsgBuffer = MsgBuffer<<1;
						MsgNbr++;
						
					}
				}
				else if ((TREQR1 & MsgBuffer) == 0 ) // is transmission done
				{
					IF1CMSK1 = 0x0018; // =>
					// WRRD = 0 // read
					// CONTROL = 1 // transfer control
					// CIP = 1 // clear INTPND bit
					IF1CREQ1 = MsgNbr; // start transfer
					//---------------------------------
					// send fifo msg
					SendFifoCanMsg(1);
//					putch('T');
				}
			}
		}
		else
		{
			//putch('!');
				__wait_nop();	/*    */
			__wait_nop();
		}
	}
//	putch('E');
	//putch(0x0A);putch(0x0D);
	EnInterrupt();
	//receiveMsgHandler(1);
}
//--------------------------------------------------------------------------------------------------------
void __interrupt CAN_2_IRQ(void)
{
	U16 MsgNbr;
	U32 MsgBuffer;
	Message *pM;
	//Message msg;

	DisInterrupt();
//	putch('3');putch(0x20);

	MsgNbr = INTR2;			//stor MsgNbr
	if(INTR2 == 0x8000)		/* status int */
	{
		//state only when INTR==0x8000
		if(STATR2_BOFF == 1)		//bus off
		{
			/*Restart bus*/
			//putch('B');
			ResetCan(2);
		}
		if(STATR2_EWARN==0x01)		//error warning
		{
			//putch('W');
			/*Restart bus*/
			ResetCan(2);
		}
		if(!((STATR2_BOFF)|(STATR2_EWARN)|(STATR2_EPASS))) //error active
		{
			//putch('A');
			/*Restart bus*/
			ResetCan(2);
		}
	}
	else
	{
		if( (MsgNbr>=1) && (MsgNbr<=0x20) ) // valid buffer number
		{
			//putch(0x30+MsgNbr/10);putch(0x30+MsgNbr%10);putch(0x20);
			MsgBuffer = ((U32)0x01) << (MsgNbr-1);
			// Check whether the interrupt source is a valid buffer
			if(((MSGVAL2 & MsgBuffer) != 0) && ((INTPND2 & MsgBuffer) != 0))
			{
				// Check whether the interrupt cause is receive or transmit
				if( (NEWDT2 & MsgBuffer) != 0 ) // is a receive interrupt
				{
					
					while( (NEWDT2 & MsgBuffer) != 0 )
					{
						//putch('R');
						// call the receive handler (must clear NEWDAT & INTPND)
						/*fetch data from msg RAM*/
						IF2CMSK2	= 0x003F;	
	                              //   Receive Control Information, Message data and Arbitration from Message Buffer
	                              // WRRD    = 0 : Write information from object to interface
	                              // MASK    = 0 : Do not transfer Mask regsiter
	                              // ARB     = 1 : Transfer Arbitration Register (ID Register)
	                              // CONTROL = 1 : Transfer object control register (e.g. Data lenght, IRQ enable, etc)
	                              // CIP     = 1 : (Clear Interrupt Pending flag)
	                              // TXREQ   = 1 : Set Transmission Request
	                              // DATAA   = 1 : Transfer Data A register
	                              // DATAB   = 1 : Transfer Data B register
						IF2CREQ2 = MsgNbr;		//transmit msgRAM to IF
						
						if(IF2MCTR2_MSGLST==0x01)
						{	// clear overflow
							__wait_nop();  //mag lost
							//IF1MCTR2	= 0x1488;		//NewDat=0 MSGLST=0 INTPND=0 UMSK=1 TXIE=0
														//RXIE=1 RMTEN=0 TXRQST=0 EOB=1
							IF2MCTR2_MSGLST	= 0;		// Clear MSGLST Flag
							IF2CMSK2		= 0x0090;	//WRRD=1 CONTROL=1 other=0
														//for clear MSGLST
							IF2CREQ2 = MsgNbr;
							//putch('O');
						}
						
						if (ptrMsgRcv[2].count <(MAX_STACK_MSG_RX-2)) 
						{
							pM = (Message *)&stackMsgRcv[2][ptrMsgRcv[2].end];
							
							pM->rtr		= IF2ARB2_DIR;
							pM->len		= IF2MCTR2_DLC;
							pM->cob_id	= STD2MSG(IF2ARB2);
							*((U32 *)pM->data)     = IF2DTA2;
							*((U32 *)(pM->data+4)) = IF2DTB2;
							
							ptrMsgRcv[2].end++;
							if(ptrMsgRcv[2].end == MAX_STACK_MSG_RX)
								ptrMsgRcv[2].end = 0;
							ptrMsgRcv[2].count++;
						}
						/*
						msg.rtr		= IF2ARB2_DIR;
						msg.len		= IF2MCTR2_DLC;
						msg.cob_id	= STD2MSG(IF2ARB2);
						*((U32 *)msg.data)     = IF2DTA2;
						*((U32 *)(msg.data+4)) = IF2DTB2;
					
						receiveMsgHandler_(2, &msg);
						*/
						//else // буфер полон
						setTimer(&program.TimerCan2, TIME_OUT_CAN);
						
						if(IF2MCTR2_EOB == 1) break;
						MsgBuffer = MsgBuffer<<1;
						MsgNbr++;
					}	
				}
				else if ((TREQR2 & MsgBuffer) == 0 ) // is transmission done
				{
					
					IF1CMSK2 = 0x0018; // =>
					// WRRD = 0 // read
					// CONTROL = 1 // transfer control
					// CIP = 1 // clear INTPND bit
					IF1CREQ2 = MsgNbr; // start transfer
					//---------------------------------
					// send fifo msg
					SendFifoCanMsg(2);
				}
			}
		}
		else
		{
			//putch('!');
			__wait_nop();	/*    */
			__wait_nop();
		}
	}
	//putch(0x0A);putch(0x0D);
	EnInterrupt();
	//receiveMsgHandler(2);

}
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------
