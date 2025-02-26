#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 90

void sortAsc(char str[])
{
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (str[i] > str[j])
            {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

void sortDsc(char str[])
{
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = i + 1; j < len; j++)
        {
            if (str[i] < str[j])
            {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}
void process(char buff[], char resp1[], char resp2[])
{
}
main()
{
    int sockfd, newsockfd, retval, i;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    int a = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("\nSocket creation error");
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);
    retval = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == 1)
    {
        printf("Binding error");
        close(sockfd);
    }

    retval = listen(sockfd, 1);
    if (retval == -1)
    {
        close(sockfd);
    }
    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr *)&clientaddr, &actuallen);
    if (newsockfd == -1)
    {
        close(sockfd);
    }
    int row,col;
    while (1)
    {
        

        recv(newsockfd, &row, sizeof(int), 0);
        recv(newsockfd, &col, sizeof(int), 0);

        printf("Received cols and rows \n");

        int** matrix= (int **)malloc(row*sizeof(int*));
        for(int i=0 ;i<row; i++){
            matrix[i]=(int*)malloc(col*sizeof(int));
        }
        printf("Receiving matrixs \n");
        recv(newsockfd, matrix[0], row * col * sizeof(int), 0);
        printf("Received matrixs \n");
        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                matrix[i][j]=matrix[i][j]*2;
            }
        }
        printf("sending abck matrixs \n");
        send(newsockfd,matrix[0],row*col*sizeof(int),0);
    }
    close(sockfd);
    close(newsockfd);
}
