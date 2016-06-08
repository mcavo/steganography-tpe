#ifndef _WAVMANAGER_
#define _WAVMANAGER_

#include "../include/bytesmanager.h"

typedef DWORD 		CKSIZE;			// 32-bit unsigned size value

typedef struct {
    BYTE    chunkID[4];				// 'RIFF'
    CKSIZE  chunkSize;				// File Size
    BYTE    format[4];				// Format: 'WAVE'
} RIFF_CK;

typedef struct {
	BYTE	chunkID[4];				// 'fmt '
	CKSIZE	chunkSize;				// 16 para PCM.Size of rest of subchunk. /* Common fields */
	WORD 	wFormatTag;				// Format category,i.e.:PCM = 1 (no compres.)
	WORD 	wChannels;				// Number of channels:1, mono; 2, stereo DWORD dwSamplesPerSec; // Sampling rate: Mhz
	DWORD 	dwAvgSamplePerSec;
	DWORD 	dwAvgBytesPerSec;
	WORD 	wBlockAlign;
	WORD 	wBitsPerSample;			// 8, 16, etc.
	WORD	extraParamSize;			// If PCM, doesn't exist
	BYTE* 	extraParams;			// space for extra params
} FMT_CK;

typedef struct {
	BYTE 	chunkID[4];				// 'data'
	CKSIZE 	chunkSize;
	BYTE* 	soundData;
} DATA_CK;

typedef struct {
	RIFF_CK riff_desc;				// MANDATORY
	FMT_CK 	fmt;					// MANDATORY
	DATA_CK data;					// Wave Data Chunk MANDATORY 
} WAVSTR;

int writeWavFile(char* wavfile, WAVSTR* wavstr);
int getWavStr(char* wavfile, WAVSTR* wavstr);

#endif