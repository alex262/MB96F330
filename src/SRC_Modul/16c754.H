//------------------------------------------------------------------------------ 
#ifndef    AT16C554H 
#define    AT16C554H 
 
//------------------------------------------------------------------------------ 
 
#include   "timer.h" 
#include   "AT78E58BP.H" 
#include   "System.h" 
#include   "Asyn_422.H" 
#include   <intrins.h> 
#include   <string.h> 
//------------------------------------------------------------------------------ 
//							ÃÑÏÆ£âõ 
//------------------------------------------------------------------------------ 
#define	 PortNumber			4 
#define  SERIAL_STU_BUSY 	0x01				// Ä¡ààÏêÓ²ÁúæÎŞË1ŞŒÓ²0ŞŒÏåßà 
#define  SERIAL_STU_FREE 	0x00			 
//------------------------------------------------------------------------------ 
//                  Â£ÜèÒÚÆ£âõ 
//------------------------------------------------------------------------------ 
#define 	CY18432M 
#ifdef 		CY18432M 
	#define 	B50 		0x0900				// DLM ŞŒÈïÀÛŞËDLL ŞŒÅİÀÛŞË 
	#define 	B300 		0x0180 
	#define 	B600 		0x00c0 
	#define 	B1200 		0x0060 
	#define 	B2400 		0x0030 
	#define 	B4800 		0x0018 
	#define 	B9600 		0x000c 
	#define 	B19200 		0x0006 
	#define 	B38400 		0x0003 
	#define 	B57600 		0x0002 
	#define 	B115200 	0x0001 
#else 
	#define 	B200 		0x0900				// DLM ŞŒÈïÀÛŞËDLL ŞŒÅİÀÛŞË 
	#define 	B1200 		0x0180 
	#define 	B2400 		0x00c0 
	#define 	B4800 		0x0060 
	#define 	B9600 		0x0030 
	#define 	B19200 		0x0018 
	#define 	B38400 		0x000c 
	#define 	B76800 		0x0006 
	#define 	B153600 	0x0003 
	#define 	B230400 	0x0002 
	#define 	B460800 	0x0001 
#endif 
//------------------------------------------------------------------------------ 
//           16C554 ÔêÂÏÌÔÄöÖ¿Æ£âõ 
//------------------------------------------------------------------------------ 
#define 		RHR 			0X00		// ÍãÚìÚ§ÎíÁÃæÌÔÄöÖ¿ 					æËÆÑ 
#define 		THR 			0X00		// Ç€ÛİÚ§ÎíÁÃæÌÔÄöÖ¿ 					æËàÄ 
#define 		IER 			0X01		// æàÆßäÚàıÌÔÄöÖ¿     					æËàÄ 
#define 		FCR 			0X02		// FIFO ÏèæÖÌÔÄöÖ¿						æËàÄ 
#define 		ISR 			0X02		// æàÆßçÄÜÌÔÄöÖ¿							æËÆÑ 
#define 		LCR 			0X03		// ßïÏèæÖÌÔÄöÖ¿							æËàÄ 
#define 		MCR 			0X04		// MODEM ÏèæÖÌÔÄöÖ¿						æËàÄ 
#define 		LSR 			0X05		// ßïçÄÜÌÔÄöÖ¿							æËÆÑ 
#define 		MSR 			0X06		// MODEM çÄÜÌÔÄöÖ¿						æËÆÑ 
#define 		SPR 			0X07		// æàÌôÍñÉäíÄöÌÔÄöÖ¿,						ÏÙÆÑàÄ 
#define 		DLL 			0X00		// Â£ÜèÒÚÇ€ÙœÇæÖÅâçãÅİÀÛŞËLCR bit7 ŞŒ1,	ÏÙÆÑàÄ 
#define 		DLM 			0X01		// Â£ÜèÒÚÇ€ÙœÇæÖÅâçãÈïÀÛŞËLCR bit7 ŞŒ1	ÏÙÆÑàÄ 
 
#define FIFO_EN 				0X01		// FIFO ÚÉÔìÓÑşçæ 
#define FIFO_TRI_LEVEL_1 		0X00		// FIFO Ä½Ç€ÌÆÁ¹ŞŒ1È³çæÍê 
#define FIFO_TRI_LEVEL_4 		0X40		// FIFO Ä½Ç€ÌÆÁ¹ŞŒ4È³çæÍê 
#define FIFO_TRI_LEVEL_8 		0X80		// FIFO Ä½Ç€ÌÆÁ¹ŞŒ8È³çæÍê 
#define FIFO_TRI_LEVEL_14 		0XC0		// FIFO Ä½Ç€ÌÆÁ¹ŞŒ14È³çæÍê 
#define FIFO_DMA_MODE 			0X08		// FIFO DMA ÔÚÍá¨ä¸ÓÑşçæ 
#define FIFO_CLEAR_RCVR 		0X02		// ×õÃ§ÍãÚìFIFO æàÅÔÔêØíÚ§Îí 
#define FIFO_CLEAR_XMIT 		0X04		// ×õÃ§Ç€ÛİFIFO æàÅÔÔêØíÚ§Îí 
 
#define IER_MODEM_INT 			0X08		// MODEM çÄÜæàÆßÖÑÁŞŞË 
#define IER_RECEIVELINE_INT 	0X04		// ÍãÚìßïçÄÜæàÆßÖÑÁŞŞË 
#define IER_TRANSMITHOLD_INT 	0X02		// Ç€ÛİÁÃæÌÔÄöÖ¿æàÆßÖÑÁŞŞË 
#define IER_RECEIVEHOLD_INT 	0X01		// ÍãÚìÁÃæÌÔÄöÖ¿æàÆßÖÑÁŞŞË 
 
#define LCR_WORD_LEN5 			0X00		// Ú§ÎíŞËÃªÆØŞŒ5 
#define LCR_WORD_LEN6 			0X01		// Ú§ÎíŞËÃªÆØŞŒ6 
#define LCR_WORD_LEN7 			0X02		// Ú§ÎíŞËÃªÆØŞŒ7 
#define LCR_WORD_LEN8 			0X03		// Ú§ÎíŞËÃªÆØŞŒ8 
#define LCR_STOP_LEN1 			0X00		// İæÉŞËÃªÆØŞŒ1 
#define LCR_STOP_LEN2 			0X04		// İæÉŞËÃªÆØŞŒ2 ç€âòÊ ÅÁÚ§ÎíŞËŞŒ5ÚÁİæÉŞËŞŒ1.5ŞË 
#define LCR_PARITY_EN 			0X08		// àáùŞËÚÉÔìÏèæÖŞË 
#define LCR_PARITY_EVEN 		0X10		// EVENÕÌàáùÙøæÓŞËÔØŞ×ùÏ³ßÒŞŒÖöàáùODD 
#define LCR_PARITY_SET 			0X20		// ÖöÕÌàáù×ÏæÖÙøæÓŞË 
#define LCR_OUT_BREAK 			0X40		// TXÚôÃ³BREAKàÕÊÕ 
#define LCR_DIV_EN 				0X80		// äÚàıÇ€ÛİÂ£ÜèÒÚ 
#define LCR_DIV_Dis				0X00		// ÍæÉÇ€ÛİÂ£ÜèÒÚ 
#define MCR_INTX_EN 			0X08		// æàÆßâ§ÍÕÚôÃ³äÚàıÏèæÖÓÑşçæ 
#define MCR_INTX_Dis 			0X00		// æàÆßâ§ÍÕÚôÃ³ÍæÉÏèæÖÓÑşçæ 
 
//------------------------------------------------------------------------------ 
//           16C554 æàÆßÚÒÌ¢Æ£âõ 
//------------------------------------------------------------------------------ 
#define EVENT_LSR 				0X06		// LSR çÄÜÌÔÄöÖ¿ÚÒÌ¢ 
#define EVENT_LSR_RECEIVE 		0X01		// Ú§ÎíÍãÚìÁÄöÅÍÁÄöÌÔÄöÖ¿ËFIFOæà 
#define EVENT_LSR_OVERRUN 		0X02		// ÍãÚìâÖŞËË§ÄöÖ¿æàÅÔÚ§ÎíÇ€ÙœÈÂÈ×ÄıŞƒÚÒÌ¢ 
#define EVENT_LSR_PARITY 		0X04		// ÍãÚìÚ§ÎíàáùÄıŞƒÚÒÌ¢ 
#define EVENT_LSR_FRAMING 		0X08		// ÍãÚìÚ§ÎíÇ€Ùœæ¨ÄıŞƒ 
#define EVENT_LSR_BREAK 		0X10		// ÍãÚìÅÍâËÈ³BREAK ÚÒÌ¢ 
#define EVENT_LSR_TRANHOLDEMPTY 0X20		// Ç€ÛİÁÃæÌÔÄöÖ¿ËFIFO ÏåÚÒÌ¢äÚàıçÀäèÚ§Îí 
#define EVENT_LSR_TRANSMITEMPTY 0x40		// Ç€ÛİÖ¿ÏåÚÒÌ¢ 
#define EVENT_LSR_FIFOERROR 	0X80		// FIFO æàãàÄıŞƒÚ§Îí 
#define EVENT_RXRDY 			0X04		// Ú§ÎíÍãÚìÎİà¿æàÆßÚÒÌ¢ 
#define EVENT_RXRDY_TIMEOUT 	0X0C		// ÍãÚìÚ§ÎíÃÚÁæàÆßÚÒÌ¢ 
#define EVENT_TXRDY 			0X02		// Ú§ÎíÇ€ÛİÎİà¿æàÆßÚÒÌ¢ 
#define EVENT_MSR 				0X00		// MODEM çÄÜæàÆßÚÒÌ¢ 
#define FIFO_SIZE 				16 
 
//------------------------------------------------------------------------------------- 
//      16C554 È¿ÆÛÏêRAMãÃÙô 
//------------------------------------------------------------------------------------- 
#define STARTADDR_SERIAL1 		0X0000		// Ä¡ààÏêİ£ÅĞ1Ö¹ÚÌÅèæÇ 
#define STARTADDR_SERIAL2 		0X0008		// Ä¡ààÏêİ£ÅĞ2Ö¹ÚÌÅèæÇ 
#define STARTADDR_SERIAL3 		0X0010		// Ä¡ààÏêİ£ÅĞ3Ö¹ÚÌÅèæÇ 
#define STARTADDR_SERIAL4 		0X0018		// Ä¡ààÏêİ£ÅĞ4Ö¹ÚÌÅèæÇ 
 
//------------------------------------------------------------------------------------- 
//       ÖôÛÃÑÏ 
//------------------------------------------------------------------------------------- 
#define   	Init_16c554_Count    	10 
#define 	FREE_RESET 				0 
#define 	RESET 					1 
//------------------------------------------------------------------------------------- 
//         ÆğÄ¡ÏêßğÉèÅÔÚ§ÎíÍñÉÉ 
//------------------------------------------------------------------------------------- 
typedef struct st_CommHandle 
			   { 
				 Uint   RAMAddress_Map; 
				 Uchar  CommReceiveBuffer[CommMaxReceiveBuffer];  //Ä¡ÏêÍãÚåËÊÃõ×¼ 
				 Uchar  CommSendBuffer[CommMaxSendBuffer];        //Ä¡ÏêÇ€ÛİËÊÃõ×¼ 
				 Uchar  CommReceive_Head;                         //Ä¡ÏêÍãÚåËÊÃõ×¼İÇæÈåû 
				 Uchar  CommReceive_Trail;                        //Ä¡ÏêÍãÚåËÊÃõ×¼ŞÂæÈåû 
				 Uchar  CommSend_Head;                            //Ä¡ÏêÇ€ÛİËÊÃõ×¼İÇæÈåû 
				 Uchar  CommSend_Trail;                           //Ä¡ÏêÇ€ÛİËÊÃõ×¼ŞÂæÈåû 
			     Uchar  Comm_Stu;                                 //Ä¡ààÏêÇ€ÛİÖ¿Ó²ÁúæÎÚ§çù 
			     Uchar  Comm_Priority;                            //Ä¡ààÏêÍãÚìËÊÃõ×¼Ú§ÎíÄ²ĞıãÕßØÌÆÁ¹Ú§çù 
			   };//end struct 
 
typedef struct st_CommHandle TCommHandle; 
//------------------------------------------------------------------------------------- 
//        ÆğÄ¡Ïê16C554 ×§ÆÃÜàÉãÓÊÚ§ 
//------------------------------------------------------------------------------------- 
extern bool 	Init_16c554(void); 
extern void 	Port_open(Uchar PortID,Uint Baud); 
extern void 	Port_close(Uchar PortID); 
extern int  	Port_Receive(Uchar PortID,void *Buffer,int Count); 
extern int  	Port_Send(Uchar PortID,void *Buffer,int Count); 
//------------------------------------------------------------------------------------- 
#endif 
//------------------------------------------------------------------------------------- 