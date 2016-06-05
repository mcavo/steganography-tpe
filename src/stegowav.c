#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*********************/
/*    BytesManager   */
/*********************/

typedef uint8_t		BYTE;
typedef uint16_t	WORD;
typedef uint32_t	DWORD;

WORD littleEndianBITEArrayToWORD(BYTE buffer[2]);
DWORD littleEndianBITEArrayToDWORD(BYTE buffer[4]);
void WORDTolittleEndianBITEArray(WORD x, BYTE buffer[2]);
void DWORDTolittleEndianBITEArray(DWORD x, BYTE buffer[4]);

DWORD
littleEndianBITEArrayToDWORD(BYTE buffer[4]) {
	return buffer[0] | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
}

WORD
littleEndianBITEArrayToWORD(BYTE buffer[2]) {
	return buffer[0] | (buffer[1]<<8);
}

void
DWORDTolittleEndianBITEArray(DWORD x, BYTE buffer[4]) {
	buffer[0] = (BYTE)(x >> 0);
	buffer[1] = (BYTE)(x >> 8);
	buffer[2] = (BYTE)(x >> 16);
	buffer[3] = (BYTE)(x >> 24);
}

void
WORDTolittleEndianBITEArray(WORD x, BYTE buffer[2]) {
	buffer[0] = (BYTE)(x >> 0);
	buffer[1] = (BYTE)(x >> 8);
}

/*********************/
/*     WavManager    */
/*********************/

typedef DWORD 		CKSIZE;			// 32-bit unsigned size value

typedef struct {
    BYTE    chunkID[4];				// 'RIFF'
    CKSIZE  chunkSize;				// File Size
    BYTE    format[4];				// Format: 'WAVE'
} RIFF_CK;

typedef struct {
	BYTE	chunkID[4];				// 'fmt '
	CKSIZE	chunkSize;				// 16 para PCM.Size of rest of subchunk. /* Common fields */
	WORD 	wFormatTag;				// Format category,i.e.:PCM = 1 (no compres.)
	WORD 	wChannels;				// Number of channels:1, mono; 2, stereo DWORD dwSamplesPerSec; // Sampling rate: Mhz
	DWORD 	dwAvgSamplePerSec;
	DWORD 	dwAvgBytesPerSec;
	WORD 	wBlockAlign;
	WORD 	wBitsPerSample;			// 8, 16, etc.
} FMT_CK;

typedef struct {
	BYTE 	chunkID[4];				// 'data'
	CKSIZE 	chunkSize;
	BYTE* 	soundData;
} DATA_CK;

typedef struct {
	RIFF_CK riff_desc;				// MANDATORY
	FMT_CK fmt;
	DATA_CK data;					// Wave Data Chunk MANDATORY 
} WAVSTR;

int writeWavFile(char* wavfile, WAVSTR* wavstr);
int getWavStr(char* wavfile, WAVSTR* wavstr);

int
getWavStr(char* wavname, WAVSTR* wavstr) {
	FILE* wavfile = fopen(wavname, "rb");
	if (wavfile == NULL) {
		printf("Error opening file\n");
		return 0;
	}
	if (wavstr == NULL) {
		fclose(wavfile);
		return 0;
	}
	BYTE bufferWORD[2];
	BYTE bufferDWORD[4];

	int read = 0;

	read = fread(wavstr->riff_desc.chunkID, sizeof(wavstr->riff_desc.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->riff_desc.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(wavstr->riff_desc.format, sizeof(wavstr->riff_desc.format), 1, wavfile);

	read = fread(wavstr->fmt.chunkID, sizeof(wavstr->fmt.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wFormatTag = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wChannels = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.dwAvgSamplePerSec = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->fmt.dwAvgBytesPerSec = littleEndianBITEArrayToDWORD(bufferDWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wBlockAlign = littleEndianBITEArrayToWORD(bufferWORD);
	read = fread(bufferWORD, sizeof(bufferWORD), 1, wavfile);
	wavstr->fmt.wBitsPerSample = littleEndianBITEArrayToWORD(bufferWORD);

	read = fread(wavstr->data.chunkID, sizeof(wavstr->data.chunkID), 1, wavfile);
	read = fread(bufferDWORD, sizeof(bufferDWORD), 1, wavfile);
	wavstr->data.chunkSize = littleEndianBITEArrayToDWORD(bufferDWORD);
	wavstr->data.soundData = malloc(wavstr->data.chunkSize);
	if(wavstr->data.soundData == NULL) {
		fclose(wavfile);
		return 0;
	}
	read = fread(wavstr->data.soundData, wavstr->data.chunkSize, 1, wavfile);
	fclose(wavfile);
	return 1;

}

int
writeWavFile(char* wavname, WAVSTR* wavstr) {
	FILE* wavfile = fopen(wavname, "wb");
	if (wavfile == NULL) {
		printf("Error opening file\n");
		return 0;
	}
	if (wavstr == NULL) {
		fclose(wavfile);
		return 0;
	}
	BYTE bufferWORD[2];
	BYTE bufferDWORD[4];

	fwrite(wavstr->riff_desc.chunkID,sizeof(BYTE),sizeof(wavstr->riff_desc.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->riff_desc.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	fwrite(wavstr->riff_desc.format,sizeof(BYTE),sizeof(wavstr->riff_desc.format),wavfile);

	fwrite(wavstr->fmt.chunkID,sizeof(BYTE),sizeof(wavstr->fmt.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wFormatTag, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wChannels, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.dwAvgSamplePerSec, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->fmt.dwAvgBytesPerSec, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wBlockAlign, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);
	WORDTolittleEndianBITEArray(wavstr->fmt.wBitsPerSample, bufferWORD);
	fwrite(bufferWORD,sizeof(BYTE),sizeof(bufferWORD),wavfile);

	fwrite(wavstr->data.chunkID,sizeof(BYTE),sizeof(wavstr->data.chunkID),wavfile);
	DWORDTolittleEndianBITEArray(wavstr->data.chunkSize, bufferDWORD);
	fwrite(bufferDWORD,sizeof(BYTE),sizeof(bufferDWORD),wavfile);
	fwrite(wavstr->data.soundData,wavstr->data.chunkSize,1,wavfile);
	fclose(wavfile);
	return 1;

}

/*********************/
/*        LSB        */
/*********************/

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
		for(j=0 ; j<8/n ; j++) {
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

/*********************/
/*    Parse input    */
/*********************/

EMBEDSTR* parseEmbedStr(int argc, char **argv);
EXTRACTSTR* parseExtractStr(int argc, char **argv);

EMBEDSTR* parseEmbedStr(int argc, char **argv) {
	EMBEDSTR* emb = calloc(sizeof(EMBEDSTR),sizeof(char));
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-in")==0) {

				int len = strlen(argv[i+1]);
				emb->infile = malloc(sizeof(char)*(len+1));
				memcpy(emb->infile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-p")==0) {

				emb->wav = malloc(sizeof(WAVSTR));
				printf("%s\n", "Parsing wav...");
				getWavStr(argv[i+1],emb->wav);

			} else if (strcmp(argv[i],"-out")==0) {

				int len = strlen(argv[i+1]);
				emb->stegowav = malloc(sizeof(char)*(len+1));
				memcpy(emb->stegowav,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"LSB1")==0) {
					emb->tech = LSB1;
				} else if (strcmp(argv[i+1],"LSB4")==0) {
					emb->tech = LSB4;
				} else if (strcmp(argv[i+1],"LSBE")==0) {
					emb->tech = LSBE;
				} else {
					if(emb->infile!=NULL)
						free(emb->infile);
					if(emb->wav!=NULL)
						free(emb->wav);
					free(emb);
					return NULL;
				}
			}
		}
	}

	return emb;

}

EXTRACTSTR* parseExtractStr(int argc, char **argv) {

	EXTRACTSTR* ext = calloc(sizeof(EXTRACTSTR),sizeof(char));
	int i;
	for (i = 2 ; i < argc ; i++) {
		if (argv[i][0] == '-') {

			if (strcmp(argv[i],"-p")==0) {

				ext->wav = malloc(sizeof(WAVSTR));
				printf("%s\n", "Parsing wav...");
				getWavStr(argv[i+1],ext->wav);

			} else if (strcmp(argv[i],"-out")==0) {

				int len = strlen(argv[i+1]);
				ext->outfile = malloc(sizeof(char)*(len+1));
				memcpy(ext->outfile,argv[i+1],len+1);

			} else if (strcmp(argv[i],"-steg")==0) {
				if (strcmp(argv[i+1],"LSB1")==0) {
					ext->tech = LSB1;
				} else if (strcmp(argv[i+1],"LSB4")==0) {
					ext->tech = LSB4;
				} else if (strcmp(argv[i+1],"LSBE")==0) {
					ext->tech = LSBE;
				} else {
					if(ext->outfile!=NULL)
						free(ext->outfile);
					if(ext->wav!=NULL)
						free(ext->wav);
					free(ext);
					return NULL;
				}
			}
		}
	}

	return ext;
	
}


/*********************/
/*        MAIN       */
/*********************/

int main(int argc, char **argv)
{
	int i;

	if (strcmp(argv[1],"-embed")==0) {

		EMBEDSTR* embed = parseEmbedStr(argc,argv);
		lsbEncryptWrapper(embed);


	} else if(strcmp(argv[1],"-extract")==0) {

		EXTRACTSTR* extract = parseExtractStr(argc,argv);
		lsbDecryptWrapper(extract);

	} else {
		printf("Error: invalid input. Parameters -embed or -extract expected.\n");
	}

    return 0;
}



