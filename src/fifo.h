/**
 * \file
 *
 * \brief This file controls the software FIFO management.
 *
 * These functions manages FIFOs thanks to simple a API. The FIFO can
 * be 100% full thanks to a double-index range implementation. For example,
 * a FIFO of 4 elements can be implemented: the FIFO can really hold up to 4
 * elements.
 * This is particurly well suited for any kind of application needing a lot of
 * small FIFO.
 *
 */

#ifndef _FIFO_H_
#define _FIFO_H_

#include "global.h"

/**
 * \defgroup fifo_group First-In-First-Out Buffer (FIFO)
 *
 * These functions manages FIFOs thanks to simple a API. The FIFO can
 * be 100% full thanks to a double-index range implementation. For example,
 * a FIFO of 4 elements can be implemented: the FIFO can really hold up to 4
 * elements. This is particurly well suited for any kind of application
 * needing a lot of small FIFO. The maximum fifo size is 128 items (uint8,
 * uint16 or uint32). Note that the driver, thanks to its conception, does
 * not use interrupt protection.
 *
 * @{
 */

//! Error codes used by FIFO driver.
enum {
	FIFO_OK = 0,          //!< Normal operation.
	FIFO_ERROR_OVERFLOW,  //!< Attempt to push something in a FIFO that is full.
	FIFO_ERROR_UNDERFLOW, //!< Attempt to pull something from a FIFO that is empty
	FIFO_ERROR,           //!< Error condition during FIFO initialization
};

//! FIFO descriptor used by FIFO driver.
struct fifo_desc {
	union
	{
		uint32_t	*u32ptr; //!< Pointer to unsigned-32 bits location
		uint16_t	*u16ptr; //!< Pointer to unsigned-16 bits location
		uint8_t		*u8ptr;  //!< Pointer to unsigned-8 bits location
	}  buffer;
	volatile uint16_t read_index;  //!< Read index
	volatile uint16_t write_index; //!< Write index
	uint16_t size;                 //!< Size of the FIFO (unit is in number of 'element')
	uint16_t mask;                 //!< Mask used to speed up FIFO operation (wrapping)
};

typedef struct fifo_desc fifo_desc_t;

void fifo_init(fifo_desc_t *fifo_desc, void *buffer, uint16_t size);
uint8_t fifo_get_used_size(fifo_desc_t *fifo_desc);
uint8_t fifo_get_free_size(fifo_desc_t *fifo_desc);
uint8_t fifo_is_empty(fifo_desc_t *fifo_desc);
uint8_t fifo_is_full(fifo_desc_t *fifo_desc);
void fifo_push_uint8_nocheck(fifo_desc_t *fifo_desc, uint8_t item);
int8_t fifo_push_uint8(fifo_desc_t *fifo_desc, uint8_t item);
void fifo_push_uint16_nocheck(fifo_desc_t *fifo_desc, uint16_t item);
int8_t fifo_push_uint16(fifo_desc_t *fifo_desc, uint16_t item);
void fifo_push_uint32_nocheck(fifo_desc_t *fifo_desc, uint32_t item);
int8_t fifo_push_uint32(fifo_desc_t *fifo_desc, uint32_t item);
uint8_t fifo_pull_uint8_nocheck(fifo_desc_t *fifo_desc);
int8_t fifo_pull_uint8(fifo_desc_t *fifo_desc, uint8_t *item);
uint16_t fifo_pull_uint16_nocheck(fifo_desc_t *fifo_desc);
int8_t fifo_pull_uint16(fifo_desc_t *fifo_desc, uint16_t *item);
uint32_t fifo_pull_uint32_nocheck(fifo_desc_t *fifo_desc);
int8_t fifo_pull_uint32(fifo_desc_t *fifo_desc, uint32_t *item);
uint32_t fifo_peek_uint32(fifo_desc_t *fifo_desc);
uint16_t fifo_peek_uint16(fifo_desc_t *fifo_desc);
uint8_t fifo_peek_uint8(fifo_desc_t *fifo_desc);
void fifo_flush(fifo_desc_t *fifo_desc);


/*uint8_t fifo_get_used_size(fifo_desc_t *fifo_desc);
uint8_t fifo_get_free_size(fifo_desc_t *fifo_desc);
uint8_t fifo_is_empty(fifo_desc_t *fifo_desc);
uint8_t fifo_is_full(fifo_desc_t *fifo_desc);
void fifo_push_uint8_nocheck(fifo_desc_t *fifo_desc, uint8_t item);
int8_t fifo_push_uint8(fifo_desc_t *fifo_desc, uint8_t item);
void fifo_push_uint16_nocheck(fifo_desc_t *fifo_desc, uint16_t item);
 int8_t fifo_push_uint16(fifo_desc_t *fifo_desc, uint16_t item);
 void fifo_push_uint32_nocheck(fifo_desc_t *fifo_desc, uint32_t item);
 int8_t fifo_push_uint32(fifo_desc_t *fifo_desc, uint32_t item);
 uint8_t fifo_pull_uint8_nocheck(fifo_desc_t *fifo_desc);
 int8_t fifo_pull_uint8(fifo_desc_t *fifo_desc, uint8_t *item);
 uint16_t fifo_pull_uint16_nocheck(fifo_desc_t *fifo_desc);
 int8_t fifo_pull_uint16(fifo_desc_t *fifo_desc, uint16_t *item);
 uint32_t fifo_pull_uint32_nocheck(fifo_desc_t *fifo_desc);
 int8_t fifo_pull_uint32(fifo_desc_t *fifo_desc, uint32_t *item);
 uint32_t fifo_peek_uint32(fifo_desc_t *fifo_desc);
 uint16_t fifo_peek_uint16(fifo_desc_t *fifo_desc);
 uint8_t fifo_peek_uint8(fifo_desc_t *fifo_desc);
 void fifo_flush(fifo_desc_t *fifo_desc);*/


#endif  // _FIFO_H_
