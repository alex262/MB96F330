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
void SetUartTRD_Reg(BYTE ch, BYTE data);
void SetUartESIR_Reg(BYTE ch, BYTE data);
WORD GetUartRDR_addr(BYTE ch);
void ClearErrorUart(BYTE ch);
void UartESIR_RDRF_Clear(BYTE ch);
WORD GetUartTDR_addr(BYTE ch);


void receive_line(void);
int16_t receive_line_echo(uint16_t *cnt);
int16_t  scan_line(BYTE *str); 
uint32_t Inputhex(uint8_t digits);
uint32_t ASCIItobin(uint8_t k);
void puthex(uint32_t n, uint8_t digits);
void putbytehex(uint8_t n);
//void putdec(uint32_t x);
void putdec(uint32_t x, uint8_t len);
BYTE getch(void);
void putch(BYTE ch);
void puts(BYTE *buf);
BYTE getkey(BYTE LKey, BYTE HKey);
BYTE upcase(BYTE k);
void puts_bin_byte(uint8_t data);
void puts_bin_word(uint16_t data);
BYTE GetNumRxIRQUart(BYTE ch);
BYTE GetNumTxIRQUart(BYTE ch);
void UartESIR_TDRE_Clear(BYTE ch);
void DrawUartReg(BYTE ch);
void SetUartSMR_UPCL(BYTE ch, BYTE data);
void UART_ESIR_SSR_TDRE(BYTE ch);


__interrupt void irq_uart0_rx(void);
__interrupt void irq_uart1_rx(void);
__interrupt void irq_uart2_rx(void);
__interrupt void irq_uart3_rx(void);
__interrupt void irq_uart5_rx(void);
__interrupt void irq_uart7_rx(void);
__interrupt void irq_uart8_rx(void);
__interrupt void irq_uart9_rx(void);

__interrupt void irq_uart0_tx(void);
__interrupt void irq_uart1_tx(void);
__interrupt void irq_uart2_tx(void);
__interrupt void irq_uart3_tx(void);
__interrupt void irq_uart5_tx(void);
__interrupt void irq_uart7_tx(void);
__interrupt void irq_uart8_tx(void);
__interrupt void irq_uart9_tx(void);

#endif /* UART_H */