obj: main.o ftpServer.o
	gcc -o obj main.o ftpServer.o
main.o: main.c ftpServer.h
	gcc -c main.c
ftpServer.o: ftpServer.h GLOBAL.h
	fcc -c ftpServer.c
clean:
	rm obj main.o ftpServer.o
