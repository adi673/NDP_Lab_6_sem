#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_DOMAINS 5
#define BUFFER_SIZE 1024

// DNS Entry Structure
typedef struct {
    char domain[50];
    char ip[20];
} DNSRecord;

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // DNS Database (Structure-based)
    DNSRecord database[MAX_DOMAINS] = {
        {"www.google.com", "142.250.183.206"},
        {"www.facebook.com", "157.240.229.35"},
        {"www.github.com", "140.82.113.3"},
        {"www.openai.com", "104.18.0.123"},
        {"www.microsoft.com", "20.112.52.29"}
    };

    // Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind Address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections
    listen(sockfd, 5);
    printf("DNS Server is running on port %d...\n", PORT);

    // Accept client connection
    addr_size = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addr_size);
    if (newsockfd == -1) {
        perror("Accept failed");
        exit(1);
    }

    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // Receive domain name from client
        recv(newsockfd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "exit") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received domain: %s\n", buffer);

        // Search in DNS Database
        char response[BUFFER_SIZE] = "Domain not found!";
        for (int i = 0; i < MAX_DOMAINS; i++) {
            if (strcmp(database[i].domain, buffer) == 0) {
                strcpy(response, database[i].ip);
                break;
            }
        }

        // Send Response
        send(newsockfd, response, strlen(response), 0);
    }

    // Close connections
    close(newsockfd);
    close(sockfd);
    return 0;
}
