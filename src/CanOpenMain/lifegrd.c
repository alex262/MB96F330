/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : lifegrd.c
 *                                                       *
 *********************************************************/

/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */

#include <stdio.h>
#include "applicfg.h"
#include "lifegrd.h"
#include "canOpenDriver.h"
#include "objacces.h"
#include "objdictdef.h"
#include "timer.h"

extern UNS8 bDeviceNodeId;
extern e_nodeState nodeState;
//===========================================================
/*
s_heartbeat_entry    	heartBeatTable[NB_OF_HEARTBEAT_PRODUCERS];
s_ourHeartBeatValues 	ourHeartBeatValues;
e_nodeState 			NMTable[MAX_CAN_BUS_ID][NMT_MAX_NODE_ID]; */
//===========================================================
/* Used only for the heartbeat to send.
* Values are 0 or 1
*/
UNS8 toggle = 0;
//===========================================================
/** This variable indicates wheter the device is in the Notfall-state (this
*  means that one of its heartbeatproducers hasn't send a heartbeat message. 
*/
UNS8 bErrorOccured;
//===========================================================
/*e_nodeState getNodeState (UNS8 bus_id, UNS8 nodeId)
{
	e_nodeState networkNodeState = NMTable[bus_id][nodeId]; // Do not read the toggle bit of the node Guarding
	return networkNodeState;
}*/
//===========================================================
/* Retourne le node-id */
UNS8 proceedNMTerror(UNS8 bus_id, Message* m )
{
//	UNS16 time, should_time;
//	UNS8 *   pbConsumerHeartbeatCount;
	// Pointer to HBConsumerTimeArray (Array of expected heartbeat cycle time for each node.
	// HBConsumerTimeArray is defined in objdict.c *  \param CheckAccess if other than 0, do not read if the data is Write Only
//	UNS32 *  pbConsumerHeartbeatEntry;// Index 1016 on 32 bits
	UNS8 *   pdwSize;
	UNS8     dwSize;//, count, ConsummerHeartBeat_nodeId ;
//	UNS8 index; // Index to scan the table of heartbeat consumers
	UNS8 nodeId = (UNS8) GET_NODE_ID((*m));

	pdwSize = &dwSize;
	if((m->rtr == 1) ) /* Notice that only the master can have sent this node guarding request */
	{ // Receiving a NMT NodeGuarding (request of the state by the master)
		//  only answer to the NMT NodeGuarding request, the master is not checked (not implemented)
		if (nodeId == bDeviceNodeId )
		{
			Message msg;
			msg.cob_id = bDeviceNodeId + 0x700;
			msg.len = (UNS8)0x01;
			msg.rtr = 0;
			msg.data[0] = getState(); 
			if (toggle)
			{
				msg.data[0] |= 0x80 ;
				toggle = 0 ;
			}
			else
				toggle = 1 ; 
				// send the nodeguard response.
			MSG_WAR(0x3130, "Sending NMT Nodeguard to master, state: ", nodeState);
			f_can_send( bus_id, &msg );
		}  
		return 1 ;
	}
	if (m->rtr == 0) // Not a request CAN
	{
		MSG_WAR(0x3110, "Received NMT nodeId : ", nodeId);
		/* the slave's state receievd is stored in the NMTable */
		// The state is stored on 7 bit
	//	NMTable[bus_id][nodeId] = (e_nodeState) ((*m).data[0] & 0x7F) ;
    
		/* Boot-Up frame reception */
//		if ( NMTable[bus_id][nodeId] == Initialisation)
//		{
			// The device send the boot-up message (Initialisation)
			// to indicate the master that it is entered in pre_operational mode
			// Because the  device enter automaticaly in pre_operational mode,
			// the pre_operational mode is stored 
			//  NMTable[bus_id][nodeId] = Pre_operational;
	//		MSG_WAR(0x3100, "The NMT is a bootup from node : ", nodeId);
	//		return 1;
	//	}
      
/*		if( NMTable[bus_id][nodeId] != Unknown_state ) 
		{
			if( getODentry( (UNS16)0x1016, (UNS8)0x0, 
							(void * *) &pbConsumerHeartbeatCount, 
							pdwSize, 0 ) == OD_SUCCESSFUL )
			{
				if (*pbConsumerHeartbeatCount > (UNS8)NB_OF_HEARTBEAT_PRODUCERS)
					count = (UNS8)NB_OF_HEARTBEAT_PRODUCERS ;
				else
					count = *pbConsumerHeartbeatCount ;

				for( index = (UNS8)0x00; index < count; index++ )
				{
					if( getODentry( (UNS16)0x1016, (UNS8)index + 1,
									(void * *)&pbConsumerHeartbeatEntry, 
									pdwSize, 0 ) == OD_SUCCESSFUL )
					{
						should_time = (UNS16) ( (*pbConsumerHeartbeatEntry) & (UNS32)0x0000FFFF ) ;
						ConsummerHeartBeat_nodeId = (UNS8)( ((*pbConsumerHeartbeatEntry) & (UNS32)0x00FF0000) >> (UNS8)16 );
						if (( should_time )&&( nodeId == ConsummerHeartBeat_nodeId ))
						{
							time = getTime16( &(heartBeatTable[index].time) );
							MSG_WAR(0x3105, "HeartBeat received from node : ", nodeId );
							MSG_WAR(0x3106, "                     at time : ", time);
							setTime16( &heartBeatTable[index].time, (UNS8)0x0000 );
							if( bErrorOccured == TRUE )
							{
								lifeguardCallback( (UNS8)CONNECTION_OK ); // a little strange
								return 0 ;
							}
						}
					}
				} // end for
			}  
		} //End if( NMTable[bus_id][nodeId] != Unknown_state) 
	*/
	} // End if (m->rtr == 0)
	
	return 1;
}
//===========================================================
/*
void heartbeatMGR(void)
{
	UNS8 index; // Index to scan the table of heartbeat consumers
	Message msg;
	UNS16 time, should_time;
	UNS8 *   pbConsumerHeartbeatCount;
	// Pointer to HBConsumerTimeArray (Array of expected heartbeat cycle time for each node.
	// HBConsumerTimeArray is defined in objdict.c
	UNS32 *  pbConsumerHeartbeatEntry;// Index 1016 on 32 bits
	// Pointer to HBProducerTime (cycle of the heartbeat time producer, defined in objdict.c
	UNS16 *  pbProducerHeartbeatEntry; // index 1017 on 16 bits
	UNS8 *   pdwSize;
	UNS8     dwSize, nodeId, count ;
	pdwSize = &dwSize;
	// Concern heartbeats to receive
	// *****************************
	// now we have to check if one or more heartbeat messages haven't been
	// received within the configured time. if this is the case, we have to call 
	// lifeguardCallback( ) with the argument LOST_HEARTBEAT_MESSAGE
	if( getODentry( (UNS16)0x1016, (UNS8)0x0, (void * *)
		&pbConsumerHeartbeatCount, pdwSize, 0 ) == OD_SUCCESSFUL )
	{
		if (*pbConsumerHeartbeatCount > (UNS8)NB_OF_HEARTBEAT_PRODUCERS)
			count = (UNS8)NB_OF_HEARTBEAT_PRODUCERS ;
		else
			count = *pbConsumerHeartbeatCount ;
       	for( index = (UNS8)0x00; index < count; index++ )
		{
			MSG_WAR(0x3120, "index : ", index);
			if( getODentry( (UNS16)0x1016, (UNS8)index + 1,
							(void * *)&pbConsumerHeartbeatEntry, 
							pdwSize, 0 ) == OD_SUCCESSFUL )
			{
				should_time = (UNS16) ( (*pbConsumerHeartbeatEntry) & (UNS32)0x0000FFFF ) ;
				MSG_WAR(0x3121, "should_time : ", should_time ) ;
				if ( should_time )
				{
					time = getTime16( &(heartBeatTable[index].time) );
					if ( should_time < time )
					{
						nodeId = (UNS8)(((*pbConsumerHeartbeatEntry)&(UNS32)0x00FF0000)>>(UNS8)16);
						MSG_WAR(0x2101, "HeartBeat not received. Node : ",nodeId);
						MSG_WAR(0x2102, "                 actual time : ",time);
						MSG_WAR(0x2103, "                    max time : ",should_time);
						setTime16( &heartBeatTable[index].time, (UNS16)0x0000 );
						lifeguardCallback( (UNS8)HEARTBEAT_CONSUMER_LOST );
						heartbeatError( nodeId );
					}
				}
			}
		}       
	}
	if( getODentry( (UNS16)0x1017, (UNS8)0,
                  (void * *)&pbProducerHeartbeatEntry, pdwSize, 0 ) == OD_SUCCESSFUL )
	{
		should_time = *pbProducerHeartbeatEntry ;
		if ( should_time )
		{
			time = getTime16( &ourHeartBeatValues.ourTime ); // actual time
			if( ( time >= should_time ) )
			{
				// Time expired, the heartbeat must be sent immediately
				// generate the correct node-id: this is done by the offset 1792
				// (decimal) and additionaly
				// the node-id of this device.
				msg.cob_id = bDeviceNodeId + 0x700;
				msg.len = (UNS8)0x01;
				msg.rtr = 0;
				msg.data[0] = getState(); // No toggle for heartbeat !
				// send the heartbeat
				f_can_send(0, &msg );//?????????????????????????
				f_can_send(1, &msg );//?????????????
				// reset the timers...
				setTime16( &(ourHeartBeatValues.ourTime), (UNS16)0x0000 );
				ourHeartBeatValues.ourLastTime = (UNS16)0x0000;
				MSG_WAR(0x3104, "HeartBeat sent at Time : ", time);
			}
			else
			{
				ourHeartBeatValues.ourLastTime = getTime16( &(ourHeartBeatValues.ourTime) );
			}
		} // end if( ourHeartBeatValues.ourShouldTime != (UNS16)0x0000 )
	} 
}*/
//===========================================================
void heartbeatInit(void)
{
	bErrorOccured = FALSE;
}
//===========================================================
void lifeguardCallback( UNS8 bReason )
{ 	
	if( bReason == HEARTBEAT_CONSUMER_LOST )
	{
		bErrorOccured = TRUE;
	}
	else if( bReason == CONNECTION_OK )
	{
		bErrorOccured = FALSE;
	}
}