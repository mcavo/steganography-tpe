#include "../include/bytesmanager.h"

DWORD
littleEndianBITEArrayToDWORD(BYTE buffer[4]) {
	return buffer[0] | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
}

WORD
littleEndianBITEArrayToWORD(BYTE buffer[2]) {
	return buffer[0] | (buffer[1]<<8);
}

void
DWORDTolittleEndianBITEArray(DWORD x, BYTE buffer[4]) {
	buffer[0] = (BYTE)(x >> 0);
	buffer[1] = (BYTE)(x >> 8);
	buffer[2] = (BYTE)(x >> 16);
	buffer[3] = (BYTE)(x >> 24);
}

void
WORDTolittleEndianBITEArray(WORD x, BYTE buffer[2]) {
	buffer[0] = (BYTE)(x >> 0);
	buffer[1] = (BYTE)(x >> 8);
}