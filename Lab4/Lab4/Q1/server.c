#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#define MAXSIZE 100

// Structure to hold student details
struct student_info {
    char reg_no[MAXSIZE];
    char name[MAXSIZE];
    char subject_code[MAXSIZE];
    char address[MAXSIZE];
    char dept[MAXSIZE];
    char semester[MAXSIZE];
    char section[MAXSIZE];
    char courses[MAXSIZE];
    int marks;
};

// Sample student database (hardcoded for this example)
struct student_info student_db[3];

// Function to handle registration number request (Child Process 1)
void handle_reg_no(int sockfd, struct student_info *info) {
    char response[MAXSIZE];
    snprintf(response, sizeof(response), "Name: %s, Address: %s, PID: %d", info->name, info->address, getpid());
    send(sockfd, response, strlen(response), 0);
}

// Function to handle name request (Child Process 2)
void handle_name(int sockfd, struct student_info *info) {
    char response[MAXSIZE];
    snprintf(response, sizeof(response), "Dept: %s, Semester: %s, Section: %s, Courses: %s, PID: %d",
             info->dept, info->semester, info->section, info->courses, getpid());
    send(sockfd, response, strlen(response), 0);
}

// Function to handle subject code request (Child Process 3)
void handle_subject_code(int sockfd, struct student_info *info) {
    char response[MAXSIZE];
    snprintf(response, sizeof(response), "Marks in %s: %d, PID: %d", info->subject_code, info->marks, getpid());
    send(sockfd, response, strlen(response), 0);
}

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    struct student_info info;
    char option[MAXSIZE];
    int i;

    // Creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    // Binding the socket
    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        perror("Binding failed");
        close(sockfd);
        exit(1);
    }

    // Listening for incoming connections
    retval = listen(sockfd, 5);
    if (retval == -1) {
        perror("Listen failed");
        close(sockfd);
        exit(1);
    }

    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        perror("Accept failed");
        close(sockfd);
        exit(1);
    }

    // Initialize the student_db with sample data using strcpy
    strcpy(student_db[0].reg_no, "12345");
    strcpy(student_db[0].name, "John Doe");
    strcpy(student_db[0].subject_code, "CS101");
    strcpy(student_db[0].address, "123 Main St, City");
    strcpy(student_db[0].dept, "Computer Science");
    strcpy(student_db[0].semester, "3");
    strcpy(student_db[0].section, "A");
    strcpy(student_db[0].courses, "Data Structures, Algorithms");
    student_db[0].marks = 85;

    strcpy(student_db[1].reg_no, "67890");
    strcpy(student_db[1].name, "Jane Smith");
    strcpy(student_db[1].subject_code, "MA102");
    strcpy(student_db[1].address, "456 Elm St, City");
    strcpy(student_db[1].dept, "Mathematics");
    strcpy(student_db[1].semester, "2");
    strcpy(student_db[1].section, "B");
    strcpy(student_db[1].courses, "Calculus, Linear Algebra");
    student_db[1].marks = 92;

    strcpy(student_db[2].reg_no, "11223");
    strcpy(student_db[2].name, "Alice Brown");
    strcpy(student_db[2].subject_code, "PHY101");
    strcpy(student_db[2].address, "789 Oak St, City");
    strcpy(student_db[2].dept, "Physics");
    strcpy(student_db[2].semester, "1");
    strcpy(student_db[2].section, "C");
    strcpy(student_db[2].courses, "Mechanics, Thermodynamics");
    student_db[2].marks = 78;
    while(1){
    printf(" \n waiting to receive : \n");
    // Receiving option from client
    recv(newsockfd, option, sizeof(option), 0);
    recv(newsockfd, &info, sizeof(info), 0); // Receive student info (this could be empty if it's just the option)

    pid_t pid = fork();

    if (pid == 0) {  // Child Process 1 - Handle Registration Number
        if (strcmp(option, "1") == 0) {
            // Simulate fetching data from database using reg_no
            for (i = 0; i < sizeof(student_db) / sizeof(student_db[0]); i++) {
                if (strcmp(student_db[i].reg_no, info.reg_no) == 0) {
                    handle_reg_no(newsockfd, &student_db[i]);
                    break;
                }
            }
        }
    } else {
        pid = fork();
        if (pid == 0) {  // Child Process 2 - Handle Name of Student
            if (strcmp(option, "2") == 0) {
                // Simulate fetching data from database using name
                for (i = 0; i < sizeof(student_db) / sizeof(student_db[0]); i++) {
                    if (strcmp(student_db[i].name, info.name) == 0) {
                        handle_name(newsockfd, &student_db[i]);
                        break;
                    }
                }
            }
        } else {
            pid = fork();
            if (pid == 0) {  // Child Process 3 - Handle Subject Code
                if (strcmp(option, "3") == 0) {
                    // Simulate fetching data from database using subject_code
                    for (i = 0; i < sizeof(student_db) / sizeof(student_db[0]); i++) {
                        if (strcmp(student_db[i].subject_code, info.subject_code) == 0) {
                            handle_subject_code(newsockfd, &student_db[i]);
                            break;
                        }
                    }
                }
            }
        }
    }

    wait(NULL); // Wait for all child processes to terminate
    }
    close(newsockfd);
    close(sockfd);

    return 0;
}
