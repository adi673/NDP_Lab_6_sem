#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAXSIZE], input[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        printf("\nMenu:\n");
        printf("1. Insert a new book\n");
        printf("2. Delete a book\n");
        printf("3. Display all books\n");
        printf("4. Search for a book\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        fgets(input, MAXSIZE, stdin);

        int choice = atoi(input);
        memset(buffer, 0, MAXSIZE);

        if (choice == 1) {
            printf("Enter book details (Title;Author;Accession Number;Total Pages;Publisher): ");
            fgets(input, MAXSIZE, stdin);
            snprintf(buffer, MAXSIZE, "INSERT %s", input);
        } else if (choice == 2) {
            printf("Enter book title to delete: ");
            fgets(input, MAXSIZE, stdin);
            snprintf(buffer, MAXSIZE, "DELETE %s", input);
        } else if (choice == 3) {
            strcpy(buffer, "DISPLAY");
        } else if (choice == 4) {
            printf("Enter title or author to search: ");
            fgets(input, MAXSIZE, stdin);
            snprintf(buffer, MAXSIZE, "SEARCH %s", input);
        } else if (choice == 5) {
            strcpy(buffer, "EXIT");
            send(sockfd, buffer, strlen(buffer), 0);
            break;
        } else {
            printf("Invalid choice. Try again.\n");
            continue;
        }

        send(sockfd, buffer, strlen(buffer), 0);
        memset(buffer, 0, MAXSIZE);
        recv(sockfd, buffer, MAXSIZE, 0);
        printf("Server response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
