/* ------------------------------------------------------------------------- */
/* THIS SAMPLE CODE IS PROVIDED AS IS AND IS SUBJECT TO ALTERATIONS. FUJITSU */
/* MICROELECTRONICS ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR */
/* ELIGIBILITY FOR ANY PURPOSES.                                             */
/*                 (C) Fujitsu Microelectronics Europe GmbH                  */
/* ************************************************************************* */
/*               FUJITSU MICROELECTRONICS EUROPE GMBH                        */
/*               Pittlerstrasse 47, 63225 Langen, Germany                    */
/*               Tel.:++49/6103/690-0,Fax - 122                              */
/*                                                                           */
/* The following software is for demonstration purposes only.                */
/* It is not fully tested, nor validated in order to fullfill                */
/* its task under all circumstances. Therefore, this software                */
/* or any part of it must only be used in an evaluation                      */
/* laboratory environment.                                                   */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* USB Bootloader global includes                                            */
/* Version 1.0 MSc                                                           */
/*                                                                           */
/*                                                                   		 */
/*                                                                           */
/* ------------------------------------------------------------------------- */

#ifndef	_H_BOOTLOADER
#define _H_BOOTLOADER

#define FIXBOOT_ADR				0xDF0080  // Fixboot Vector Address, DO NOT CHANGE          (default 0xDF0080)
#define FIXBOOT_MAGICADR		0xDF0030  // Fixboot Magicword Address, DO NOT CHANGE       (default 0xDF0030)
#define	BOOTLOADER_START		0xDF0080  // Start of Bootloader                            (default 0xDF0080)
#define VECTTABLE				0xDF0000  // Vector Table Start, change also in vectors.c   (default 0xDF0000)
#define RESETVECT				0xFFFFDC  // Resetvector Address                            (default 0xFFFFDC)
#define RINGBUFFERMAX	512	// DO NOT CHANGE, 512Byte Block Read                            (default 512)
#define USE_UART		1   // 0: No UART Information, 1: UART Information                  (default 1)

typedef struct
{
	unsigned long cob_id;
	unsigned char rtr;
	unsigned char len;		// message length (0 to 8)
	unsigned char data[8];	// data 
}TMsgCan;

unsigned char ServiceBootloadCan(unsigned char bus_id, TMsgCan *m);

#define TYPE_UART				0  //обновляемся по uart
#define TYPE_CAN				1  //обновляемся по can

extern const char START_BOOT[];


//#define FLASHDBG	1		//for debuging the flash process, uncomment if not wanted       (default uncommented)

#endif