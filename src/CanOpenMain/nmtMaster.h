/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *********************************************************
           File : nmtMaster.h
 *-------------------------------------------------------*
 *                                                       *      
 *                                                       *
 *********************************************************/

#ifndef __nmtMaster_h__
#define __nmtMaster_h__


/** Initialize the array NMTable (at UNKNOWN_STATE, a state that does not exist)
 * where are stored the slaves state
 * and the array NodeStateWaited which store the node_id of the slaves 
 * from which the master is waiting for the state
 * return 0 */
 
UNS8 initCANopenMaster (void);

/** Transmit a NMT message on the bus number bus_id
 * to the slave whose node_id is ID
 * bus_id is hardware dependant
 * cs represents the order of state changement:
 * cs =  NMT_Start_Node            // Put the node in operational mode             
 * cs =	 NMT_Stop_Node		   // Put the node in stopped mode
 * cs =	 NMT_Enter_PreOperational  // Put the node in pre_operational mode  
 * cs =  NMT_Reset_Node		   // Put the node in initialization mode 
 * cs =  NMT_Reset_Comunication	   // Put the node in initialization mode 
 * The mode is changed according to the slave state machine mode :
 *        initialisation  ---> pre-operational (Automatic transition)
 *        pre-operational <--> operational
 *        pre-operational <--> stopped
 *        pre-operational, operational, stopped -> initialisation
 *
 * return f_can_send(bus_id,&m)*/

UNS8 masterSendNMTstateChange (UNS8 bus_id, UNS8 Node_ID, UNS8 cs);

/** Transmit a Node_Guard message on the bus number bus_id
 * to the slave whose node_id is nodeId
 * bus_id is hardware dependant
 * return f_can_send(bus_id,&m) */
 
UNS8 masterSendNMTnodeguard (UNS8 bus_id, UNS8 nodeId);

/** Prepare a Node_Guard message transmission on the bus number bus_id
 * to the slave whose node_id is nodeId
 * Put nodeId = 0 to send an NMT broadcast.
 * This message will ask for the slave, whose node_id is nodeId, its state
 * bus_id is hardware dependant
 * return 0 */

UNS8 masterReadNodeState (UNS8 bus_id, UNS8 nodeId);


#endif // __nmtMaster_h__
