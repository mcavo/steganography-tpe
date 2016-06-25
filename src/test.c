int lsbExtractWrapper(EXTRACTSTR* ext) {
	int n;
	BYTE removemask;
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
	BYTE bufferExtension[30];
	DWORD len, fileLen;
	unsigned long bytesPerSample = ext->wav->fmt.wBitsPerSample / 8;
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
	printf("Len: %u\n", len);
	BYTE bufferFile[len];
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

	if (ext->cipher == NULL) {

		memcpy(bufferFile, bufferData, len);
		fileLen = len;

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
		if (error) {
			return !OK;
		}
		fileLen = bigEndianBITEArrayToDWORD(bufferFile);
		for (j=0 ; bufferFile[j+fileLen+4]!=0 ; j++) {
			bufferExtension[j] = bufferFile[j+fileLen+4];
		}
		bufferExtension[j] = 0;
	}


	char* filename = malloc(strlen(ext->outfile) + i);
	if( filename == NULL ) {
		return ERROR_OUT_OF_MEMORY;
	}

	memcpy(filename, ext->outfile, strlen(ext->outfile));
	memcpy(filename + strlen(ext->outfile), bufferExtension, i);

	FILE* fptr = fopen(filename,"wb");
	fwrite(bufferFile+4,sizeof(BYTE),fileLen,fptr);
	fclose(fptr);
	return OK;
}