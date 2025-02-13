#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/calc_socket"
#define MAXSIZE 1024

void handle_request(int client_sock)
{
    int option;
    char buffer[MAXSIZE];

    while (1)
    {
        // Receive the option from the client
        int n = recv(client_sock, &option, sizeof(int), 0);
        if (n <= 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        // Process the request
        switch (option)
        {
        case 1:
        { // Addition & Subtraction
            int a, b, sum, diff;
            recv(client_sock, &a, sizeof(int), 0);
            recv(client_sock, &b, sizeof(int), 0);
            sum = a + b;
            diff = a - b;
            snprintf(buffer, sizeof(buffer), "Sum: %d, Difference: %d", sum, diff);
            send(client_sock, buffer, strlen(buffer), 0);
            break;
        }

        case 2:
        { // Solve linear equation (ax + b = 0)
            int a, b;
            double x;
            char response[MAXSIZE]; // Use a clean buffer for response

            recv(client_sock, &a, sizeof(int), 0);
            recv(client_sock, &b, sizeof(int), 0);

            if (a == 0)
            {
                snprintf(response, sizeof(response), "Invalid equation (a cannot be 0)");
            }
            else
            {
                x = -(double)b / a;
                if (x == -0.00) // ✅ Fix for -0.00 appearing
                    x = 0.00;
                snprintf(response, sizeof(response), "x = %.2f", x);
            }

            send(client_sock, response, strlen(response) + 1, 0); // ✅ Send with null-terminator
            break;
        }

        case 3:
        { // Matrix Multiplication
            int m1, n1, m2, n2;
            recv(client_sock, &m1, sizeof(int), 0);
            recv(client_sock, &n1, sizeof(int), 0);
            int mat1[m1][n1];
            for (int i = 0; i < m1; i++)
                recv(client_sock, mat1[i], n1 * sizeof(int), 0);

            recv(client_sock, &m2, sizeof(int), 0);
            recv(client_sock, &n2, sizeof(int), 0);
            int mat2[m2][n2];
            for (int i = 0; i < m2; i++)
                recv(client_sock, mat2[i], n2 * sizeof(int), 0);

            if (n1 != m2)
            {
                strcpy(buffer, "Matrix multiplication not possible.");
            }
            else
            {
                int res[m1][n2];
                memset(res, 0, sizeof(res));
                for (int i = 0; i < m1; i++)
                    for (int j = 0; j < n2; j++)
                        for (int k = 0; k < n1; k++)
                            res[i][j] += mat1[i][k] * mat2[k][j];

                snprintf(buffer, sizeof(buffer), "Resultant Matrix:\n");
                for (int i = 0; i < m1; i++)
                {
                    for (int j = 0; j < n2; j++)
                    {
                        char temp[10];
                        snprintf(temp, sizeof(temp), "%d ", res[i][j]);
                        strcat(buffer, temp);
                    }
                    strcat(buffer, "\n");
                }
            }
            send(client_sock, buffer, strlen(buffer), 0);
            break;
        }

        case 4:
            printf("Client requested exit.\n");
            return;

        default:
            strcpy(buffer, "Invalid option.");
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }
}

int main()
{
    int server_sock, client_sock;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;

    // Create socket
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        perror("Socket creation failed");
        exit(1);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("Bind failed");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 5) == -1)
    {
        perror("Listen failed");
        close(server_sock);
        exit(1);
    }

    printf("Server waiting for connections...\n");

    while (1)
    {
        client_len = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock == -1)
        {
            perror("Accept failed");
            continue;
        }

        printf("Client connected.\n");
        handle_request(client_sock);
        close(client_sock);
    }

    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
