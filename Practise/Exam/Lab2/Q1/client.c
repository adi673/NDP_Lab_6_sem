#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 50

main()
{
    char buff[MAXSIZE];
    char get[MAXSIZE];
    int sockfd, retval, i;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("\nSocket Creation Error");
        return;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    retval = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (retval == -1)
    {
        printf("Connection error");
        return;
    }
    pid_t pid = fork();

    if (pid == 0)
    {
        while (1)
        {
            memset(buff, '\0', sizeof(buff));
            printf("\n Enter Message to send : ");
            fgets(buff, MAXSIZE, stdin);
	        buff[strcspn(buff, "\n")] = '\0';
            send(sockfd, buff, sizeof(buff), 0);
        }
    }
    else if (pid > 0)
    {
        while (1)
        {
            memset(get, '\0', sizeof(get)); // Clear buffer
            recv(sockfd, get, sizeof(get) - 1, 0);
            printf("\n Message from server is : %s", get);
            
        }
    }
    close(sockfd);
}
