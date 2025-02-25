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

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        // Get domain name from user
        printf("Enter domain name (or type 'exit' to quit): ");
        fgets(domain, MAXSIZE, stdin);
        domain[strcspn(domain, "\n")] = '\0';  // Remove newline character

        // Exit if the user types "exit"
        if (strcmp(domain, "exit") == 0) {
            break;
        }

        // Send domain name to server
        if (send(sockfd, domain, strlen(domain) + 1, 0) == -1) {
            perror("Send failed");
            break;
        }

        // Receive IP address or error message from server
        memset(buffer, 0, MAXSIZE);
        int bytes_received = recv(sockfd, buffer, MAXSIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Connection closed by server.\n");
            break;
        }
        buffer[bytes_received] = '\0';  // Ensure null termination
        printf("Received IP address: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
