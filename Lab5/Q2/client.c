#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 100
#define PORT 12347

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char str1[MAXSIZE], str2[MAXSIZE], result[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }

    // Get user input
    printf("Enter first string: ");
    fgets(str1, MAXSIZE, stdin);
    str1[strcspn(str1, "\n")] = '\0';  // Remove newline

    printf("Enter second string: ");
    fgets(str2, MAXSIZE, stdin);
    str2[strcspn(str2, "\n")] = '\0';  // Remove newline

    // Send strings to server
    send(sockfd, str1, strlen(str1) + 1, 0);
    send(sockfd, str2, strlen(str2) + 1, 0);

    // Receive result from server
    recv(sockfd, result, sizeof(result), 0);
    printf("Server Response: %s\n", result);

    close(sockfd);
    return 0;
}
