/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "mb96338us.h"
#include "global.h"
#include "spi.h"

/** V A R I A B L E S **********************************************/
// работаем использу€ UART

void InitSPI_1(void)
{
	PRRR8_SIN9_R	= 1;
	PRRR8_SOT9_R	= 1;
	PRRR9_SCK9_R	= 1;
	

	PIER07_IE7	= 1;	// enable SIN9
	DDR07_D6	= 1;	// SOT9 = output
	DDR07_D5	= 1;	// SCLK = output

	BGR9		= 47;	// 1M Baud @ 48 MHz
	SCR9		= 0x03;	// reception and transmission enable
	SSR9		= 0x04;	// MSB first, no interrupts
	SMR9		= 0x83;	// Mode 2, SCLK enable, SOT enable

	ESCR9		= 0x01;	// SCES = 1 => CPOL = 0
	ECCR9		= 0x00;	// SCDE = 0 => CPHA = 0
}
void InitSPI_1_inv(void)
{
	PRRR8_SIN9_R	= 1;
	PRRR8_SOT9_R	= 1;
	PRRR9_SCK9_R	= 1;
	

	PIER07_IE7	= 1;	// enable SIN9
	DDR07_D6	= 1;	// SOT9 = output
	DDR07_D5	= 1;	// SCLK = output

	//BGR9		= 47;	// 1M Baud @ 48 MHz
	BGR9		= 23;	// 2M Baud @ 48 MHz
	SCR9		= 0x03;	// reception and transmission enable
	SSR9		= 0x04;	// MSB first, no interrupts
	SMR9		= 0x83;	// Mode 2, SCLK enable, SOT enable

	ESCR9		= 0x00;	// SCES = 0 => CPOL = 0
	ECCR9		= 0x00;	// SCDE = 0 => CPHA = 0
}

void InitSPI_2(void)
{
	

	PIER01_IE2	= 1;	// enable SIN3
	DDR01_D2	= 0;	// SIN3 = output
	DDR01_D3	= 1;	// SOT3 = output
	DDR01_D4	= 1;	// SCLK = output

	BGR3		= 47;	// 1M Baud @ 48 MHz
	SCR3		= 0x03;	// reception and transmission enable
	SSR3		= 0x04;	// MSB first, no interrupts
	SMR3		= 0x83;	// Mode 2, SCLK enable, SOT enable

	ESCR3		= 0x01;	// SCES = 1 => CPOL = 0
	ECCR3		= 0x00;	// SCDE = 0 => CPHA = 0
}
void InitSPI_2_inv(void)
{
	

	PIER01_IE2	= 1;	// enable SIN3
	DDR01_D2	= 0;	// SIN3 = output
	DDR01_D3	= 1;	// SOT3 = output
	DDR01_D4	= 1;	// SCLK = output

	//BGR3		= 47;	// 1M Baud @ 48 MHz
	BGR3		= 23;	// 2M Baud @ 48 MHz
	SCR3		= 0x03;	// reception and transmission enable
	SSR3		= 0x04;	// MSB first, no interrupts
	SMR3		= 0x83;	// Mode 2, SCLK enable, SOT enable

	ESCR3		= 0x00;	// SCES = 0 => CPOL = 0
	ECCR3		= 0x00;	// SCDE = 0 => CPHA = 0
}

//*******************************************************************
