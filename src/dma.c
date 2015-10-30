/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "mb96338us.h"
#include "global.h"
#include "dma.h"

//---------------------------------------------------------------------------
static volatile DMA_Descriptor * pDmaDescr	= (volatile DMA_Descriptor *)(&BAPL0);
static volatile BYTE * pDmaDisel			= (volatile BYTE *)(&DISEL0);


//---------------------------------------------------------------------------
void InitDAM_ch(DmaConfig_t  * DmaConfig)
{
	volatile DMA_Descriptor * pDmaDescr_t;
	
	if(DmaConfig->ch >= COUNT_DMA_CH) return;
	
	pDmaDescr_t = &pDmaDescr[DmaConfig->ch];
		
	pDmaDisel[DmaConfig->ch] = DmaConfig->nIRQ;
	pDmaDescr_t->IOAL = (BYTE)DmaConfig->adrIOA;
	pDmaDescr_t->IOAH = (BYTE)(DmaConfig->adrIOA>>8);
	pDmaDescr_t->DCTL = (BYTE)DmaConfig->Count;
	pDmaDescr_t->DCTH = (BYTE)(DmaConfig->Count>>8);
	pDmaDescr_t->BAPL = (BYTE)(DmaConfig->adrBAP);
	pDmaDescr_t->BAPM = (BYTE)(DmaConfig->adrBAP>>8);
	pDmaDescr_t->BAPH = (BYTE)(DmaConfig->adrBAP>>16);
	pDmaDescr_t->DMACS.bit._SE	= DmaConfig->SE;
	pDmaDescr_t->DMACS.bit._DIR	= DmaConfig->DIR;
	pDmaDescr_t->DMACS.bit._BF	= DmaConfig->BF;
	pDmaDescr_t->DMACS.bit._BW	= DmaConfig->BW;
	pDmaDescr_t->DMACS.bit._IF	= DmaConfig->IF;
	pDmaDescr_t->DMACS.bit._BPD	= DmaConfig->BPD;
	
	CLEARBIT(DSR,DmaConfig->ch);// Clear transfer end interrupt, if any
	//SETBIT(DER,DmaConfig->ch);	// DMA ch enable
}
//---------------------------------------------------------------------------
void DMA_Enable(BYTE nCH)
{
	SETBIT(DER,nCH);	// DMA ch enable
}
//---------------------------------------------------------------------------
void DMA_Disable(BYTE nCH)
{
	CLEARBIT(DER,nCH);	// DMA ch disable
}
//---------------------------------------------------------------------------
void DMA_change_BAP(BYTE nCH, DWORD	adrBAP)
{
	volatile DMA_Descriptor * pDmaDescr_t;
	
	if(nCH >= COUNT_DMA_CH) return;
	
	pDmaDescr_t = &pDmaDescr[nCH];
	
	pDmaDescr_t->BAPL = (BYTE)(adrBAP);
	pDmaDescr_t->BAPM = (BYTE)(adrBAP>>8);
	pDmaDescr_t->BAPH = (BYTE)(adrBAP>>16);
}
//---------------------------------------------------------------------------
void DMA_change_BAP_restart(BYTE nCH, DWORD	adrBAP, WORD	Count)
{
	volatile DMA_Descriptor * pDmaDescr_t;
	
	if(nCH >= COUNT_DMA_CH) return;
	
	pDmaDescr_t = &pDmaDescr[nCH];
	
	pDmaDescr_t->BAPL = (BYTE)(adrBAP);
	pDmaDescr_t->BAPM = (BYTE)(adrBAP>>8);
	pDmaDescr_t->BAPH = (BYTE)(adrBAP>>16);

	pDmaDescr_t->DCTL = (BYTE)Count;
	pDmaDescr_t->DCTH = (BYTE)(Count>>8);

	CLEARBIT(DSR,nCH);	// Clear transfer end interrupt, if any
	SETBIT(DER,nCH);	// DMA ch enable
}
//---------------------------------------------------------------------------
void DMA_restart(BYTE nCH, WORD	Count)
{
	volatile DMA_Descriptor * pDmaDescr_t;
	
	if(nCH >= COUNT_DMA_CH) return;

	pDmaDescr_t = &pDmaDescr[nCH];
	
	pDmaDescr_t->DCTL = (BYTE)Count;
	pDmaDescr_t->DCTH = (BYTE)(Count>>8);

	CLEARBIT(DSR,nCH);	// Clear transfer end interrupt, if any
	SETBIT(DER,nCH);	// DMA ch enable
}
//---------------------------------------------------------------------------
WORD DMA_GetCnt(BYTE nCH)
{
	if(nCH == 0) return DCT0;
	if(nCH == 1) return DCT1;
	if(nCH == 2) return DCT2;
	if(nCH == 3) return DCT3;
	if(nCH == 4) return DCT4;
	if(nCH == 5) return DCT5;
	if(nCH == 6) return DCT6;
	if(nCH == 7) return DCT7;
	if(nCH == 8) return DCT8;
	if(nCH == 9) return DCT9;
	return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void DrawDMAStatus(void)
{
	DisInterrupt();
	puts((BYTE *)"-----------S T A T U S   D M A ------------\n\r\n\r");
	
	sprintf(B_Out,"DISEL0 - %d\n\r", DISEL0);	puts(B_Out);
	sprintf(B_Out,"DISEL1 - %d\n\r", DISEL1);	puts(B_Out);
	sprintf(B_Out,"DISEL2 - %d\n\r", DISEL2);	puts(B_Out);
	sprintf(B_Out,"DISEL3 - %d\n\r", DISEL3);	puts(B_Out);
	sprintf(B_Out,"DISEL4 - %d\n\r", DISEL4);	puts(B_Out);
	sprintf(B_Out,"DISEL5 - %d\n\r", DISEL5);	puts(B_Out);
	sprintf(B_Out,"DISEL6 - %d\n\r", DISEL6);	puts(B_Out);
	sprintf(B_Out,"DISEL7 - %d\n\r", DISEL7);	puts(B_Out);
	sprintf(B_Out,"DISEL8 - %d\n\r", DISEL8);	puts(B_Out);
	sprintf(B_Out,"DISEL9 - %d\n\r", DISEL9);	puts(B_Out);
	
	puts("DSR  - ");	puts_bin_word(DSR);puts("\n\r");
	puts("DSSR - ");	puts_bin_word(DSSR);puts("\n\r");
	puts("DER  - ");	puts_bin_word(DER);puts("\n\r");
	
	sprintf(B_Out,"0:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT0, IOA0, BAPH0, BAPM0, BAPL0);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"1:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT1, IOA1, BAPH1, BAPM1, BAPL1);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"2:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT2, IOA2, BAPH2, BAPM2, BAPL2);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"3:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT3, IOA3, BAPH3, BAPM3, BAPL3);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"4:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT4, IOA4, BAPH4, BAPM4, BAPL4);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"5:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT5, IOA5, BAPH5, BAPM5, BAPL5);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"6:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT6, IOA6, BAPH6, BAPM6, BAPL6);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"7:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT7, IOA7, BAPH7, BAPM7, BAPL7);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"8:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT8, IOA8, BAPH8, BAPM8, BAPL8);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	sprintf(B_Out,"9:DCT - 0x%04X, IOA - 0x%04X, BAP - 0x%02X %02X %02X, DMACS - ", DCT9, IOA9, BAPH9, BAPM9, BAPL9);	puts(B_Out);
	puts_bin_byte(DMACS0); puts("\n\r");
	
	EnInterrupt();
	
}







