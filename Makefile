all:
	gcc -o stegowav src/stegowav.c src/lsb.c src/bytesmanager.c src/wavmanager.c src/ciphermanager.c -lssl -lcrypto
clean:
	rm -f stegowav
	rm -f data/test/*
