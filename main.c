#ifndef MAIN_C
#define MAIN_C

#include "ftpServer.h"
#include "GLOBAL.h"

struct epoll_event sipev;
int main()
{
	int socketfd,epfd,eventnum;
	char buffer[MAXSIZE];
	memset(buffer,0,MAXSIZE);
	socketfd = socketBind(IPADDRESS, PORT);
	listen(socketfd, LISQUEUE);
	struct epoll_event events[EPOLLEVENTSIZE];
	epfd = epoll_create(FDMAXSIZE);
	
	//add event
	sipev.events = EPOLLIN;
	sipev.data.fd = socketfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, socketfd, &sipev);
	//
	
	while(1)
	{
		eventnum = epoll_wait(epfd, events, EPOLLEVENTSIZE, -1);
		if(eventnum < 0)
		{
			printf("Epoll error:");
			continue;
		}
		etModDealEvents(socketfd, epfd, eventnum, buffer, events);

	}
	
	close(epfd);

	return 0;
}



#endif
