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
	int length,file,namelen;
	
	if(write(socketfd, command, strlen(command)) < 0)
	{
		printf("get command write error!\n");
		return;
	}
	
	printf("send command\"%s\" success\n", command);
	
	namelen = strlen(command)-3;
	char* filen[namelen];
	strcpy(filen,command+3);
	printf("filename is %s\n", filen);
	if((file = open(filen, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0)
	{
		printf("open file error!\n");
		return;
	}
	printf("open file %s success\n", filen);
	while((length = read(socketfd, buf, MAXSIZE)) > 0)
	{
		//printf("writing...\n");
		if(write(file, buf, length) < 0)
			printf("write file error!\n");
	}
	close(file);

	return;
}

void putFile(struct sockaddr_in addr, char* command)
{
	char buf[MAXSIZE];
	int length,file,namelen,flag;
	printf("the file is %s\n", command);
	if(write(socketfd, command, strlen(command)) < 0)
	{
		printf("put command write error!\n");
		return;
	}
	
	namelen = strlen(command)-3;
	char* filen[namelen];
	strcpy(filen,command+3);

	if((file = open(filen, O_RDONLY)) < 0)
	{
		printf("open file error!\n");
		return;
	}
	if((flag = read(socketfd, command, MAXSIZE)) > 0)
	{
		while((length = read(file, buf, MAXSIZE)) > 0 && (strcmp(command,"yes") == 0) )
		{
			if(write(socketfd, buf, length) < 0)
				printf("write file error!\n");
		}
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
	command[j-1] = '\0';
}




#endif
