#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#define MAXSIZE 90

int palindrome(char buff[])
{
    int len = strlen(buff);
    int i = 0;
    while (i < (len-1) / 2)
    {
	printf(" %c %c \n",buff[i],buff[len-i-2]);
        if (buff[i] != buff[len - i - 2])
        {
            return 0;
        }
        i++;
    }
    return 1;
}

void count(char buff[], char output[])
{
    int *arr = (int *)malloc(5 * sizeof(int));
    int len = strlen(buff);
    for (int i = 0; i < len-1; i++)
    {

        if (buff[i] == 'A' || buff[i] == 'a')
        {
            arr[0]++;
        }
        else if (buff[i] == 'E' || buff[i] == 'e')
        {
            arr[1]++;
        }
        else if (buff[i] == 'I' || buff[i] == 'i')
        {
            arr[2]++;
        }
        else if (buff[i] == 'O' || buff[i] == 'o')
        {
            arr[3]++;
        }
        else if (buff[i] == 'U' || buff[i] == 'u')
        {
            arr[4]++;
        }
    }
    strcat(output, "Ocurrence is as follows : \n");

    int num[100];
    snprintf(num,sizeof(num), "A:%d ,E:%d, I:%d, O:%d, U:%d \n",arr[0],arr[1],arr[2],arr[3],arr[4]);
    strcat(output, num);
}
void process(char buff[], char output[])
{
    int pali = palindrome(buff);
    if (pali)
    {
        strcpy(output, "is Palindrome\n");
    }
    else
    {
        strcpy(output, "is not Palindrome\n");
    }
   
    int num[30];
    sprintf(num, " Size of String is %d \n", strlen(buff));
    strcat(output, num);
    count(buff, output);
}

main()
{
    int sockfd, newsockfd, retval, i;
    socklen_t actuallen;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    char output[1000];

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
    memset(buff, '\0', sizeof(buff));
    while (1)
    {
        recv(newsockfd, buff, sizeof(buff), 0);
        memset(output, '\0', sizeof(output));
        process(buff, output);
        send(newsockfd,output,sizeof(output),0);
    }
    close(sockfd);
    close(newsockfd);
}
