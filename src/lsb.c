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
	unsigned long sampleCounter = 0;
	for(i=0 ; i<len ; i++) {
		mask = initmask;
		for(j=0 ; j<8 ; j+=n) {
			byte = mask & data[i];
			byte = byte >> (8-n-j);
			wav->data.soundData[sampleCounter*bytesPerSample] = wav->data.soundData[sampleCounter*bytesPerSample] & removemask; // corregir esto
			wav->data.soundData[sampleCounter*bytesPerSample] = wav->data.soundData[sampleCounter*bytesPerSample] | byte;
			sampleCounter++;
			mask = mask >> n;
		}
	}
}

int lsbEncryptWrapper(EMBEDSTR* emb) {
	if (emb->tech == LSB1 || emb->tech == LSB4) {
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
		DWORDTolittleEndianBITEArray(filelen, bufferDWORD);
		if ( (emb->wav->data.chunkSize / bytesPerSample)  < (filelen + 4 + strlen(emb->infile + extindex) + 1) * (8/n))
			return 1;
		int datalen = filelen + 4 + strlen(emb->infile + extindex) + 1;
		BYTE* data = malloc(filelen + 4 + strlen(emb->infile + extindex) + 1);
		memcpy(data, bufferDWORD, sizeof(bufferDWORD));
		int read = 0;
		read = fread(data + sizeof(bufferDWORD), filelen, 1, file);
		memcpy(data + sizeof(bufferDWORD) + filelen, emb->infile + extindex, strlen(emb->infile + extindex) + 1);
		lsbEncrypt(emb->wav, data, filelen + 4 + strlen(emb->infile + extindex) + 1, n);
		free(data);
		fclose(file);
		writeWavFile(emb->stegowav, emb->wav);
	}
	return 2;
}

int lsbDecryptWrapper(EXTRACTSTR* ext) {
	if (ext->tech == LSB1 || ext->tech == LSB4) {
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
		unsigned long sampleCounter = 0;
		for(i=0 ; i<4; i++) {
			bufferDWORD[i] = 0;
			for(j=0; j<8 ; j+=n){
				byte = ext->wav->data.soundData[sampleCounter*bytesPerSample] & removemask;
				byte = byte << (8-n-j);
				bufferDWORD[i] = bufferDWORD[i] | byte;
				sampleCounter++;
			}
		}
		len = littleEndianBITEArrayToDWORD(bufferDWORD);
		BYTE bufferFile[len];
		for(i=0 ; i<len; i++)
			bufferFile[i] = 0;
		for(i=0 ; i<len; i++) {
			bufferFile[i]=0;
			for(j=0; j<8 ; j+=n){
				byte = ext->wav->data.soundData[sampleCounter*bytesPerSample] & removemask;
				byte = byte << (8-n-j);
				bufferFile[i] = bufferFile[i] | byte;
				sampleCounter++;
			}
		}
			
		i=0;
		do {
			bufferExtension[i] = 0;
			for(j=0; j<8 ; j+=n){
				byte = ext->wav->data.soundData[sampleCounter*bytesPerSample] & removemask;
				byte = byte << (8-n-j);
				bufferExtension[i] = bufferExtension[i] | byte;
				sampleCounter++;
			}
		} while( i<30 && bufferExtension[i++]!=0);

		char* filename = malloc(strlen(ext->outfile) + i);
		memcpy(filename, ext->outfile, strlen(ext->outfile));
		memcpy(filename + strlen(ext->outfile), bufferExtension, i);

		FILE* fptr = fopen(filename,"wb");
		fwrite(bufferFile,sizeof(BYTE),len,fptr);
		fclose(fptr);
		return 1;
	}

	return 1;

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