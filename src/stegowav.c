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
int missingEmbedStrParameters(EMBEDSTR* emb);
int validExtractStr(EXTRACTSTR* emb);

EMBEDSTR* parseEmbedStr(int argc, char **argv) {
	EMBEDSTR* emb = calloc(sizeof(EMBEDSTR),1);
	if(emb == NULL) {
		return emb;
	}
	FILE* f;
	int i, error;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-in")==0) {

				f = fopen(argv[i+1], "rb");
				if (f==NULL) {
					printf("%s %s\n", "File doesn't exist:", argv[i+1]);
					freeEmbedStr(emb);
					fclose(f);
					return NULL;
				}
				fclose(f);
				int len = strlen(argv[i+1]);
				emb->infile = malloc(sizeof(char)*(len+1));
				memcpy(emb->infile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-p")==0) {

				if (strcmp(argv[i+1]+(strlen(argv[i+1]) - strlen(".wav")), ".wav") != 0) {
					printf("%s%s%s\n", "Parameter invalid: \"", argv[i+1], "\", MUST be a wav file.");
					freeEmbedStr(emb);
					return NULL;
				} else {
					f = fopen(argv[i+1], "rb");
					if (f==NULL) {
						printf("%s %s\n", "File doesn't exist:", argv[i+1]);
						freeEmbedStr(emb);
						fclose(f);
						return NULL;
					}
					fclose(f);
				}
				emb->wav = calloc(sizeof(WAVSTR),1);
				if( (error = getWavStr(argv[i+1],emb->wav)) != OK ) {

				}



			} else if (strcmp(argv[i],"-out")==0) {

				if (strcmp(argv[i+1]+(strlen(argv[i+1]) - strlen(".wav")), ".wav") != 0) {
					printf("%s%s%s\n", "Parameter invalid: \"", argv[i+1], "\", MUST be a wav file.");
					freeEmbedStr(emb);
					return NULL;
				} else {
					f = fopen(argv[i+1], "rb");
					if (f==NULL) {
						freeEmbedStr(emb);
						fclose(f);
						return NULL;
					}
					fclose(f);
				}

				int len = strlen(argv[i+1]);
				emb->stegowav = malloc(sizeof(char)*(len+1));
				memcpy(emb->stegowav,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"lsb1")==0) {
					emb->tech = LSB1;
				} else if (strcmp(argv[i+1],"lsb4")==0) {
					emb->tech = LSB4;
				} else if (strcmp(argv[i+1],"lsbE")==0) {
					emb->tech = LSBE;
				} else {
					printf("%s%s%s\n", "Invalid embed technique: \"", argv[i+1], "\", MUST be [\"lsb1\"|\"lsb4\"|\"lsbe\"].");
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
					printf("%s%s%s\n", "Invalid encrypting algorithm: \"", argv[i+1], "\", MUST be [\"aes128\"|\"aes192\"|\"aes256\"|\"des\"].");
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
					printf("%s%s%s\n", "Invalid encrypting mode: \"", argv[i+1], "\", MUST be [\"cfb\"|\"ecb\"|\"ofb\"|\"cbc\"].");
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

	i = missingEmbedStrParameters(emb);

	if (i == 1) {
		freeEmbedStr(emb);
		fclose(f);
		return NULL;
	}

	return emb;

}

EXTRACTSTR* parseExtractStr(int argc, char **argv) {

	EXTRACTSTR* ext = calloc(sizeof(EXTRACTSTR),sizeof(char));
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-p")==0) {

				ext->wav = calloc(sizeof(WAVSTR),1);
				getWavStr(argv[i+1],ext->wav);
				if(ext->wav == NULL)
					printf("%s\n", "wav null");

			} else if (strcmp(argv[i],"-out")==0) {

				int len = strlen(argv[i+1]);
				ext->outfile = malloc(sizeof(char)*(len+1));
				memcpy(ext->outfile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"lsb1")==0) {
					ext->tech = LSB1;
				} else if (strcmp(argv[i+1],"lsb4")==0) {
					ext->tech = LSB4;
				} else if (strcmp(argv[i+1],"lsbE")==0) {
					ext->tech = LSBE;
				} else {
					freeExtractStr(ext);
					printf("%s\n", "mal lsb");
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

int missingEmbedStrParameters(EMBEDSTR* emb) {
	int ret = 0;

	if (emb == NULL)
		return 1;

	if (emb->tech == 0) {
		ret = 1;
		printf("%s\n", "Parameter missing: -steg [\"lsb1\"|\"lsb4\"|\"lsbe\"]");
		return 1;
	}

	if (emb->infile == NULL) {
		ret = 1;
		printf("%s\n", "Parameter missing: -in \"file\"");
		return 1;
	}

	if (emb->wav == NULL) {
		ret = 1;
		printf("%s\n", "Parameter missing: -p \"wavefile\"");
		return 1;
	}

	if (emb->stegowav == NULL) {
		ret = 1;
		printf("%s\n", "Parameter missing: -out \"stegowavefile\"");
		return 1;
	}

	if (emb->cipher != NULL) {
		if (emb->cipher->alg == 0) {
			ret = 1;
			printf("%s\n", "Parameter missing: -a [\"aes128\"|\"aes192\"|\"aes256\"|\"des\"]");
			return 1;
		}
		if (emb->cipher->mode == 0) {
			ret = 1;
			printf("%s\n", "Parameter missing: -m [\"cfb\"|\"ecb\"|\"ofb\"|\"cbc\"]");
			return 1;
		}
		if (emb->cipher->pass == NULL) {
			ret = 1;
			printf("%s\n", "Parameter missing: -pass password");
			return 1;
		}
	}
	return 0;
}


int validExtractStr(EXTRACTSTR* emb) {
	return 0;	
}


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
		else 
			out = lsbeEmbedWrapper(embed);
		freeEmbedStr(embed);


	} else if(strcmp(argv[1],"-extract")==0) {
		EXTRACTSTR* extract = parseExtractStr(argc,argv);
		if ( extract->tech == LSB1 || extract->tech == LSB4 )
			out = lsbExtractWrapper(extract);
		else
			out = lsbeExtractWrapper(extract);
		freeExtractStr(extract);

	} else {
		printf("Error: invalid input. Parameters -embed or -extract expected.\n");
	}

    return 0;
}

/* 
gcc -o stegowav stegowav.c lsb.c bytesmanager.c wavmanager.c ciphermanager.c
./stegowav -extract -out "data/test/funT1" -p "data/AnaTest/funT1.wav" -steg lsb1
./stegowav -extract -out "data/test/funT4" -p "data/AnaTest/funT4.wav" -steg lsb4
./stegowav -extract -out "data/test/funTE" -p "data/AnaTest/funTE.wav" -steg lsbE
./stegowav -extract -out "data/test/funT1E" -p "data/AnaTest/funT1E.wav" -steg lsb1 -a "aes192" -m "cbc" -pass "oculto"
*/
