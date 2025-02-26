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
    char output[1000];
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
    memset(buff,'\0',sizeof(buff));
    memset(output,'\0',sizeof(output));
    while(1){
        printf("\n Enter String\n");
        fgets(buff,MAXSIZE,stdin);
        send(sockfd,buff,sizeof(buff),0);

        memset(buff,'\0',sizeof(buff));

        recv(sockfd,output,sizeof(output),0);
        printf("\n Result is : %s \n",output);
        memset(output,'\0',sizeof(output));

        
    }

	close(sockfd);
}
