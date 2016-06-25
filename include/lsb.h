#ifndef _LSB_
#define _LSB_

#include "wavmanager.h"
#include "ciphermanager.h"

// Steganography techniques

#define LSB1						1
#define LSB4						2
#define LSBE						3

// Return codes

#define OK							0
#define ERROR_OUT_OF_MEMORY			1
#define ERROR_NOT_ENOUGH_SPACE		2

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

/* Steganography */

int lsbEmbedWrapper(EMBEDSTR* emb);
int lsbeEmbedWrapper(EMBEDSTR* emb);

int lsbExtractWrapper(EXTRACTSTR* ext);
int lsbeExtractWrapper(EXTRACTSTR* ext);

/* Steganalysis */

int lsbFitsExtract(EXTRACTSTR* ext, DWORD* availableBytes, DWORD* requiredBytes);
int lsbeFitsExtract(EXTRACTSTR* ext, DWORD* availableBytes, DWORD* requiredBytes);

int lsbExtractContent(EXTRACTSTR* ext);
int lsbeExtractContent(EXTRACTSTR* ext);

/* Free structures */

void freeEmbedStr(EMBEDSTR* emb);
void freeExtractStr(EXTRACTSTR* ext);

#endif
