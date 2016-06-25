#include "../include/wavmanager.h"
#include <stdio.h>
#include <stdlib.h>

int
getWavStr(char* wavname, WAVSTR* wavstr) {
	FILE* wavfile = fopen(wavname, "rb");
	if (wavfile == NULL) {
		printf("Error opening file\n");
		return 1;
	}
	if (wavstr == NULL) {
		fclose(wavfile);
		return 1;
	}
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

	if (wavstr->fmt.chunkSize != 16) {
		read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
		wavstr->fmt.extraParamSize = littleEndianBITEArrayToWORD(bufferWORD);
		wavstr->fmt.extraParams = malloc(wavstr->fmt.extraParamSize);
		if(wavstr->fmt.extraParams == NULL) {
			fclose(wavfile);
			return 1;
		}
		read = fread(wavstr->fmt.extraParams, wavstr->fmt.extraParamSize, 1, wavfile);
	}

	read = fread(wavstr->data.chunkID, sizeof(wavstr->data.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->data.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	wavstr->data.soundData = malloc(wavstr->data.chunkSize);
	if(wavstr->data.soundData == NULL) {
		fclose(wavfile);
		if(wavstr->fmt.extraParams != NULL)
			free(wavstr->fmt.extraParams);
		return 1;
	}
	read = fread(wavstr->data.soundData, wavstr->data.chunkSize, 1, wavfile);
	fclose(wavfile);
	return 0;

}

int
writeWavFile(char* wavname, WAVSTR* wavstr) {
	FILE* wavfile = fopen(wavname, "wb");
	if (wavfile == NULL) {
		printf("Error opening file\n");
		return 0;
	}
	if (wavstr == NULL) {
		fclose(wavfile);
		return 0;
	}
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
	fclose(wavfile);
	return 1;

}

void freeWavStr(WAVSTR* wav) {
	if (wav == NULL) {
		return;
	}
	if (wav->fmt.extraParams != NULL)
		free(wav->fmt.extraParams);
	if (wav->data.soundData != NULL)
		free(wav->data.soundData);
	free(wav);
}
