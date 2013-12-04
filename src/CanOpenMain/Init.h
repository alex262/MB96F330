/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : init.h
 *********************************************************/
#ifndef __INIT_h__
#define __INIT_h__
/// buffer used by SDO

extern s_transfer transfers[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];	

/// those following arrays store the node_id from which a 
/// SDO frame response is waited for

extern WriteDic writeNetworkDictWaited[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];
extern ReadDic  readNetworkDictWaited[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];
extern UNS8 bDeviceNodeId;
extern e_nodeState nodeState;
/************************* prototypes ******************************/

/** Returns the state of the node */

e_nodeState getState (void);

/** Change the state of the node */

void setState (e_nodeState newState);

/** Returns the nodId */

UNS8 getNodeId (void);

/** Define the node ID. Initialize the object dictionary*/

UNS8 setNodeId (UNS8 nodeId);

/** Initialize the different arrays used by the application
 * return 0 */
 
UNS8 initCANopenMain (void);

#endif