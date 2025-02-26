#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 90
int seatchArr(int key, int arr[], int size){
	for (int i = 0; i < size; i++) {
        if (arr[i] == key)
            return i + 1;  // Returning 1-based index
    }
    return -1;
}

int sortAsc(int arr[], int size){
	for(int i=0; i<size-1; i++){
		for(int j=0; j<size-i-1; j++){
			if(arr[j]>arr[j+1]){
				int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
			}
		}
	}
}

void sortDesc(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
} 

void split(int arr[], int size, char *buff){
	char odd[100]="Odd: ";
	char event[100]="event: ";

	for(int i=0; i<size; i++){
		char num[100];
		sprintf(num, "%d", arr[i]);
		if(arr[i]%2==0){
			strcat(even,num);
		}else{
			strcat(odd,num);
		}
	}
	snprintf(buff, MAXSIZE, "%s\n%s",even,odd);
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
	int size;
	recv(newsockfd,&size, sizeof(size),0);
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
				int val=searchArr(search,arr,size);
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
				sprintf(buff,"%s",arr);
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
				send(newsockfd,buff,sizeof(buff),0);
				break;
			}
		}
	}
	free(arr);
	close(sockfd);
	close(newsockfd);
}

