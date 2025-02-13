#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define PORT 3388
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char filename[MAXSIZE], buffer[MAXSIZE], option[2], search_str[MAXSIZE], replace_str[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        exit(1);
    }

    printf("Enter filename: ");
    fgets(filename, MAXSIZE, stdin);
    send(sockfd, filename, strlen(filename), 0);

    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);
    if (strcmp(buffer, "File not present") == 0) {
        close(sockfd);
        return 0;
    }

    while (1) {
        printf("\nMenu:\n1. Search\n2. Replace\n3. Reorder\n4. Exit\nEnter choice: ");
        scanf("%s", option);
        send(sockfd, option, sizeof(option), 0);

        if (option[0] == '1') { // Search
            printf("Enter string to search: ");
            scanf("%s", search_str);
            send(sockfd, search_str, sizeof(search_str), 0);
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);

        } else if (option[0] == '2') { // Replace
            printf("Enter string to replace: ");
            scanf("%s", search_str);
            printf("Enter new string: ");
            scanf("%s", replace_str);
            send(sockfd, search_str, sizeof(search_str), 0);
            send(sockfd, replace_str, sizeof(replace_str), 0);
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);

        } else if (option[0] == '3') { // Reorder
            recv(sockfd, buffer, sizeof(buffer), 0);
            printf("%s\n", buffer);

        } else if (option[0] == '4') { // Exit
            break;
        }
    }

    close(sockfd);
    return 0;
}
