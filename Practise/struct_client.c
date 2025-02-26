#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;
    char query[50], response[256];
    int option;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        printf("\nEnter Option:\n1. Registration Number\n2. Name of the Student\n3. Subject Code\nEnter choice: ");
        scanf("%d", &option);
        getchar();  // Clear newline from buffer

        printf("Enter your query: ");
        fgets(query, sizeof(query), stdin);
        query[strcspn(query, "\n")] = '\0';

        send(sockfd, &option, sizeof(int), 0);
        send(sockfd, query, sizeof(query), 0);

        recv(sockfd, response, sizeof(response), 0);
        printf("\nServer Response:\n%s\n", response);
    }

    close(sockfd);
    return 0;
}
