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
    char reg_no[15];
    char name[50];
    char address[100];
    char dept[20];
    int semester;
    char section;
    char courses[3][20];
    int marks;
} Student;

// Sample database
Student database[MAX_STUDENTS] = {
    {"2021001", "Alice", "123 Main St", "CSE", 4, 'A', {"Math", "OS", "DBMS"}, 85},
    {"2021002", "Bob", "456 Elm St", "ECE", 3, 'B', {"Circuits", "DSP", "VLSI"}, 78},
    {"2021003", "Charlie", "789 Oak St", "ME", 2, 'C', {"Thermo", "Mech", "Auto"}, 90}
};

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addr_size;
    char query[50];
    int option;

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

    listen(sockfd, 1);
    addr_size = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addr_size);
    if (newsockfd == -1) {
        perror("Accept failed");
        exit(1);
    }

    while (1) {
        recv(newsockfd, &option, sizeof(int), 0);
        recv(newsockfd, query, sizeof(query), 0);

        Student student;  // NOT a pointer, storing the entire structure
        int found = 0;

        // Search in database
        for (int i = 0; i < MAX_STUDENTS; i++) {
            if ((option == 1 && strcmp(database[i].reg_no, query) == 0) ||
                (option == 2 && strcmp(database[i].name, query) == 0) ||
                (option == 3 && strcmp(database[i].courses[0], query) == 0) ||
                (option == 3 && strcmp(database[i].courses[1], query) == 0) ||
                (option == 3 && strcmp(database[i].courses[2], query) == 0)) {
                student = database[i];  // Store the matched student
                found = 1;
                break;
            }
        }

        char response[256];

        if (found) {
            if (option == 1) {
                sprintf(response, "PID: %d\nName: %s\nAddress: %s", getpid(), student.name, student.address);
            } else if (option == 2) {
                sprintf(response, "PID: %d\nDept: %s\nSemester: %d\nSection: %c\nCourses: %s, %s, %s", 
                        getpid(), student.dept, student.semester, student.section, 
                        student.courses[0], student.courses[1], student.courses[2]);
            } else if (option == 3) {
                sprintf(response, "PID: %d\nMarks: %d", getpid(), student.marks);
            }
        } else {
            sprintf(response, "No record found.");
        }

        send(newsockfd, response, sizeof(response), 0);
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
