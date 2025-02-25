#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 12347
#define MAXSIZE 100

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAXSIZE];
    char str1[MAXSIZE], str2[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }

    // Prompt the user to enter two strings
    printf("Enter first string: ");
    fgets(str1, MAXSIZE, stdin);
    str1[strcspn(str1, "\n")] = '\0';  // Remove newline character

    printf("Enter second string: ");
    fgets(str2, MAXSIZE, stdin);
    str2[strcspn(str2, "\n")] = '\0';  // Remove newline character

    // Send the strings to the server
    send(sockfd, str1, strlen(str1), 0);
    send(sockfd, str2, strlen(str2), 0);

    // Receive the response from the server
    int recv_len = recv(sockfd, buffer, MAXSIZE, 0);
    if (recv_len > 0) {
        buffer[recv_len] = '\0';  // Ensure null termination
        printf("Received from server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
