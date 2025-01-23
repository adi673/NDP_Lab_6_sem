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
	char buffP[MAXSIZE];
	char buffC[MAXSIZE];
	
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
	
	if(pid == 0) {   // Child Process
        while(1) {
            memset(buffC, '\0', sizeof(buffC));
            recedbytes = recv(newsockfd, buffC, sizeof(buffC), 0);
            if(recedbytes == -1) {
                close(newsockfd);
                exit(1);
            }

            if(buffC[0] == 's' && buffC[1] == 't' && buffC[2] == 'o' && buffC[3] == 'p') {
                break;
            }

            printf("\nReceived from client at Child Process: %s\n", buffC);
        }
        close(newsockfd);
        exit(0);  // Exit the child process after done
    } else {    // Parent process
        // Close the original socket in the parent as it's no longer needed
        close(sockfd);

        while(1) {
            memset(buffP, '\0', sizeof(buffP));
            recedbytes = recv(newsockfd, buffP, sizeof(buffP), 0);
            if(recedbytes == -1) {
                close(newsockfd);
                break;
            }

            if(buffP[0] == 's' && buffP[1] == 't' && buffP[2] == 'o' && buffP[3] == 'p') {
                break;
            }

            printf("\nReceived from client at Parent Process: %s\n", buffP);
        }

        wait(NULL);  // Wait for child process to terminate
        close(newsockfd);
    }
	return 0;
}


