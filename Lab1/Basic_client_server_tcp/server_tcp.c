#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 90

main()
{
	int sockfd,newsockfd,retval,i;
	socklen_t actuallen;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr,clientaddr;
	char buff[MAXSIZE];
	
	int a=0;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	printf("\n Intialized Socket \n");
	if(sockfd==-1){
		printf("\nSocket creation error\n");
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=htons(INADDR_ANY);
	retval=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	printf("\n Binding complete \n");
	if(retval==1){
		printf("\nBinding error\n");
		close(sockfd);
	}

	retval=listen(sockfd,1);
	printf("\n Listening completed \n");
	if(retval==-1){
		close(sockfd);
	}
	actuallen=sizeof(clientaddr);
	newsockfd=accept(sockfd,(struct sockaddr*)&clientaddr,&actuallen);
	printf("\n Accpeted Client Connection \n");
	if(newsockfd==-1){
		close(sockfd);
	}
	printf("\n Going inside Loop Infinite \n");
	for (i = 0; ; i+=1){
		
		memset(buff, '\0', sizeof(buff));
		printf(" \n waiting Here \n");
		recedbytes=recv(newsockfd,buff,sizeof(buff),0);
		printf(" \n waiting Done \n");
		if(recedbytes==-1){
			close(sockfd);
			close(newsockfd);
		}
		
		if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p'){
			break;
		}
		printf("%s \n",buff);
		memset(buff, '\0', sizeof(buff));
		scanf("%s",buff);
		buff[strlen(buff)] = '\0';
		int s = strlen(buff) * sizeof(char);
		sentbytes=send(newsockfd,buff,s,0);
		if(sentbytes==-1){
			close(sockfd);
			close(newsockfd);
		}
		
		if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p'){
			break;
		}
		printf("\n loop completed \n" );
	}
	close(sockfd);
	close(newsockfd);
}

