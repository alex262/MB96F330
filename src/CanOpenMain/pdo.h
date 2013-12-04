/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *
 *********************************************************
           File : sdo.h
 *                                                       *
 *********************************************************/


#ifndef __pdo_h__
#define __pdo_h__
#include "can.h"
#include "global.h"

#ifdef CASH_PDO_DATA
typedef struct
{
	UNS8	count;
	UNS8	msg[8];
}CASH_PDO;
extern CASH_PDO	process_PDO_CASH;
//extern CASH_PDO Cash_Pdo_Data[];
UNS8 buildPDO_CASH(UNS16 index);
#endif
/** Transmit a PDO data frame on the bus bus_id
 * pdo is a structure which contains the pdo to transmit
 * bus_id is hardware dependant
 * return f_can_send(bus_id,&m) or 0xFF if error
 * request can take the value  REQUETE or DONNEES */
 
UNS8 sendPDO (UNS8 bus_id, s_PDO *pdo, UNS8 request);

/** Prepare a PDO frame transmission, 
 * whose different parameters are stored in process_var table,
 * to the slave.
 * bus_id is hardware dependant
 * call the function sendPDO
 * return the result of the function sendPDO or 0xFF if error */

UNS8 PDOmGR (UNS8 bus_id, UNS32 cobId);

/** Prepare the PDO defined at index to be sent by  PDOmGR
 * Copy all the data to transmit in process_var
 * *pwCobId : returns the value of the cobid. (subindex 1)
* Return 0 or 0xFF if error. */

UNS8 buildPDO (UNS16 index, UNS32 **pwCobId);

/** Transmit a PDO request frame on the bus bus_id
 * to the slave.
 * bus_id is hardware dependant */
 
void sendPDOrequest (UNS8 bus_id,  UNS32 cobId);


/** Compute a PDO frame reception
 * bus_id is hardware dependant
 * return 0xFF if error, else return 0 */
 
UNS8 proceedPDO (UNS8 bus_id, Message *m);


/* used by the application to send a variable by PDO.
 * Check in which PDO the variable is mapped, and send the PDO. 
 * of course, the others variables mapped in the PDO are also sent !
 * ( ie when a specific event occured)
 * bus_id is hardware dependant
 * variable is a pointer to the variable which has to be sent. Must be
 * defined in the object dictionary
 * return 0xFF if error, else return 0 */
 
UNS8 sendPDOevent (UNS8 bus_id, void * variable);

#endif
