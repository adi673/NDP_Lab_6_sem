#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAXSIZE 100
#define SOCKET_PATH "/tmp/socket_file"

int main() {
    int client_sock;
    struct sockaddr_un server_addr;
    char buffer[MAXSIZE * MAXSIZE];  // Buffer to hold all permutations

    // Create socket
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Client socket creation failed");
        exit(1);
    }

    // Set up the server address
    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // Connect to the server
    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("Client connect failed");
        close(client_sock);
        exit(1);
    }

    printf("Enter a string to send to the server: ");
    fgets(buffer, MAXSIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character if present

    // Send the string to the server
    send(client_sock, buffer, strlen(buffer), 0);

    // Receive all permutations from the server at once
    int n = recv(client_sock, buffer, sizeof(buffer), 0);
    if (n == -1) {
        perror("Failed to receive data from server");
        close(client_sock);
        exit(1);
    }

    buffer[n] = '\0';  // Null-terminate the received data

    // Print all the permutations received from the server
    printf("Received permutations from the server:\n%s", buffer);

    // Close the client socket
    close(client_sock);

    return 0;
}

