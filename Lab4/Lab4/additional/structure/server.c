#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define MAX_BOOKS 100
#define BUFFER_SIZE 1024

typedef struct {
    char title[100];
    char author[100];
    int accession_number;
    int total_pages;
    char publisher[100];
} Book;

Book book_db[MAX_BOOKS];
int book_count = 0;

void insert_book(int sockfd);
void delete_book(int sockfd);
void display_books(int sockfd);
void search_book(int sockfd);
void handle_client(int client_sock);

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock < 0) {
            perror("Server accept failed");
            continue;
        }

        handle_client(client_sock);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}

void handle_client(int client_sock) {
    int choice;
    while (1) {
        recv(client_sock, &choice, sizeof(choice), 0);
        switch (choice) {
            case 1: insert_book(client_sock); break;
            case 2: delete_book(client_sock); break;
            case 3: display_books(client_sock); break;
            case 4: search_book(client_sock); break;
            case 5: 
                send(client_sock, "Exiting server...", 17, 0);
                return;
            default: 
                send(client_sock, "Invalid option", 14, 0); 
                break;
        }
    }
}

void insert_book(int sockfd) {
    Book new_book;
    recv(sockfd, &new_book, sizeof(new_book), 0);

    if (book_count < MAX_BOOKS) {
        book_db[book_count++] = new_book;
        send(sockfd, "Book inserted successfully", 26, 0);
    } else {
        send(sockfd, "Database is full", 17, 0);
    }
}

void delete_book(int sockfd) {
    int accession_number;
    recv(sockfd, &accession_number, sizeof(accession_number), 0);

    for (int i = 0; i < book_count; i++) {
        if (book_db[i].accession_number == accession_number) {
            for (int j = i; j < book_count - 1; j++) {
                book_db[j] = book_db[j + 1];
            }
            book_count--;
            send(sockfd, "Book deleted successfully", 25, 0);
            return;
        }
    }
    send(sockfd, "Book not found", 15, 0);
}

void display_books(int sockfd) {
    char buffer[BUFFER_SIZE] = "";
    for (int i = 0; i < book_count; i++) {
        char book_info[BUFFER_SIZE];
        snprintf(book_info, sizeof(book_info), 
                 "Title: %s, Author: %s, Accession Number: %d, Pages: %d, Publisher: %s\n",
                 book_db[i].title, book_db[i].author, book_db[i].accession_number,
                 book_db[i].total_pages, book_db[i].publisher);
        strcat(buffer, book_info);
    }

    if (strlen(buffer) == 0) {
        send(sockfd, "No books available", 18, 0);
    } else {
        send(sockfd, buffer, strlen(buffer), 0);
    }
}

void search_book(int sockfd) {
    char search_term[100];
    recv(sockfd, search_term, sizeof(search_term), 0);

    char result[BUFFER_SIZE] = "";
    int found = 0;
    for (int i = 0; i < book_count; i++) {
        if (strstr(book_db[i].title, search_term) || strstr(book_db[i].author, search_term)) {
            char book_info[BUFFER_SIZE];
            snprintf(book_info, sizeof(book_info), 
                     "Title: %s, Author: %s, Accession Number: %d, Pages: %d, Publisher: %s\n",
                     book_db[i].title, book_db[i].author, book_db[i].accession_number,
                     book_db[i].total_pages, book_db[i].publisher);
            strcat(result, book_info);
            found = 1;
        }
    }

    if (found) {
        send(sockfd, result, strlen(result), 0);
    } else {
        send(sockfd, "No books found", 15, 0);
    }
}
