/*********************************************************                   
 *             Master/slave CANopen Library              *
 *********************************************************
 *                 canOpenDriver.h                       *
 *********************************************************
*/

#ifndef __canOpenDriver_h__
#define __canOpenDriver_h__

#include "applicfg.h" 
#include "can.h" 

/** Called when a message is received by the application from the board
 * bus_id is hardware dependant
 * return 0xFF if error (unknown message received, 
 * else return proceed_infos[fc].process_function(bus_id,&m)
 * i.e return the result of the function Proceed_xxx called
 * Return 0 if no new message.*/

UNS8 receiveMsgHandler (UNS8 bus_id);
void receiveMsgHandler_(UNS8 nCAN, Message *m);



#endif // __canOpenDriver_h__
