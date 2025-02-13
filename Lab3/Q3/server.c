#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>
#include <sys/wait.h>

#define PORT 3388
#define MAXSIZE 1024

// Function to sort a string in ascending order
void sort_ascending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (str[i] > str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

// Function to sort a string in descending order
void sort_descending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (str[i] < str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[MAXSIZE], digits[MAXSIZE], chars[MAXSIZE];

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    // Listen for connections
    if (listen(server_sock, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    if (client_sock == -1) {
        perror("Client connection failed");
        exit(1);
    }
    printf("Client connected...\n");

    while (1) {
        memset(buffer, 0, MAXSIZE);
        recv(client_sock, buffer, sizeof(buffer), 0);
        if (strcmp(buffer, "exit") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Received string: %s\n", buffer);

        // Separate characters and digits
        int charIndex = 0, digitIndex = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isdigit(buffer[i]))
                digits[digitIndex++] = buffer[i];
            else if (isalpha(buffer[i]))
                chars[charIndex++] = buffer[i];
        }
        digits[digitIndex] = '\0';
        chars[charIndex] = '\0';

        pid_t pid = fork();

        if (pid == 0) { // Child process (sort numbers in ascending order)
            sort_ascending(digits);
            sprintf(buffer, "Child Process [%d]: %s", getpid(), digits);
            send(client_sock, buffer, strlen(buffer), 0);
            exit(0);
        } else { // Parent process (sort characters in descending order)
            wait(NULL);
            sort_descending(chars);
            sprintf(buffer, "Parent Process [%d]: %s", getpid(), chars);
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
