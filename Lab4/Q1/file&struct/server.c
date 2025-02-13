#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Structure to hold student information
typedef struct {
    char reg_no[20];
    char name[50];
    char address[100];
    char dept[20];
    char semester[10];
    char section[10];
    char courses[100];
    char subject_code[10];
    char marks[10];
} Student;

// Function to get student details from file
void get_student_details(char *key, int option, char *response) {
    FILE *file = fopen("students.txt", "r");
    if (!file) {
        strcpy(response, "Error: Could not open file.");
        return;
    }

    Student student;
    int found = 0;
    while (fscanf(file, "%s %s %s %s %s %s %s %s %s", student.reg_no, student.name, student.address,
                  student.dept, student.semester, student.section, student.courses,
                  student.subject_code, student.marks) != EOF) {
        if ((option == 1 && strcmp(student.reg_no, key) == 0) ||
            (option == 2 && strcmp(student.name, key) == 0) ||
            (option == 3 && strcmp(student.subject_code, key) == 0)) {
            found = 1;
            pid_t pid = getpid();
            if (option == 1) {
                snprintf(response, BUFFER_SIZE, "Child PID: %d\nName: %s\nAddress: %s\n", pid, student.name, student.address);
            } else if (option == 2) {
                snprintf(response, BUFFER_SIZE, "Child PID: %d\nDept: %s\nSemester: %s\nSection: %s\nCourses: %s\n", 
                         pid, student.dept, student.semester, student.section, student.courses);
            } else if (option == 3) {
                snprintf(response, BUFFER_SIZE, "Child PID: %d\nMarks: %s\n", pid, student.marks);
            }
            break;
        }
    }
    fclose(file);
    if (!found) {
        strcpy(response, "No record found.");
    }
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    char buffer[BUFFER_SIZE];

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(server_sock);
        exit(1);
    }

    // Listen for connections
    if (listen(server_sock, 5) == -1) {
        perror("Listen failed");
        close(server_sock);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock == -1) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) { // Child process to handle client
            close(server_sock);
            recv(client_sock, buffer, BUFFER_SIZE, 0);
            
            int option;
            char key[50], response[BUFFER_SIZE];
            sscanf(buffer, "%d %s", &option, key);
            
            get_student_details(key, option, response);
            send(client_sock, response, strlen(response), 0);
            
            close(client_sock);
            exit(0);
        } else {
            close(client_sock);
        }
    }
    
    close(server_sock);
    return 0;
}
