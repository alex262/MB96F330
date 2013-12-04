/*********************************************************                   
 *                                                       *
 *             Master/slave CANopen Library              *
 *                                                       *

 *********************************************************
           File : variahw.c
 *-------------------------------------------------------*
 * For Microcontroler Motorola MC9S12  (HCS12)           *      
 * Others functions                                      *
 *********************************************************/
//#define DEBUG_WAR_CONSOLE_ON
#include <applicfg.h>

/******************************************************************************/
char *
hex_convert (char *buf, unsigned long value, char lastCar)
{
  //Thanks to Stéphane Carrez for this function
  char num[32];
  int pos;

  *buf++ = '0';
  *buf++ = 'x';

  pos = 0;
  while (value != 0) {
    char c = value & 0x0F;
    num[pos++] = "0123456789ABCDEF"[(unsigned) c];
    value = (value >> 4) & (0x0fffffffL);
    }
  if (pos == 0)
    num[pos++] = '0';

  while (--pos >= 0)
    *buf++ = num[pos];

  *buf++ = lastCar;
  *buf = 0;
  return buf;
}





