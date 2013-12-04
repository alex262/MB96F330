/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : applicfg.h
 *-------------------------------------------------------*
 *                                                       *      
 *                                                       *
 *********************************************************/

#ifndef __APPLICFG_HC12__
#define __APPLICFG_HC12__




/// Define the architecture : little_endian or big_endian
// -----------------------------------------------------
// Test :
// UNS32 v = 0x1234ABCD;
// char *data = &v;
//
// Result for a little_endian architecture :
// data[0] = 0xCD;
// data[1] = 0xAB;
// data[2] = 0x34;
// data[3] = 0x12;
//
// Result for a big_endian architecture :
// data[0] = 0x12;
// data[1] = 0x34;
// data[2] = 0xAB;
// data[3] = 0xCD;

// little_endian (Intel CPU)
//#define CANOPEN_LITTLE_ENDIAN

// big_endian (Motorola, CS12)
//#define CANOPEN_BIG_ENDIAN

// CANOPEN_LITTLE_ENDIAN (INTEL)
//#define CANOPEN_LITTLE_ENDIAN


// If defined, code is generated to print messages to the console
// (short or large message, depending on DEBUG_CAN)
// Should be defined
//#define DEBUG_ERR_CONSOLE_ON
//#define DEBUG_WAR_CONSOLE_ON

// If defined, a code is generated for large debuging messages on the console.
// Should not be defined. (waste time and memory)
//#define DEBUG_CAN

//for sending messages PDO_error 

#define  PDO_ERROR


// Use or not the PLL
//#define USE_PLL



// Several hardware definitions functions
// --------------------------------------

/// Convert an integer to a string in hexadecimal format
/// If you do not wants to use a lastCar, put lastCar = '\0' (end of string)
/// ex : value = 0XABCDEF and lastCar = '\n'
/// buf[0] = '0'
/// buf[1] = 'X'
/// buf[2] = 'A'
/// ....
/// buf[7] = 'F'
/// buf[8] = '\n'
/// buf[9] = '\0'
extern char *
hex_convert (char *buf, unsigned long value, char lastCar);


// Integers
#define INTEGER8
#define INTEGER16 int
#define INTEGER24
#define INTEGER32 long int
#define INTEGER40
#define INTEGER48
#define INTEGER56
#define INTEGER64
 
// Unsigned integers
#define UNS8   unsigned char
#define UNS16  unsigned short
#define UNS32  unsigned long
#define UNS24
#define UNS40
#define UNS48
#define UNS56
#define UNS64 unsigned __int64

// Reals
#define REAL32	float
#define REAL64  double

///The nodes states 
//-----------------
// values are choosen so, that they can be sent directly
// for heartbeat messages...
// Must be coded on 7 bits only
/* Should not be modified */
enum enum_nodeState {
  Initialisation  = 0x00, 
  Disconnected    = 0x01,
  Connecting      = 0x02,
  Preparing       = 0x02,
  Stopped         = 0x04,
  Operational     = 0x05,
  Pre_operational = 0x7F,
  Unknown_state   = 0x0F
};

typedef enum enum_nodeState e_nodeState;

//------------------------sending error message with PDO-------------------------
/*to send error message with PDO, before all the noeud must runing in operational
 state. 
 this PDO is send in event when an error has occured.
 this PDO contain the number of error "4 bytes" and the value of error "4 bytes":
*/

//the state alloude to send PDO is OPERTIONAL STATE
extern e_nodeState         nodeState;

// this function is used to send PDO with the value and number of error
extern UNS8 sendPDOevent( UNS8 bus_id, void * variable );



// the number of the error, to send in a PDO
// These variables are defined in the object dictionnary
extern UNS32 canopenErrNB ;

// the value, to send in a PDO
extern UNS32 canopenErrVAL;
//-------------------------------------------------------------------------------

// if not null, allow the printing of message to the console
// Could be managed by PDO
extern UNS8 printMsgErrToConsole;
extern UNS8 printMsgWarToConsole;
/// Definition of error and warning macros
// --------------------------------------


#ifdef DEBUG_CAN
#  define MSG(string, args) 
#else
#  define MSG(string, args)
#endif

/// Definition of MSG_ERR
// ---------------------


#ifdef DEBUG_ERR_CONSOLE_ON
#    define MSG_ERR(num, str, val) 	clrwdt;sprintf(B_Out,"0x%X ",num); puts(B_Out); \
									clrwdt;sprintf(B_Out,str); puts(B_Out);		\
									clrwdt;sprintf(B_Out," 0x%X \n\r",val); puts(B_Out);
#    define MSG_ERR_long(num, str, val)  clrwdt;sprintf(B_Out,"0x%X ",num); puts(B_Out); \
									clrwdt;sprintf(B_Out,str); puts(B_Out);		\
									clrwdt;sprintf(B_Out," 0x%lX\n\r",val); puts(B_Out);            
#else
#  	define MSG_ERR(num, str, val)  
#   define MSG_ERR_long(num, str, val)           
#endif


/// Definition of MSG_WAR
// ---------------------
#ifdef DEBUG_WAR_CONSOLE_ON
#    define MSG_WAR(num, str, val)  clrwdt;sprintf(B_Out,"0x%X ",num); puts(B_Out); \
									clrwdt;sprintf(B_Out,str); puts(B_Out);		\
									clrwdt;sprintf(B_Out," 0x%X \n\r",val); puts(B_Out);   
#    define MSG_WAR_long(num, str, val)  clrwdt;sprintf(B_Out,"0x%X ",num); puts(B_Out); \
									clrwdt;sprintf(B_Out,str); puts(B_Out);		\
									clrwdt;sprintf(B_Out," 0x%lX\n\r",val); puts(B_Out);     
#  else
#  define MSG_WAR(num, str, val) 
#  define MSG_WAR_long(num, str, val)
#endif



#endif
