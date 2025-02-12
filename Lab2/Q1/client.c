#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3388
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char filename[MAXSIZE], buffer[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    while (1) {
        printf("Enter filename (or 'exit' to quit): ");
        scanf("%s", filename);

        send(sockfd, filename, strlen(filename), 0);
        if (strcmp(filename, "exit") == 0) break;

        memset(buffer, 0, MAXSIZE);
        recv(sockfd, buffer, MAXSIZE, 0);
        printf("Server Response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
