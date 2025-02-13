#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PORT 3388
#define MAXSIZE 1024

void handle_client(int client_sock) {
    char filename[MAXSIZE], buffer[MAXSIZE], option[2], search_str[MAXSIZE], replace_str[MAXSIZE];

    recv(client_sock, filename, sizeof(filename), 0);
    filename[strcspn(filename, "\n")] = '\0'; // Remove newline

    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        send(client_sock, "File not present", strlen("File not present"), 0);
        close(client_sock);
        return;
    }

    send(client_sock, "File found", strlen("File found"), 0);

    while (1) {
        recv(client_sock, option, sizeof(option), 0);

        if (option[0] == '1') { // Search
            recv(client_sock, search_str, sizeof(search_str), 0);

            lseek(fd, 0, SEEK_SET);
            int count = 0;
            char file_content[MAXSIZE];
            int file_size = read(fd, file_content, sizeof(file_content) - 1);
            file_content[file_size] = '\0';  // Null-terminate

            char *ptr = file_content;
            while ((ptr = strstr(ptr, search_str)) != NULL) {
                count++;
                ptr += strlen(search_str);
            }

            if (count > 0) {
                sprintf(buffer, "String found %d times", count);
            } else {
                strcpy(buffer, "String not found");
            }
            send(client_sock, buffer, strlen(buffer), 0);
        } 
        else if (option[0] == '2') { // Replace
            recv(client_sock, search_str, sizeof(search_str), 0);
            recv(client_sock, replace_str, sizeof(replace_str), 0);

            lseek(fd, 0, SEEK_SET);
            char file_content[MAXSIZE * 2], *ptr;
            int file_size = read(fd, file_content, sizeof(file_content) - 1);
            file_content[file_size] = '\0';

            ptr = strstr(file_content, search_str);
            if (ptr != NULL) {
                FILE *fp = fopen(filename, "w");
                char *start = file_content;

                while ((ptr = strstr(start, search_str)) != NULL) {
                    *ptr = '\0';
                    fprintf(fp, "%s%s", start, replace_str);
                    start = ptr + strlen(search_str);
                }
                fprintf(fp, "%s", start);
                fclose(fp);
                send(client_sock, "String replaced", strlen("String replaced"), 0);
            } else {
                send(client_sock, "String not found", strlen("String not found"), 0);
            }
        } 
        else if (option[0] == '3') { // Reorder
            lseek(fd, 0, SEEK_SET);
            char file_content[MAXSIZE];
            int file_size = read(fd, file_content, sizeof(file_content) - 1);
            file_content[file_size] = '\0';

            for (int i = 0; i < file_size - 1; i++) {
                for (int j = i + 1; j < file_size; j++) {
                    if (file_content[i] > file_content[j]) {
                        char temp = file_content[i];
                        file_content[i] = file_content[j];
                        file_content[j] = temp;
                    }
                }
            }

            FILE *fp = fopen(filename, "w");
            fprintf(fp, "%s", file_content);
            fclose(fp);
            send(client_sock, "File reordered by ASCII", strlen("File reordered by ASCII"), 0);
        } 
        else if (option[0] == '4') { // Exit
            break;
        }
    }

    close(fd);
    close(client_sock);
}

int main() {
    int sockfd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        exit(1);
    }

    if (listen(sockfd, 5) == -1) {
        perror("Listening failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);
    addr_size = sizeof(client_addr);

    while (1) {
        client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");
        handle_client(client_sock);
    }

    close(sockfd);
    return 0;
}
