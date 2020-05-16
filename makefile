serv: main.o ftpServer.o
	gcc -o serv main.o ftpServer.o
main.o: main.c ftpServer.h GLOBAL.h
	gcc -c main.c
ftpServer.o: ftpServer.h GLOBAL.h
	gcc -c ftpServer.c
clean:
	-rm main.o ftpServer.o
