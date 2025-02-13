#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/calc_socket"
#define MAXSIZE 1024

int main() {
    int client_sock;
    struct sockaddr_un server_addr;
    char buffer[MAXSIZE];
    int option;

    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_sock);
        exit(1);
    }

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add/Subtract two integers\n");
        printf("2. Solve linear equation (ax + b = 0)\n");
        printf("3. Multiply two matrices\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        send(client_sock, &option, sizeof(int), 0);

        if (option == 1) {
            int a, b;
            printf("Enter two integers: ");
            scanf("%d %d", &a, &b);
            send(client_sock, &a, sizeof(int), 0);
            send(client_sock, &b, sizeof(int), 0);
        } else if (option == 2) {
            int a, b;
            printf("Enter coefficients a and b (for ax + b = 0): ");
            scanf("%d %d", &a, &b);
            send(client_sock, &a, sizeof(int), 0);
            send(client_sock, &b, sizeof(int), 0);
        } else if (option == 3) {
            int m1, n1, m2, n2;
            printf("Enter dimensions of first matrix (rows cols): ");
            scanf("%d %d", &m1, &n1);
            int mat1[m1][n1];
            printf("Enter first matrix:\n");
            for (int i = 0; i < m1; i++)
                for (int j = 0; j < n1; j++)
                    scanf("%d", &mat1[i][j]);

            printf("Enter dimensions of second matrix (rows cols): ");
            scanf("%d %d", &m2, &n2);
            int mat2[m2][n2];
            printf("Enter second matrix:\n");
            for (int i = 0; i < m2; i++)
                for (int j = 0; j < n2; j++)
                    scanf("%d", &mat2[i][j]);

            send(client_sock, &m1, sizeof(int), 0);
            send(client_sock, &n1, sizeof(int), 0);
            for (int i = 0; i < m1; i++)
                send(client_sock, mat1[i], n1 * sizeof(int), 0);
            send(client_sock, &m2, sizeof(int), 0);
            send(client_sock, &n2, sizeof(int), 0);
            for (int i = 0; i < m2; i++)
                send(client_sock, mat2[i], n2 * sizeof(int), 0);
        } else if (option == 4) {
            break;
        }

        recv(client_sock, buffer, MAXSIZE, 0);
        printf("Result: %s\n", buffer);
    }

    close(client_sock);
    return 0;
}
