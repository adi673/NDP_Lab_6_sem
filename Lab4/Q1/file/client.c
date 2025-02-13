#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\nMenu:\n1. Registration Number\n2. Name of the Student\n3. Subject Code\n4. Exit\nEnter your choice: ");
        char option;
        scanf(" %c", &option);
        getchar();

        if (option == '4') {
            printf("Exiting...\n");
            break;
        }

        printf("Enter query: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        char request[BUFFER_SIZE];
        sprintf(request, "%c %s", option, buffer);

        send(client_sock, request, strlen(request), 0);

        int n = recv(client_sock, buffer, sizeof(buffer), 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Response from server:\n%s\n", buffer);
        }
    }

    close(client_sock);
    return 0;
}
