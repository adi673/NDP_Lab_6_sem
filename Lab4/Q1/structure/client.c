#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[MAXLINE], choice[10], data[50];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    while (1) {
        // Display menu
        printf("\nMenu:\n1. Registration Number\n2. Name of the Student\n3. Subject Code\n4. Exit\nEnter your choice: ");
        fgets(choice, sizeof(choice), stdin);
        choice[strcspn(choice, "\n")] = 0;

        if (strcmp(choice, "4") == 0) {
            printf("Exiting...\n");
            break;
        }

        // Get data input
        printf("Enter the value: ");
        fgets(data, sizeof(data), stdin);
        data[strcspn(data, "\n")] = 0;

        // Send request to server
        snprintf(buffer, sizeof(buffer), "%s %s", choice, data);
        send(sock, buffer, strlen(buffer), 0);

        // Receive and display response
        recv(sock, buffer, sizeof(buffer), 0);
        printf("Server Response:\n%s", buffer);
    }

    close(sock);
    return 0;
}
