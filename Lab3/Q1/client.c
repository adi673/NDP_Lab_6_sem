#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define MAXSIZE 90

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
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
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("\nConnecting to server...\n");
    if (retval == -1) {
        perror("Connection error");
        close(sockfd);
        exit(1);
    }

    pid_t pid = fork();

    if (pid == 0) {  // Child Process (Receiving from Server)
        while (1) {
            memset(buff, 0, sizeof(buff));
            int recedbytes = recv(sockfd, buff, sizeof(buff) - 1, 0);
            if (recedbytes <= 0) {
                perror("Receive error or server disconnected");
                break;
            }

            printf("\nReceived from server: %s\n", buff);

            // Exit if server sends "stop"
            if (strncmp(buff, "stop", 4) == 0) {
                break;
            }
        }
    } else {  // Parent Process (Sending to Server)
        while (1) {
            memset(buff, 0, sizeof(buff));
            printf("\nEnter message to send: ");
            fflush(stdout); // Ensures prompt is displayed correctly

            fgets(buff, sizeof(buff), stdin);
            buff[strcspn(buff, "\n")] = '\0'; // Remove newline character

            int sentbytes = send(sockfd, buff, strlen(buff), 0);
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

    close(sockfd);
    return 0;
}
