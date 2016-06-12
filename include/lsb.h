#ifndef _LSB_
#define _LSB_

#include "wavmanager.h"
#include "ciphermanager.h"

// Steganography technique

#define LSB1				1
#define LSB4				2
#define LSBE				3

// Return codes

#define OK					0
#define OUT_OF_MEMORY		1
#define NOT_ENOUGH_SPACE	2

typedef struct {
	unsigned char tech;
	char* infile;
	char* stegowav;
	WAVSTR* wav;
	CIPHERSTR* cipher;
} EMBEDSTR;

typedef struct {
	unsigned char tech;
	char* outfile;
	WAVSTR* wav;
	CIPHERSTR* cipher;
} EXTRACTSTR;

int lsbEncryptWrapper(EMBEDSTR* emb);
int lsbeEncryptWrapper(EMBEDSTR* emb);
int lsbDecryptWrapper(EXTRACTSTR* ext);
int lsbeDecryptWrapper(EXTRACTSTR* ext);

void freeEmbedStr(EMBEDSTR* emb);
void freeExtractStr(EXTRACTSTR* ext);

#endif
