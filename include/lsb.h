#ifndef _LSB_
#define _LSB_

#include "../include/wavmanager.h"

// Steganography technique

#define LSB1	0
#define LSB4	1
#define LSBE	2

// Encryption algorithm

#define AES128	0
#define AES192	1
#define AES256	2
#define DES		3

// Cypher mode

#define ECB		0
#define CFB		1
#define OFB		2
#define CBC		3

typedef struct {
	unsigned char alg;
	unsigned char mode;
	char* pass;
} CYPHERSTR;

typedef struct {
	unsigned char tech;
	char* infile;
	char* stegowav;
	WAVSTR* wav;
	CYPHERSTR* cypher;
} EMBEDSTR;

typedef struct {
	unsigned char tech;
	char* outfile;
	WAVSTR* wav;
	CYPHERSTR* cypher;
} EXTRACTSTR;

int lsbEncryptWrapper(EMBEDSTR* emb);
int lsbDecryptWrapper(EXTRACTSTR* ext);

#endif
