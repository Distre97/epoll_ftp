cli: main.o ftpClient.o
	gcc -o cli main.o ftpClient.o
main.o: main.c ftpClient.h
	gcc -c main.c
ftpClient.o: ftpClient.c ftpClient.h GLOBAL.h
	gcc -c ftpClient.c
clean:
	-rm main.o ftpClient.o
