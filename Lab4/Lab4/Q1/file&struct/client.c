#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void display_menu() {
    printf("\nMenu:\n");
    printf("1. Search by Registration Number\n");
    printf("2. Search by Name\n");
    printf("3. Search by Subject Code\n");
    printf("4. Exit\n");
}

int main() {
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(client_sock);
        exit(1);
    }

    while (1) {
        display_menu();
        printf("Enter your choice: ");
        int option;
        scanf("%d", &option);
        getchar(); // To consume newline

        if (option == 4) {
            printf("Exiting...\n");
            break;
        }

        char key[50];
        printf("Enter search key: ");
        fgets(key, 50, stdin);
        key[strcspn(key, "\n")] = '\0'; // Remove newline

        // Send option and key to server
        snprintf(buffer, BUFFER_SIZE, "%d %s", option, key);
        send(client_sock, buffer, strlen(buffer), 0);

        // Receive and print the response
        recv(client_sock, buffer, BUFFER_SIZE, 0);
        printf("\nResponse from server:\n%s\n", buffer);
    }

    close(client_sock);
    return 0;
}
