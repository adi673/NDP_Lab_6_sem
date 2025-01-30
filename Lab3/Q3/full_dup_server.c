#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define MAXSIZE 90

// Function to compare characters for descending order sorting
int compare_descending(const void* a, const void* b) {
    return (*(char*)b - *(char*)a);
}

// Function to compare characters for ascending order sorting
int compare_ascending(const void* a, const void* b) {
    return (*(char*)a - *(char*)b);
}

void sort_numbers(char* str, char* sorted_numbers) {
    int i, j = 0;
    int nums[MAXSIZE];

    // Extract numbers from the string
    for(i = 0; str[i] != '\0'; i++) {
        if(str[i] >= '0' && str[i] <= '9') {
            nums[j++] = str[i] - '0';  // Convert char to int
        }
    }

    // Sort numbers in ascending order
    qsort(nums, j, sizeof(int), compare_ascending);

    // Convert sorted numbers back to string
    for(i = 0; i < j; i++) {
        sorted_numbers[i] = nums[i] + '0';  // Convert int to char
    }
    sorted_numbers[i] = '\0';  // Null terminate the string
}

void sort_characters(char* str, char* sorted_characters) {
    int i, j = 0;
    char chars[MAXSIZE];

    // Extract characters from the string
    for(i = 0; str[i] != '\0'; i++) {
        if((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) {
            chars[j++] = str[i];
        }
    }

    // Sort characters in descending order
    qsort(chars, j, sizeof(char), compare_descending);

    // Convert sorted characters back to string
    for(i = 0; i < j; i++) {
        sorted_characters[i] = chars[i];
    }
    sorted_characters[i] = '\0';  // Null terminate the string
}

int main() {
     char buff[MAXSIZE];
    int sockfd, newsockfd, retval;
    int recedbytes, sentbytes;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nInitialized Socket\n");
    if (sockfd == -1) {
        printf("\nSocket Creation Error\n");
        return -1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388); // Server listens on port 3388
    serveraddr.sin_addr.s_addr = INADDR_ANY; // Listen on any network interface

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    printf("\nBinding complete\n");

    if (retval == -1) {
        printf("\nBinding error\n");
        close(sockfd);  // Properly close the socket in case of an error
        return -1; // Exit with an error
    }

    retval = listen(sockfd, 5);  // Listen for a maximum of 5 connections
    if (retval == -1) {
        printf("\nListen error\n");
        close(sockfd);  // Close socket on error
        return -1;
    }

    printf("\nListening for incoming connections...\n");
    actuallen = sizeof(clientaddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);
    if (newsockfd == -1) {
        printf("\nAccept error\n");
        close(sockfd);  // Close socket on error
        return -1;
    }

    printf("\nAccepted client connection\n");

    printf("\nGoing inside infinite loop \n");

    while(1) {
        memset(buff, '\0', sizeof(buff));

        // Receive data from the client
        recedbytes = recv(newsockfd, buff, sizeof(buff), 0);
        if(recedbytes == -1) {
            close(sockfd);
            close(newsockfd);
            return -1;
        }

        printf("\nReceived from client: %s\n", buff);

        pid_t pid = fork();  // Create a new process
        if(pid == 0) {   // Child process
            char sorted_numbers[MAXSIZE];
            sort_numbers(buff, sorted_numbers);
            printf("Child Process (PID: %d) sorted numbers: %s\n", getpid(), sorted_numbers);
            send(newsockfd, sorted_numbers, strlen(sorted_numbers), 0);
            exit(0);  // Exit child process after handling the task
        } else {   // Parent process
            char sorted_characters[MAXSIZE];
            sort_characters(buff, sorted_characters);
            printf("Parent Process (PID: %d) sorted characters: %s\n", getpid(), sorted_characters);
            send(newsockfd, sorted_characters, strlen(sorted_characters), 0);
            wait(NULL);  // Wait for the child process to finish
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
