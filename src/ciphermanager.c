#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include "../include/ciphermanager.h"

int decrypt (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	EVP_CIPHER_CTX ctx;
	BYTE out[len]; 
	DWORD outl, templ;
    BYTE key[KEY_SIZE];
    BYTE iv[KEY_SIZE];

	printf("Cipher pass:%s\n", cipher->pass); 
	printf("Algorithm: %s\n", cipher->alg == AES192 ? "AES192" : "?"); 
	printf("Mode: %s\n", cipher->mode == CBC ? "CBC" : "?"); 

	switch (cipher->alg) { 
		case AES128: break; 
		case AES192: 
			EVP_BytesToKey(EVP_aes_192_cbc(), EVP_md5(), NULL, cipher->pass, strlen(cipher->pass),1, key, iv); 
			printf("%s\n", "Pase BytesToKey"); 
			EVP_CIPHER_CTX_init(&ctx); 
			printf("%s\n", "Pase cypher init"); 
			EVP_DecryptInit_ex(&ctx, EVP_aes_192_cbc(), NULL, key, iv); 
			printf("%s\n", "Pase encrypt init"); 
			break; 
		case AES256: break; 
		default: 
			return 1; 
	} 
	EVP_DecryptUpdate(&ctx, out, &outl, cipherdata, len); 
	printf("%s\n", "Pase encrypt update"); 
	EVP_DecryptFinal_ex(&ctx, out + outl, &templ); 
	printf("%s\n", "Pase encrypt final"); 
	outl +=templ; 
	printf("templ: %u\n", templ);
	printf("len: %u\n", len);
	printf("outl: %u\n", outl);
	memcpy(plaindata, out, outl); 
	EVP_CIPHER_CTX_cleanup(&ctx); 
	printf("%s\n", "Pase cipher cleanup");

	return 0;
}

int encrypt (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata) {
	EVP_CIPHER_CTX ctx;
	BYTE out[len]; 
	DWORD outl, templ;
    BYTE key[KEY_SIZE];
    BYTE iv[KEY_SIZE];

	printf("Cipher pass:%s\n", cipher->pass); 
	printf("Algorithm: %s\n", cipher->alg == AES192 ? "AES192" : "?"); 
	printf("Mode: %s\n", cipher->mode == CBC ? "CBC" : "?"); 

	switch (cipher->alg) { 
		case AES128: break; 
		case AES192: 
			EVP_BytesToKey(EVP_aes_192_cbc(), EVP_md5(), NULL, cipher->pass, strlen(cipher->pass),1, key, iv); 
			printf("%s\n", "Pase BytesToKey"); 
			EVP_CIPHER_CTX_init(&ctx); 
			printf("%s\n", "Pase cypher init"); 
			EVP_EncryptInit_ex(&ctx, EVP_aes_192_cbc(), NULL, key, iv); 
			printf("%s\n", "Pase encrypt init"); 
			break; 
		case AES256: break; 
		default: 
			return 1; 
	} 
	EVP_EncryptUpdate(&ctx, out, &outl, cipherdata, len); 
	printf("%s\n", "Pase encrypt update"); 
	EVP_EncryptFinal_ex(&ctx, out + outl, &templ); 
	printf("%s\n", "Pase encrypt final"); 
	outl +=templ; 
	printf("len: %u\n", len);
	printf("outl: %u\n", outl);
	memcpy(plaindata, out, outl); 
	EVP_CIPHER_CTX_cleanup(&ctx); 
	printf("%s\n", "Pase cipher cleanup");

	return 0;
}

void freeCipherStr(CIPHERSTR* cipher) {
	if (cipher == NULL)
		return;
	if (cipher->pass != NULL)
		free(cipher->pass);
	free(cipher);
}