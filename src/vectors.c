/************************************************************************/
/*               (C) Fujitsu Semiconductor Europe GmbH (FSEU)           */
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
/* FSEU does not warrant that the deliverables do not infringe any      */
/* third party intellectual property right (IPR). In the event that     */
/* the deliverables infringe a third party IPR it is the sole           */
/* responsibility of the customer to obtain necessary licenses to       */
/* continue the usage of the deliverable.                               */
/*                                                                      */
/* To the maximum extent permitted by applicable law FSEU disclaims all */
/* warranties, whether express or implied, in particular, but not       */
/* limited to, warranties of merchantability and fitness for a          */
/* particular purpose for which the deliverable is not designated.      */
/*                                                                      */
/* To the maximum extent permitted by applicable law, FSEU’s liability  */
/* is restricted to intentional misconduct and gross negligence.        */
/* FSEU is not liable for consequential damages.                        */
/*                                                                      */
/* (V1.5)                                                               */
/************************************************************************/

/* ------------------------------------------------------------------------- */
/* VECTORS.C                                                                 */
/* - Interrupt level (priority) setting                                      */
/* - Interrupt vector definition                                             */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/* 'Id:: vectors.c 7277 2011-05-30 10:11:11Z rlande                        ' */
/* ------------------------------------------------------------------------- */
/* History:                                                                  */
/* Date        Version  Author  Description                                  */
/* 2009-05-06  2.0      RLa     Started with versioning.                     */
/* ------------------------------------------------------------------------- */

#include "mb96338us.h"
#include "rlt.h"
#include "uart.h"
#include "can.h"

/*---------------------------------------------------------------------------
   InitIrqLevels()
   This function  pre-sets all interrupt control registers. It can be used
   to set all interrupt priorities in static applications. If this file
   contains assignments to dedicated resources, verify  that the
   appropriate controller is used.
   NOTE: value 7 disables the interrupt and value 0 sets highest priority.
-----------------------------------------------------------------------------*/

#define MIN_ICR  12
#define MAX_ICR  122

#define DEFAULT_ILM_MASK 7

void InitIrqLevels(void)
{
	volatile int irq;

	for (irq = MIN_ICR; irq <= MAX_ICR; irq++) 
	{
		ICR = (irq << 8) | DEFAULT_ILM_MASK;
	}
	//----------------------------------
	// enable the timer interrupt
	ICR = (56 << 8) | 2;	// RLT0
	//----------------------------------
	// UART
	ICR = (94 << 8) | 3;	// UART0 RX
	ICR = (95 << 8) | 3;	// UART0 TX
	//----------------------------------
	// CAN
	ICR = (33 << 8) | 4;	// CAN0
	ICR = (34 << 8) | 4;	// CAN1
	ICR = (35 << 8) | 4;	// CAN2
	//----------------------------------
}

/*---------------------------------------------------------------------------
   Prototypes
   Add your own prototypes here. Each vector definition needs is proto-
   type. Either do it here or include a header file containing them.
-----------------------------------------------------------------------------*/

__interrupt void DefaultIRQHandler (void);

/*---------------------------------------------------------------------------
   Vector definiton for MB9633x
   Use following statements to define vectors. All resource related
   vectors are predefined. Remaining software interrupts can be added here
   as well.
   NOTE: If software interrupts 0 to 7 are defined here, this might 
   conflict with the reset vector in the start-up file.
-----------------------------------------------------------------------------*/

#pragma intvect DefaultIRQHandler 11   /* Non-maskable Interrupt       */
#pragma intvect DefaultIRQHandler 12   /* Delayed Interrupt            */
#pragma intvect DefaultIRQHandler 13   /* RC Timer                     */
#pragma intvect DefaultIRQHandler 14   /* Main Clock Timer             */
#pragma intvect DefaultIRQHandler 15   /* Sub Clock Timer              */
#pragma intvect DefaultIRQHandler 16   /* Reserved                     */
#pragma intvect DefaultIRQHandler 17   /* EXT0                         */
#pragma intvect DefaultIRQHandler 18   /* EXT1                         */
#pragma intvect DefaultIRQHandler 19   /* EXT2                         */
#pragma intvect DefaultIRQHandler 20   /* EXT3                         */
#pragma intvect DefaultIRQHandler 21   /* EXT4                         */
#pragma intvect DefaultIRQHandler 22   /* EXT5                         */
#pragma intvect DefaultIRQHandler 23   /* EXT6                         */
#pragma intvect DefaultIRQHandler 24   /* EXT7                         */
#pragma intvect DefaultIRQHandler 25   /* EXT8                         */
#pragma intvect DefaultIRQHandler 26   /* EXT9                         */
#pragma intvect DefaultIRQHandler 27   /* EXT10                        */
#pragma intvect DefaultIRQHandler 28   /* EXT11                        */
#pragma intvect DefaultIRQHandler 29   /* EXT12                        */
#pragma intvect DefaultIRQHandler 30   /* EXT13                        */
#pragma intvect DefaultIRQHandler 31   /* EXT14                        */
#pragma intvect DefaultIRQHandler 32   /* EXT15                        */
#pragma intvect CAN_0_IRQ         33   /* CAN0                         */
#pragma intvect CAN_1_IRQ         34   /* CAN1                         */
#pragma intvect CAN_2_IRQ         35   /* CAN2                         */
#pragma intvect DefaultIRQHandler 36   /* PPG0                         */
#pragma intvect DefaultIRQHandler 37   /* PPG1                         */
#pragma intvect DefaultIRQHandler 38   /* PPG2                         */
#pragma intvect DefaultIRQHandler 39   /* PPG3                         */
#pragma intvect DefaultIRQHandler 40   /* PPG4                         */
#pragma intvect DefaultIRQHandler 41   /* PPG5                         */
#pragma intvect DefaultIRQHandler 42   /* PPG6                         */
#pragma intvect DefaultIRQHandler 43   /* PPG7                         */
#pragma intvect DefaultIRQHandler 44   /* PPG8                         */
#pragma intvect DefaultIRQHandler 45   /* PPG9                         */
#pragma intvect DefaultIRQHandler 46   /* PPG10                        */
#pragma intvect DefaultIRQHandler 47   /* PPG11                        */
#pragma intvect DefaultIRQHandler 48   /* PPG12                        */
#pragma intvect DefaultIRQHandler 49   /* PPG13                        */
#pragma intvect DefaultIRQHandler 50   /* PPG14                        */
#pragma intvect DefaultIRQHandler 51   /* PPG15                        */
#pragma intvect DefaultIRQHandler 52   /* PPG16                        */
#pragma intvect DefaultIRQHandler 53   /* PPG17                        */
#pragma intvect DefaultIRQHandler 54   /* PPG18                        */
#pragma intvect DefaultIRQHandler 55   /* PPG19                        */
#pragma intvect ReloadTimer0	  56   /* RLT0                         */
#pragma intvect DefaultIRQHandler 57   /* RLT1                         */
#pragma intvect DefaultIRQHandler 58   /* RLT2                         */
#pragma intvect DefaultIRQHandler 59   /* RLT3                         */
#pragma intvect DefaultIRQHandler 60   /* RLT6-PPG                     */
#pragma intvect DefaultIRQHandler 61   /* ICU0                         */
#pragma intvect DefaultIRQHandler 62   /* ICU1                         */
#pragma intvect DefaultIRQHandler 63   /* ICU2                         */
#pragma intvect DefaultIRQHandler 64   /* ICU3                         */
#pragma intvect DefaultIRQHandler 65   /* ICU4                         */
#pragma intvect DefaultIRQHandler 66   /* ICU5                         */
#pragma intvect DefaultIRQHandler 67   /* ICU6                         */
#pragma intvect DefaultIRQHandler 68   /* ICU7                         */
#pragma intvect DefaultIRQHandler 69   /* ICU8                         */
#pragma intvect DefaultIRQHandler 70   /* ICU9                         */
#pragma intvect DefaultIRQHandler 71   /* OCU0                         */
#pragma intvect DefaultIRQHandler 72   /* OCU1                         */
#pragma intvect DefaultIRQHandler 73   /* OCU2                         */
#pragma intvect DefaultIRQHandler 74   /* OCU3                         */
#pragma intvect DefaultIRQHandler 75   /* OCU4                         */
#pragma intvect DefaultIRQHandler 76   /* OCU5                         */
#pragma intvect DefaultIRQHandler 77   /* OCU6                         */
#pragma intvect DefaultIRQHandler 78   /* OCU7                         */
#pragma intvect DefaultIRQHandler 79   /* OCU8                         */
#pragma intvect DefaultIRQHandler 80   /* OCU9                         */
#pragma intvect DefaultIRQHandler 81   /* OCU10                        */
#pragma intvect DefaultIRQHandler 82   /* OCU11                        */
#pragma intvect DefaultIRQHandler 83   /* FRT0                         */
#pragma intvect DefaultIRQHandler 84   /* FRT1                         */
#pragma intvect DefaultIRQHandler 85   /* FRT2                         */
#pragma intvect DefaultIRQHandler 86   /* FRT3                         */
#pragma intvect DefaultIRQHandler 87   /* Real Time Clock 0            */
#pragma intvect DefaultIRQHandler 88   /* Clock Calibration Unit 0     */
#pragma intvect DefaultIRQHandler 89   /* I2C0                         */
#pragma intvect DefaultIRQHandler 90   /* I2C1                         */
#pragma intvect DefaultIRQHandler 91   /* ADC0                         */
#pragma intvect DefaultIRQHandler 92   /* ALARM0                       */
#pragma intvect DefaultIRQHandler 93   /* ALARM1                       */
#pragma intvect irq_uart0_rx	  94   /* LIN-UART 0 RX                */
#pragma intvect irq_uart0_tx	  95   /* LIN-UART 0 TX                */
#pragma intvect DefaultIRQHandler 96   /* LIN-UART 1 RX                */
#pragma intvect DefaultIRQHandler 97   /* LIN-UART 1 TX                */
#pragma intvect DefaultIRQHandler 98   /* LIN-UART 2 RX                */
#pragma intvect DefaultIRQHandler 99   /* LIN-UART 2 TX                */
#pragma intvect DefaultIRQHandler 100  /* LIN-UART 3 RX                */
#pragma intvect DefaultIRQHandler 101  /* LIN-UART 3 TX                */
#pragma intvect DefaultIRQHandler 102  /* LIN-UART 5 RX                */
#pragma intvect DefaultIRQHandler 103  /* LIN-UART 5 TX                */
#pragma intvect DefaultIRQHandler 104  /* LIN-UART 7 RX                */
#pragma intvect DefaultIRQHandler 105  /* LIN-UART 7 TX                */
#pragma intvect DefaultIRQHandler 106  /* LIN-UART 8 RX                */
#pragma intvect DefaultIRQHandler 107  /* LIN-UART 8 TX                */
#pragma intvect DefaultIRQHandler 108  /* LIN-UART 9 RX                */
#pragma intvect DefaultIRQHandler 109  /* LIN-UART 9 TX                */
#pragma intvect DefaultIRQHandler 110  /* Main Flash IRQ               */
#pragma intvect DefaultIRQHandler 111  /* Reserved                     */
#pragma intvect DefaultIRQHandler 112  /* USB EP0 IN  (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 113  /* USB EP0 OUT (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 114  /* USB EP1     (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 115  /* USB EP2     (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 116  /* USB EP3     (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 117  /* USB EP4     (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 118  /* USB EP5     (only MB9633xU)  */
#pragma intvect DefaultIRQHandler 119  /* USB Function Flag 1 (only MB9633xU) */
#pragma intvect DefaultIRQHandler 120  /* USB Function Flag 2 (only MB9633xU) */
#pragma intvect DefaultIRQHandler 121  /* USB MiniHost Flag 1 (only MB9633xU) */
#pragma intvect DefaultIRQHandler 122  /* USB MiniHost Flag 2 (only MB9633xU) */

/*---------------------------------------------------------------------------
   DefaultIRQHandler()
   This function is a placeholder for all vector definitions. Either use
   your own placeholder or add necessary code here. 
-----------------------------------------------------------------------------*/

__interrupt 
void DefaultIRQHandler (void)
{
    __DI();                              /* disable interrupts */
    while(1)
    {
        __wait_nop();                    /* halt system */
    }
}
