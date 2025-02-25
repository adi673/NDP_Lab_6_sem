#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 100

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char domain[MAXSIZE], buffer[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address setup
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        // Get domain name input
        printf("Enter domain name (or type 'exit' to quit): ");
        fgets(domain, MAXSIZE, stdin);
        domain[strcspn(domain, "\n")] = '\0';  // Remove newline character

        if (strcmp(domain, "exit") == 0) {
            break;
        }

        // Send domain to server
        if (send(sockfd, domain, strlen(domain) + 1, 0) == -1) {
            perror("Send failed");
            break;
        }

        // Receive response
        memset(buffer, 0, MAXSIZE);
        int bytes_received = recv(sockfd, buffer, MAXSIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Connection closed by server.\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("Received IP address: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
