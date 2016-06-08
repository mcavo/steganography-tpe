#ifndef _LSB_
#define _LSB_

#include "../include/wavmanager.h"

// Steganography technique

#define LSB1				0
#define LSB4				1
#define LSBE				2

// Cipher algorithm

#define AES128				0
#define AES192				1
#define AES256				2
#define DES					3

// Cipher mode

#define ECB					0
#define CFB					1
#define OFB					2
#define CBC					3

// Return codes

#define OK					0
#define OUT_OF_MEMORY		1
#define NOT_ENOUGH_SPACE	2

typedef struct {
	unsigned char alg;
	unsigned char mode;
	char* pass;
} CIPHERSTR;

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

#endif
