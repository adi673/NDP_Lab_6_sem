#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#define MAXSIZE 50

main()
{
	char buff[MAXSIZE];
	int sockfd,retval,i;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	printf("\n Intialized Socket \n");
	if(sockfd==-1)
	{
		printf("\nSocket Creation Error \n");
		return;
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	retval=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	printf("\n Connecting to server of IP address 127.0.0.1 \n");
	if(retval==-1)
	{
		printf(" \n Connection error \n");
		return;
	}
	printf("\n Going inside Loop Infinite \n");
	for (i = 0; ; i+=1)
	{
		memset(buff, '\0', sizeof(buff));
		printf("enter the text\n");
		scanf("%s",buff);
		buff[strlen(buff)] = '\0';
		int s = strlen(buff) * sizeof(char);
		sentbytes=send(sockfd,buff,s,0);
		if(sentbytes==-1)
		{
			printf("!!");
			close(sockfd);
		}
		if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
		{
			break;
		}
		memset(buff, '\0', sizeof(buff));
		printf(" \n waiting Here \n");
		recedbytes=recv(sockfd,buff,sizeof(buff),0);
		printf(" \n waiting Done \n");
		printf ("%s \n", buff);
		
		if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p')
		{
			break;
		}
		printf("\n loop completed \n");
	}
	close(sockfd);
}
