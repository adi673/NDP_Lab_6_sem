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
	// memset(arr, 0, MAXSIZE*sizeof(int));
	
	int size;
	printf("\n Enter sizeof array\n");
	scanf("%d",&size);
	send(sockfd,&size,sizeof(int),0);
	
	char buff[100];

	int *arr=(int *)malloc(size * sizeof(int));
	printf("\n ENter array memebres\n");
	for(int i=0; i<size; i++){
		scanf("%d",&arr[i]);
	}
	send(sockfd, arr, size* sizeof(int),0);

	int option;
	while(1){
		
		memset(buff,'\0',sizeof(buff));
		printf("\nEnter Option:\n1. search\n2. Sort \n3. Split\nEnter choice: ");
        scanf("%d", &option);
		send(sockfd, &option, sizeof(option),0);

		switch(option){
			case 1:{
				int search;
				printf("\n Enter Reg no \n");
				scanf("%d",&search);
				send(sockfd,&search, sizeof(search),0);
				recv(sockfd,buff,sizeof(buff),0);
				printf("%s\n",buff);
				break;
			}
			case 2:{
				recv(sockfd,buff,sizeof(buff),0);
				printf("%s\n",buff);
				break;
			}
			case 3:{
				recv(sockfd,buff,sizeof(buff),0);
				printf("%s\n",buff);
				break;
			}
			default :{
				printf("\nInvalid choice, try again\n");
				break;
			}
		}
	}
	free(arr);
	close(sockfd);
	return 0;
}
