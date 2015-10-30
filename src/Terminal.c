/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Lyapin A.V.
 *
 ******************************************************************************/
//_____  I N C L U D E S ___________________________________________________
#include <string.h>
//#include <stdio.h>
#include "global.h"
#include "uart.h"
#include "fifo.h"
#include "pakuart.h"
#include "dma.h"


//_____ M A C R O S ________________________________________________________
//! Maximal number of characters in file path.
#define MAX_FILE_PATH_LENGTH  30

/*! \name Special Char Values
 */
//! @{
#define CR                    '\r'
#define LF                    '\n'
#define CRLF                  "\r\n"
#define CTRL_C                0x03
#define CTRL_Q                0x11
#define BKSPACE_CHAR          '\b'
#define ABORT_CHAR            CTRL_C
#define QUIT_APPEND           CTRL_Q
#define HISTORY_CHAR          0x1B	//ESC
//! @}
/*! \name Shell Commands
 */
//! @{
#define CMD_NONE					0
#define CMD_HELP					1
#define CMD_ADDR					2
#define CMD_CYKLE					3
#define CMD_TIMERS					4
#define CMD_CAN0					5
#define CMD_CAN1					6
#define CMD_CAN2					7
#define CMD_DMA						8
//! @}
/*! \name String Values for Commands
 */
//! @{
#define STR_HELP              		"help"
#define STR_ADDR              		"addr"
#define STR_CYKLE              		"cycle"
#define STR_TIMERS					"timers"
#define STR_CAN0					"can0"
#define STR_CAN1					"can1"
#define STR_CAN2					"can2"
#define STR_DMA					"dma"
//! @}
/*! \name String Messages
 */
//! @{
#define MSG_PROMPT            "$>"
#define MSG_WELCOME           "\r\n" \
                              "-------------------------\r\n" \
                              "    Fujitsu Shell\r\n" \
                              "-------------------------\r\n"
#define MSG_ER_CMD_NOT_FOUND  "Command not found\r\n"

#define MSG_HELP             "Поддерживаемые команды \r\n" \
                              " cycle - среднее время цикла, мкс \r\n" \
                              " addr  - вывод адреса блока \r\n" \
                              " timers- таймеры системы \r\n"\
                              " canx  - отображение регистров can (x-номер 0,1,2) \r\n"\
                              " dma   - отображение регистров DMA \r\n"

//! @}

//_____ D E C L A R A T I O N S ____________________________________________
//! flag for a command presence
static BOOL cmd = false;
//! command number
static uint8_t   cmd_type = CMD_NONE;
//! flag for first ls : mount if set
//static bool first_ls = true ;
//! string length
static uint8_t   i_str = 0;

//! string for command
static char cmd_str[10 + 2 * MAX_FILE_PATH_LENGTH];
//! string for first arg
static char par_str1[MAX_FILE_PATH_LENGTH];
//! string for second arg
static char par_str2[MAX_FILE_PATH_LENGTH];

//! buffer for command line
//static char str_buff[MAX_FILE_PATH_LENGTH];


static void parse_cmd_t(void)
{
	uint8_t i, j;

	// Get command type.
	for (i = 0; cmd_str[i] != ' ' && i < i_str; i++);

	if (i)
	{
		cmd = true;
		// Save last byte
		j = cmd_str[i];
		// Reset vars
		cmd_str[i] = '\0';
		par_str1[0] = '\0';
		par_str2[0] = '\0';

	// Decode command type.
	if  	(!strcmp(cmd_str, STR_HELP    				)) cmd_type = CMD_HELP;
	else if (!strcmp(cmd_str, STR_ADDR    				)) cmd_type = CMD_ADDR;
	else if (!strcmp(cmd_str, STR_TIMERS    			)) cmd_type = CMD_TIMERS;
	else if (!strcmp(cmd_str, STR_CAN0    			    )) cmd_type = CMD_CAN0;
	else if (!strcmp(cmd_str, STR_CAN1    			    )) cmd_type = CMD_CAN1;
	else if (!strcmp(cmd_str, STR_CAN2    			    )) cmd_type = CMD_CAN2;
	else if (!strcmp(cmd_str, STR_DMA    			    )) cmd_type = CMD_DMA;
#ifdef EN_SR_ZN_CYKL
	else if (!strcmp(cmd_str, STR_CYKLE    				)) cmd_type = CMD_CYKLE;
#endif

		else
		{
			// error : command not found
			puts((BYTE *)MSG_ER_CMD_NOT_FOUND);
			cmd = false;
		}
		// restore last byte
		cmd_str[i] = j;
	}
	// if command isn't found, display prompt
	if (!cmd)
	{
		puts((BYTE *)MSG_PROMPT);
	return;
	}

  // Get first arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for(; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str1[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str1[j] = '\0';
  }

  // Get second arg (if any).
  if (++i < i_str)
  {
    j = 0;
    // remove " if used
    if (cmd_str[i] == '"')
    {
      i++;
      for (; cmd_str[i] != '"' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
      i++;
    }
    // get the arg directly
    else
    {
      for (; cmd_str[i] != ' ' && i < i_str; i++, j++)
      {
        par_str2[j] = cmd_str[i];
      }
    }
    // null terminated arg
    par_str2[j] = '\0';
  }
}


void build_cmd(int c)
{
	switch (c)
	{
		// Add CRLF.
		//case CR:
		case LF:
			puts((BYTE *)CRLF);
			// Add NUL char.
			i_str--;
			cmd_str[i_str] = '\0';
			// Decode the command.
			parse_cmd_t();
			i_str = 0;
			break;
		// ^c abort cmd.
		case ABORT_CHAR:
			// Reset command length.
			i_str = 0;
			// Display prompt.
			puts((BYTE *)"\r\n" MSG_PROMPT);
			break;
		// Backspace.
		case BKSPACE_CHAR:
			if (i_str > 0)
			{
				// Replace last char.
				puts((BYTE *)"\b \b");
				// Decraese command length.
				i_str--;
			}
			break;
		default:
			// Echo.
			putch(c);
			// Append to cmd line.
			cmd_str[i_str++] = c;
			break;
	}
}
//========================================


void Terminal (void)
{
	U8 c;
	
	if (GetRxByte(&c) == FIFO_ERROR_UNDERFLOW)
		return;
	// While a usable user command on RS232 isn't received, build it
	if (!cmd)
	{
		build_cmd(c);
	}
	// perform the command
	if (cmd)
	{
		switch (cmd_type)
		{
			case CMD_DMA:
				DrawDMAStatus();
				break;
			case CMD_CAN0:
				DrawCanStatus(0);
				break;
			case CMD_CAN1:
				DrawCanStatus(1);
				break;
			case CMD_CAN2:
				DrawCanStatus(2);
				break;
	
			
			#ifdef EN_SR_ZN_CYKL
				case CMD_CYKLE:
					puts((BYTE *)" Среднее время цикла - ");
					putdec(program.SrCikl_mks);
					puts((BYTE *)" мкс \n\r");
					break;
			#endif
		
			case CMD_ADDR:
				puts((BYTE *)" Адрес блока - ");
				putdec(ADDR,3);
				puts((BYTE *)"\n\r");
				
				break;
			case CMD_TIMERS:
				PrintTimerService();
				break;
			//=============================================
			case CMD_HELP:
				// Display help on USART
				puts((BYTE *)MSG_HELP);
				break;
			// Unknown command.
			default:
				// Display error message.
				puts((BYTE *)MSG_ER_CMD_NOT_FOUND);
				break;
		}
		// Reset vars.
		cmd_type = CMD_NONE;
		cmd = false;
		// Display prompt.
		puts((BYTE *)MSG_PROMPT);
	}
}