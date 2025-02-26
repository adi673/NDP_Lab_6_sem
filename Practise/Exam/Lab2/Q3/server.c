#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 90

void sortAsc(char str[]){
    int len = strlen(str);
    for(int i=0; i<len-1; i++){
        for(int j=i+1; j<len; j++){
            if(str[i]>str[j]){
                char temp=str[i];
                str[i]=str[j];
                str[j]=temp;
            }
        }
    }
}

void sortDsc(char str[]){
    int len = strlen(str);
    for(int i=0; i<len-1; i++){
        for(int j=i+1; j<len; j++){
            if(str[i]<str[j]){
                char temp=str[i];
                str[i]=str[j];
                str[j]=temp;
            }
        }
    }
}
void process( char buff[], char resp1[], char resp2[]){
    char digits[MAXSIZE];
    char chars[MAXSIZE];
    memset(digits,'\0',sizeof(digits));
    memset(chars,'\0',sizeof(chars));
    int digitIndex=0;
    int charIndex=0;
    for(int i=0; buff[i]!='\0';i++){
        if(isdigit(buff[i])){
            digits[digitIndex++]=buff[i];
        }else if(isalpha(buff[i])){
            chars[charIndex++]=buff[i];
        }
    }
    pid_t pid=fork();
    if(pid==0){
        sortAsc(digits);
        sprintf(resp1,"childProcess [%d] %s",getpid(), digits);
        exit(0);
    }else{
        wait(NULL);
        sortDsc(chars);
        sprintf(resp2,"ParentProcess [%d] %s",getpid(), chars);
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
	char resp1[200];
    char resp2[200];
	while(1){
        memset(buff,'\0',sizeof(buff));
        recv(newsockfd,buff,sizeof(buff),0);
        memset(resp1,'\0',sizeof(resp1));
        memset(resp2,'\0',sizeof(resp2));
        process(buff,resp1,resp2);
        send(newsockfd,resp1,sizeof(resp1),0);
        send(newsockfd,resp2,sizeof(resp2),0);
    }
	close(sockfd);
	close(newsockfd);
}

