#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXSIZE 100
#define MAXDOMAINS 3

// Structure to hold domain name and IP address
struct dns_entry {
    char domain[MAXSIZE];
    char ip_address[MAXSIZE];
};

// Sample DNS entries
struct dns_entry dns_db[MAXDOMAINS] = {
    {"google.com", "8.8.8.8"},
    {"example.com", "93.184.216.34"},
    {"yahoo.com", "98.137.11.163"}
};

// Function to look up domain in the DNS database
char* lookup_domain(char* domain) {
    for (int i = 0; i < MAXDOMAINS; i++) {
        if (strcmp(dns_db[i].domain, domain) == 0) {
            return dns_db[i].ip_address;
        }
    }
    return NULL;  // Domain not found
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    char buffer[MAXSIZE];

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address
    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Listen for incoming connections
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
            // Receive domain name from client
            memset(buffer, 0, MAXSIZE);
            int bytes_received = recv(newsockfd, buffer, MAXSIZE - 1, 0);
            if (bytes_received <= 0) {
                printf("Client disconnected.\n");
                break;
            }
            buffer[bytes_received] = '\0';  // Ensure null termination
            printf("Received domain name: %s\n", buffer);

            // Look up domain name in the DNS database
            char *ip = lookup_domain(buffer);
            if (ip) {
                send(newsockfd, ip, strlen(ip) + 1, 0);  // Send IP address
            } else {
                send(newsockfd, "Domain not found", strlen("Domain not found") + 1, 0);
            }
        }

        close(newsockfd);  // Close connection with this client
    }

    close(sockfd);  // Close the server socket
    return 0;
}
