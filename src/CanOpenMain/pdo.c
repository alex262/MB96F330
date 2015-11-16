/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : pdo.c
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
#include "mb96338us.h"
#include "applicfg.h"
#include "canOpenDriver.h"
#include "can.h"
#include "def.h"
#include "objdictdef.h"
#include "objacces.h"
#include "pdo.h"
/**************extern variables declaration*********************************/
extern e_nodeState  nodeState;	    		/* slave's state       */
extern UNS8         bDeviceNodeId;	   	/* NodeId             */
/********************variables declaration*********************************/

// buffer used by PDO
s_process_var process_var;
/****************************************************************************/
UNS8 sendPDO(UNS8 bus_id, s_PDO *pdo, UNS8 req)
{
	if( nodeState == Operational ) 
	{
		//Message m;
		/* Message copy for sending */
		//m.cob_id = 
		pdo->cob_id &= 0x7FF; // Because the cobId is 11 bytes length
		if ( req == DONNEES ) 
		{
		//	UNS8 i;
			//m.rtr = DONNEES;
			pdo->rtr= DONNEES;
		//	m.len = pdo->len;
		//	for (i = 0 ; i < pdo->len ; i++)
		//		m.data[i] = pdo->data[i];
		}
		else 
		{
			//m.rtr = REQUETE;
			//m.len = 0;
			pdo->len = 0;
			pdo->rtr = REQUETE;
		}
		
		MSG_WAR(0x3901, "sendPDO cobId :", pdo->cob_id);
		MSG_WAR(0x3902,  "     Nb octets  : ",  pdo->len);
		//for (i = 0 ; i < m.len ; i++) 
		//{
			//MSG_WAR(0x3920,"           data : ", m->data[i]);
		//}
		return f_can_send(bus_id,pdo);
	} // end if 
	return 0xFF;
}
/***************************************************************************/
UNS8 PDOmGR(UNS8 bus_id, UNS32 cobId) //PDO Manager
{
	UNS8 res;
	UNS8 i;
	s_PDO pdo;

	MSG_WAR(0x3903, "PDOmGR",0);
	/* if PDO is waiting for transmission,
	preparation of the message to send */
	if(process_var.state == (TS_DOWNLOAD & ~TS_WAIT_SERVER))
	{
		pdo.cob_id = cobId;
		pdo.len =  process_var.count;
		//memcpy(&(pdo.data), &(process_var.data), pdo.len);
		// Ce memcpy devrait être portable
		for ( i = 0 ; i < pdo.len ; i++) 
			pdo.data[i] = process_var.data[i];

		res = sendPDO(bus_id, &pdo, DONNEES);
		process_var.state |= TS_WAIT_SERVER;
		return res;
	}
	return 0xFF;
}
/**************************************************************************/
#ifdef CASH_PDO_DATA
CASH_PDO process_PDO_CASH;
s_PDO Cash_Pdo_Data[MAX_COUNT_OF_PDO_TRANSMIT];
UNS8 buildPDO_CASH(UNS16 index)
{
	UNS16 	ind;
	UNS8    subInd,i;
	UNS8 *  pMappingCount = NULL;      	// count of mapped objects...
	void *  pMappedAppObject = NULL; 	// pointer to the var which is mapped to a pdo
	UNS32 * pMappingParameter = NULL;  	// pointer fo the var which holds the mapping parameter of an mapping entry  
	UNS8 *  pSize;
	UNS8    size;
	UNS8 	offset;
	UNS8 	status;
	UNS32   objDict;	

	pSize = &size;
	status = state1;
	subInd=(UNS8)0x00;
	offset = 0x00;
	ind = index - 0x1800;


	if( nodeState != Operational ) return 0xFF;
	
	while (1) 
	{
		if(status == state1)
		{
				status = state2;
		}
		if(status == state2)
		{
			objDict = getODentry( (UNS16)0x1A00 + ind, (UNS8)0,(void * *)&pMappingCount, pSize, 0 );
			if( objDict == (UNS32)OD_SUCCESSFUL )
			{
				status = state3;
				if(*pMappingCount==0)
					return 0xFF;
			}
			else 
				return 0xFF;
		}
		if(status == state3)
		{
			objDict = getODentry( (UNS16)0x1A00 + ind, subInd + 1,(void * *)&pMappingParameter, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				status = state4;
			}
			else 
				return 0xFF;
		}
		if(status == state4)
		{
			objDict = getODentry((UNS16)((*pMappingParameter) >> 16),
								(UNS8)(((*pMappingParameter) >> 8 ) & 0x000000FF),
								(void * *)&pMappedAppObject, pSize, 0 ); 
			if( objDict == OD_SUCCESSFUL )
			{
				size = (*pMappingParameter & (UNS32)0x000000FF) >> 3 ; // in bytes
	  			//DisInterrupt();	 // 4.7 uc
	  			for(i=0;i<size;i++)	
	  				process_PDO_CASH.msg[offset+i]=((UNS8 *)pMappedAppObject)[i];
	  			//EnInterrupt();
				offset += size;
				process_PDO_CASH.count = offset;
				subInd++;
				status = state5;
			}
			else 
				return 0xFF;
		}
		if(status == state5)
		{
			if( subInd < *pMappingCount )
			{
				status = state3;
			}
			else 
			{
				//DisInterrupt();
				pMappingCount = Cash_Pdo_Data[ind].data;
				pSize = process_PDO_CASH.msg;
				
	  			for(i=0;i<offset;i++)
				{
					pMappingCount[i] = pSize[i];
					//Cash_Pdo_Data[ind].data[i]=process_PDO_CASH.msg[i];	
				}
				Cash_Pdo_Data[ind].len=offset;
				//EnInterrupt();
				return 0;
			}
		}
	} // end while
}
#endif
//*************************************************************************************************
UNS8 buildPDO(UNS16 index, UNS32 **pwCobId)
{
	UNS16 ind;
	UNS8      subInd;
	UNS8 *     pMappingCount = NULL;      // count of mapped objects...
	// pointer to the var which is mapped to a pdo
	void *     pMappedAppObject = NULL; 
	// pointer fo the var which holds the mapping parameter of an mapping entry  
	UNS32 *    pMappingParameter = NULL;  
	UNS8 *    pSize;
	UNS8      size;
	UNS8 offset;
	UNS8 status;
	UNS32    objDict;	

	pSize = &size;
	status = state1;
	subInd=(UNS8)0x00;
	offset = 0x00;
	ind = index - 0x1800;

	MSG_WAR(0x3904,"Prepare PDO to send index :", index);

	/* only operational state allows PDO transmission */
	if( nodeState != Operational ) 
	{
		MSG_WAR(0x2905, "Unable to send the PDO (node not in OPERATIONAL mode). Node : ", index);
		return 0xFF;
	}
	while (1) 
	{
		//switch( status ) 
		{
		//case state1:	/* get PDO CobId */
		if(status == state1)
		{
			objDict = getODentry( index, (UNS8)1,(void * *)pwCobId, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				status = state2;
			//	break;
			}
			else 
			{
				MSG_ERR(0x1906,"Error in dictionnary subIndex 1, index : ", index);
				return 0xFF;
			}
		}
		//case state2:  /* get mapped objects number to transmit with this PDO */
		if(status == state2)
		{
			objDict = getODentry( (UNS16)0x1A00 + ind, (UNS8)0,(void * *)&pMappingCount, pSize, 0 );
			MSG_WAR(0x3907, "Nb maped objects : ",* pMappingCount);
			MSG_WAR(0x3908, "        at index : ", 0x1A00 + ind);
			if( objDict == (UNS32)OD_SUCCESSFUL )
			{
				status = state3;
			//	break;
			}
			else 
				return 0xFF;
        }
        //case state3:	/* get mapping parameters */
		if(status == state3)
		{
			objDict = getODentry( (UNS16)0x1A00 + ind, subInd + 1,(void * *)&pMappingParameter, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x3909, "Get the mapping      at index : ", (UNS16)0x1A00 + ind);
				MSG_WAR(0x390A, "                     subIndex : ", subInd + 1);
				MSG_WAR(0x390B, "                     value    : ", *(UNS32 *)pMappingParameter);
				status = state4;
			//	break;
			}
			else 
				return 0xFF;
        }
        //case state4:	/* get data to transmit */
        if(status == state4)
		{
			objDict = getODentry( (UNS16)((*pMappingParameter) >> 16),
								(UNS8)(((*pMappingParameter) >> 8 ) & 0x000000FF),
								(void * *)&pMappedAppObject, pSize, 0 ); 
			MSG_WAR(0x390C, "Write in PDO the variable : ",((*pMappingParameter) >> 16));
			MSG_WAR(0x390D, "                 subIndex : ",(UNS8)(((*pMappingParameter) >> 8 ) & 0x000000FF));/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x390E, "                 value    : ", *(UNS32*)pMappedAppObject);
				memcpy(&process_var.data[offset],pMappedAppObject,(*pMappingParameter&(UNS32)0x000000FF)>>3 );
#       ifdef CANOPEN_BIG_ENDIAN
				{
					// data must be transmited with low byte first
					UNS8 pivot, i;
					UNS8 sizeData = (*pMappingParameter & (UNS32)0x000000FF) >> 3 ; // in bytes
					for ( i = 0 ; i < ( sizeData >> 1)  ; i++) 
					{
						pivot = process_var.data[offset + (sizeData - 1) - i];
						process_var.data[offset + (sizeData - 1) - i] = process_var.data[offset + i];
						process_var.data[offset + i] = pivot ;
					}
				}
#       endif
				offset += (UNS8) ((*pMappingParameter & (UNS32)0x000000FF) >> 3) ;
				process_var.count = offset;
				subInd++;
				status = state5;
			//	break;					
			}
			else 
			{
				MSG_ERR(0x190F, "no mapped data ",0);
				return 0xFF;
			}
    	}
    	//case state5:	/* loop to get all the data to transmit */
		if(status == state5)
		{
			if( subInd < *pMappingCount )
			{
				status = state3;
			//	break;
			}
			else 
			{
				status = state6;	
			//	break;
			}
		}
		//case state6:
		if(status == state6)
			return 0;
		}// end switch case
	} // end while
  
//	return 0;
}
/**************************************************************************/
void sendPDOrequest( UNS8 bus_id, UNS32 cobId )
{		
	UNS8     ind = 0;		/*index*/
	UNS32 *	 pwCobId;	
	UNS8 *   pSize;
	UNS8     size;
	UNS32    objDict;  
	pSize = &size;

	MSG_WAR(0x3910, "sendPDOrequest ",0);  
	// Sending the request only if the cobid have been found on the PDO receive
	// part dictionary
	for( ind = (UNS8)0x00; ind < (UNS8)dict_cstes.max_count_of_PDO_transmit;) 
	{ 
		/*get the CobId*/
		// *pwCobId & 0x7f : to have only the  7 bits of the node Id
      	objDict = getODentry( (UNS16)0x1400/*0x1800*/ + ind, (UNS8)1,(void * *)&pwCobId, pSize, 0 );
    	if( ( objDict == OD_SUCCESSFUL) && ((*pwCobId & 0x7f) == cobId ) ) 
    	{
      		s_PDO pdo;
      		pdo.cob_id = *pwCobId;
      		pdo.len = 0;
      		sendPDO(bus_id, &pdo, REQUETE);	
    	}
	}
}
/***********************************************************************/
UNS8 proceedPDO(UNS8 bus_id, Message *m)
{			
	UNS8   	numPdo,
    		numMap;  // Number of the mapped varable                           
	UNS8 		i;
	UNS8 *     	pMappingCount = NULL;    // count of mapped objects...
	// pointer to the var which is mapped to a pdo...
	void *     	pMappedAppObject = NULL;  
	// pointer fo the var which holds the mapping parameter of an mapping entry
	UNS32 *    	pMappingParameter = NULL;  
	UNS8  *    	pTransmissionType = NULL; // pointer to the transmission type
	UNS32 *    	pwCobId = NULL;
	UNS8  *    	pSize;
	UNS8       	size;
	UNS8        offset;
	UNS8        status;
	UNS32       objDict;
	UNS8 		sizeData;

	pSize = &size;
	status = state1;

	/* operational state only allows PDO reception */
	if(nodeState == Operational) 
	{ 
		//UNS8 fc;	// the function code
		status = state1;
		// Hack to map PDOtx 1, 2, 3, & 4 in the same table
		//nodeId = (UNS8) (GET_NODE_ID((*m))); // The id can be different than the node Id. I must change that.
		MSG_WAR(0x3931, "proceedPDO, cobID : ", ((*m).cob_id & 0x7ff)); 
    
//		fc = (UNS8) (GET_FUNCTION_CODE((*m)));
		offset = 0x00;
		numPdo = 0;
		numMap = 0;

		if((*m).rtr == DONNEES ) 
		{ // The PDO received is not a request.
			MSG_WAR(0x3930, "max count of PDO received = ", dict_cstes.max_count_of_PDO_receive);

			/* study of all the PDO stored in the dictionary */   
			while( numPdo < dict_cstes.max_count_of_PDO_receive)
			{
				//switch( status )
				{
				//case state1:	/* data are stored in process_var array */
				if(status == state1)
				{	//memcpy(&(process_var.data), &m->data, (*m).len);
					// Ce memcpy devrait être portable.
					for ( i = 0 ; i < m->len ; i++) 
						process_var.data[i] = m->data[i];
					process_var.count = (*m).len;
					process_var.state = ~TS_WAIT_SERVER;

					status = state2; 
				//	break;
				}
				//case state2:
				if(status == state2)
				{
					/* get CobId of the dictionary correspondant to the received PDO */
					objDict = getODentry( (UNS16)0x1400 + numPdo, (UNS8)1,(void * *)&pwCobId, pSize, 0); 
					if( objDict == OD_SUCCESSFUL )
					{
						status = state3;
						MSG_WAR(0x3936, "PDO read in the object dictionnary : 0x1400 + ", numPdo);
				//		break;	
					}
					else 
					{
						MSG_ERR(0x1949, "PDO cannot be read in the object dictionnary : 0x1400 + ", numPdo);
						return 0xFF;
					}
				}
				//case state3:	/* check the CobId coherance */
				if(status == state3)	//*pwCobId is the cobId read in the dictionary at the state 3
				{
					if( *pwCobId == ((*m).cob_id & 0x7ff) )
					{
						// The cobId is recognized
						status = state4;
						MSG_WAR(0x3937, "the cobId is recognized :  ",*pwCobId );
				//		break;
					}
					else 
					{
						// cobId received does not match with those write in the dictionnary
						MSG_WAR(0x3950, "the cobId received isn't those in dict : ", *pwCobId);
						numPdo++;
						status = state2;
						/* you need to read again the data  transfered in PDO because 
						process_var is modified in MSG_ERR:sending PDO error 
						The lines following are very strange and should be removed. (FD)*/
						for ( i = 0 ; i < m->len ; i++) 
							process_var.data[i] = m->data[i];
						process_var.count = (*m).len;
						process_var.state = ~TS_WAIT_SERVER;
					//	break;
					}
				}
				if(status == state4)
				{
				//case state4:	/* get mapped objects number */
					// The cobId of the message received has been found in the dictionnary.
					objDict = getODentry((UNS16)0x1600 + numPdo, (UNS8)0,
										(void * *)&pMappingCount, pSize, 0); 
					// pMappingCount : number of mapped variables. It is a UNS8
					if( objDict  == OD_SUCCESSFUL )
					{	
						status = state5;
						MSG_WAR(0x3938, "Number of objects mapped : ",*pMappingCount );
				//		break;
					}
					else 
					{
						MSG_ERR(0x1951, "Failure while trying to get map count : 0X1600 + ", numPdo);	
						return 0xFF;
					}
				}
				if(status == state5)
				{
				//case state5:	/* get mapping parameters. Read the subindex (numMap + 1)*/
					/* ex : for numPdo = 0 and numMap = 0,
					* pMappingParameter points to Index1600[1].pobject, ie  RxMap1.object[0].
					* defined in objdict.c
					* RxMap1.object[i] (32 bits) contains the index, subindex where the mapped 
					* variable is defined, and its size. (cf ds301 V.4.02 object 1600h p.109).
					* pSize points to  Index1600[1].size, which contains the size of RxMap1.object[i]
					*/
					objDict = getODentry( (UNS16)0x1600 + numPdo, numMap + 1,
					(void * *)&pMappingParameter, pSize, 0);
					if ( objDict == OD_SUCCESSFUL ) 
					{
						MSG_WAR_long(0x3939, "got mapping parameter : ", *pMappingParameter);
						status = state6;
				//		break;
					}
					else 
					{
						MSG_ERR(0x1952, "Couldn't get mapping parameter for subindex : ", numMap + (UNS8)1);
						return 0xFF;
					}
				}
				if(status == state6)
				{
				//case state6:
					/* ex : for numPdo = 0 and numMap = 0,
					* *pMappingParameter points to  RxMap1.object[0] (RxMap1.object[1] if numMap = 1)
					* where the 16 MSB bits
					* contains the index where the value is defined.
					* The medium 8 bits contains the subindex where the value is defined.
					* The lower 8 bits contains the  length of the variable.
					* (See ds301 V.4.02 object 1600h p.109 fig 66)*/
					objDict = getODentry((UNS16)((*pMappingParameter) >> 16),
										(UNS8)(((*pMappingParameter) >> 8 ) & 0xFF),
										(void * *)&pMappedAppObject, pSize, 0 );
					/* pMappedAppObject points to the variable mapped, and pSize to its size.   */
					if( objDict == OD_SUCCESSFUL ) 
					{
						status = state7;
						MSG_WAR(0x393A, "Receiving data", 0);
				//		break;
					}
					else 
						return 0xFF;
				}
				if(status == state7)
				{
				//case state7: /*attribution of the data to the corresponding data*/
					/* the variable of the application is updated with the value
					* received in the PDO. OUF !  */

#       ifdef CANOPEN_BIG_ENDIAN
					{
						UNS8 pivot, i;
						UNS8 sizeData = (*pMappingParameter & 0xFF) >> 3 ; // in bytes
						for ( i = 0 ; i < ( sizeData >> 1)  ; i++) 
						{
							pivot = process_var.data[offset + (sizeData - 1) - i];
							process_var.data[offset + (sizeData - 1) - i] = process_var.data[offset + i];
							process_var.data[offset + i] = pivot ;
						}
					}
#       endif
				
					//memcpy( pMappedAppObject, &process_var.data[offset],(((*pMappingParameter) & 0xFF ) >> 3));
	  				sizeData = (*pMappingParameter & 0xFF) >> 3 ; 
	  				for(i=0;i<sizeData;i++)
							((UNS8 *)pMappedAppObject)[i]=process_var.data[offset+i];
							
					MSG_WAR(0x3932, "Variable updated with value received by PDO cobid : ", m->cob_id);  
					MSG_WAR(0x3933, "         Mapped at index : ", (*pMappingParameter) >> 16);
					MSG_WAR(0x3934, "                subindex : ", ((*pMappingParameter) >> 8 ) & 0xFF);
					MSG_WAR(0x393B, "                data : ",*((UNS32 *)pMappedAppObject));
					offset += (UNS8) (((*pMappingParameter) & 0xFF) >> 3) ;
					numMap++;	
					status = state8;
				//	break;
				}
				if(status == state8)
				{
				//case state8:	
					/*loop to attribuate all data to the corresponding variables*/
					if( numMap < *pMappingCount ) 
					{
						status = state5;	
					//	break;
					}
					else 
					{
						offset=0x00;		
						numMap = 0;
						return 0;
					}
				}
				}// end switch status		 
			}// end while	
		}// end if Donnees 
		else if ((*m).rtr == REQUETE )
		{  
			MSG_WAR(0x3935, "Receive a PDO request cobId : ", m->cob_id);
			while( numPdo < dict_cstes.max_count_of_PDO_transmit)
			{ 
				/* study of all PDO stored in the objects dictionary */
				//switch( status )
				{
				if(status == state1)
				{
				//case state1:	/* check the CobId */
					/* get CobId of the dictionary which match to the received PDO */
					objDict = getODentry((UNS16)0x1800 + numPdo, (UNS8)1,
										(void * *)&pwCobId, pSize, 0);
					if (objDict == OD_SUCCESSFUL && (*pwCobId==(*m).cob_id)) 
					{
						status = state4;
				//		break;
					}
					else
					{
						numPdo++;
						status = state1;
					//	break;
					}
				}
				if(status == state4)
				{
				//case state4:	/* check transmission type (after request?) */
					objDict = getODentry( (UNS16)0x1800 + numPdo, 2, 
										(void * *)&pTransmissionType, pSize, 0);
					if ((objDict == OD_SUCCESSFUL) && ((*pTransmissionType == TRANS_RTR) 
						|| (*pTransmissionType == TRANS_RTR_SYNC )) ) 
					{
						status = state5;
						//================================================================
						//	Send Cash data
						//================================================================
						#ifdef CASH_PDO_DATA
						//status=Cash_Pdo_Data[numPdo].len;
						//for(i=0;i<status;i++)
						//{
						//	process_var.data[i]=Cash_Pdo_Data[numPdo].data[i];
						//}
						//process_var.count=status;
						Cash_Pdo_Data[numPdo].cob_id=*pwCobId;
						sendPDO(bus_id, &Cash_Pdo_Data[numPdo], DONNEES);
						process_var.state |= TS_WAIT_SERVER;
						//putch(0x30+numPdo);
						return 0;
						#endif
						//================================================================
						//break;
					}
					else
						// The requested PDO is not to send on request. So, does nothing.
						return 0xFF;
				}
				if(status == state5)
				{
				//case state5:	/* get mapped objects number */
					objDict = getODentry( (UNS16)0x1A00 + numPdo, 
											0,(void * *)&pMappingCount, pSize, 0 );
					if( objDict  == OD_SUCCESSFUL ) 
					{
						status = state6;
					//	break;
					}
					else
						return 0xFF;
				}
				if(status == state6)
				{
				//case state6:	/* get mapping parameters */
					/* state 6, 7, 8 compute a message with the variables of the application,
					* to send them in a mapping PDO
					*/
					objDict = getODentry((UNS16)0x1A00 + numPdo,numMap + (UNS8)1,
										(void * *)&pMappingParameter, pSize, 0 );
					if( objDict == OD_SUCCESSFUL ) 
					{		
						status = state7;
					//	break;
					}
					else
						return 0xFF;
				}
				if(status == state7)
				{
				//case state7:
					objDict = getODentry((UNS16)((*pMappingParameter) >> (UNS8)16), 
								(UNS8)(((*pMappingParameter) >> (UNS8)8) & 0xFF),
								(void * *)&pMappedAppObject, pSize, 0 );
					if( objDict == OD_SUCCESSFUL )
					{
						// Comme on copie vers un tableau d'octets, il n'y a pas de contrainte
						// d'alignement. Ce memcpy devrait être portable.
						//memcpy(&process_var.data[offset],pMappedAppObject,((*pMappingParameter)&0xFF)>>3); 
						
						sizeData = (*pMappingParameter & 0xFF) >> 3 ;
						for(i=0;i<sizeData;i++)
							process_var.data[offset+i]=((UNS8 *)pMappedAppObject)[i];
							
#       ifdef CANOPEN_BIG_ENDIAN
						{
							UNS8 pivot, i;
							UNS8 sizeData = (*pMappingParameter & (UNS32)0x000000FF) >> 3 ; // in bytes
							for ( i = 0 ; i < ( sizeData >> 1)  ; i++) 
							{
								pivot = process_var.data[offset + (sizeData - 1) - i];
								process_var.data[offset + (sizeData - 1) - i] = process_var.data[offset + i];
								process_var.data[offset + i] = pivot ;
							}
						}
#       endif
						offset += (UNS8) (((*pMappingParameter) & 0xFF) >> 3)  ;
						process_var.count = offset;
						numMap++;
						status = state8;
					//	break;
					}
					else
						return 0xFF;
				}
				if(status == state8)
				{
				//case state8:	/* loop to get all mapped objects */
					if( numMap < *pMappingCount )
					{
						status = state6;	
					//	break;
					}
					else 
					{
						process_var.state =(TS_DOWNLOAD & ~TS_WAIT_SERVER);
						PDOmGR( bus_id, *pwCobId ); // Transmit the PDO
						// I think we should quit the function here by a return 0 (FD)
						status = state9;
					//	break;
					}
				}
				if(status == state9)
				{
				//case state9:	
					return 0;
				}
				}// end switch status
			}// end while				
		}// end if Requete
		return 0;
	}// end if( nodeState == Operational)
	return 0;
}
/*********************************************************************/
UNS8 sendPDOevent( UNS8 bus_id, void * variable )
{	
	UNS32      objDict = 0;
	UNS8       ind, sub_ind;
	UNS8       status; 
	UNS8       offset;
	UNS8 *     pMappingCount = NULL;
	UNS32 *    pMappingParameter = NULL;
	void *     pMappedAppObject = NULL;
	UNS8 *     pTransmissionType = NULL; // pointer to the transmission type
	UNS32 *    pwCobId = NULL;	
	UNS8 *     pSize;
	UNS8       size;

  
	ind     = 0x00;
	sub_ind = 1; 
	offset  = 0x00;
	pSize   = &size;
	status  = state1;

	// look for the index and subindex where the variable is mapped
	// Then, send the pdo which contains the variable.

	MSG_WAR (0x193A, "sendPDOevent", 0);
	while(1)
	{
		//switch ( status ) 
		{
		if(status == state1)
		{
		//case state1 : 
			//get the number of entries, i.e. get the number of mapped objects
			objDict = getODentry((UNS16)0x1A00 + ind,0,(void * *)&pMappingCount,pSize,0);
			if( objDict == OD_SUCCESSFUL ) 
			{
				// Some objects are mapped at the index 0x1A00 + ind
				status = state2;
			//	break;
			}
			else 
			{
				MSG_WAR(0x393B,"Unable to send variable on event :  not mapped in a PDO to send on event", 0);
				return 0xFF;
			}
		}
		if(status == state2)
		{
		//case state2 : //get the Mapping Parameters 
			if ( sub_ind <= *pMappingCount )
			{
				objDict = getODentry( (UNS16)0x1A00 + ind, sub_ind,(void * *)&pMappingParameter, pSize, 0);
				status = state3;
				// pMappingParamete contains the index (16b), subindex(8b) and the
				// length (8b) of the mapped object
			}
			else 
			{
				// all the mapped objects at this index have been scanned
				ind++;
				sub_ind = 1;
				status = state1;
			}
			//break;
		}
		if(status == state3)
		{
		//case state3 :
			if( objDict == OD_SUCCESSFUL ) 
			{
				status = state4;
			//	break;
			}
			else 
			{
				// should never happend
				MSG_ERR(0x193D, "Error in dict. at index ... : probably nb on subindex (index 0) > subindex defined. Index : ", 0x1A00 + ind);
				return 0xFF;
			}
      	}
      	if(status == state4)
		{
		//case state4 :  //get the address of the corresponding variable
			objDict = getODentry( (UNS16)((*pMappingParameter) >> 16), 
					(UNS8)(( (*pMappingParameter) >> (UNS8)8 ) & (UNS32)0x000000FF),
					(void * *)&pMappedAppObject, pSize, 0 );
			if( objDict == OD_SUCCESSFUL ) 
			{
				status = state5;
			//	break;
			}
			else 
			{
				MSG_ERR(0x193E, "Error in dict. at index : ", (*pMappingParameter) >> (UNS8)16);
				MSG_ERR(0x193F, "               subindex : ", ((*pMappingParameter) >> (UNS8)8 ) & (UNS32)0x000000FF);
				return 0xFF;
			}
		}
		if(status == state5)
		{
		//case state5 :
			if ( pMappedAppObject == variable ) 
			{
				//MSG_WAR(0x3940, "Variable to send found at index : ", (*pMappingParameter) >> 16);
				//MSG_WAR(0x3941, "                       subIndex : ", ((*pMappingParameter) >> 8 ) & 0x000000FF);
				
				sub_ind = 0;
				
				//Mapped PDO at index (0x1A00 + ind) use the PDO parameters defined at (0x1800 + ind).
				objDict = getODentry( (UNS16)0x1800 + ind, (UNS8)1,
						(void * *)&pwCobId, pSize, 0);
				if( objDict == OD_SUCCESSFUL ) 
				{
					status = state6;
					//MSG_WAR(0x3042, "                          cobid : ", *pwCobId & 0x7FF);
					//MSG_WAR(0x3043, "           is definded at index : ", 0x1800 + ind);
				}
				else 
				{
					MSG_ERR(0x1944, "Error in dict at subIndex : 1, index : ", 0x1800 + ind);
					return 0xFF;
				}
			//	break;
			} // end if ( pMappedAppObject == variable )
			else 
			{
				sub_ind++;
				status = state2;
			}
			//break;
		}
		if(status == state6)
		{
		//case state6 :	/* check transmission type (after request?) */
			objDict = getODentry( (UNS16)0x1800 + ind, (UNS8)2,(void * *)&pTransmissionType, pSize, 0);
			if( objDict == OD_SUCCESSFUL) 
			{
				status = state7;
			//	break;
			}
			else 
			{
				MSG_ERR(0x1945, "Error in dict at subIndex : 2, index : ", 0x1800 + ind);
				return 0xFF;
			}
		}
		if(status == state7)
		{
		//case state7:
			if(*pTransmissionType == TRANS_EVENT) 
			{
				status = state8;
			//	break;
			}
			else
			{
				// The variable have been found in a PDO, but this PDO is not to send on request. But ...
				// Perhaps the variable is mapped in an other PDO, which is to send on request ?
				// So we must continue to scan the PDO Transmit entries.
				ind++;
				status = state1;
				//MSG_WAR(0x2946, "Not allowed to send the variable on event : transmission type does not match 255 at subindex : 2, index : ", 0x1800 + ind);
				//MSG_WAR(0x2947, "      Transmission type is : ", *pTransmissionType);
				return 0xFF;
			}
		}
		if(status == state8)
		{//case state8:
			buildPDO(0x1800 + ind, &pwCobId);
			process_var.state = (TS_DOWNLOAD & ~TS_WAIT_SERVER);
			PDOmGR( bus_id, *pwCobId ); // Send the PDO
			return 0;
		}
		} // end switch
	} // end while(1)
//	return 0;
}