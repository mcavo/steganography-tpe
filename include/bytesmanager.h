#ifndef _BYTESMANAGER_
#define _BYTESMANAGER_

#include <stdint.h>

typedef uint8_t		BYTE;
typedef uint16_t	WORD;
typedef uint32_t	DWORD;

WORD littleEndianBITEArrayToWORD(BYTE buffer[2]);
DWORD littleEndianBITEArrayToDWORD(BYTE buffer[4]);
void WORDTolittleEndianBITEArray(WORD x, BYTE buffer[2]);
void DWORDTolittleEndianBITEArray(DWORD x, BYTE buffer[4]);

#endif