/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : objacces.c
 *                                                       *
 *********************************************************/

/* Comment when the code is debugged */
//#define DEBUG_CAN
//#define DEBUG_WAR_CONSOLE_ON
//#define DEBUG_ERR_CONSOLE_ON
/* end Comment when the code is debugged */
#include <string.h>
#include <stdio.h>
#include "objdictdef.h"
#include "objacces.h"

extern __far  const dict_cste dict_cstes;	/*see in objdictdef.h*/



UNS8 accessDictionaryError(UNS16 index, UNS8 subIndex, 
			     UNS8 sizeDataDict, UNS8 sizeDataGiven, UNS32 code)
{
	MSG_WAR(0x2B09,"Dictionary index : ", index);
	MSG_WAR(0X2B10,"        subindex : ", subIndex);
	switch (code) 
	{
		case  OD_NO_SUCH_OBJECT: 
			MSG_WAR(0x2B11,"Index not found ", index);
			break;
		case OD_NO_SUCH_SUBINDEX :
			MSG_WAR(0x2B12,"SubIndex not found ", subIndex);
			break;   
		case OD_WRITE_NOT_ALLOWED :
			MSG_WAR(0x2B13,"Write not allowed, data is read only ", index);
			break;         
		case OD_LENGTH_DATA_INVALID :    
			MSG_WAR(0x2B14,"Conflict size data. Should be (bytes)  : ", sizeDataDict);
			MSG_WAR(0x2B15,"But you have given the size  : ", sizeDataGiven);
		break;
		default :
			MSG_WAR(0x2B16, "Unknown error code : ", code);
	}
	return 0; 
}	
//=======================================================================
UNS32 getODentry( 	UNS16 wIndex,
					UNS8 bSubindex,
					void * * ppbData,
					UNS8 * pdwSize,
					UNS8 checkAccess)
{
	UNS32 errorCode;
	const indextable __far *ptrTable;
	ptrTable = scanOD(wIndex, bSubindex, 0, &errorCode);
	if (errorCode != OD_SUCCESSFUL)
		return errorCode;

	if( ptrTable->bSubCount <= bSubindex ) 
	{
		// Subindex not found
		accessDictionaryError(wIndex, bSubindex, 0, 0, OD_NO_SUCH_SUBINDEX);
		return OD_NO_SUCH_SUBINDEX;
	}
	if (checkAccess && (ptrTable->pSubindex[bSubindex].bAccessType == WO)) 
	{
		accessDictionaryError(wIndex, bSubindex, 0, 0, OD_WRITE_NOT_ALLOWED);
		return OD_READ_NOT_ALLOWED;
	}

	*ppbData = ptrTable->pSubindex[bSubindex].pObject;
	*pdwSize = ptrTable->pSubindex[bSubindex].size;
	return OD_SUCCESSFUL;
}
//=============================================================================
UNS32 setODentry( 	UNS16 wIndex,
					UNS8 bSubindex, 
					void * pbData, 
					UNS8 dwSize, 
					UNS8 checkAccess)
{
	UNS8 szData;
	UNS32 errorCode;
	const indextable __far *ptrTable;
	ptrTable = scanOD(wIndex, bSubindex, dwSize, &errorCode);
	if (errorCode != OD_SUCCESSFUL)
		return errorCode;

	if( ptrTable->bSubCount <= bSubindex ) 
	{
		// Subindex not found
		accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_SUBINDEX);
		return OD_NO_SUCH_SUBINDEX;
	}
	szData = ptrTable->pSubindex[bSubindex].size;
	if (szData != dwSize) 
	{
		accessDictionaryError(wIndex, bSubindex, szData, dwSize, OD_LENGTH_DATA_INVALID);
		return OD_LENGTH_DATA_INVALID;
	}
	if (checkAccess && (ptrTable->pSubindex[bSubindex].bAccessType == RO)) 
	{
		accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_WRITE_NOT_ALLOWED);
		return OD_WRITE_NOT_ALLOWED;
	}
	memcpy(ptrTable->pSubindex[bSubindex].pObject,pbData, dwSize );
	return OD_SUCCESSFUL;
}
//=================================================================================

const indextable __far * scanOD (UNS16 wIndex,
							UNS8 bSubindex, 
							UNS8 dwSize,
							UNS32 * errorCode)
{
	UNS16 offset = 0;
	const indextable __far *ptrTable; 
	// A propos des memcpy, comme la source est toujours un tableau de char 
	// (cf canOpenMain.c), il n'y a pas de pb d'alignement, et ceux-ci devraient
	// être portables. (FD)
	*errorCode = OD_SUCCESSFUL; // Default value.
  
	if( ( wIndex >= (UNS16)0x1400 ) && ( wIndex <= (UNS16)0x15FF ) ) 
	{	 
		/***************************************/
		/* Receive PDO Communication Parameter */
		/***************************************/
		offset = wIndex - (UNS16)0x1400;
		if( (wIndex > dict_cstes.receive_PDO_last) ||
			(receivePDOParameter[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = receivePDOParameter + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x1800 ) && ( wIndex <= (UNS16)0x19FF ) ) 
	{ 
		/****************************************/
		/* Transmit PDO Communication Parameter */
		/****************************************/
		offset = wIndex - (UNS16)0x1800;
		if( (wIndex > dict_cstes.transmit_PDO_last) ||
			(transmitPDOParameter[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = transmitPDOParameter + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x1600 ) && ( wIndex <= (UNS16)0x17FF ) ) 
	{ 
		/****************************************/
		/* Receive mapping Parameters           */
		/****************************************/
		offset = wIndex - (UNS16)0x1600;
		if( (wIndex > dict_cstes.receive_PDO_mapping_last) ||
			(RxPDOMappingTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = RxPDOMappingTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x1A00 ) && ( wIndex <= (UNS16)0x1BFF ) ) 
	{ 
		/****************************************/
		/* Transmit mapping Parameters          */
		/****************************************/
		offset = wIndex - (UNS16)0x1A00;
		if( (wIndex > dict_cstes.transmit_PDO_mapping_last) ||
		(TxPDOMappingTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = TxPDOMappingTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x2000 ) && ( wIndex <= (UNS16)0x5FFF ) ) 
	{	 
		/****************************************/
		/* Manufacturer specific profile        */
		/****************************************/
		offset = wIndex - (UNS16)0x2000;
		if( (wIndex > dict_cstes.manufacturerSpecificLastIndex) ||
		(manufacturerProfileTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = manufacturerProfileTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6000 ) && ( wIndex <= (UNS16)0x61FF ) ) 
	{ 
		/****************************************/
		/* Digital Input specific profile       */
		/****************************************/
		offset = wIndex - (UNS16)0x6000;
		if( (wIndex > dict_cstes.digitalInputTableLastIndex) ||
			(digitalInputTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = digitalInputTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6200 ) && ( wIndex <= (UNS16)0x63FF ) ) 
	{ 
		/****************************************/
		/* Digital Output specific profile      */
		/****************************************/
		offset = wIndex - (UNS16)0x6200;
		if( (wIndex > dict_cstes.digitalOutputTableLastIndex) ||
			(digitalOutputTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = digitalOutputTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6410 ) && ( wIndex <= (UNS16)0x6424 ) ) 
	{ 
		/****************************************/
		/* Analog Output specific profile      */
		/****************************************/
		offset = wIndex - (UNS16)0x6410;
		if( (wIndex > dict_cstes.analogOutputTableLastIndex) ||
			(analogOutputTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = analogOutputTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6440 ) && ( wIndex <= (UNS16)0x6447 ) ) 
	{ 
		/****************************************/
		/* Analog Output Set specific profile      */
		/****************************************/
		offset = wIndex - (UNS16)0x6440;
		if( (wIndex > dict_cstes.analogOutputSetTableLastIndex) ||
			(analogOutputSetTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = analogOutputSetTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6400 ) && ( wIndex <= (UNS16)0x640f ) ) 
	{ 
		/****************************************/
		/* Analog Input specific profile      */
		/****************************************/
		offset = wIndex - (UNS16)0x6400;
		if( (wIndex > dict_cstes.analogInputTableLastIndex) ||
			(analogInputTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = analogInputTable + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x6420 ) && ( wIndex <= (UNS16)0x6432 ) ) 
	{ 
		/****************************************/
		/* Analog Input Set specific profile      */
		/****************************************/
		offset = wIndex - (UNS16)0x6420;
		if( (wIndex > dict_cstes.analogInputSetTableLastIndex) ||
			(analogInputSetTable[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = analogInputSetTable + offset;
		return ptrTable;
	}else if( ( wIndex >= (UNS16)0x1200 ) && ( wIndex <= (UNS16)0x127F ) ) 
	{ 
		/****************************************/
		/* Server SDO Parameter                 */
		/****************************************/
		offset = wIndex - (UNS16)0x1200;
		if( (wIndex > dict_cstes.server_SDO_last) ||
			(serverSDOParameter[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = serverSDOParameter + offset;
		return ptrTable;
	}
	else if( ( wIndex >= (UNS16)0x1280 ) && ( wIndex <= (UNS16)0x12FF ) ) 
	{ 
		/****************************************/
		/* Client SDO Parameter                 */
		/****************************************/
		offset = wIndex - (UNS16)0x1280;
		if( (wIndex > dict_cstes.client_SDO_last) ||
			(clientSDOParameter[offset].pSubindex == NULL)) 
		{
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = clientSDOParameter + offset;
		return ptrTable;
	}else if( ( wIndex >= (UNS16)0x1000 ) && ( wIndex <= (UNS16)0x11FF ) ) 
	{
		/***********************************************/
		/* we are in the communication profile area... */
		/***********************************************/
		offset = wIndex - (UNS16)0x1000;
		if( (wIndex > dict_cstes.comm_profile_last ) || 
			(CommunicationProfileArea[offset].pSubindex == NULL))
		{
			// This index is not defined. 
			accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
			*errorCode = OD_NO_SUCH_OBJECT;
			return 0;
		}
		ptrTable = CommunicationProfileArea + offset;
		return ptrTable;
	}
	
	else
	{
		accessDictionaryError(wIndex, bSubindex, 0, dwSize, OD_NO_SUCH_OBJECT);
		*errorCode = OD_NO_SUCH_OBJECT;
		return 0;
	} 
  //return OD_SUCCESSFUL;
}