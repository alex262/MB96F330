/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : nmtSlave.h
 *                                                       *
 *********************************************************/


#ifndef __nmtSlave_h__
#define __nmtSlave_h__


/** Threat the reception of a NMT message from the master
 * *m is a pointer to the message received
 * bus_id is hardware dependant
 * return 0 if OK, -1 if the slave is not allowed, by its state,
 * to receive the message */
 
UNS8 proceedNMTstateChange (UNS8 bus_id, Message * m);

/** Transmit the boot-Up frame when the slave is moving from initialization
 * state to pre_operational state.
 * bus_id is hardware dependant
 * return f_can_send(bus_id,&m) */

UNS8 slaveSendBootUp (UNS8 bus_id);

#endif // __nmtSlave_h__
