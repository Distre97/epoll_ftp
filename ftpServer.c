#include "ftpServer.h"
#include "GLOBAL.h"
int socketBind(char *ipaddr, int port)
{
	int socketfd,status;
	struct sockaddr_in serv,clit;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(socketfd != -1);
	
	memset(&serv, 0, sizeof(serv));

	//set serv's parameters
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(ipaddr);
	serv.sin_port = htons(port);
	//
	
	status = bind(socketfd, (struct sockaddr *)&serv, sizeof(serv));
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
			newsta = oldsta | O_NONBLOCK;
			fcntl(flag, F_SETFL, newsta);
			//

			//set et mod
			sipev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
			sipev.data.fd = flag;
			epoll_ctl(epfd, EPOLL_CTL_ADD, flag, &sipev);

		}
		else if(events[iter].events & EPOLLIN)
		{
			readCommand(epfd, socketfd, buffer);
			deleteBlack(buffer);	
			//cut command
			
			//
			
			if(strncmp(buffer, "ls", 2) == 0)
				listFiles(socketfd);
			else if(strncmp(buffer, "help", 4) == 0)
				readHelpFile(socketfd);
			else if(strncmp(buffer, "get", 3) == 0)
			{	
				isFinished = getFile(socketfd, buffer+3);
				if(isFinished == 1)
					send(socketfd, "succussful downloading!", MAXSIZE, 0);
			}
			else if(strncmp(buffer, "put", 3) == 0)
			{	
				isFinished = putFile(socketfd, buffer+3);
				if(isFinished == 1)
					send(socketfd, "succussful uploading!", MAXSIZE, 0);
			}
			else
				printf("No such command! Try \"help\" ! \n");	
		}
		else if(events[iter].events & EPOLLRDHUP)
		{
			close(socketfd);
			epoll_ctl(epfd, EPOLL_CTL_DEL, socketfd, NULL);
			printf("%d client break link\n",socketfd);
		}


	}
}

void readCommand(int epfd, int socketfd, char* buffer)
{
	while(1)
	{
		int num = recv(socketfd, buffer, MAXSIZE, 0);
		if(num <= 0)
		{
			if((errno == EAGAIN) || (errno == EWOULDBLOCK))
				break;
			else
				printf("read error!\n");
			close(socketfd);
			epoll_ctl(epfd, EPOLL_CTL_DEL, socketfd, NULL);
		}
	}
	send(socketfd, "recived!", 8, 0);
}

void listFiles(int socketfd)
{
	DIR* mydir = NULL;
	struct dirent* myitem = NULL;
	char filenames[MAXSIZE];
	bzero(filenames, MAXSIZE);
	mydir = opendir(".");
	if(mydir == NULL)
	{
		send(socketfd, "open dir error!", MAXSIZE, 0);
	}
	else
	{
		while((myitem = readdir(mydir)) != NULL)
		{
			if(sprintf(filenames, myitem->d_name, MAXSIZE) < 0)
			{
				send(socketfd, "write buffer error!", MAXSIZE, 0);
				break;
			}
			if(send(socketfd, filenames, MAXSIZE, 0) < 0)
			{
				printf("send error!\n");
				break;
			}
		}	
	}
	closedir(mydir);
	close(socketfd);
}

void readHelpFile(int socketfd)
{
	printf("help!\n");
}

int getFile(int socketfd, char* filename)
{
	int file, bufsz;
	char buffer[MAXSIZE];
	bzero(buffer,MAXSIZE);
	file = open(filename, O_RDONLY);
	if(file < 0)
	{
		send(socketfd,"get file error!", MAXSIZE, 0);
		return 0;
	}
	
	while((bufsz=read(file, buffer, MAXSIZE)) > 0)
	{
		if(send(socketfd, buffer, bufsz, 0) < 0)
		{
			printf("send file %s error\n", filename);
			close(file);
			return 0;
		}
	}
	close(file);
	close(socketfd);
	return 1;

}

int putFile(int socketfd, char* filename)
{
	int file, bufsz;
	char buffer[MAXSIZE];
	bzero(buffer, MAXSIZE);

	file = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0644);
	if(file < 0)
	{
		send(socketfd, "put file error!", MAXSIZE, 0);
		return 0;
	}

	while((bufsz=recv(socketfd, buffer, MAXSIZE, 0)) > 0)
	{
		if(write(file, buffer, bufsz) < 0)
		{
			printf("recive file %s error\n", filename);
			close(file);
			return 0;
		}
	}

	close(file);
	close(socketfd);
	return 1;
}





void deleteBlack(char* buffer)
{
	int iter,space;
	space = 0;
	for(iter = 0; buffer[iter] != '\0'; iter++)
	{
		if(buffer[iter] != ' ')
		{
			buffer[space] = buffer[iter];
			space++;
		}
	}
	buffer[space] = '\0';
}











