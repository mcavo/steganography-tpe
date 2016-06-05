#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"
#include "../include/wavmanager.h"

/*********************/
/*    Parse input    */
/*********************/

EMBEDSTR* parseEmbedStr(int argc, char **argv);
EXTRACTSTR* parseExtractStr(int argc, char **argv);

EMBEDSTR* parseEmbedStr(int argc, char **argv) {
	EMBEDSTR* emb = calloc(sizeof(EMBEDSTR),sizeof(char));
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-in")==0) {

				int len = strlen(argv[i+1]);
				emb->infile = malloc(sizeof(char)*(len+1));
				memcpy(emb->infile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-p")==0) {

				emb->wav = malloc(sizeof(WAVSTR));
				printf("%s\n", "Parsing wav...");
				getWavStr(argv[i+1],emb->wav);

			} else if (strcmp(argv[i],"-out")==0) {

				int len = strlen(argv[i+1]);
				emb->stegowav = malloc(sizeof(char)*(len+1));
				memcpy(emb->stegowav,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"LSB1")==0) {
					emb->tech = LSB1;
				} else if (strcmp(argv[i+1],"LSB4")==0) {
					emb->tech = LSB4;
				} else if (strcmp(argv[i+1],"LSBE")==0) {
					emb->tech = LSBE;
				} else {
					if(emb->infile!=NULL)
						free(emb->infile);
					if(emb->wav!=NULL)
						free(emb->wav);
					free(emb);
					return NULL;
				}
			}
		}
	}

	return emb;

}

EXTRACTSTR* parseExtractStr(int argc, char **argv) {

	EXTRACTSTR* ext = calloc(sizeof(EXTRACTSTR),sizeof(char));
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-p")==0) {

				ext->wav = malloc(sizeof(WAVSTR));
				printf("%s\n", "Parsing wav...");
				getWavStr(argv[i+1],ext->wav);

			} else if (strcmp(argv[i],"-out")==0) {

				int len = strlen(argv[i+1]);
				ext->outfile = malloc(sizeof(char)*(len+1));
				memcpy(ext->outfile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"LSB1")==0) {
					ext->tech = LSB1;
				} else if (strcmp(argv[i+1],"LSB4")==0) {
					ext->tech = LSB4;
				} else if (strcmp(argv[i+1],"LSBE")==0) {
					ext->tech = LSBE;
				} else {
					if(ext->outfile!=NULL)
						free(ext->outfile);
					if(ext->wav!=NULL)
						free(ext->wav);
					free(ext);
					return NULL;
				}
			}
		}
	}

	return ext;
	
}


/*********************/
/*        MAIN       */
/*********************/

int main(int argc, char **argv)
{
	int i;

	if (strcmp(argv[1],"-embed")==0) {

		EMBEDSTR* embed = parseEmbedStr(argc,argv);
		lsbEncryptWrapper(embed);


	} else if(strcmp(argv[1],"-extract")==0) {

		EXTRACTSTR* extract = parseExtractStr(argc,argv);
		lsbDecryptWrapper(extract);

	} else {
		printf("Error: invalid input. Parameters -embed or -extract expected.\n");
	}

    return 0;
}

/* 

./stegowav -embed -in "../data/data.txt" -p "../data/wavfile.wav" -out "../data/wavfile2.wav" -steg LSB1
./stegowav -extract -out "../data/data2" -p "../data/wavfile2.wav" -steg LSB1

*/
