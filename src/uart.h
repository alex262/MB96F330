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
/** \file uart.h
 **
 ** - updated uart module to the golden coding rules 
 **
 ** History:
 **   - 2010-03-30    1.0  MSc  First version  (works with 16FX)
 *****************************************************************************/

#ifndef _UART_H
#define _UART_H

#include "global.h"

void InitUART(BYTE ch);
void UART_RX_IntSet(BYTE ch, BYTE st);
void UART_TX_IntSet(BYTE ch, BYTE st);

void receive_line(void);
int16_t receive_line_echo(uint16_t *cnt);
int16_t  scan_line(char_t *str); 
uint32_t Inputhex(uint8_t digits);
uint32_t ASCIItobin(uint8_t k);
void puthex(uint32_t n, uint8_t digits);
void putbytehex(uint8_t n);
//void putdec(uint32_t x);
void putdec(uint32_t x, uint8_t len);
char_t getch(void);
void putch(char_t ch);
void puts(char_t *buf);
char_t getkey(char_t LKey, char_t HKey);
char_t upcase(char_t k);

__interrupt void irq_uart0_rx(void);
__interrupt void irq_uart0_tx(void);

__interrupt void irq_uart1_rx(void);
__interrupt void irq_uart2_rx(void);
__interrupt void irq_uart3_rx(void);
__interrupt void irq_uart5_rx(void);
__interrupt void irq_uart7_rx(void);
__interrupt void irq_uart8_rx(void);
__interrupt void irq_uart9_rx(void);

#endif /* UART_H */