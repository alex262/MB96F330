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
//							Á��ƣ�� 
//------------------------------------------------------------------------------ 
#define	 PortNumber			4 
#define  SERIAL_STU_BUSY 	0x01				// ġ����Ӳ�����ˁ�1ތӲ��0ތ���� 
#define  SERIAL_STU_FREE 	0x00			 
//------------------------------------------------------------------------------ 
//                  £����ƣ�� 
//------------------------------------------------------------------------------ 
#define 	CY18432M 
#ifdef 		CY18432M 
	#define 	B50 		0x0900				// DLM ތ�����ˁ�DLL ތ������ 
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
	#define 	B200 		0x0900				// DLM ތ�����ˁ�DLL ތ������ 
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
//           16C554 ��������ֿƣ�� 
//------------------------------------------------------------------------------ 
#define 		RHR 			0X00		// ����ڧ����������ֿ�� 					���� 
#define 		THR 			0X00		// ǀ��ڧ����������ֿ�� 					���� 
#define 		IER 			0X01		// ������������ֿ��     					���� 
#define 		FCR 			0X02		// FIFO ��������ֿ��						���� 
#define 		ISR 			0X02		// ������܍����ֿ��							���� 
#define 		LCR 			0X03		// ����������ֿ��							���� 
#define 		MCR 			0X04		// MODEM ��������ֿ��						���� 
#define 		LSR 			0X05		// ����܍����ֿ��							���� 
#define 		MSR 			0X06		// MODEM ��܍����ֿ��						���� 
#define 		SPR 			0X07		// ������ɞ��������ֿ,						������ 
#define 		DLL 			0X00		// £����ǀٜ������������ˁ�LCR bit7 ތ1,	������ 
#define 		DLM 			0X01		// £����ǀٜ������������ˁ�LCR bit7 ތ1��	������ 
 
#define FIFO_EN 				0X01		// FIFO ����ӝ���� 
#define FIFO_TRI_LEVEL_1 		0X00		// FIFO Ľǀ����ތ1ȳ���� 
#define FIFO_TRI_LEVEL_4 		0X40		// FIFO Ľǀ����ތ4ȳ���� 
#define FIFO_TRI_LEVEL_8 		0X80		// FIFO Ľǀ����ތ8ȳ���� 
#define FIFO_TRI_LEVEL_14 		0XC0		// FIFO Ľǀ����ތ14ȳ���� 
#define FIFO_DMA_MODE 			0X08		// FIFO DMA ԁ����ӝ���� 
#define FIFO_CLEAR_RCVR 		0X02		// ��ç����FIFO ��������ڧ�� 
#define FIFO_CLEAR_XMIT 		0X04		// ��çǀ��FIFO ��������ڧ�� 
 
#define IER_MODEM_INT 			0X08		// MODEM ��܍���������� 
#define IER_RECEIVELINE_INT 	0X04		// ��������܍���������� 
#define IER_TRANSMITHOLD_INT 	0X02		// ǀ����������ֿ���������� 
#define IER_RECEIVEHOLD_INT 	0X01		// ������������ֿ���������� 
 
#define LCR_WORD_LEN5 			0X00		// ڧ����ê��ތ5 
#define LCR_WORD_LEN6 			0X01		// ڧ����ê��ތ6 
#define LCR_WORD_LEN7 			0X02		// ڧ����ê��ތ7 
#define LCR_WORD_LEN8 			0X03		// ڧ����ê��ތ8 
#define LCR_STOP_LEN1 			0X00		// ݁����ê��ތ1 
#define LCR_STOP_LEN2 			0X04		// ݁����ê��ތ2 ���� ��ڧ����ތ5��݁����ތ1.5�� 
#define LCR_PARITY_EN 			0X08		// ���������������� 
#define LCR_PARITY_EVEN 		0X10		// EVEN�����������ˁ�ԍ����ϳ��ތ������ODD 
#define LCR_PARITY_SET 			0X20		// ������������������ 
#define LCR_OUT_BREAK 			0X40		// TX��óBREAK���� 
#define LCR_DIV_EN 				0X80		// ����ǀ��£���� 
#define LCR_DIV_Dis				0X00		// ͞��ǀ��£���� 
#define MCR_INTX_EN 			0X08		// ���������ó��������ӝ���� 
#define MCR_INTX_Dis 			0X00		// ���������ó͞������ӝ���� 
 
//------------------------------------------------------------------------------ 
//           16C554 ������̢ƣ�� 
//------------------------------------------------------------------------------ 
#define EVENT_LSR 				0X06		// LSR ��܍����ֿ��̢ 
#define EVENT_LSR_RECEIVE 		0X01		// ڧ��������������������ֿːFIFO�� 
#define EVENT_LSR_OVERRUN 		0X02		// ��������˧��ֿ����ڧ��ǀٜ������ރ��̢ 
#define EVENT_LSR_PARITY 		0X04		// ����ڧ��������ރ��̢ 
#define EVENT_LSR_FRAMING 		0X08		// ����ڧ��ǀٜ���ރ 
#define EVENT_LSR_BREAK 		0X10		// ��������ȳBREAK ��̢ 
#define EVENT_LSR_TRANHOLDEMPTY 0X20		// ǀ����������ֿːFIFO ����̢��������ڧ�� 
#define EVENT_LSR_TRANSMITEMPTY 0x40		// ǀ��ֿ����̢ 
#define EVENT_LSR_FIFOERROR 	0X80		// FIFO ������ރڧ�� 
#define EVENT_RXRDY 			0X04		// ڧ���������������̢ 
#define EVENT_RXRDY_TIMEOUT 	0X0C		// ����ڧ��Í��������̢ 
#define EVENT_TXRDY 			0X02		// ڧ��ǀ�����������̢ 
#define EVENT_MSR 				0X00		// MODEM ��܍������̢ 
#define FIFO_SIZE 				16 
 
//------------------------------------------------------------------------------------- 
//      16C554 ȿ����RAM���� 
//------------------------------------------------------------------------------------- 
#define STARTADDR_SERIAL1 		0X0000		// ġ����ݣ��1ֹ������ 
#define STARTADDR_SERIAL2 		0X0008		// ġ����ݣ��2ֹ������ 
#define STARTADDR_SERIAL3 		0X0010		// ġ����ݣ��3ֹ������ 
#define STARTADDR_SERIAL4 		0X0018		// ġ����ݣ��4ֹ������ 
 
//------------------------------------------------------------------------------------- 
//       ��۝Á�� 
//------------------------------------------------------------------------------------- 
#define   	Init_16c554_Count    	10 
#define 	FREE_RESET 				0 
#define 	RESET 					1 
//------------------------------------------------------------------------------------- 
//         ��ġ��������ڧ������ 
//------------------------------------------------------------------------------------- 
typedef struct st_CommHandle 
			   { 
				 Uint   RAMAddress_Map; 
				 Uchar  CommReceiveBuffer[CommMaxReceiveBuffer];  //ġ����������׼ 
				 Uchar  CommSendBuffer[CommMaxSendBuffer];        //ġ��ǀ������׼ 
				 Uchar  CommReceive_Head;                         //ġ����������׼������ 
				 Uchar  CommReceive_Trail;                        //ġ����������׼������ 
				 Uchar  CommSend_Head;                            //ġ��ǀ������׼������ 
				 Uchar  CommSend_Trail;                           //ġ��ǀ������׼������ 
			     Uchar  Comm_Stu;                                 //ġ����ǀ��ֿӲ����ڧ�� 
			     Uchar  Comm_Priority;                            //ġ������������׼ڧ��Ĳ����������ڧ�� 
			   };//end struct 
 
typedef struct st_CommHandle TCommHandle; 
//------------------------------------------------------------------------------------- 
//        ��ġ��16C554 קƏ����Ɏ��ʏڧ 
//------------------------------------------------------------------------------------- 
extern bool 	Init_16c554(void); 
extern void 	Port_open(Uchar PortID,Uint Baud); 
extern void 	Port_close(Uchar PortID); 
extern int  	Port_Receive(Uchar PortID,void *Buffer,int Count); 
extern int  	Port_Send(Uchar PortID,void *Buffer,int Count); 
//------------------------------------------------------------------------------------- 
#endif 
//------------------------------------------------------------------------------------- 