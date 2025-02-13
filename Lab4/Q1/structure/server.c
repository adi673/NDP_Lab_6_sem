#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_STUDENTS 5
#define MAXLINE 1024

// Structure to store student information
typedef struct {
    char reg_no[20];
    char name[50];
    char address[100];
    char department[10];
    int semester;
    char section;
    char courses[100];
    char subject_code[10];
    int marks;
} Student;

// Sample student database (Array of Structures)
Student students[MAX_STUDENTS] = {
    {"12345", "John", "123_Street", "CS", 3, 'A', "CS101,CS102,CS103", "CS101", 90},
    {"67890", "Alice", "456_Avenue", "EE", 2, 'B', "EE101,EE102", "EE101", 85},
    {"11111", "Bob", "789_Boulevard", "ME", 4, 'C', "ME101,ME102", "ME101", 75},
    {"54321", "Charlie", "321_Road", "CE", 1, 'D', "CE101,CE102", "CE101", 88},
    {"98765", "Emma", "654_Lane", "IT", 2, 'A', "IT101,IT102,IT103", "IT103", 92}
};

// Function to find student by registration number
Student* find_by_reg_no(char* reg_no) {
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (strcmp(students[i].reg_no, reg_no) == 0) {
            return &students[i];
        }
    }
    return NULL;
}

// Function to find student by name
Student* find_by_name(char* name) {
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (strcmp(students[i].name, name) == 0) {
            return &students[i];
        }
    }
    return NULL;
}

// Function to find student by subject code
Student* find_by_subject_code(char* subject_code) {
    for (int i = 0; i < MAX_STUDENTS; i++) {
        if (strcmp(students[i].subject_code, subject_code) == 0) {
            return &students[i];
        }
    }
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[MAXLINE];

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen for client connections
    if (listen(server_fd, 5) == -1) {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addr_len);
        if (new_socket == -1) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected.\n");

        // Read client request
        recv(new_socket, buffer, sizeof(buffer), 0);
        buffer[strcspn(buffer, "\n")] = 0;

        pid_t pid = fork();
        if (pid == 0) {  // Child process
            close(server_fd);
            char response[MAXLINE];

            if (buffer[0] == '1') {  // Registration Number
                Student* student = find_by_reg_no(buffer + 2);
                if (student) {
                    snprintf(response, sizeof(response), "PID: %d\nName: %s\nAddress: %s\n", getpid(), student->name, student->address);
                } else {
                    strcpy(response, "Student not found.\n");
                }
            } else if (buffer[0] == '2') {  // Name
                Student* student = find_by_name(buffer + 2);
                if (student) {
                    snprintf(response, sizeof(response), "PID: %d\nDept: %s\nSemester: %d\nSection: %c\nCourses: %s\n", 
                             getpid(), student->department, student->semester, student->section, student->courses);
                } else {
                    strcpy(response, "Student not found.\n");
                }
            } else if (buffer[0] == '3') {  // Subject Code
                Student* student = find_by_subject_code(buffer + 2);
                if (student) {
                    snprintf(response, sizeof(response), "PID: %d\nMarks: %d\n", getpid(), student->marks);
                } else {
                    strcpy(response, "Subject not found.\n");
                }
            } else {
                strcpy(response, "Invalid choice.\n");
            }

            // Send response to client
            send(new_socket, response, strlen(response), 0);
            close(new_socket);
            exit(0);
        }
        close(new_socket);  // Parent closes client socket
        wait(NULL);  // Wait for child process
    }

    close(server_fd);
    return 0;
}
