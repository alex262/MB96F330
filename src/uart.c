/******************************************************************************
 * $Id$ / $Rev$ / $Date$
 * $URL$
 *****************************************************************************/
/*               (C) Fujitsu Microelectronics Europe GmbH               */
/*                                                                      */
/* The following software deliverable is intended for and must only be  */
/* used for reference and in an evaluation laboratory environment.      */
/* It is provided on an as-is basis without charge and is subject to    */
/* alterations.                                                         */
/* It is the user’s obligation to fully test the software in its        */
/* environment and to ensure proper functionality, qualification and    */
/* compliance with component specifications.                            */
/*                                                                      */
/* In the event the software deliverable includes the use of open       */
/* source components, the provisions of the governing open source       */
/* license agreement shall apply with respect to such software          */
/* deliverable.                                                         */
/* FME does not warrant that the deliverables do not infringe any       */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FME disclaims all  */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FME’s liability   */
/* is restricted to intention and gross negligence.                     */
/* FME is not liable for consequential damages.                         */
/*                                                                      */
/* (V1.3)                                                               */
/*****************************************************************************/
/** \file uart.c
 **
 ** - updated uart module to the golden coding rules 
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 *****************************************************************************/

//#include "base_type.h"
#include "mb96338us.h"  /* include all IO-Register definitions */
#include "uart.h"       /* include all UART-Functions */
#include "bootloader.h"

//#include "integer.h"
/*===========================================================================*/

/* declarations */

/* global variables */
//char_t line_buffer[255];
char_t line_buffer[50];
char_t *line_buffer_ptr;
uint8_t use_uart = 0;
/* constants */
const char_t ASCII[] = "0123456789ABCDEF";

/* prototypes */

/*===========================================================================*/


/*****************************************************************************/
/* UART0 Communication routines */
/*****************************************************************************/

/*****************************************************************************
 *  DESCRIPTION:    Initializes UART1
 *
 *  PARAMETERS:     None
 *
 *  RETURNS:        None
 *****************************************************************************/
void InitUART(BYTE ch)
{
	if(ch>7) return;
	
	switch(ch)
	{
		case 0:
		{	
			PIER08_IE2	= 1;	// enable SIN0
			DDR08_D3	= 1;	// SOT0 = output
			BGR0		= 417;	// 115200 Baud @ 48 MHz
			SCR0		= 0x17;	// 8N1
			SMR0		= 0x0d;	// enable SOT0, Reset, normal mode
			SSR0		= 0x00;	// LSB first
			break;
		}
		case 1:
		{	
			PIER08_IE5	= 1; 	// enable SIN1
			DDR08_D6	= 1;	// SOT1 = output
			BGR1		= 417;	// 115200 Baud @ 48 MHz
			SCR1		= 0x17;	// 8N1
			SMR1		= 0x0d;	// enable SOT1, Reset, normal mode
			SSR1		= 0x00;	// LSB first
			break;
		}
		case 2:
		{	
			PIER05_IE0	= 1;	// enable SIN2
			DDR05_D1	= 1;	// SOT2 = output
			BGR2		= 417;	// 115200 Baud @ 48 MHz
			SCR2		= 0x17;	// 8N1
			SMR2		= 0x0d;	// enable SOT2, Reset, normal mode
			SSR2		= 0x00;	// LSB first
			break;
		}
		case 3:
		{	
			PIER01_IE2	= 1;	// enable SIN3
			DDR01_D3	= 1;	// SOT3 = output
			BGR3		= 417;	// 115200 Baud @ 48 MHz
			SCR3		= 0x17;	// 8N1
			SMR3		= 0x0d;	// enable SOT3, Reset, normal mode
			SSR3		= 0x00;	// LSB first
			break;
		}
		case 4:
		{	
			PIER10_IE4	= 1;	// enable SIN5
			DDR10_D3	= 1;	// SOT5 = output
			BGR5		= 417;	// 115200 Baud @ 48 MHz
			SCR5		= 0x17;	// 8N1
			SMR5		= 0x0d;	// enable SOT5, Reset, normal mode
			SSR5		= 0x00;	// LSB first
			break;
		}
		case 5:
		{	
			PIER00_IE2	= 1;	// enable SIN7
			DDR00_D1	= 1;	// SOT7 = output
			BGR7		= 417;	// 115200 Baud @ 48 MHz
			SCR7		= 0x17;	// 8N1
			SMR7		= 0x0d;	// enable SOT7, Reset, normal mode
			SSR7		= 0x00;	// LSB first
			break;
		}
		case 6:
		{	
			PIER00_IE5	= 1;	// enable SIN8
			DDR00_D4	= 1;	// SOT8 = output
			BGR8		= 417;	// 115200 Baud @ 48 MHz
			SCR8		= 0x17;	// 8N1
			SMR8		= 0x0d;	// enable SOT8, Reset, normal mode
			SSR8		= 0x00;	// LSB first
			break;
		}
		case 7:
		{	
			PIER17_IE3	= 1;	// enable SIN9
			DDR17_D4	= 1;	// SOT9 = output
			BGR9		= 417;	// 115200 Baud @ 48 MHz
			SCR9		= 0x17;	// 8N1
			SMR9		= 0x0d;	// enable SOT9, Reset, normal mode
			SSR9		= 0x00;	// LSB first
			break;
		}
	}
						
}
void UART_RX_IntSet(BYTE ch, BYTE st)
{
	if(ch>7) return;
	
	switch(ch)
	{
		case 0:
		{	
			SSR0_RIE	= st&1;	
			break;
		}
		case 1:
		{	
			SSR1_RIE	= st&1;	
			break;
		}
		case 2:
		{	
			SSR2_RIE	= st&1;	
			break;
		}
		case 3:
		{	
			SSR3_RIE	= st&1;	
			break;
		}
		case 4:
		{	
			SSR5_RIE	= st&1;	
			break;
		}
		case 5:
		{	
			SSR7_RIE	= st&1;	
			break;
		}
		case 6:
		{	
			SSR8_RIE	= st&1;	
			break;
		}
		case 7:
		{	
			SSR9_RIE	= st&1;	
			break;
		}
	}
						
}
void UART_TX_IntSet(BYTE ch, BYTE st)
{
	if(ch>7) return;
	
	switch(ch)
	{
		case 0:
		{	
			SSR0_TIE	= st&1;	
			break;
		}
		case 1:
		{	
			SSR1_TIE	= st&1;	
			break;
		}
		case 2:
		{	
			SSR2_TIE	= st&1;	
			break;
		}
		case 3:
		{	
			SSR3_TIE	= st&1;	
			break;
		}
		case 4:
		{	
			SSR5_TIE	= st&1;	
			break;
		}
		case 5:
		{	
			SSR7_TIE	= st&1;	
			break;
		}
		case 6:
		{	
			SSR8_TIE	= st&1;	
			break;
		}
		case 7:
		{	
			SSR9_TIE	= st&1;	
			break;
		}
	}
						
}

void InitUART0( void )
{
	PIER08 |= 0x04; // enable SIN0
	DDR08_D3 = 1;      // SOT0 = output


	// BGR0 = 832;  //  9600 Baud @  8MHz
	// BGR0 = 416;  // 19200 Baud @  8MHz
	// BGR0 = 207;  // 38400 Baud @  8MHz

	//BGR0 = 1666; //  9600 Baud @ 16MHz
	// BGR0 = 832;  // 19200 Baud @ 16MHz
	// BGR0 = 416;  // 38400 Baud @ 16MHz

	// BGR0 = 2083; //  9600 Baud @ 20MHz
	// BGR0 = 1041; // 19200 Baud @ 20MHz
	// BGR0 = 520;  // 38400 Baud @ 20MHz

	// BGR0 = 2499; //  9600 Baud @ 24MHz
	// BGR0 = 1249; // 19200 Baud @ 24MHz
	// BGR0 = 624;  // 38400 Baud @ 24MHz

	// BGR0 = 3332;	//  9600 Baud @ 32MHz
	// BGR0 = 1666;	// 19200 Baud @ 32MHz
	// BGR0 = 832;	// 38400 Baud @ 32MHz

	BGR0 = 417;		// 115200 Baud @ 48 MHz

	SCR0 = 0x17; // 8N1
	SMR0 = 0x0d; // enable SOT0, Reset, normal mode
	SSR0 = 0x00; // LSB first
	use_uart = 0;
}
void InitUART1( void )
{
    PIER08_IE5 = 1; // enable SIN1
    DDR08_D6 = 1;	// SOT1 = output

    BGR1 = 417;  	// 115.2kbaud @ 48 MHz
    SCR1 = 0x17; 	// 8N1
    SMR1 = 0x0d; 	// enable SOT1, Reset, normal mode
    SSR1 = 0x00; 	// LSB first
    use_uart = 1;
}

/*****************************************************************************/
/*                                                                           */
/*                 UART Reception Interrupt Service Routine                  */
/*                                                                           */
/*****************************************************************************/
static BYTE	UART_START_BOOT_n = 0;
__interrupt void irq_uart0_rx(void)
{
	BYTE ch;

	if (SSR0_ORE | SSR0_FRE)
	{
		SCR0_TXE = 0;
		SCR0_CRE = 1;    // clear reception errors
		SCR0_TXE = 1;
	}
	ch = RDR0;
	//ESIR0 = 0x01;
	
	IRQ_RX0(ch);
	//===========================================
	// BOOTLOADER
	//===========================================
	if(ch == START_BOOT[UART_START_BOOT_n])
	{
		UART_START_BOOT_n++;
		if(UART_START_BOOT_n == 10)
		{
			DisInterrupt();
			ServiceBootloadUart();
			EnInterrupt();
		}
	}else
	{
		UART_START_BOOT_n = 0;
	}
	//==========================================
}
__interrupt void irq_uart0_tx(void)
{
	IRQ_TX0();
}

__interrupt void irq_uart1_rx(void)
{
	char ch;

	if (SSR1_ORE | SSR1_FRE)
	{
		SCR1_TXE = 0;
		SCR1_CRE = 1;    // clear reception errors
		SCR1_TXE = 1;
	}
	ch = RDR1;
	//ESIR1 = 0x01;
}
__interrupt void irq_uart2_rx(void)
{
	char ch;

	if (SSR2_ORE | SSR2_FRE)
	{
		SCR2_TXE = 0;
		SCR2_CRE = 1;    // clear reception errors
		SCR2_TXE = 1;
	}
	ch = RDR2;
	//ESIR2 = 0x01;
}
__interrupt void irq_uart3_rx(void)
{
	char ch;

	if (SSR3_ORE | SSR3_FRE)
	{
		SCR3_TXE = 0;
		SCR3_CRE = 1;    // clear reception errors
		SCR3_TXE = 1;
	}
	ch = RDR3;
	//ESIR3 = 0x01;
}
__interrupt void irq_uart5_rx(void)
{
	char ch;

	if (SSR5_ORE | SSR5_FRE)
	{
		SCR5_TXE = 0;
		SCR5_CRE = 1;    // clear reception errors
		SCR5_TXE = 1;
	}
	ch = RDR5;
	//ESIR5 = 0x01;
}
__interrupt void irq_uart7_rx(void)
{
	char ch;

	if (SSR7_ORE | SSR7_FRE)
	{
		SCR7_TXE = 0;
		SCR7_CRE = 1;    // clear reception errors
		SCR7_TXE = 1;
	}
	ch = RDR7;
	//ESIR7 = 0x01;
}
__interrupt void irq_uart8_rx(void)
{
	char ch;

	if (SSR8_ORE | SSR8_FRE)
	{
		SCR8_TXE = 0;
		SCR8_CRE = 1;    // clear reception errors
		SCR8_TXE = 1;
	}
	ch = RDR8;
	//ESIR8 = 0x01;
}
__interrupt void irq_uart9_rx(void)
{
	char ch;

	if (SSR9_ORE | SSR9_FRE)
	{
		SCR9_TXE = 0;
		SCR9_CRE = 1;    // clear reception errors
		SCR9_TXE = 1;
	}
	ch = RDR9;
//	ESIR9 = 0x01;
}

/*****************************************************************************
 *  DESCRIPTION:    sends a single character  (no timeout !)
 *
 *  PARAMETERS:     Character
 *
 *  RETURNS:        None
 *****************************************************************************/
void putch (char_t ch)        /* sends a char_t */
{
  switch (use_uart) {
    case 0:
      while (SSR0_TDRE == 0);      // wait for transmit buffer empty
      TDR0 = ch;                   // put ch into buffer
      break;
    case 1:
      while (SSR1_TDRE == 0);      // wait for transmit buffer empty
      TDR1 = ch;                   // put ch into buffer
      break;
  }
}

/*****************************************************************************
 *  DESCRIPTION:    sends a complete string (0-terminated) 
 *
 *  PARAMETERS:     Pointer to (constant) file of bytes in mem
 *
 *  RETURNS:        None
 *****************************************************************************/
void puts(char_t *buf)              
{
	while (*buf != '\0')
	{ 
		if(*buf == '\n') putch('\r');
		putch(*buf++);        /* send every char_t of string */
	}
}

/*****************************************************************************
 *  DESCRIPTION:    sends a x-digit Hex-number (as ASCII charcaters)
 *                  to terminal via ext. UART
 *
 *  PARAMETERS:     Value and number of Hex-digits (e.g. FF = 2 Hex digits)
 *
 *  RETURNS:        NONE
 *****************************************************************************/
void puthex(uint32_t n, uint8_t digits)
{
    uint8_t i,ch,div=0;

    puts("0x");                /* hex string */
    div=(digits-1) << 2;    /* init shift divisor */

    for (i=0; i<digits; i++) 
    {
      ch = (n >> div) & 0xF;/* get hex-digit value */
      putch(ASCII[ch]);        /* prompt to terminal as ASCII */
      div-=4;                /* next digit shift */
   }
}

void putbytehex(BYTE n)
{
    uint8_t digits = 2;
    uint8_t i,ch,div=0;

    puts("0x");                /* hex string */
    div=(digits-1) << 2;    /* init shift divisor */

    for (i=0; i<digits; i++) 
    {
      ch = (n >> div) & 0xF;/* get hex-digit value */
      putch(ASCII[ch]);        /* prompt to terminal as ASCII */
      div-=4;                /* next digit shift */
   }
}
/*****************************************************************************
 *  DESCRIPTION:    send a x-digit Dec-number (as ASCII charcaters)
 *                  to terminal via ext. UART
 *
 *  PARAMETERS:     integer value
 *
 *  RETURNS:        None
 *****************************************************************************/
void putdec(uint32_t x, uint8_t len)
{
	int16_t i;
	char_t buf[9];
	
	if(len>8) len = 8;
	if (x == 0) 
	{
		puts("       0");
		return;
	}
	buf[len]='\0';                /* end sign of string */
	
	for (i=len; i>0; i--) 
	{
		buf[i-1] = ASCII[x % 10];
		x = x/10;

	}

	for (i=0; buf[i]=='0'; i++) // no print16_t of zero 
	{
		buf[i] = ' ';
	}
	puts(buf);                    /* send string */
}

/*****************************************************************************
 *  DESCRIPTION:    waits for and returns incoming character (no timeout !)
 *
 *  PARAMETERS:     None
 *
 *  RETURNS:        Character or "-1" (Error)
 *****************************************************************************/

char_t getch (void)   
{
    char_t data;
    while((!SSR0_RDRF) && (!SSR1_RDRF));
    if (SSR0_RDRF) use_uart = 0;
    if (SSR1_RDRF) use_uart = 1;
    switch (use_uart) {
      case 0:
       while(!SSR0_RDRF);            /* wait for data in buffer */
       if(SSR0_FRE || SSR0_ORE || SSR0_PE)     // Framing-, overrun- or parity error
       {
         data = RDR0;    
         SCR0_CRE = 1;                         // reset FRE, ORE, PE error flags
         return (-1);
       } else
         return (RDR0);                /* return char_t */
       break;
       
      case 1:
       while(!SSR1_RDRF);            /* wait for data in buffer */
       if(SSR1_FRE || SSR1_ORE || SSR1_PE)     // Framing-, overrun- or parity error
       {
         data = RDR1;    
         SCR1_CRE = 1;                         // reset FRE, ORE, PE error flags
         return (-1);
       } else
         return (RDR1);                /* return char_t */
       break;
    }
    return (-1);
}

/*****************************************************************************
 *  DESCRIPTION:    Converts ASCII inputs (0..8,a..f,A..F) to num. value
 *
 *  PARAMETERS:     ASCII input
 *
 *  RETURNS:        value or -1 (error)
 *****************************************************************************/
uint32_t ASCIItobin(uint8_t k)
{
  char_t d=(char_t) -1;
  if ((k > 47) & (k < 58)) d = k - 48;  /* 0..9 */
  if ((k > 64) & (k < 71)) d = k - 55;  /* A..F */
  if ((k > 96) & (k < 103)) d = k - 87; /* a..f */
  return d;
}

/*****************************************************************************
 *  DESCRIPTION:    Receives a string until CR from ext UART
 *
 *  PARAMETERS:     (stored in global variable "line_buffer")
 *
 *  RETURNS:        None
 *****************************************************************************/
void receive_line(void)
{
  uint8_t ch;
  uint16_t i=0;

    do {
        ch = getch();             
        if((ch == '\r') | (ch=='\n')) break;
        line_buffer[i++] = ch;
    } while(1);
   
   line_buffer[i]='\0';
   line_buffer_ptr = line_buffer + i;
}

/*****************************************************************************
 *  DESCRIPTION:    Receives a string until CR from ext UART + Echo
 *
 *  PARAMETERS:     (stored in global variable "line_buffer")
 *
 *  RETURNS:        None
 *****************************************************************************/
int16_t receive_line_echo(uint16_t *cnt)
{
  
  uint8_t ch;
  uint16_t i=0;
  memset(line_buffer,0,sizeof(line_buffer));

    do {
            ch = 0xFF;
        while(ch == 0xFF)
        {
          ch = getch(); 
        }       
        if ((i == 0) & (ch != 8) | (i > 0)) putch(ch);       
        if((ch == 13) | (ch==27) | (i > 252)) break;
        if(ch == 8)
        {
            if  (i > 0) line_buffer[i--] = '\0';
        } else
        {
            line_buffer[i++] = ch;
        }
   } while(1);

   line_buffer[i++]='\0';
   line_buffer_ptr = line_buffer + i;
   if (cnt) *cnt = i;
   //BufferPtr = (char_t)  &line_buffer;
   return (int16_t)(&line_buffer);
}

/***********************************************************************
 * DESCRIPTION: Scans the string 'line_buffer' for the string 'string'.
 *              Both strings must be null-terminated. 'line_buffer_ptr'
 *              is set appropriately.
 * RETURN VALUE: 1, if string 'string' is contained in string 'line_buffer'
 *                  at its beginning. 'line_buffer_ptr' points to the
 *                  next character of 'line_buffer' after the equal field.
 *               2, if string 'string' was not found in string 'line_buffer'.
 *               0, if strings are identical to the final null character.
 ************************************************************************/
int16_t scan_line(char_t *str) 
{
    line_buffer_ptr = line_buffer;

    while((int16_t)*line_buffer_ptr==(int16_t)*str)
    {
        if((int16_t)*str=='\0') return(0);
        line_buffer_ptr++;
        str++;
    }
    
    if((int16_t)*str=='\0') return(1);
    return(2);
}

/*****************************************************************************
 *  DESCRIPTION:    Inputs a x-digit Hex-number (ASCII characters expected)
 *                      ESC aborts input
 *
 *  PARAMETERS:     Number of Hex-digits (e.g. FF = 2 Hex digits)
 *
 *  RETURNS:        Input value or -1 (abort)
 *****************************************************************************/
uint32_t Inputhex(uint8_t digits)
{
   uint32_t number=0,digit=0;
   uint8_t abort=0,mlt=0,i,key;

   mlt=(4*(digits-1));  /* 20 for 6 hex-digit numers, 4 for 2 hex-digits */
   for (i=0;i<digits;i++)
   {
     digit = -1;     
     while ((digit==-1) & (!abort))     /* input next valid digit */
     {
       key = getch();             /* wait for next key input (ASCII) */
       putch(key);
       if (key == 27) abort=1;    /* ESC aborts */
       digit = ASCIItobin(key);   /* convert to number */ 
       if (digit == -1) putch(8); /* backspace if not valid */
     }
     number+= (digit << mlt);     /* add digit value to number */
     mlt-=4;                      /* next digit shift */
   }

   if (!abort) 
      return number;             /* return input value */
   else
   {
      puts("\n\n input cancled \n");
      return -1;                /* return abort indicator */
   }
}

char_t getkey(char_t LKey, char_t HKey)
{
   char_t key;

   do                           /* input next valid digit */
   {
     key = upcase(getch());     /* wait for next key input (0-9,A-Z,a-z) */
     if (key == 27)
     {
       puts("\r>>> cancel input \n");
       return -1;            /* return with ESC aborts */
     }

     if ( (key < LKey) || (key > HKey) )
     {
       /* undefinded key pressed */
       puts("\r>>> key not defined \r");
     }
     else
     {
       puts("\r>                   \r");
       putch(key);
       return key;              /* return input value */
     }

   } while(1);
}

char_t upcase(char_t k)
{
  char_t d=(char_t) -1;
  if ((k > 47) & (k < 58))  d = k;      /* 0..9 */
  if ((k > 64) & (k < 71))  d = k;      /* A..F */
  if ((k > 96) & (k < 123)) d = k - 32; /* a..f */
  if (k == 27) d = k;                   /* ESC  */ 
  return d;
}
//============================================================================
// ----------------------- End of Procedure ---------------------------------
//============================================================================
