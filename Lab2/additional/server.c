#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 3388
#define MAXSIZE 1024

void analyze_file(const char *filename, int client_sock) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        send(client_sock, "File not present", strlen("File not present"), 0);
        return;
    }

    char buffer[MAXSIZE], file_contents[MAXSIZE * 10] = "";
    int file_size = 0, alphabets = 0, lines = 0, spaces = 0, digits = 0, others = 0;
    
    while (fgets(buffer, MAXSIZE, fp)) {
        strcat(file_contents, buffer);
        file_size += strlen(buffer);
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isalpha(buffer[i])) alphabets++;
            else if (isdigit(buffer[i])) digits++;
            else if (isspace(buffer[i])) spaces++;
            else others++;
        }
        lines++;
    }
    fclose(fp);

    // Send file details
    send(client_sock, file_contents, strlen(file_contents), 0);
    sprintf(buffer, "\nFile Size: %d bytes\nAlphabets: %d\nLines: %d\nSpaces: %d\nDigits: %d\nOther Characters: %d", 
            file_size, alphabets, lines, spaces, digits, others);
    send(client_sock, buffer, strlen(buffer), 0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char filename[MAXSIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

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
        memset(filename, 0, MAXSIZE);
        recv(client_sock, filename, sizeof(filename), 0);
        filename[strcspn(filename, "\n")] = 0; // Remove newline

        if (strcmp(filename, "stop") == 0) {
            printf("Stopping server...\n");
            break;
        }

        analyze_file(filename, client_sock);
    }

    close(client_sock);
    close(server_sock);
    return 0;
}
