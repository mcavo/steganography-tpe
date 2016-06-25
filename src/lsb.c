#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lsb.h"

int getextensionindex(char* word);
DWORD getLen(FILE* file);

//LSB1 & LSB4
void lsbEmbed(WAVSTR* wav, BYTE* data, DWORD len, BYTE n) {
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

void lsbeEmbed(WAVSTR* wav, BYTE* data, DWORD len) {
	unsigned long i, j, k;	
	BYTE byte, mask;
	BYTE initmask = 0b10000000;
	mask = initmask;
	for( i=0, j=0, k=0 ; i<wav->data.chunkSize && j < len; i++ ) {
		if ( wav->data.soundData[i] >= (unsigned char) 0b11111110 ) {
			wav->data.soundData[i] = wav->data.soundData[i] & 0b11111110;
			byte = data[j] & mask;
			byte = byte >> (7 - k);
			wav->data.soundData[i] = wav->data.soundData[i] | byte;
			if ( mask == 0b00000001 ) {
				mask = initmask;
				j++;
				k = 0;
			} else {
				mask = mask >> 1;
				k++;
			}
		}
	}
}

int lsbEmbedWrapper(EMBEDSTR* emb) {
	int n, len;
	unsigned long bytesPerSample = emb->wav->fmt.wBitsPerSample / 8;
	if (emb->tech == LSB1)
		n = 1;
	else
		n = 4;

	FILE* file = fopen(emb->infile, "rb");
	DWORD filelen = getLen(file);

	int extindex = getextensionindex(emb->infile);
	int extlen = strlen(emb->infile + extindex) + 1;

	int datalen = sizeof(DWORD) + filelen + extlen;

	if ( (emb->wav->data.chunkSize / bytesPerSample)  < datalen * (8/n))
		return ERROR_NOT_ENOUGH_SPACE;

	BYTE* data = malloc(datalen);
	BYTE* embeddata;

	if (data == NULL) {
		fclose(file);
		return ERROR_OUT_OF_MEMORY;
	}

	DWORDTobigEndianBITEArray(filelen, data);

	fread(data + sizeof(DWORD), filelen, 1, file);
	fclose(file);

	memcpy(data + sizeof(DWORD) + filelen, emb->infile + extindex, extlen);

	if (emb->cipher == NULL) {

		embeddata = data;
		len = datalen;

	} else {

		embeddata = malloc(sizeof(DWORD) + datalen + CIPHER_BLOCK_SIZE);
		if (data == NULL) {
			free(data);
			return ERROR_OUT_OF_MEMORY;
		}

		encrypt(emb->cipher, data, datalen, embeddata, &len);

		free(data);

	}

	lsbEmbed(emb->wav, embeddata, len, n);
	free(embeddata);
	writeWavFile(emb->stegowav, emb->wav);
	
	return OK;
}

int lsbeEmbedWrapper(EMBEDSTR* emb) {

	FILE* file = fopen(emb->infile, "rb");
	DWORD filelen = getLen(file);
	int extindex = getextensionindex(emb->infile);
	DWORD extlen = strlen(emb->infile + extindex) + 1;
	unsigned long i;
	unsigned long availableBytes=0;

	for( i=0 ; i<emb->wav->data.chunkSize ; i++ ) {
		if ( emb->wav->data.soundData[i] >= 0b11111110 ) {
			availableBytes++; 
		}
	}

	DWORD datalen = filelen + sizeof(DWORD) + extlen;
	DWORD len;

	if ( availableBytes <  datalen * 8 ) {
		fclose(file);
		return ERROR_NOT_ENOUGH_SPACE;
	}


	BYTE* data = malloc(datalen);
	BYTE* embeddata;
	DWORDTobigEndianBITEArray(filelen, data);

	fread(data + sizeof(DWORD), filelen, 1, file);
	fclose(file);

	memcpy(data + sizeof(DWORD) + filelen, emb->infile + extindex, extlen);

	if (emb->cipher == NULL) {

		embeddata = data;
		len = datalen;

	} else {

		embeddata = malloc(sizeof(DWORD) + datalen + CIPHER_BLOCK_SIZE);
		if (data == NULL) {
			free(data);
			return ERROR_OUT_OF_MEMORY;
		}

		encrypt(emb->cipher, data, datalen, embeddata, &len);

		free(data);

	}

	lsbeEmbed(emb->wav, embeddata, len);
	free(embeddata);
	writeWavFile(emb->stegowav, emb->wav);

	return OK;
}

int lsbeExtractWrapper(EXTRACTSTR* ext) {
	BYTE bufferExtension[30];
	BYTE bufferDWORD[4];
	unsigned long i, j, k;	
	BYTE byte;
	DWORD len, fileLen;
	bufferDWORD[0] = 0;

	for( i=0, j=0, k=0 ; i<ext->wav->data.chunkSize && j < 4; i++ ) {
		if ( ext->wav->data.soundData[i] >= 0b11111110 ) {
			byte = ext->wav->data.soundData[i] & 0x01;
			byte = byte << (7 - k);
			bufferDWORD[j] = bufferDWORD[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				j++;
				if (j<4)
					bufferDWORD[j] = 0;

			}
		}
	}

	len = bigEndianBITEArrayToDWORD(bufferDWORD);
	printf("len: %u\n", len);
	BYTE bufferData[len];
	BYTE bufferFile[len];
	bufferData[0] = 0;
	for( j=0, k=0 ; i<ext->wav->data.chunkSize && j < len; i++) {
		if ( ext->wav->data.soundData[i] >= 0xFE ) {
			byte = ext->wav->data.soundData[i] & 0x01;
			byte = byte << (7 - k);
			bufferData[j] = bufferData[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				j++;
				if (j < len)
					bufferData[j] = 0;
			}
		}
	}	

	if (ext->cipher == NULL) {

		memcpy(bufferFile, bufferData, len);
		fileLen = len;
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

	} else {
		int error = decrypt (ext->cipher, bufferData, len, bufferFile);
		if (error) {
			return !OK;
		}
		fileLen = bigEndianBITEArrayToDWORD(bufferFile);
		memcpy(bufferFile, bufferFile+4, fileLen);
		for (j=0 ; bufferFile[j+fileLen]!=0 ; j++) {
			bufferExtension[j] = bufferFile[j+fileLen];
		}
		bufferExtension[j++] = 0;

	}

	char* filename = malloc(strlen(ext->outfile) + j);
	if( filename == NULL ) {
		return ERROR_OUT_OF_MEMORY;
	}

	memcpy(filename, ext->outfile, strlen(ext->outfile));
	memcpy(filename + strlen(ext->outfile), bufferExtension, j);

	FILE* fptr = fopen(filename,"wb");
	fwrite(bufferFile,sizeof(BYTE),fileLen,fptr);
	fclose(fptr);
	free(filename);
	return OK;
}

int lsbExtractWrapper(EXTRACTSTR* ext) {
	BYTE removemask;
	int n;
	if (ext->tech == LSB1) {
		n = 1;
		removemask = 0x01;
	} else {
		n = 4;
		removemask = 0x0F;
	}
	BYTE byte;
	DWORD len, filelen;
	BYTE bufferDWORD[4];
	BYTE bufferExtension[30];
	DWORD i,j;
	DWORD bytesPerSample = ext->wav->fmt.wBitsPerSample / 8;
	unsigned long sampleCounter = 1;
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
	BYTE bufferData[len];
	BYTE bufferFile[len]; 
	for(i=0 ; i<len; i++) {
		bufferData[i]=0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferData[i] = bufferData[i] | byte;
			sampleCounter++;
		}
	}

	if (ext->cipher == NULL) {

		memcpy(bufferFile, bufferData, len);
		filelen = len;

		i=0;
		do {
			bufferExtension[i] = 0;
			for(j=0; j<8 ; j+=n){
				byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
				byte = byte << (8-n-j);
				bufferExtension[i] = bufferExtension[i] | byte;
				sampleCounter++;
			}
			printf("%c", bufferExtension[i]);
			i++;
		} while( i<30 && bufferExtension[i-1]!=0);

	} else {
		int error = decrypt (ext->cipher, bufferData, len, bufferFile);
		if (error != OK) {
			return !OK;
		}
		filelen = bigEndianBITEArrayToDWORD(bufferFile);
		printf("%u\n", filelen);
		for (i=0 ; bufferFile[i + filelen + sizeof(DWORD)] != 0 ; i++) {
			bufferExtension[i] = bufferFile[i + filelen + sizeof(DWORD)];
			printf("%c", bufferExtension[i]);
		}
		bufferExtension[i++] = 0;
		printf("\n%u\n", i + filelen + sizeof(DWORD));

		for(j=0 ; j<filelen ; j++) {
			bufferFile[j] = bufferFile[j+4];
		}
	}

	char* filename = malloc(strlen(ext->outfile) + i);
	memcpy(filename, ext->outfile, strlen(ext->outfile));
	memcpy(filename + strlen(ext->outfile), bufferExtension, i);

	printf("%s\n", filename);

	FILE* fptr = fopen(filename,"wb");
	fwrite(bufferFile,sizeof(BYTE),filelen,fptr);
	fclose(fptr);

	return OK;
}

int lsbFitsExtract(EXTRACTSTR* ext, DWORD* availableBytes, DWORD* requiredBytes) {
	*availableBytes = ext->wav->data.chunkSize / (ext->wav->fmt.wBitsPerSample / 8);
	BYTE removemask;
	int n;
	if (ext->tech == LSB1) {
		n = 1;
		removemask = 0x01;
	} else {
		n = 4;
		removemask = 0x0F;
	}
	BYTE byte;
	BYTE bufferDWORD[4];
	unsigned long i,j;
	DWORD bytesPerSample = ext->wav->fmt.wBitsPerSample / 8;
	unsigned long sampleCounter = 1;
	for(i=0 ; i<4; i++) {
		bufferDWORD[i] = 0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferDWORD[i] = bufferDWORD[i] | byte;
			sampleCounter++;
		}
	}
	*requiredBytes = (bigEndianBITEArrayToDWORD(bufferDWORD) + sizeof(DWORD)) * (8/n);
	return *availableBytes >= *requiredBytes;
}

int lsbeFitsExtract(EXTRACTSTR* ext, DWORD* availableBytes, DWORD* requiredBytes) {
	BYTE bufferDWORD[4];
	DWORD i, j, k;	
	BYTE byte;
	DWORD a = 0;
	for( i=0 ; i<ext->wav->data.chunkSize ; i++ ) {
		if ( ext->wav->data.soundData[i] >= 0xFE ) {
			a++; 
		}
	}
	*availableBytes = a;
	bufferDWORD[0] = 0;
	for( i=0, j=0, k=0 ; i<ext->wav->data.chunkSize && j < 4; i++ ) {
		if ( ext->wav->data.soundData[i] >= 0xFE ) {
			byte = ext->wav->data.soundData[i] & 0x01;
			byte = byte << (7 - k);
			bufferDWORD[j] = bufferDWORD[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				j++;
				if (j<4)
					bufferDWORD[j] = 0;

			}
		}
	}

	*requiredBytes = (bigEndianBITEArrayToDWORD(bufferDWORD) + sizeof(DWORD)) * 8;

	return *availableBytes >= *requiredBytes;
}

int lsbExtractContent(EXTRACTSTR* ext) {
	BYTE removemask;
	int n;
	if (ext->tech == LSB1) {
		n = 1;
		removemask = 0x01;
	} else {
		n = 4;
		removemask = 0x0F;
	}
	BYTE byte;
	DWORD len;
	BYTE bufferDWORD[4];
	DWORD i,j;
	DWORD bytesPerSample = ext->wav->fmt.wBitsPerSample / 8;
	unsigned long sampleCounter = 1;
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
	BYTE bufferData[len]; 
	for(i=0 ; i<len; i++) {
		bufferData[i]=0;
		for(j=0; j<8 ; j+=n){
			byte = ext->wav->data.soundData[sampleCounter*bytesPerSample-1] & removemask;
			byte = byte << (8-n-j);
			bufferData[i] = bufferData[i] | byte;
			sampleCounter++;
		}
	}
	FILE* fptr = fopen(ext->outfile,"wb");
	fwrite(bufferData,sizeof(BYTE),len,fptr);
	fclose(fptr);
	return OK;
}

int lsbeExtractContent(EXTRACTSTR* ext) {
	BYTE bufferDWORD[4];
	unsigned long i, j, k;	
	BYTE byte;
	DWORD len;
	bufferDWORD[0] = 0;
	for( i=0, j=0, k=0 ; i<ext->wav->data.chunkSize && j < 4; i++ ) {
		if ( ext->wav->data.soundData[i] >= 0xFE ) {
			byte = ext->wav->data.soundData[i] & 0x01;
			byte = byte << (7 - k);
			bufferDWORD[j] = bufferDWORD[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				j++;
				if (j<4)
					bufferDWORD[j] = 0;
			}
		}
	}
	len = bigEndianBITEArrayToDWORD(bufferDWORD);
	BYTE bufferData[len];
	bufferData[0] = 0;
	for( j=0, k=0 ; i<ext->wav->data.chunkSize && j < len; i++) {
		if ( ext->wav->data.soundData[i] >= 0b11111110 ) {
			byte = ext->wav->data.soundData[i] & 0b00000001;
			byte = byte << (7 - k);
			bufferData[j] = bufferData[j] | byte;
			k++;
			if ( k == 8 ) {
				k = 0;
				j++;
				if (j < len)
					bufferData[j] = 0;
			}
		}
	}
	FILE* fptr = fopen(ext->outfile,"wb");
	fwrite(bufferData,sizeof(BYTE),len,fptr);
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

void freeEmbedStr(EMBEDSTR* emb) {
	if (emb == NULL)
		return;
	if (emb->infile != NULL)
		free(emb->infile);
	if (emb->stegowav != NULL)
		free(emb->stegowav);
	freeWavStr(emb->wav);
	freeCipherStr(emb->cipher);
	free(emb);
}

void freeExtractStr(EXTRACTSTR* ext) {
	if (ext == NULL)
		return;
	if (ext->outfile != NULL)
		free(ext->outfile);
	freeWavStr(ext->wav);
	freeCipherStr(ext->cipher);
	free(ext);
}
