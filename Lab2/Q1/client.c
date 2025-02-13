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
    int fd, count = 0;

    // Receive file name from client
    recv(client_sock, filename, sizeof(filename), 0);
    filename[strcspn(filename, "\n")] = 0;

    // Open the file
    fd = open(filename, O_RDWR);
    if (fd == -1) {
        send(client_sock, "File not present", strlen("File not present"), 0);
        close(client_sock);
        return;
    }

    // Send confirmation
    send(client_sock, "File found", strlen("File found"), 0);

    while (1) {
        memset(option, 0, sizeof(option));
        recv(client_sock, option, sizeof(option), 0);

        if (option[0] == '1') { // Search
            recv(client_sock, search_str, sizeof(search_str), 0);
            search_str[strcspn(search_str, "\n")] = 0;

            lseek(fd, 0, SEEK_SET);
            count = 0;
            while (read(fd, buffer, sizeof(buffer)) > 0) {
                char *ptr = buffer;
                while ((ptr = strstr(ptr, search_str)) != NULL) {
                    count++;
                    ptr++;
                }
            }
            if (count > 0) {
                sprintf(buffer, "String found %d times", count);
            } else {
                strcpy(buffer, "String not found");
            }
            send(client_sock, buffer, strlen(buffer), 0);
        
        } else if (option[0] == '2') { // Replace
            recv(client_sock, search_str, sizeof(search_str), 0);
            recv(client_sock, replace_str, sizeof(replace_str), 0);

            lseek(fd, 0, SEEK_SET);
            char file_content[MAXSIZE], *ptr;
            read(fd, file_content, sizeof(file_content));
            ptr = strstr(file_content, search_str);

            if (ptr != NULL) {
                FILE *fp = fopen(filename, "w");
                while ((ptr = strstr(file_content, search_str)) != NULL) {
                    *ptr = '\0';
                    fprintf(fp, "%s%s", file_content, replace_str);
                    file_content = ptr + strlen(search_str);
                }
                fprintf(fp, "%s", file_content);
                fclose(fp);
                send(client_sock, "String replaced", strlen("String replaced"), 0);
            } else {
                send(client_sock, "String not found", strlen("String not found"), 0);
            }

        } else if (option[0] == '3') { // Reorder
            lseek(fd, 0, SEEK_SET);
            read(fd, buffer, sizeof(buffer));
            int len = strlen(buffer);
            for (int i = 0; i < len - 1; i++) {
                for (int j = i + 1; j < len; j++) {
                    if (buffer[i] > buffer[j]) {
                        char temp = buffer[i];
                        buffer[i] = buffer[j];
                        buffer[j] = temp;
                    }
                }
            }
            FILE *fp = fopen(filename, "w");
            fprintf(fp, "%s", buffer);
            fclose(fp);
            send(client_sock, "File content sorted", strlen("File content sorted"), 0);
        
        } else if (option[0] == '4') { // Exit
            break;
        }
    }
    close(fd);
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    printf("Server listening on port %d...\n", PORT);
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock == -1) {
            perror("Client accept failed");
            continue;
        }
        handle_client(client_sock);
    }

    close(server_sock);
    return 0;
}
