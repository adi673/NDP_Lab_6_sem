#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAXSIZE 100
#define SOCKET_PATH "/tmp/socket_file"

// Function to swap characters
void swap(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

// Recursive function to generate all permutations of the string
void permute(char *str, int l, int r, int client_sock) {
    if (l == r) {
        char buffer[MAXSIZE];  // Temporary buffer for sending each permutation
        snprintf(buffer, sizeof(buffer), "%s\n", str);
        send(client_sock, buffer, strlen(buffer), 0);
    } else {
        for (int i = l; i <= r; i++) {
            swap((str + l), (str + i)); // Swap to create a new permutation
            permute(str, l + 1, r, client_sock);
            swap((str + l), (str + i)); // Backtrack
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAXSIZE];

    // Create socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Server socket creation failed");
        exit(1);
    }

    // Remove any existing socket file
    unlink(SOCKET_PATH);

    // Set up the server address
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Server bind failed");
        close(server_sock);
        exit(1);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) == -1) {
        perror("Server listen failed");
        close(server_sock);
        exit(1);
    }

    printf("Server waiting for connections...\n");

    while (1) {
        // Accept a client connection
        client_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == -1) {
            perror("Server accept failed");
            continue; // Keep server running even if accept fails
        }

        printf("Server connected to client\n");

        while (1) {
            // Receive the string from the client
            memset(buffer, 0, MAXSIZE);
            int n = recv(client_sock, buffer, MAXSIZE - 1, 0);
            if (n <= 0) {
                printf("Client disconnected.\n");
                close(client_sock);
                break;
            }

            buffer[n] = '\0'; // Null-terminate the received string
            printf("Received string: %s\n", buffer);

            // If client sends "exit", close connection
            if (strcmp(buffer, "exit") == 0) {
                printf("Client requested to exit. Closing connection.\n");
                close(client_sock);
                break;
            }

            // Generate and send each permutation directly
            permute(buffer, 0, strlen(buffer) - 1, client_sock);
        }
    }

    // Close the server socket (this line is unreachable but kept for completeness)
    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
