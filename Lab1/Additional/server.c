#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXSIZE 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cli_len;
    char buffer[MAXSIZE];

    // Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("TCP Server listening on port %d...\n", PORT);

    while (1) {
        cli_len = sizeof(cliaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&cliaddr, &cli_len);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("\nConnected to Client!\n");
        printf("Client IP: %s\n", inet_ntoa(cliaddr.sin_addr));
        printf("Client Port: %d\n", ntohs(cliaddr.sin_port));

        memset(buffer, 0, MAXSIZE);
        read(newsockfd, buffer, sizeof(buffer));
        printf("Received: %s\n", buffer);

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
