#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include "../include/ciphermanager.h"

// int encrypt (CIPHERSTR* cipherstr, BYTE* plaindata, DWORD len, BYTE* cipherdata) {
// 	switch (cipherstr->alg) {
// 		case AES128: 
// 			break;
// 		case AES192: 
// 			break;
// 		case AES256: 
// 			break;
// 		case DES: 
// 			break;
// 		default:
// 			return -1;
// 	}
// }

int decrypt (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	EVP_CIPHER_CTX ctx;
	BYTE out[len]; 
	DWORD outl, templ;
    BYTE key[KEY_SIZE];
    BYTE iv[KEY_SIZE];

	switch (cipher->alg) {
		case AES128: break;
		case AES192: 
			EVP_BytesToKey(EVP_aes_192_cbc(), EVP_md5(), NULL, cipher->pass, strlen(cipher->pass),1, key, iv);
			EVP_CIPHER_CTX_init(&ctx);
			EVP_EncryptInit_ex(&ctx, EVP_aes_192_cbc(), NULL, key, iv); 
		break;
		case AES256: break;
		default:
			return 1;
			break;
	}
	EVP_EncryptUpdate(&ctx, out, &outl, cipherdata, len); 
	EVP_EncryptFinal_ex(&ctx, out + outl, &templ);
	outl +=templ;
	memcpy(plaindata, out, outl);
	EVP_CIPHER_CTX_cleanup(&ctx);

	return 0;
}

void freeCipherStr(CIPHERSTR* cipher) {
	if (cipher == NULL)
		return;
	if (cipher->pass != NULL)
		free(cipher->pass);
	free(cipher);
}