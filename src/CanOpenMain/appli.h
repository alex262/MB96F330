/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : appli.h
 *                                                       *
 *********************************************************/
#include <applicfg.h>
#ifndef __appli_h__
#define __appli_h__
/**************************prototypes*********************/
/**
This function must be implemented in the user's application
code.It is called when a heartbeat have not been received 
befor elapsed time.*/

//void heartbeatError (UNS8 heartbeatID);
//--------------------------------FONCTIONS-------------------------------------
// Interruption timer 3. (The timer 4 is used by CanOpen)
void incDate();
void initialisation(void);
void preOperational(void);
void operational(void);
void stopped(void);

void DriverCanOpen();

extern UNS8	ADDR_NODE;


#endif // __appli_h__
