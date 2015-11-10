/************************************************************************
 ************************************************************************/
#ifndef _FLASH2_
#define _FLASH2_

void fwait2(unsigned long a);
unsigned char erase2(__far unsigned int *sector_adr);
unsigned char write2(__far unsigned int *adr, unsigned int wdata);


#endif 
/********************************* EOF **********************************/
