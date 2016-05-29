#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> 

typedef uint8_t		BYTE;
typedef uint16_t	WORD;
typedef uint32_t	DWORD;

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
} FMT_CK;

typedef struct {
	BYTE 	chunkID[4];				// 'data'
	CKSIZE 	chunkSize;
	BYTE* 	soundData;
} DATA_CK;

typedef struct {
	RIFF_CK riff_desc;				// MANDATORY
	FMT_CK fmt;
	DATA_CK data;					// Wave Data Chunk MANDATORY 
} WAVSTR;

int writeWavFile(FILE* wavfile, WAVSTR* wavstr);
int getWavStr(FILE* wavfile, WAVSTR* wavstr);

WORD littleEndianBITEArrayToWORD(BYTE buffer[2]);
DWORD littleEndianBITEArrayToDWORD(BYTE buffer[4]);
void WORDTolittleEndianBITEArray(WORD x, BYTE buffer[2]);
void DWORDTolittleEndianBITEArray(DWORD x, BYTE buffer[4]);


int
getWavStr(FILE* wavfile, WAVSTR* wavstr) {
	if (wavfile == NULL || wavstr == NULL)
		return 0;
	BYTE bufferWORD[2];
	BYTE bufferDWORD[4];

	int read = 0;

	read = fread(wavstr->riff_desc.chunkID, sizeof(wavstr->riff_desc.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->riff_desc.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(wavstr->riff_desc.format, sizeof(wavstr->riff_desc.format), 1, wavfile);

	read = fread(wavstr->fmt.chunkID, sizeof(wavstr->fmt.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wFormatTag = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wChannels = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.dwAvgSamplePerSec = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.dwAvgBytesPerSec = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wBlockAlign = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wBitsPerSample = littleEndianBITEArrayToWORD(bufferWORD);

	read = fread(wavstr->data.chunkID, sizeof(wavstr->data.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->data.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	wavstr->data.soundData = malloc(wavstr->data.chunkSize);
	if(wavstr->data.soundData == NULL)
		return 0;
	read = fread(wavstr->data.soundData, wavstr->data.chunkSize, 1, wavfile);
	return 1;

}

int
writeWavFile(FILE* wavfile, WAVSTR* wavstr) {
	if (wavfile == NULL || wavstr == NULL)
		return 0;
	BYTE bufferWORD[2];
	BYTE bufferDWORD[4];

	fwrite(wavstr->riff_desc.chunkID,sizeof(BYTE),sizeof(wavstr->riff_desc.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->riff_desc.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	fwrite(wavstr->riff_desc.format,sizeof(BYTE),sizeof(wavstr->riff_desc.format),wavfile);

	fwrite(wavstr->fmt.chunkID,sizeof(BYTE),sizeof(wavstr->fmt.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wFormatTag, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wChannels, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.dwAvgSamplePerSec, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.dwAvgBytesPerSec, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wBlockAlign, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wBitsPerSample, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);

	fwrite(wavstr->data.chunkID,sizeof(BYTE),sizeof(wavstr->data.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->data.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	fwrite(wavstr->data.soundData,wavstr->data.chunkSize,1,wavfile);
	return 0;

}

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