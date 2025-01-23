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
void permute(char *str, int l, int r, char *result) {
    if (l == r) {
        // Add the permutation to the result
        strcat(result, str);
        strcat(result, "\n");  // Add newline between permutations
    } else {
        for (int i = l; i <= r; i++) {
            swap((str + l), (str + i)); // Swap to create a new permutation
            permute(str, l + 1, r, result);
            swap((str + l), (str + i)); // Backtrack
        }
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[MAXSIZE];
    char result[MAXSIZE * MAXSIZE] = "";  // Buffer to store all permutations

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
    if (listen(server_sock, 1) == -1) {
        perror("Server listen failed");
        close(server_sock);
        exit(1);
    }

    printf("Server waiting for connection...\n");

    // Accept a client connection
    client_len = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == -1) {
        perror("Server accept failed");
        close(server_sock);
        exit(1);
    }

    printf("Server connected to client\n");

    // Receive the string from the client
    int n = recv(client_sock, buffer, MAXSIZE, 0);
    if (n == -1) {
        perror("Failed to receive data from client");
        close(client_sock);
        close(server_sock);
        exit(1);
    }
    buffer[n] = '\0'; // Null-terminate the received string

    printf("Received string: %s\n", buffer);

    // Generate all permutations and store them in the result buffer
    permute(buffer, 0, strlen(buffer) - 1, result);

    // Send all permutations to the client in one go
    send(client_sock, result, strlen(result), 0);

    // Close the client and server sockets
    close(client_sock);
    close(server_sock);
    unlink(SOCKET_PATH);

    return 0;
}

