#ifndef	_H_1WARE_
#define _H_1WARE_

void GetDataWare(BYTE Ch);

BYTE DS2482_detect(void);
BYTE DS2482_reset(void);
BYTE DS2482_search_triplet(BYTE search_direction);

BYTE OWFirst(void);
BYTE OWSearch(void);
BYTE OWReset(void);
BYTE calc_crc8(BYTE data);
void OWWriteByte(BYTE sendbyte);
BYTE OWNext(void);

#endif