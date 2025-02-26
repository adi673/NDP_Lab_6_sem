#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#define MAXSIZE 100

main()
{
	char buff[MAXSIZE];
	int sockfd,retval,i;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
	{
		printf("\nSocket Creation Error");
		return;
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	retval=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(retval==-1)
	{
		printf("Connection error");
		return;
	}
    char resp[200];
	while(1){
        memset(buff,'\0',sizeof(buff));
        fgets(buff,sizeof(buff),stdin);
        buff[strcspn(buff, "\n")] = '\0'; // Remove newline
        send(sockfd,buff,sizeof(buff),0);
        memset(resp,'\0',sizeof(resp));
        recv(sockfd,resp,sizeof(resp),0);
	printf("%s",resp);
    }
	close(sockfd);
}
