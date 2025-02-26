#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_STUDENTS 3

// Structure to store student details
typedef struct {
    char reg_no[10];
    char name[50];
    char address[100];
    char dept[20];
    int semester;
    char section;
    char courses[3][10];  // 3 subjects
    int marks[3];         // Marks for 3 subjects
} Student;

// Sample student database
Student database[MAX_STUDENTS] = {
    {"S101", "Alice", "123 Main St", "CSE", 3, 'A', {"CS101", "CS102", "CS103"}, {85, 90, 75}},
    {"S102", "Bob", "456 Park Ave", "ECE", 2, 'B', {"EC201", "EC202", "EC203"}, {78, 82, 88}},
    {"S103", "Charlie", "789 Elm St", "ME", 4, 'C', {"ME301", "ME302", "ME303"}, {91, 87, 80}}
};

// Function to handle requests
void process_request(int client_sock) {
    int option;
    char query[50];
    recv(client_sock, &option, sizeof(option), 0);
    recv(client_sock, query, sizeof(query), 0);

    Student *student = NULL;
    char response[256];
    memset(response, 0, sizeof(response));

    // Search for the student in the database
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if ((option == 1 && strcmp(database[i].reg_no, query) == 0) ||
            (option == 2 && strcmp(database[i].name, query) == 0) ||
            (option == 3 && strcmp(database[i].courses[0], query) == 0) || 
            (option == 3 && strcmp(database[i].courses[1], query) == 0) ||
            (option == 3 && strcmp(database[i].courses[2], query) == 0)) {
            student = &database[i];
            break;
        }
    }

    if (student == NULL) {
        strcpy(response, "No student found.");
    } else {
        pid_t pid = fork();
        if (pid == 0) { // Child process handles request
            if (option == 1) { 
                sprintf(response, "PID: %d\nName: %s\nAddress: %s", getpid(), student->name, student->address);
            } else if (option == 2) { 
                sprintf(response, "PID: %d\nDept: %s\nSemester: %d\nSection: %c\nCourses: %s, %s, %s", 
                        getpid(), student->dept, student->semester, student->section, 
                        student->courses[0], student->courses[1], student->courses[2]);
            } else if (option == 3) { 
                sprintf(response, "PID: %d\nMarks: %d, %d, %d", 
                        getpid(), student->marks[0], student->marks[1], student->marks[2]);
            }
            send(client_sock, response, sizeof(response), 0);
            exit(0);
        } else if (pid > 0) { // Parent does nothing, just continues
            wait(NULL);
        }
    }
    send(client_sock, response, sizeof(response), 0);
}

int main() {
    int sockfd, client_sock;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addr_size;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("Bind failed");
        exit(1);
    }

    listen(sockfd, 5);
    printf("Server listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(clientaddr);
        client_sock = accept(sockfd, (struct sockaddr*)&clientaddr, &addr_size);
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }
        process_request(client_sock);
        close(client_sock);
    }

    close(sockfd);
    return 0;
}
