#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAXSIZE 1024
#define DB_FILE "books.txt"

// Function to insert a new book
void insert_book(char *book_info, int client_sock) {
    FILE *file = fopen(DB_FILE, "a");
    if (!file) {
        send(client_sock, "Error opening database file.\n", 29, 0);
        return;
    }
    fprintf(file, "%s\n", book_info);
    fclose(file);
    send(client_sock, "Book inserted successfully.\n", 28, 0);
}

// Function to delete a book by title
void delete_book(char *title, int client_sock) {
    FILE *file = fopen(DB_FILE, "r");
    if (!file) {
        send(client_sock, "Error opening database file.\n", 29, 0);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        send(client_sock, "Error creating temp file.\n", 26, 0);
        fclose(file);
        return;
    }

    char line[MAXSIZE];
    int found = 0;
    while (fgets(line, MAXSIZE, file)) {
        if (strstr(line, title) == line) {
            found = 1;
            continue;
        }
        fprintf(temp, "%s", line);
    }

    fclose(file);
    fclose(temp);
    remove(DB_FILE);
    rename("temp.txt", DB_FILE);

    if (found)
        send(client_sock, "Book deleted successfully.\n", 27, 0);
    else
        send(client_sock, "Book not found.\n", 16, 0);
}

// Function to display all books
void display_books(int client_sock) {
    FILE *file = fopen(DB_FILE, "r");
    if (!file) {
        send(client_sock, "Error opening database file.\n", 29, 0);
        return;
    }

    char buffer[MAXSIZE] = "";
    char line[MAXSIZE];

    while (fgets(line, MAXSIZE, file)) {
        strcat(buffer, line);
    }
    fclose(file);

    if (strlen(buffer) == 0)
        strcpy(buffer, "No books found in database.\n");

    send(client_sock, buffer, strlen(buffer), 0);
}

// Function to search books by title or author
void search_book(char *query, int client_sock) {
    FILE *file = fopen(DB_FILE, "r");
    if (!file) {
        send(client_sock, "Error opening database file.\n", 29, 0);
        return;
    }

    char buffer[MAXSIZE] = "";
    char line[MAXSIZE];
    int found = 0;

    while (fgets(line, MAXSIZE, file)) {
        if (strstr(line, query)) {
            strcat(buffer, line);
            found = 1;
        }
    }
    fclose(file);

    if (!found)
        strcpy(buffer, "Book not found.\n");

    send(client_sock, buffer, strlen(buffer), 0);
}

int main() {
    int sockfd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    listen(sockfd, 5);
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive client request
        memset(buffer, 0, MAXSIZE);
        recv(client_sock, buffer, MAXSIZE, 0);

        // Process client request
        if (strncmp(buffer, "INSERT", 6) == 0) {
            insert_book(buffer + 7, client_sock);
        } else if (strncmp(buffer, "DELETE", 6) == 0) {
            delete_book(buffer + 7, client_sock);
        } else if (strncmp(buffer, "DISPLAY", 7) == 0) {
            display_books(client_sock);
        } else if (strncmp(buffer, "SEARCH", 6) == 0) {
            search_book(buffer + 7, client_sock);
        } else if (strncmp(buffer, "EXIT", 4) == 0) {
            send(client_sock, "Goodbye!\n", 9, 0);
            close(client_sock);
            break;
        } else {
            send(client_sock, "Invalid command.\n", 17, 0);
        }

        close(client_sock);
    }

    close(sockfd);
    return 0;
}
