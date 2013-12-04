/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : objdictdef.h
 *                                                       *      
 *                                                       *
 *********************************************************/

#ifndef __objdictdef_h__
#define __objdictdef_h__

#include "applicfg.h"
#include "def.h"
#include "can.h"

/************************* CONSTANTES **********************************/
/** this are static defined datatypes taken fCODE the canopen standard. They
 *  are located at index 0x0001 to 0x001B. As described in the standard, they
 *  are in the object dictionary for definition purpose only. a device does not
 *  to support all of this datatypes. */
 
#define boolean         0x01
#define int8            0x02
#define int16           0x03
#define int32           0x04
#define uint8           0x05
#define uint16          0x06
#define uint32          0x07
#define real32          0x08
#define visible_string  0x09
#define octet_string    0x0A
#define unicode_string  0x0B
#define time_of_day     0x0C
#define time_difference 0x0D

#define domain          0x0F
#define int24           0x10
#define real64          0x11
#define int40           0x12
#define int48           0x13
#define int56           0x14
#define int64           0x15
#define uint24          0x16

#define uint40          0x18
#define uint48          0x19
#define uint56          0x1A
#define uint64          0x1B

#define pdo_communication_parameter 0x20
#define pdo_mapping                 0x21
#define sdo_parameter               0x22
#define identity                    0x23



/** definitions of the different types of PDOs' transmission
 * 
 * SYNCHRO(n) means that the PDO will be transmited every n SYNC signal. */
 
#define TRANS_EVERY_N_SYNC(n) 	(n) /*n = 1 to 240 */
#define TRANS_SYNC_MIN    		1    /* Trans after reception of n SYNC. n = 1 to 240 */
#define TRANS_SYNC_MAX    		240  /* Trans after reception of n SYNC. n = 1 to 240 */
#define TRANS_RTR_SYNC        	252  /* Transmission on request */
#define TRANS_RTR             	253  /* Transmission on request */
#define TRANS_EVENT           	255  /* Transmission on event */

/** Each entry of the object dictionary can be READONLY (RO), READ/WRITE (RW),
 *  WRITE-ONLY (WO), OR CONSTANT (CONST)
 *  At the moment SDO-Transfer is not implemented, and therefore the functions
 *  which accesses the object dictionary do not care about this acces-types.
 *  Of course this is topic to change! */
 
#ifndef CONST
enum e_accessAttribute { RW, WO, RO, CONST=2 };
#else
enum e_accessAttribute { RW, WO, RO};
#endif

/** Each entry of the object dictionary can be a different object. this objects
 *  are defined by the CANopen standard. see standard for more information.
 *  At the moment the object dictionary does not really care about this
 *  different types. this is topic to change! */

/** count the number of SYNC messages */

extern UNS8 count_sync[2][31];

/************************ STRUCTURES ****************************/

/**Constant dictionary structure */

struct s_dict_cste
{

/** defines the last available entry of the communication profile
 *  area. Adjust this var, if you have additional enties. */

UNS16 comm_profile_last;

/** This is the last defined entry for sdo-paramters.
 *  This means: this is the last entry for which memory
 *  has been allocated. Adjust this value if you need more
 *  or less entries. */

UNS16 server_SDO_last;

/** indicates the count of defined PDOs */

UNS16 max_count_of_server_SDO;

UNS16 client_SDO_last;

UNS16 max_count_of_client_SDO;

/** This is the last defined entry for pdo-paramters (rx).
 *  This means: this is the last entry for which memory
 *  has been allocated. Adjust this value if you need more
 *  or less entries. */

UNS16 receive_PDO_last;

/** Autogenerated value: indicates the count of defined PDOs
 *  for reception */

UNS16 max_count_of_PDO_receive;

/** This is the last defined entry for pdo-paramters (tx).
 *  This means: this is the last entry for which memory
 *  has been allocated. Adjust this value if you need more
 *  or less entries. */

UNS16 transmit_PDO_last;

/** Indicates the count of defined PDOs
 *  for tramsmission */
 
UNS16 max_count_of_PDO_transmit;

/** This is the last PDO-Mapping parameter (rx) for which memory
 *  has been allocated */
 
UNS16 receive_PDO_mapping_last;

/** This is the last PDO-Mapping parameter (tx) for which memory
 *  has been allocated */
 
UNS16 transmit_PDO_mapping_last;

/** This is the last manufacturer specific data table for which memory
 *  has been allocated */
 
UNS16 manufacturerSpecificLastIndex ;

/** This is the last digital Input Table Index for which memory
 *  has been allocated */
 
UNS16 digitalInputTableLastIndex;

/** This is the last digital Output Table Index for which memory
 *  has been allocated */
 
UNS16 digitalOutputTableLastIndex;
/**/
UNS16 analogOutputTableLastIndex;
UNS16 analogOutputSetTableLastIndex;

UNS16 analogInputTableLastIndex;
UNS16 analogInputSetTableLastIndex;

/** Number of nodes from which can be received a heartbeat */

UNS8 nbOfHeartbeatProducers;

};

typedef struct s_dict_cste dict_cste;

/** Below follows some more datatypes (complex ones) defined by the CANopen
 *  standard. */

/** This is the datatype for the communication parameter of PDOs defined
 *  by the CANopen standard */

typedef struct td_s_pdo_communication_parameter  // Index: 0x20
{
    /** number of supported entries = nb subindex - 1*/
    
	UNS8   count;
    
	/** COB-ID */
    
	UNS32  cob_id;
    
	/** transmission type: 
	255 (0XFF) : Transmission on event
	253 (0XFD) : Transmission on request. Use instead the #define RTR 253
        1 - 240    : Transmission on synchro. Use instead the macro SYNCHRO(n)
                     n : to transmit every n SYNC message.  */
    UNS8   type;
    
	/** inhibit time: not supported yet.*/
	
    UNS16  inhibit_time;
    
	/** reserved (by CANopen standard) */
	
    UNS8   reserved;
    
	/** event timer: not supported yet. */
	
    UNS16  event_timer;
} s_pdo_communication_parameter;


// s_pdo_mapping_parameter // Index: 0x21 is defined in objdict.c
// ---------------------------------------------------------------

/** Struct needed for SDO parameter. Defined by the CANopen standard*/

typedef struct td_s_sdo_parameter // Index: 0x22
{
    /** number of supported entries  */
	
    UNS8   count;
    
	/** COB-ID client->server */
    
	UNS32  cob_id_client;
	
    /** COB-ID server->client */
    
	UNS32  cob_id_server;
    /** node ID of SDO's client resp. server  */
    
	UNS8   node_id;
} s_sdo_parameter;


/** Struct needed for the idendity of a CANopen device. Defined
 *  by the CANopen standard */
 
typedef struct td_s_identity  // Index: 0x23
{
    /** number of supported entries  */
    
	UNS8   count;
    
	/** Vendor-ID (given by the CAN-CIA) */
	
    UNS32  vendor_id;
    
	/** Product code     */
	
    UNS32  product_code;

    /** Revision number  */
	
    UNS32  revision_number;
    
	/** Serial number of this device */
   
 	UNS32  serial_number;
} s_identity;


/** This are some structs which are neccessary for creating the entries
*  of the object dictionary. */

typedef struct td_subindex
{
    enum e_accessAttribute  bAccessType;	// Access type (RO, RW, ...)
    UNS8                    bDataType; 		// Defines of what datatype the entry is
    UNS8                    size;      		// The size (in Byte) of the variable
    void*                   pObject;   	// This is the pointer of the Variable
} subindex;



/** Struct for creating entries in the communictaion profile */

typedef struct td_indextable
{
    subindex*   pSubindex;   // Pointer to the subindex
    UNS8   		bSubCount;   // the count of valid entries in for this subindex
							// note: this count can differ fCODE the count defined
							// in some object dictionary entries (in subindex 0).
							// this count here defines how many memory has been
							// allocated. this memory does not have to be used.
} indextable;

typedef struct s_proc_info
{
	UNS8 function_code;
	//UNS8 name[10];
	UNS8 (*process_function)(UNS8, Message *);
} proceed_info;

/********************* EXTERN DECLARATION *******************************/

/*************************************************************************
 ** This is the part of the object directory which contains the
* communication profile...
*************************************************************************/

/** This var holds the pointers to the entries of the communication
*  profile area. */

extern __far const indextable CommunicationProfileArea[];

/*************************************************************************
* This is the part of the object directory which contains the
* SDO Parameters
*************************************************************************/

extern __far const indextable serverSDOParameter[];

extern __far const indextable clientSDOParameter[];

/*************************************************************************
* This is the part of the object directory which contains the
* PDO Parameters
*************************************************************************/

/** Entries for the PDO Parameters (such as transmission type,
 *  cob-id, inhibit time, ... */
 
extern __far const indextable receivePDOParameter[];

/** The transmitPDOParameter array. As the name indicates: this array
 *  is needed to store PDO(tx) PDO parameters */

extern __far const indextable transmitPDOParameter[];

/*************************************************************************
* This is the part of the object directory which contains the
* PDO Mapping parameters
*************************************************************************/

/** This variable indexes all entries neccessary for pdo-mapping for
 *  receiving PDOs */
 
extern __far const indextable RxPDOMappingTable[ ];

/** This variable indexes all entries neccessary for pdo-mapping for
 *  transmitting PDOs */
 
extern __far const indextable TxPDOMappingTable[ ];

/*************************************************************************
*  Manufacturer Specific Profile
*************************************************************************/

extern __far const indextable manufacturerProfileTable[];

/*************************************************************************
*  DS-401: digital&analog input/output device
*************************************************************************/

/** This variable indexes all entries which are responsible for digital
 *  inputs (input means: a value which is captured by this node and sent to
 *  another node
 *  e.g. the state of a port is sent to the server ) */
 
extern __far const indextable digitalInputTable[];

/** This variable indexes all entries which are responsible for digital
 *  outputs. */
 
extern __far const indextable digitalOutputTable[];
/*******************************************************************/
extern __far const indextable analogInputTable[];
extern __far const indextable analogOutputTable[];
extern __far const indextable analogInputSetTable[];
extern __far const indextable analogOutputSetTable[];

extern __far const dict_cste dict_cstes;

/************************** MACROS *********************************/
/** This macro helps creating the object dictionary entries.
 *  by calling this macro
 *  it creates an entry in form of: 7 of entries, pointer to the entry. This
 *  macro was sponsered by Edouard Tisserant: thanks!
 */

#define DeclareIndexTableEntry(entryname)    \
{ (subindex*)entryname,sizeof(entryname)/sizeof(entryname[0])}

///CANopen usefull helpers

#define GET_NODE_ID(m)         		(m.cob_id & 0x1f)
#define GET_FUNCTION_CODE(m)    	(m.cob_id >> 7)|((m.cob_id >> 1)&0x30)	//(m.cob_id >> 7)
#define GET_FUNCTION_CODE_ptr(m)    (m->cob_id >> 7)|((m->cob_id >> 1)&0x30)//(m.cob_id >> 7)


#endif // __objdictdef_h__
