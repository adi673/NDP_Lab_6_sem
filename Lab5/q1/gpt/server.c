#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 100
#define MAX_CLIENTS 2

int client_count = 0;
char received_strings[MAX_CLIENTS][MAXSIZE];

void handle_client(int client_sock, int client_no) {
    char buffer[MAXSIZE];
    memset(buffer, '\0', sizeof(buffer));

    recv(client_sock, buffer, sizeof(buffer), 0);
    printf("Received from Client %d: %s\n", client_no + 1, buffer);

    strncpy(received_strings[client_no], buffer, MAXSIZE);

    if (client_count == MAX_CLIENTS) {
        char final_message[MAXSIZE];
        snprintf(final_message, sizeof(final_message), "Manipal %s %s", received_strings[0], received_strings[1]);
        printf("Final Message: %s\n", final_message);
        
        send(client_sock, final_message, strlen(final_message), 0);
    } else {
        send(client_sock, "Waiting for another client...", 30, 0);
    }

    close(client_sock);
    exit(0);
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addr_size;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket Creation Error");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12346);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Binding failed");
        close(sockfd);
        return -1;
    }

    listen(sockfd, MAX_CLIENTS);
    printf("Server is listening...\n");

    while (client_count < MAX_CLIENTS) {
        addr_size = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addr_size);
        if (newsockfd == -1) {
            perror("Accept error");
            continue;
        }

        printf("Client %d connected: %s:%d\n", client_count + 1, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        
        if (fork() == 0) {
            close(sockfd);
            handle_client(newsockfd, client_count);
        }

        client_count++;
    }

    printf("Max clients connected. Sending termination message...\n");

    // Notify all clients to terminate
    for (int i = 0; i < MAX_CLIENTS; i++) {
        send(newsockfd, "Terminate session", 17, 0);
    }

    close(sockfd);
    return 0;
}
