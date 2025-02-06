#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // Include for inet_ntoa()

#define MAXSIZE 90

int main() {
    char buff[MAXSIZE];
    int sockfd, newsockfd, retval;
    int recedbytes, sentbytes;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nInitialized Socket\n");
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12346); // Server listens on port 12346
    serveraddr.sin_addr.s_addr = INADDR_ANY; // Listen on any network interface

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("\nBinding complete\n");

    if (retval == -1) {
        perror("\nBinding failed\n");
        close(sockfd);  // Properly close the socket in case of an error
        return -1; // Exit with an error
    }

    retval = listen(sockfd, 5);  // Listen for a maximum of 5 connections
    if (retval == -1) {
        printf("\nListen error\n");
        close(sockfd);  // Close socket on error
        return -1;
    }

    printf("\nListening for incoming connections...\n");
    actuallen = sizeof(clientaddr);
    
    int client_no = 0;

    while(1) {
        printf("\n ----------listening new client %d -------------------\n", ++client_no);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
        if (newsockfd == -1) {
            printf("\nAccept error\n");
            close(sockfd);  // Close socket on error
            return -1;
        }

        // Extract and print the client’s IP address and port number
        char *client_ip = inet_ntoa(clientaddr.sin_addr);  // Convert to string
        unsigned short client_port = ntohs(clientaddr.sin_port); // Convert to host byte order
        
        printf("\nClient connected: IP = %s, Port = %d\n", client_ip, client_port);

        pid_t pid = fork();  // Create a new process
        if (pid == 0) {   // Child process
            close(retval);
            while (1) {
                printf("\n--------------Child process PID: %d-------------\n", getpid());
                memset(buff, '\0', sizeof(buff));

                // Receive data from the client
                recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
                if (recedbytes == -1) {
                    close(sockfd);
                    close(newsockfd);
                    return -1;
                }

                if (recedbytes == 0) {
                    printf("\nClient disconnected. Exiting child process...\n");
                    break;
                } 

                int n = strlen(buff) * sizeof(char);
                printf("\nReceived from client: %s\n", buff);
                send(newsockfd, buff, n, 0);
            }
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
