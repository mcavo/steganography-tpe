all:
	gcc -o stegowav src/stegowav.c src/lsb.c src/bytesmanager.c src/wavmanager.c src/ciphermanager.c -lssl -lcrypto
	gcc -o steganalysisFileLength src/steganalysisFileLength.c src/lsb.c src/bytesmanager.c src/wavmanager.c src/ciphermanager.c -lssl -lcrypto
	gcc -o steganalysisFileContent src/steganalysisFileContent.c src/lsb.c src/bytesmanager.c src/wavmanager.c src/ciphermanager.c -lssl -lcrypto
	gcc -o steganalysisEOF src/steganalysisEOF.c src/lsb.c src/bytesmanager.c src/wavmanager.c src/ciphermanager.c -lssl -lcrypto
clean:
	rm -f stegowav
	rm -f steganalysisFileLength
