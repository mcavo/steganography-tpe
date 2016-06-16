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
int validEmbedStr(EMBEDSTR* emb);
int validExtractStr(EXTRACTSTR* emb);

EMBEDSTR* parseEmbedStr(int argc, char **argv) {
	EMBEDSTR* emb = calloc(sizeof(EMBEDSTR),1);
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-in")==0) {

				int len = strlen(argv[i+1]);
				emb->infile = malloc(sizeof(char)*(len+1));
				memcpy(emb->infile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-p")==0) {

				emb->wav = calloc(sizeof(WAVSTR),1);
				printf("Parsing %s...\n", argv[i+1]);
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
					freeEmbedStr(emb);
					return NULL;
				}
			} else if (strcmp(argv[i],"-a")==0) {
				if (emb->cipher == NULL) {
					emb->cipher = calloc(sizeof(CIPHERSTR),1);
					if (emb->cipher == NULL) {
						freeEmbedStr(emb);
						return NULL;
					}
				}
				if (strcmp(argv[i+1],"aes128")==0) {
					emb->cipher->alg = AES128;
				} else if (strcmp(argv[i+1],"aes192")==0) {
					emb->cipher->alg = AES192;
				} else if (strcmp(argv[i+1],"aes256")==0) {
					emb->cipher->alg = AES256;
				} else if (strcmp(argv[i+1],"des")==0) {
					emb->cipher->alg = DES;
				} else {
					freeEmbedStr(emb);
					return NULL;
				}
			} else if (strcmp(argv[i],"-m")==0) {
				if (emb->cipher == NULL) {
					emb->cipher = calloc(sizeof(CIPHERSTR),1);
					if (emb->cipher == NULL) {
						freeEmbedStr(emb);
						return NULL;
					}
				}
				if (strcmp(argv[i+1],"ecb")==0) {
					emb->cipher->mode = ECB;
				} else if (strcmp(argv[i+1],"cfb")==0) {
					emb->cipher->mode = CFB;
				} else if (strcmp(argv[i+1],"ofb")==0) {
					emb->cipher->mode = OFB;
				} else if (strcmp(argv[i+1],"cbc")==0) {
					emb->cipher->mode = CBC;
				} else {
					freeEmbedStr(emb);
					return NULL;
				}
			} else if (strcmp(argv[i],"-pass")==0) {
				if (emb->cipher == NULL) {
					emb->cipher = calloc(sizeof(CIPHERSTR),1);
					if (emb->cipher == NULL) {
						freeEmbedStr(emb);
						return NULL;
					}
				}

				int len = strlen(argv[i+1]);
				emb->cipher->pass = malloc(sizeof(char)*(len+1));
				memcpy(emb->cipher->pass,argv[i+1],len+1);

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
					freeExtractStr(ext);
					return NULL;
				}
			} else if (strcmp(argv[i],"-a")==0) {
				if (ext->cipher == NULL) {
					ext->cipher = calloc(sizeof(CIPHERSTR),1);
					if (ext->cipher == NULL) {
						freeExtractStr(ext);
						return NULL;
					}
				}
				if (strcmp(argv[i+1],"aes128")==0) {
					ext->cipher->alg = AES128;
				} else if (strcmp(argv[i+1],"aes192")==0) {
					ext->cipher->alg = AES192;
				} else if (strcmp(argv[i+1],"aes256")==0) {
					ext->cipher->alg = AES256;
				} else if (strcmp(argv[i+1],"des")==0) {
					ext->cipher->alg = DES;
				} else {
					freeExtractStr(ext);
					return NULL;
				}
			} else if (strcmp(argv[i],"-m")==0) {
				if (ext->cipher == NULL) {
					ext->cipher = calloc(sizeof(CIPHERSTR),1);
					if (ext->cipher == NULL) {
						freeExtractStr(ext);
						return NULL;
					}
				}
				if (strcmp(argv[i+1],"ecb")==0) {
					ext->cipher->mode = ECB;
				} else if (strcmp(argv[i+1],"cfb")==0) {
					ext->cipher->mode = CFB;
				} else if (strcmp(argv[i+1],"ofb")==0) {
					ext->cipher->mode = OFB;
				} else if (strcmp(argv[i+1],"cbc")==0) {
					ext->cipher->mode = CBC;
				} else {
					freeExtractStr(ext);
					return NULL;
				}
			} else if (strcmp(argv[i],"-pass")==0) {
				if (ext->cipher == NULL) {
					ext->cipher = calloc(sizeof(CIPHERSTR),1);
					if (ext->cipher == NULL) {
						freeExtractStr(ext);
						return NULL;
					}
				}

				int len = strlen(argv[i+1]);
				ext->cipher->pass = malloc(sizeof(char)*(len+1));
				memcpy(ext->cipher->pass,argv[i+1],len+1);

			}
		}
	}

	return ext;
	
}

// int validEmbedStr(EMBEDSTR* emb) {

// }

// int validExtractStr(EXTRACTSTR* emb) {
	
// }


/*********************/
/*        MAIN       */
/*********************/

int main(int argc, char **argv)
{
	int out;

	if (strcmp(argv[1],"-embed")==0) {

		EMBEDSTR* embed = parseEmbedStr(argc,argv);
		if ( embed->tech == LSB1 || embed->tech == LSB4 )
			out = lsbEmbedWrapper(embed);
		else if ( embed->tech == LSBE ) 
			out = lsbeEmbedWrapper(embed);
		else
			printf("%s\n", "Error:");
		freeEmbedStr(embed);


	} else if(strcmp(argv[1],"-extract")==0) {

		EXTRACTSTR* extract = parseExtractStr(argc,argv);
		if ( extract->tech == LSB1 || extract->tech == LSB4 )
			out = lsbExtractWrapper(extract);
		else if ( extract->tech == LSBE ) 
			out = lsbeExtractWrapper(extract);
		else
			printf("%s\n", "Error:");
		freeExtractStr(extract);

	} else {
		printf("Error: invalid input. Parameters -embed or -extract expected.\n");
	}

    return 0;
}

/* 

gcc -o stegowav stegowav.c lsb.c bytesmanager.c wavmanager.c ciphermanager.c
./stegowav -extract -out "data/test/funT1" -p "data/AnaTest/funT1.wav" -steg LSB1
./stegowav -extract -out "data/test/funT4" -p "data/AnaTest/funT4.wav" -steg LSB4
./stegowav -extract -out "data/test/funTE" -p "data/AnaTest/funTE.wav" -steg LSBE
./stegowav -extract -out "data/test/funT1E" -p "data/AnaTest/funT1E.wav" -steg LSB1 -a "aes192" -m "cbc" -pass "oculto"
./stegowav -embed -out "data/test/testwav2.wav" -in "data/data.txt" -p "data/testwav.wav" -steg LSB1 -a "aes192" -m "cbc" -pass "123456"
./stegowav -extract -out "data/test/data" -p "data/test/testwav2.wav" -steg LSB1 -a "aes192" -m "cbc" -pass "123456"
*/
