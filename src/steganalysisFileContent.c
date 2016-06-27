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

int main(int argc, char **argv) {

	FILE* f;
	char* wav;
	char* out;
	int i, technique, len;

	for(i = 1 ; i < 7 ; i+=2) {
		if(i==argc) {
			printf("%s\n", "Missing parameters.");
			return 0;
		}

		if(strcmp(argv[i],"-p")==0) {

			if (strcmp(argv[i+1]+(strlen(argv[i+1]) - strlen(".wav")), ".wav") != 0) {
				printf("%s%s%s\n", "Parameter invalid: \"", argv[i+1], "\", HAS TO be a wav file.");
				return 0;
			}

			f = fopen(argv[i+1], "rb");
			if (f==NULL) {
				printf("%s %s\n", "File doesn't exist:", argv[i+1]);
				return 0;
			}

			fclose(f);

			len = strlen(argv[i+1]);
			wav = calloc(len+1, 1);
			memcpy(wav, argv[i+1], len);

		} else if (strcmp(argv[1],"-out")==0) {

			f = fopen(argv[i+1], "rb");
			if (f!=NULL) {
				fclose(f);
				printf("%s %s\n", "File already exist:", argv[i+1]);
				return 0;
			}

			len = strlen(argv[i+1]);
			out = calloc(len+1, 1);
			memcpy(out, argv[i+1], len);

		} else if (strcmp(argv[1],"-steg")==0) {

			if (strcmp(argv[i+1],"LSB1")==0) {
					technique = LSB1;
				} else if (strcmp(argv[i+1],"LSB4")==0) {
					technique = LSB4;
				} else if (strcmp(argv[i+1],"LSBE")==0) {
					technique = LSBE;
				} else {
					printf("%s%s%s\n", "Invalid embed technique: \"", argv[i+1], "\", HAS TO be [\"LSB1\"|\"LSB4\"|\"LSBE\"].");
					return 0;
				}

		} else {
			printf("%s %s\n", "Invalid parameter:", argv[i]);
			return 0;
		}
	}

	writeFile(wav, out, technique);

	return 0;
}