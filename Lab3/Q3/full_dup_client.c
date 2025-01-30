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

int main()
{
    char buff[MAXSIZE];
    int sockfd, retval;
    int recedbytes, sentbytes;
    struct sockaddr_in serveraddr;
    char result[MAXSIZE];

    // Take input string from the user
    printf("Enter a string: ");
    fgets(buff, MAXSIZE, stdin);
    
    // Remove newline character if any
    if (buff[strlen(buff) - 1] == '\n') {
        buff[strlen(buff) - 1] = '\0';
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nInitialized Socket \n");
    if (sockfd == -1) {
        printf("\nSocket Creation Error \n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("\nConnecting to server at IP address 127.0.0.1\n");
    if (retval == -1) {
        printf("\nConnection error\n");
        close(sockfd);
        return -1;
    }

    // Send the string to the server
    send(sockfd, buff, strlen(buff), 0);

    // Receive the sorted results from the server (both child and parent processes)
    while (1) {
        memset(result, '\0', sizeof(result));
        recedbytes = recv(sockfd, result, sizeof(result), 0);

        if (recedbytes <= 0) {
            // If no data is received or an error occurs, break the loop
            if (recedbytes == 0) {
                printf("Server closed the connection\n");
            } else {
                printf("Receive error\n");
            }
            break;
        }

        // Print the result from the child process
        printf("Received from server (Child Process): %s\n", result);

        // Clear the result buffer and receive the next part from the parent process
        memset(result, '\0', sizeof(result));
        recedbytes = recv(sockfd, result, sizeof(result), 0);

        if (recedbytes <= 0) {
            // If no data is received or an error occurs, break the loop
            if (recedbytes == 0) {
                printf("Server closed the connection\n");
            } else {
                printf("Receive error\n");
            }
            break;
        }

        // Print the result from the parent process
        printf("Received from server (Parent Process): %s\n", result);

        // Optional: Add an exit condition (like a stop message) if needed
        if (strcmp(result, "stop") == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
