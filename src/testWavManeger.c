#include "wavmanager.h"
#include <stdio.h>

int main() {
	FILE* ptr = fopen("../data/wavfile.wav", "rb");
	if (ptr == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
	WAVSTR* wav = malloc(sizeof(WAVSTR));
	getWavStr(ptr,wav);
	fclose(ptr);
	FILE* ptr2 = fopen("./wavfile2.wav", "wb");

	writeWavFile(ptr2,wav);
	fclose(ptr2);

	free(wav->data.soundData);
	free(wav);
}