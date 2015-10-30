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
 */

#include "fifo.h"

// size должно быть кратно 128

void fifo_init(fifo_desc_t *fifo_desc, void *buffer, WORD size)
{
	// Check the size parameter. It must be a 2-power.
	//Assert ((!size) || (size & (size - 1)));

	// ... and must fit in a uint8_t. Since the read and write indexes are using a
	// double-index range implementation, the max FIFO size is thus 128 items.
	//Assert (size > 128);

	// Fifo starts empty.
	fifo_desc->read_index  = 0;
	fifo_desc->write_index = 0;

	// Save the size parameter.
	fifo_desc->size = size;

	// Create a mask to speed up the FIFO management (index swapping).
	fifo_desc->mask = (2 * (WORD)size) - 1;

	// Save the buffer pointer.
	fifo_desc->buffer.u8ptr = buffer;
}

 /**
 *  \brief Returns the number of elements in the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return The number of used elements.
 */
uint8_t fifo_get_used_size(fifo_desc_t *fifo_desc)
{
	return ((fifo_desc->write_index - fifo_desc->read_index) & fifo_desc->mask);
}
#pragma inline fifo_get_used_size
/**
 *  \brief Returns the remaining free spaces of the FIFO (in number of elements).
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return The number of free elements.
 */
uint8_t fifo_get_free_size(fifo_desc_t *fifo_desc)
{
	return fifo_desc->size - fifo_get_used_size(fifo_desc);
}
#pragma inline fifo_get_free_size

/**
 *  \brief Tests if a FIFO is empty.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return Status
 *    \retval true when the FIFO is empty.
 *    \retval false when the FIFO is not empty.
 */
uint8_t fifo_is_empty(fifo_desc_t *fifo_desc)
{
	return (fifo_desc->write_index == fifo_desc->read_index);
}
#pragma inline fifo_is_empty
/**
 *  \brief Tests if a FIFO is full.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return Status
 *    \retval true when the FIFO is full.
 *    \retval false when the FIFO is not full.
 */
uint8_t fifo_is_full(fifo_desc_t *fifo_desc)
{
	return (fifo_get_used_size(fifo_desc) == fifo_desc->size);
}
#pragma inline fifo_is_full

/**
 *  \brief Puts a new 8-bits element into the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
void fifo_push_uint8_nocheck(fifo_desc_t *fifo_desc, uint8_t item)
{
	uint16_t write_index;

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u8ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;
}
#pragma inline fifo_push_uint8_nocheck

/**
 *  \brief Puts a new 8-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_push_uint8(fifo_desc_t *fifo_desc, uint8_t item)
{
	uint16_t write_index;

	if (fifo_is_full(fifo_desc)) {
		return FIFO_ERROR_OVERFLOW;
	}

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u8ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;

	return FIFO_OK;
}
#pragma inline fifo_push_uint8

/**
 *  \brief Puts a new 16-bits element into the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
void fifo_push_uint16_nocheck(fifo_desc_t *fifo_desc, uint16_t item)
{
	uint16_t write_index;

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u16ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;
}
#pragma inline fifo_push_uint16_nocheck

/**
 *  \brief Puts a new 16-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_push_uint16(fifo_desc_t *fifo_desc, uint16_t item)
{
	uint16_t write_index;

	if (fifo_is_full(fifo_desc)) {
		return FIFO_ERROR_OVERFLOW;
	}

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u16ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;

	return FIFO_OK;
}
#pragma inline fifo_push_uint16

/**
 *  \brief Puts a new 32-bits element into the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 */
void fifo_push_uint32_nocheck(fifo_desc_t *fifo_desc, uint32_t item)
{
	uint16_t write_index;

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u32ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;
}
#pragma inline fifo_push_uint32_nocheck

/**
 *  \brief Puts a new 32-bits element into the FIFO and
 *         checks for a possible overflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_push_uint32(fifo_desc_t *fifo_desc, uint32_t item)
{
	uint16_t write_index;

	if (fifo_is_full(fifo_desc)) {
		return FIFO_ERROR_OVERFLOW;
	}

	write_index = fifo_desc->write_index;
	fifo_desc->buffer.u32ptr[write_index & (fifo_desc->mask >> 1)] = item;
	write_index = (write_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->write_index = write_index;

	return FIFO_OK;
}
#pragma inline fifo_push_uint32

/**
 *  \brief Gets a 8-bits element from the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
uint8_t fifo_pull_uint8_nocheck(fifo_desc_t *fifo_desc)
{
	uint16_t read_index;
	BYTE item;

	read_index = fifo_desc->read_index;
	item = fifo_desc->buffer.u8ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return item;
}
#pragma inline fifo_pull_uint8_nocheck

/**
 *  \brief Gets a 8-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_pull_uint8(fifo_desc_t *fifo_desc, uint8_t *item)
{
	uint16_t read_index;

	if (fifo_is_empty(fifo_desc)) {
		return FIFO_ERROR_UNDERFLOW;
	}

	read_index = fifo_desc->read_index;
	*item = fifo_desc->buffer.u8ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return FIFO_OK;
}

#pragma inline fifo_pull_uint8

/**
 *  \brief Gets a 16-bits element from the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
uint16_t fifo_pull_uint16_nocheck(fifo_desc_t *fifo_desc)
{
	uint16_t read_index;
	uint16_t item;

	read_index = fifo_desc->read_index;
	item = fifo_desc->buffer.u16ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return item;
}

#pragma inline fifo_pull_uint16_nocheck
/**
 *  \brief Gets a 16-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_pull_uint16(fifo_desc_t *fifo_desc, uint16_t *item)
{
	uint16_t read_index;

	if (fifo_is_empty(fifo_desc)) {
		return FIFO_ERROR_UNDERFLOW;
	}

	read_index = fifo_desc->read_index;
	*item = fifo_desc->buffer.u16ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return FIFO_OK;
}

#pragma inline fifo_pull_uint16
/**
 *  \brief Gets a 32-bits element from the FIFO
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \return extracted element.
 */
uint32_t fifo_pull_uint32_nocheck(fifo_desc_t *fifo_desc)
{
	uint16_t read_index;
	uint32_t item;

	read_index = fifo_desc->read_index;
	item = fifo_desc->buffer.u32ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return item;
}

#pragma inline fifo_pull_uint32_nocheck
/**
 *  \brief Gets a 32-bits element from the FIFO and
 *         checks for a possible underflow.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *  \param item       extracted element.
 *
 *  \return Status
 *    \retval FIFO_OK when no error occurred.
 *    \retval FIFO_ERROR_UNDERFLOW when the FIFO was empty.
 */
int8_t fifo_pull_uint32(fifo_desc_t *fifo_desc, uint32_t *item)
{
	uint16_t read_index;

	if (fifo_is_empty(fifo_desc)) {
		return FIFO_ERROR_UNDERFLOW;
	}

	read_index = fifo_desc->read_index;
	*item = fifo_desc->buffer.u32ptr[read_index & (fifo_desc->mask >> 1)];
	read_index = (read_index + 1) & fifo_desc->mask;

	// Must be the last thing to do.
	//barrier();
	fifo_desc->read_index = read_index;

	return FIFO_OK;
}

#pragma inline fifo_pull_uint32
/**
 *  \brief Gets a 32-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
uint32_t fifo_peek_uint32(fifo_desc_t *fifo_desc)
{
	return fifo_desc->buffer.u32ptr[fifo_desc->read_index & (fifo_desc->mask >> 1)];
}

#pragma inline fifo_peek_uint32
/**
 *  \brief Gets a 16-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
uint16_t fifo_peek_uint16(fifo_desc_t *fifo_desc)
{
	return fifo_desc->buffer.u16ptr[fifo_desc->read_index & (fifo_desc->mask >> 1)];
}

#pragma inline fifo_peek_uint16
/**
 *  \brief Gets a 8-bits element from the FIFO but does
 *         not remove it from the FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 *
 *  \retval item      extracted element.
 */
uint8_t fifo_peek_uint8(fifo_desc_t *fifo_desc)
{
	return fifo_desc->buffer.u8ptr[fifo_desc->read_index & (fifo_desc->mask >> 1)];
}

#pragma inline fifo_peek_uint8
/**
 *  \brief Flushes a software FIFO.
 *
 *  \param fifo_desc  The FIFO descriptor.
 */
void fifo_flush(fifo_desc_t *fifo_desc)
{
	// Fifo starts empty.
	fifo_desc->read_index = fifo_desc->write_index = 0;
}
#pragma inline fifo_flush

/**
 * @}
 */
