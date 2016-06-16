#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include "../include/ciphermanager.h"

typedef const EVP_CIPHER*(*evp_cipher_function)();

int decrypt_openssl(evp_cipher_function function, CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata);
int encrypt_openssl(evp_cipher_function function, CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen);

int decrypt_aes128 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata);
int decrypt_aes192 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata);
int decrypt_aes256 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata);
int decrypt_des (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata);

int encrypt_aes128 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen);
int encrypt_aes192 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen);
int encrypt_aes256 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen);
int encrypt_des (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen);

int decrypt_openssl(evp_cipher_function function, CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	EVP_CIPHER_CTX ctx;
	BYTE out[len]; 
	DWORD outl, templ;
	unsigned int keyLen, ivLen;
	keyLen = EVP_CIPHER_key_length(function());
	ivLen = EVP_CIPHER_iv_length(function());
	BYTE key[keyLen];
	BYTE iv[ivLen];
	EVP_BytesToKey(function(), EVP_md5(), NULL, cipher->pass, strlen(cipher->pass),1, key, iv);
	EVP_CIPHER_CTX_init(&ctx);
	EVP_DecryptInit_ex(&ctx, function(), NULL, key, iv);
	EVP_DecryptUpdate(&ctx, out, &outl, cipherdata, len);
	EVP_DecryptFinal_ex(&ctx, out + outl, &templ); 
	outl +=templ;
	memcpy(plaindata, out, outl);
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 0;
}

int encrypt_openssl(evp_cipher_function function, CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	EVP_CIPHER_CTX ctx;
	BYTE out[len + CIPHER_BLOCK_SIZE];
	DWORD templ;
	unsigned int keyLen, ivLen;
	keyLen = EVP_CIPHER_key_length(function());
	ivLen = EVP_CIPHER_iv_length(function());
	BYTE key[keyLen];
	BYTE iv[ivLen];

	EVP_BytesToKey(function(), EVP_md5(), NULL, cipher->pass, strlen(cipher->pass),1, key, iv);
	EVP_CIPHER_CTX_init(&ctx);
	EVP_EncryptInit_ex(&ctx, function(), NULL, key, iv);
	EVP_EncryptUpdate(&ctx, out, cipherdatalen, cipherdata, len);
	EVP_EncryptFinal_ex(&ctx, out + *cipherdatalen, &templ);
	*cipherdatalen +=templ;

	DWORDTobigEndianBITEArray(*cipherdatalen, plaindata);
	memcpy(plaindata + sizeof(DWORD), out, *cipherdatalen); 
	*cipherdatalen +=sizeof(DWORD);
	EVP_CIPHER_CTX_cleanup(&ctx);
	return 0;
} 

int decrypt (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	switch (cipher->alg) { 
		case AES128: 
			return decrypt_aes128(cipher, cipherdata, len, plaindata); 
		case AES192: 
			return decrypt_aes192(cipher, cipherdata, len, plaindata);
		case AES256: 
			return decrypt_aes256(cipher, cipherdata, len, plaindata); 
		default: 
			return decrypt_des(cipher, cipherdata, len, plaindata);; 
	}
}

int encrypt (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	switch (cipher->alg) { 
		case AES128: 
			return encrypt_aes128(cipher, cipherdata, len, plaindata, cipherdatalen); 
		case AES192: 
			return encrypt_aes192(cipher, cipherdata, len, plaindata, cipherdatalen);
		case AES256: 
			return encrypt_aes256(cipher, cipherdata, len, plaindata, cipherdatalen); 
		default: 
			return encrypt_des(cipher, cipherdata, len, plaindata, cipherdatalen);; 
	}
}

int decrypt_aes128 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	switch (cipher->mode) { 
		case ECB: 
			return decrypt_openssl(EVP_aes_128_ecb, cipher, cipherdata, len, plaindata); 
		case CFB: 
			return decrypt_openssl(EVP_aes_128_cfb, cipher, cipherdata, len, plaindata);
		case OFB: 
			return decrypt_openssl(EVP_aes_128_ofb, cipher, cipherdata, len, plaindata); 
		default: 
			return decrypt_openssl(EVP_aes_128_cbc, cipher, cipherdata, len, plaindata); 
	}
}

int decrypt_aes192 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	switch (cipher->mode) { 
		case ECB: 
			return decrypt_openssl(EVP_aes_192_ecb, cipher, cipherdata, len, plaindata); 
		case CFB: 
			return decrypt_openssl(EVP_aes_192_cfb, cipher, cipherdata, len, plaindata);
		case OFB: 
			return decrypt_openssl(EVP_aes_192_ofb, cipher, cipherdata, len, plaindata); 
		default: 
			return decrypt_openssl(EVP_aes_192_cbc, cipher, cipherdata, len, plaindata); 
	}
}
 
int decrypt_aes256 (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	switch (cipher->mode) { 
		case ECB: 
			return decrypt_openssl(EVP_aes_256_ecb, cipher, cipherdata, len, plaindata); 
		case CFB: 
			return decrypt_openssl(EVP_aes_256_cfb, cipher, cipherdata, len, plaindata);
		case OFB: 
			return decrypt_openssl(EVP_aes_256_ofb, cipher, cipherdata, len, plaindata); 
		default: 
			return decrypt_openssl(EVP_aes_256_cbc, cipher, cipherdata, len, plaindata); 
	}
}

int decrypt_des (CIPHERSTR* cipher, BYTE* cipherdata, DWORD len, BYTE* plaindata) {
	switch (cipher->mode) { 
		case ECB: 
			return decrypt_openssl(EVP_des_ecb, cipher, cipherdata, len, plaindata); 
		case CFB: 
			return decrypt_openssl(EVP_des_cfb, cipher, cipherdata, len, plaindata);
		case OFB: 
			return decrypt_openssl(EVP_des_ofb, cipher, cipherdata, len, plaindata); 
		default: 
			return decrypt_openssl(EVP_des_cbc, cipher, cipherdata, len, plaindata); 
	}
} 

int encrypt_aes128 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	switch (cipher->mode) { 
		case ECB:
			return encrypt_openssl(EVP_aes_128_ecb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		case CFB: 
			return encrypt_openssl(EVP_aes_128_cfb, cipher, plaindata, len, cipherdata, cipherdatalen);
		case OFB: 
			return encrypt_openssl(EVP_aes_128_ofb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		default: 
			return encrypt_openssl(EVP_aes_128_cbc, cipher, plaindata, len, cipherdata, cipherdatalen); 
	}
} 

int encrypt_aes192 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	switch (cipher->mode) { 
		case ECB:
			return encrypt_openssl(EVP_aes_192_ecb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		case CFB: 
			return encrypt_openssl(EVP_aes_192_cfb, cipher, plaindata, len, cipherdata, cipherdatalen);
		case OFB: 
			return encrypt_openssl(EVP_aes_192_ofb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		default: 
			return encrypt_openssl(EVP_aes_192_cbc, cipher, plaindata, len, cipherdata, cipherdatalen); 
	}
}

int encrypt_aes256 (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	switch (cipher->mode) { 
		case ECB:
			return encrypt_openssl(EVP_aes_256_ecb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		case CFB: 
			return encrypt_openssl(EVP_aes_256_cfb, cipher, plaindata, len, cipherdata, cipherdatalen);
		case OFB: 
			return encrypt_openssl(EVP_aes_256_ofb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		default: 
			return encrypt_openssl(EVP_aes_256_cbc, cipher, plaindata, len, cipherdata, cipherdatalen); 
	}
} 

int encrypt_des (CIPHERSTR* cipher, BYTE* plaindata, DWORD len, BYTE* cipherdata, DWORD* cipherdatalen) {
	switch (cipher->mode) { 
		case ECB:
			return encrypt_openssl(EVP_des_ecb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		case CFB: 
			return encrypt_openssl(EVP_des_cfb, cipher, plaindata, len, cipherdata, cipherdatalen);
		case OFB: 
			return encrypt_openssl(EVP_des_ofb, cipher, plaindata, len, cipherdata, cipherdatalen); 
		default: 
			return encrypt_openssl(EVP_des_cbc, cipher, plaindata, len, cipherdata, cipherdatalen); 
	}
} 

void freeCipherStr(CIPHERSTR* cipher) {
	if (cipher == NULL)
		return;
	if (cipher->pass != NULL)
		free(cipher->pass);
	free(cipher);
}
