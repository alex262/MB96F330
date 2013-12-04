/************************************************************************
 ************************************************************************/
#ifndef _FLASH_
#define _FLASH_

void fwait(unsigned long a);
void nerase(unsigned long adr);
unsigned char erase(__far unsigned int *sector_adr);
unsigned int RamRead(unsigned long adr);
unsigned char write(__far unsigned int *adr, unsigned int wdata);

#define read_byte(adr) (*(__far unsigned char *)(adr))
#define read_word(adr) (*(__far unsigned int  *)(adr))
#define read_long(adr) (*(__far unsigned long *)(adr))

#endif 
/********************************* EOF **********************************/
