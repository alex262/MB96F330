/************************************************************************/
/************************************************************************/

#include "mb96338us.h"
#include "bootloader.h"

#define DQ7 0x0080	// data polling flag
#define DQ5 0x0020	// time limit exceeding flag
#define DQ3 0x0008	//sector erase timer flag

#pragma section FAR_CODE=RAMCODE, attr=CODE		// RAM location
#pragma section CODE=RAMCODE, attr=CODE

#define far_vuint	__far volatile unsigned int
#define far_uint	__far unsigned int


/*************************************************/
//------------------------------------------------------------------------------------------------------------------
void fwait(unsigned long a)
{
	unsigned long i;
	
	for (i = 0; i < a; i++)
	{
		__wait_nop();
	}
}
/**************** FLASH ROUTINES *****************/

// Erases no sector, but displays that it'll not erased

void nerase(unsigned long adr)
{
//	puts("leaving sector: ");
//	puthex(adr,6);
//	puts(" - BOOT-LOADER\n");
}

// Erases Flash sector

unsigned char erase(__far unsigned int *sector_adr)
{
	unsigned char flag = 0;
	unsigned char MCSRA_save;
	unsigned int  MTCRA_save;
	__far volatile unsigned int *seq_AAAA;
	__far volatile unsigned int *seq_5554;
	seq_AAAA = (far_uint*)(((unsigned long)sector_adr & 0x0FFF000) + 0x0AAA);
	seq_5554 = (far_uint*)(((unsigned long)sector_adr & 0x0FFF000) + 0x0554);
	// preparations
	//puts1("erasing sector: ");
	//puthex1(sector_adr,6);
	//puts1("\n");
	MCSRA_save = MCSRA;		// save Flash settings
	MTCRA_save = MTCRA;
		
	MCSRA_CRBE = 0;			// disable Code Read Buffer
	MCSRA_DRBE = 0;			// disable Data Read Buffer

	MTCRA = 0x4B3D;			//was 0x4B3D    // slow down Flash access to 4 wait states

	MCSRA_WE = 1;			// set write enable flag
	
	*seq_AAAA = 0x00AA;		// sends the command to the pointed address
	*seq_5554 = 0x0055;
	*seq_AAAA = 0x0080;
	*seq_AAAA = 0x00AA;
	*seq_5554 = 0x0055;
	*sector_adr = 0x0030;	// erase sector at the pointed address
	
	while ((*sector_adr & DQ3) != DQ3);  // sector erase timer ready?
	
	while(flag == 0)
	{
		if((*sector_adr & DQ7) == DQ7)	// Toggle bit
		{
			flag = 1;					// successful erased
		}
		
		if((*sector_adr & DQ5) == DQ5)	// time out
		{
			if((*sector_adr & DQ7) == DQ7)
			{
				flag = 1;				// successful erased
			}
			else
			{
				flag = 2;				// timeout error
			}
		}
	}
	
	MCSRA_WE = 0;          // reset write enable flag

	MTCRA = MTCRA_save;    // restore Flash settings
	MCSRA = MCSRA_save;
	
	return(flag);
}

unsigned int RamRead(unsigned long adr)
{
	unsigned int y;	
	y = (*(unsigned int*) adr);
	return y;
}


// Write word data to flash

unsigned char write(__far unsigned int *adr, unsigned int wdata)
{
	unsigned char flag = 0;
	unsigned char MCSRA_save;
	unsigned int  MTCRA_save;
	__far volatile unsigned int *seq_AAAA;
	__far volatile unsigned int *seq_5554;
	
	seq_AAAA = (far_uint*)(((unsigned long)adr & 0x0FFF000) + 0x0AAA);
	seq_5554 = (far_uint*)(((unsigned long)adr & 0x0FFF000) + 0x0554);
	
	// preparations
	#ifdef FLASHDBG
	puts("<FLASH ADR ");
	puthex(adr,6);
	puts(" D: ");
	puthex((unsigned long)(wdata),4);
	puts(">\n");
	#endif
	
	MCSRA_save = MCSRA;	// save Flash settings
	MTCRA_save = MTCRA;
	
	MCSRA_CRBE = 0;		// disable Code Read Buffer
	MCSRA_DRBE = 0;		// disable Data Read Buffer
	
	MTCRA = 0x4B3D;		//was 0x4B3D     // slow down Flash access to 4 wait states

	MCSRA_WE = 1;		// set write enable flag
	fwait(100);
	*seq_AAAA = 0x00AA;	// sends the command to the pointed address
	*seq_5554 = 0x0055;
	*seq_AAAA = 0x00A0;
	*adr = wdata;		// send data to the pointed address

	while(flag == 0)
	{
		if((*adr & DQ7) == (wdata & DQ7))	// Toggle bit
		{
			flag = 1;						// successful written
		}
		
		if((*adr & DQ5) == DQ5)				// time out
		{
			if((*adr & DQ7) == (wdata & DQ7))
			{
				flag = 1;					// successful written
			}
			else
			{
				flag = 2;					// timeout error
			}
		}
	}
	MTCRA = MTCRA_save;  // restore Flash settings
	MCSRA = MCSRA_save;
	fwait(100);	
	MCSRA_WE = 0;        // reset write enable flag
	fwait(100);
	return(flag);
}


