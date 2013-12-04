/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *

 *********************************************************
           File : nmtSlave.c

 * Functions for NMT used only by slaves nodes           *      
 *                                                       *
 *********************************************************/

/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */
#include <string.h>
#include <stdio.h>
#include "def.h"
#include "canOpenDriver.h"
#include "objdictdef.h"
#include "objacces.h"
#include "lifegrd.h"
#include "nmtSlave.h"

/**********************extern variables declaration***************************/
extern UNS8 bDeviceNodeId;	      // module node_id 		
extern e_nodeState nodeState;	      // slave's state in the state machine 
extern __far const dict_cste dict_cstes;    // contains some communication parameters
//extern UNS8 count_sync[];   	      // count the number of SYNC messages
                                      // necessary because a message may not be sent at every SYNC, but
                                      // every "n" SYNC. n is stored in this table.
extern proceed_info proceed_infos[];  // Array of messages processing information
// buffer used when PDO reception or transmission
extern s_process_var process_var;
extern s_ourHeartBeatValues ourHeartBeatValues;

/*****************************variables declaration***************************/

UNS8 togglebit = 0;

/*******************************************************************)*********/
/* put the slave in the state wanted by the master */	
UNS8 proceedNMTstateChange(UNS8 bus_id, Message *m)
{
	if((nodeState == Pre_operational) || (nodeState == Operational) || (nodeState == Stopped)) 
	{
    
		MSG_WAR(0x3400, "NMT received. for node :  ", (*m).data[1]);
    
		// Check if this NMT-message is for this node
		// byte 1 = 0 : all the nodes are concerned (broadcast)
    	
		if( ( (*m).data[1] == 0 ) || ( (*m).data[1] == bDeviceNodeId ) )
		{
    		switch( (*m).data[0])
			{ // command specifier (cs)			
			case NMT_Start_Node:
				if ( (nodeState == Pre_operational) || (nodeState == Stopped) )
					nodeState = Operational;
				break; 
			case NMT_Stop_Node:
				if ( nodeState == Pre_operational || nodeState == Operational )
					nodeState = Stopped;
				break;
			case NMT_Enter_PreOperational:
				if ( nodeState == Operational || nodeState == Stopped )
					nodeState= Pre_operational;
				break;
			case NMT_Reset_Node:
				nodeState = Initialisation;
				break;
			case NMT_Reset_Comunication:
				nodeState = Initialisation;
				break;
			}// end switch
		}// end if( ( (*m).data[1] == 0 ) || ( (*m).data[1] == bDeviceNodeId ) )
		return 0;  
	}
	return 0xFF;
}
/*****************************************************************************/
UNS8 slaveSendBootUp(UNS8 bus_id)
{
	Message m;
	
	MSG_WAR(0x3407, "Send a Boot-Up msg ", 0);
	
	/* message configuration */
	m.cob_id = NODE_GUARD << 7 | bDeviceNodeId;
	m.rtr = DONNEES;
	m.len = 1;
	m.data[0] = 0x00;
    
	return f_can_send(bus_id,&m);
}