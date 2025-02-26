#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 100

void swap(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

void permute(char *str, int l, int r, char *resp) {
    if (l == r) {
        printf("%s\n", str);
        strcat(resp,str);
        strcat(resp, "\n"); // Add newline for better readability
        return;
    }
    for (int i = l; i <= r; i++) {
        swap(&str[l], &str[i]);
        permute(str, l + 1, r,resp);
        swap(&str[l], &str[i]);  // Backtrack
    }
}

main()
{
	int sockfd,newsockfd,retval,i;
	socklen_t actuallen;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr,clientaddr;
	char buff[MAXSIZE];
	
	int a=0;
	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(sockfd==-1)
	{
	printf("\nSocket creation error");
	}

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=htons(INADDR_ANY);
	retval=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(retval==1)
	{
		printf("Binding error");
		close(sockfd);
	}

	retval=listen(sockfd,1);
	if(retval==-1)
	{
	close(sockfd);
	}
	actuallen=sizeof(clientaddr);
	newsockfd=accept(sockfd,(struct sockaddr*)&clientaddr,&actuallen);
	if(newsockfd==-1)
	{
		close(sockfd);
	}
	char resp[200];
	while(1){
        memset(buff,'\0',sizeof(buff));
        recv(newsockfd,buff,sizeof(buff),0);
        memset(resp,'\0',sizeof(resp));
        permute(buff,0,strlen(buff)-1,resp);
        send(newsockfd,resp,sizeof(resp),0);
    }
	close(sockfd);
	close(newsockfd);
}

