#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define MAXSIZE 90

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nInitialized Socket\n");
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(1);
    }

    // Configure server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("\nBinding complete\n");
    if (retval == -1) {
        perror("Binding error");
        close(sockfd);
        exit(1);
    }

    // Listen for client
    retval = listen(sockfd, 1);
    printf("\nListening...\n");
    if (retval == -1) {
        perror("Listening error");
        close(sockfd);
        exit(1);
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    printf("\nAccepted Client Connection\n");
    if (newsockfd == -1) {
        perror("Client accept error");
        close(sockfd);
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {  // Child Process (Receiving from Client)
        while (1) {
            memset(buff, 0, sizeof(buff));
            int recedbytes = recv(newsockfd, buff, sizeof(buff) - 1, 0);
            if (recedbytes <= 0) {
                perror("Receive error or client disconnected");
                break;
            }

            printf("\nReceived from client: %s\n", buff);

            // Exit if client sends "stop"
            if (strncmp(buff, "stop", 4) == 0) {
                break;
            }
        }
    } else {  // Parent Process (Sending to Client)
        while (1) {
            memset(buff, 0, sizeof(buff));
            printf("\nEnter message to send: ");
            fflush(stdout); // Ensures prompt is displayed correctly

            fgets(buff, sizeof(buff), stdin);
            buff[strcspn(buff, "\n")] = '\0'; // Remove newline character

            int sentbytes = send(newsockfd, buff, strlen(buff), 0);
            if (sentbytes == -1) {
                perror("Send error");
                break;
            }

            // Exit if "stop" is sent
            if (strncmp(buff, "stop", 4) == 0) {
                break;
            }
        }
        wait(NULL); // Wait for child process to finish
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
