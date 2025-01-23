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
	pid_t pid = fork();  // Create a new process
	
	if(pid==0){   //child Process
		while(1){
			memset(buff, '\0', sizeof(buff));
			recedbytes=recv(newsockfd,buff,sizeof(buff),0);
			if(recedbytes==-1){
				close(sockfd);
				close(newsockfd);
			}
		
			if (buff[0] == 's' && buff[1] == 't' && buff[2] == 'o' && buff[3] == 'p'){
				break;
			}
			printf("\nReceived from client: %s\n", buff);
		
		}
	
	}else{	//Parent process
		while(1){
			printf("I'm the parent process with PID %d and PPID %d.\n", getpid(), getppid());
			printf("My child's PID is %d.\n", pid);

			memset(buff, '\0', sizeof(buff));
			printf("\nEnter message to send: ");
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
		}
		wait(NULL);
	
	
	}
	close(sockfd);
	close(newsockfd);
}


