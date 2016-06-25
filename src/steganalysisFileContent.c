#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"
#include "../include/wavmanager.h"


void writeFile(char * file, char* out, int technique) {

	int len = strlen(out);
	char* outfile = malloc(len + 1);
	memcpy(outfile, out, len);
	outfile[len] = 0;

	EXTRACTSTR* ext = calloc(sizeof(EXTRACTSTR),1);
	ext->wav = malloc(sizeof(WAVSTR));
	getWavStr(file,ext->wav);
	ext->tech = technique;
	ext->outfile = outfile;
	if (technique == LSBE)
		lsbeExtractContent(ext);
	else
		lsbExtractContent(ext);
	freeExtractStr(ext);

}

int main() {

	writeFile("data/EPOCH/mamamia13.wav", "data/test/mamamia13_4", LSB4);

	writeFile("data/EPOCH/mamamia13a.wav", "data/test/mamamia13a_1", LSB1);	
	writeFile("data/EPOCH/mamamia13a.wav", "data/test/mamamia13a_4", LSB4);

	writeFile("data/EPOCH/mandauna13a.wav", "data/test/mandauna13a_4", LSB4);

	writeFile("data/EPOCH/mandauna13b.wav", "data/test/mandauna13b_1", LSB1);
	writeFile("data/EPOCH/mandauna13b.wav", "data/test/mandauna13b_E", LSBE);

	return 1;
}