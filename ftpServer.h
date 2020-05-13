#ifndef _FTPSERVER_H
#define _FTPSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in,h>
#include <signal.h>

#include 'GLOBAL.h'

//for saving commands
struct cmdqueue{
	char* cmd[MAXSIZE];
	struct cmdqueue* next;
}cmdQ;

//function for kenal
int socketBind(char *ipaddr, int port);
void etModDealEvents(int socketfd, int epfd, int eventNum, char* buffer, struct epoll_event *events, struct epoll_event sipev);
//

//function for transport
void readCommand(int epfd, int socketfd, char* buffe);
void listFiles(int socketfd);
void readHelpFile(int socketfd);
int getFile(int socketfd, char* filename);
int putFile(int socketfd, char* filename);
void cmdHistory(int socketfd, char)
//
//extra things









#endif
