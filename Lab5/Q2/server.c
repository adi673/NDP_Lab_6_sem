#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 100
#define PORT 12345

// Function to check if two strings are anagrams
int areAnagrams(char *str1, char *str2) {
    int count[256] = {0}; 

    if (strlen(str1) != strlen(str2))
        return 0;

    for (int i = 0; str1[i] && str2[i]; i++) {
        count[str1[i]]++;
        count[str2[i]]--;
    }

    for (int i = 0; i < 256; i++)
        if (count[i] != 0)
            return 0;

    return 1;
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addr_size;
    char str1[MAXSIZE], str2[MAXSIZE], result[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Binding failed");
        close(sockfd);
        return -1;
    }

    listen(sockfd, 5);
    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addr_size);
        if (newsockfd == -1) {
            perror("Accept failed");
            continue;
        }

        // Log the connection with date & time
        time_t now;
        time(&now);
        printf("\n[Connected] %sClient IP: %s | Port: %d\n", ctime(&now), inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // Receive strings from the client
        recv(newsockfd, str1, sizeof(str1), 0);
        recv(newsockfd, str2, sizeof(str2), 0);

        printf("Received strings: '%s' and '%s'\n", str1, str2);

        // Check for anagram
        if (areAnagrams(str1, str2)) {
            strcpy(result, "Strings are anagrams.");
        } else {
            strcpy(result, "Strings are NOT anagrams.");
        }

        // Send result back to client
        send(newsockfd, result, strlen(result), 0);
        printf("Sent result: %s\n", result);

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
