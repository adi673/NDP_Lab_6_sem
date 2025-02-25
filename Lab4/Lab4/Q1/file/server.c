#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_request(int client_sock) {
    char request[BUFFER_SIZE], response[BUFFER_SIZE];

    // Receive the request
    recv(client_sock, request, sizeof(request), 0);
    request[strcspn(request, "\n")] = '\0';

    char option;
    char query[100];

    sscanf(request, "%c %s", &option, query);

    FILE *file;
    char line[BUFFER_SIZE];
    int found = 0;

    if (option == '1') {  // Registration Number - Name & Address
        file = fopen("students.txt", "r");
        if (file == NULL) {
            sprintf(response, "Error opening file.\n");
        } else {
            while (fgets(line, sizeof(line), file)) {
                char reg_no[20], name[50], address[100];
                sscanf(line, "%s %s %[^\n]", reg_no, name, address);
                if (strcmp(reg_no, query) == 0) {
                    sprintf(response, "PID: %d\nName: %s\nAddress: %s\n", getpid(), name, address);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                sprintf(response, "Registration Number not found.\n");
            }
            fclose(file);
        }
    } else if (option == '2') {  // Name - Enrollment Details
        file = fopen("enrollment.txt", "r");
        if (file == NULL) {
            sprintf(response, "Error opening file.\n");
        } else {
            while (fgets(line, sizeof(line), file)) {
                char name[50], dept[50], semester[10], section[10], courses[100];
                sscanf(line, "%s %s %s %s %[^\n]", name, dept, semester, section, courses);
                if (strcmp(name, query) == 0) {
                    sprintf(response, "PID: %d\nDept: %s\nSemester: %s\nSection: %s\nCourses: %s\n", getpid(), dept, semester, section, courses);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                sprintf(response, "Student Name not found.\n");
            }
            fclose(file);
        }
    } else if (option == '3') {  // Subject Code - Marks
        file = fopen("marks.txt", "r");
        if (file == NULL) {
            sprintf(response, "Error opening file.\n");
        } else {
            while (fgets(line, sizeof(line), file)) {
                char subject[10], marks[10];
                sscanf(line, "%s %s", subject, marks);
                if (strcmp(subject, query) == 0) {
                    sprintf(response, "PID: %d\nSubject Code: %s\nMarks: %s\n", getpid(), subject, marks);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                sprintf(response, "Subject Code not found.\n");
            }
            fclose(file);
        }
    } else {
        sprintf(response, "Invalid option.\n");
    }

    send(client_sock, response, strlen(response), 0);
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running...\n");

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {  // Child process
            close(server_sock);
            handle_request(client_sock);
            exit(0);
        } else {
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}
