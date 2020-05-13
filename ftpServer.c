#include 'ftpServer.h'

int socketBind(char *ipaddr, int port)
{
	int socketfd,status;
	struct sockaddr_in serv,clit;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(socketfd != -1);
	
	memset(&serv, 0 sizeof(serv));

	//set serv's parameters
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(ipaddr);
	serv.sin_port = htons(port);
	//
	
	status = bind(socketfd, (struct sockaddr *)&serv. sizeof(serv));
	assert(status != -1);

	return socketfd;

}

void etModDealEvents(int socketfd, int epfd, int eventNum, char* buffer, struct epoll_event *events, struct epoll_event sipev)
{
	int iter, sfd, oldsta, newsta, flag, length, isFinished;
	for(; iter < eventNum; iter++)
	{
		isFinished = 0;
		sfd = events[iter].data.fd;
		if(sfd == socketfd)
		{
			struct sockaddr_in clit;
			length = sizeof(clit);
			flag = accept(sfd, (struct sockaddr*)&clit, &length);
			if(flag < 0)
			{
				printf("client connecting error\n");
				continue;
			}

			//set nonblock
			oldsta = fcntl(flag, F_GETFL);
			newsta = oldsta | O_NONBOLCK;
			fcntl(flag, F_SETFL, newsta);
			//

			//set et mod
			sipev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
			sipev.data.fd = flag;
			epoll_ctl(epollfd, EPOLL_CTL_ADD, flag, &sipev);

		}
		else if(events[iter].events & EPOLLIN)
		{
			readCommand(epfd, socketfd, buffer);
			
			//cut command
			
			//
			
			if(strncmp(buffer, "ls", 2) == 0)
				listFiles(socketfd);
			else if(strncmp(buffer, "help", 4) == 0)
				readHelpFile(socketfd);
			else if(strncmp(buffer, "get", 3) == 0)
			{	
				isFinished = getFile(socketfd, buffer+4);
				if(isFinished == 1)
					send(socketfd, "succussful downloading!", MAXSIZE, 0);
			}
			else if(strncmp(buffer, "put", 3) == 0)
			{	
				isFinished = putFile(socketfd, buffer+4);
				if(isFinished == 1)
					send(socketfd, "succussful uploading!", MAXSIZE, 0);
			}
			else
				printf("No such command! Try \"help\" ! \n");	
		}
		
		


	}
}














