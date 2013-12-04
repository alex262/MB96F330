/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *********************************************************
           File : canOpenMaster.c

 * Functions used by a master node only                  *      
 *                                                       *
 *********************************************************/


/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */

#include <stddef.h> /* for NULL */
#include <string.h>
#include <stdio.h>
#include "can.h"

#include "applicfg.h"
#include "can.h"
#include "def.h"
#include "objdictdef.h"
#include "canOpenDriver.h"
#include "nmtMaster.h"

/********************extern variables declaration****************************/
extern UNS8 bDeviceNodeId;
extern proceed_info proceed_infos[];  // Array of message processing information

/************************variables declaration********************************/
// Network Management Table, stores slaves'state

extern e_nodeState NMTable[MAX_CAN_BUS_ID][NMT_MAX_NODE_ID]; 

/* creation of an array which stores the node_id of the nodes
from which the master is waiting for a response.
Takes the values TRUE or FALSE*/ 

UNS8 NodeStateWaited[MAX_CAN_BUS_ID][NMT_MAX_NODE_ID];

/****************************************************************************/
UNS8 initCANopenMaster(void)
{
	UNS8 i, j;
	
	/* NodeStateWaited array initialization */
	for (i = 0 ; i < MAX_CAN_BUS_ID ; i++) 
	{
		for (j = 0 ; j < NMT_MAX_NODE_ID ; j++)
			NodeStateWaited[i][j] = FALSE;
	} // end (i = 0 ; i < MAX_CAN_BUS_ID ; i++)
  return 0;
}
/******************************************************************************/
UNS8 masterSendNMTstateChange(UNS8 bus_id, UNS8 Node_ID, UNS8 cs)
{
	Message m;

	MSG_WAR(0x3501, "Send_NMT cs : ", cs);
	MSG_WAR(0x3502, "    to node : ", Node_ID);
	/* message configuration */
	m.cob_id = 0x0000; /*(NMT) << 7*/
	m.rtr = DONNEES;
	m.len = 2;
	m.data[0] = cs;
	m.data[1] = Node_ID;
	return f_can_send(bus_id,&m);
}
/****************************************************************************/
UNS8 masterSendNMTnodeguard(UNS8 bus_id, UNS8 nodeId)
{
	Message m;
  
	MSG_WAR(0x3503, "Send_NODE_GUARD to node : ", nodeId);
	
	/* message configuration */
	m.cob_id = nodeId | (NODE_GUARD << 7);
	m.rtr = REQUETE;
	m.len = 1;
  
	return f_can_send(bus_id,&m);
}
/******************************************************************************/
UNS8 masterReadNodeState(UNS8 bus_id, UNS8 nodeId)
{
	// FIXME: should warn for bad toggle bit.

	/* NMTable configuration to indicate that the master is waiting
	* for a Node_Guard frame from the slave whose node_id is ID */
	NMTable[bus_id][nodeId] = Unknown_state; // A state that does not exist
	NodeStateWaited[bus_id][nodeId] = TRUE;

	if (nodeId == 0) 
	{ // NMT broadcast
		UNS8 i = 0;
		for (i = 0 ; i < NMT_MAX_NODE_ID ; i++) 
		{
			NMTable[bus_id][i] = Unknown_state;
			NodeStateWaited[bus_id][i] = TRUE;
		}
	}
	masterSendNMTnodeguard(bus_id,nodeId);
	return 0;
}