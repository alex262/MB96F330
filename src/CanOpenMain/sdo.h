/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************

           File : sdo.h
 *                                                       *
 *********************************************************/

#ifndef __sdo_h__
#define __sdo_h__

/******************** CONSTANTES ****************/
/// Abort Domain Transfer
// ----------------------
/* Should not be modified */

#define ADT_server 0x80
#define ADT_client 0x80


/** When a structure needn't yet to be used
 * the structure fields are put to 0 */

void resetSDOline (UNS8 bus_id, UNS8 num);

/** Search for an unused line in the transfers array
 * to store a new SDO, frame IDD or IDU, which is arriving
 * bus_id is hardware dependant
 * return 0xFF if all the lines are on use. Else, return 0 */

UNS8 getSDOfreeLine (UNS8 bus_id, UNS8 *line);

/** Search for the line, in the transfers array, which contains the
 * beginning of the reception of a fragmented SDO
 * when a SDO frame, DDS or UDS, is arriving
 * bus_id is hardware dependant
 * nodeId correspond to the message node-id 
 * return 0xFF if error.  Else, return 0 */

UNS8 getSDOlineOnUse (UNS8 bus_id, UNS8 nodeId, UNS8 *line);

/** Transmit a SDO frame on the bus bus_id
 * sdo is a structure which contains the sdo to transmit
 * bus_id is hardware dependant
 * return f_can_send(bus_id,&m) or 0xFF if error */
 
UNS8 sendSDO (UNS8 bus_id, s_SDO sdo);

/** Transmit a SDO error to the client. The reasons may be :
 * Read/Write to a undefined object
 * Read/Write to a undefined subindex
 * Read/write a not valid length object
 * Write a read only object */
 
UNS8 sendSDOabort(UNS8 bus_id, UNS16 index, UNS8 subIndex, UNS32 abortCode);

/** Function called by fonction Read/writeNetworkDict
 * to prepare a SDO frame transmission
 * to the slave whose node_id is ID
 * bus_id is hardware dependant
 * Line gives the position of the SDO in the storing array
 * return 0xFF if error, 0 if no message to send, or
 * return the result of the function sendSDO */
 
UNS8 SDOmGR (UNS8 bus_id, UNS8 line);

/** Treat a SDO frame reception
 * bus_id is hardware dependant
 * call the function sendSDO
 * return 0xFF if error
 *        0x80 if transfert aborted by the server
 *        0x0  ok */
 
UNS8 proceedSDO (UNS8 bus_id, Message *m);

/** Used by the application to send a SDO request frame to write the data *data
 * at the index and subindex indicated
 * in the dictionary of the slave whose node_id is nodeId
 * Count : nb of bytes to write in the dictionnary
 * bus_id is hardware dependant
 * return 0xFF if error, else return 0 */
 
UNS8 writeNetworkDict (UNS8 bus_id, UNS8 nodeId, UNS16 index, 
		       UNS8 subindex, UNS8 count, void *data); 

/** Used by the application to send a SDO request frame to read
 * in the dictionary of the slave whose node_id is ID
 * at the index and subindex indicated
 * the data read will be stored in the string pointed by data
 * bus_id is hardware dependant
 * return 0xFF if error, else return 0 */
 
UNS8 readNetworkDict (UNS8 bus_id, UNS8 nodeId, UNS16 index, UNS8 subindex);

/** Use this function after a readNetworkDict to get the result.
  Returns : SDO_SUCCESS = 1    // data is available
            SDO_ABORTED = 0x80 // Aborted transfer (The server have sent an abort SDO)
            SDO_IN_PROGRESS = 0x0 // Receiving the data

  example :
  UNS32 data;
  UNS8 size;
  readNetworkDict(0, 0x05, 0x1016, 1) // get the data index 1016 subindex 1 of node 5
  while (getReadResultNetworkDict (0, 0x05, &data, &size) != SDO_IN_PROGRESS);*/
  
UNS8 getReadResultNetworkDict (UNS8 bus_id, UNS8 nodeId, void* data, UNS8 *size);


/**
  Use this function after a writeNetworkDict to get the result of the write.
  Returns : SDO_SUCCESS = 1    // data have been written in the server's directory.
            SDO_ABORTED = 0x80 // Aborted transfer (The server have sent an abort SDO)
            SDO_IN_PROGRESS = 0x0 // Writing the data.
  example :
  UNS32 data = 0x50;
  UNS8 size
  writeNetworkDict(0, 0x05, 0x1016, 1, size, &data) // write the data index 1016 subindex 1 of node 5
  while ( getWriteResultNetworkDict (0, 0x05) != SDO_IN_PROGRESS);  */
  
UNS8 getWriteResultNetworkDict (UNS8 bus_id, UNS8 nodeId);

/**
  This function can be used by the client SDO after a getWriteResultNetworkDict or a getReadResultNetworkDict if
one of those function have returned a SDO_ABORTED.
It returns the SDO abort code, the index and subindex where the error occurs.*/

s_SDOabort getSDOerror (UNS8 bus_id, UNS8 nodeId);

#endif
