#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"
#include "../include/wavmanager.h"

DWORD getLen2(FILE* file) {
	DWORD len;
	fseek(file, 0L, SEEK_END);
	len = ftell(file);
	rewind(file);
	return len;
}

DWORD getWavLen(char* file) {
	WAVSTR* wav = calloc(sizeof(WAVSTR), 1);
	getWavStr(file, wav);
	DWORD wavLen = wav->data.chunkSize + 44;
	if(wav->fmt.extraParamSize != 0) {
		wavLen += 4 + wav->fmt.extraParamSize;
	}
	return wavLen;
}

int main(int argc, char **argv) {

	FILE* f;

	if(argc != 2) {
		printf("%s\n", "Missing wave file path.");
		return 0;
	}

	if (strcmp(argv[1]+(strlen(argv[1]) - strlen(".wav")), ".wav") != 0) {
		printf("%s%s%s\n", "Parameter invalid: \"", argv[1], "\", HAS TO be a wav file.");
		return 0;
	}

	f = fopen(argv[1], "rb");
	if (f==NULL) {
		printf("%s %s\n", "File doesn't exist:", argv[1]);
		return 0;
	}

	DWORD realLen = getLen2(f);

	DWORD wavLen = getWavLen(argv[1]);

	if (realLen == wavLen) {
		printf("%s\n", "No EOF embedded data");
	} else {
		printf("%s %d %s\n", "There are", realLen - wavLen, "bytes of EOF embedded data.");
	}

	return 0;

}