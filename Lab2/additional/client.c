#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char filename[MAXSIZE], buffer[MAXSIZE * 10];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server.\n");

    while (1) {
        printf("\nEnter filename (or 'stop' to exit): ");
        fgets(filename, MAXSIZE, stdin);
        filename[strcspn(filename, "\n")] = 0; // Remove newline

        send(sockfd, filename, strlen(filename), 0);
        if (strcmp(filename, "stop") == 0) {
            break;
        }

        // Receive file content
        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("File Content:\n%s\n", buffer);

        // Receive file statistics
        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, sizeof(buffer), 0);
        printf("File Statistics:\n%s\n", buffer);
    }

    close(sockfd);
    return 0;
}
