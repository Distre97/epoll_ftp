#ifndef _FTPCLIENT_C_
#define _FTPCLIENT_C_


#include "ftpClient.h"

void listFiles(struct sockaddr_in addr, char* buffer)
{
	char buf[MAXSIZE];
	printf("buffer is %s",buffer);
	if(write(socketfd, buffer, strlen(buffer)) < 0)
	{
		printf("write command error!\n");
		return;
	}
	printf("begin read file list\n");
	while(read(socketfd, buf, MAXSIZE) > 0)
	{
		printf("%s", buf);
	}
	
	printf("...\n");
	close(socketfd);
	return;
}

void breakLink()
{
	if(socketfd >= 0)
		printf("Client %d is out of connected!\n",socketfd);
	close(socketfd);
	socketfd = NOTCONNETED;
}

void getFile(struct sockaddr_in addr, char* command)
{
	char buf[MAXSIZE];
	int length,file;

	if(write(socketfd, command, strlen(command)) < 0)
	{
		printf("get command write error!\n");
		return;
	}

	if(read(socketfd, buf, MAXSIZE) < 0)
	{
		printf("file download error!\n");
		return;
	}

	if((file = open(command+3, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
	{
		printf("open file error!\n");
		return;
	}

	while((length = read(socketfd, buf, MAXSIZE)) > 0)
	{
		if(write(file, buf, length) < 0)
			printf("write file error!\n");
	}
	close(file);

	return;
}

void putFile(struct sockaddr_in addr, char* command)
{
	char buf[MAXSIZE];
	int length,file;

	if(write(socketfd, command, strlen(command)) < 0)
	{
		printf("put command write error!\n");
		return;
	}

	if((file = open(command+3, O_RDONLY)) < 0)
	{
		printf("open file error!\n");
		return;
	}

	while((length = read(file, buf, MAXSIZE)) > 0)
	{
		if(write(socketfd, buf, length) < 0)
			printf("write file error!\n");
	}
	close(file);


	return;
}

void cmdCut(char* command)
{
	int i,j=0;
	for(i=0; command[i] != '\0'; i++)
	{
		if(command[i] != ' ')
		{
			command[j] = command[i];
			j++;
		}
	}
	command[j] = '\0';
}




#endif
