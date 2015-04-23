
/**************************************************************************/
/* Object Dictionary for the node Linux_slave, default node_id : 0x01       */
/*                                                                        */
/* Computed by "makeobjetdict.php"                                        */
/**************************************************************************/
/* Computed by strComputed */

/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *

 *********************************************************
           File : objdict.c
 *-------------------------------------------------------*
 * This is where you defined the dictionary of your      *
 *  application                                          *      
 *                                                       *
 *********************************************************/
 /* Computed by strEntete */

//#define DEBUG_CAN

#include <stddef.h>
#include <global.h>
#include INCLUDE_H

#include "def.h"
#include "pdo.h"
#include "sdo.h"
#include "sync.h"
#include "objdictdef.h"
#include "nmtSlave.h"
#include "lifegrd.h"
#include "bootloader.h"

/**************************************************************************/
/* The node id                                                            */
/**************************************************************************/
/* Computed by strNode */
/* node_id default value. 
   You should always overwrite this by using the function setNodeId(UNS8 nodeId) in your C code.
*/
#define NODE_ID 0x02

/**************************************************************************/
/* Declaration of the mapped variables                                    */
/**************************************************************************/
/* Computed by strDeclareMapVar */
//UNS32 canopenErrNB;	// Mapped at index 0x6000, subindex 0x0
//UNS32 canopenErrVAL;// Mapped at index 0x6001, subindex 0x0

//*****************************************************************************/
/* Computed by strStartDico */

/* Array of message processing information */
/* Should not be modified */
const proceed_info proceed_infos[] = 
{
	{NMT,		/*"NMT",*/			proceedNMTstateChange},		//0
	{SYNC,		/*"SYNC"*/			proceedSYNC},				//1
	{TIME_STAMP,/*"TIME_STAMP"*/	0},							//2
	{PDO1,		/*"PDO1tx",*/		proceedPDO},				//3		1
	{PDO2,		/*"PDO1rx",*/		proceedPDO},				//4		2
	{PDO3,		/*"PDO2tx",*/		proceedPDO},				//5		3
	{PDO4,		/*"PDO2rx",*/		proceedPDO},				//6		4
	{PDO5,		/*"PDO3tx",*/		proceedPDO},				//7		5
	{PDO6,		/*"PDO3rx",*/		proceedPDO},				//8		6
	{PDO7,		/*"PDO4tx",*/		proceedPDO},				//9		7
	{PDO8,		/*"PDO4rx",*/		proceedPDO},				//10	8
	{SDOtx,		/*"SDOtx",*/		proceedSDO},				//11
	{SDOrx,		/*"SDOrx",*/		proceedSDO},				//12
	{0xD,		/*"BOOTLOADER",*/	ServiceBootloadUpd},		//13
	{NODE_GUARD,/*"NODE GUARD",*/	proceedNMTerror},			//14
	{0xF,		/*"Unknown",*/		0},							//15
	{0x10,		/*"PDO5",*/			proceedPDO},				//16	9
	{0x11,		/*"PDO6",*/			proceedPDO},				//17	10
	{0x12,		/*"PDO7",*/			proceedPDO},				//18	11
	{0x13,		/*"PDO8",*/			proceedPDO},				//19	12
	{0x14,		/*"PDO9",*/			proceedPDO},				//20	13
	{0x15,		/*"PDO10",*/		proceedPDO},				//21	14
	{0x16,		/*"PDO11",*/		proceedPDO},				//22	15
	{0x17,		/*"PDO12",*/		proceedPDO},				//23	16
	{0x18,		/*"PDO13",*/		proceedPDO},				//24	17
	{0x19,		/*"PDO14",*/		proceedPDO},				//25	18
	{0x1A,		/*"PDO15",*/		proceedPDO},				//26	19
	{0x1B,		/*"PDO",*/			proceedPDO},				//27	20
	{0x1C,		/*"PDO15",*/		proceedPDO},				//28	21
	{0x1D,		/*"PDO15",*/		proceedPDO},				//29	22
	{0x1E,		/*"PDO15",*/		proceedPDO},				//30	23
	{0x1F,		/*"PDO15",*/		proceedPDO},				//31	24
	{0x20,		/*"PDO15",*/		proceedPDO},				//32	25
	{0x21,		/*"PDO15",*/		proceedPDO},				//33	26
	{0x22,		/*"PDO15",*/		proceedPDO},				//34	27
	{0x23,		/*"PDO15",*/		proceedPDO},				//35	28
	{0x24,		/*"PDO15",*/		proceedPDO},				//36	29
	{0x25,		/*"PDO15",*/		proceedPDO},				//37	30
	{0x26,		/*"PDO15",*/		proceedPDO},				//38	31

	{0x27,	/*	"Unknown",*/		0},							//39
	{0x28,	/*	"Unknown",*/		0},							//40
	{0x29,	/*	"Unknown",*/		0},							//41
	{0x2A,	/*	"Unknown",*/		0},							//42
	{0x2B,	/*	"Unknown",*/		0},							//43
	{0x2C,	/*	"Unknown",*/		0},							//44
	{0x2D,	/*	"Unknown",*/		0},							//45
	{0x2E,	/*	"Unknown",*/		0},							//46
	{0x2F,	/*	"Unknown",*/		0},							//47
	{0x30,	/*	"Unknown",*/		0},							//48
	{0x31,	/*	"Unknown",*/		0},							//49
	{0x32,	/*	"Unknown",*/		0},							//50
	{0x33,	/*	"Unknown",*/		0},							//51
	{0x34,	/*	"Unknown",*/		0},							//52
	{0x35,	/*	"Unknown",*/		0},							//53
	{0x36,	/*	"Unknown",*/		0},							//54
	{0x37,	/*	"Unknown",*/		0},							//55
	{0x38,	/*	"Unknown",*/		0},							//56
	{0x39,	/*	"Unknown",*/		0},							//57
	{0x3A,	/*	"Unknown",*/		0},							//58
	{0x3B,	/*	"Unknown",*/		0},							//59
	{0x3C,	/*	"Unknown",*/		0},							//60
	{0x3D,	/*	"Unknown",*/		ServiceObmenData1},			//61
	{0x3E,	/*	"Unknown",*/		ServiceObmenData2},			//62
	{0x3F,	/*	"Unknown",*/		ServiceMaster}				//63
};
const UNS8 Num_Proc_Max=sizeof(proceed_infos)/sizeof(proceed_info)-1;



//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//
//                       OBJECT DICTIONARY
//                   
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// Make your change, depending of your application
 
/** index 1000h: device type. You have to change the value below, so
 *  it fits your canopen-slave-module
 */
/* Not used, so, should not be modified */
#define OBJNAME devicetype
const UNS32 OBJNAME = DEVICE_TYPE;

const subindex Index1000[] =
{
  { RO, uint32, sizeof(UNS32), (void*)&OBJNAME }
};
#undef OBJNAME

/** index 1001: error register. Change the entries to fit your application
 */
/* Not used, so, should not be modified */
#define OBJNAME errorRegister
/*const*/ UNS8 OBJNAME = 0x0;
const subindex Index1001[] =
{
  { RO, uint8, sizeof(UNS8), (void*)&OBJNAME }
};
#undef OBJNAME

/** index 1005: COB_ID SYNC
*/
/* Should not be modified */
#define OBJNAME CobIdSync
UNS32 OBJNAME = 0x40000080; // bit 30 = 1 : device can generate a SYNC message
const subindex Index1005[] =
{
  { RW, uint32, sizeof(UNS32), (void*)&OBJNAME }
};
#undef OBJNAME

/** index 1006: SYNC period
*/

#define OBJNAME SyncPeriod
// For producing the SYNC signal every n micro-seconds.
// Put O to not producing SYNC
/*const*/ UNS32 OBJNAME = 0x0; /* Default 0 to not produce SYNC */
const subindex Index1006[] =
{
  { RW, uint32, sizeof(UNS32), (void*)&OBJNAME }
};
#undef OBJNAME
//==================================================================================================
/** index 1008: _азва-и_ устрRcства*/

#define OBJNAME Device_Name
const char OBJNAME[] = NAME_MODUL;

const subindex Index1008[] =
{
  { RO, visible_string, sizeof(OBJNAME), (void*)&OBJNAME }
};
#undef OBJNAME

/** index 100A: '_рси   рR_раммы*/
#define OBJNAME SoftwareVer
//const char OBJNAME[] = { __TIME__" " __DATE__}; 
const subindex Index100A[] =
{
  { RO, visible_string, sizeof(OBJNAME), (void*)&OBJNAME }
};
#undef OBJNAME

//=================================================================================================
/** index 100B : The node Id number
 */
/* Value may be modified */
#define OBJNAME bDeviceNodeId
UNS8 bDeviceNodeId = NODE_ID;
subindex Index100B[] =
{
  { RW, uint8, sizeof(UNS8), (void*) &OBJNAME }
};
#undef OBJNAME

/**************************************************************************/
/* HeartBeat consumers : The nodes which can send a heartbeat             */
/**************************************************************************/
/* Computed by strHeartBeatConsumers */
//static  UNS32 HBConsumerTimeArray[] = {0x00000000}; // Format 0x00NNTTTT (N=Node T=time in ms)

//static  UNS8 HBConsumerCount = 1; // 1 nodes could send me their heartbeat.
/*
subindex Index1016[] = {
  { RO, uint8, sizeof(UNS8), (void*)&HBConsumerCount },
  { RW, uint32, sizeof(UNS32), (void*)&HBConsumerTimeArray[0] }};

/**************************************************************************/
/* The node produce an heartbeat                                          */
/**************************************************************************/
/* Computed by strHeartBeatProducer */
/* Every HBProducerTime, the node sends its heartbeat */
static UNS16 HBProducerTime = 0;  /* in ms. If 0 : not activated */ 
const subindex Index1017[] =
{
	{ RW, uint16, sizeof(UNS16), &HBProducerTime }
};

/**************************************************************************/
/* Next to 0x1018                                                 */
/**************************************************************************/
/* Computed by strVaria1 */
/** index 1018: identify object. Adjust the entries for your node/company
 */
/* Values can be modified */
#define OBJNAME theIdentity
const s_identity OBJNAME =
{
	16,       		// number of supported entries
	0x1234,  		// Vendor-ID (given by the can-cia)
	0x5678,  		// Product Code
	0x1364,  		// Revision number
	0x79641234,  	// serial number
} ;

UNS16	ID_PROG_FLASH;
/* Should not be modified */
const subindex Index1018[] =
{
	{ RO, uint8,  sizeof(UNS8),  (void*)&OBJNAME.count },					// 0
	{ RO, uint32, sizeof(UNS32), (void*)&OBJNAME.vendor_id},				// 1
	{ RO, uint32, sizeof(UNS32), (void*)&OBJNAME.product_code},				// 2
	{ RO, uint32, sizeof(UNS32), (void*)&OBJNAME.revision_number},			// 3
	{ RO, uint32, sizeof(UNS32), (void*)&OBJNAME.serial_number},			// 4
	{ RW, uint16, sizeof(UNS16), (void*)&ID_PROG_FLASH},					// 5
 
	{ RO, uint8, sizeof(UNS8), (void*)&program.Cnt1WareDev},				// 6

	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[0][0]},		// 7
	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[0][4]},		// 8

	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[1][0]},		// 9
	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[1][4]},		// 10

	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[2][0]},		// 11
	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[2][4]},		// 12

	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[3][0]},		// 13
	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[3][4]},		// 14

	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[4][0]},		// 15
	{ RO, uint32, sizeof(UNS32), (void*)&program.SN_1Ware_Dev[4][4]}		// 16
};
#undef OBJNAME

/** now the communication profile entries are grouped together, so they
 *  can be accessed in a standardised manner. This could be memory-optimized
 *  if the empty entries wouldn't be added, but then the communication profile
 *  area must be accessed different (see objacces.c file)
 */
/* Should not be modified */
__far const indextable CommunicationProfileArea[] =
{
  DeclareIndexTableEntry(Index1000), // creates a line like: { Index1000, 1 },
  DeclareIndexTableEntry(Index1001),
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  DeclareIndexTableEntry(Index1005),
  DeclareIndexTableEntry(Index1006),
  { NULL, 0 },
  DeclareIndexTableEntry(Index1008),
  { NULL, 0 },
  DeclareIndexTableEntry(Index100A),
  DeclareIndexTableEntry(Index100B),
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },//DeclareIndexTableEntry(Index1016),
  DeclareIndexTableEntry(Index1017),
  DeclareIndexTableEntry(Index1018),
};

/**************************************************************************/
/* The SDO Server parameters                                              */
/**************************************************************************/
/* Computed by strSdoServer */
/* BEWARE You cannot define more than one SDO server */


#define INDEX_LAST_SDO_SERVER           0x1200
#define DEF_MAX_COUNT_OF_SDO_SERVER     INDEX_LAST_SDO_SERVER - 0x11FF

/* The values should not be modified here, but can be changed at runtime */
#define OBJNAME serverSDO1
static s_sdo_parameter OBJNAME = 
{ 3,                   // Number of entries. Always 3 for the SDO	       
  0x600 + NODE_ID,     // The cob_id transmited in CAN msg to the server     
  0x580 + NODE_ID,     // The cob_id received in CAN msg from the server  
  NODE_ID              // The node id of the client. Should not be modified 
};
static subindex Index1200[] =
{
  { RO, uint8,  sizeof( UNS8 ), (void*)&OBJNAME.count },
  { RO, uint32, sizeof( UNS32), (void*)&OBJNAME.cob_id_client },
  { RO, uint32, sizeof( UNS32), (void*)&OBJNAME.cob_id_server },
  { RW, uint8,  sizeof( UNS8),  (void*)&OBJNAME.node_id }
};
#undef OBJNAME

/** Create the server SDO Parameter area.
 */
/* Should not be modified */
__far const indextable serverSDOParameter[] =
{
  DeclareIndexTableEntry(Index1200)
};

/**************************************************************************/
/* The SDO(s) clients                                                     */
/**************************************************************************/
/* Computed by strSdoClients */
/* For a slave node, declare only one SDO client to send data to the master */
/* The master node must have one SDO client for each slave */
#define INDEX_LAST_SDO_CLIENT           0x1280
#define DEF_MAX_COUNT_OF_SDO_CLIENT     1

#define _CREATE_SDO_CLIENT_(SDONUM) \
static  s_sdo_parameter clientSDO ## SDONUM = \
{ 3,    /* Number of entries. Always 3 for the SDO*/\
0x580,  /* The cob_id transmited in CAN msg to the server*/\
0x600,  /* The cob_id received in CAN msg from the server*/\
0x00    /* The node id of the client. Should not be modified */\
};\
static  subindex Index ## SDONUM  [] =\
{\
	{ RO, uint8, sizeof( UNS8 ), (void*)&clientSDO ## SDONUM.count },\
	{ RO, uint32, sizeof( UNS32), (void*)&clientSDO ## SDONUM.cob_id_client },\
	{ RO, uint32, sizeof( UNS32), (void*)&clientSDO ## SDONUM.cob_id_server },\
	{ RO, uint8, sizeof( UNS8), (void*)&clientSDO ## SDONUM.node_id }\
};

_CREATE_SDO_CLIENT_(1280)

/* Create the client SDO Parameter area. */
__far const indextable   clientSDOParameter[] =
{
  DeclareIndexTableEntry(Index1280)
};

/**************************************************************************/
/* The PDO(s) Which could be received                                     */
/**************************************************************************/
/* Computed by strPdoReceive */
#define INDEX_LAST_PDO_RECEIVE  0x141F
#define MAX_COUNT_OF_PDO_RECEIVE 31

#define _CREATE_RXPDO_(RXPDO) \
static  s_pdo_communication_parameter RxPDO ## RXPDO = \
{ 2,      /* Number of entries. Always 2*/\
0x000,  /* Default cobid*/\
0x00    /* Transmission type. See objetdictdef.h*/\
};\
static const subindex Index ## RXPDO[] =\
{\
	{ RO, uint8, sizeof( UNS8 ), (void*)&RxPDO ## RXPDO.count },\
	{ RW, uint32, sizeof( UNS32), (void*)&RxPDO ## RXPDO.cob_id },\
	{ RW, uint8, sizeof( UNS8), (void*)&RxPDO ## RXPDO.type }\
};

// This define the PDO receive entries from index 0x1400 to 0x1402 
_CREATE_RXPDO_(1400)
_CREATE_RXPDO_(1401)
_CREATE_RXPDO_(1402)
_CREATE_RXPDO_(1403)
_CREATE_RXPDO_(1404)
_CREATE_RXPDO_(1405)
_CREATE_RXPDO_(1406)
_CREATE_RXPDO_(1407)
_CREATE_RXPDO_(1408)
_CREATE_RXPDO_(1409)
_CREATE_RXPDO_(140A)
_CREATE_RXPDO_(140B)
_CREATE_RXPDO_(140C)
_CREATE_RXPDO_(140D)
_CREATE_RXPDO_(140E)
_CREATE_RXPDO_(140F)
_CREATE_RXPDO_(1410)
_CREATE_RXPDO_(1411)
_CREATE_RXPDO_(1412)
_CREATE_RXPDO_(1413)
_CREATE_RXPDO_(1414)
_CREATE_RXPDO_(1415)
_CREATE_RXPDO_(1416)
_CREATE_RXPDO_(1417)
_CREATE_RXPDO_(1418)
_CREATE_RXPDO_(1419)
_CREATE_RXPDO_(141A)
_CREATE_RXPDO_(141B)
_CREATE_RXPDO_(141C)
_CREATE_RXPDO_(141D)
_CREATE_RXPDO_(141E)
_CREATE_RXPDO_(141F)

/* Create the Receive PDO Parameter area. */
__far const indextable   receivePDOParameter[] =
{ 
	DeclareIndexTableEntry(Index1400),
	DeclareIndexTableEntry(Index1401),
	DeclareIndexTableEntry(Index1402),
	DeclareIndexTableEntry(Index1403),
	DeclareIndexTableEntry(Index1404),
	DeclareIndexTableEntry(Index1405),
	DeclareIndexTableEntry(Index1406),
	DeclareIndexTableEntry(Index1407),
	DeclareIndexTableEntry(Index1408),
	DeclareIndexTableEntry(Index1409),
	DeclareIndexTableEntry(Index140A),
	DeclareIndexTableEntry(Index140B),
	DeclareIndexTableEntry(Index140C),
	DeclareIndexTableEntry(Index140D),
	DeclareIndexTableEntry(Index140E),
	DeclareIndexTableEntry(Index140F),
	DeclareIndexTableEntry(Index1410),
	DeclareIndexTableEntry(Index1411),
	DeclareIndexTableEntry(Index1412),
	DeclareIndexTableEntry(Index1413),
	DeclareIndexTableEntry(Index1414),
	DeclareIndexTableEntry(Index1415),
	DeclareIndexTableEntry(Index1416),
	DeclareIndexTableEntry(Index1417),
	DeclareIndexTableEntry(Index1418),
	DeclareIndexTableEntry(Index1419),
	DeclareIndexTableEntry(Index141A),
	DeclareIndexTableEntry(Index141B),
	DeclareIndexTableEntry(Index141C),
	DeclareIndexTableEntry(Index141D),
	DeclareIndexTableEntry(Index141E),
	DeclareIndexTableEntry(Index141F)
};

/**************************************************************************/
/* The PDO(s) Which could be transmited                                   */
/**************************************************************************/
/* Computed by strPdoTransmit */
#define INDEX_LAST_PDO_TRANSMIT  0x181F
#define MAX_COUNT_OF_PDO_TRANSMIT 31
/** Usually the ID of a transmitting PDO is 0x180 + device_node_id,
*  but the node_id is not known during compilation... so what to do?!
*  the correct values have to be setted up during bootup of the device...
*/
#define _CREATE_TXPDO_(TXPDO) \
static  s_pdo_communication_parameter TxPDO ## TXPDO = \
{ 2,      /* Number of entries. Always 2*/\
0x000,  /* Default cobid*/\
0x00    /* Transmission type. See objetdictdef.h*/\
};\
static const subindex Index ## TXPDO[] =\
{\
	{ RO, uint8, sizeof( UNS8 ), (void*)&TxPDO ## TXPDO.count },\
	{ RW, uint32, sizeof( UNS32), (void*)&TxPDO ## TXPDO.cob_id },\
	{ RW, uint8, sizeof( UNS8), (void*)&TxPDO ## TXPDO.type }\
};

// This define the PDO receive entries from index 0x1800 to 0x1802 
_CREATE_TXPDO_(1800)
_CREATE_TXPDO_(1801)
_CREATE_TXPDO_(1802)
_CREATE_TXPDO_(1803)
_CREATE_TXPDO_(1804)
_CREATE_TXPDO_(1805)
_CREATE_TXPDO_(1806)
_CREATE_TXPDO_(1807)
_CREATE_TXPDO_(1808)
_CREATE_TXPDO_(1809)
_CREATE_TXPDO_(180a)
_CREATE_TXPDO_(180b)
_CREATE_TXPDO_(180c)
_CREATE_TXPDO_(180d)
_CREATE_TXPDO_(180e)
_CREATE_TXPDO_(180f)
_CREATE_TXPDO_(1810)
_CREATE_TXPDO_(1811)
_CREATE_TXPDO_(1812)
_CREATE_TXPDO_(1813)
_CREATE_TXPDO_(1814)
_CREATE_TXPDO_(1815)
_CREATE_TXPDO_(1816)
_CREATE_TXPDO_(1817)
_CREATE_TXPDO_(1818)
_CREATE_TXPDO_(1819)
_CREATE_TXPDO_(181A)
_CREATE_TXPDO_(181B)
_CREATE_TXPDO_(181C)
_CREATE_TXPDO_(181D)
_CREATE_TXPDO_(181E)
_CREATE_TXPDO_(181F)

/* Create the Transmit PDO Parameter area. */
__far const indextable   transmitPDOParameter[] =
{
	DeclareIndexTableEntry(Index1800),
	DeclareIndexTableEntry(Index1801),
	DeclareIndexTableEntry(Index1802),
	DeclareIndexTableEntry(Index1803),
	DeclareIndexTableEntry(Index1804),
	DeclareIndexTableEntry(Index1805),
	DeclareIndexTableEntry(Index1806),
	DeclareIndexTableEntry(Index1807), 
	DeclareIndexTableEntry(Index1808),
	DeclareIndexTableEntry(Index1809),
	DeclareIndexTableEntry(Index180a),
	DeclareIndexTableEntry(Index180b),
	DeclareIndexTableEntry(Index180c),
	DeclareIndexTableEntry(Index180d),
	DeclareIndexTableEntry(Index180e),
	DeclareIndexTableEntry(Index180f),
	DeclareIndexTableEntry(Index1810),
	DeclareIndexTableEntry(Index1811),
	DeclareIndexTableEntry(Index1812),
	DeclareIndexTableEntry(Index1813),
	DeclareIndexTableEntry(Index1814),
	DeclareIndexTableEntry(Index1815),
	DeclareIndexTableEntry(Index1816),
	DeclareIndexTableEntry(Index1817),
	DeclareIndexTableEntry(Index1818),
	DeclareIndexTableEntry(Index1819),
	DeclareIndexTableEntry(Index181A),
	DeclareIndexTableEntry(Index181B),
	DeclareIndexTableEntry(Index181C),
	DeclareIndexTableEntry(Index181D),
	DeclareIndexTableEntry(Index181E),
	DeclareIndexTableEntry(Index181F)
};

/**************************************************************************/
/* PDO Mapping parameters                                                 */
/**************************************************************************/
/* Computed by strPdoParam */

#  define PDO_MAP(index, sub_index, size_variable_in_bits)\
0x ## index ## sub_index ## size_variable_in_bits

/* Beware : 
index                 *must* be writen 4 numbers in hexa
sub_index             *must* be writen 2 numbers in hexa
size_variable_in_UNS8 *must* be writen 2 numbers in hexa
*/
/* Max number of data which can be put in a PDO
   Example, one PDO contains 2 objects, an other contains 5 objects.
   put 
   MAX_COUNT_OF_PDO_MAPPING 5
*/
#define MAX_COUNT_OF_PDO_MAPPING 8

typedef struct td_s_pdo_mapping_parameter  // Index: 0x21
{
/** count of mapping entries
	*/
	UNS8 count;
	/** mapping entries itself.
	*/
	UNS32 object[MAX_COUNT_OF_PDO_MAPPING];
} s_pdo_mapping_parameter;

/**************************************************************************/
/* The mapping area of the PDO received                                   */
/**************************************************************************/
/* Computed by strPdoReceiveMapTop */
/* Note, The index 160x is used to map the PDO 140x. The relation between the two is automatic */
/* Computed by  strCreateRxMap */
#define _CREATE_RXMAP_(RXMAP) \
static  s_pdo_mapping_parameter RxMap ## RXMAP = \
{ 0,\
{\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00)\
 }\
 };\
 const subindex Index ## RXMAP [] =\
{\
  { RW, uint8, sizeof( UNS8 ), (void*)&RxMap ## RXMAP.count },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[0] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[1] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[2] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[3] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[4] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[5] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[6] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&RxMap ## RXMAP.object[7] }\
};
/* Computed by strPdoReceiveMapBot */
#define INDEX_LAST_PDO_MAPPING_RECEIVE  0x161F
_CREATE_RXMAP_(1600)
_CREATE_RXMAP_(1601)
_CREATE_RXMAP_(1602)
_CREATE_RXMAP_(1603)
_CREATE_RXMAP_(1604)
_CREATE_RXMAP_(1605)
_CREATE_RXMAP_(1606)
_CREATE_RXMAP_(1607)
_CREATE_RXMAP_(1608)
_CREATE_RXMAP_(1609)
_CREATE_RXMAP_(160A)
_CREATE_RXMAP_(160B)
_CREATE_RXMAP_(160C)
_CREATE_RXMAP_(160D)
_CREATE_RXMAP_(160E)
_CREATE_RXMAP_(160F)
_CREATE_RXMAP_(1610)
_CREATE_RXMAP_(1611)
_CREATE_RXMAP_(1612)
_CREATE_RXMAP_(1613)
_CREATE_RXMAP_(1614)
_CREATE_RXMAP_(1615)
_CREATE_RXMAP_(1616)
_CREATE_RXMAP_(1617)
_CREATE_RXMAP_(1618)
_CREATE_RXMAP_(1619)
_CREATE_RXMAP_(161A)
_CREATE_RXMAP_(161B)
_CREATE_RXMAP_(161C)
_CREATE_RXMAP_(161D)
_CREATE_RXMAP_(161E)
_CREATE_RXMAP_(161F)


__far const indextable   RxPDOMappingTable[ ] =
{
	DeclareIndexTableEntry(Index1600),
	DeclareIndexTableEntry(Index1601),
	DeclareIndexTableEntry(Index1602),
	DeclareIndexTableEntry(Index1603),
	DeclareIndexTableEntry(Index1604),
	DeclareIndexTableEntry(Index1605),
	DeclareIndexTableEntry(Index1606),
	DeclareIndexTableEntry(Index1607),
	DeclareIndexTableEntry(Index1608),
	DeclareIndexTableEntry(Index1609),
	DeclareIndexTableEntry(Index160A),
	DeclareIndexTableEntry(Index160B),
	DeclareIndexTableEntry(Index160C),
	DeclareIndexTableEntry(Index160D),
	DeclareIndexTableEntry(Index160E),
	DeclareIndexTableEntry(Index160F),
	DeclareIndexTableEntry(Index1610),
	DeclareIndexTableEntry(Index1611),
	DeclareIndexTableEntry(Index1612),
	DeclareIndexTableEntry(Index1613),
	DeclareIndexTableEntry(Index1614),
	DeclareIndexTableEntry(Index1615),
	DeclareIndexTableEntry(Index1616),
	DeclareIndexTableEntry(Index1617),
	DeclareIndexTableEntry(Index1618),
	DeclareIndexTableEntry(Index1619),
	DeclareIndexTableEntry(Index161A),
	DeclareIndexTableEntry(Index161B),
	DeclareIndexTableEntry(Index161C),
	DeclareIndexTableEntry(Index161D),
	DeclareIndexTableEntry(Index161E),
	DeclareIndexTableEntry(Index161F),
};

/**************************************************************************/
/* The mapping area of the PDO transmited                                   */
/**************************************************************************/
/* Computed by strPdoTransmitMapTop */
/* Note, The index 18xx is used to map the PDO 1Axxx. The relation between the two is automatic */
/* Computed by  strCreateRxMap */
#define _CREATE_TXMAP_(TXMAP) \
static  s_pdo_mapping_parameter TxMap ## TXMAP = \
{ 0,\
{\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00),\
    PDO_MAP(0000, 00, 00)\
 }\
 };\
 const subindex Index ## TXMAP [] =\
{\
  { RW, uint8, sizeof( UNS8 ), (void*)&TxMap ## TXMAP.count },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[0] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[1] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[2] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[3] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[4] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[5] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[6] },\
  { RW, uint32, sizeof( UNS32 ), (void*)&TxMap ## TXMAP.object[7] }\
};
/* Computed by strPdoTransmitMapBot */
#define INDEX_LAST_PDO_MAPPING_TRANSMIT  0x1A1F
_CREATE_TXMAP_(1a00)
_CREATE_TXMAP_(1a01)
_CREATE_TXMAP_(1a02)
_CREATE_TXMAP_(1a03)
_CREATE_TXMAP_(1a04)
_CREATE_TXMAP_(1a05)
_CREATE_TXMAP_(1a06)
_CREATE_TXMAP_(1a07)
_CREATE_TXMAP_(1a08)
_CREATE_TXMAP_(1a09)
_CREATE_TXMAP_(1a0a)
_CREATE_TXMAP_(1a0b)
_CREATE_TXMAP_(1a0c)
_CREATE_TXMAP_(1a0d)
_CREATE_TXMAP_(1a0e)
_CREATE_TXMAP_(1a0f)
_CREATE_TXMAP_(1a10)
_CREATE_TXMAP_(1a11)
_CREATE_TXMAP_(1a12)
_CREATE_TXMAP_(1a13)
_CREATE_TXMAP_(1a14)
_CREATE_TXMAP_(1a15)
_CREATE_TXMAP_(1a16)
_CREATE_TXMAP_(1a17)
_CREATE_TXMAP_(1a18)
_CREATE_TXMAP_(1a19)
_CREATE_TXMAP_(1a1A)
_CREATE_TXMAP_(1a1B)
_CREATE_TXMAP_(1a1C)
_CREATE_TXMAP_(1a1D)
_CREATE_TXMAP_(1a1E)
_CREATE_TXMAP_(1a1F)


__far const indextable   TxPDOMappingTable[ ] =
{
	DeclareIndexTableEntry(Index1a00),
	DeclareIndexTableEntry(Index1a01),
	DeclareIndexTableEntry(Index1a02),
	DeclareIndexTableEntry(Index1a03),
	DeclareIndexTableEntry(Index1a04),
	DeclareIndexTableEntry(Index1a05),
	DeclareIndexTableEntry(Index1a06),
	DeclareIndexTableEntry(Index1a07),
	DeclareIndexTableEntry(Index1a08),
	DeclareIndexTableEntry(Index1a09),
	DeclareIndexTableEntry(Index1a0a),
	DeclareIndexTableEntry(Index1a0b),
	DeclareIndexTableEntry(Index1a0c),
	DeclareIndexTableEntry(Index1a0d),
	DeclareIndexTableEntry(Index1a0e),
	DeclareIndexTableEntry(Index1a0f),
	DeclareIndexTableEntry(Index1a10),
	DeclareIndexTableEntry(Index1a11),
	DeclareIndexTableEntry(Index1a12),
	DeclareIndexTableEntry(Index1a13),
	DeclareIndexTableEntry(Index1a14),
	DeclareIndexTableEntry(Index1a15),
	DeclareIndexTableEntry(Index1a16),
	DeclareIndexTableEntry(Index1a17),
	DeclareIndexTableEntry(Index1a18),
	DeclareIndexTableEntry(Index1a19),
	DeclareIndexTableEntry(Index1a1A),
	DeclareIndexTableEntry(Index1a1B),
	DeclareIndexTableEntry(Index1a1C),
	DeclareIndexTableEntry(Index1a1D),
	DeclareIndexTableEntry(Index1a1E),
	DeclareIndexTableEntry(Index1a1E)
};
/**************************************************************************/
/* The mapped variables at index 0x2000 - 0x5fff                          */
/**************************************************************************/
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//  BLOCK KCU-4
//
//      ###    ###      ########      ###    ###       ####             
//      ###   ###     ####    ###     ###    ###       ####             
//      ###   ###     ####    ####    ###    ###       ####             
//      ###  ###      ####    ####    ###    ###       #### ####        
//      ###  ###      ####            ###    ###       ####  ###        
//      ###  ##       ####            ###    ###       ####  ###        
//      ######        ####            ###    ###       ###   ###        
//      #######       ####            ###    ###     ####    ###        
//      ###  ###      ####            ###    ###     ####   ####        
//      ###  ###      ####    ####    ###    ###     #############      
//      ###   ###     ####    ####    ###    ###            ####        
//      ###   ###     ####    ####    ###    ###            ####        
//      ###    ###      ########       ########             ####        
//
//=================================================================================================
//=================================================================================================
#ifdef PLATA_KCU4
/********* Index 2000 *********/
static const UNS8 highestSubIndex_2000 = 6; // number of subindex - 1
subindex Index2000[] = 
{
	{ RO, uint8, sizeof(UNS8), 		(void*)&highestSubIndex_2000 },
	{ RW, uint16, sizeof (UNS16), 	(void*)&program.ms},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.sec},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.min},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.hour },
	{ RW, uint32, sizeof (UNS32), 	(void*)&program.day },
	{ RW, uint8, sizeof (UNS8), 	(void*)&Kcu.Regim}
};

/********* Index 2001 *********/
static UNS8 highestSubIndex_2001 = 1; // number of subindex - 1
subindex Index2001[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2001 },
	{ RO, uint16,sizeof (UNS16), (void*)&Kcu.Info.word}
};

#define MANUFACTURER_SPECIFIC_LAST_INDEX 0x2001
__far const indextable manufacturerProfileTable[] = 
{
	DeclareIndexTableEntry(Index2000),
	DeclareIndexTableEntry(Index2001)
};

/********* Index 6000 *********/
#define DIGITAL_INPUT_LAST_TABLE_INDEX 0x6000

static UNS8 highestSubIndex_6000 = 9; // number of subindex - 1
subindex Index6000[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&highestSubIndex_6000},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FC[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FC[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FC[2]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FO[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FO[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.FO[2]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.St27V[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.St27V[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Kcu.St27V[2]}
};

__far const indextable digitalInputTable[] = 
{
  DeclareIndexTableEntry(Index6000)
};
#endif
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
// Блок АЦП8
//
//
//      ####       ####   ####    ############    #######       
//     #######     ####   ####    #### ## ####   ###   ####     
//     #######     ####   ####    ###      ###  ####   ####     
//   ####   ####   ####   ####    ####     ###  ####   ####     
//   ####   ####   ####   ####    ####    ####  ####   ####     
//   ####   ####   ####   ####    ####    ####  ###### ####     
//   ####   ####   ####   ####    ####    ####  ###### ####     
//   ####   ####   ####   ####    ####    ####    #######       
//   ###########   ####   ####    ####    ####  #### ######     
//   ###########   ####   ####    ####    ####  #### ######     
//   ####   ####   ####   ####    ####    ####  ####   ####     
//   ####   ####   ####   ####    ####    ####  ####   ####     
//   ####   ####   ####   ####    ####    ####  ####   ####     
//   ####   ####   #############  ####    ####   ##### ####     
//   ####   ####   #############  ####    ####    #######       
//                          ####                                
//                          ####                                
//
//=================================================================================================
//=================================================================================================
#ifdef 	PLATA_ADC8
/********* Index 2000 ***************************************************/
static const UNS8 highestSubIndex_2000 = 5; // number of subindex - 1
subindex Index2000[] = 
{
	{ RO, uint8,	sizeof(UNS8),	(void*)&highestSubIndex_2000 },
	{ RW, uint16, sizeof (UNS16), 	(void*)&program.ms},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.sec},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.min},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.hour },
	{ RW, uint32, sizeof (UNS32), 	(void*)&program.day }
};

/********* Index 2001 ******************************************************/
static UNS8 highestSubIndex_2001 = 6; // number of subindex - 1
subindex Index2001[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2001 },
	{ RO, uint16,sizeof (UNS16), (void*)&Adc8.Info.word},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stFLT1},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stFLT2},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stERR1},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stERR2},
	{ RW, uint8,sizeof (UNS8), (void*)&Adc8.WriteTar}
};

static UNS8 highestSubIndex_2002 = 32; // number of subindex - 1
subindex Index2002[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2002 },

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[0][7]},

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[1][7]},
	
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[2][7]},
	
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.adc_mux_new[3][7]}
};

static UNS8 highestSubIndex_2003 = 13; // number of subindex - 1
subindex Index2003[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2003 },

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][7]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][8]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][9]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][10]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][11]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[0][12]}
};
//	
static UNS8 highestSubIndex_2004 = 13; // number of subindex - 1
subindex Index2004[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2004 },

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][7]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][8]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][9]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][10]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][11]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[1][12]}
};

static UNS8 highestSubIndex_2005 = 13; // number of subindex - 1
subindex Index2005[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2005 },

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][7]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][8]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][9]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][10]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][11]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[2][12]}
};
static UNS8 highestSubIndex_2006 = 13; // number of subindex - 1
subindex Index2006[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2006 },

	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][0]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][1]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][2]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][3]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][4]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][5]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][6]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][7]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][8]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][9]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][10]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][11]},
	{ RW, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc_new[3][12]}
};

//
static UNS8 highestSubIndex_2007 = 16; // number of subindex - 1
subindex Index2007[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2007 },

	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][0]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][1]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][2]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][3]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][4]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][5]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][6]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][7]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][8]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][9]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][10]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][11]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][12]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][13]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][14]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[0][15]}
};

static UNS8 highestSubIndex_2008 = 16; // number of subindex - 1
subindex Index2008[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2008 },

	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][0]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][1]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][2]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][3]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][4]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][5]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][6]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][7]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][8]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][9]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][10]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][11]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][12]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][13]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][14]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[1][15]}
};

static UNS8 highestSubIndex_2009 = 16; // number of subindex - 1
subindex Index2009[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2009 },

	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][0]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][1]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][2]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][3]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][4]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][5]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][6]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][7]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][8]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][9]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][10]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][11]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][12]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][13]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][14]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[2][15]}
};


static UNS8 highestSubIndex_200A = 16; // number of subindex - 1
subindex Index200A[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_200A },

	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][0]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][1]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][2]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][3]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][4]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][5]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][6]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][7]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][8]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][9]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][10]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][11]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][12]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][13]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][14]},
	{ RO, uint8, sizeof(UNS8), (void*)&Adc8.reg_adc[3][15]}
};


#define MANUFACTURER_SPECIFIC_LAST_INDEX 0x200A
__far const indextable manufacturerProfileTable[] = 
{
	DeclareIndexTableEntry(Index2000),
	DeclareIndexTableEntry(Index2001),
	DeclareIndexTableEntry(Index2002),
	DeclareIndexTableEntry(Index2003),
	DeclareIndexTableEntry(Index2004),
	DeclareIndexTableEntry(Index2005),
	DeclareIndexTableEntry(Index2006),
	DeclareIndexTableEntry(Index2007),
	DeclareIndexTableEntry(Index2008),
	DeclareIndexTableEntry(Index2009),
	DeclareIndexTableEntry(Index200A)
};
/********* Index 6403 ******************************************************/
static UNS8 highestSubIndex_6403 = 32; // number of subindex - 1

const subindex Index6403[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6403},

	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[0]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[1]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[2]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[3]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[4]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[5]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[6]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[7]},

	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[8]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[9]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[10]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[11]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[12]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[13]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[14]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[15]},

	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[16]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[17]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[18]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[19]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[20]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[21]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[22]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[23]},

	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[24]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[25]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[26]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[27]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[28]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[29]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[30]},
	{ RO, real32, sizeof (REAL32),(void*)&Adc8.f_adc_data[31]}
};
static UNS8 highestSubIndex_6405 = 32; // number of subindex - 1
const subindex Index6405[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6405},
	//============================================
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[0].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[1].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[2].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[3].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[4].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[5].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[6].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[7].ofs},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[8].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[9].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[10].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[11].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[12].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[13].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[14].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[15].ofs},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[16].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[17].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[18].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[19].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[20].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[21].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[22].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[23].ofs},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[24].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[25].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[26].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[27].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[28].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[29].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[30].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[31].ofs}
};
static UNS8 highestSubIndex_6406 = 32; // number of subindex - 1
const subindex Index6406[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6406},
	
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[0].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[1].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[2].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[3].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[4].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[5].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[6].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[7].k},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[8].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[9].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[10].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[11].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[12].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[13].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[14].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[15].k},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[16].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[17].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[18].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[19].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[20].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[21].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[22].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[23].k},

	{ RW, real32, sizeof (REAL32),(void*)&TarRam[24].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[25].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[26].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[27].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[28].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[29].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[30].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[31].k}
};

#define ANALOG_INPUT_LAST_TABLE_INDEX 0x6406
__far const indextable analogInputTable[] = 
{
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	DeclareIndexTableEntry(Index6403),
	{ NULL, 0 },
	DeclareIndexTableEntry(Index6405),
	DeclareIndexTableEntry(Index6406)
};

#endif
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//=================================================================================================
//                                                                                                                                                      
//         ####            #########      #####    #####       #########         #########                           #########            #########     
//         #####           #########      #####    #####       #########         #########                           ##########           #########     
//    ##############    #####    #####    #####    #####     ####     ####     #####    #####                      #####    #####      #####    #####   
//    ##############    #####    #####    #####    #####     #####    ####     #####    #####                      #####    #####      #####    #####   
//  #####  ####  #####  #####      ###    #####    #####     #####    ####     #####    #####                      #####    #####      #####    #####   
//  #####  ####  #####  #####       ##    #####    #####     ####     ####     #####    #####                      #####    #####      #####    #####   
//  #####  ####  #####  #####             #####    #####              ####     #####    #####                               #####      #####    #####   
//  #####  ####  #####  #####             #####    #####            ####       #####    #####    ##############         #######        #####    #####   
//  #####  ####  #####  #####             #####    #####           #####       #####    #####    ##############        #######         #####    #####   
//  #####  ####  #####  #####              #############         #####           ############                               #####      #####    #####   
//  #####  ####  #####  #####                ###########         #####           ############                               #####      #####    #####   
//    ##############    #####      ###             #####       #####                  ####                         #####    #####      #####    #####   
//    ##############    #####      ###             #####       #####                 #####                         #####    #####      #####    #####   
//         ####         #####    #####    #####    #####     ####                  #####                           #####    #####      #####    #####   
//         #####         ############      #############     #############       # #####                           #####    ####        #############   
//         #####           #########         #########       ##############      #######                             ##########           #########     
//
//=================================================================================================
//=================================================================================================
#ifdef 	PLATA_FSU_29_30
/********* Index 2000 *********/
static const UNS8 highestSubIndex_2000 = 5; // number of subindex - 1
subindex Index2000[] = 
{
	{ RO, uint8, sizeof(UNS8), 		(void*)&highestSubIndex_2000 },
	{ RW, uint16, sizeof (UNS16), 	(void*)&program.ms},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.sec},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.min},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.hour },
	{ RW, uint32, sizeof (UNS32), 	(void*)&program.day }
};

/********* Index 2001 *********/
static UNS8 highestSubIndex_2001 = 10; // number of subindex - 10
subindex Index2001[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2001 },
	{ RO, uint16,sizeof (UNS16), (void*)&Fsu.Info.word},												//	1
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.OL[0]},		// 1 - обрыв в обмотке реле						//	2
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.OL[1]},		// 1 - обрыв в обмотке реле						//	3
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.OL[2]},		// 1 - обрыв в обмотке реле						//	4
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.Diag[0]},	// 1 - прегрузка либо отключение по перегреву	//	5
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.Diag[1]},	// 1 - прегрузка либо отключение поп перегреву	//	6
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.Diag[2]},	// 1 - прегрузка либо отключение поп перегреву	//	7
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.DiagSum[0]},// 1 - есть неисправность обобщённая			//	8
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.DiagSum[1]},// 1 - есть неисправность обобщённая			//	9
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.DiagSum[2]}	// 1 - есть неисправность обобщённая			//	10
};

#define MANUFACTURER_SPECIFIC_LAST_INDEX 0x2001
__far const indextable manufacturerProfileTable[] = 
{
	DeclareIndexTableEntry(Index2000),
	DeclareIndexTableEntry(Index2001)
};

/********* Index 6200 *********/
static UNS8 highestSubIndex_6200 = 3; // number of subindex - 1
subindex Index6200[] = 
{
	{ RW, uint8, sizeof (UNS8), (void*)&highestSubIndex_6200},
	{ RW, uint8, sizeof (UNS8), (void*)&Fsu.NewData[0]},
	{ RW, uint8, sizeof (UNS8), (void*)&Fsu.NewData[1]},
	{ RW, uint8, sizeof (UNS8), (void*)&Fsu.NewData[2]}
};
/*-----------------------------------------------------------*/
static UNS8 highestSubIndex_6201 = 3; // number of subindex - 1
subindex Index6201[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&highestSubIndex_6201},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.SetData[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.SetData[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.SetData[2]}
};
/********* Index 6209 *********/
static UNS8 highestSubIndex_6209 = 3; // number of subindex - 1
subindex Index6209[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&highestSubIndex_6209},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.K[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.K[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Fsu.K[2]}
};
#define DIGITAL_OUTPUT_LAST_TABLE_INDEX 0x6209
__far const indextable digitalOutputTable[] = 
{
  DeclareIndexTableEntry(Index6200),
  DeclareIndexTableEntry(Index6201),
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  { NULL, 0 },
  DeclareIndexTableEntry(Index6209)
};
#endif

//=================================================================================================
//=================================================================================================
//=================================================================================================
//                                                                                                                                                      
//   ######         ######      ##############         ###############                                ############           ############               
//   ######        #######     ###############         ###############                                ############           ############               
//   ######        #######   ######     ######      ######     #######                             ######     #######      ######     ######            
//   ######        #######  #######     ######      ######      ######                             ######      ######     #######     ######            
//   ######        #######   ######     ######      ######     #######                             ######     #######     #######     ######            
//   ######        #######   ######     ######      ######     #######                             ######     #######     #######     ######            
//   ######        #######   ######     ######      ######     #######                                        #######                 ######            
//   ######        #######   ######     ######      ######     #######                                         ######                 ######            
//   #####################   ######     ######      ######     #######      #################               ######                 ######               
//   #####################   ######     ######      ######     #######     ##################               ######                 ######               
//   ######        #######   ######     ######      ######     #######                                   ######                 #######                 
//   ######        #######   ######     ######      ######     #######                                   ######                 ######                  
//   ######        #######   ######     ######      ######     #######                                ######                 #######                    
//   ######        #######   ######     ######      ######     #######                                ######                 #######                    
//   ######        #######   ######     ######      ######      ######                             ######                  ######                       
//   ######        #######   ######     ######      ######      ######                             ######                 #######                       
//   ######        ######################################################                          ##################     ##################            
//   ######        ######################################################                          ##################     ##################            
//                        ######           #############          #######                                                                               
//                        ######           ############           #######                                                                               
//                                                                                                                                                      //                                                                                                                                                      
//=================================================================================================
//=================================================================================================
// 22 и 23
#ifdef 	PLATA_NDD22
/********* Index 2000 *********/
static const UNS8 highestSubIndex_2000 = 5; // number of subindex - 1
subindex Index2000[] = 
{
	{ RO, uint8, sizeof(UNS8), 		(void*)&highestSubIndex_2000 },
	{ RW, uint16, sizeof (UNS16), 	(void*)&program.ms},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.sec},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.min},
	{ RW, uint8, sizeof (UNS8), 	(void*)&program.hour},
	{ RW, uint32, sizeof (UNS32), 	(void*)&program.day}
};

/********* Index 2001 *********/
static UNS8 highestSubIndex_2001 = 5; // number of subindex - 10
subindex Index2001[] = 
{
	{ RO, uint8, sizeof(UNS8), (void*)&highestSubIndex_2001 },
	{ RO, uint16,sizeof (UNS16), (void*)&Ndd.Info.word},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stFLT1},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stFLT2},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stERR1},
	{ RO, uint8,sizeof (UNS8), (void*)&program.stERR2},
};
#define MANUFACTURER_SPECIFIC_LAST_INDEX 0x2001
__far const indextable manufacturerProfileTable[] = 
{
	DeclareIndexTableEntry(Index2000),
	DeclareIndexTableEntry(Index2001)
};


static UNS8 highestSubIndex_6000 = 4; // number of subindex - 1
subindex Index6000[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&highestSubIndex_6000},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.Din[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.Din[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.Din[2]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.Din[3]}
};
/********* Index 6009 *********/
static UNS8 highestSubIndex_6009 = 4; // number of subindex - 1
subindex Index6009[] = 
{
	{ RO, uint8, sizeof (UNS8), (void*)&highestSubIndex_6009},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.valid[0]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.valid[1]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.valid[2]},
	{ RO, uint8, sizeof (UNS8), (void*)&Ndd.valid[3]}
};

#define DIGITAL_INPUT_LAST_TABLE_INDEX 0x6009
__far const indextable digitalInputTable[] = 
{
	DeclareIndexTableEntry(Index6000),
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	DeclareIndexTableEntry(Index6009)
};
#endif
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//                                                                                                                                                      
//                                                                                                                                                      
//  ######   ######        #####        #################                               #####             ######                                        
//  ######   ######      #########      #################                             #######           ########                                        
//  ######   ######     ###########     ######     ######                          ##########        ###########                                        
//  ######   ######   ######   ######   ######     ######                         ###########      #############                                        
//  ######   ######   ######   ######   ######     ######                               #####             ######                                        
//  ######   ######   ######   ######   ######     ######      ###############          #####             ######                                        
//  ######   ######   ######   ######   ######     ######      ###############          #####             ######                                        
//  ######   ######   ###############   ######     ######                               #####             ######                                        
//  ######   ######   ###############   ######     ######                               #####             ######                                        
//  ######   ######   ######   ######   ######     ######                               #####             ######                                        
//  ################# ######   ######   ######     ######                               #####             ######                                        
//  ################# ######   ######   ######     ######                               #####             ######                                        
//             ######                                                                                                                                   
//              ####                                                                                                                                    
//                                                                                                                                                      
//==================================================================================================================================================================================================
#ifdef 	PLATA_DAC11
/********* Index 2000 *********/
static const UNS8 highestSubIndex_2000 = 11; // number of subindex - 1
subindex Index2000[] = 
{
	{ RO, uint8,  sizeof(UNS8), 	(void*)&highestSubIndex_2000 },	//0
	{ RW, uint16, sizeof(UNS16), 	(void*)&program.ms},			//1
	{ RW, uint8,  sizeof(UNS8), 	(void*)&program.sec},			//2
	{ RW, uint8,  sizeof(UNS8), 	(void*)&program.min},			//3
	{ RW, uint8,  sizeof(UNS8), 	(void*)&program.hour},			//4
	{ RW, uint32, sizeof(UNS32), 	(void*)&program.day},			//5
	{ RW, uint8,  sizeof(UNS8), 	(void*)&Dac11.WriteTar},		//6
	{ RO, uint32, sizeof(UNS32), 	(void*)&Dac11.ErrorDAC[0]},		//7
	{ RO, uint16, sizeof(UNS16), 	(void*)&Dac11.Master[0]},		//8
	{ RW, uint16, sizeof (UNS16), 	(void*)&Dac11.NewOutDac},		//9
	{ RO, uint16, sizeof (UNS16), 	(void*)&Dac11.EnOutDac[0]},		//10
	{ RO, uint16, sizeof (UNS16), 	(void*)&Dac11.DiagRele}			//11
};
/********* Index 2001 *********/
static UNS8 highestSubIndex_2001 = 1; // number of subindex - 10
subindex Index2001[] = 
{
	{ RO, uint8,	sizeof(UNS8),	(void*)&highestSubIndex_2001 },
	{ RO, uint16,	sizeof(UNS16),	(void*)&Dac11.Info.word}
};
#define MANUFACTURER_SPECIFIC_LAST_INDEX 0x2001
__far const indextable manufacturerProfileTable[] = 
{
	DeclareIndexTableEntry(Index2000),
	DeclareIndexTableEntry(Index2001)
};

/********* Index 6413 *********/
static UNS8 highestSubIndex_6413 = 12; // number of subindex - 1
subindex Index6413[] = 
{
	{ RO, uint8,  sizeof (UNS8), (void*)&highestSubIndex_6413},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[0]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[1]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[2]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[3]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[4]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[5]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[6]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[7]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[8]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[9]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[10]},
	{ WO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_New[11]}
};
static UNS8 highestSubIndex_6417 = 12; // number of subindex - 1
subindex Index6417[] = 
{
  { RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6417},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[0]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[1]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[2]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[3]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[4]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[5]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[6]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[7]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[8]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[9]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[10]},
  { RO, real32, sizeof (REAL32),(void*)&Dac11.fDAC_Set[11]}
};

static UNS8 highestSubIndex_6418 = 12; // number of subindex - 1
const subindex Index6418[] = 
{
  { RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6418},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[0].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[1].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[2].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[3].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[4].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[5].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[6].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[7].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[8].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[9].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[10].ofs},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[11].ofs}
};
static UNS8 highestSubIndex_6419 = 12; // number of subindex - 1
const subindex Index6419[] = 
{
  { RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6419},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[0].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[1].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[2].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[3].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[4].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[5].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[6].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[7].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[8].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[9].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[10].k},
  { RW, real32, sizeof (REAL32),(void*)&TarRam[11].k}
};

#define ANALOG_OUTPUT_LAST_TABLE_INDEX 0x6419

__far const indextable analogOutputTable[] = 
{
	{ NULL, 0 },//0
	{ NULL, 0 },//1
	{ NULL, 0 },//2
	DeclareIndexTableEntry(Index6413),
	{ NULL, 0 },//4
	{ NULL, 0 },//5
	{ NULL, 0 },//6
	DeclareIndexTableEntry(Index6417),
	DeclareIndexTableEntry(Index6418),
	DeclareIndexTableEntry(Index6419)
};

/********* Index 6403 ******************************************************/
static UNS8 highestSubIndex_6403 = 12; // number of subindex - 1

const subindex Index6403[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6403},

	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[0]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[1]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[2]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[3]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[4]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[5]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[6]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[7]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[8]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[9]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[10]},
	{ RO, real32, sizeof (REAL32),(void*)&Dac11.fADC[11]}
};
static UNS8 highestSubIndex_6405 = 12; // number of subindex - 1
const subindex Index6405[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6405},
	//============================================
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[12].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[13].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[14].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[15].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[16].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[17].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[18].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[19].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[20].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[21].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[22].ofs},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[23].ofs}
};
static UNS8 highestSubIndex_6406 = 12; // number of subindex - 1
const subindex Index6406[] = 
{
	{ RO, uint8, 	sizeof (UNS8), 	(void*)&highestSubIndex_6406},
	
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[12].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[13].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[14].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[15].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[16].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[17].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[18].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[19].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[20].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[21].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[22].k},
	{ RW, real32, sizeof (REAL32),(void*)&TarRam[23].k}
};

#define ANALOG_INPUT_LAST_TABLE_INDEX 0x6406
__far const indextable analogInputTable[] = 
{
	{ NULL, 0 },
	{ NULL, 0 },
	{ NULL, 0 },
	DeclareIndexTableEntry(Index6403),
	{ NULL, 0 },
	DeclareIndexTableEntry(Index6405),
	DeclareIndexTableEntry(Index6406)
};




#endif
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================
//==================================================================================================================================================================================================

#ifndef MANUFACTURER_SPECIFIC_LAST_INDEX
	#define MANUFACTURER_SPECIFIC_LAST_INDEX 0
	__far const indextable manufacturerProfileTable[] = {{ NULL, 0 }};
#endif

#ifndef DIGITAL_INPUT_LAST_TABLE_INDEX
	#define DIGITAL_INPUT_LAST_TABLE_INDEX 0
	__far const indextable digitalInputTable[] = {{ NULL, 0 }};
#endif

#ifndef DIGITAL_OUTPUT_LAST_TABLE_INDEX
	#define DIGITAL_OUTPUT_LAST_TABLE_INDEX 0
	__far const indextable digitalOutputTable[]= {{ NULL, 0 }};
#endif

#ifndef ANALOG_OUTPUT_LAST_TABLE_INDEX
	#define ANALOG_OUTPUT_LAST_TABLE_INDEX 0
	__far const indextable analogOutputTable[] = {{ NULL, 0 }};
#endif

#ifndef	ANALOG_OUTPUT_SET_LAST_TABLE_INDEX
	#define ANALOG_OUTPUT_SET_LAST_TABLE_INDEX 0
	__far const indextable analogOutputSetTable[] = {{ NULL, 0 }};
#endif 

#ifndef ANALOG_INPUT_LAST_TABLE_INDEX
	#define ANALOG_INPUT_LAST_TABLE_INDEX 0
	__far const indextable analogInputTable[]= {{ NULL, 0 }};
#endif

#ifndef	ANALOG_INPUT_SET_LAST_TABLE_INDEX
	#define ANALOG_INPUT_SET_LAST_TABLE_INDEX 0
	__far const indextable analogInputSetTable[] = {{ NULL, 0 }};
#endif 

//=================================================================================================
//=================================================================================================


UNS8 count_sync[NB_LINE_CAN][MAX_COUNT_OF_PDO_TRANSMIT];

#define COMM_PROFILE_LAST 0x1018

/* Should not be modified */
__far const dict_cste dict_cstes = {
COMM_PROFILE_LAST,
INDEX_LAST_SDO_SERVER,            
DEF_MAX_COUNT_OF_SDO_SERVER,	    
INDEX_LAST_SDO_CLIENT,	          
DEF_MAX_COUNT_OF_SDO_CLIENT,	    
INDEX_LAST_PDO_RECEIVE,	          
MAX_COUNT_OF_PDO_RECEIVE,	        
INDEX_LAST_PDO_TRANSMIT,	        
MAX_COUNT_OF_PDO_TRANSMIT,	      
INDEX_LAST_PDO_MAPPING_RECEIVE,	  
INDEX_LAST_PDO_MAPPING_TRANSMIT,

MANUFACTURER_SPECIFIC_LAST_INDEX,

DIGITAL_INPUT_LAST_TABLE_INDEX,
DIGITAL_OUTPUT_LAST_TABLE_INDEX,

ANALOG_OUTPUT_LAST_TABLE_INDEX,
ANALOG_OUTPUT_SET_LAST_TABLE_INDEX,

ANALOG_INPUT_LAST_TABLE_INDEX,
ANALOG_INPUT_SET_LAST_TABLE_INDEX,

NB_OF_HEARTBEAT_PRODUCERS	  
};  
