/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : sdo.c
 *                                                       *
 *********************************************************/
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON


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
#include "sdo.h"



/**************extern variables declaration*********************************/
extern e_nodeState  nodeState;	    	/* slave's state       */
extern UNS8         bDeviceNodeId;	    /* NodeId             */
/********************variables declaration*********************************/
// buffer used by SDO
s_transfer transfers[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];	

// those following arrays store the node_id from which a 
// SDO frame response is waited for
WriteDic 	writeNetworkDictWaited[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];
ReadDic 	readNetworkDictWaited[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID];
s_SDOabort 	SDOabort[MAX_CAN_BUS_ID][SDO_MAX_NODE_ID]; // Contains information of abort reasons.
/***************************************************************************/
void resetSDOline( UNS8 bus_id, UNS8 num )
{
	UNS8 i; 

	transfers[bus_id][num].nodeId = 0;
	transfers[bus_id][num].state =0;
	transfers[bus_id][num].index = 0;
	transfers[bus_id][num].subindex = 0;
	transfers[bus_id][num].count = 0;
	transfers[bus_id][num].offset = 0;
	for (i = 0 ; i < SDO_MAX_DOMAIN_LEN ; i++)
		transfers[bus_id][num].data[i] = 0;
}
/***************************************************************************/
UNS8 getSDOfreeLine( UNS8 bus_id, UNS8 *line )
{
	UNS8 i;
  
	for (i = 0 ; i < SDO_MAX_NODE_ID ; i++)
	{
		if ( transfers[bus_id][i].nodeId == 0 ) 
		{
		*line = i;
		return 0;
		} // end if
	} // end for
	return 0xFF;
}
/***************************************************************************/
UNS8 getSDOlineOnUse( UNS8 bus_id, UNS8 nodeId, UNS8 *line )
{
	UNS8 i;
    
	for (i = 0 ; i < SDO_MAX_NODE_ID ; i++)
	{
		if ( transfers[bus_id][i].nodeId == nodeId ) 
		{
			*line = i;
			return 0;
		}
	} 
	return 0xFF;
}
/***************************************************************************/
UNS8 sendSDO(UNS8 bus_id, s_SDO sdo)
{
	UNS32 objDict;

	if( (nodeState == Operational) ||  (nodeState == Pre_operational ))  
	{
		Message m;
		UNS8 i;
		UNS32 * pwCobId = NULL;
		UNS8 * pwNodeId = NULL;
		UNS8 *    pSize;
		UNS8      size;	
		pSize = &size;	
		MSG_WAR(0x3A00, "sendSDO",0);			

		/*get the communication CobId*/
		if ( sdo.nodeId == bDeviceNodeId )	/*case server*/
			getODentry( 0x1200, (UNS8)2, (void * *)&pwCobId, pSize, 0);
		else
		{			/*case client*/
			UNS16 sdoNum = 0;
			for( ; sdoNum < dict_cstes.max_count_of_client_SDO; sdoNum++)
			{
				MSG_WAR(0x3A52,"Reading index : ", 0x1280 + sdoNum);
				objDict = getODentry( (UNS16)0x1280 + sdoNum, (UNS8)3, (void * *)&pwNodeId, pSize, 0);
				MSG_WAR(0x3A53, "Read in dict, nodeId = ", *pwNodeId);
				if (objDict == OD_SUCCESSFUL)
				{
					if(*pwNodeId == sdo.nodeId)
					{
						break;
					}
				}
				else 
				{
					MSG_ERR(0x1A50, "Erreur dans les SDO client, subindex 2, index : ", (UNS16)1280 + sdoNum);
					return 0xFF;
				}
			}
			if (sdoNum == dict_cstes.max_count_of_client_SDO)
			{
				MSG_WAR (0x2A51, "No SDO client corresponds to the mesage sent to node ", sdo.nodeId);
				return 0xFF;
			}
			getODentry( (UNS16)0x1280 + sdoNum, (UNS8)1, (void * *)&pwCobId, pSize, 0);
		}
		/* message copy for sending */
		
		m.cob_id = *pwCobId;
		m.rtr = DONNEES; 
		
		//the length of SDO must be 8
		
		m.len = 8;
		
		//memcpy(&m.data, &sdo.body, m.len);
		// This Memcpy depends on packing structure. Avoid
		
		if (sdo.len > 0)
			m.data[0] = sdo.body.SCS;
		for (i = 1 ; i < sdo.len ; i++) 
		{
			m.data[i] =  sdo.body.data[i - 1];
		}
		for (i = sdo.len ; i < 8; i++)
			m.data[i]=0;
		return f_can_send(bus_id,&m);
	}
	return 0xFF;
}
/***************************************************************************/
UNS8 sendSDOabort(UNS8 bus_id, UNS16 index, UNS8 subIndex, UNS32 abortCode)
{
	s_SDO sdo;
	UNS8 ret;
	MSG_WAR(0x2A5F,"Sending SDO abort", abortCode);
	sdo.len = 8;
	sdo.body.SCS = 0x80;
	sdo.nodeId = bDeviceNodeId;
	// Index
	sdo.body.data[0] = index & 0xFF; // LSB
	sdo.body.data[1] = (index >> 8) & 0xFF; // MSB
	// Subindex
	sdo.body.data[2] = subIndex;
	// Data
	sdo.body.data[3] = (UNS8)(abortCode & 0xFF);
	sdo.body.data[4] = (UNS8)((abortCode >> 8) & 0xFF);
	sdo.body.data[5] = (UNS8)((abortCode >> 16) & 0xFF);
	sdo.body.data[6] = (UNS8)((abortCode >> 24) & 0xFF);
	ret = sendSDO(bus_id, sdo);
	return ret;
}
/**************************************************************************/
UNS8 SDOmGR(UNS8 bus_id, UNS8 line) //Flux Manager
{
	UNS8 res;
	UNS8 i;
	UNS8 line_transfers;	
	UNS8 t, n, c, e, s;
	s_SDO sdo;

	MSG_WAR(0x3A11, "SDOmGR ", 0);
	res = 0xFF;
	sdo.nodeId = transfers[bus_id][line].nodeId ;

	if(TS_HAVE_TO_DO(transfers[bus_id][line].state)) 
	{
        switch(TS_ACTIVITY(transfers[bus_id][line].state)) 
		{   
			// Initiate a Domain (up/down)load ( first frame )   
		case TS_ACTIVATED:	
			MSG_WAR(0x3A12, "Initiate Domain ", 0);
   
			//memcpy(&sdo.body.data[0], &transfers[bus_id][line].index, 2);
			// This Memcpy depends on packing structure. Avoid
			
			sdo.body.data[0] = transfers[bus_id][line].index & 0xFF;        // LSB
			sdo.body.data[1] = (transfers[bus_id][line].index >> 8) & 0xFF; // MSB of index (16 b)
			sdo.body.data[2] = transfers[bus_id][line].subindex;
      
			if(TS_IS_DOWNLOAD(transfers[bus_id][line].state))
			{
				// Number of bytes to transfer < 5 -> expedited tranfer
				MSG_WAR(0x3A13, "Download ", 0);
				if(transfers[bus_id][line].count < 5)
				{
					n = 4 - transfers[bus_id][line].count;
					e = 1;
					s = 1;
					sdo.len = 8 - n;
					for(i=0; i<4-n; i++)
						sdo.body.data[i+3] = transfers[bus_id][line].data[i];
					// Next call will finish.
					transfers[bus_id][line].offset = transfers[bus_id][line].count; 
				} else 
				{ // Normal transfer
					n = 0;
					e = 0;
					s = 1;
					//the first byte of D containts the LSB of number of data to be 
					//download and the last byte of D contains the MSB
					sdo.body.data[3] = transfers[bus_id][line].count;	
					sdo.body.data[6] = 0;
					sdo.body.data[4] = 0;
					sdo.body.data[5] = 0;
					sdo.len = 8;
					transfers[bus_id][line].offset = 0;
				}
				sdo.body.SCS = IDD_client(n,e,s);
			} else 
			{	// initiate upload, expedited transfer
				MSG_WAR(0x3A14, "Upload ", 0);
				sdo.len = 4;
				sdo.body.SCS = IDU_client;
				transfers[bus_id][line].offset = 0;
			}
			res = sendSDO(bus_id, sdo);
			TS_SET_ACTIVITY(transfers[bus_id][line].state,TS_WORKING | TS_WAIT_SERVER);
			break;
			// Follow Domain (up/down)load 
			// ( following frames, if more that one is needed )	
		case TS_WORKING:	
			MSG_WAR(0x3A15, "Domain Segment ", 0);
      
			getSDOlineOnUse( bus_id,transfers[bus_id][line].nodeId, &line_transfers );
			line = line_transfers;

			if(TS_IS_DOWNLOAD(transfers[bus_id][line].state)) 
			{		       
				MSG_WAR(0x3A16, "Download ", 0);
				i = transfers[bus_id][line].count - transfers[bus_id][line].offset;    
        
				if(i <= 0)
				{ // Download Finished
					res = 0;
					transfers[bus_id][line].state = TS_FREE;
					resetSDOline( bus_id, line );
					break;					
				} 
				else 
				{ // Follow segmented transfer
					if(i>7) 
					{
						n = 0;		// There is no unused byte
						c = 0;		// It is not the last message		
					} 
					else 
					{
						n = 7 - i;	        // There could have unused bytes
						c = 1;	        // This is the last message
					}
					sdo.len = 8 - n;
					for(i=0; i<7-n; i++)
						sdo.body.data[i] =transfers[bus_id][line].data[transfers[bus_id][line].offset++];	
					// take the toggle bit	
					t = (transfers[bus_id][line].state & TS_TOGGLE) >> 4;    
					sdo.body.SCS = DDS_client(t,n,c);
					// toggle afterward
					transfers[bus_id][line].state ^= TS_TOGGLE;   
					if (c)
						resetSDOline( bus_id, line );
				}				
			} 
			else 
			{    // Upload			
				MSG_WAR(0x3A17, "Upload ", 0);
				
				//memcpy(&sdo.body.data[0], &transfers[bus_id][line].index, 2);
				// This Memcpy depends on packing structure. Avoid
        
				sdo.body.data[0] = transfers[bus_id][line].index & 0xFF;        // LSB
				sdo.body.data[1] = (transfers[bus_id][line].index >> 8) & 0xFF; // MSB of index (16 b)
				sdo.body.data[2] = transfers[bus_id][line].subindex;		
				sdo.len = 4;
				// take toggle bit
				t = (transfers[bus_id][line].state & TS_TOGGLE) >> 4;
				sdo.body.SCS = UDS_client(t);
				// toggle afterward
				transfers[bus_id][line].state ^= TS_TOGGLE;
			}  
      
			res = sendSDO(bus_id, sdo);
			TS_SET_ACTIVITY(transfers[bus_id][line].state,TS_WORKING | TS_WAIT_SERVER);
			break;
		default:	// Transfer not in use or transfer error. Blub blub...  
			resetSDOline( bus_id, line );
		break;
		}
	}
	return res;
}
/***************************************************************************/
UNS8 proceedSDO(UNS8 bus_id, Message *m)
{
	UNS8 res;
	UNS8 err;
	UNS8 line;
	UNS8 i,j;
	UNS8 	t,  /* toggle bit */
			n, /* if e=1 and s=1, indicate the bytes number without data, else n=0 */ 
			c, /* c=1 if last segment, else 0 */
			e, /* e=0 normal transfer, e=1 expedited transfer */
			s; /* size indicator */
	UNS32 odError; // Error while reading or writing in object dictionary
	s_SDO sdo;
	UNS32 *     pwServerCobId = NULL;
	UNS32 *     pwClientCobId = NULL;
	UNS8 *      pSize;
	UNS8        size;
	UNS32 objDict;

	UNS8 * p;	

	// Data which were not initialised. (Bogue) This values must be confirmed. (FD)
	c = 0;
	s = 0;

	pSize = &size; 
	MSG_WAR(0x3A18, "proceedSDO ", 0);	

	if((nodeState == Operational) || (nodeState == Pre_operational))
	{		
		sdo.nodeId = (UNS8)(GET_NODE_ID((*m)));
		MSG_WAR(0x3A19, "Received SDO for nodeId : ",sdo.nodeId);	
		sdo.len = (*m).len;
		//memcpy(&sdo.body, &m->data, sdo.len);
		if (sdo.len > 0)
			sdo.body.SCS = m->data[0]; //SDO command specifier
		for (i = 1 ; i < sdo.len ; i++)
			sdo.body.data[i - 1] = m->data[i];
		
		err = getSDOlineOnUse( bus_id, sdo.nodeId, &line );
		if(err)
			err = getSDOfreeLine( bus_id, &line );

		/* *case client. The client is a requester. It is expected an SDO from a node   	*/
		/* 2 cases :                                                                    							*/
		/* 1 - SDO Download                                                             					*/
		/* we compute here the SDO request received from the node whose                 	*/
		/* dictionnary have been written.                                               					*/
		/* 2 - SDO Upload                                                               						*/
		/* We compute here the SDO received from a node whose dictionnary have been   */
		/* read. Assume that before, a SDO request have been sent to this node          	*/
		/* If there really is a running transfer and we are waiting for a               			*/
		/*   message and the message is not Abort                                       				*/	
		if(TS_ACTIVITY(transfers[bus_id][line].state) == TS_WORKING && !TS_HAVE_TO_DO(transfers[bus_id][line].state) 
			&& (*m).rtr == DONNEES) 
		{
			//!(sdo.body.SCS & ADT_server) && (*m).rtr == DONNEES) {
			MSG_WAR(0x3A1F, "I am a client (ie a requester) ", 0);
			//for all clients SDO
			for( j=(UNS8)0x00; j < dict_cstes.max_count_of_client_SDO; j++ ) 
			{  // check whether the received COBs are interesting for us:*/
				objDict = getODentry( (UNS16)0x1280 + j, (UNS8)2,(void * *)&pwClientCobId, pSize, 0);
				if (objDict != OD_SUCCESSFUL) 
				{
					MSG_ERR(0x1A20, "Error reading dictionnary subIndex 2, index : ", 0x1280 + j);
				}
				if( (objDict == OD_SUCCESSFUL)  && (*pwClientCobId == (*m).cob_id) ) 
				{
					/* 1 - The SDO received is the request form the server, after having downloaded data to
					its dictionary.*/
					MSG_WAR(0x3A10, "sdo.body.SCS = ", sdo.body.SCS);
					if (IS_ABORTED_REQUEST(sdo.body.SCS)) 
					{
						SDOabort[bus_id][sdo.nodeId].index = transfers[bus_id][line].index;
						SDOabort[bus_id][sdo.nodeId].subIndex = transfers[bus_id][line].subindex;
						SDOabort[bus_id][sdo.nodeId].abortCode = sdo.body.data[3]  | (sdo.body.data[4] << 8) |
						(sdo.body.data[5] << 16) | (sdo.body.data[6] << 24);

						if(TS_IS_DOWNLOAD(transfers[bus_id][line].state)) 
						{
							MSG_WAR(0x2A21, "Transfert DOWNLOAD aborted ", SDO_ABORTED);
							writeNetworkDictWaited[bus_id][sdo.nodeId].state = SDO_ABORTED;
						}
						else 
						{
							MSG_WAR(0x2A20, "Transfert UPLOAD aborted ", SDO_ABORTED);
							readNetworkDictWaited[bus_id][sdo.nodeId].state = SDO_ABORTED;
						}
						// Free line because aborted transfert
						resetSDOline( bus_id, line );
						return SDO_ABORTED;
					}
					if(IS_DOWNLOAD_RESPONSE(sdo.body.SCS)) 
					{ 
						// FIXME : Should warn for strange & unexpected paquets...
						/* if the transfer is finished */
						if(transfers[bus_id][line].count <= transfers[bus_id][line].offset)
						{
							if( writeNetworkDictWaited[bus_id][sdo.nodeId].state == SDO_IN_PROGRESS )
							{
								writeNetworkDictWaited[bus_id][sdo.nodeId].state = SDO_SUCCESS;
								res = TS_IS_ON(transfers[bus_id][line].state);
							}
							//}
							transfers[bus_id][line].state = TS_FREE;
						}
					}//end if(IS_DOWNLOAD_RESPONSE(sdo.body.SCS))
					else 
					{ // Upload			  
						/*upload response (reading response)*/
						if(IS_SEGMENTED_RESPONSE(sdo.body.SCS))
						{ 
							//segment containing data to store.
							
							MSG_WAR(0x3A21, "upload response segmented(UDS) ", 0);
							UDS_server_params(sdo.body.SCS,t,n,c)
							
							// FIXME : should warn for bad toggle bit
							//FIXME : should take care of data overrun
							for(i=0; i<7-n; i++) 
							{
								transfers[bus_id][line].
								data[(int)(transfers[bus_id][line].offset++)] = sdo.body.data[i];
							}
							if(c)
							{	// If last segment get in finished state.
								transfers[bus_id][line].state = TS_FINISHED;
								if (readNetworkDictWaited[bus_id][sdo.nodeId].state == SDO_IN_PROGRESS)
								{
									readNetworkDictWaited[bus_id][sdo.nodeId].state = SDO_SUCCESS;
		    
									//memcpy(readNetworkDictWaited[bus_id][sdo.nodeId].data,
									//&transfers[bus_id][line].data[0],
									//transfers[bus_id][line].offset);
									// En fait, ce memcpy devrait être portable.
		    
									for (i = 0 ; i < transfers[bus_id][line].offset ; i++)
										readNetworkDictWaited[bus_id][sdo.nodeId].data[i]=
											transfers[bus_id][line].data[i];
									readNetworkDictWaited[bus_id][sdo.nodeId].size =
										transfers[bus_id][line].offset;
								}
								res = TS_IS_ON(transfers[bus_id][line].state);
							}//end if(c)
						}//end if(IS_SEGMENTED_RESPONSE(sdo.body.SCS))		
						else 
						{	//Initiate Upload Response
							MSG_WAR(0x3A22, "upload response initiate(IDU) ", 0);
							IDU_server_params(sdo.body.SCS,n,e,s)
							// FIXME : should warn for crazy dictionary (sub)index
							if(e)
							{	    //expedited transfer
								if(!s) n = 0;  //"data bytes contain data to be downloaded"
									for(i=0; i<4-n; i++)
										transfers[bus_id][line].data[i] =sdo.body.data[i+3];
								transfers[bus_id][line].count = 4-n;
								transfers[bus_id][line].state = TS_FINISHED;
		    
								if( readNetworkDictWaited[bus_id][sdo.nodeId].state == SDO_IN_PROGRESS) 
								{
									readNetworkDictWaited[bus_id][sdo.nodeId].state = SDO_SUCCESS;
		    
									//memcpy(
									//&readNetworkDictWaited[bus_id][sdo.nodeId].data,
									//&transfers[bus_id][line].data[0], 
									//transfers[bus_id][line].count);
									// En fait, ce memcpy devrait être portable.
									for (i = 0 ; i < transfers[bus_id][line].count ; i++)
										readNetworkDictWaited[bus_id][sdo.nodeId].data[i] = 
											transfers[bus_id][line].data[i];
									readNetworkDictWaited[bus_id][sdo.nodeId].size 
										=transfers[bus_id][line].count;
								}
								res = TS_IS_ON(transfers[bus_id][line].state);
							}//end if(e) 
							else 
							{		//normal transfer
								if(s)
								{
									transfers[bus_id][line].count =sdo.body.data[3];
									transfers[bus_id][line].offset = 0;
								}// end if(s)
								else
								{ // FIXME : e=0 et s=0   ?
									transfers[bus_id][line].offset = 0;
								}// end if(!s)
							}// end normal transfer
						}// end Initial Upload Response
					}// end UPLOAD
	   
					// Stop waiting for this node after each received message.
					transfers[bus_id][line].state &= ~TS_WAIT_SERVER;
				
					SDOmGR(bus_id,line);
				}// end objDict
			}// end for
		}// end client
		
		/*case server*/
		/* In most of case, the server is a slave, whose dictionnary is configured by the */
		/* master                                                                                 */
		/*If there is a unexpected message, i.e an SDO request
		and the message is not Abort*/
		
		else if(!(sdo.body.SCS & ADT_client) && sdo.nodeId == bDeviceNodeId ) 
		{
			MSG_WAR(0x3A23, "I am serveur (make change in my dictionary or send data on request)", 0);
			objDict = getODentry( (UNS16)0x1200, (UNS8)1,(void * *)&pwServerCobId, pSize, 0 );
			if (objDict != OD_SUCCESSFUL) 
			{
				MSG_ERR(0x1A24, "Unable to read SDO Server param. at index 0x1200, subindex 1", 0);
			}
			if( (objDict == OD_SUCCESSFUL) && (*pwServerCobId == (*m).cob_id) ) 
			{      		
				if(IS_UPLOAD_REQUEST(sdo.body.SCS)) 
				{	/*the client must send data*/ 
					if(IS_SEGMENTED_REQUEST(sdo.body.SCS)) 
					{
						MSG_WAR(0x3A24, "upload request segment(UDS) ", 0);
						
						//getSDOlineOnUse( bus_id, sdo.ID, &line );
						/*get the request frame parameter t récupération*/
						
						UDS_cli_params(sdo.body.SCS,t);			    
						
						// Compute the nb of bytes to transfer	
						
						i = transfers[bus_id][line].count - transfers[bus_id][line].offset;
						if ( i <= 0 ) 
						{ // End of transfert.
							MSG_WAR(0x3A25, "Transfert finished ", 0);
							resetSDOline( bus_id, line );
						}
						else 
						{	
							MSG_WAR(0x3A26, "Nb byte of data to transmit : ", i);
							if (i > 7 ) 
							{	// several trames are needed to be transfered
								n=0;
								c=0;   			
							} 
							else 
							{	//it's the last trame to be transfered	 
								n = 7 - i;
								c=1;
							}
							for(i=0; i<7-n ;i++)
								sdo.body.data[i] = 
									transfers[bus_id][line].data[transfers[bus_id][line].offset++];
							sdo.body.SCS = UDS_serv(t,n,c);			
							sdo.len = 8-n;
	    
							/* transfers[bus_id][line].state=  ; */
							
							sendSDO(bus_id, sdo); 
							//   if (c)
							//	resetSDOline( bus_id, line );
						}
					}//end if(IS_SEGMENTED_REQUEST(sdo.body.SCS))  
					else 
					{
						MSG_WAR(0x3A27, "initiate upload request(IDU) ", 0);
						getSDOfreeLine( bus_id, &line );
						
						/* get the data size and the data which will be transfered
						in the transfer array */
						//memcpy(&transfers[bus_id][line].index, &sdo.body.data[0], 2);
						// This Memcpy depends on packing structure. Avoid
	
						transfers[bus_id][line].index =  
							((sdo.body.data[1] & 0xFF) << 8) | // MSB of index, which is on 16 b
							(sdo.body.data[0] & 0xFF); // LSB
						transfers[bus_id][line].subindex=sdo.body.data[2];
						transfers[bus_id][line].count=0;
						transfers[bus_id][line].offset=0;

						p = NULL;	
						odError = getODentry( transfers[bus_id][line].index, 
							transfers[bus_id][line].subindex,  
							(void * *) &p, 
							(UNS8 *) &transfers[bus_id][line].count, 
							0 );
						if (odError != OD_SUCCESSFUL) 
						{
							sendSDOabort(bus_id, 
								(UNS16)transfers[bus_id][line].index, 
								(UNS8) transfers[bus_id][line].subindex,
								odError);
							resetSDOline (bus_id, line);
							return 0xFF;
						}
						for(i=0;i<transfers[bus_id][line].count;i++)
							transfers[bus_id][line].data[i] = *(p+i);

						MSG_WAR(0x3A28, "Reading in dict and send value. index : ", transfers[bus_id][line].index);
						MSG_WAR(0x3A29, "                             subIndex : ", transfers[bus_id][line].subindex);
						MSG_WAR(0x3A29, "                             size : ", transfers[bus_id][line].count);

#   ifdef DEBUG_WAR_CONSOLE_ON
						switch ( transfers[bus_id][line].count)
						{
						case 4 :
							MSG_WAR(0x3A29, "              data : ", *(UNS32 *)transfers[bus_id][line].data);
						break ;
						case 2 :
							MSG_WAR(0x3A29, "              data : ", *(UNS16 *)transfers[bus_id][line].data);
							break ;
						default :          
							MSG_WAR(0x3A29, "              data : ", *transfers[bus_id][line].data);
							break ;
						}
#   endif

#              ifdef CANOPEN_BIG_ENDIAN
						{
							UNS8 pivot;
							UNS8 i;
							UNS8 count = transfers[bus_id][line].count;
							for (i = 0 ; i < (transfers[bus_id][line].count >> 1); i++) 
							{
								pivot = transfers[bus_id][line].data[count -i -1];
								transfers[bus_id][line].data[count -i -1] =
									transfers[bus_id][line].data[i];
								transfers[bus_id][line].data[i] = pivot;
							}
						}
#             endif   
						/* if data size < 5, data size is indicated in the frame,
						transfer is expedited */ 
						if ( transfers[bus_id][line].count < 5 ) 
						{	
							n = 4 - transfers[bus_id][line].count;
							e = 1;
							s = 1;
							//memcpy(&sdo.body.data[0], &transfers[bus_id][line].index,2);
							// This Memcpy depends on packing structure. Avoid
							sdo.body.data[0] = transfers[bus_id][line].index & 0xFF;        // LSB
							sdo.body.data[1] = (transfers[bus_id][line].index >> 8) & 0xFF; // MSB 
							sdo.body.data[2] = transfers[bus_id][line].subindex;
							for(i=0; i<4-n ;i++)
								sdo.body.data[i+3] = transfers[bus_id][line].data[i]; 
						}// end if ( transfers[bus_id][line].count < 5 )
						else 
						{
							n = 0;
							e = 0;
							s = 1;
							//the first byte of D containts the LSB of number of data to be 
							//download and the last byte of D contains the MSB
							sdo.body.data[3] = transfers[bus_id][line].count;	
							sdo.body.data[6] = 0;
							sdo.body.data[4] = 0;
							sdo.body.data[5] = 0;
						}
						sdo.body.SCS = IDU_serv(n,e,s);	
						sdo.len = 8;
						transfers[bus_id][line].offset = 0;
						sendSDO(bus_id, sdo);
					}// end initiate upload request(IDU)
				}// end if(IS_UPLOAD_REQUEST(sdo.body.SCS))
				else 
				{	// download request	
					if(IS_SEGMENTED_REQUEST(sdo.body.SCS)) 
					{ 
						MSG_WAR(0x3A2A, "download request segment(DDS) ", 0);
						getSDOlineOnUse(  bus_id, sdo.nodeId, &line );
						DDS_cli_params(sdo.body.SCS,t,n,c); 
						for(i=0;i<7-n;i++)
							transfers[bus_id][line].
						data[transfers[bus_id][line].offset++] =sdo.body.data[i];

						if ( c ) 
						{	/* last trame, the objects dictionnary is updated */
							MSG_WAR(0x3A2B,"Last segment ", 0);
							transfers[bus_id][line].count =transfers[bus_id][line].offset;
							transfers[bus_id][line].offset = 0;

							MSG_WAR(0x3A2C, "Writing in dict. index : ", transfers[bus_id][line].index);
							MSG_WAR(0x3A2D, "              subIndex : ", transfers[bus_id][line].subindex);

#             ifdef CANOPEN_BIG_ENDIAN
							{
								UNS8 pivot;
								UNS8 i;
								UNS8 count = transfers[bus_id][line].count;
								for (i = 0 ; i < (transfers[bus_id][line].count >> 1); i++) 
								{
									pivot = transfers[bus_id][line].data[count -i -1];
									transfers[bus_id][line].data[count -i -1] =transfers[bus_id][line].data[i];
									transfers[bus_id][line].data[i] = pivot;
								}
							}
#             endif

							odError = setODentry((UNS16)transfers[bus_id][line].index,
								(UNS8) transfers[bus_id][line].subindex, 
								(void *)&transfers[bus_id][line].data[0],
								(UNS8)transfers[bus_id][line].count,
								1);
							if (odError != OD_SUCCESSFUL) 
							{
								sendSDOabort(bus_id, 
									(UNS16)transfers[bus_id][line].index, 
									(UNS8) transfers[bus_id][line].subindex,
									odError);
								resetSDOline (bus_id, line);
								return 0xFF;
							}
						}
						sdo.body.SCS = DDS_serv(t);
					}//end if(IS_SEGMENTED_REQUEST(sdo.body.SCS)) 
					else 
					{
						MSG_WAR(0x3A2E, "initiate download request(IDD) ", 0);
						IDD_cli_params(sdo.body.SCS,n,e,s);
						transfers[bus_id][line].nodeId = sdo.nodeId;   
						//memcpy(&transfers[bus_id][line].index, &sdo.body.data[0], 2);
						// This Memcpy depends on packing structure. Avoid
						transfers[bus_id][line].index =  
							((sdo.body.data[1] & 0xFF) << 8) | // MSB of index, which is on 16 b
							(sdo.body.data[0] & 0xFF); // LSB
						transfers[bus_id][line].subindex = sdo.body.data[2];

						if ( e )
						{	//all data are transfered in this trame
							/*transfer of those data in the transfer array*/
							if(s)
							{
#ifdef CANOPEN_BIG_ENDIAN
								for (i=0; i<4-n ;i++)
									transfers[bus_id][line].data[3-n-i] = sdo.body.data[i+3];
#else 
								for(i=0; i<4-n ;i++)
									transfers[bus_id][line].data[i] =  sdo.body.data[i+3];
#endif

								transfers[bus_id][line].count = 4 - n;
								transfers[bus_id][line].offset = 0;

								odError = setODentry((UNS16)transfers[bus_id][line].index,
										(UNS8) transfers[bus_id][line].subindex,
										(void *) &transfers[bus_id][line].data[0],
										(UNS8)transfers[bus_id][line].count,
										1);
								if (odError != OD_SUCCESSFUL) 
								{
									sendSDOabort(bus_id, 
										(UNS16)transfers[bus_id][line].index, 
										(UNS8) transfers[bus_id][line].subindex,
										odError);
									resetSDOline (bus_id, line);
									return 0xFF;
								}
								
								MSG_WAR(0x3A2F, "Writing in dict. index : ", transfers[bus_id][line].index);
								MSG_WAR(0x3A30, "              subIndex : ", transfers[bus_id][line].subindex);
#ifdef DEBUG_WAR_CONSOLE_ON
								switch ( transfers[bus_id][line].count)
								{
									case 4 :
										MSG_WAR_long(0x3A30, "                  data : ", *(UNS32 *)transfers[bus_id][line].data);
									//	MSG_WAR(0x3A30, "                  data : ", *(UNS32 *)transfers[bus_id][line].data);
										break ;
									case 2 :
										MSG_WAR(0x3A30, "                  data : ", *(UNS16 *)transfers[bus_id][line].data);
										break ;
									default :          
										MSG_WAR(0x3A30, "                  data : ", *transfers[bus_id][line].data);
										break ;
								}
#endif
							}
							else 
							{
								transfers[bus_id][line].offset = 0;
								transfers[bus_id][line].count = 0;	
							}	
						}
						else
						{
							if(s)
							{
								transfers[bus_id][line].count  = sdo.body.data[3];
								transfers[bus_id][line].offset = 0;
							}
							else
							{
								transfers[bus_id][line].offset = 0;
								transfers[bus_id][line].count = 0;
							}
						}
						sdo.body.SCS = IDD_server;
					}// end initiate download request(IDD)
					/* response frame transmission */
					
					sdo.len = 4;
	  
					//memcpy(&sdo.body.data[0], &transfers[bus_id][line].index, 2);
					// This Memcpy depends on packing structure. Avoid
	  
					sdo.body.data[0] = transfers[bus_id][line].index & 0xFF;        // LSB
					sdo.body.data[1] = (transfers[bus_id][line].index >> 8) & 0xFF; // MSB 
					sdo.body.data[2] = transfers[bus_id][line].subindex;	      
					/* transfers[bus_id][(int)sdo.ID].state = ~TS_WAIT_SERVER; */
					sendSDO(bus_id, sdo);
	
					if ( e || c )
						resetSDOline( bus_id, line );
				}// end download request
			}// end objDict
			else 
			{
				MSG_ERR(0x1A31,"COBID client->server defined at index 1200, subindex 1 != than received : ", (*m).cob_id)
			}
		}// end case server
		else // FIXME : should warn for unexpected message
		{	
			MSG_WAR(0x1A31, "Error state ",0);
			transfers[bus_id][line].state = TS_ERROR;	
		}
		return 0;
	}/* end if( (nodeState == Operational) ||
		(nodeState == Pre_operational) ) */
	return 0xFF;
}
/*******************************************************************)******/
UNS8 writeNetworkDict(UNS8 bus_id, UNS8 nodeId, UNS16 index, 
		     UNS8 subindex, UNS8 count, void *data)
{
	UNS8 res;
	UNS8 line;
	UNS8 i, j;
	UNS32*    pwClientCobId = NULL;
	UNS8 *    pSize;	
	UNS8      size;	
	UNS32     objDict;
	UNS8 SDOFound = 0;
	pSize = &size;

	//for(i=0;i<SDO_MAX_DOMAIN_LEN;i++)
	//transfers[bus_id][ID].data[i] = 0;

	MSG_WAR(0x3A36, "Send SDO to write in the dictionary of node : ", nodeId);
	MSG_WAR(0x3A37, "                                   At index : ", index);
	MSG_WAR(0x3A38, "                                   subIndex : ", subindex);
	MSG_WAR(0x3A39, "                                   nb bytes : ", count);
	
	res = getSDOlineOnUse(bus_id, nodeId, &line);
	if (res) 
	{
		res = getSDOfreeLine( bus_id, &line );
		if (res) 
		{
			MSG_ERR(0x1A50, "Full SDO buffer. Unable to send SDO to node : ", nodeId); 
			return (0xFF);
		}
	}
	else 
	{
		MSG_WAR (0x3A40, "This is aborting the last SDO transfert", 0);
		resetSDOline(bus_id, line );
	}
	// for every client SDO
	// Check which SDO to use to communicate with the node
	for( i = 0x00; i < dict_cstes.max_count_of_client_SDO; i++ ) 
	{  // check whether the received COBs (server -> client are interesting for us:*/
		objDict = getODentry( (UNS16)0x1280 + i, (UNS8)1,(void * *)&pwClientCobId, pSize,0);
		if( objDict == OD_SUCCESSFUL && ((*pwClientCobId & 0x7f) == nodeId)) 
		{
			SDOFound = 1;
			MSG_WAR(0x3A49,"Sending SDO to write dict with CobId : ", *pwClientCobId);
			MSG_WAR(0x3A50,"        SDO client defined at index  : ", 0x1280 + i);
			transfers[bus_id][line].nodeId    = nodeId;
			transfers[bus_id][line].index     = index;
			transfers[bus_id][line].subindex  = subindex;
			transfers[bus_id][line].count     = count;
			// Copie de tableau de char vers tableau de char. Ce memcpy est portable
			//memcpy(&transfers[bus_id][line].data, data, count);
			
			for (j = 0 ; j < count ; j++) 
			{
#         ifdef CANOPEN_BIG_ENDIAN
				transfers[bus_id][line].data[count - 1 - j] = ((char *)data)[j];
#         else 
				transfers[bus_id][line].data[j] = ((char *)data)[j];
#         endif
			}
			transfers[bus_id][line].state = TS_DOWNLOAD;
			// Send the SDO to the slave, to write in the slave's directory
			writeNetworkDictWaited[bus_id][nodeId].state = SDO_IN_PROGRESS;
			SDOmGR(bus_id, line);

			return 0;
		}// end if
    }// end for
	if (!SDOFound)
		MSG_ERR(0x1A50, "No SDO client found to communicate with node : ", nodeId);
	return 0xFF;
}
/***************************************************************************/
UNS8 readNetworkDict(UNS8 bus_id, UNS8 nodeId, UNS16 index, UNS8 subindex)
{
	UNS8 res;
	UNS8 i, k;
	UNS8 line;
	UNS32 *   pwClientCobId = NULL;
	UNS8 *    pSize;
	UNS8      size;	
	UNS32     objDict;

	pSize = &size;
	MSG_WAR(0x3A3A, "Send SDO to read in the dictionary of node : ", nodeId);
	MSG_WAR(0x3A3B, "                                  At index : ", index);
	MSG_WAR(0x3A3C, "                                  subIndex : ", subindex);

	res = getSDOlineOnUse(bus_id, nodeId, &line);
	if (res) 
	{
		res = getSDOfreeLine( bus_id, &line );
		if (res) 
		{
			MSG_ERR(0x1A3A, "Full SDO buffer. Unable to send SDO to node : ", nodeId); 
			return (0xFF);
		}	
	}
	else 
	{
		MSG_WAR (0x3A3D, "This is aborting the last SDO transfert", 0);
		resetSDOline(bus_id, line );
	}
 
	for ( k=0; k<SDO_MAX_DOMAIN_LEN; k++)
		readNetworkDictWaited[bus_id][nodeId].data[k] = 0;
		
	//for( k=0; k<SDO_MAX_DOMAIN_LEN; k++)
		//  transfers[bus_id][ID].data[k] = 0;
  
	// for every client SDO
	
	for( i=(UNS8)0x00; i<dict_cstes.max_count_of_client_SDO; i++ ) 
	{
		// check whether the received COBs are interesting for us:
		objDict = getODentry( (UNS16)0x1280 + i, (UNS8)1,(void * *)&pwClientCobId, pSize,0);
		if( (objDict == OD_SUCCESSFUL) && ((*pwClientCobId & 0x7f) == nodeId)) 
		{	
			transfers[bus_id][line].nodeId = nodeId;
			transfers[bus_id][line].index = index;
			transfers[bus_id][line].subindex = subindex;
			transfers[bus_id][line].count = 0;
			transfers[bus_id][line].state = TS_UPLOAD;
				
			SDOmGR(bus_id, line);

			readNetworkDictWaited[bus_id][nodeId].state = SDO_IN_PROGRESS;
		    return 0;
		}// end if
	}// end for	
	return 0xFF;
}
/***************************************************************************/
UNS8 getReadResultNetworkDict(UNS8 bus_id, UNS8 nodeId, void* data, UNS8 *size)
{
	UNS8 i;
	* size = 0;

	if (readNetworkDictWaited[bus_id][nodeId].state == SDO_SUCCESS) 
	{ 
		* size = readNetworkDictWaited[bus_id][nodeId].size;
		for  ( i = 0 ; i < *size ; i++) 
		{
#     ifdef CANOPEN_BIG_ENDIAN
			((char *)data)[*size - 1 - i] = readNetworkDictWaited[bus_id][nodeId].data[i];
#     else 
			((char *)data)[i] = readNetworkDictWaited[bus_id][nodeId].data[i];
#     endif
		}
	}
	return readNetworkDictWaited[bus_id][nodeId].state;
}
/***************************************************************************/
UNS8 getWriteResultNetworkDict(UNS8 bus_id, UNS8 node_id)
{
	return (writeNetworkDictWaited[bus_id][node_id].state);
}
/***************************************************************************/
s_SDOabort getSDOerror(UNS8 bus_id, UNS8 nodeId)
{
	MSG_WAR(0x2A71,"Received SDO abort from node : ", nodeId);
	accessDictionaryError(SDOabort[bus_id][nodeId].index, 
			SDOabort[bus_id][nodeId].subIndex, 
			0, 
			0, 
			SDOabort[bus_id][nodeId].abortCode);
	return SDOabort[bus_id][nodeId];
}