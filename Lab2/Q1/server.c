#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3388
#define MAXSIZE 1024

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen;
    char filename[MAXSIZE], buffer[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    listen(sockfd, 5);
    printf("Server listening on port %d...\n", PORT);

    addrlen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen);
    if (newsockfd == -1) {
        perror("Accept failed");
        close(sockfd);
        return 1;
    }

    while (1) {
        memset(filename, 0, MAXSIZE);
        recv(newsockfd, filename, MAXSIZE, 0);
        if (strcmp(filename, "exit") == 0) break;

        FILE *file = fopen(filename, "r");
        if (!file) {
            strcpy(buffer, "File not present");
            send(newsockfd, buffer, strlen(buffer), 0);
        } else {
            strcpy(buffer, "1. Search 2. Replace 3. Reorder 4. Exit");
            send(newsockfd, buffer, strlen(buffer), 0);
            fclose(file);
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
