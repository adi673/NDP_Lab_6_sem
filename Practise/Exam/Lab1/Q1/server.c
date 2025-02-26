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
	int size;
	recv(newsockfd,&size, sizeof(size));
	int *arr=(int *)malloc(size * sizeof(int));
	recv(newsockfd,arr, size* sizeof(int),0);

	int option;
	while(1){
		memset(buff, '\0', sizeof(buff));
		recv(newsockfd, &option, sizeof(option),0);

		if (option == 4) {
            printf("Client disconnected\n");
            break;
        }

		switch(option){
			case 1:{
				int search;
				recv(newsockfd, &search, sizeof(search),0);
				int val=search(search,arr,size);
				if(val==-1){
					snprintf(buff,sizeof(buff),"not found");
					
				}else{
					snprintf(buff, sizeof(buff), "Found at position %d ",val);
					
				}
				send(newsockfd, buff, sizeof(buff), 0);
				break;
			}
			case 2:{
				sortAsc(arr,size);
				sprintf(buff,"%s",arr)
				for(i=0; i<size; i++){
					char num;
					sprintf(num, "%d", arr[i]);
					strcar(buff,num);
				}
				send(newsockfd,buff,sizeof(buff),0);
				break;
			}
			case 3:{
				split(arr,size,buff);
				send(nenwsockfd,buff,sizeof(buff),0);
				break;
			}
		}
	}
	free(arr);
	close(sockfd);
	close(newsockfd);
}

