/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : canOpenDriver.c
 *********************************************************/

//#define  DEBUG_CAN  
//#define DEBUG_WAR_CONSOLE_ON 
//#define DEBUG_ERR_CONSOLE_ON 

#include "mb96338us.h"

#include "canOpenDriver.h"
#include "can.h"
#include "objdictdef.h"
#include "uart.h"

extern proceed_info proceed_infos[];
extern UNS8 Num_Proc_Max;

/***************************************************************************/
/* receiveMsgHandler : call the receive function 
 and call the implied process function
 Parameters : the function used to receive can messages. 
 Function used only when platform is Linux or hcs12*/
UNS8 receiveMsgHandler(UNS8 nCAN)
{
	Message m;
	UNS8 fc;	       //the function code
	while(CAN_ReceiveMessage(nCAN, &m))
	{
		fc = GET_FUNCTION_CODE(m);//(m->cob_id >> 7);//
		MSG_ERR(0x0000, "CobID RX      :", m.cob_id);
		MSG_ERR(0x0000, "Function code :", fc);
		
		if(fc>Num_Proc_Max)
		{
			MSG_ERR(0x0000, "Function code is big :", fc);
			return 0xFF;
		}
		if(proceed_infos[fc].process_function!=NULL) 
		{
			proceed_infos[fc].process_function(nCAN,&m);
		}
	}
	return 0;   
}
void receiveMsgHandler_(UNS8 nCAN, Message *m)
{
	UNS8 fc;	       //the function code

	fc = GET_FUNCTION_CODE_ptr(m);
	MSG_ERR(0x0000, "CobID RX      :", m->cob_id);
	MSG_ERR(0x0000, "Function code :", fc);
		
	if(fc>Num_Proc_Max)
	{
		MSG_ERR(0x0000, "Function code is big :", fc);
		return;
	}
	if(proceed_infos[fc].process_function!=NULL) 
	{
		proceed_infos[fc].process_function(nCAN, m);
	}
}
