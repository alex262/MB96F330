
#ifndef _CRC32_H
#define _CRC32_H

#include "global.h"

DWORD Crc32Sum(BYTE *buf, WORD len);
BYTE Crc32Sum_Check(BYTE *buf, WORD len, DWORD crc_get);

#endif 