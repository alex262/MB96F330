//bootloader.c
#include "mb96338us.h"
#include "bootloader.h"
#include "global.h"

#define BOOTLOADER_CODE_ADDR		0xDF5800	//адрес должен совпадать с адресом секции CODE_BOOTLOADER
#define BOOTLOADER_CODE_SIZE		0x2400	

//фиксированное расположение кода для последующего копирования в RAM 
// младшая часть адреса (16 бит) соответствует области памяти RAM куда будет скопирован код
#pragma section CODE=CODE_BOOTLOADER,		attr=CODE,	locate=0xDF5800
#pragma section FAR_CODE=CODE_BOOTLOADER,	attr=CODE,	locate=0xDF5800

//расположение стека на время загрузки программы - гарантия что не затрем стек при копировании кода в RAM
#pragma asm
	.SECTION  STACK_BOOTLOADER, STACK, LOCATE=07E00H
	.ALIGN 2
	.RES.B 0A0H
#pragma endasm

//переопределение стеков
#pragma inline set_stack
void set_stack(void)
{
	__asm("	MOVL A, #07EA0H");	//верхушка стека LOCATE + size
    __asm("	MOVW SP,A");
    __asm("	MOV  A, #0");
    __asm("	MOV  USB, A");
    __asm("	MOV  SSB, A");
}

//фиксированное расположение переменных и инициализационых данных - гарантия что они не
// окажутся в области куда будет скопирован код
#pragma section INIT=INIT_BOOTLOADER, 		attr=DATA, locate=0x7EA0
#pragma section FAR_INIT=INIT_BOOTLOADER,	attr=DATA, locate=0x7EA0
#pragma section DATA=DATA_BOOTLOADER, 		attr=DATA, locate=0x7F40
#pragma section FAR_DATA=DATA_BOOTLOADER, 	attr=DATA, locate=0x7F40

//------------------------------------------------------------------------------------------------------------------
#define TYPE_UART				0  //обновляемся по uart
#define TYPE_CAN				1  //обновляемся по can

WORD VerBootloader = 0x0103; 
const char START_BOOT[] = {'B','O','O','T','L','O','A','D','E','R'};

//------------------------------------------------------------------------------------------------------------------

char ASCII_[] = "0123456789ABCDEF";
char SPACES[]="       0";

//------------------------------------------------------------------------------------------------------------------
static U16	ID_BOOT;			// ID для обмена с УСО
static U8	CAN_BUS;			// номер CAN по которому идёт обмен
static char_t BufferCAN[50];	// буфер 
//------------------------------------------------------------------------------------------------------------------
#define SWAP_USHORT(x) ( ((x) >> 8) | ((x) << 8) )	// Swap 2 bytes of a word
//------------------------------------------------------------------------------------------------------------------
static BYTE	WATCH_DOG_CODE_ = 0x55;		

#define clrwdt_ 			WDTCP = WATCH_DOG_CODE_; WATCH_DOG_CODE_ = ~WATCH_DOG_CODE_; // очистка WatchDog
//------------------------------------------------------------------------------------------------------------------
__near unsigned char erase(__far unsigned int *sector_adr);
__near unsigned char write(__far unsigned int *adr, unsigned int wdata);
#define read_byte(adr) (*(__far unsigned char *)(adr))

__near void SendCanBuf(char_t *buf, U8 len);
__near U8 CanReciveMsg(TMsgCan *msg);
//------------------------------------------------------------------------------------------------------------------

__near int get_ch_ (char * ch)
{
	char c;
	if(SSR0_RDRF)
	{
		if (SSR0_ORE || SSR0_PE) 
		{	/* overrun or parity error */
			c = RDR0;
			return (-1);
		}else
		{
			*ch = RDR0;
			return (1);				/* return char */
		}
	}
	return (-1);					/* return char */
}
//------------------------------------------------------------------------------------------------------------------
__near void putch_ (char_t ch)				/* sends a char_t */
{
	while (SSR0_TDRE == 0);			// wait for transmit buffer empty
	TDR0 = ch;						// put ch into buffer
}
//------------------------------------------------------------------------------------------------------------------
__near void puts_count(const char_t *buf, U8 count)
{
	while ((count--) > 0) putch_(*buf++);
}
//------------------------------------------------------------------------------------------------------------------
__near void puts_(const char_t *buf)
{
	while (*buf != '\0')putch_(*buf++);
}
//------------------------------------------------------------------------------------------------------------------
__near void puthex_(unsigned long n, char digits)
{
	char i,ch,div=0;

	div=(digits-1) << 2;		/* init shift divisor */

	for (i=0; i<digits; i++)
	{
		ch = (n >> div) & 0xF;	/* get hex-digit value */
		putch_(ASCII_[ch]);		/* prompt to terminal as ASCII */
		div-=4;					/* next digit shift */
	}
}
//------------------------------------------------------------------------------------------------------------------
__near void conv_hex(unsigned long n, char digits)
{
	char i,ch,div=0;

	div=(digits-1) << 2;		/* init shift divisor */

	for (i=0; i<digits; i++)
	{
		ch = (n >> div) & 0xF;	/* get hex-digit value */
		BufferCAN[i] =ASCII_[ch];		/* prompt to terminal as ASCII */
		div-=4;					/* next digit shift */
	}
}
//------------------------------------------------------------------------------------------------------------------
__near void buf_8bit_hex_(U8 num, U8 n)
{
	BufferCAN[num]		= ASCII_[(n>>4)&0xF];
	BufferCAN[num+1]	= ASCII_[n&0xF];
}
//------------------------------------------------------------------------------------------------------------------
__near void put_8bit_hex_(unsigned char n)
{
	putch_(ASCII_[(n>>4)&0xF]);
	putch_(ASCII_[n&0xF]);
}
//------------------------------------------------------------------------------------------------------------------
__near BYTE OstatokDiv10(uint32_t x)
{
	while(x >= 10)
	{
		x-=10;
	}
	return x;
}
__near uint32_t  Div10(uint32_t x)
{
	uint32_t ret=0;
	
	while(x >= 10)
	{
		x-=10;
		ret++;
	}
	return ret;
}
__near void putdec_(uint32_t x)
{
	int16_t i;
	char_t buf[9];
	if (x == 0) 
	{
		puts_(SPACES);
		return;
	}
	buf[8]='\0';				/* end sign of string */

	for (i=8; i>0; i--) 
	{
		//buf[i-1] = ASCII_[x % 10];
		buf[i-1] = ASCII_[OstatokDiv10(x)];
		x = Div10(x);
		//x = x/10;
	}

	for (i=0; buf[i]=='0'; i++)	// no print16_t of zero 
	{
		buf[i] = ' ';
	}
	puts_(buf);					/* send string */
}
//------------------------------------------------------------------------------------------------------------------
__near void conv_dec(uint32_t x)
{
	int16_t i;
	char_t buf[9];
	buf[8]='\0';				/* end sign of string */

	for (i=8; i>0; i--) 
	{
		//buf[i-1] = ASCII_[x % 10];
		buf[i-1] = ASCII_[OstatokDiv10(x)];
		x = Div10(x);
		//x = x/10;
	}

	for (i=0; buf[i]=='0'; i++)	// no print16_t of zero 
	{
		buf[i] = ' ';
	}
	for(i=0;i<9;i++)
		BufferCAN[i] = buf[i];
}
//------------------------------------------------------------------------------------------------------------------
/*convert string hex value to long*/
__near DWORD StrHexToLong(BYTE *inputStr, BYTE lenStr)
/* 
const char *inputStr: input string
unsigned char lenStr: length of hey value
*/
{
	BYTE i;
	BYTE c;
	DWORD sum = 0;
	for(i=0;i<lenStr;i++)
	{
		c = inputStr[i];
		if ((c > 96) & (c < 123)) c = c - 32;
		if ((inputStr[i] >= '0') & (inputStr[i] <= '9'))
		{
			sum = sum * 16;
			sum += inputStr[i] - '0';
		} else if ((c >= 'A') & (c <= 'F'))
		{
			sum = sum * 16;
			sum += (c - 'A') + 10;
		} else
		{
			return sum;
		}
	}
	return sum;
}
//------------------------------------------------------------------------------------------------------------------
__near int write_MHX_com(BYTE * pData)
{
	__far unsigned int *adr;
	DWORD data,crc;
	BYTE crcbyte;
	BYTE ofs, i;

	if(pData[0] != 'S') return (-1); // error input
		
	
	if (pData[1] == '2')
	{// S-Record type 2
		// reading record length 
		i = (DWORD)StrHexToLong(&pData[2],2) & 0xFF; // storing record length
		if(i != 0x14) return (-3);
		crc=0x14;			// building CRC
			
		// storing addresslength and converting it into a even word address 
		adr = (__far unsigned int *)((unsigned long)StrHexToLong(&pData[4],6));
		
//		if((DWORD)adr >= 0xF80000)	{
			// building crc 
			crc +=(DWORD)((((DWORD)adr & 0xFF0000) >> 16) + (((DWORD)adr & 0x00FF00) >> 8) + ((DWORD)adr & 0x0000FF));
			//crc +=pData[4]+pData[5]+pData[6]+pData[7]+pData[8]+pData[9];
				
			ofs = 10;
			for(i=0;(i<8);i++) // check if main app is located in bootloader section
			{
				// reading data
				data = StrHexToLong(&pData[ofs+4*i],4);	// conversion
		
				// building crc
				crc += (data & 0x00FF) + ((data & 0xFF00) >> 8);
				// swap data
				if(data != 0xFFFF)
				{
					data = SWAP_USHORT(data);	
					write(adr,data);
				}
						
				adr++;
			}
			data = StrHexToLong(&pData[42],2);	// conversion
			crcbyte = (BYTE)data; /* data is CRC byte */
					
			// CRC check 
			if ((0xFF ^ (BYTE)crc) != crcbyte) 
			{
				// CRC error 
				//write((__far unsigned int *)RESETVECT,0x0000);	// deleting everything in the resetvector
				//write((__far unsigned int *)(RESETVECT+2),0x0000); // deleting everything in the resetvector
				return (-4);
			}
//		}else return 0;
	}else return (-2);
	return (1);
}
//------------------------------------------------------------------------------------------------------------------
#define COUNT_MEM_PART				2
static unsigned long ADDR_START_MEM[COUNT_MEM_PART]	= {0xDF0000,0xF80000};
static unsigned long ADDR_END_MEM[COUNT_MEM_PART]	= {0xDF7FFF,0xFFFFFF};

#define CMD_NONE					0x00
#define CMD_EXIT					0x01
#define CMD_BOOTLOADER				0x02
#define CMD_ERASE					0x03
#define CMD_REBOOT					0x04
#define CMD_READ					0x05
#define CMD_WRITE					0x06
#define CMD_GET_VER_BOOT			0x07

char STR_EXIT[]="EXIT";
char STR_BOOTLOADER[]="BOOTLOADER";
char STR_ERASE[]="ERASE";
char STR_REBOOT[]="REBOOT";
char STR_READ[]="READ";
char STR_WRITE[]="WRITE";
char STR_GET_VER_BOOT[]="GETBOOTVER";

static BOOL cmd = FALSE;
#define END_LINE	'\0'

//! command number
static BYTE   cmd_type = CMD_NONE;
static BYTE   i_str = 0;

//! string for command
#define MAX_LEN_CMD_LINE	50

static char cmd_str[MAX_LEN_CMD_LINE];
//! string for first arg
static char par_str1[20];
//! string for second arg
static char par_str2[20];

__near static void parse_cmd(void);

//------------------------------------------------------------------------------------------------------------------
__near void build_cmd_b(int c)
{
	if(c == END_LINE)
	{
		// Add NUL char.
		//i_str--;
		cmd_str[i_str] = '\0';
		// Decode the command.
		parse_cmd();
		i_str = 0;
	}else
	{	// Echo.
		// Append to cmd line.
		cmd_str[i_str++] = c;
		if(i_str >= MAX_LEN_CMD_LINE) i_str=0;
	}
}
//------------------------------------------------------------------------------------------------------------------
__near BYTE _strlen(const char *rhs)
{
	BYTE res = 0;
	while(*(rhs++)) ++res;

	return res;
}
//------------------------------------------------------------------------------------------------------------------
__near int _strcmp(const char *first, const char *last)
{
	while ((*first && *last)&&(*first == *last))
	{
		++first;
		++last;	
	}
	return (*first - *last);
}
//------------------------------------------------------------------------------------------------------------------
__near static void parse_cmd(void)
{
	BYTE i, j;
	// Get command type.
	for (i = 0; cmd_str[i] != ' ' && i < i_str; i++);
	if (i)
	{
		cmd = TRUE;
		// Save last byte
		j = cmd_str[i];
		// Reset vars
		cmd_str[i] = '\0';
		par_str1[0] = '\0';
		par_str2[0] = '\0';

		// Decode command type.
		if  	(!_strcmp(cmd_str, STR_BOOTLOADER	)) cmd_type = CMD_BOOTLOADER;
		else if (!_strcmp(cmd_str, STR_EXIT			)) cmd_type = CMD_EXIT;
		else if (!_strcmp(cmd_str, STR_ERASE		)) cmd_type = CMD_ERASE;
		else if (!_strcmp(cmd_str, STR_REBOOT		)) cmd_type = CMD_REBOOT;
		else if (!_strcmp(cmd_str, STR_READ			)) cmd_type = CMD_READ;
		else if (!_strcmp(cmd_str, STR_WRITE		)) cmd_type = CMD_WRITE;
		else if (!_strcmp(cmd_str, STR_GET_VER_BOOT	)) cmd_type = CMD_GET_VER_BOOT;
		else
		{
			cmd = FALSE;
		}
		// restore last byte
		cmd_str[i] = j;
	}
	// if command isn't found, display prompt
	if (!cmd)
	{
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
//------------------------------------------------------------------------------------------------------------------
static BYTE BufferReadCom[44];
static BYTE CountReadComm=0;

static char_t STR_OK[10] = {'O','K',0,0,0,0,0,0,0,0};
static char_t STR_OK_A[5] = {'O','K',' ','A',0};
static char_t STR_OK_B[5] = {'O','K',' ','B',0};

__near void wait1mks()
{
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
	__wait_nop();
}

__near void wait_mks(WORD n)
{
	WORD i;
	for(i=0; i<n; i++) wait1mks();
}

char STR_ERROR[]="ERORR ";
char STR_READY[]="READY";
char STR_END_READ[]="END READ";
char STR_S214[]="S214";

// par1 - номер CAN U16 - ID CAN сообщения 
__near void ObrCmd(BYTE TypeInterf)
{
	TMsgCan msg;
	char ch;
	BYTE i, j, Data[0x10], st_send, crc;
	DWORD adr, adr_s;
	int ret;
	switch (cmd_type)
	{
		case CMD_BOOTLOADER:
			if(TypeInterf == TYPE_UART)
			{
				puts_count(STR_OK, 3);
			}
			if(TypeInterf == TYPE_CAN)
			{
				SendCanBuf(STR_OK, 3);
			}
			//StBootloader = 1;
			/* =============================================================== */
			/* Enable Sectors for FLASH writing and erasing               ++++ */
			/* =============================================================== */
			FMWC1 = 0xFF;       // enable sector for FLASH writing
			FMWC5 = 0xFF;       // enable sector for FLASH writing 
			/* =============================================================== */
			break;
		case CMD_GET_VER_BOOT:
			/* =============================================================== */
			/* Запрос версии BOOTLOADERa                                 +++++ */
			/* =============================================================== */
				SendCanBuf((char_t*)(&VerBootloader), 2);
			break;
		case CMD_ERASE:
			//if(StBootloader == 1)
			/* =============================================================== */
			/* Erasing flash sectors                                     +++++ */
			/* =============================================================== */
/*
			nerase(0xDF0000);	// NOT erases SA0
			nerase(0xDF2000);	// NOT erases SA1
			nerase(0xDF4000);	// NOT erases SA2
			nerase(0xDF6000);	// NOT erases SA3
*/
			erase((__far unsigned int *)0xDF0000);	// NOT erases SA0
			clrwdt_;
			erase((__far unsigned int *)0xDF2000);	// NOT erases SA1
			clrwdt_;
			erase((__far unsigned int *)0xDF4000);	// NOT erases SA2
			clrwdt_;
			erase((__far unsigned int *)0xDF6000);	// NOT erases SA3
			clrwdt_;

			erase((__far unsigned int *)0xF80000);	// erases SA32
			clrwdt_;
			erase((__far unsigned int *)0xF90000);	// erases SA33
			clrwdt_;
			erase((__far unsigned int *)0xFA0000);	// erases SA34
			clrwdt_;
			erase((__far unsigned int *)0xFB0000);	// erases SA35
			clrwdt_;
			erase((__far unsigned int *)0xFC0000);	// erases SA36
			clrwdt_;
			erase((__far unsigned int *)0xFD0000);	// erases SA37
			clrwdt_;
			erase((__far unsigned int *)0xFE0000); 	// erases SA38
			clrwdt_;
			erase((__far unsigned int *)0xFF0000);	// erases SA39
			clrwdt_;
			//flashResetVector(); // прописываем обратно RESET VECTOR
			//EnInterrupt();				// enables interrupts after flash erase
			if(TypeInterf == TYPE_UART)
			{
				puts_count(STR_OK, 3);
			}
			if(TypeInterf == TYPE_CAN)
			{
				SendCanBuf(STR_OK, 3);
			}
			/* =============================================================== */
			break;
		case CMD_REBOOT:
			/* =============================================================== */
			/* Reset microcontroller                                      +++++*/
			/* =============================================================== */
			if(TypeInterf == TYPE_UART)
			{
				puts_count(STR_OK, 9);
			}
			if(TypeInterf == TYPE_CAN)
			{
				SendCanBuf(STR_OK, 8);
			}
			RCR_SRSTG = 1;	// software - reset
			/* =============================================================== */			
			break;
		case CMD_READ:
			/* =============================================================== */			
			/*   READ FLASH MEMORY                                        +++++*/
			/* =============================================================== */			
			for(i=0; i<COUNT_MEM_PART; i++)
			{
				adr = ADDR_START_MEM[i];
				while(1)
				{
					st_send=0;
					adr_s = adr;
					for(j=0; j<0x10; j++)
					{
						Data[j] = read_byte(adr++);	
						if(Data[j] != 0xFF) st_send=1;
					}
					if(st_send)
					{
						crc = 0;
						if(TypeInterf == TYPE_UART)
						{
							puts_(STR_S214);
							puthex_(adr_s,6);
						}
						if(TypeInterf == TYPE_CAN)
						{
							SendCanBuf(STR_S214, 4);
							conv_hex(adr_s,6);
							SendCanBuf(BufferCAN, 6);
						}
						for(j=0; j<0x10; j++)
						{
							if(TypeInterf == TYPE_UART)
							{
								put_8bit_hex_(Data[j]);
							}
							if(TypeInterf == TYPE_CAN)
							{
								buf_8bit_hex_(2*j, Data[j]);
							}
							crc+=Data[j];
						}
						if(TypeInterf == TYPE_CAN)
						{
							SendCanBuf(BufferCAN, 0x10*2);
						}

						crc+=0x14;
						crc+=adr_s&0xFF;
						crc+=(adr_s>>8)&0xFF;
						crc+=(adr_s>>16)&0xFF;
						crc = crc^0xFF;
						if(TypeInterf == TYPE_UART)
						{
							put_8bit_hex_(crc);
						}
						if(TypeInterf == TYPE_CAN)
						{
							buf_8bit_hex_(0, crc);
							SendCanBuf(BufferCAN, 2);
							//------------------------
							// add timeout for MP3
							wait_mks(2000);
						}
					}
					if(adr >= ADDR_END_MEM[i]) break;
				}
			}
			if(TypeInterf == TYPE_UART)
			{
				puts_(STR_END_READ);putch_(0);
			}
			if(TypeInterf == TYPE_CAN)
			{
				BufferCAN[0] = 'E';
				BufferCAN[1] = 'N';
				BufferCAN[2] = 'D';
				BufferCAN[3] = ' ';
				BufferCAN[4] = 'R';
				BufferCAN[5] = 'E';
				BufferCAN[6] = 'A';
				BufferCAN[7] = 'D';
				BufferCAN[8] = 0;
				
				SendCanBuf(BufferCAN, 9);
			}
			break;
		//--------------------------------------------------
		case CMD_WRITE:
			/* =============================================================== */
			/* Programming microcontroller                               ++++++*/
			/* =============================================================== */
			CountReadComm=0;
			if(TypeInterf == TYPE_UART)
			{
				puts_(STR_READY);putch_(0);
			}
			if(TypeInterf == TYPE_CAN)
			{
				BufferCAN[0] = 'R';
				BufferCAN[1] = 'E';
				BufferCAN[2] = 'A';
				BufferCAN[3] = 'D';
				BufferCAN[4] = 'Y';
				BufferCAN[5] = 0;
				
				SendCanBuf(BufferCAN, 6);
			}
			while(1)
			{
				clrwdt_;					// обнуление WatchDog таймера 
				if(TypeInterf == TYPE_UART)
				{
					if(get_ch_(&ch) == 1) BufferReadCom[CountReadComm++] = ch;
				}
				if(TypeInterf == TYPE_CAN)
				{
					if(CanReciveMsg(&msg) == TRUE)	
					{// приняли сообщение, обрабатываем
						if(msg.cob_id == ID_BOOT)// обрабатываем только наши пакеты, хотя они и так фильтруются на входе
						{
							for(i=0; i<msg.len; i++)
								BufferReadCom[CountReadComm++] = msg.data[i];
						}	
					}	
				}
				if(CountReadComm == 44)
				{
					ret = write_MHX_com(BufferReadCom);
					if(ret<0)
					{
						if(TypeInterf == TYPE_UART)
						{
							puts_(STR_ERROR);putdec_(ret*(-1));putch_(0);
						}
						if(TypeInterf == TYPE_CAN)
						{
							SendCanBuf(STR_ERROR, 6);
							conv_dec(ret*(-1));
							SendCanBuf(BufferCAN, 9);
						}
						break;
					}
					CountReadComm = 0;
					if(TypeInterf == TYPE_UART)
					{
						if(ret == 0){	puts_(STR_OK_B);putch_(0);putch_(0);putch_(0);}
						if(ret == 1){	puts_(STR_OK_A);putch_(0);putch_(0);putch_(0);}
					}
					if(TypeInterf == TYPE_CAN)
					{
						if(ret == 0)	SendCanBuf(STR_OK_B, 5);
						if(ret == 1)	SendCanBuf(STR_OK_A, 5);
					}
				}
				if(CountReadComm == 4)
				{
					if((BufferReadCom[0] == 'E')&&(BufferReadCom[1] == 'N')&&(BufferReadCom[2] == 'D')&&(BufferReadCom[3] == 0))
					{
						CountReadComm = 0;
						if(TypeInterf == TYPE_UART)
						{
							puts_count(STR_OK, 9);
						}
						if(TypeInterf == TYPE_CAN)
						{
							SendCanBuf(STR_OK, 3);
						}
						break;
					}
				}
			}
			/* =============================================================== */			
			break;
		//--------------------------------------------------
		// Unknown command.
		default:
			break;
	}
}


//------------------------------------------------------------------------------------------------------------------
__far void ServiceBootloadUartStart (void)
{
	char ch;
	/* =============================================================== */
	/* Enable Sectors for FLASH writing and erasing                    */
	/* =============================================================== */
	FMWC1 = 0xFF;       // enable sector for FLASH writing
	FMWC5 = 0xFF;       // enable sector for FLASH writing 
	/* =============================================================== */
	
	puts_count(STR_OK, 3);
	clrwdt_;
	while(1)
	{
		clrwdt_;					// обнуление WatchDog таймера 
		
		if(get_ch_(&ch)>=0) 
		{
				
			// While a usable user command on RS232 isn't received, build it
			if (!cmd)
			{
				build_cmd_b(ch);
			}
			// perform the command
			if (cmd)
			{
				ObrCmd(TYPE_UART);
				
				// Reset vars.
				cmd_type = CMD_NONE;
				cmd = FALSE;
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
#define MSG2STD_(msg) ((msg & 0x000007FFL) << 18)
#define STD2MSG_(reg) (((reg & 0x1FFFFFFFL) >> 18) & 0x000007FFL)

__near void CAN0_buffer_(U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB0 = MSG2STD_(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB0_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB0_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK0			= MSG2STD_(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK20_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR0_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR0_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR0_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR0_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR0_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR0_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR0_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR0_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK0  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ0 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN1_buffer_(U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if((Num==0)||(Num>32)) return;
	
	// Prepare Arbitration Interface Register
	IF1ARB1 = MSG2STD_(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB1_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB1_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Mask Interface Register 
	IF1MSK1			= MSG2STD_(mask);// <<< Setup Mask corresponding to your application 
	IF1MSK21_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 

	// Prepare Message Control Interface Register 
	IF1MCTR1_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR1_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR1_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR1_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR1_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR1_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR1_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR1_EOB	= 1;	// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK1  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ1 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN2_buffer_(U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if((Num==0)||(Num>32)) return;
	// Prepare Arbitration Interface Register
	IF1ARB2 = MSG2STD_(setID);	// <<< Define message id: Use MSG2STD() for 11bit IDs / Use MSG2EXT() for 29bit IDs
	IF1ARB2_XTD		= 0;		// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB2_DIR		= Dir&1;	// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL	= 1;		// <<< 0: Buffer invalid / 1: Buffer valid 
	// Prepare Mask Interface Register 
	IF1MSK2			= MSG2STD_(mask);		// <<< Setup Mask corresponding to your application 
	IF1MSK22_MXTD	= 1;		// <<< 0: ID type is not used for filtering / 1: ... is used for filtering 
	// Prepare Message Control Interface Register 
	IF1MCTR2_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR2_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR2_UMASK	= 1;		// <<< 0: Not use Filter mask (Full CAN Object) / 1: Use Filter Mask 
	IF1MCTR2_TXIE	= 0;		// <<< 0: Disable Tx-Interrupt / 1: Enable Tx-Interrupt 
	IF1MCTR2_RXIE	= 0;		// <<< 0: Disable Rx-Interrupt / 1: Enable Rx-Interrupt 
	IF1MCTR2_RMTEN	= 0;		// <<< 0: Disable Remote / 1: Enable Remote 
	IF1MCTR2_TXRQST	= 0;		// Clear Transmission Request 
	IF1MCTR2_EOB	= 1;		// <<< 0: Buffer is part of FiFo / 1: Single message buffer  

	IF1CMSK2  = 0xF0;	// Prepare Interface Command Mask Register: 
						// WRRD    = 1 : Write information from interface to object
						// MASK    = 1 : Write Mask register
						// ARB     = 1 : Write Arbitration Register (ID Register)
						// CONTROL = 1 : Write object control register (e.g. Data length, IRQ enable, etc)
						// CIP     = 0 : (dont care for writing to object)
						// TXREQ   = 0 : No Transmission Request
						// DATAA   = 0 : Do not change Data A register
						// DATAB   = 0 : Do not change Data B register

	IF1CREQ2 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN_buffer_(U8 NumCan, U8 Num, U8 Dir, U16 setID, U32 mask)
{
	if(NumCan == 0)
		CAN0_buffer_(Num, Dir, setID, mask);
	else if(NumCan == 1)
		CAN1_buffer_(Num, Dir, setID, mask);
	else if(NumCan == 2)
		CAN2_buffer_(Num, Dir, setID, mask);
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN0_buffer_off(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB0_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR0_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR0_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR0_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR0_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK0  = 0xF0;

	IF1CREQ0 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
__near void CAN1_buffer_off(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB1_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR1_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR1_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR1_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR1_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK1  = 0xF0;

	IF1CREQ1 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
__near void CAN2_buffer_off(U8 Num)
{
	if((Num==0)||(Num>32)) return;
	
	IF1ARB2_MSGVAL	= 0;		// <<< 0: Buffer invalid / 1: Buffer valid 

	// Prepare Message Control Interface Register 
	IF1MCTR2_NEWDAT	= 0;		// Clear NEWDAT Flag 
	IF1MCTR2_MSGLST	= 0;		// Clear MSGLST Flag 
	IF1MCTR2_INTPND	= 0;		// Clear INTPND Flag 
	IF1MCTR2_TXRQST	= 0;		// Clear Transmission Request 

	IF1CMSK2  = 0xF0;

	IF1CREQ2 = Num;		// Transfer the Interface Register Content to the Message Buffer
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN_buffer_off(U8 NumCan, U8 Num)
{
	if(NumCan == 0)
		CAN0_buffer_off(Num);
	else if(NumCan == 1)
		CAN0_buffer_off(Num);
	else if(NumCan == 2)
		CAN0_buffer_off(Num);
}
//------------------------------------------------------------------------------------------------------------------
__near void CAN_ConfigMsgBox_(U8 NumCan)
{
	U8 i;
	
	for(i=0;i<31;i++) // первые 31 ящик работают на приём
	{
		if(i==0)
			CAN_buffer_(NumCan, i+1, 0, ID_BOOT, 0x7FF); // настраиваем один ящик на прием
		else
			CAN_buffer_off(NumCan, i+1);	// остальные ящики отключаем
	}
}
//------------------------------------------------------------------------------------------------------------------
__near void ResetCan_(U8 nCan)
{
	if(nCan == 0)
	{
		/*Restart bus*/
		CTRLR0_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT0_TEC!=0)||(ERRCNT0_REC!=0));//see if recovered
	}
	if(nCan == 1)
	{
		/*Restart bus*/
		CTRLR1_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT1_TEC!=0)||(ERRCNT1_REC!=0));//see if recovered
	}
	if(nCan == 2)
	{
		/*Restart bus*/
		CTRLR2_INIT = 0;		/*enable CAN controller*/
		//while((ERRCNT2_TEC!=0)||(ERRCNT2_REC!=0));//see if recovered
	}
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CanReciveMsg0(TMsgCan *msg)
{
	if(INTR0 == 0x8000)		/* status int */
	{
		if(STATR0_BOFF == 1)		//bus off
		{
			ResetCan_(0);
		}
		if(STATR0_EWARN==0x01)		//error warning
		{
			ResetCan_(0);
		}
		if(!((STATR0_BOFF)|(STATR0_EWARN)|(STATR0_EPASS))) //error active
		{
			ResetCan_(0);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT0 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK0	= 0x003F;	
		IF2CREQ0	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR0_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR0_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK0		= 0x0090;	//WRRD=1 CONTROL=1 other=0
										//for clear MSGLST
			IF2CREQ0 = 1;
		}
			
		msg->rtr		= IF2ARB0_DIR;
		msg->len		= IF2MCTR0_DLC;
		msg->cob_id		= STD2MSG_(IF2ARB0);
		*((U32 *)msg->data)     = IF2DTA0;
		*((U32 *)(msg->data+4)) = IF2DTB0;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CanReciveMsg1(TMsgCan *msg)
{
	if(INTR1 == 0x8000)		/* status int */
	{
		if(STATR1_BOFF == 1)		//bus off
		{
			ResetCan_(1);
		}
		if(STATR1_EWARN==0x01)		//error warning
		{
			ResetCan_(1);
		}
		if(!((STATR1_BOFF)|(STATR1_EWARN)|(STATR1_EPASS))) //error active
		{
			ResetCan_(1);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT1 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK1	= 0x003F;	
		IF2CREQ1	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR1_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR1_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK1		= 0x0090;	//WRRD=1 CONTROL=1 other=0
											//for clear MSGLST
			IF2CREQ1 = 1;
		}
			
		msg->rtr		= IF2ARB1_DIR;
		msg->len		= IF2MCTR1_DLC;
		msg->cob_id		= STD2MSG_(IF2ARB1);
		*((U32 *)msg->data)     = IF2DTA1;
		*((U32 *)(msg->data+4)) = IF2DTB1;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CanReciveMsg2(TMsgCan *msg)
{
	if(INTR2 == 0x8000)		/* status int */
	{
		if(STATR2_BOFF == 1)		//bus off
		{
			ResetCan_(2);
		}
		if(STATR2_EWARN==0x01)		//error warning
		{
			ResetCan_(2);
		}
		if(!((STATR2_BOFF)|(STATR2_EWARN)|(STATR2_EPASS))) //error active
		{
			ResetCan_(2);
		}
	}
	// Check whether the interrupt cause is receive or transmit
	if( (NEWDT2 & 1) != 0 ) // is a receive interrupt
	{
		IF2CMSK2	= 0x003F;	
		IF2CREQ2	= 1;		//transmit msgRAM to IF
			
		if(IF2MCTR2_MSGLST==0x01)
		{	// clear overflow
			__wait_nop();  //mag lost
			IF2MCTR2_MSGLST	= 0;		// Clear MSGLST Flag
			IF2CMSK2		= 0x0090;	//WRRD=1 CONTROL=1 other=0
										//for clear MSGLST
			IF2CREQ2 = 1;
		}
			
		msg->rtr		= IF2ARB2_DIR;
		msg->len		= IF2MCTR2_DLC;
		msg->cob_id		= STD2MSG_(IF2ARB2);
		*((U32 *)msg->data)     = IF2DTA2;
		*((U32 *)(msg->data+4)) = IF2DTB2;
		return TRUE;
	}
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CanReciveMsg(TMsgCan *msg)
{
	if(CAN_BUS == 0) return CanReciveMsg0(msg);
	if(CAN_BUS == 1) return CanReciveMsg1(msg);
	if(CAN_BUS == 2) return CanReciveMsg2(msg);
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CAN0_SendMessage_(TMsgCan *msg)
{
	U32 timeout=0;
	
	// Prepare Arbitration Interface Register 
	IF1ARB0 = MSG2STD_(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB0_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB0_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB0_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR0		=(msg->len&0xF) | (1<<7);

	IF1DTA0 = *((U32 *)(msg->data));
	IF1DTB0 = *((U32 *)(msg->data+4));

	IF1CMSK0  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR0 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR0_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR0_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK0  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR0_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK0_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ0 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CAN1_SendMessage_(TMsgCan *msg)
{
	U32 timeout=0;
	// Prepare Arbitration Interface Register 
	IF1ARB1 = MSG2STD_(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB1_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB1_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB1_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR1		=(msg->len&0xF) | (1<<7);

	IF1DTA1 = *((U32 *)(msg->data));
	IF1DTB1 = *((U32 *)(msg->data+4));

	IF1CMSK1  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR1 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR1_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR1_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK1  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR1_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK1_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ1 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CAN2_SendMessage_(TMsgCan *msg)
{
	U32 timeout=0;
	// Prepare Arbitration Interface Register 
	IF1ARB2 = MSG2STD_(msg->cob_id);		// <<< MSG2STD: 11bit ID / MSG2EXT: 29bit ID 
	IF1ARB2_XTD    = 0;			// <<< 0: 11bit ID / 1: 29bit ID 
	IF1ARB2_DIR    = 1;			// <<< 0: RX Buffer / 1: TX Buffer 
	IF1ARB2_MSGVAL = 1;			// <<< 0: Buffer invalid / 1: Buffer valid 

	IF1MCTR2		=(msg->len&0xF) | (1<<7);

	IF1DTA2 = *((U32 *)(msg->data));
	IF1DTB2 = *((U32 *)(msg->data+4));

	IF1CMSK2  = 0xF7;			// Prepare Interface Command Mask Register: 

	IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer 

	// Wait for transmission of message
	while(((TREQR2 & 0x80000000) != 0) && (timeout++ < 0xFFFF) && (CTRLR2_INIT != 1))
	{
		__wait_nop();
	}
	if( (timeout == 0xFFFF) || (CTRLR2_INIT == 1) )
	{
		// Cancel transmission request 
		IF1CMSK2  = 0x10;	// Prepare Interface Command Mask Register: Load only control data from message object
		
		IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer 
		IF1MCTR2_TXRQST = 0;// Clear Transmission Request 
		IF1CMSK2_WRRD = 1;	// Prepare Interface Command Mask Register
		IF1CREQ2 = 32;		// Transfer the Interface Register Content to the Message Buffer
		return FALSE;		// return error
	}
	return TRUE;
}
//------------------------------------------------------------------------------------------------------------------
__near U8 CanSendMsg(TMsgCan *msg)
{
	if(CAN_BUS == 0) return CAN0_SendMessage_(msg);
	if(CAN_BUS == 1) return CAN1_SendMessage_(msg);
	if(CAN_BUS == 2) return CAN2_SendMessage_(msg);
	return FALSE;
}
//------------------------------------------------------------------------------------------------------------------
__near void SendCanBuf(char_t *buf, U8 len)
{
	TMsgCan msg;
	U8 i;
	
	msg.rtr = 0;
	msg.cob_id = ID_BOOT;
	
	if(len>0)
	{
		while(len>0)
		{
			clrwdt_;	// обнуление WatchDog таймера 
			
			if(len>8) msg.len = 8;
			else msg.len = len;
			
			len -= msg.len;
			
			for(i=0; i<msg.len; i++)
				msg.data[i] = *buf++;
				
			CanSendMsg(&msg);
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
__far void ServiceBootloadCanStart(BYTE bus_id)
{
	
	TMsgCan msg;
	U8 i;
	/* =============================================================== */
	/*    НАСТРАИВАЕМ CAN ДЛЯ ПРОГРАММАТОРА                            */
	/* =============================================================== */
	ID_BOOT	= ADDR|(((WORD)0xD)<<7);
	CAN_BUS = bus_id;
	CAN_ConfigMsgBox_(bus_id);	// Конфигурим наш CAN на прием сообщений с нашим адресом .. младшие 5 бит адресс остальные нули
	/* =============================================================== */
	/* =============================================================== */
	/* Enable Sectors for FLASH writing and erasing                    */
	/* =============================================================== */
	FMWC1 = 0xFF;       // enable sector for FLASH writing
	FMWC5 = 0xFF;       // enable sector for FLASH writing 
	/* =============================================================== */
	
	SendCanBuf(STR_OK, 3);		// подтверждаем что загрузчик загрузился
	
	while(1)// принимаем сообщения 
	{
		clrwdt_;					// обнуление WatchDog таймера 
		//----------------------------------------------------
		if(CanReciveMsg(&msg) == TRUE)	
		{// приняли сообщение, обрабатываем
			if(msg.cob_id == ID_BOOT)
			{// обрабатываем только наши пакеты, хотя они и так фильтруются на входе
				
				for(i=0; i<msg.len; i++)
				{
					if (!cmd)
					{
						build_cmd_b(msg.data[i]);
					}
					// perform the command
					if (cmd)
					{
						ObrCmd(TYPE_CAN);
						
						// Reset vars.
						cmd_type = CMD_NONE;
						cmd = FALSE;
					}
				}
			}
		}
		//----------------------------------------------------
	}
	//return 0;
}

//------------------------------------------------------------------------------------------------------------------

//копирование всего кода bootloader.c в RAM
void CopyLoaderToRam(void)
{
int i;
__far unsigned int *src=(__far unsigned int *)BOOTLOADER_CODE_ADDR; 
__far volatile unsigned int *dst=(__far unsigned int *)((unsigned long)(BOOTLOADER_CODE_ADDR & 0xFFFF));
	__DI();	//запрещяем все прерывания
	LEDR_ON;
	LEDG_ON;
	clrwdt_;	
	for (i=0;i<(BOOTLOADER_CODE_SIZE >> 1);i++)
		*(dst++)=*(src++);
	clrwdt_;	
	LEDR_ON;
	LEDG_OFF;
}


typedef __far void can_start(BYTE bus_id);

// CAN Open код функции 0xD
unsigned char ServiceBootloadCan(BYTE bus_id,TMsgCan *m)
{
static BYTE bus_id_saved=0;
can_start *f;
	// проверяем сообщение здесь, m может быть затерто при копировании кода в RAM 
	if(m->len != 8) return 0;
	if(m->data[0] != 'B') return 0;
	if(m->data[1] != 'O') return 0;
	if(m->data[2] != 'O') return 0;
	if(m->data[3] != 'T') return 0;
	if(m->data[4] != 'L') return 0;
	if(m->data[5] != 'O') return 0;
	if(m->data[6] != 'A') return 0;
	if(m->data[7] != 'D') return 0;
	bus_id_saved=bus_id;
	set_stack();	//брoсаем старый стек и переходим на новый
	CopyLoaderToRam();
	f=(can_start*)((unsigned long)ServiceBootloadCanStart & 0xFFFF);	//обрезали верхнюю часть адреса 
	f(bus_id_saved);
	return 1;
}

typedef __far void uart_start(void);

void ServiceBootloadUart(void)
{
uart_start *f;
	set_stack();	//брoсаем старый стек и переходим на новый
	CopyLoaderToRam();
	f=(uart_start*)((unsigned long)ServiceBootloadUartStart & 0xFFFF);	//обрезали верхнюю часть адреса 
	f();	//вызов ServiceBootloadUartStart из RAM
}

/**************** FLASH ROUTINES *****************/

#define DQ7 0x0080	// data polling flag
#define DQ5 0x0020	// time limit exceeding flag
#define DQ3 0x0008	//sector erase timer flag

#define far_vuint	__far volatile unsigned int
#define far_uint	__far unsigned int
//------------------------------------------------------------------------------------------------------------------
__near void fwait(unsigned long a)
{
	unsigned long i;
	
	for (i = 0; i < a; i++)
	{
		__wait_nop();
	}
}

// Erases Flash sector
__near unsigned char erase(__far unsigned int *sector_adr)
{
	unsigned char flag = 0;
	unsigned char MCSRA_save;
	unsigned int  MTCRA_save;
	__far volatile unsigned int *seq_AAAA;
	__far volatile unsigned int *seq_5554;
	seq_AAAA = (far_uint*)(((unsigned long)sector_adr & 0x0FFF000) + 0x0AAA);
	seq_5554 = (far_uint*)(((unsigned long)sector_adr & 0x0FFF000) + 0x0554);
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
	LEDG=~LEDG;	
	return(flag);
}

__near unsigned int RamRead(unsigned long adr)
{
	unsigned int y;	
	y = (*(unsigned int*) adr);
	return y;
}


// Write word data to flash
__near unsigned char write(__far unsigned int *adr, unsigned int wdata)
{
	unsigned char flag = 0;
	unsigned char MCSRA_save;
	unsigned int  MTCRA_save;
	__far volatile unsigned int *seq_AAAA;
	__far volatile unsigned int *seq_5554;
	
	seq_AAAA = (far_uint*)(((unsigned long)adr & 0x0FFF000) + 0x0AAA);
	seq_5554 = (far_uint*)(((unsigned long)adr & 0x0FFF000) + 0x0554);
	
	// preparations
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
	LEDG=~LEDG;
	return(flag);
}

