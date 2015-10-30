#ifndef __CAN__
#define __CAN__
#include "global.h"
//------------------------- зарегистрированные скорости CAN -----------------------------------
// Predefined CAN bit timings
/*-----------------------------------------------------------------------------
             C   B    T S  R       // CLK  = CAN-Clock Speed
             L   A    Q P  S       // Baud = Baudrate
             K   U         J       // TQ   = Number of Time Quanta
                 D         W       // SP   = Sample Point Position [%]
                                   // RSJW = Resynchronisation Jump Width [TQ]
-----------------------------------------------------------------------------*/
#define BTR_16M_20k8_24_66_4 0x7EDF     // BTR config 20.83 kBaud 
#define BTR_16M_33K0_22_68_4 0x6DD5     // BTR config 33.05 kBaud 
#define BTR_16M_33K3_20_70_4 0x5CD7     // BTR config 33.33 kBaud 
#define BTR_16M_83K3_24_66_4 0x7EC7     // BTR config 83.33 kBaud 
#define BTR_16M_100k_20_70_4 0x5CC7     // BTR config 100.0 kBaud 
#define BTR_16M_125k_16_68_3 0x4987     // BTR config 125.0 kBaud 
#define BTR_16M_500k_16_68_3 0x4981     // BTR config 500.0 kBaud 
#define BTR_16M_1M00_16_68_3 0x4980     // BTR config   1.0 MBaud 
//-----------------------------------------------------------------------------
// Macros for ID-Transfer from/to the Arbitration Registers
//-----------------------------------------------------------------------------
#define MSG2STD(msg) ((msg & 0x000007FFL) << 18)
#define MSG2EXT(msg) (msg & 0x1FFFFFFFL)
#define STD2MSG(reg) (((reg & 0x1FFFFFFFL) >> 18) & 0x000007FFL)
#define EXT2MSG(reg) (reg & 0x1FFFFFFFL)

#define CAN_125 	BTR_16M_125k_16_68_3
#define CAN_500		BTR_16M_500k_16_68_3
#define CAN_1000	BTR_16M_1M00_16_68_3
//------------------------------- Порты CAN контроллера -----------------------------------
//------------------------------- Типы масок идентификаторов ----------------------------------
//------------------------------- Типы сообщений --------------------------
#define CANMSG_MINE			0x0			// индивидуальное сообщение
#define CANMSG_BROADCAST	0x1			// широковещательное сообщение
//------------------------------ Типы ящиков --------------------------------------
#define MSGBOX_RX			0x0			// Ящик для приема сообщений
#define MSGBOX_TX			0x1			// ящик для отправки сообщений
#define MSGBOX_RF			0x2			// ящик для приема удаленных фреймов
#define CANMSG_TRANSMIT		0x5			// сообщение на передачу

#define DIR_RX				0x0			// ящик приемник
#define DIR_TX				0x1			// ящик передатчик


typedef struct
{
	unsigned int id;
	unsigned char box;
	unsigned char dlc;
	unsigned char *msg;
}TSendMsgInfo;


/** Can message structure */
struct s_Message  
{
	U32 cob_id;	// l'ID du mesg
	U8 rtr;		// remote transmission request. 0 if not rtr, 
	// 1 for a rtr message
	U8 len;		// message length (0 to 8)
	U8 data[8];	// data 
};

typedef struct s_Message Message;
//------------------------------------------------------------------------------

void SetupCAN(U16 BaudRate0, U16 BaudRate1, U16 BaudRate2);

U8 CAN_SendMessage(U8 NumCan, Message *msg);
#define f_can_send CAN_SendMessage

U8 CAN_ReceiveMessage(U8 nCAN, Message *m);
void DrawCanStatus(BYTE i);

void __interrupt CAN_0_IRQ(void);
void __interrupt CAN_1_IRQ(void);
void __interrupt CAN_2_IRQ(void);



#endif