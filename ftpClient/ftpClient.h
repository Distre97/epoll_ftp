#ifndef _FTPCLIENT_H_
#define _FTPCLIENT_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#include "GLOBAL.h"

extern int socketfd;
char buffer[MAXSIZE];


void listFiles(struct sockaddr_in addr, char* buffer);
void getFile(struct sockaddr_in addr, char* command);
void putFile(struct sockaddr_in addr, char* command);
void breakLink();
void cmdCut(char* command);











#endif
