/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : sync.c
 *                                                       *
 *********************************************************/

/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */
#include "mb96338us.h"
#include "stddef.h" /* for NULL */
#include "string.h"
#include "stdio.h"

#include "applicfg.h"
#include "canOpenDriver.h"
#include "can.h"
#include "def.h"
#include "objdictdef.h"
#include "objacces.h"
#include "timer.h"
#include "pdo.h"
#include "sync.h"



/**************extern variables declaration*********************************/
extern UNS8 bDeviceNodeId;	      // module node_id 
extern e_nodeState nodeState;	      // slave's state in the state machine 
extern s_process_var process_var;

/********************variables declaration*********************************/

s_sync_values SyncValues ; 

/****************************************************************************/
/*UNS8 computeSYNC(void)
{
  	UNS32 * ptrcobid ;
  	UNS32 * pwPeriodSync;
  	UNS8  * pSize;
  	UNS8    size;
  	UNS32   objDict;
  	UNS32 cob_id ;
  	UNS32 time;
	pSize = &size;

  	if( nodeState != Operational ) 
    	return 1;
  
	objDict = getODentry( (UNS16) 0x1005,(UNS8) 0, (void * *)(&ptrcobid), pSize, 0 );

  	if ( objDict == OD_SUCCESSFUL )
    {
      	if ( (*ptrcobid) & 0x40000000) 
		{
	  		objDict = getODentry( (UNS16) 0x1006,(UNS8) 0,  (void * *)&pwPeriodSync, pSize, 0 ); 
	  		if ( objDict == OD_SUCCESSFUL )
	    	{
	      		if(*pwPeriodSync)
				{
		  			time = getTime32( &SyncValues.ourTime );
					if( ( time >= SyncValues.ourShouldTime ) )
					{
						// Time expired, the sync must be sent immediately
						setTime32( &(SyncValues.ourTime), 0 );
						SyncValues.ourLastTime = 0;
						SyncValues.ourShouldTime = (UNS32) (*pwPeriodSync) ;
						cob_id = (*ptrcobid) & 0x1FFFFFFF ;
						sendSYNC(0, cob_id ) ;
						return 0 ;
					}
					else
					{
						SyncValues.ourLastTime = getTime32( &(SyncValues.ourTime) );
						return 1 ;
					}
				}
				else
					return 1 ;
			}
		}  
    }
	MSG_ERR(0x1000,"Compute sync error", 0);
	return 0xFF;
}*/
/*********************************************************************/
UNS8 sendSYNC(UNS8 bus_id, UNS32 cob_id)
{
	Message m;
	UNS8 resultat ;
  
	MSG_WAR(0x3001, "sendSYNC ", 0);
  
	m.cob_id = cob_id ;
	m.rtr = DONNEES;
	m.len = 0;
	resultat = f_can_send(bus_id,&m) ;
	if (resultat)
    {
      proceedSYNC(bus_id, &m) ; 
    }
	return resultat ;
}
/*****************************************************************************/
#ifdef CASH_PDO_DATA
extern CASH_PDO process_PDO_CASH;
extern s_PDO Cash_Pdo_Data[MAX_COUNT_OF_PDO_TRANSMIT];
#endif;
UNS8 proceedSYNC(UNS8 bus_id, Message *m)
{

	UNS8 	pdoNum,       // number of the actual processed pdo-nr.
		prp_j;

	UNS8 *     pMappingCount = NULL;      // count of mapped objects...
	
	// pointer to the var which is mapped to a pdo
	void *     pMappedAppObject = NULL; 
	
	// pointer fo the var which holds the mapping parameter of an mapping entry  
	UNS32 *    pMappingParameter = NULL;  
  
	// pointer to the transmissiontype...
	UNS8 *     pTransmissionType = NULL;  
	UNS32 *    pwCobId = NULL;	
	UNS32   objDict;	

	UNS8 *    pSize;
	UNS8      size;
	UNS16 index;
	UNS8 subIndex;
	UNS8 offset;
	UNS8 status;
	UNS8 sizeData;

	pSize = &size;

	MSG_WAR(0x3002, "SYNC received. Proceed. ", 0);

	status = state3;
	pdoNum=(UNS8)0x00;
	prp_j=(UNS8)0x00;
	offset = 0x00;

	/* only operational state allows PDO transmission */
	if( nodeState != Operational ) 
		return 0;
	
	/* So, the node is in operational state */
	/* study all PDO stored in the objects dictionary */	
 
	while( pdoNum < dict_cstes.max_count_of_PDO_transmit ) 
	{  
		//switch( status ) 
		if (status == state3 )
		{	
		//case state3:    /* get the PDO transmission type */
			objDict = getODentry( (UNS16)0x1800 + pdoNum, (UNS8)2,
						(void * *)&pTransmissionType, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x3005, "Reading PDO at index 0x1800 + ", pdoNum);
				status = state4; 
				//break;
			}
			else 
			{
				MSG_ERR(0x1006, "PDO cannot be read at index : 0x1800 + ", pdoNum);
				return 0xFF;
			}
		}
		//case state4:	/* check if transmission type is after (this) SYNC */
                        /* The message may not be transmited every SYNC but every n SYNC */
      	if (status == state4 )
		{
			if((*pTransmissionType >= TRANS_SYNC_MIN) && (*pTransmissionType <= TRANS_SYNC_MAX) &&
				(++count_sync[bus_id][pdoNum] >= *pTransmissionType) ) 
			{	
				count_sync[bus_id][pdoNum] = 0;
				MSG_WAR(0x3007, "  PDO is on SYNCHRO. Trans type : ", *pTransmissionType);
				#ifdef CASH_PDO_DATA
					objDict = getODentry((UNS16)0x1800 + pdoNum, (UNS8)1,(void * *)&pwCobId, pSize, 0);
					if (objDict == OD_SUCCESSFUL) 
					{
						Cash_Pdo_Data[pdoNum].cob_id=*pwCobId;
						Cash_Pdo_Data[pdoNum].rtr   =DONNEES;
						f_can_send(bus_id,&Cash_Pdo_Data[pdoNum]);
						//sendPDO(bus_id, &Cash_Pdo_Data[pdoNum], DONNEES);
					}
					pdoNum++;
					status = state3;
				#else
					status = state5;
				#endif;
			}
			else 
			{
				MSG_WAR(0x3008, "  Not on synchro or not at this SYNC. Trans type : ",*pTransmissionType);
				pdoNum++;
				status = state3;
			}
		}
		//case state5:	/* get PDO CobId */
		if (status == state5 )
		{
			objDict = getODentry( (UNS16)0x1800 + pdoNum, (UNS8)1, 
					(void * *)&pwCobId, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x3009, "  PDO CobId is : ", *pwCobId);
				status = state7;
				//break;
			}
			else 
			{
				MSG_ERR(0x100A, "  PDO CobId cannot be read at index 0x1800 +", pdoNum);
				return 0xFF;
			}
		}
		//case state7:  /* get mapped objects number to transmit with this PDO */
		if (status == state7 )
		{
			objDict = getODentry( (UNS16)0x1A00 + pdoNum, (UNS8)0,
						(void * *)&pMappingCount, pSize, 0 );
			if( objDict == (UNS32)OD_SUCCESSFUL )
			{
				MSG_WAR(0x300D, "  Number of objects mapped : ",*pMappingCount );
				status = state8;
				//break;
			}
			else 
			{
				MSG_ERR(0x100E, "  Failure while trying to get map count : 0X1A00 + ", pdoNum);
				return 0xFF;
			}
		}
		//case state8:	/* get mapping parameters */
		if (status == state8 )
		{
			objDict = getODentry( (UNS16)0x1A00 + pdoNum, prp_j + (UNS8)1,
					(void * *)&pMappingParameter, pSize, 0 );
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x300F, "  got mapping parameter : ", *pMappingParameter);
				MSG_WAR(0x3050, "    at index : ", 0x1A00 + pdoNum);
				MSG_WAR(0x3051, "    sub-index : ", prp_j + 1);
				status = state9;
				//break;
			}
			else 
			{
				MSG_ERR(0x1010, "  Couldn't get mapping parameter for subindex : ", prp_j + (UNS8)1);
				return 0xFF;
			}
		}
		//case state9:	/* get data to transmit */ 
		if (status == state9 )
		{
			index = (UNS16)((*pMappingParameter) >> 16);
			subIndex = (UNS8)(( (*pMappingParameter) >> (UNS8)8 ) & (UNS32)0x000000FF);
			// <<3 because in *pMappingParameter the size is in bits
			sizeData = (UNS8) ((*pMappingParameter & (UNS32)0x000000FF) >> 3) ;
			objDict = getODentry(index, subIndex, (void * *)&pMappedAppObject, pSize, 0 ); 
			if( objDict == OD_SUCCESSFUL )
			{
				MSG_WAR(0x3011, "  Mapped data found size bytes : ", *pSize);
				MSG_WAR(0x3012, "    at index : ", index);
				MSG_WAR(0x3013, "    sub-index : ", subIndex);
				if (sizeData != *pSize) 
				{
					MSG_WAR(0x2052, "  Size of data different than (size in mapping / 8) : ", sizeData);
				}
				memcpy(&process_var.data[offset], pMappedAppObject, sizeData);
#       ifdef CANOPEN_BIG_ENDIAN
				{
					// data must be transmited with low byte first
					UNS8 pivot, i;
					for ( i = 0 ; i < ( sizeData >> 1)  ; i++) 
					{
						pivot = process_var.data[offset + (sizeData - 1) - i];
						process_var.data[offset + (sizeData - 1) - i] = process_var.data[offset + i];
						process_var.data[offset + i] = pivot ;
					} // end for
				}
#       endif
	
				offset += sizeData ;
				process_var.count = offset;
				prp_j++;
				status = state10;	 
				//break;					
			} // end if
			else 
			{
				MSG_ERR(0x1013, " Couldn't find mapped variable at index-subindex-size : ", (UNS16)(*pMappingParameter));
				return 0xFF;
			}
		}
        //case state10:	/* loop to get all the data to transmit */
        if (status == state10 )
		{
			if( prp_j < *pMappingCount )
			{
				MSG_WAR(0x3014, "  next variable mapped : ", prp_j);
				status = state8;
				//break;
			}
			else 
			{
				MSG_WAR(0x3015, "  End scan mapped variable", 0);
				process_var.state = TS_DOWNLOAD & (~TS_WAIT_SERVER);
				PDOmGR( bus_id, *pwCobId );	
				MSG_WAR(0x3016, "  End of this pdo. Should have been sent", 0);
				pdoNum++;
				offset = 0x00;
				prp_j = 0x00;
				status = state3;
				//break;
			}
		}
		//case state11:     
		//if (status == state11 )
		//{
		//	MSG_WAR(0x3017, "next pdo index : ", pdoNum);
		//	status = state3;
			//break;
		//}
        //default:
		//	MSG_ERR(0x1019,"Unknown state has been reached : %d",status);
		//	return 0xFF;
		//}// end switch case
    }// end while( prp_i<dict_cstes.max_count_of_PDO_transmit )
    return 0;
}
