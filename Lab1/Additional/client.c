#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[MAXSIZE];

    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Enter message: ");
    fgets(buffer, MAXSIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline

    // Send message
    write(sockfd, buffer, strlen(buffer));

    close(sockfd);
    return 0;
}
