#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"

int getextensionindex(char* word);
DWORD getLen(FILE* file);

//LSB1 & LSB4
void lsbEncrypt(WAVSTR* wav, BYTE* data, DWORD len, BYTE n) {
	BYTE initmask, removemask;
	if (n == 1) {
		initmask = 0b10000000;
		removemask = 0b11111110;
	} else {
		initmask = 0b11110000;
		removemask = 0b11110000;
	}
	BYTE byte, mask;
	unsigned long bytesPerSample = wav->fmt.wBitsPerSample / 8;
	unsigned long i,j;
	unsigned long sampleCounter = 1;
	for(i=0 ; i<len ; i++) {
		mask = initmask;
		for(j=0 ; j<8 ; j+=n) {
			byte = mask & data[i];
			byte = byte >> (8-n-j);
			wav->data.soundData[sampleCounter*bytesPerSample - 1] = wav->data.soundData[sampleCounter*bytesPerSample - 1] & removemask;
			wav->data.soundData[sampleCounter*bytesPerSample - 1] = wav->data.soundData[sampleCounter*bytesPerSample - 1] | byte;
			sampleCounter++;
			mask = mask >> n;
		}
	}
}

void lsbeEncrypt(WAVSTR* wav, BYTE* data, DWORD len) {
	unsigned long i, j, k;	
	BYTE byte, mask;
	BYTE initmask = 0b10000000;
	mask = initmask;

	for( i=0, j=0, k=0 ; i<wav->data.chunkSize && j < len; i++ ) {
		if ( wav->data.soundData[i] >= (unsigned char) 0b11111110 ) {
			wav->data.soundData[i] = wav->data.soundData[i] & 0b11111110;
			byte = data[j] & mask;
			byte = byte >> (7 - k);
			k++;
			wav->data.soundData[i] = wav->data.soundData[i] | byte;
			if ( mask == 0b00000001 ) {
				mask = initmask;
				j++;
				k = 0;
			} else {
				mask = mask >> 1;
			}
			
		}
	}
}

int lsbEncryptWrapper(EMBEDSTR* emb) {
	int i, n;
	if (emb->tech == LSB1)
		n = 1;
	else
		n = 4;
	unsigned long bytesPerSample = emb->wav->fmt.wBitsPerSample / 8;
	FILE* file = fopen(emb->infile, "rb");
	DWORD filelen = getLen(file);
	int extindex = getextensionindex(emb->infile);
	BYTE bufferDWORD[4];
	DWORDTobigEndianBITEArray(filelen, bufferDWORD);
	if ( (emb->wav->data.chunkSize / bytesPerSample)  < (filelen + 4 + strlen(emb->infile + extindex) + 1) * (8/n))
		return NOT_ENOUGH_SPACE;
	int datalen = filelen + 4 + strlen(emb->infile + extindex) + 1;
	BYTE* data = malloc(datalen);
	if (data == NULL) {
		fclose(file);
		return OUT_OF_MEMORY;
	}
	memcpy(data, bufferDWORD, sizeof(bufferDWORD));
	int read = 0;
	read = fread(data + sizeof(bufferDWORD), filelen, 1, file);
	memcpy(data + sizeof(bufferDWORD) + filelen, emb->infile + extindex, strlen(emb->infile + extindex) + 1);
	lsbEncrypt(emb->wav, data, filelen + 4 + strlen(emb->infile + extindex) + 1, n);
	free(data);
	fclose(file);
	writeWavFile(emb->stegowav, emb->wav);
	
	return OK;
}

int lsbeEncryptWrapper(EMBEDSTR* emb) {

	FILE* file = fopen(emb->infile, "rb");
	DWORD filelen = getLen(file);
	int extindex = getextensionindex(emb->infile);
	BYTE bufferDWORD[4];
	DWORDTobigEndianBITEArray(filelen, bufferDWORD);
	int i;
	unsigned long availableBytes=0;
	for( i=0 ; i<emb->wav->data.chunkSize ; i++ ) {
		if ( emb->wav->data.soundData[i] >= 0b11111110 ) {
			availableBytes++; 
		}
	}
	printf("Available: %lu\n", availableBytes);
	printf("Requested: %lu\n", (filelen + 4 + strlen(emb->infile + extindex) + 1) * 8);
	if ( availableBytes <  (filelen + 4 + strlen(emb->infile + extindex) + 1) * 8 ) {
		fclose(file);
		return NOT_ENOUGH_SPACE;
	}
	int datalen = filelen + 4 + strlen(emb->infile + extindex) + 1;
	BYTE* data = malloc(datalen);
	memcpy(data, bufferDWORD, sizeof(bufferDWORD));
	int read = 0;
	read = fread(data + sizeof(bufferDWORD), filelen, 1, file);
	memcpy(data + sizeof(bufferDWORD) + filelen, emb->infile + extindex, strlen(emb->infile + extindex) + 1);
	lsbeEncrypt(emb->wav, data, datalen);
	free(data);
	fclose(file);
	writeWavFile(emb->stegowav, emb->wav);

	return OK;

}

int lsbeDecryptWrapper(EXTRACTSTR* ext) {
	if (ext == NULL)
		printf("%s\n", "Holis");
	BYTE bufferExtension[30];
	BYTE bufferDWORD[4];
	unsigned long i, j, k;	
	BYTE byte, mask;
	BYTE initmask = 0b10000000;
	DWORD len;
	mask = initmask;
	bufferDWORD[0] = 0;
	for( i=0, j=0, k=0 ; i<ext->wav->data.chunkSize && j < 4; i++ ) {
		if ( ext->wav->data.soundData[i] >= 0b11111110 ) {
			byte = ext->wav->data.soundData[i] & 0b00000001;
			byte = byte << (7 - k);
			bufferDWORD[j] = bufferDWORD[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				if (j<3)
					bufferDWORD[++j] = 0;
			}
		}
	}

	len = bigEndianBITEArrayToDWORD(bufferDWORD);
	printf("Len: %u\n", len);
	BYTE bufferFile[len];
	bufferFile[0] = 0;
	for( j=0, k=0 ; i<ext->wav->data.chunkSize && j < len; i++) {
		if ( ext->wav->data.soundData[i] >= 0b11111110 ) {
			byte = ext->wav->data.soundData[i] & 0b00000001;
			byte = byte << (7 - k);
			bufferDWORD[j] = bufferDWORD[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				if (j < len - 1)
					bufferFile[++j] = 0;
			}
		}
	}	

	j = 0;
	do {
		bufferExtension[j] = 0;
		for ( k=0 ; i<ext->wav->data.chunkSize && k < 8 ; i++ ) {
			if ( ext->wav->data.soundData[i] >= 0b11111110 ) {
				byte = ext->wav->data.soundData[i] & 0b00000001;
				byte = byte << (7 - k);
				bufferExtension[j] = bufferExtension[j] | byte;
				k++;
			}
		}
	} while (i<ext->wav->data.chunkSize && j<30 && bufferExtension[j++]!=0);

	printf("Extention: %s\n", bufferExtension);

	char* filename = malloc(strlen(ext->outfile) + j);
	if( filename == NULL ) {
		return OUT_OF_MEMORY;
	}
	memcpy(filename, ext->outfile, strlen(ext->outfile));
	memcpy(filename + strlen(ext->outfile), bufferExtension, j);

	printf("Filename: %s\n", filename);

	FILE* fptr = fopen(filename,"wb");
	fwrite(bufferFile,sizeof(BYTE),len,fptr);
	fclose(fptr);
	free(filename);
	return OK;

}

int lsbDecryptWrapper(EXTRACTSTR* ext) {
	/* Get len */
	BYTE initmask, removemask;
	int n;
	if (ext->tech == LSB1) {
		n = 1;
		initmask = 0b10000000;
		removemask = 0b00000001;
	} else {
		n = 4;
		initmask = 0b11110000;
		removemask = 0b00001111;
	}
	BYTE byte, mask;
	BYTE bufferDWORD[4];
	unsigned long i,j;
	BYTE bufferExtension[30];
	DWORD len;
	unsigned long bytesPerSample = ext->wav->fmt.wBitsPerSample / 8;
	unsigned long sampleCounter = 1;
	printf("%s\n", "Holis");
	for(i=0 ; i<4; i++) {
		bufferDWORD[i] = 0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferDWORD[i] = bufferDWORD[i] | byte;
			sampleCounter++;
		}
	}
	len = bigEndianBITEArrayToDWORD(bufferDWORD);
	printf("Len: %u\n", len);
	BYTE bufferFile[len];
	for(i=0 ; i<len; i++)
		bufferFile[i] = 0;
	for(i=0 ; i<len; i++) {
		bufferFile[i]=0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferFile[i] = bufferFile[i] | byte;
			sampleCounter++;
		}
	}
	printf("%s\n", "Holis");	
	i=0;
	do {
		bufferExtension[i] = 0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferExtension[i] = bufferExtension[i] | byte;
			sampleCounter++;
		}
	} while( i<30 && bufferExtension[i++]!=0);
	printf("Extention: %s\n", bufferExtension);
	char* filename = malloc(strlen(ext->outfile) + i);
	if( filename == NULL ) {
		return OUT_OF_MEMORY;
	}
	memcpy(filename, ext->outfile, strlen(ext->outfile));
	memcpy(filename + strlen(ext->outfile), bufferExtension, i);

	printf("Filename: %s\n", filename);

	FILE* fptr = fopen(filename,"wb");
	fwrite(bufferFile,sizeof(BYTE),len,fptr);
	fclose(fptr);
	return OK;

}

int getextensionindex(char* word) {
	int i;
	for ( i = strlen(word) - 1 ; i >= 0 ; i -- ) {
		if ( word[i] == '.' ) {
			return i;
		}
	}
	return i;
}

DWORD getLen(FILE* file) {
	DWORD len;
	fseek(file, 0L, SEEK_END);
	len = ftell(file);
	rewind(file);
	return len;
}