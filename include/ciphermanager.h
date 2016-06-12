#ifndef _CIPHERMANAGER_
#define _CIPHERMANAGER_

#include "bytesmanager.h"

// Cipher algorithm

#define AES128				1
#define AES192				2
#define AES256				3
#define DES					4

// Cipher mode

#define ECB					1
#define CFB					2
#define OFB					3
#define CBC					4

#define KEY_SIZE			16

typedef struct {
	unsigned char alg;
	unsigned char mode;
	char* pass;
} CIPHERSTR;

int encrypt (CIPHERSTR* cipherstr, BYTE* plaindata, DWORD len, BYTE* cipherdata);
int decrypt (CIPHERSTR* cipherstr, BYTE* cipherdata, DWORD len, BYTE* plaindata);

void freeCipherStr(CIPHERSTR* cipher);

#endif