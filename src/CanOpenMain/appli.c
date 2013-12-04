/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *

 *********************************************************
           File : appli.c
 *-------------------------------------------------------*
 * This is where you put your code                       *      
 *                                                       *
 *********************************************************/

#define PDO_ERROR /* To send in a PDO an error, in all situation */


/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */

#include <stddef.h> /* for NULL */
#include "applicfg.h"
#include "objacces.h"
#include "def.h"
#include "sdo.h"
#include "pdo.h"
#include "canOpenDriver.h"
#include "objdictdef.h"
#include "lifegrd.h"
#include "timer.h"
#include "nmtSlave.h" 
#include "init.h" 
#include "sync.h"
#include "appli.h"

#include "uart.h"

#include "global.h"

#include INCLUDE_H



// MB90 configuration
// -----------------------------------------------------


// Required definition variables
// -----------------------------
// The variables that you should define for debugging.
// They are used by the macro MSG_ERR and MSG_WAR in applicfg.h
// if the node is a slave, they can be mapped in the object dictionnary.
// if not null, allow the printing of message to the console
// Could be managed by PDO

//UNS8 printMsgErrToConsole = 1;
//UNS8 printMsgWarToConsole = 1;



/*************************User's variables declaration**************************/
e_nodeState lastState = Unknown_state;
//------------------------------------------------------------------------------
UNS8	ADDR_NODE=1;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


/*********************************************************************/
void heartbeatError( UNS8 heartbeatID )
{
  MSG_ERR(0x1F00, "HeartBeat not received from node : ", heartbeatID);
}


/*********************************************************************/
void initialisation( void )
{ 
	MSG_WAR(0X3F05, "I am in INITIALISATION mode ", 0);
	/* Defining the node Id */
	setNodeId(ADDR_NODE);
	//------------------------
	MSG_WAR(0x3F06, "My node ID is : ", getNodeId()); 
	initCANopenMain();    //initialisation du canopen 
	heartbeatInit();      //initialisation du lifeguarding
	
	SetupCAN(SPEED_CAN_0,SPEED_CAN_1,SPEED_CAN_2);	//initialisation du bus Can
}


/*********************************************************************/
void preOperational(void)
{
  // Test if the heartBeat have been received. Send headbeat
 // heartbeatMGR();
  // Read message
  //receiveMsgHandler(0);
}


/********************************************************************/
void operational( void )
{ 
  	// Test if the heartBeat have been received. Send headbeat
  	//heartbeatMGR();
 
  
}

/*****************************************************************************/
void stopped( void )
{
  //heartbeatMGR();
  // Read message
  //receiveMsgHandler(0);
}


/*****************************************************************************/

/********************************* MAIN ***************************************/

void DriverCanOpen()
{
	TYPE_DATA_TIMER t_Can0, t_Can1, t_Can2;
	BYTE sum_w_c,sum_c;
	BYTE i;
	
	//for(i=0; i<NB_LINE_CAN; i++)
	//{
	//	if(stInitCAN[i] == TRUE) receiveMsgHandler(i);
	//}				
	
	t_Can0 = getTimer(&program.TimerCan0);
	t_Can1 = getTimer(&program.TimerCan1);
	t_Can2 = getTimer(&program.TimerCan2);
	
	sum_w_c	=0;
	sum_c	=0;
	
	if(SPEED_CAN_0)
	{
		sum_c++;
		if(t_Can0 == 0)
		{
			(*STATE_BLOCK)|=ST_Can1; 
		}
		else
		{
			(*STATE_BLOCK)&=~ST_Can1; 
			sum_w_c++;
		}
	}
	if(SPEED_CAN_1)
	{
		sum_c++;
		if(t_Can1 == 0)
		{
			(*STATE_BLOCK)|=ST_Can2; 
		}
		else
		{
			(*STATE_BLOCK)&=~ST_Can2; 
			sum_w_c++;
		}
	}
	if(SPEED_CAN_2)
	{
		sum_c++;
		if(t_Can2 == 0)
		{
			(*STATE_BLOCK)|=ST_Can3; 
		}
		else
		{
			(*STATE_BLOCK)&=~ST_Can3; 
			sum_w_c++;
		}
	}
	if(sum_w_c >= NEED_WORK_CAN)
	{
		ClearError(ind_COM1);
	}else
	{
		SetError(ind_COM1);
	}
	/*if(sum_c == sum_w_c) // количество инициализированных CAN равно количеству CAN по которым идёт обмен
	{
		ClearError(ind_COM1);
	}*/
	if(sum_w_c == 0) // если не один CAN не работает
	{
		SetError(ind_COMA);
	}else
	{
		ClearError(ind_COMA);
	}
	// ------------------------------------
	// CanOpen slave state machine         
	// ------------------------------------

	switch( getState() ) 
	{				
		case Initialisation:
			if (lastState != getState()) 
			{
				MSG_WAR(0x3F10, "Entering in INITIALISATION mode ", 0);
				SetError(ind_TEST);
			}
			initialisation();
			// change automatically into pre_operational state  
			lastState = Initialisation;
			setState(Pre_operational);
			break;
					
		case Pre_operational:
			if (lastState != getState()) 
			{
				MSG_WAR(0x3F11, "Entering in PRE_OPERATIONAL mode ", 0);
				SetError(ind_TEST);
			}
			preOperational();
			//if (lastState == Initialisation)
			//slaveSendBootUp(0);
			lastState = Pre_operational;
			
			break;
		case Operational:
			if (lastState != getState()) 
			{
				MSG_WAR(0x3F12, "Entering in OPERATIONAL mode ", 0);
				ClearError(ind_TEST);
			}
			operational();
			lastState = Operational;	
			break;
			
		case Stopped:
			if (lastState != getState()) 
			{
				MSG_WAR(0X3F13, "Entering in  STOPPED mode", 0);
				SetError(ind_TEST);
			}
			stopped();
			lastState = Stopped;
			break;
	}//end switch case	
}
