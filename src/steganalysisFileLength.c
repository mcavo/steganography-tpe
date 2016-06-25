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
	printf("%s\n", technique);
	ext->tech = LSB1;
	fits = lsbFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("%s %s\n", (fits ? "Could be" : "Can't be"), technique);
	technique = "LSB4";
	printf("%s\n", technique);
	ext->tech = LSB4;
	fits = lsbFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("%s %s\n", (fits ? "Could be" : "Can't be"), technique);
	technique = "LSBE";
	printf("%s\n", technique);
	ext->tech = LSBE;
	fits = lsbeFitsExtract(ext, &availableBytes, &requiredBytes);
	printf("%s: %u - %s: %u\n", "Available bytes", availableBytes, "Required bytes", requiredBytes);
	printf("%s %s\n", (fits ? "Could be" : "Can't be"), technique);

	freeExtractStr(ext);

}

int main() {

	printf("\n\n%s\n", "mamamia13.wav");
	printf("%s\n", "-------------");
	checkFile("data/EPOCH/mamamia13.wav");
	printf("\n\n%s\n", "mamamia13a.wav");
	printf("%s\n", "--------------");
	checkFile("data/EPOCH/mamamia13a.wav");
	printf("\n\n%s\n", "mandauna13a.wav");
	printf("%s\n", "---------------");
	checkFile("data/EPOCH/mandauna13a.wav");
	printf("\n\n%s\n", "mandauna13b.wav");
	printf("%s\n", "---------------");
	checkFile("data/EPOCH/mandauna13b.wav");

	return 1;
}