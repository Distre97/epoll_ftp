#include "ftpServer.h"
#include "GLOBAL.h"
int socketBind(char *ipaddr, int port)
{
	int listenfd,status,newsta;
	struct sockaddr_in serv,clit;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(listenfd != -1);
	
	memset(&serv, 0, sizeof(serv));
	
	//set nonblock
	newsta = fcntl(listenfd, F_GETFL);
	newsta = newsta | O_NONBLOCK;
	fcntl(listenfd, F_SETFL, newsta);
	//

	//set serv's parameters
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = inet_addr(ipaddr);
	serv.sin_port = htons(port);
	//
	
	status = bind(listenfd, (struct sockaddr *)&serv, sizeof(serv));
	assert(status != -1);

	return listenfd;

}

void etModDealEvents(int socketfd, int epfd, int eventNum, char* buffer, struct epoll_event *events)
{
	
	int iter, sfd, oldsta, newsta, flag, length, isFinished;
	for(iter = 0; iter < eventNum; iter++)
	{

		isFinished = 0;
		sfd = events[iter].data.fd;
		printf("sfd is:%d, socketfd is %d\n", sfd, socketfd);
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
			else
				printf("client connecting succuss!\n");
		
			//set nonblock
			oldsta = fcntl(flag, F_GETFL);
			newsta = oldsta | O_NONBLOCK;
			fcntl(flag, F_SETFL, newsta);

			//set et mod
			sipev.events = EPOLLIN | EPOLLET;
			sipev.data.fd = flag;
			epoll_ctl(epfd, EPOLL_CTL_ADD, flag, &sipev);

		}
		else if(events[iter].events & EPOLLIN)
		{
			printf("connected succuss!\n");
			readCommand(epfd, sfd, buffer);
			//deleteBlack(buffer);	
			//cut command
			//printf("read command over!\n");
			//
			
			if(strncmp(buffer, "ls", 2) == 0)
				listFiles(sfd);
			else if(strncmp(buffer, "help", 4) == 0)
				readHelpFile(sfd);
			else if(strncmp(buffer, "get", 3) == 0)
			{	
				isFinished = getFile(sfd, buffer+3);
				if(isFinished == 1)
					printf("succussful uploading!");
			}
			else if(strncmp(buffer, "put", 3) == 0)
			{	
				isFinished = putFile(sfd, buffer+3);
				if(isFinished == 1)
					printf("succussful downloading!");
			}
			else
				printf("No such command! Try \"help\" ! \n");	
		}
		else if(events[iter].events & EPOLLRDHUP)
		{
			close(sfd);
			epoll_ctl(epfd, EPOLL_CTL_DEL, sfd, NULL);
			printf("%d client break link\n",sfd);
		}
		else if(events[iter].events & EPOLLOUT)
		{
			int len;
			while(1)
			{
				len = write(sfd, buffer, strlen(buffer));
				if(len == -1 && errno != EAGAIN)
				{
					printf("write error\n");
					break;
				}
			}
			close(sfd);
			sipev.data.fd = sfd;
			sipev.events = EPOLLIN;
			epoll_ctl(epfd, EPOLL_CTL_MOD, sfd, &sipev);
		}

	}
}

void readCommand(int epfd, int socketfd, char* buffer)
{
	printf("run into readCommand\n");
	if(read(socketfd, buffer, MAXSIZE -1)  <= 0)
	{
		if(errno == EINPROGRESS)
			printf("dealing...\n");
		else if(errno == EINTR)
			printf("interrupt error!\n");
		else if(errno == ECONNRESET)
			printf("link error\n");
		else if(errno == 11)
			printf("there is no data in buffer\n");
		printf("%d",errno);
		close(socketfd);
		epoll_ctl(epfd, EPOLL_CTL_DEL, socketfd, NULL);
		return;
	}
	else
	{
		printf("command is %s\n.", buffer);
		sipev.data.fd = socketfd;
		sipev.events = EPOLLOUT;
		epoll_ctl(epfd, EPOLL_CTL_MOD, socketfd, &sipev);
	}
}

void listFiles(int socketfd)
{
	int len;
	DIR* mydir = NULL;
	struct dirent* myitem = NULL;
	char filenames[MAXSIZE];
	mydir = opendir(".");
	if(mydir == NULL)
	{
		write(socketfd, "open dir error!", MAXSIZE);
	}
	else
	{
		while((myitem = readdir(mydir)) != NULL)
		{
			bzero(filenames, MAXSIZE);
			if(sprintf(filenames, myitem->d_name, MAXSIZE) < 0)
			{
				write(socketfd, "write buffer error!", MAXSIZE);
				break;
			}
			len = strlen(filenames);
			filenames[len] = '\t';
			if(write(socketfd, filenames, MAXSIZE) < 0)
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
		write(socketfd,"get file error!", MAXSIZE);
		return 0;
	}
	
	while((bufsz=read(file, buffer, MAXSIZE)) > 0)
	{
		if(write(socketfd, buffer, bufsz) < 0)
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
		write(socketfd, "put file error!", MAXSIZE);
		return 0;
	}

	while((bufsz=read(socketfd, buffer, MAXSIZE)) > 0)
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











