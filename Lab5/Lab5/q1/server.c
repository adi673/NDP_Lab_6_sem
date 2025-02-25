#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#define PORT 12347
#define MAXSIZE 100

// Function to check if two strings are anagrams
int are_anagrams(char *str1, char *str2) {
    int count[256] = {0};

    // If lengths are not the same, they cannot be anagrams
    if (strlen(str1) != strlen(str2)) {
        return 0;
    }

    for (int i = 0; str1[i] != '\0'; i++) {
        count[str1[i]]++;
        count[str2[i]]--;
    }

    for (int i = 0; i < 256; i++) {
        if (count[i] != 0) {
            return 0;
        }
    }
    return 1;
}

// Function to get the current date and time
void get_current_time(char *time_str) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    strftime(time_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Function to handle communication with each client
void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[MAXSIZE];
    char str1[MAXSIZE], str2[MAXSIZE];
    char time_str[26];
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

    // Get current time
    get_current_time(time_str);

    // Display server date and time with client socket address
    printf("[Connected] %s\n", time_str);
    printf("Client IP: %s | Port: %d\n", client_ip, ntohs(client_addr.sin_port));

    // Receive two strings from client
    recv(client_sock, str1, MAXSIZE, 0);
    recv(client_sock, str2, MAXSIZE, 0);

    // Ensure strings are null-terminated
    str1[MAXSIZE - 1] = '\0';
    str2[MAXSIZE - 1] = '\0';

    // Log received strings
    printf("Received strings: '%s' and '%s'\n", str1, str2);

    // Check if the strings are anagrams
    if (are_anagrams(str1, str2)) {
        sprintf(buffer, "Strings are anagrams.");
    } else {
        sprintf(buffer, "Strings are NOT anagrams.");
    }

    // Send the result to the client
    send(client_sock, buffer, strlen(buffer), 0);
    
    // Close the client socket
    close(client_sock);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pid_t pid;

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_size);
        if (client_fd == -1) {
            perror("Accept failed");
            close(server_fd);
            exit(1);
        }

        // Fork a new process for each client
        pid = fork();
        if (pid == 0) {
            // Child process: handle client
            close(server_fd);
            handle_client(client_fd, client_addr);
            exit(0);
        } else {
            // Parent process: close the client socket
            close(client_fd);
        }
    }

    // Close the server socket
    close(server_fd);
    return 0;
}
