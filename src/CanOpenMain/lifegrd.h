/*********************************************************
 *          File : lifegrd.h
 *                                                       *      
 *                                                       *
 *********************************************************/

#ifndef __lifegrd_h__
#define __lifegrd_h__

#include "applicfg.h"
#include "can.h"
#include "def.h"


/** Struct for creating a table where all cob_ids of the consumers are stored.
 *  inclusive the time since the last heartbeat... */
 
typedef struct td_s_heartbeat_entry
{
  /** ID of the node which we have to guard */

	UNS8 nodeId;

  /** this is the maximum time between two heartbeat messages... if this
   *  value is set
   *  to 0, then this heartbeat is disabled.  */
 
	UNS16 should_time;

  /** time since the last heartbeat-message. scale is not yet clear.
   *  maybe 1 = 1ms...
   *  everytime a new heartbeat-message has been received, this var must
   *  be set to 0  */
  
	UNS16 time;
} s_heartbeat_entry;

/** This variable holds all heartbeat related values for our hearbeat consumers */

//extern s_heartbeat_entry heartBeatTable[];


/** Struct for creating a table where the needed heartbeat values for this
 *  CANopen node
 *  are stored. Thanks to this table, the heartbeat related functions don't
 *  have to
 *  access the object dictionary every time. */
 
typedef struct td_s_ourHeartBeatValues
{
  /** the configured time within we have to send our heartbeat message  */
  
  UNS16 ourShouldTime;
  
  /** the time which has passed since the last sending of a heartbeat
   *   message   */
  
  UNS16 ourTime;
  
  /** the time when the heartbeat-function (Process_LifeGuard) has been called.
   *  This is neccessary to know, so we can decide when to send our heartbeat.  */
  
  UNS16 ourLastTime;
} s_ourHeartBeatValues;

/** This variable holds the values about the heartbeat settings of this node */

//extern s_ourHeartBeatValues ourHeartBeatValues;

/* The state of each node */

//extern e_nodeState NMTable[MAX_CAN_BUS_ID][NMT_MAX_NODE_ID];

//*************************************************************************/
//Functions
//*************************************************************************/

/**Today, this function is empty*/

void heartbeatInit (void);

/** To read the state of a node
 *  This can be used by the master after having sent a life guard request,
 *  of by any node if it is waiting for heartbeat. */

//e_nodeState getNodeState (UNS8 bus_id, UNS8 nodeId);


/** Checks whether all heartbeat messages (where this node is a heartbeat
 *  consumer) have arrived within the configured time (configuration in the 
 *  object dictionary).
 *  if not all messages have arrived, lifeguardCallback is called, so the user
 *  can perform any error-handling.
 * This function also sends the heartbeat messages of this node, if neccessary. */

//void heartbeatMGR (void);

/** This function is called by heartbeatMGR( ) if one or more heartbeat
 *  message haven't arrived within the configured time. The argument is one of
 *  the following: CONNECTION_OK, CONNECTION_FAILED, GUARDING_MESSAGE_LOST,
 *  HEARTBEAT_MESSAGE_LOST,
 *  HEARTBEAT_CONSUMER_LOST defined in file def.h
 *  \param bReason can be one of the Error-states defined in def.h, such as
 *          - CONNECTION_OK
 *          - CONNECTION_FAILED
 *          - GUARDING_MESSAGE_LOST
 *          - HEARTBEAT_MESSAGE_LOST
 *          - HEARTBEAT_CONSUMER_LOST */

void lifeguardCallback (UNS8 bReason);

/** This function is called by proccessRxCanMessages( ). it is responsible to
 *  process a canopen-message which seams to be an NMT Error Control
 *  Messages. At them moment we assume that every NMT error control message
 *  is a heartbeat message.
 *  \param Message The CAN-message which has to be analysed.
 *  If a BootUp message is detected, it will return the nodeId of the Slave who booted up*/

UNS8 proceedNMTerror (UNS8 bus_id, Message* m);


/** This function is called by proccessHeartBeat( ).
 *  When a heartbeat is not received during the waited time,
 *  this function is called.
 *  \param node id of the  node which has not sent its heartbeat
 *  This function is defined in the application files.  */

void heartbeatError (UNS8 nodeId);


#endif //__lifegrd_h__
