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
    int option;
    char query[50], response[256];

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
        printf("\nSelect an option:\n");
        printf("1. Search by Registration Number\n");
        printf("2. Search by Student Name\n");
        printf("3. Search by Subject Code\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &option);

        if (option == 4) {
            break;
        }

        printf("Enter your query: ");
        scanf("%s", query);

        send(sockfd, &option, sizeof(option), 0);
        send(sockfd, query, sizeof(query), 0);

        recv(sockfd, response, sizeof(response), 0);
        printf("Response from server:\n%s\n", response);
    }

    close(sockfd);
    return 0;
}
