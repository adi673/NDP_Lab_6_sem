#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    char title[100];
    char author[100];
    int accession_number;
    int total_pages;
    char publisher[100];
} Book;

void insert_book(int sockfd);
void delete_book(int sockfd);
void display_books(int sockfd);
void search_book(int sockfd);

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    int choice;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    while (1) {
        printf("\nBook Database Operations\n");
        printf("1. Insert new book\n");
        printf("2. Delete a book\n");
        printf("3. Display all books\n");
        printf("4. Search a book\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        send(sockfd, &choice, sizeof(choice), 0);

        switch (choice) {
            case 1: insert_book(sockfd); break;
            case 2: delete_book(sockfd); break;
            case 3: display_books(sockfd); break;
            case 4: search_book(sockfd); break;
            case 5: 
                printf("Exiting...\n");
                close(sockfd);
                return 0;
            default: printf("Invalid choice\n");
        }
    }

    close(sockfd);
    return 0;
}

void insert_book(int sockfd) {
    Book new_book;
    printf("Enter book title: ");
    getchar();  // Consume newline left by previous scanf
    fgets(new_book.title, sizeof(new_book.title), stdin);
    new_book.title[strcspn(new_book.title, "\n")] = '\0';  // Remove newline

    printf("Enter author name: ");
    fgets(new_book.author, sizeof(new_book.author), stdin);
    new_book.author[strcspn(new_book.author, "\n")] = '\0';

    printf("Enter accession number: ");
    scanf("%d", &new_book.accession_number);

    printf("Enter total pages: ");
    scanf("%d", &new_book.total_pages);

    printf("Enter publisher name: ");
    getchar();  // Consume newline
    fgets(new_book.publisher, sizeof(new_book.publisher), stdin);
    new_book.publisher[strcspn(new_book.publisher, "\n")] = '\0';

    send(sockfd, &new_book, sizeof(new_book), 0);
    char response[BUFFER_SIZE];
    recv(sockfd, response, sizeof(response), 0);
    printf("%s\n", response);
}

void delete_book(int sockfd) {
    int accession_number;
    printf("Enter accession number to delete: ");
    scanf("%d", &accession_number);
    send(sockfd, &accession_number, sizeof(accession_number), 0);

    char response[BUFFER_SIZE];
    recv(sockfd, response, sizeof(response), 0);
    printf("%s\n", response);
}

void display_books(int sockfd) {
    char response[BUFFER_SIZE];
    recv(sockfd, response, sizeof(response), 0);
    printf("%s\n", response);
}

void search_book(int sockfd) {
    char search_term[100];
    printf("Enter title or author to search: ");
    getchar();  // Consume newline
    fgets(search_term, sizeof(search_term), stdin);
    search_term[strcspn(search_term, "\n")] = '\0';

    send(sockfd, search_term, sizeof(search_term), 0);

    char response[BUFFER_SIZE];
    recv(sockfd, response, sizeof(response), 0);
    printf("%s\n", response);
}
