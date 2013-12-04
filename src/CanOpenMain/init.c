/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : init.c
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

#include "applicfg.h"
#include "canOpenDriver.h"
#include "can.h"
#include "def.h"
#include "objdictdef.h"
#include "objacces.h"
#include "init.h"
#include "lifegrd.h"
/************** variables declaration*********************************/
// buffer used by SDO
extern s_transfer transfers[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];	

e_nodeState nodeState;
/*********************************************************************/

e_nodeState getState(void)
{
	return nodeState;
}
/*********************************************************************/
void setState(e_nodeState newState)
{
	nodeState = newState;
}
/*********************************************************************/
UNS8 getNodeId(void)
{
	return bDeviceNodeId;
}
/*********************************************************************/
UNS8 setNodeId(UNS8 nodeId)
{
	UNS32 ret;
	UNS8 i;
	UNS32 pbData32;
	// bDeviceNodeId is defined in the object dictionary.
	bDeviceNodeId = nodeId;
	// Writing in the object dictionary
	// ** Initialize index 0x100B
	// At index 0x100B, we have put the nodeId. Why not ?
	ret = setODentry(0x100B, 0, &nodeId, 1, 0);
	// ** Initialize the server(s) SDO parameters
	// Remember that only one SDO server is allowed, defined at index 0x1200
	pbData32 = 0x600 + nodeId;
	ret = setODentry(0x1200, 1, &pbData32, 4, 0); // Subindex 1
	pbData32 = 0x580 + nodeId;
	ret = setODentry(0x1200, 2, &pbData32, 4, 0); // Subindex 2
	// Subindex 3 : node Id client. As we do not know the value, we put the node Id Server
	ret = setODentry(0x1200, 3, &nodeId, 1, 0);
	// ** Initialize the client(s) SDO parameters  
	// Nothing to initialize (no default values required by the DS 401)
	// ** Initialize the receive PDO communication parameters. Only for 0x1400 to 0x1403
	{
		UNS32 cobID[] = {0x200, 0x300, 0x400, 0x500};
		for (i = 0 ; (i < dict_cstes.max_count_of_PDO_receive)&&(i < 4) ; i++) 
		{
			pbData32 = cobID[i] + nodeId;
			ret = setODentry(0x1400 + i, 1, &pbData32, 4, 0); // Subindex 1
		}
	}
	// ** Initialize the transmit PDO communication parameters. Only for 0x1800 to 0x1803
	{
		UNS32 cobID[] = {0x180, 0x280, 0x380, 0x480};
		for (i = 0 ; (i < dict_cstes.max_count_of_PDO_transmit)&&(i < 4) ; i++) 
		{
			pbData32 = cobID[i] + nodeId;
			ret = setODentry(0x1800 + i, 1, &pbData32, 4, 0); // Subindex 1
		}
	}
	return 0;
}
/*********************************************************************/
UNS8 initCANopenMain(void)
{
	UNS8 i, k; 
	UNS8 j;

	/* transfer structure initialization */
	for (i = 0 ; i < MAX_CAN_BUS_ID ; i++) 
	{
		for (j = 0 ; j < SDO_MAX_NODE_ID ; j++) 
		{
			transfers[i][j].nodeId = 0;
			transfers[i][j].state = 0;
			transfers[i][j].index = 0;
			transfers[i][j].subindex = 0;
			transfers[i][j].count = 0;
			transfers[i][j].offset = 0;
			for (k = 0 ; k < SDO_MAX_DOMAIN_LEN ; k++)
				transfers[i][j].data[k] = 0;
		}
	}// end (i = 0 ; i < MAX_CAN_BUS_ID ; i++)

	/* writeNetworkDictWaited array initialization */
	for (i = 0 ; i < MAX_CAN_BUS_ID ; i++) 
	{
		for (j = 0 ; j < SDO_MAX_NODE_ID ; j++)
			writeNetworkDictWaited[i][j].state = SDO_SUCCESS;
	}
		
	/* readNetworkDictWaited array initialization */
	for ( i=0; i<MAX_CAN_BUS_ID; i++)
	{
		for ( j=0; j<SDO_MAX_NODE_ID; j++)
		{
			readNetworkDictWaited[i][j].state = SDO_SUCCESS;
			for ( k=0; k<SDO_MAX_DOMAIN_LEN; k++)
				readNetworkDictWaited[i][j].data[k] = 0;
		}
	}

  /* NMTable */
/*	for (i = 0 ; i < MAX_CAN_BUS_ID ; i++)
		for (j = 0 ; j < NMT_MAX_NODE_ID ; j++)
			NMTable[i][j] = Unknown_state;*/
    return 0;
}