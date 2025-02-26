#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buffer[BUFFER_SIZE];

    // Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set Server Address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Connect to Server
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to DNS Server.\n");

    while (1) {
        printf("\nEnter domain name to resolve (or 'exit' to quit): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

        // Send Domain Name to Server
        send(sockfd, buffer, strlen(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Receive Response
        memset(buffer, 0, BUFFER_SIZE);
        recv(sockfd, buffer, BUFFER_SIZE, 0);
        printf("Resolved IP: %s\n", buffer);
    }

    // Close Connection
    close(sockfd);
    return 0;
}
