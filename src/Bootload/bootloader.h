//bootloader.h
#ifndef	_H_BOOTLOADER
#define _H_BOOTLOADER

typedef struct
{
	unsigned long cob_id;
	unsigned char rtr;
	unsigned char len;		// message length (0 to 8)
	unsigned char data[8];	// data 
}TMsgCan;

extern unsigned char ServiceBootloadUpd(unsigned char bus_id, TMsgCan *m);
extern void ServiceBootloadUart(void);

extern const char START_BOOT[];

#endif
