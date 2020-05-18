
#include "ftpClient.h"


//char buffer[MAXSIZE];
int socketfd = NOTCONNETED;

int main()
{
	int addlen;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(IPADDRESS);
	addr.sin_port = htons(PORT);
	addlen = sizeof(addr);

	if((socketfd=socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("error in get socketfd!\n");
		exit(1);
	}

	if(connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		printf("connect error!\n");
		exit(1);
	}

	while(1)
	{
		if(socketfd < 0)
		{	
			printf("no client connected, bye!\n");
			break;
		}

		bzero(&buffer, MAXSIZE);
		printf("ftp> ");
		fgets(buffer, MAXSIZE, stdin);
		cmdCut(buffer);
		
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




