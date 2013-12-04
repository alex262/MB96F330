//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
#include "global.h"
#include "I2C.h"
#include "D1Ware.h"
#include "uart.h"
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
#define POLL_LIMIT  200
// Flags on	I2C_read 
#define	ACK	   1
#define	NACK   0
#define	EXPECT_ACK	  1
//#define	EXPECT_NACK	  0

// defines the data	direction (reading from	I2C	device)	in I2C_start(),I2C_rep_start() 
#define	I2C_READ	1

// defines the data	direction (writing to I2C device) in I2C_start(),I2C_rep_start() 
#define	I2C_WRITE	0

// One Wire	Commands
#define	readByteCommand		0x96
#define	writeByteCommand	0xA5
#define	resetOneWireCommand	0xB4
#define	onewireBitCommand	0x87


// DS2482 Commands and pointers
#define	setReadPointerCommand	0xE1
#define	readDataRegister		0xE1
#define	writeConfigCommand		0xD2
#define	statusRegister			0xF0
#define	configRegister			0xC3
#define	deviceReset				0xF0
 
// 1-Wire levels
#define	NORMAL		0
#define	POWER_DELIV	1

// 1-Wire speeds
#define	Overdrive 1
#define	Standard 0
//--------------------------------------------------------------------------
// DS2482 commands
#define	CMD_DRST   0xF0
#define	CMD_WCFG   0xD2
#define	CMD_CHSL   0xC3
#define	CMD_SRP	   0xE1
#define	CMD_1WRS   0xB4
#define	CMD_1WWB   0xA5
#define	CMD_1WRB   0x96
#define	CMD_1WSB   0x87
#define	CMD_1WT	   0x78

// DS2482 config bits
#define	CONFIG_APU	0x01
#define	CONFIG_PPM	0x02
#define	CONFIG_SPU	0x04
#define	CONFIG_1WS	0x08

// DS2482 status bits 
#define	STATUS_1WB	0x01
#define	STATUS_PPD	0x02
#define	STATUS_SD	0x04
#define	STATUS_LL	0x08
#define	STATUS_RST	0x10
#define	STATUS_SBR	0x20
#define	STATUS_TSB	0x40
#define	STATUS_DIR	0x80

// API mode	bit	flags
#define	MODE_STANDARD				   0x00
#define	MODE_OVERDRIVE				   0x01
#define	MODE_STRONG					   0x02


// Search state
BYTE ROM_NO[8];
int	LastDiscrepancy;
int	LastFamilyDiscrepancy;
int	LastDeviceFlag;
BYTE crc8;

// DS2482 state
static const BYTE I2C_address=0x30;
BYTE short_detected;
//BYTE c1WS, cSPU, cPPM, cAPU;
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------
void GetDataWare(BYTE ch)
{
	BYTE lastD, i;
	BYTE cnt, rslt;
		
	SetWorkChI2C(ch);
	
	DS2482_detect();
	
	
	cnt	= 0;
	rslt = OWFirst();
	while (rslt)
	{
		for (i = 0; i < 8; i++)
		{
			program.SN_1Ware_Dev[cnt][i] = ROM_NO[i];
		}
		rslt = OWNext();
		cnt++;
		if(cnt == MAX_COUNT_1WARE_DEV)
			break;
	}
	program.Cnt1WareDev = cnt;
}
//--------------------------------------------------------------------------
// Write the configuration register	in the DS2482. The configuration 
// options are provided	in the lower nibble	of the provided	config byte. 
// The uppper nibble in	bitwise	inverted when written to the DS2482.
//	
// Returns:	 TRUE: config written and response correct
//			 FALSE:	response incorrect
//
BYTE DS2482_write_config(BYTE config)
{
	BYTE read_config;

	// Write configuration (Case A)
	//	 S AD,0	[A]	WCFG [A] CF	[A]	Sr AD,1	[A]	[CF] A\	P
	//	[] indicates from slave
	//	CF configuration byte to write

	I2C_Start(I2C_address |	I2C_WRITE);
	I2C_Write(CMD_WCFG);
	I2C_Write(config | (~config	<< 4));
	I2C_Continue(I2C_address | I2C_READ);
	read_config	= I2C_LastRead();
	I2C_Stop();

	// check for failure due to	incorrect read back
	if (config != read_config)
	{
		// handle error
		// ...
		DS2482_reset();

		return FALSE;
	}
	return TRUE;
}
//--------------------------------------------------------------------------
// Perform a device	reset on the DS2482
//
// Returns:	TRUE if	device was reset
//			FALSE device not detected or failure to	perform	reset
//
BYTE DS2482_reset(void)
{
	BYTE	status;

	// Device Reset
	//	 S AD,0	[A]	DRST [A] Sr	AD,1 [A] [SS] A\ P
	//	[] indicates from slave
	//	SS status byte to read to verify state

	I2C_Start(I2C_address |	I2C_WRITE);
	I2C_Write(CMD_DRST);
	
	I2C_Continue(I2C_address | I2C_READ);
	
	status = I2C_LastRead();
	I2C_Stop();

	// check for failure due to	incorrect read back	of status
	return ((status	& 0xF7)	== 0x10);
}
//--------------------------------------------------------------------------
// DS2428 Detect routine that sets the I2C address and then	performs a 
// device reset	followed by	writing	the	configuration byte to default values:
//	 1-Wire	speed (c1WS) = standard	(0)
//	 Strong	pull-up	(cSPU) = off (0)
//	 Presence pulse	masking	(cPPM) = off (0)
//	 Active	pull-up	(cAPU) = on	(CONFIG_APU	= 0x01)
//
// Returns:	TRUE if	device was detected	and	written
//			FALSE device not detected or failure to	write configuration	byte
//

BYTE DS2482_detect(void)
{
	// set global address
	//I2C_address =	addr;

	// reset the DS2482	ON selected	address
	if (!DS2482_reset())
		return FALSE;

	// default configuration
/*	c1WS = FALSE;
	cSPU = FALSE;
	cPPM = FALSE;
	cAPU = CONFIG_APU;*/

//#define	CONFIG_APU	0x01
//#define	CONFIG_PPM	0x02
//#define	CONFIG_SPU	0x04
//#define	CONFIG_1WS	0x08


	// write the default configuration setup
	if (!DS2482_write_config(/*CONFIG_SPU|CONFIG_APU*/0))//c1WS |	cSPU | cPPM	| cAPU))
		return FALSE;

	return TRUE;
}

//-------------------------------------------------------------------------------------------------------
// Find	the	'first'	devices	on the 1-Wire network
// Return TRUE	: device found,	ROM	number in ROM_NO buffer
//		  FALSE	: no device	present
//
BYTE OWFirst(void)
{
	// reset the search	state
	LastDiscrepancy			= 0;
	LastDeviceFlag			= FALSE;
	LastFamilyDiscrepancy	= 0;

	return OWSearch();
}

//--------------------------------------------------------------------------
// The 'OWSearch' function does	a general search.  This	function
// continues from the previous search state. The search	state
// can be reset	by using the 'OWFirst' function.
// This	function contains one parameter	'alarm_only'.
// When	'alarm_only' is	TRUE (1) the find alarm	command
// 0xEC	is sent	instead	of the normal search command 0xF0.
// Using the find alarm	command	0xEC will limit	the	search to only
// 1-Wire devices that are in an 'alarm' state.
//
// Returns:	  TRUE (1) : when a	1-Wire device was found	and	its
//						 Serial	Number placed in the global	ROM	
//			  FALSE	(0): when no new device	was	found.	Either the
//						 last search was the last device or	there
//						 are no	devices	on the 1-Wire Net.
//
BYTE OWSearch(void)
{
	BYTE id_bit_number;
	BYTE last_zero,	rom_byte_number, search_result;
	BYTE id_bit, cmp_id_bit;
	BYTE rom_byte_mask,	search_direction, status;

	// initialize for search
	id_bit_number =	1;
	last_zero =	0;
	rom_byte_number	= 0;
	rom_byte_mask =	1;
	search_result =	FALSE;
	crc8 = 0;

	// if the last call	was	not	the	last one
	if (!LastDeviceFlag)
	{
		// 1-Wire reset
		if (!OWReset())
		{
			// reset the search
			LastDiscrepancy	= 0;
			LastDeviceFlag = FALSE;
			LastFamilyDiscrepancy =	0;
			return FALSE;
		}
		// issue the search	command	
		OWWriteByte(0xF0);	

		// loop	to do the search
		do{
			// if this discrepancy if before the Last Discrepancy
			// on a	previous next then pick	the	same as	last time
			if (id_bit_number <	LastDiscrepancy)
			{
				if ((ROM_NO[rom_byte_number] & rom_byte_mask) >	0)
					search_direction = 1;
				else
					search_direction = 0;
			}
			else
			{
				// if equal	to last	pick 1,	if not then	pick 0
				if (id_bit_number == LastDiscrepancy)
					search_direction = 1;
				else
					search_direction = 0;
			}

			// Perform a triple	operation on the DS2482	which will perform 2 read bits and 1 write bit
			status = DS2482_search_triplet(search_direction);

			// check bit results in	status byte
			id_bit = ((status &	STATUS_SBR)	== STATUS_SBR);
			cmp_id_bit = ((status &	STATUS_TSB)	== STATUS_TSB);
			search_direction = ((status	& STATUS_DIR) == STATUS_DIR) ? 1 : 0;

			// check for no	devices	on 1-Wire
			if ((id_bit) &&	(cmp_id_bit))
				break;
			else
			{
				if ((!id_bit) && (!cmp_id_bit) && (search_direction	== 0))
				{
					last_zero =	id_bit_number;

					// check for Last discrepancy in family
					if (last_zero <	9)
						LastFamilyDiscrepancy =	last_zero;
				}

				// set or clear	the	bit	in the ROM byte	rom_byte_number
				// with	mask rom_byte_mask
				if (search_direction ==	1)
					ROM_NO[rom_byte_number]	|= rom_byte_mask;
				else
					ROM_NO[rom_byte_number]	&= ~rom_byte_mask;

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;

				// if the mask is 0	then go	to new SerialNum byte rom_byte_number and reset	mask
				if (rom_byte_mask == 0)
				{
					calc_crc8(ROM_NO[rom_byte_number]);	// accumulate the CRC
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		}while(rom_byte_number	< 8);  // loop until through all ROM bytes 0-7

		// if	the	search was successful then
		if (!((id_bit_number < 65) ||	(crc8 != 0)))
		{
			//	search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			LastDiscrepancy = last_zero;

			//	check for last device
			if	(LastDiscrepancy ==	0)
				LastDeviceFlag = TRUE;

			search_result = TRUE;
		}
	}

	// if no	device found then reset	counters so	next 'search' will be like a first
	if (!search_result || (ROM_NO[0]	== 0))
	{
		LastDiscrepancy =	0;
		LastDeviceFlag = FALSE;
		LastFamilyDiscrepancy	= 0;
		search_result	= FALSE;
	}
	return search_result;
}
//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current 
// global 'crc8' value. 
// Returns current global crc8 value
//
BYTE calc_crc8(BYTE data)
{
	BYTE i; 

	// See Application Note 27
	crc8 = crc8 ^ data;
	for (i = 0; i < 8; ++i)
	{
		if (crc8 & 1)
			crc8 = (crc8 >> 1) ^ 0x8c;
		else
			crc8 = (crc8 >> 1);
	}

	return crc8;
}

//---------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Use the DS2482 help command '1-Wire triplet' to perform one bit of a 1-Wire
// search. This command does two read bits and one write bit. The write bit
// is either the default direction (all device have same bit) or in case of 
// a discrepancy, the 'search_direction' parameter is used. 
//
// Returns – The DS2482 status byte result from the triplet command
//
BYTE DS2482_search_triplet(BYTE search_direction)
{
	BYTE status;
	BYTE poll_count = 0;

	// 1-Wire Triplet (Case B)
	//   S AD,0 [A] 1WT [A] SS [A] Sr AD,1 [A] [Status] A [Status] A\ P
	//                                         \--------/        
	//                           Repeat until 1WB bit has changed to 0
	//  [] indicates from slave
	//  SS indicates byte containing search direction bit value in msbit
	
	I2C_Start(I2C_address | I2C_WRITE);
	I2C_Write(CMD_1WT);
	I2C_Write(search_direction ? 0x80 : 0x00);
	I2C_Continue(I2C_address | I2C_READ);

	// loop checking 1WB bit for completion of 1-Wire operation 
	// abort if poll limit reached
	do
	{
		status = I2C_Read();
	}while ((status & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

	status = I2C_LastRead();

	I2C_Stop();

	// check for failure due to poll limit reached
	if (poll_count >= POLL_LIMIT)
	{
		// handle error
		// ...
		DS2482_reset();
		return 0;
	}
	// return status byte
	return status;
}

//---------------------------------------------------------------------------
//-------- Basic 1-Wire functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns: TRUE(1):  presence pulse(s) detected, device(s) reset
//          FALSE(0): no presence pulses detected
//
BYTE OWReset(void)
{
	BYTE status;
	BYTE poll_count = 0;

	// 1-Wire reset (Case B)
	//   S AD,0 [A] 1WRS [A] Sr AD,1 [A] [Status] A [Status] A\ P
	//                                   \--------/        
	//                       Repeat until 1WB bit has changed to 0
	//  [] indicates from slave

	I2C_Start(I2C_address | I2C_WRITE);
	I2C_Write(CMD_1WRS);
	I2C_Continue(I2C_address | I2C_READ);

	// loop checking 1WB bit for completion of 1-Wire operation 
	// abort if poll limit reached
	do
	{
		status = I2C_Read();
	}while ((status & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

	status = I2C_LastRead();

	I2C_Stop();

	// check for failure due to poll limit reached
	if (poll_count >= POLL_LIMIT)
	{
		// handle error
		// ...
		DS2482_reset();
		return FALSE;
	}

	// check for short condition
	if (status & STATUS_SD)
		short_detected = TRUE;
	else
		short_detected = FALSE;

	// check for presence detect
	if (status & STATUS_PPD)
		return TRUE;
	else
		return FALSE;
}
//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
void OWWriteByte(BYTE sendbyte)
{
	BYTE status;
	BYTE poll_count = 0;

	// 1-Wire Write Byte (Case B)
	//   S AD,0 [A] 1WWB [A] DD [A] Sr AD,1 [A] [Status] A [Status] A\ P
	//                                          \--------/        
	//                             Repeat until 1WB bit has changed to 0
	//  [] indicates from slave
	//  DD data to write

	I2C_Start(I2C_address | I2C_WRITE);
	I2C_Write(CMD_1WWB);
	I2C_Write(sendbyte);
	I2C_Continue(I2C_address | I2C_READ);

	// loop checking 1WB bit for completion of 1-Wire operation 
	// abort if poll limit reached
	do
	{
		status = I2C_Read();
	}while ((status & STATUS_1WB) && (poll_count++ < POLL_LIMIT));

	status = I2C_LastRead();

	I2C_Stop();

	// check for failure due to poll limit reached
	if (poll_count >= POLL_LIMIT)
	{
		// handle error
		// ...
		DS2482_reset();
	}
}
//--------------------------------------------------------------------------
// Find the 'next' devices on the 1-Wire network
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
BYTE OWNext(void)
{
	// leave the search state alone
	return OWSearch();
}

//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------

