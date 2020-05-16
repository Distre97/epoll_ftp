
#include "ftpClient.h"


//char buffer[MAXSIZE];
int socketfd = NOTCONNETED;

int main()
{
	printf("%d\t", socketfd);
	int addlen;
	struct sockaddr_in addr;
	printf("%d\t", socketfd);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	printf("%d\t", socketfd);
	addr.sin_addr.s_addr = inet_addr(IPADDRESS);
	addr.sin_port = htons(PORT);
	printf("%d\t", socketfd);
	addlen = sizeof(addr);
	printf("%d\t", socketfd);

	if((socketfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("error in get socketfd!\n");
		exit(1);
	}
	printf("%d\t", socketfd);


	if(connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("connect error!\n");
		exit(1);
	}

	printf("%d\t", socketfd);

	while(1)
	{
		
		bzero(&buffer, MAXSIZE);
		printf("ftp> ");
		fgets(buffer, MAXSIZE, stdin);
		cmdCut(buffer);
		printf("%s\m",&buffer);
		if(socketfd < 0)
		{	
			printf("no client connected, bye!\n");
			break;
		}
		if(strncmp(buffer, "ls", 2) == 0)
			listFiles(addr, buffer);
		else if(strncmp(buffer, "get", 3) == 0)
			getFile(addr, buffer);
		else if(strncmp(buffer, "put", 3) == 0)
			putFile(addr, buffer);
		else if(strncmp(buffer, "exit", 4) == 0)
			breakLink();
//		else if(strncmp(buffer, "help", 4) == 0)
//			helpInfo();
		else
			printf("No such command, please try again or try \"help\"!\n");

	}
	
	if(socketfd != NOTCONNETED)
		close(socketfd);
	
	return 0;
}




