/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "mb96338us.h"
#include "global.h"
#include "ocu.h"

/** V A R I A B L E S **********************************************/

void InitFreeTimer16_0(void) 
{
	TCCSL0_CLK  = 0x00; /* 000 clk, 001 clk/2, 010 clk/4 011 clk/8              */
	TCCSL0_CLR  = 0;    /* 0 no effect, 1 clear counter                           */
	TCCSL0_MODE = 1;    /* 0 init by reset or clear bit                           */
                      /* 1 init by reset, clear or match of compare register 0  */
	TCCSL0_STOP = 0;    /* 0 counter enable, 1 counter disable                    */
	TCCSL0_IVFE = 0;    /* 0 interrupt disable, 1 interrupt enable                */
	TCCSL0_IVF  = 0;    /* 0 no irq, 1 irq (overflow or compare) pending          */ 

	/* clear counter */
	TCDT0 = 0x0000; 
}

void InitOCU_01(WORD ocp1, WORD ocp2) 
{
	/*-----------  Initialize Output Compare Unit 0/1  ------------*/

	OCCP0 = ocp1; /* Compare value CR0 */
	OCCP1 = ocp2; /* Compare value CR1 */

	OCS0_ICP0 = 0; /* clear output compare match flag */
	OCS0_ICP1 = 0; 
	
	OCS1_OTD0 = 0; /* output compare pin output level definition */
	OCS1_OTD1 = 0;  
	
	OCS1_OTE0 = 1; /* enable Output compare pin output */  
	OCS1_OTE1 = 0;  
	
	OCS1_CMOD0 = 1; /* reverse output level */
	OCS1_CMOD1 = 1; /* reverse output level */
	
	OCS0_ICE0 = 0; /* enable output compare interrupt request */
	OCS0_ICE1 = 0; 
	
	OCS0_CST0 = 1; /* enable comparision for CR0/CR1 */
	OCS0_CST1 = 1;  
}

//*******************************************************************
