#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

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

int main() {
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    struct student_info info;
    char option[MAXSIZE];
    char buffer[MAXSIZE];
    
    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(1);
    }

    // Set up the server address
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);  // Server's port number
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address

    // Connect to the server
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == -1) {
        perror("Connection failed");
        close(sockfd);
        exit(1);
    }
    while(1){
    // Display options and read user choice
    printf("Select an option to request student details:\n");
    printf("1. Registration Number\n");
    printf("2. Name of the Student\n");
    printf("3. Subject Code\n");
    printf("Enter your choice: ");
    fgets(option, MAXSIZE, stdin);
    option[strlen(option) - 1] = '\0';  // Remove newline character

    // Send the option to the server
    send(sockfd, option, strlen(option), 0);
    
    // Depending on the option, ask for the relevant input and send it to the server
    if (strcmp(option, "1") == 0) {
        printf("Enter Registration Number: ");
        fgets(info.reg_no, MAXSIZE, stdin);
        info.reg_no[strlen(info.reg_no) - 1] = '\0'; // Remove newline character
        send(sockfd, &info, sizeof(info), 0);
    } 
    else if (strcmp(option, "2") == 0) {
        printf("Enter Name of the Student: ");
        fgets(info.name, MAXSIZE, stdin);
        info.name[strlen(info.name) - 1] = '\0'; // Remove newline character
        send(sockfd, &info, sizeof(info), 0);
    } 
    else if (strcmp(option, "3") == 0) {
        printf("Enter Subject Code: ");
        fgets(info.subject_code, MAXSIZE, stdin);
        info.subject_code[strlen(info.subject_code) - 1] = '\0'; // Remove newline character
        send(sockfd, &info, sizeof(info), 0);
    } 
    else {
        printf("Invalid option selected.\n");
        close(sockfd);
        exit(1);
    }

    // Receive and print the response from the server (details)
    memset(buffer, '\0', sizeof(buffer));  // Clear the buffer
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Received from server: %s\n", buffer);
    } 
    // Close the socket
    close(sockfd);

    return 0;
}
