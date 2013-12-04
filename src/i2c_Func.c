/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "mb96338us.h"
#include "global.h"
#include "i2c.h"
#include "timer.h"

/** V A R I A B L E S **********************************************/
static const BYTE control = 0xA0;	// Control byte variable
#define WRITE   0x00
#define READ    0x01
static BYTE	SelectChI2C = 0;
//static TYPE_DATA_TIMER TimerI2C;

//*******************************************************************
void SetWorkChI2C(BYTE ch)
{
	if(ch<2) SelectChI2C = ch;
}
//*******************************************************************
void I2C_Init(BYTE ch)
{
	if(ch == 0)
	{
		ICCR0_EN  = 0;              // stop I2C interface
		ICCR0_CS4 = 0;              // CS4..0 : set prescaler
		ICCR0_CS3 = 0;
		ICCR0_CS2 = 1;
		ICCR0_CS1 = 1;
		ICCR0_CS0 = 1;
		ICCR0_EN  = 1;              // enable I2C interface
	
		IDAR0 = 0;                  // clear data register
	
		IBCR0_BER  = 0;             // clear bus error interrupt flag
		IBCR0_BEIE = 0;             // bus error interrupt disabled
		IBCR0_ACK  = 0;             // no acknowledge generated
		IBCR0_GCAA = 0;             // no call acknowledge is generated
		IBCR0_INTE = 0;             // disable interrupt
		IBCR0_INT  = 0;             // clear transfer interrupt request flag
	
		PIER04_IE4 = 1;             // SDA0 pin to input 
		PIER04_IE5 = 1;             // SCL0 pin to input 
		PUCR04_PU4 = 1;
		PUCR04_PU5 = 1;
	}
	if(ch == 1)
	{
		ICCR1_EN  = 0;              // stop I2C interface
		ICCR1_CS4 = 0;              // CS4..0 : set prescaler
		ICCR1_CS3 = 0;
		ICCR1_CS2 = 0;
		ICCR1_CS1 = 1;
		ICCR1_CS0 = 1;
		ICCR1_EN  = 1;              // enable I2C interface
	
		IDAR1 = 0;                  // clear data register
	
		IBCR1_BER  = 0;             // clear bus error interrupt flag
		IBCR1_BEIE = 0;             // bus error interrupt disabled
		IBCR1_ACK  = 0;             // no acknowledge generated
		IBCR1_GCAA = 0;             // no call acknowledge is generated
		IBCR1_INTE = 0;             // disable interrupt
		IBCR1_INT  = 0;             // clear transfer interrupt request flag
	
		PIER04_IE6 = 1;             // SDA0 pin to input 
		PIER04_IE7 = 1;             // SCL0 pin to input 
		PUCR04_PU6 = 1;
		PUCR04_PU7 = 1;
	}
}
//*******************************************************************
void I2C_Acknowlegde(void)
{
	//add_timer(&TimerI2C);
	//setTimer(&TimerI2C, 5);
	
	
	if(SelectChI2C == 0)
	{
		while(IBSR0_LRB == 1);
		{
			//if(getTimer(&TimerI2C) == 0) break;	
		}	
	}
	if(SelectChI2C == 1)
	{
		while(IBSR1_LRB == 1);	
		{
			//if(getTimer(&TimerI2C) == 0) break;	
		}	
			// no anwser from slave, program stucks here
			// a timeout mechanism should be implemented here
	}
	//del_timer(&TimerI2C);
}
//*******************************************************************
void I2C_Start(BYTE slave_address)
{
	if(SelectChI2C == 0)
	{
		do{
			IBCR0_BER = 0;                // clear bus error interrupt flag
			ICCR0_EN  = 1;                // enable I2C interface
			IDAR0 = slave_address;        // slave_address is sent out with start condition
		
			IBCR0_MSS = 1;                // set master mode and set start condition
			IBCR0_INT = 0;                // clear transfer end interrupt flag
		
			while(IBCR0_INT == 0);	        // look if transfer is in process
		}
		while (IBCR0_BER == 1);           // retry if Bus-Error detected
	
		while(IBSR0_LRB == 1)             // no acknowledge means device not ready
		{                                 // maybe last write cycle not ended yet
			IBCR0_SCC = 1;                // try restart (= continue)
			while (IBCR0_INT == 0);       // wait that transfer is finished
		}
	}
	if(SelectChI2C == 1)
	{
		do{
			IBCR1_BER = 0;                // clear bus error interrupt flag
			ICCR1_EN  = 1;                // enable I2C interface
			IDAR1 = slave_address;        // slave_address is sent out with start condition
		
			IBCR1_MSS = 1;                // set master mode and set start condition
			IBCR1_INT = 0;                // clear transfer end interrupt flag
		
			while(IBCR1_INT == 0);        // look if transfer is in process
		}
		while (IBCR1_BER == 1);         // retry if Bus-Error detected
	
		while(IBSR1_LRB == 1)           // no acknowledge means device not ready
		{                               // maybe last write cycle not ended yet
			IBCR1_SCC = 1;                // try restart (= continue)
			while (IBCR1_INT == 0);       // wait that transfer is finished
		}
	}
}
//*******************************************************************
void I2C_Continue(BYTE slave_address)
{
	if(SelectChI2C == 0)
	{
		IDAR0 = slave_address;          // slave_address is sent out with start condition
		IBCR0_SCC = 1;                  // restart (= continue)
		while (IBCR0_INT == 0);         // wait that transfer is finished
	}
	if(SelectChI2C == 1)
	{
		IDAR1 = slave_address;          // slave_address is sent out with start condition
		IBCR1_SCC = 1;                  // restart (= continue)
		while(IBCR1_INT == 0);         // wait that transfer is finished
	}
}
//*******************************************************************
void I2C_Stop(void)
{
	if(SelectChI2C == 0)
	{
		while (IBCR0_INT == 0);         // wait that transfer is finished
		IBCR0_MSS = 0;                  // change to slave and release stop condition
		IBCR0_INT = 0;                  // clear transfer end interrupt flag
		while (IBSR0_BB);               // wait till bus free
	}
	if(SelectChI2C == 1)
	{
		while(IBCR1_INT == 0);         // wait that transfer is finished
		IBCR1_MSS = 0;                  // change to slave and release stop condition
		IBCR1_INT = 0;                  // clear transfer end interrupt flag
		while(IBSR1_BB);               // wait till bus free
	}
}
//*******************************************************************
void I2C_Write(BYTE value)
{
	if(SelectChI2C == 0)
	{
		IDAR0 = value;                  // load data or address in to register
		IBCR0_INT = 0;                  // clear transfer end intrerupt flag
		while (IBCR0_INT == 0);         // look if transfer is in process
		I2C_Acknowlegde();              // wait for Acknowledge
	}
	if(SelectChI2C == 1)
	{
		IDAR1 = value;                  // load data or address in to register
		IBCR1_INT = 0;                  // clear transfer end intrerupt flag
		while(IBCR1_INT == 0);         // look if transfer is in process
		I2C_Acknowlegde();              // wait for Acknowledge
	}
}
//*******************************************************************
BYTE I2C_Read(void)
{
	if(SelectChI2C == 0)
	{
		IBCR0_ACK = 1;                  // no acknowledge has to be send
		IBCR0_INT = 0;                  // clear transfer end interrupt flag
		while (IBCR0_INT == 0);         // wait that transfer is finished
		return(IDAR0);                  // read received data out
	}
	if(SelectChI2C == 1)
	{
		IBCR1_ACK = 1;                  // no acknowledge has to be send
		IBCR1_INT = 0;                  // clear transfer end interrupt flag
		while(IBCR1_INT == 0);         // wait that transfer is finished
		return(IDAR1);                  // read received data out
	}
	return 0;
}
//*******************************************************************
BYTE I2C_LastRead(void)
{
	if(SelectChI2C == 0)
	{
		IBCR0_ACK = 0;                  // acknowledge has to be sent after last byte
		IBCR0_INT = 0;                  // clear transfer end interrupt flag
		while(IBCR0_INT == 0);          // wait that transfer is finished
		return(IDAR0);                  // read received data out
	}
	if(SelectChI2C == 1)
	{
		IBCR1_ACK = 0;                  // acknowledge has to be sent after last byte
		IBCR1_INT = 0;                  // clear transfer end interrupt flag
		while(IBCR1_INT == 0);          // wait that transfer is finished
		return(IDAR1);                  // read received data out
	}
	return 0;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
//        1WARE
/*---------------------------------------------------------------------------*/
/*#define ADDR_1WARE 0x30
void I2CBus_write(BYTE* txBytes, BYTE len)
{
	BYTE i;
	
	I2C_Start(ADDR_1WARE| WRITE);

	for (i = 0; i < len; i++)		// Loop through data bytes
	{
		I2C_Write(txBytes[i]);	// Output next data byte
	}
	I2C_Stop();	
}

void I2CBus_read(BYTE* rxBytes, BYTE len)
{
	BYTE i;
	
	I2C_Start(ADDR_1WARE| READ);

	for (i = 0; i < len; i++)			// Loop through data bytes
	{
		if (i < (len - 1))			// Check if more data will be read
		{
			rxBytes[i] = I2C_Read();	// If not last, input byte & send ACK
		}
		else
		{
			rxBytes[i] = I2C_LastRead();	// If last byte, input byte & send NAK
		}
	}
	I2C_Stop();							// Generate Stop condition
}*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
//         EEPROM
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/********************************************************************
 * Function:        void HighDensByteWrite(BYTE data)
 *
 * Description:     This function writes a single byte to a
 *                  high-density (>= 32 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
void HighDensByteWrite(WORD address, BYTE data)
{
	I2C_Start(control| WRITE);
	
	I2C_Write((BYTE)(address>>8));	// Output address MSB
	I2C_Write((BYTE)address);		// Output address LSB
	I2C_Write(data);				// Output data byte
	I2C_Stop();	
} 
/********************************************************************
 * Function:        void HighDensPageWrite(BYTE *data,
 *                                         BYTE numbytes)
 *
 * Description:     This function writes multiple bytes to a
 *                  high-density (>= 32 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
void HighDensPageWrite(WORD address, BYTE *data, WORD numbytes)
{
	WORD i, count;		// Loop counter
	
	WORD adr_page, add=0;
	
	while(numbytes>0)
	{
		adr_page = address&0x7F;
		
		if(adr_page+numbytes>127)
		{
			count = 128-adr_page;
			numbytes -= count;
		}else
		{
			count = numbytes;
			numbytes = 0;
		}
		
		I2C_Start(control| WRITE);
		I2C_Write((BYTE)(address>>8));	// Output address MSB
		I2C_Write((BYTE)address);		// Output address LSB
		
		for (i = 0; i < count; i++)  // Loop through data bytes
		{
			I2C_Write(data[i+add]);				// Output next data byte
		}
		I2C_Stop();	
		
		address += count;
		add += count;
		clrwdt;
		msDelay(10);
	}
	
}
/********************************************************************
 * Function:        void HighDensByteRead(BYTE *data)
 *
 * Description:     This function reads a single byte from a
 *                  high-density (>= 32 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
void HighDensByteRead(WORD address, BYTE *data)
{
	I2C_Start(control | WRITE);
	I2C_Write((BYTE)(address>>8));	// Output address MSB
	I2C_Write((BYTE)address);		// Output address LSB

	I2C_Continue(control | READ);   // Restart, with READ comand

	*data = I2C_LastRead();   // receive last byte from EEPROM
	I2C_Stop();               // Generate Stop condition
}
/********************************************************************
 * Function:        void HighDensSequentialRead(BYTE *data,
 *                                              BYTE numbytes)
 *
 * Description:     This function reads multiple bytes from a
 *                  high-density (>= 32 Kb) serial EEPROM device.
 *
 * Dependencies:    'control' contains the control byte
 *                  'address' contains the address word
 *******************************************************************/
void HighDensSequentialRead(WORD address, BYTE *data, WORD numbytes)
{
	WORD i;                 // Loop counter

	I2C_Start(control | WRITE);
	I2C_Write((BYTE)(address>>8));	// Output address MSB
	I2C_Write((BYTE)address);		// Output address LSB

	I2C_Continue(control | READ);   // Restart, with READ comand

	for (i = 0; i < numbytes; i++)  // Loop through data bytes
	{
		clrwdt;
		if (i < (numbytes - 1))     // Check if more data will be read
		{
			data[i] = I2C_Read(); // If not last, input byte & send ACK
		}
		else
		{
			data[i] = I2C_LastRead(); // If last byte, input byte & send NAK
		}
	}
	I2C_Stop();               // Generate Stop condition
}

