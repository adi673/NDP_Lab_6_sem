#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_BOOKS 100
#define MAXSIZE 1024

// Structure for book details
typedef struct {
    char title[100];
    char author[100];
    char accession_number[20];
    int total_pages;
    char publisher[100];
} Book;

Book books[MAX_BOOKS];
int book_count = 0;

// Function to initialize books in server memory
void initialize_books() {
    strcpy(books[0].title, "C Programming");
    strcpy(books[0].author, "Dennis Ritchie");
    strcpy(books[0].accession_number, "101");
    books[0].total_pages = 300;
    strcpy(books[0].publisher, "Prentice Hall");

    strcpy(books[1].title, "Operating Systems");
    strcpy(books[1].author, "Silberschatz");
    strcpy(books[1].accession_number, "102");
    books[1].total_pages = 600;
    strcpy(books[1].publisher, "Wiley");

    strcpy(books[2].title, "Data Structures");
    strcpy(books[2].author, "Seymour Lipschutz");
    strcpy(books[2].accession_number, "103");
    books[2].total_pages = 450;
    strcpy(books[2].publisher, "McGraw Hill");

    book_count = 3;
}

// Function to insert a new book
void insert_book(char *book_info, int client_sock) {
    if (book_count >= MAX_BOOKS) {
        send(client_sock, "Book database is full.\n", 24, 0);
        return;
    }

    Book *b = &books[book_count];

    sscanf(book_info, "%[^;];%[^;];%[^;];%d;%[^;]", b->title, b->author, b->accession_number, &b->total_pages, b->publisher);
    book_count++;

    send(client_sock, "Book inserted successfully.\n", 28, 0);
}

// Function to delete a book by title
void delete_book(char *title, int client_sock) {
    int found = 0;
    for (int i = 0; i < book_count; i++) {
        if (strcmp(books[i].title, title) == 0) {
            found = 1;
            for (int j = i; j < book_count - 1; j++) {
                books[j] = books[j + 1];
            }
            book_count--;
            break;
        }
    }
    if (found)
        send(client_sock, "Book deleted successfully.\n", 27, 0);
    else
        send(client_sock, "Book not found.\n", 16, 0);
}

// Function to display all books
void display_books(int client_sock) {
    char buffer[MAXSIZE] = "";
    if (book_count == 0) {
        send(client_sock, "No books available.\n", 21, 0);
        return;
    }

    for (int i = 0; i < book_count; i++) {
        char book_info[256];
        snprintf(book_info, sizeof(book_info), "Title: %s, Author: %s, Accession No: %s, Pages: %d, Publisher: %s\n",
                 books[i].title, books[i].author, books[i].accession_number, books[i].total_pages, books[i].publisher);
        strcat(buffer, book_info);
    }

    send(client_sock, buffer, strlen(buffer), 0);
}

// Function to search books by title or author
void search_book(char *query, int client_sock) {
    char buffer[MAXSIZE] = "";
    int found = 0;

    for (int i = 0; i < book_count; i++) {
        if (strstr(books[i].title, query) || strstr(books[i].author, query)) {
            char book_info[256];
            snprintf(book_info, sizeof(book_info), "Title: %s, Author: %s, Accession No: %s, Pages: %d, Publisher: %s\n",
                     books[i].title, books[i].author, books[i].accession_number, books[i].total_pages, books[i].publisher);
            strcat(buffer, book_info);
            found = 1;
        }
    }

    if (!found)
        strcpy(buffer, "Book not found.\n");

    send(client_sock, buffer, strlen(buffer), 0);
}

int main() {
    int sockfd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAXSIZE];

    // Initialize book database
    initialize_books();

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
