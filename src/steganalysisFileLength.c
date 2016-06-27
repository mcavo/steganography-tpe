#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"
#include "../include/wavmanager.h"


void checkFile(char * file) {

	char* technique;
	DWORD availableBytes, requiredBytes;
	int fits;

	EXTRACTSTR* ext = calloc(sizeof(EXTRACTSTR),1);

	ext->wav = malloc(sizeof(WAVSTR));
	getWavStr(file,ext->wav);
	
	technique = "LSB1";
	printf("%s:\n", technique);
	ext->tech = LSB1;
	fits = lsbFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("=> %s %s\n", (fits ? "Could be" : "Couldn't be"), technique);
	technique = "LSB4";
	printf("%s:\n", technique);
	ext->tech = LSB4;
	fits = lsbFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("=> %s %s\n", (fits ? "Could be" : "Couldn't be"), technique);
	technique = "LSBE";
	printf("%s:\n", technique);
	ext->tech = LSBE;
	fits = lsbeFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("=> %s %s\n", (fits ? "Could be" : "Couldn't be"), technique);

	freeExtractStr(ext);

}

int main(int argc, char **argv) {

	FILE* f;

	if(argc != 2) {
		printf("%s\n", "Missing wav file path.");
		return 0;
	}

	if (strcmp(argv[1]+(strlen(argv[1]) - strlen(".wav")), ".wav") != 0) {
		printf("%s%s%s\n", "Parameter invalid: \"", argv[1], "\" HAS TO be a wav file.");
		return 0;
	}

	f = fopen(argv[1], "rb");
	if (f==NULL) {
		printf("%s %s\n", "File doesn't exist:", argv[1]);
		return 0;
	}
	fclose(f);
	checkFile(argv[1]);
	return 0;
}