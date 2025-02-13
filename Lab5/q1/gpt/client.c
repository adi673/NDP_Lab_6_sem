#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 100

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char buffer[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket Creation Error");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12346);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection error");
        close(sockfd);
        return -1;
    }

    printf("Enter string (Institute Of / Technology): ");
    fgets(buffer, MAXSIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    send(sockfd, buffer, strlen(buffer), 0);

    memset(buffer, '\0', sizeof(buffer));
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Server response: %s\n", buffer);

    close(sockfd);
    return 0;
}
