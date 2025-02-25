#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 100
#define FILENAME "dns_records.txt"

// Function to search for domain in the file
char* lookup_domain(char* domain, char* ip_address) {
    FILE* file = fopen(FILENAME, "r");
    if (!file) {
        perror("File opening failed");
        return NULL;
    }

    char file_domain[MAXSIZE], file_ip[MAXSIZE];
    while (fscanf(file, "%s %s", file_domain, file_ip) != EOF) {
        if (strcmp(file_domain, domain) == 0) {
            strcpy(ip_address, file_ip);
            fclose(file);
            return ip_address;
        }
    }

    fclose(file);
    return NULL;  // Not found
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    char buffer[MAXSIZE];

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Server address setup
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    if (listen(sockfd, 5) == -1) {
        perror("Listen failed");
        exit(1);
    }

    printf("Server is running. Waiting for clients...\n");

    while (1) {
        // Accept client connection
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Connected to a client.\n");

        while (1) {
            // Receive domain name
            memset(buffer, 0, MAXSIZE);
            int bytes_received = recv(newsockfd, buffer, MAXSIZE - 1, 0);
            if (bytes_received <= 0) {
                printf("Client disconnected.\n");
                break;
            }
            buffer[bytes_received] = '\0';
            printf("Received domain name: %s\n", buffer);

            // Search in file
            char ip_address[MAXSIZE];
            char* result = lookup_domain(buffer, ip_address);
            if (result) {
                send(newsockfd, ip_address, strlen(ip_address) + 1, 0);
            } else {
                send(newsockfd, "Domain not found", strlen("Domain not found") + 1, 0);
            }
        }

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
