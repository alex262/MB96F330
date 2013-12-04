/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : sync.h
 *                                                       *
 *********************************************************/

#ifndef __SYNC_h__
#define __SYNC_h__


/** Time management for sending the SYNC */

typedef struct
{
	UNS32 ourShouldTime;
	
	/** the time which has passed since the last sending of a sync
   *   message */
   
	UNS32 ourTime;
	
  /** the time when the computesync-function (computeSYNC) has been called.
   *  This is neccessary to know, so we can decide when to send our sync. */
  /** The current time when the time test function has been called.   */
    
	UNS32 ourLastTime;
} s_sync_values ;

extern s_sync_values SyncValues ;

/** transmit a SYNC message on the bus number bus_id
 * bus_id is hardware dependant
 * return f_can_send(bus_id,&m) */
 
UNS8 sendSYNC (UNS8 bus_id, UNS32 cob_id);

/** Transmit a SYNC message after time * periodeSync, trigered by the timer.
 * periodeSync is defined in the dictionary, index 0x1006.
 * return 0 or 1. The meaning of the return value must be confirmed.
 * This function test if the timer have reached the time. If yes, send a SYNC
 * If no, returns without sending a SYNC. So, this function must be used in a loop.
 * For a better accuracy, put this function in void timerInterrupt( int iValue )
 * Returns 
 * 0 : Time expired -> SYNC is sent
 * 1 : Time non expired or not authorized to send the SYNC (value = 0) in objdic at index 1006)
 * 0xFF : error. */
 
UNS8 computeSYNC (void);

/** This function is called when the node is receiving a SYNC 
 * message (cob-id = 0x80).
 * What does the function :
 * check if the node is in OERATIONAL mode. (other mode : return 0 but does nothing).
 * Get the SYNC cobId by reading the dictionary index 1005. (Return -1 if it does not correspond 
 * to the cobId received).
 * Scan the dictionary from index 0x1800 to the last PDO defined (dict_cstes.max_count_of_PDO_transmit)
 *   for each PDO whose transmission type is on synchro (transmission type < 241) and if the msg must
 *   be send at this SYNC. read the COBID. Verify that the nodeId inside the 
 *   nodeId correspond to bDeviceNodeId. (Assume that the cobId of a PDO Transmit is made 
 *   with the node id of the node who transmit), get the mapping, launch PDOmGR to send the PDO
 * *m is a pointer to the message received
 * bus_id is hardware dependant
 * return 0 if OK, 0xFF if error */
 
UNS8 proceedSYNC (UNS8 bus_id, Message * m);

#endif