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
	serveraddr.sin_port=htons(3387);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	retval=connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(retval==-1)
	{
		printf("Connection error");
		return;
	}
    int row,col;
	while(1){
        printf("\n ENter Row and columns \n");
        scanf("%d",&row);
        scanf("%d",&col);

        send(sockfd, &row, sizeof(int), 0);
        send(sockfd, &col, sizeof(int), 0);

        int** matrix = (int **)malloc(row * sizeof(int *)); 
        for (int i = 0; i < row; i++) {
            matrix[i] = (int *)malloc(col * sizeof(int)); 
        }
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
               scanf("%d", &matrix[i][j]);
            }
            
        }
        printf("\n sending matrix\n");
        send(sockfd,matrix[0], col*row*sizeof(int),0);
        printf("\nsent matrix\n");
        
        int** matrix2 = (int **)malloc(row * sizeof(int *)); 
        for (int i = 0; i < row; i++) {
            matrix2[i] = (int *)malloc(col * sizeof(int)); 
        }
        recv(sockfd,matrix2[0],col*row*sizeof(int),0);
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                printf("%d ", matrix2[i][j]);
            }
            printf("\n");
        }

        
    }
	close(sockfd);
}
