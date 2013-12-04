/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : def.h
 *********************************************************/

#ifndef __def_h__
#define __def_h__

#include "applicfg.h"


//#include "can.h"

/** definitions used for object dictionary access. ie SDO Abort codes . 
(See DS 301 v.4.02 p.48) */

#define OD_SUCCESSFUL 	             0x00000000
#define OD_READ_NOT_ALLOWED          0x06010001
#define OD_WRITE_NOT_ALLOWED         0x06010002
#define OD_NO_SUCH_OBJECT            0x06020000
#define OD_LENGTH_DATA_INVALID       0x06070010
#define OD_NO_SUCH_SUBINDEX 	     0x06090011

// En attendant ...

#define SUCCESSFUL 		OD_SUCCESSFUL
#define NO_SUCH_INDEX 	0x12

/** If someone/something tries to access an object dictionary
 * subentry (subindex) which doesn't exist*/
 
//#define SUBINDEX_NOT_EXIST   	     0x13


/** lifeguard: connection is OK */

#define CONNECTION_OK		     0x33

/** lifeguard: connection failed */

#define CONNECTION_FAILED 	     0x31

/** lifeguard: guarding message lost */

#define GUARDING_MESSAGE_LOST	     0x32

/** lifeguard: heartbeat message lost */

#define HEARTBEAT_MESSAGE_LOST	     0x34

/** lifegurad: heartbeat consumer lost */

#define HEARTBEAT_CONSUMER_LOST      0x35

/******************** CONSTANTES ****************/
/** Number of nodes which sends heartbeat nmt messages to the master
 This value can be changed. (0 is not allowed)*/

#define NB_OF_HEARTBEAT_PRODUCERS 10

/** Constantes which permit to define if a PDO frame
   is a request one or a data one*/
/* Should not be modified */

#define REQUETE 1
#define DONNEES 0

/// Misc constants
// --------------
/* Should not be modified */

#define Rx 0
#define Tx 1
#define TRUE  1
#define FALSE 0

/** Number of can bus to use */
/* Can be modified */

#define MAX_CAN_BUS_ID     2//1 

/* Should not be modified */

/// max bytes to transmit. Fixed in DS301

#define SDO_MAX_DOMAIN_LEN 32    
#define SDO_MAX_NODE_ID    2

/// max bytes of data in a PDO

#define PDO_MAX_LEN        8    

/** Used for NMTable[bus][nodeId]	  
 You can put less of 128 if on the netwo
 are connected only smaller nodeId node.*/

#define NMT_MAX_NODE_ID    32  
                           
/*Status of the SDO transmission */

#define SDO_SUCCESS     0x1   // data are available                            
#define	SDO_ABORTED     0x80  // Received an abort message. Data not available 
#define	SDO_IN_PROGRESS 0x0   // Receiving the data.                       

/*  Function Codes 
  --------------- 
  This is the default configuration, 
  defined in the canopen DS301 
  It can be modified, if you wants 
  to change the priority of theses messages */

#define NMT	   	   0x0
#define SYNC       0x1
#define TIME_STAMP 0x2
#define PDO1       0x3
#define PDO2       0x4
#define PDO3       0x5
#define PDO4       0x6
#define PDO5       0x7
#define PDO6       0x8
#define PDO7       0x9
#define PDO8       0xA
#define SDOtx      0xB
#define SDOrx      0xC
#define NODE_GUARD 0xE

/// NMT Command Specifier, sent by master to change a slave state
// -------------------------------------------------------------
/* Should not be modified */

#define NMT_Start_Node              0x01
#define NMT_Stop_Node               0x02
#define NMT_Enter_PreOperational    0x80
#define NMT_Reset_Node              0x81
#define NMT_Reset_Comunication      0x82

/// constantes used in the different state machines
// -----------------------------------------------
/* Must not be modified */

#define state1  0x01
#define state2  0x02
#define state3  0x03
#define state4  0x04
#define state5  0x05
#define state6  0x06
#define state7  0x07
#define state8  0x08
#define state9  0x09
#define state10 0x0A
#define state11 0x0B

/// SDO Command Specifier Patterns
// ------------------------------
/* Should not be modified */
///Initiate Domain Upload request

#define IDU_client  0x40

///Initiate Domain Download request

#define IDD_serv    0x60

///Initiate Domain Download response

#define IDD_server  0x60

///Domain Download Segment response

#define DDS_server  0x20

///Initiate Domain Upload response

#define IDU_server  0x40

///Domain Upload Segment response

#define UDS_server  0x00

///Initiate Domain Upload response

#define IDU_cli     0x40

///Domain Upload Segment response

#define UDS_cli     0x60

///Domain Download Segment response

#define DDS_cli     0x00

///Initiate Domain Download response

#define IDD_cli     0x20

/// Different SDO transfer states
/* Must not be modified */
/// Bits 0-1 (working state) 

#define TS_INACTIV      0x00
#define TS_ACTIVATED	0x01
#define TS_WORKING	 	0x03
#define TS_ERROR	 	0x02

/// Bits 0-2 (Action)

#define TS_FREE		 	0x00
#define TS_DOWNLOAD	 	0x01
#define TS_DOWNLOADING	0x02
#define TS_FINISHED	 	0x04
#define TS_UPLOAD	 	0x05
#define TS_UPLOADING	0x06

/// Bit 3

#define TS_WAIT_SERVER	 0x08
#define TS_TOGGLE	 0x10

/******************* STRUCTURES *********************/

/** The PDO structure */
//struct struct_s_PDO 
//{
//	UNS32 cobId;	 			// COB-ID
//	UNS8  len;	  	// Number of data transmitted (in data[])
//	UNS8  data[8]; 	// Contain the data
//};

typedef struct s_Message s_PDO;

/* The Transfer structure
Used to store the different segments of 
 - a SDO received before writing in the dictionary  
 - the reading of the dictionary to put on a SDO to transmit */

struct struct_s_transfer 
{
	UNS8           nodeId;     
	//own ID if server, or node ID of the server if client
	
	// state : state of the reception
	// Take the value of the constantes above TS_XXX
	UNS8           state; 
  
	// index and subindex of the dictionary where to store
	// (for a received SDO) or to read (for a transmit SDO)
	
	UNS16 index; 
	UNS8  subindex; 
	UNS8           count;  // Number of data

	// offset :  Number of data received. So incremented during the transmission

	UNS8           offset; 
	UNS8           data [SDO_MAX_DOMAIN_LEN];
};
typedef struct struct_s_transfer s_transfer;

/* The process_var structure
 Used to store the PDO before the transmission or the reception.*/

struct struct_s_process_var 
{
	UNS8 state; 	// To prepare a pdo and delay the transmission 
					// (Obsolet. Probably not used)
	UNS8 count; 	// Size of data. Ex : for a PDO of 6 bytes of data, count = 6
	UNS8 data[PDO_MAX_LEN];
};

typedef struct struct_s_process_var s_process_var;

/// Used to read in a dictionary.

typedef struct Read 
{
	UNS8 state; // SDO_SUCCESS data are available                             
				// SDO_ABORT :  Received an abort message. Data not available 
				// SDO_IN_PROGRESS Receiving the data.                        
	UNS8 data[SDO_MAX_DOMAIN_LEN]; // The data read
	UNS8 size; // The size of the data
} ReadDic;

/// Used to trdy the write operation in a dictionary.

typedef struct Write 
{
	UNS8 state;  // SDO_SUCCESS data are available                             
				// SDO_ABORT :  Received an abort message. Data not available 
				// SDO_IN_PROGRESS Receiving the data.              
} WriteDic;

struct stuct_s_SDOabort 
{
	UNS16 index;     // Index at which abort occurs
	UNS8  subIndex;  // subIndex at which abort occurs
	UNS32 abortCode; // Abort code (SDO abort protocol)      
};
typedef struct stuct_s_SDOabort s_SDOabort;

/// The 8 bytes data of the SDO

struct BODY
{
	UNS8 SCS;		//SDO command specifier
    UNS8 data[7];
};

/// The SDO structure ...

struct struct_s_SDO 
{
  UNS8 nodeId;		//Node ID
  UNS8 len;			//body len
  struct BODY body;
};

typedef struct struct_s_SDO s_SDO;

/*************************** MACROS *************************/
// SDO (un)packing macros
///Initiate Domain Download request

/** make the cs from the parameters n,e,s,t,c */

#define IDD_client(n,e,s) \
   (0x20 | ((n & 0x03) << 2) | ((e & 0x01) << 1) | (s & 0x01))

/** Domain Download Segment request*/

#define DDS_client(t,n,c) \
   (0x00 | ((t & 0x01) << 4) | ((n & 0x07) << 1) | (c & 0x01))

/** Domain Upload Segment request */

#define UDS_client(t) 		(0x60 | ((t & 0x01) << 4))

/** Domain Download Segment request */

#define DDS_serv(t) 		(0x20 | ((t & 0x01) << 4)) 

/** Initiate Domain Upload request */

#define IDU_serv(n,e,s) \
   (0x40 | ((n & 0x03) << 2) | ((e & 0x01) << 1) | (s & 0x01)) 

/** Domain Upload Segment request */

#define UDS_serv(t,n,c) \
   (0x00 | ((t & 0x01) << 4) | ((n & 0x07) << 1) | (c & 0x01))

/* get the different parameter, n,e,s,t,c from the cs 
*/
/** Initiate Domain Upload Server parameters */

#define IDU_server_params(cs,n,e,s) \
    n = (cs & 0x0c) >> 2; e = (cs & 0x02) >> 1; s = cs & 0x01;

///Upload Domain Segmented Server parameters

#define UDS_server_params(cs,t,n,c) \
   t = (cs & 0x10) >> 4; n = (cs & 0x0e) >> 1; c = cs & 0x01;

#define UDS_cli_params(cs,t)  t = (cs & 0x10) >>4;

#define DDS_cli_params(cs,t,n,c) \
   t = (cs & 0x10) >> 4; n = (cs & 0x0e) >> 1; c = cs & 0x01;

#define IDD_cli_params(cs,n,e,s) \
   n = (cs & 0x0c) >> 2; e = (cs & 0x02) >> 1; s = cs & 0x01;

#define IS_SEGMENTED_RESPONSE(SCS)	!(SCS & 0x40)
#define IS_DOWNLOAD_RESPONSE(SCS)	(SCS & 0x20)
#define IS_UPLOAD_REQUEST(SCS)		(SCS & 0x40)

#define IS_SEGMENTED_REQUEST(SCS)	\
    (!(SCS & 0x40) && !(SCS & 0x20)) || ((SCS & 0x40) && (SCS & 0x20)) 	

#define IS_ABORTED_REQUEST(SCS) (SCS == 0x80)

/// Different SDO transfer states
/// Bits 0-1 (working state) 

#define TS_ACTIVITY(state)		(state & 0x03)
#define TS_IS_ON(state)			(state & 0x01)

#define TS_SET_ACTIVITY(state, activity)  \
     state = ((state & ~0x03) | activity);

/// Bits 0-2 (Action)

#define TS_IS_DOWNLOAD(state)	        !(state & 0x04)

/// Bit 3

#define TS_HAVE_TO_DO(state)	        !(state & 0x08)

#endif // __def_h__